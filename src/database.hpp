#ifndef DATABASE_HPP_
#define DATABASE_HPP_

#include "global_datatype.h"
#include "hexmodu.hpp"
#include <unordered_map>

struct StoreSurface {
  string m_nbh_c;
  string m_nbh_v;

  StoreSurface(ModuSurface1)
      : m_nbh_c(string()) bool operator==(const StoreSurface &rhs) const;
  bool operator<(const StoreSurface &rhs) const;
};

struct StoreModu {
  string m_nbh_c;
  string m_nbh_v;
  bool operator==(const StoreSurface &rhs) const;
  bool operator<(const StoreSurface &rhs) const;
};

class DataBase {
  using SurfaceIndex = vector<Byte>;

private:
  unordered_map
};

#endif