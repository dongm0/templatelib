#include "SeperatedPolyhedron.h"
#include "resource.h"
SeperatedPolyhedron::SeperatedPolyhedron() {}

int SeperatedPolyhedron::divSum() const { return divNum; }

bool SeperatedPolyhedron::operator<(const SeperatedPolyhedron &sP2) const {
  if (p1 < sP2.p1)
    return true;
  if (sP2.p1 < p1)
    return false;
  if (p2 < sP2.p2)
    return true;
  return false;
}

Polygon SeperatedPolyhedron::poly(int num) {
  if (num == 1)
    return p1;
  else
    return p2;
}

bool SeperatedPolyhedron::available() { return (divNum == 1); }