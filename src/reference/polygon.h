#pragma once
#include "resource.h"
#include "topology.h"
class Polygon {
  friend class Polyhedron;
  friend class SeperatedPolyhedron;

protected:
  class Topology topo;

public:
  Polygon();
  ~Polygon();
  bool operator<(const Polygon &topo2) const;
  bool operator==(const Polygon &topo2) const;
  bool operator!=(const Polygon &topo2) const;
  void print(FILE *fout = stdout) const;
};
