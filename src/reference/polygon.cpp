#include "polygon.h"
#include "polyhedron.h"
#include "topoPtr.h"
#include "topology.h"
Polygon::Polygon() {}
Polygon::~Polygon() {}
bool Polygon::operator<(const Polygon &topo2) const {
  return topo < topo2.topo;
}
bool Polygon::operator==(const Polygon &topo2) const {
  return topo == topo2.topo;
}
bool Polygon::operator!=(const Polygon &topo2) const {
  return topo != topo2.topo;
}
void Polygon::print(FILE *fout) const { topo.print(fout); }