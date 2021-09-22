#include "database.hpp"
#include "global_datatype.h"
#include "hexmodu.hpp"
#include "quicklist.hpp"
#include <fstream>
#include <iostream>

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

void worker() {
  int cnt = 0;
  int endnum = 6;
  DataBase finished;

  QuickList<HexModu, moduhash_> bfsqueue;
  HexModu cube;
  bfsqueue.PushBack(cube);
  while (!bfsqueue.Empty()) {
    auto h = bfsqueue.Front();
    auto sf = h.Surface();
    bfsqueue.PopFront();
    StoreSurface ssf(sf);
    auto &db = finished.database;
    cnt++;

    if (db.find(ssf) != db.end() && db[ssf].data.count(StoreModu(h)) != 0) {
      std::cout << cnt << std::endl;
    } else {
      if (h.size() < endnum) {
        for (auto sfcs = h.EnumerateAllSFcase(sf); auto sfc : sfcs) {
          if (auto p = h.AddHexAt(sf, sfc); p.first == true) {
            bfsqueue.PushBack(p.second);
          }
        }
      }
      if (h.size() <= 3 or !h.HasHangedElement()) {
        if (db.find(ssf) == db.end()) {
          db.insert({ssf, DataSet()});
        }
        db[ssf].data.insert(StoreModu(h));
      }
      if (cnt % 1000 == 0) {
        std::cout << cnt << std::endl;
      }
    }
  }
  ofstream fout;
  fout.open("datastore.dat");
  fout << finished.database.size() << endl << endl;
  int totalnum = 0;
  for (const auto &x : finished.database) {
    if (x.second.data.size() > 1) {

      x.first.Write(fout);
      totalnum += x.second.data.size();
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

int main() { 
    clock_t t1 = clock();
    worker(); 
    clock_t t2 = clock();
    cout <<  "time: " << float(t2-t1)/float(CLOCKS_PER_SEC) << endl;
}