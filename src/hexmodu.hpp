#ifndef HEXMODU_H
#define HEXMODU_H

#include "global_datatype.h"
#include <array>
#include <vector>

using namespace std;

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
};

#endif