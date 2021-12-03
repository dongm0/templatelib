#ifndef HEXMODU_H
#define HEXMODU_H

#include "global_datatype.h"
#include <array>
#include <vector>

using namespace std;

enum class SFCase { F1, F2, F3_1, F3_2, F4, F5 };

class HexModu;
class StoreSurface;
class StoreModu;
class moduhash_;

class ModuSurface {
  friend class HexModu;
  friend class StoreSurface;

private:
  vector<Byte> m_nbh_f;
  vector<Byte> m_nbh_v;

  // Hex2Surface
  vector<Byte> m_mapping_f;

  // Hex2Surface
  vector<Byte> m_mapping_v;
  size_t m_size_f;
  size_t m_size_v;

public:
  ModuSurface() : m_size_f(0), m_size_v(0) {}
  void Regular();
};

class HexModu {
  friend class StoreModu;
  friend class moduhash_;

private:
  vector<Byte> m_nbh_v;
  vector<Byte> m_nbh_c;
  // vector<Byte> m_cell_sheet;
  // vector<pair<Byte, Byte>> m_sheet;
  size_t m_size;
  size_t m_size_v;

private:
  tuple<Byte, Byte, Byte, Byte> GetAdjSurface_(Byte cur_c, Byte cur_sf_dir,
                                               Byte next_sf_dir) const;

public:
  HexModu()
      : m_nbh_v{0, 1, 2, 3, 4, 5, 6, 7}, m_nbh_c{-1, -1, -1, -1, -1, -1},
        m_size(1), m_size_v(8) {}
  HexModu(const HexModu &rhs)
      : m_nbh_v(rhs.m_nbh_v), m_nbh_c(rhs.m_nbh_c), m_size(rhs.m_size),
        m_size_v(rhs.m_size_v) {}

  void Regular();
  ModuSurface Surface() const;
  vector<pair<SFCase, vector<Byte>>>
  EnumerateAllSFcase(const ModuSurface &cursf) const;
  pair<bool, HexModu> AddHexAt(const ModuSurface &surface,
                               const pair<SFCase, vector<Byte>> &sfc) const;

  bool HasHangedElement() const;

  bool operator<(const HexModu &rhs) const;
  bool operator==(const HexModu &rhs) const;
  // bool operator==(const HexModu &rhs);
  size_t size() const { return m_size; }
  int getSingOffsetForSf() const;

  // boss's logic
  vector<pair<int, bool>> checkVerticeValence(const ModuSurface &sf) const {
    vector<pair<int, bool>> res;
    for (int i = 0; i < m_size_v; ++i) {
      bool issf = (sf.m_mapping_v[i] != -1);
      int cnt = 0;
      for (int j = 0; j < m_size; ++j) {
        if (find(m_nbh_v.begin() + j * 8, m_nbh_v.begin() + j * 8 + 8, i) -
                m_nbh_v.begin() + j * 8 <
            8) {
          cnt++;
        }
      }
      res.push_back(make_pair(issf, cnt));
    }
    return res;
  }
};

#endif