#include "database.hpp"
#include "global_datatype.h"
#include "hexmodu.hpp"
#include "quicklist.hpp"
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>

//#define USE_DFS
const StoreModu mmm({-1, -1, -1, 1,  -1, 2,  -1, -1, -1, 2,  -1, 0, 3,  4,  1,
                     5,  6,  0,  -1, 5,  6,  -1, 7,  2,  -1, 5,  8, 6,  7,  2,
                     3,  4,  6,  -1, 7,  2,  3,  4,  -1, 5,  7,  2, -1, 5,  6,
                     9,  4,  3,  -1, -1, -1, 9,  -1, 4,  -1, -1, 8, -1, -1, 7},
                    {0,  1,  2,  3,  4,  5,  6,  7,  0,  1,  5,  4,  8,  9,
                     10, 11, 3,  2,  1,  0,  12, 13, 9,  8,  2,  1,  9,  13,
                     14, 15, 16, 17, 12, 8,  0,  3,  18, 19, 20, 21, 3,  2,
                     13, 12, 21, 14, 17, 18, 12, 13, 9,  8,  18, 17, 16, 19,
                     14, 15, 16, 17, 21, 20, 19, 18, 21, 20, 0,  3,  22, 23,
                     24, 25, 14, 15, 20, 21, 26, 27, 23, 22});

const StoreModu mmm_final(
    {-1, -1, -1, 1,  -1, 2,  -1, -1, -1, 3,  4, 0,  -1, 5,  3,  6,  7,
     0,  -1, 5,  8,  2,  7,  1,  -1, -1, -1, 8, 9,  1,  3,  6,  -1, 7,
     8,  2,  10, 5,  7,  9,  8,  2,  -1, 5,  3, 6,  8,  2,  -1, 5,  4,
     7,  6,  3,  10, -1, -1, 6,  -1, 4,  -1, 9, -1, -1, -1, 6},
    {0,  1,  2,  3,  4,  5,  6,  7,  0,  1,  5,  4,  8,  9,  10, 11, 3,  2,
     1,  0,  12, 13, 14, 15, 0,  1,  9,  8,  15, 14, 16, 17, 8,  9,  10, 11,
     18, 19, 20, 21, 12, 15, 0,  3,  22, 17, 8,  18, 3,  2,  13, 12, 18, 19,
     23, 22, 12, 13, 14, 15, 22, 23, 16, 17, 17, 16, 9,  8,  22, 23, 19, 18,
     18, 19, 20, 21, 3,  2,  24, 25, 2,  13, 23, 19, 24, 26, 27, 20});

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
int endnum = 11;

#ifdef USE_DFS
DataBase finished;
DataSet recording;
vector<int> maxfacepernum{100, 100, 100, 100, 100, 100, 22, 25, 28, 30,
                          34,  36,  38,  41,  43,  46,  49, 51, 54};

// change into dfs
int cnt = 0;

static void dfs(const HexModu &modu);

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
const vector<int> m1fp = {100, 100, 100, 100, 100, 100, 22, 23, 24, 26,
                          28,  30,  32,  34,  35,  37,  39, 40, 42, 43,
                          45,  46,  48,  49,  50,  52,  53, 54, 56, 57};
QuickList<HexModu, moduhash_> targetlist;

void parallel_worker(int n, int max_n, const vector<HexModu> &modus) {
  int st = n, stride = max_n;
  for (int i = st; i < modus.size(); i += stride) {
    auto cur_modu = modus[i];
    int cnum = cur_modu.size();
    auto sf = cur_modu.Surface();
    StoreSurface ssf(sf);
    auto &db = finished.database;
    if (ssf.nbh_c.size() / 4 > m1fp[cur_modu.size()]) {
      continue;
    }

    {
      lock_guard<mutex> guard0(mtx_data);
      if (db.find(ssf) != db.end()) {
        lock_guard<mutex> guard(db[ssf].mtx);
        if (find(db[ssf].data.begin(), db[ssf].data.end(),
                 StoreModu(cur_modu)) != db[ssf].data.end()) {
          continue;
        }
      }
    }

    bool valenceCheck = [&]() -> bool {
      auto _res = cur_modu.checkVerticeValence(sf);
      for (auto p : _res) {
        if (p.second == false && p.first > 8) {
          return false;
        }
      }
      return true;
    }();
    if (!valenceCheck)
      continue;

    {
      if (cnum < endnum) {
        auto sfcs = cur_modu.EnumerateAllSFcase(sf);
        if (StoreModu(cur_modu) == mmm) {
            system("pause");
        }
        for (auto sfc : sfcs) {
          if (auto p = cur_modu.AddHexAt(sf, sfc); p.first == true) {
            lock_guard<mutex> guard1(mtx);
            targetlist.PushBack(p.second);
            
          //  if (StoreModu(p.second) == mmm_final) {
          //    auto _tmp = StoreModu(cur_modu);
          //    cout << "{";
          //    for (auto x : _tmp.nbh_c)
          //      cout << (int)x << ",";
          //    cout << "}" << endl << "{";
          //    for (auto x : _tmp.nbh_v)
          //      cout << (int)x << ",";
          //    cout << "}" << endl;
          //    system("pause");
          //  }
            
          }
        }
      }
      // parallel is not useless, surface calculation is time_consuming!

      if (cur_modu.size() <= 3 || !cur_modu.HasHangedElement()) {
        if (db.find(ssf) == db.end()) {
          lock_guard<mutex> guard0(mtx_data);
          db[ssf];
        }
        {
          lock_guard<mutex> guard(db[ssf].mtx);
          auto ssmodu = StoreModu(cur_modu);
          if (find(db[ssf].data.begin(), db[ssf].data.end(), ssmodu) ==
              db[ssf].data.end()) {
            db[ssf].data.push_back(ssmodu);
          }
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
    const int threadnum = 1;
    std::vector<std::thread> threads(threadnum);
    for (int i = 0; i < threadnum; ++i) {
      threads[i] = std::thread(parallel_worker, i, threadnum, cur_modus);
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
  size_t maxn = 0;
  for (const auto &x : finished.database) {
    totalnum += x.second.data.size();
    x.first.Write(fout);
    maxn = std::max(x.second.data.size(), maxn);
    fout << x.second.data.size() << endl;
    for (const auto &y : x.second.data) {
      y.Write(fout);
    }
    fout << endl;
  }
  cout << "maxn = " << maxn << endl;
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