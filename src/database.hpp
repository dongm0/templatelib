#ifndef DATABASE_HPP_
#define DATABASE_HPP_

#include "global_datatype.h"
#include "hexmodu.hpp"
#include <unordered_map>
#include <unordered_set>

class StoreSurface {
public:
  vector<Byte> nbh_c;
  vector<Byte> nbh_v;

  StoreSurface(const ModuSurface &surface)
      : nbh_c(surface.m_nbh_f), nbh_v(surface.m_nbh_v) {}
  bool operator==(const StoreSurface &rhs) const {
    return nbh_c == rhs.nbh_c && nbh_v == rhs.nbh_v;
  }
};

class StoreModu {
public:
  vector<Byte> nbh_c;
  vector<Byte> nbh_v;

  StoreModu(const HexModu &modu) : nbh_c(modu.m_nbh_c), nbh_v(modu.m_nbh_v) {}
  bool operator==(const StoreModu &rhs) const {
    return nbh_v == rhs.nbh_v && nbh_c == rhs.nbh_c;
  }
};

struct SurfaceHash {
  size_t operator()(const StoreSurface &s) const {
    std::hash<Byte> hasher;
    size_t seed = s.nbh_c.size() + s.nbh_v.size() + 2;
    seed ^= hasher(-2) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    for (Byte i : s.nbh_c) {
      seed ^= hasher(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    seed ^= hasher(-2) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    for (auto i : s.nbh_v) {
      seed ^= hasher(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
};

struct ModuHash {
  size_t operator()(const StoreModu &s) const {
    std::hash<Byte> hasher;
    size_t seed = s.nbh_c.size() + s.nbh_v.size() + 2;
    seed ^= hasher(-2) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    for (Byte i : s.nbh_c) {
      seed ^= hasher(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    seed ^= hasher(-2) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    for (auto i : s.nbh_v) {
      seed ^= hasher(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
};

class DataSet {
public:
  unordered_set<StoreModu, ModuHash> data;
};

class DataBase {
public:
  unordered_map<StoreSurface, DataSet, SurfaceHash> database;
};

#endif