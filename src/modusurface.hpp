#ifndef MODUSURFACE_H
#define MODUSURFACE_H

#include "global_datatype.h"
#include <vector>

using namespace std;

class HexModu;

class ModuSurface {
  friend class HexModu;

private:
  vector<Byte> m_nbh_f;
  vector<Byte> m_mapping;
  size_t m_size;
  ModuSurface();

public:
  void Regular();
};

#endif