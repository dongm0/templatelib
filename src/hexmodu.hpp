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
  vector<Byte> m_cell_sheet;
  vector<pair<Byte, Byte>> m_sheet;
  size_t m_size;

private:
  tuple<Byte, Byte, Byte, Byte> GetAdjSurface_(Byte cur_c, Byte cur_sf_dir,
                                               Byte next_sf_dir);

public:
  HexModu()
      : m_nbh_v{0, 1, 2, 3, 4, 5, 6, 7}, m_nbh_c{-1, -1, -1, -1,
                                                 -1, -1, -1, -1},
        m_cell_sheet{0, 0, 1, 1, 2, 2}, m_sheet{make_pair(0, 0),
                                                make_pair(0, 0),
                                                make_pair(0, 0)},
        m_size(1){};
  HexModu(const HexModu &rhs)
      : m_nbh_v(rhs.m_nbh_v), m_nbh_c(rhs.m_nbh_c),
        m_cell_sheet(rhs.m_cell_sheet), m_sheet(rhs.m_sheet),
        m_size(rhs.m_size) {}

  void Regular();
  ModuSurface Surface();
  vector<pair<SFCase, vector<Byte>>> EnumerateAllSFcase();
  pair<bool, HexModu> AddHexAt(const ModuSurface &surface,
                               const pair<SFCase, vector<Byte>> &sfc);

  bool HasHangedElement();
};

#endif