#include "database.hpp"
#include "global_datatype.h"
#include "hexmodu.hpp"
#include "quicklist.hpp"
#include <ctime>
#include <fstream>
#include <iostream>
#include <thread>
#include <mutex>

//#define USE_DFS

class moduhash_ {
public:
  size_t operator()(const HexModu &s) const {
    std::hash<Byte> hasher;
    size_t seed = s.m_nbh_c.size() + s.m_nbh_v.size() + 2;
    seed ^= hasher(-2) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    for (Byte i : s.m_nbh_c) {
      seed ^= hasher(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    seed ^= hasher(-2) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    for (auto i : s.m_nbh_v) {
      seed ^= hasher(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
};


int endnum = 10;

#ifdef  USE_DFS
DataBase finished;
DataSet recording;
vector<int> maxfacepernum{100, 100, 100, 100, 100, 100, 22, 25, 28, 30,
                          34,  36,  38,  41,  43,  46,  49, 51, 54};

// change into dfs
int cnt = 0;

static void dfs(const HexModu& modu);

void dfs_worker() {
  HexModu cube;
  dfs(cube);
  ofstream fout;
  fout.open("datastore.dat");
  fout << finished.database.size() << endl << endl;
  int totalnum = 0;
  for (const auto &x : finished.database) {
    // if (x.second.data.size() > 1) {

    x.first.Write(fout);
    totalnum += x.second.data.size();
    fout << x.second.data.size() << endl;
    for (const auto &y : x.second.data) {
      y.Write(fout);
    }
    fout << endl;
    //}
  }
  cout << "total modu num: " << totalnum << endl;
  fout.close();
}

void dfs(const HexModu &modu) {
  if (recording.data.count(StoreModu(modu)) != 0) {
    return;
  }
  auto sf = modu.Surface();
  StoreSurface ssf(sf);
  auto &db = finished.database;
  if (db.find(ssf) != db.end() && db[ssf].data.count(StoreModu(modu)) != 0) {
    return;
    // std::cout << cnt << std::endl;
  } else if (ssf.nbh_c.size() / 4 > maxfacepernum[modu.size()]) {
    return;
    // std::cout << cnt << std::endl;
  } else {

    if (modu.size() <= 3 || !modu.HasHangedElement()) {
      if (db.find(ssf) == db.end()) {
        db.insert({ssf, DataSet()});
      }
      db[ssf].data.insert(StoreModu(modu));
      recording.data.insert(StoreModu(modu));
      cnt += 1;
      std::cout << cnt << std::endl;
    }
    if (modu.size() < endnum) {
      for (auto sfcs = modu.EnumerateAllSFcase(sf); auto sfc : sfcs) {
        if (auto p = modu.AddHexAt(sf, sfc); p.first == true) {
          dfs(p.second);
        }
      }
    }
    // if (cnt % 1000 == 0) {
    //}
  };
}

#else
std::mutex mtx;
std::mutex mtx_data;
int cnt = 0;
DataBase finished;
const vector<int> maxfacepernum = {100, 100, 100, 100, 100, 100, 22, 25, 28, 30,
                                   34,  36,  38,  41,  43,  46,  49, 51, 54};
QuickList<HexModu, moduhash_> targetlist;

void parallel_worker(int n, int max_n, const vector<HexModu> &modus) {
  int st = n, stride = max_n;
  for (int i = st; i < modus.size(); i += stride) {
    auto cur_modu = modus[i];
    int cnum = cur_modu.size();
    auto sf = cur_modu.Surface();
    StoreSurface ssf(sf);
    auto &db = finished.database;

    if (db.find(ssf) != db.end() &&
        db[ssf].data.count(StoreModu(cur_modu)) != 0) {
      continue;
    } else if (ssf.nbh_c.size() / 4 > maxfacepernum[cur_modu.size()]) {
      continue;
    } else {
      if (cnum < endnum) {
        auto sfcs = cur_modu.EnumerateAllSFcase(sf);
        for (auto sfc : sfcs) {
          if (auto p = cur_modu.AddHexAt(sf, sfc); p.first == true) {
            mtx.lock();
            targetlist.PushBack(p.second);
            mtx.unlock();
          }
        }
      }
      // parallel is not useless, surface calculation is time_consuming!
      
      if (cur_modu.size() <= 3 || !cur_modu.HasHangedElement()) {
        if (db.find(ssf) == db.end()) {
          mtx_data.lock();
          db.insert({ssf, DataSet()});
          mtx_data.unlock();
        }
        if (db[ssf].data.count(StoreModu(cur_modu)) == 0) {
          mtx_data.lock();
          db[ssf].data.insert(StoreModu(cur_modu));
          mtx_data.unlock();
        }
      }
    }
  }
}


void bfs_worker() {
  HexModu cube;
  targetlist.PushBack(cube);
  for (int i = 0; i < endnum; ++i) {
    vector<HexModu> cur_modus;
    cur_modus.reserve(targetlist.Size());
    for (const auto &m : targetlist) {
      cur_modus.push_back(m);
    }
    targetlist.Clear();
    const int threadnum = 8;
    std::vector<std::thread> threads(threadnum);
    for (int i = 0; i < threadnum; ++i) {
      threads[i] =
          std::thread(parallel_worker, i, threadnum, cur_modus);
    }
    for (int i = 0; i < threadnum; ++i) {
      threads[i].join();
    }
  }

  // output
  ofstream fout;
  fout.open("datastore.dat");
  fout << finished.database.size() << endl << endl;
  int totalnum = 0;
  for (const auto &x : finished.database) {
    totalnum += x.second.data.size();
    if (x.second.data.size() > 1) {
      x.first.Write(fout);
      fout << x.second.data.size() << endl;
      for (const auto &y : x.second.data) {
        y.Write(fout);
      }
      fout << endl;
    }
  }
  cout << "total modu num: " << totalnum << endl;
  fout.close();
}

#endif //  USE_DFS

int main() {
  clock_t t1 = clock();
#ifdef USE_DFS
  dfs_worker();
#else
  bfs_worker();
#endif // USE_DFS

  clock_t t2 = clock();
  cout << "time: " << float(t2 - t1) / float(CLOCKS_PER_SEC) << endl;
}