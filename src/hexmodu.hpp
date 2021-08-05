#ifndef HEXMODU_H
#define HEXMODU_H

#include "global_datatype.h"
#include <array>
#include <vector>

using namespace std;

enum class SFCase { F1, F2, F3_1, F3_2, F4, F5 };

class HexModu;

class ModuSurface {
  friend class HexModu;

private:
  vector<Byte> m_nbh_f;
  vector<Byte> m_nbh_v;

  // Hex2Surface
  vector<Byte> m_mapping_f;

  // Hex2Surface
  vector<Byte> m_mapping_v;
  size_t m_size_f;
  size_t m_size_v;
  ModuSurface();

public:
  void Regular();
};

class HexModu {
private:
  vector<Byte> m_nbh_v;
  vector<Byte> m_nbh_c;
  size_t m_size;

public:
  HexModu()
      : m_nbh_v{0, 1, 2, 3, 4, 5, 6, 7}, m_nbh_c{-1, -1, -1, -1,
                                                 -1, -1, -1, -1},
        m_size(1) {}
  HexModu(const HexModu &rhs)
      : m_nbh_v(rhs.m_nbh_v), m_nbh_c(rhs.m_nbh_c), m_size(rhs.m_size) {}

  void Regular();
  ModuSurface Surface();
  vector<pair<SFCase, vector<Byte>>> EnumerateAllSFcase();
};

#endif