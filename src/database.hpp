#ifndef DATABASE_HPP_
#define DATABASE_HPP_

#include "global_datatype.h"
#include "hexmodu.hpp"
#include <algorithm>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>
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
  bool operator<(const StoreSurface &rhs) const {
    if (nbh_c.size() < rhs.nbh_c.size())
      return true;
    else if (nbh_c.size() > rhs.nbh_c.size())
      return false;
    if (nbh_c < rhs.nbh_c)
      return true;
    else if (nbh_c > rhs.nbh_c)
      return false;
    else if (nbh_v < rhs.nbh_v)
      return true;
    else
      return false;
  }
  void Write(ostream &out) const {
    out << nbh_c.size() / 4 << " " << nbh_v.size() / 4 + 2 << " ";
    for (const auto &x : nbh_c) {
      out << (int)x << " ";
    }
    for (const auto &x : nbh_v) {
      out << (int)x << " ";
    }
    out << endl;
    // return out;
  }
};

class StoreModu {
public:
  vector<Byte> nbh_c;
  vector<Byte> nbh_v;

  StoreModu(const HexModu &modu) : nbh_c(modu.m_nbh_c), nbh_v(modu.m_nbh_v) {}
  StoreModu(const vector<Byte> &c1, const vector<Byte> &v1)
      : nbh_c(c1), nbh_v(v1) {}
  bool operator==(const StoreModu &rhs) const {
    return nbh_v == rhs.nbh_v && nbh_c == rhs.nbh_c;
  }
  void Write(ostream &out) const {
    out << nbh_c.size() / 6 << " "
        << (*max_element(nbh_v.begin(), nbh_v.end())) + 1 << " ";
    for (const auto &x : nbh_c) {
      out << (int)x << " ";
    }
    for (const auto &x : nbh_v) {
      out << (int)x << " ";
    }
    out << endl;
    // return out;
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
  // unordered_set<StoreModu, ModuHash> data;
  vector<StoreModu> data;
  std::mutex mtx;
  DataSet() {}
};

class DataBase {
public:
  // map<StoreSurface, DataSet, SurfaceHash> database;
  // map<StoreSurface, DataSet> database;
  unordered_set<StoreModu, ModuHash> database;
};

#endif