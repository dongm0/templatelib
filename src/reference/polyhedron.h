#pragma once
#include "resource.h"
class Polyhedron {
  friend class Polygon;
  friend class PolyhedronCompare;
  friend class SeperatedPolyhedron;

private:
  int divNum;
  void bfs(int st, vector<int> &certain, vector<int> &connection,
           int &status) const;

protected:
  static TopoPtr topoSet;
  bool seperate_judge(FaceType center, vector<int> select) const;
  SeperatedPolyhedron separate(const vector<int> &certain) const; //枚举
  const Topology *topo;
  int father1, father2; //生成的父网格
  vector<int> father_root;
  // void showConstruction(const set<Polyhedron> &done, vector<int>, int&, FILE
  // *fout) const;
  void showConstruction(const set<Polyhedron> &done, vector<int>, int &,
                        vector<int> &structure) const;
  // void comb(const set<Polyhedron> &done, SeperatedPolyhedron sP1,
  // SeperatedPolyhedron sP2, vector<int> num, int &maxNum, FILE *fout) const;
  void comb(const set<Polyhedron> &done, SeperatedPolyhedron sP1,
            SeperatedPolyhedron sP2, vector<int> num, int &maxNum,
            vector<int> &structure) const;

public:
  Polyhedron(const SeperatedPolyhedron &sP1, const SeperatedPolyhedron &sP2);
  Polyhedron(PolyhedronType type);
  Polyhedron();
  ~Polyhedron();
  // bool show(const set<Polyhedron> &done) const;
  bool show(const set<Polyhedron> &done, vector<int> &structure_temp,
            FILE *fout) const;
  int divSum() const;
  bool available() const;
  int topoNum;
  vector<SeperatedPolyhedron> divide() const;
  vector<SeperatedPolyhedron> divide_old() const;
  bool operator<(const Polyhedron &topo2) const;
  bool operator==(const Polyhedron &topo2) const;
  bool operator!=(const Polyhedron &topo2) const;
  void print(FILE *fout = stdout) const;
  void showdia() const;
  void addDiameter() const;
  void free();
  static Polyhedron load(set<Polyhedron> &done);
  static int load_custom();
};

class PolyhedronCompare {
public:
  bool operator()(const Polyhedron &s1, const Polyhedron &s2) const;
};