#pragma once
#include "resource.h"
class TopoPtr {
  friend class Polyhedron;

protected:
  map<Topology, int> content;
  vector<const Topology *> list;
  int size;

public:
  TopoPtr();
  int insert(const Topology &topo);
  int maxSize();
  const Topology *getTopo(int topoNum);
};