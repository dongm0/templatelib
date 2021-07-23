#pragma once
#include "polygon.h"
#include "resource.h"
#include "topology.h"
class SeperatedPolyhedron {
  friend Polygon;
  friend Polyhedron;

protected:
  Polygon p1, p2;
  int n, m, divNum;
  vector<PointType> mapping;
  vector<PointType> m1;
  vector<PointType> m2;
  vector<int> father_root;
  int father;

public:
  int divSum() const;
  Polygon poly(int num);
  SeperatedPolyhedron();
  bool operator<(const SeperatedPolyhedron &sP2) const;
  bool available();
};