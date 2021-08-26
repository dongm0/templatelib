#include "database.hpp"
#include "global_datatype.h"
#include "hexmodu.hpp"
#include "quicklist.hpp"

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
  DataBase finished;

  QuickList<HexModu, moduhash_> bfsqueue;
  HexModu cube;
  bfsqueue.PushBack(cube);
  while (!bfsqueue.Empty()) {
    auto h = bfsqueue.Front();
    auto sf = h.Surface();
    bfsqueue.PopFront();
    for (auto sfcs = h.EnumerateAllSFcase(sf); auto sfc : sfcs) {
      if (auto p = h.AddHexAt(sf, sfc); p.first == true) {
        bfsqueue.PushBack(p.second);
      }
    }
    if (h.size() <= 3 || !h.HasHangedElement()) {
      auto &db = finished.database;
      StoreSurface ssf(sf);
      if (db.find(ssf) == db.end()) {
        db.insert({ssf, DataSet()});
      }
      db[ssf].data.insert(StoreModu(h));
    }
  }
}

int main() { worker(); }