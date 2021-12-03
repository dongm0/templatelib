#include "database.hpp"
#include "global_datatype.h"
#include "hexmodu.hpp"
#include "quicklist.hpp"
#include "variaty.h"
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iostream>
#include <mutex>
#include <set>
#include <thread>

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
int endnum = 17;
int cur_cnum = 0;

std::mutex mtx_queue;
std::mutex mtx_database;
int cnt = 0;
std::vector<StoreModu> store;
std::vector<StoreSurface> storesf;
unordered_map<HexModu, ModuSurface, moduhash_> targetlist;

void parallel_worker(int n, int max_n,
                     const vector<pair<HexModu, ModuSurface>> &modus) {
  int st = n, stride = max_n;
  for (int i = st; i < modus.size(); i += stride) {
    auto &cur_modu = modus[i].first;
    int cnum = cur_modu.size();
    const auto &sf = modus[i].second;
    StoreSurface ssf(sf);
    auto ideal_surface_fnum = cur_modu.getSingOffsetForSf();

    if (cur_modu.size() > 5 && ssf.nbh_c.size() / 4 > ideal_surface_fnum) {
      continue;
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

    if (cnum < endnum) {
      auto sfcs = cur_modu.EnumerateAllSFcase(sf);
      for (auto sfc : sfcs) {
        auto p = cur_modu.AddHexAt(sf, sfc);
        if (!p.first)
          continue;
        bool calculatesf = false;
        {
          lock_guard<mutex> guard_q(mtx_queue);
          calculatesf = targetlist.find(p.second) == targetlist.end();
        }
        if (!calculatesf)
          continue;
        auto newsf = p.second.Surface();
        auto newssf = StoreSurface(newsf);
        if (int fnum_upper = p.second.getSingOffsetForSf();
            p.second.size() > 5 && newssf.nbh_c.size() / 4 > fnum_upper) {
          continue;
        }
        {
          lock_guard<mutex> guard_q(mtx_queue);
          targetlist.insert(make_pair(p.second, newsf));
        }
      }
    }

    if (cur_modu.size() <= 3 || !cur_modu.HasHangedElement()) {
      StoreModu smodu(cur_modu);
      {
        lock_guard<mutex> guard_db(mtx_database);
        store.push_back(smodu);
        storesf.push_back(ssf);
      }
    }
  }
}

void bfs_worker() {
  HexModu cube;
  targetlist.insert(make_pair(cube, cube.Surface()));
  for (int i = 0; i < endnum; ++i) {
    clock_t t1 = clock();
    cur_cnum = i;
    vector<pair<HexModu, ModuSurface>> cur_modus;
    cur_modus.reserve(targetlist.size());
    cur_modus.assign(move_iterator(targetlist.begin()),
                     move_iterator(targetlist.end()));
    targetlist.clear();
    const int threadnum = 8;
    std::vector<std::thread> threads(threadnum);
    for (int _i = 0; _i < threadnum; ++_i) {
      threads[_i] = std::thread(parallel_worker, _i, threadnum, cur_modus);
    }
    for (int _i = 0; _i < threadnum; ++_i) {
      threads[_i].join();
    }
    storesf.shrink_to_fit();
    clock_t t2 = clock();
    std::cout << "cellnum: " << i + 1 << "\t using time: ";
    std::cout << (t2 - t1) / CLOCKS_PER_SEC << std::endl;
  }

  // output
  ofstream fout;
  fout.open(OUT_PATH);
  fout << store.size() << endl << endl;
  int totalnum = store.size();
  for (int i = 0; i < store.size(); ++i) {
    storesf[i].Write(fout);
    store[i].Write(fout);
    fout << endl;
  }
  std::cout << "total modu num: " << totalnum << endl;
  fout.close();
}

int main() {
  clock_t t1 = clock();
#ifdef USE_DFS
  dfs_worker();
#else
  bfs_worker();
#endif // USE_DFS

  clock_t t2 = clock();
  std::cout << "time: " << float(t2 - t1) / float(CLOCKS_PER_SEC) << endl;
}