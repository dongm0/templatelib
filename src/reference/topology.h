#pragma once
#include "resource.h"

class Topology {
  friend class Polyhedron;
  friend class Polygon;
  friend class SeperatedPolyhedron;
  friend class PolyhedronCompare;

protected:
  static int maxD[30];
  static int asdf[30];
  int n, m; // n是面数，m是点数
  vector<FaceType> graphF;
  vector<PointType> graphP;
  vector<char>
      edge_valence; //记录边的自由度，限定只能为345，即表面的自由度可以为1234
  void regular();
  void regular(vector<PointType> &displace);
  vector<PointType> forbidden;

public:
  Topology();
  ~Topology();
  int pointSum() const;
  int faceSum() const;
  bool operator<(const Topology &topo2) const;
  bool operator==(const Topology &topo2) const;
  bool operator!=(const Topology &topo2) const;
  void print(FILE *fout = stdout) const;
  int diameter() const;
  bool available() const;

  // 加入cellnum，单纯以cell数量与表面单元数限制网格数量的增长
  int cellnum; // pyramid与cap14没有在创建topology时设定cellnum
};
