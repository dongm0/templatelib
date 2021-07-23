#include "polyhedron.h"
#include "SeperatedPolyhedron.h"
#include "polygon.h"
#include "topoPtr.h"
#include "topology.h"
TopoPtr Polyhedron::topoSet;
int rec[20][1000] = {0};
bool needcheck = false;
// int maxface[20] = { 999, 999, 999, 999, 999, 22, 24, 24, 27, 30, 30, 30, 30,
// 30, 30, 30, 30, 30, 30, 30 };
void swap(PointType &a, PointType &b) {
  PointType c = a;
  a = b;
  b = c;
}

Polyhedron::Polyhedron() { return; }
Polyhedron::Polyhedron(PolyhedronType type) {
  if (type == CUBE) {
    Topology topology;
    topology.n = 6;
    topology.m = 8;
    divNum = 1;
    topology.cellnum = divNum;
    FaceType initGraphF[24] = {1, 2, 3, 4, 0, 2, 5, 4, 0, 1, 5, 3,
                               0, 2, 5, 4, 0, 1, 5, 3, 1, 2, 3, 4};
    topology.graphF.insert(topology.graphF.end(), initGraphF, initGraphF + 24);
    PointType initGraphP[24] = {0, 1, 2, 3, 0, 1, 5, 4, 2, 1, 5, 6,
                                3, 2, 6, 7, 3, 0, 4, 7, 4, 5, 6, 7};
    topology.graphP.insert(topology.graphP.end(), initGraphP, initGraphP + 24);
    PointType initForbid[8] = {0, 6, 1, 7, 2, 4, 3, 5};
    // to fine the smallest result ignoring the legal requirment
    topology.forbidden.insert(topology.forbidden.end(), initForbid,
                              initForbid + 8);
    char initValence[24] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    topology.edge_valence.insert(topology.edge_valence.end(), initValence,
                                 initValence + 24);
    topology.regular();
    topoNum = topoSet.insert(topology);
    topo = topoSet.getTopo(topoNum);
    father1 = -1;
    father2 = -1;
  } else if (type == PYRAMID) {
    Topology topology;
    topology.n = 16;
    topology.m = 18;
    FaceType initGraphF[64] = {
        4, 8,  1,  3,  5, 9,  2,  0, 6, 10, 3,  1, 7, 11, 0,  2,
        0, 8,  12, 11, 1, 9,  13, 8, 2, 10, 14, 9, 3, 11, 15, 10,
        0, 4,  13, 5,  1, 5,  14, 6, 2, 6,  15, 7, 3, 7,  12, 4,
        4, 13, 15, 11, 5, 14, 12, 8, 6, 15, 13, 9, 7, 12, 14, 10};
    topology.graphF.clear();
    topology.graphF.insert(topology.graphF.end(), initGraphF, initGraphF + 64);
    PointType initGraphP[64] = {
        3,  4, 0,  16, 0,  5, 1,  16, 1,  6,  2,  16, 2,  7,  3,  16,
        3,  4, 8,  15, 0,  5, 9,  12, 1,  6,  10, 13, 2,  7,  11, 14,
        0,  4, 8,  12, 1,  5, 9,  13, 2,  6,  10, 14, 3,  7,  11, 15,
        15, 8, 17, 11, 12, 9, 17, 8,  13, 10, 17, 9,  14, 11, 17, 10};
    topology.graphP.clear();
    topology.graphP.insert(topology.graphP.end(), initGraphP, initGraphP + 64);
    int next[18] = {1,  2,  3, 0,  5,  6,  7,  4,  9,
                    10, 11, 8, 13, 14, 15, 12, 16, 17};
    for (int i = 0; i < 64 - 4; i++)
      if (i % 16 < 12) {
        if (topology.graphP[i + 4] != next[topology.graphP[i]])
          printf("!!!");
        if (topology.graphF[i + 4] != next[topology.graphF[i]])
          printf("!!!");
      }
    topology.regular();
    topoNum = topoSet.insert(topology);
    topo = topoSet.getTopo(topoNum);
    divNum = -1;
  } else if (type == CAP14) {
    Topology topology;
    topology.n = 26;
    topology.m = 28;
    FaceType initGraphF[104] = {
        18, 1,  7,  22, 0,  19, 2,  5,  1,  3,  6,  5,  20, 4,  6,  2,  21, 25,
        8,  3,  1,  2,  6,  7,  2,  3,  8,  5,  0,  5,  8,  23, 6,  4,  24, 7,
        18, 10, 16, 22, 9,  19, 11, 14, 10, 12, 15, 14, 20, 13, 15, 11, 21, 25,
        17, 12, 10, 11, 15, 16, 11, 12, 17, 14, 9,  14, 17, 23, 15, 13, 24, 16,
        0,  22, 9,  19, 20, 1,  18, 10, 21, 3,  19, 12, 4,  20, 13, 25, 18, 0,
        23, 9,  22, 7,  24, 16, 23, 8,  25, 17, 4,  21, 13, 24};
    topology.graphF.insert(topology.graphF.end(), initGraphF, initGraphF + 104);
    PointType initGraphP[104] = {
        0,  1,  6,  5,  6,  1,  2,  7,  7,  2,  9,  8,  2,  3,  10, 9,  3,  4,
        11, 10, 6,  7,  8,  12, 8,  9,  10, 12, 5,  6,  12, 13, 12, 10, 11, 13,
        14, 15, 20, 19, 20, 15, 16, 21, 21, 16, 23, 22, 16, 17, 24, 23, 17, 18,
        25, 24, 20, 21, 22, 26, 22, 23, 24, 26, 19, 20, 26, 27, 26, 24, 25, 27,
        1,  0,  14, 15, 16, 2,  1,  15, 17, 3,  2,  16, 4,  3,  17, 18, 14, 0,
        5,  19, 19, 5,  13, 27, 27, 13, 11, 25, 11, 4,  18, 25};
    topology.graphP.insert(topology.graphP.end(), initGraphP, initGraphP + 104);
    topology.regular();
    topoNum = topoSet.insert(topology);
    topo = topoSet.getTopo(topoNum);
    divNum = -1;
  }
}
/*
void Polyhedron::comb(const set<Polyhedron> &done, SeperatedPolyhedron sP1,
SeperatedPolyhedron sP2, vector<int> num, int &maxNum, FILE *fout) const {
        Topology topology;

        topology.n = sP1.p2.topo.n + sP2.p2.topo.n;
        topology.m = topology.n + 2;
        bool valid = true;
        if (topology.n >= 40)
                valid = false;
        if (!valid) {
                return;
        }
        topology.graphF.assign(topology.n * 4, 0);
        topology.graphP.assign(topology.n * 4, 0);

        vector<PointType> mapping(sP2.p2.topo.m, -1);
        for (int i = 0; i < sP1.p1.topo.m; i++)
                if (sP2.mapping[i] != -1)
                        mapping[sP2.mapping[i]] = sP1.mapping[i];
        int maxPoint = sP1.p2.topo.m;
        for (int i = 0; i < sP2.p2.topo.m; i++)
                if (mapping[i] == -1)
                        mapping[i] = maxPoint++;
        for (int i = 0; i < sP1.p2.topo.n * 4; i++)
                topology.graphP[i] = sP1.p2.topo.graphP[i];
        for (int i = 0; i < sP2.p2.topo.n * 4; i++)
                topology.graphP[i + sP1.p2.topo.n * 4] =
mapping[sP2.p2.topo.graphP[i]]; map< pair<PointType, PointType>,int> mm; for
(int i = 0; i < sP1.p2.topo.n * 4; i++) if (sP1.p2.topo.graphF[i] != -1)
                        topology.graphF[i] = sP1.p2.topo.graphF[i];
                else {
                        PointType x = sP1.p2.topo.graphP[i], y =
sP1.p2.topo.graphP[(i & 3) == 3 ? i - 3 : i + 1]; pair<PointType, PointType> pr;
                        if (x < y)
                                pr = make_pair(x, y);
                        else
                                pr = make_pair(y, x);
                        mm[pr] = i;
                }

        for (int i = 0; i < sP2.p2.topo.n * 4; i++)
                if (sP2.p2.topo.graphF[i] != -1)
                        topology.graphF[i + sP1.p2.topo.n * 4] =
sP2.p2.topo.graphF[i] + sP1.p2.topo.n; else { PointType x =
mapping[sP2.p2.topo.graphP[i]], y = mapping[sP2.p2.topo.graphP[(i & 3) == 3 ? i
- 3 : i + 1]]; pair<PointType, PointType> pr; if (x < y) pr = make_pair(x, y);
                        else
                                pr = make_pair(y, x);
                        if (mm.find(pr) == mm.end()) {
                                printf("can not find correspond face\n");
                        }
                        else {
                                int point = mm[pr];
                                topology.graphF[i + sP1.p2.topo.n * 4] = point /
4; topology.graphF[point] = i / 4 + sP1.p2.topo.n;
                        }
                }

        vector<PointType> displace;
        topology.regular(displace);
        Polyhedron p1, p2;
        p1.topoNum = father1;
        set<Polyhedron>::iterator it = done.find(p1);
        p1 = *it;
        p2.topoNum = father2;
        it = done.find(p2);
        p2 = *it;
        vector<int> num1(p1.topo->m), num2(p2.topo->m);
        for (int i = 0; i < p1.topo->m; i++) {
                if (sP1.m2[i]!=-1)
                        num1[i] = num[displace[sP1.m2[i]]];
                else
                        num1[i] = maxNum++;
        }
        for (int i = 0; i < p2.topo->m; i++) {
                if (sP2.m2[i]!=-1)
                        num2[i] = num[displace[mapping[sP2.m2[i]]]];
                else {
                        for (int j = 0; j < p1.topo->m; j++)
                                if (sP1.m1[j] == sP2.m1[i])
                                        num2[i] = num1[j];
                }
        }
        p1.showConstruction(done,num1,maxNum,fout);
        p2.showConstruction(done,num2,maxNum,fout);
}
*/
void Polyhedron::comb(const set<Polyhedron> &done, SeperatedPolyhedron sP1,
                      SeperatedPolyhedron sP2, vector<int> num, int &maxNum,
                      vector<int> &structure) const {
  Topology topology;

  topology.n = sP1.p2.topo.n + sP2.p2.topo.n;
  topology.m = topology.n + 2;
  bool valid = true;
  if (topology.n >= 40)
    valid = false;
  if (!valid) {
    return;
  }
  topology.graphF.assign(topology.n * 4, 0);
  topology.graphP.assign(topology.n * 4, 0);
  topology.edge_valence.assign(topology.n * 4, 1);
  vector<PointType> mapping(sP2.p2.topo.m, -1);
  for (int i = 0; i < sP1.p1.topo.m; i++)
    if (sP2.mapping[i] != -1)
      mapping[sP2.mapping[i]] = sP1.mapping[i];
  int maxPoint = sP1.p2.topo.m;
  for (int i = 0; i < sP2.p2.topo.m; i++)
    if (mapping[i] == -1)
      mapping[i] = maxPoint++;
  for (int i = 0; i < sP1.p2.topo.n * 4; i++)
    topology.graphP[i] = sP1.p2.topo.graphP[i];
  for (int i = 0; i < sP2.p2.topo.n * 4; i++)
    topology.graphP[i + sP1.p2.topo.n * 4] = mapping[sP2.p2.topo.graphP[i]];
  map<pair<PointType, PointType>, int> mm;
  for (int i = 0; i < sP1.p2.topo.n * 4; i++)
    if (sP1.p2.topo.graphF[i] != -1)
      topology.graphF[i] = sP1.p2.topo.graphF[i];
    else {
      PointType x = sP1.p2.topo.graphP[i],
                y = sP1.p2.topo.graphP[(i & 3) == 3 ? i - 3 : i + 1];
      pair<PointType, PointType> pr;
      if (x < y)
        pr = make_pair(x, y);
      else
        pr = make_pair(y, x);
      mm[pr] = i;
    }

  for (int i = 0; i < sP2.p2.topo.n * 4; i++)
    if (sP2.p2.topo.graphF[i] != -1)
      topology.graphF[i + sP1.p2.topo.n * 4] =
          sP2.p2.topo.graphF[i] + sP1.p2.topo.n;
    else {
      PointType x = mapping[sP2.p2.topo.graphP[i]],
                y = mapping[sP2.p2.topo.graphP[(i & 3) == 3 ? i - 3 : i + 1]];
      pair<PointType, PointType> pr;
      if (x < y)
        pr = make_pair(x, y);
      else
        pr = make_pair(y, x);
      if (mm.find(pr) == mm.end()) {
        printf("can not find correspond face\n");
      } else {
        int point = mm[pr];
        topology.graphF[i + sP1.p2.topo.n * 4] = point / 4;
        topology.graphF[point] = i / 4 + sP1.p2.topo.n;
      }
    }

  vector<PointType> displace;
  topology.regular(displace);
  Polyhedron p1, p2;
  p1.topoNum = father1;
  set<Polyhedron>::iterator it = done.find(p1);
  p1 = *it;
  p2.topoNum = father2;
  it = done.find(p2);
  p2 = *it;
  vector<int> num1(p1.topo->m), num2(p2.topo->m);
  for (int i = 0; i < p1.topo->m; i++) {
    if (sP1.m2[i] != -1)
      num1[i] = num[displace[sP1.m2[i]]];
    else
      num1[i] = maxNum++;
  }
  for (int i = 0; i < p2.topo->m; i++) {
    if (sP2.m2[i] != -1)
      num2[i] = num[displace[mapping[sP2.m2[i]]]];
    else {
      for (int j = 0; j < p1.topo->m; j++)
        if (sP1.m1[j] == sP2.m1[i])
          num2[i] = num1[j];
    }
  }
  p1.showConstruction(done, num1, maxNum, structure);
  p2.showConstruction(done, num2, maxNum, structure);
}
/*
void Polyhedron::showConstruction(const set<Polyhedron> &done, vector<int> num,
int &maxNum, FILE *fout) const {
        //printf("%d\n", father1);
        if (father1 == -1 && father2 == -1) {
                for (int i = 0; i < topo->n; i ++)
                        fprintf(fout, "%-4d %-4d %-4d %-4d  ",
num[topo->graphP[i*4+0]], num[topo->graphP[i * 4 + 1]], num[topo->graphP[i * 4 +
2]], num[topo->graphP[i * 4 + 3]]); fprintf(fout, "\n");
        }
        else {
                Polyhedron p1, p2;
                p1.topoNum = father1;
                set<Polyhedron>::iterator it = done.find(p1);
                p1 = *it;
                p2.topoNum = father2;
                it = done.find(p2);
                p2 = *it;
                SeperatedPolyhedron div1 = p1.separate(father_root);
                vector<SeperatedPolyhedron> div2 = p2.divide();
                bool find = false;
                for (unsigned int j = 0; j < div2.size(); j++)
                        if (div1.p1 == div2[j].p1) {
                                Polyhedron newP(div1, div2[j]);
                                if (newP.topoNum == topoNum) {
                                        find = true;
                                        comb(done, div1, div2[j], num, maxNum,
fout);
                                }
                        }
                if (!find) {
                        printf("print error\n");
                }
        }
}
*/
void Polyhedron::showConstruction(const set<Polyhedron> &done, vector<int> num,
                                  int &maxNum, vector<int> &structure) const {
  if (father1 == -1 && father2 == -1) {
    for (int i = 0; i < topo->n; i++) {
      structure.push_back(num[topo->graphP[i * 4 + 0]]);
      structure.push_back(num[topo->graphP[i * 4 + 1]]);
      structure.push_back(num[topo->graphP[i * 4 + 2]]);
      structure.push_back(num[topo->graphP[i * 4 + 3]]);
    }
  } else {
    Polyhedron p1, p2;
    p1.topoNum = father1;
    set<Polyhedron>::iterator it = done.find(p1);
    p1 = *it;
    p2.topoNum = father2;
    it = done.find(p2);
    p2 = *it;
    SeperatedPolyhedron div1 = p1.separate(father_root);
    vector<SeperatedPolyhedron> div2 = p2.divide_old();
    bool find = false;
    for (unsigned int j = 0; j < div2.size(); j++)
      if (div1.p1 == div2[j].p1) {
        /*�����ã�ʵ������ע�͵��ӿ��ٶ�
        Polyhedron newP(div1, div2[j]);
        if (newP.topoNum == topoNum) {
                find = true;
                comb(done, div1, div2[j], num, maxNum, structure);
                return;
        }
        */
        find = true;
        comb(done, div1, div2[j], num, maxNum, structure);
        return;
      }
    if (!find) {
      printf("print error\n");
    }
  }
}
/*
bool Polyhedron::show(const set<Polyhedron> &done) const {
        vector<int> num(topo->m);
        for (int i = 0 ; i < topo->m; i++)
                num[i] = i;
        int maxNum = topo->m;
        FILE *fout;
        fopen_s(&fout, "construction.txt", "w");
        showConstruction(done,num,maxNum,fout);
        fclose(fout);
        fopen_s(&fout, "construction.txt", "r");
        bool check[100][200] = {false};
        bool nofind = false;
        int t;
        for (int i = 0; i < divNum; i++) {
                for (int j = 0; j < 24; j++) {
                        if (fscanf(fout, "%d", &t) == EOF)
                                nofind = true;
                        check[i][t] = true;
                }
        }
        bool valid = true;
        for (int i = 0; i < divNum; i++)
                for (int j = 0; j < i; j++) {
                        int same = 0;
                        for (int k = 0; k < 200; k++)
                                if (check[i][k] && check[j][k])
                                        same++;
                        if (same == 3 || same > 4)
                                valid = false;
                }
        fclose(fout);
        if (nofind) {
                valid = false;
        }
        return valid;
}
*/
bool Polyhedron::show(const set<Polyhedron> &done, vector<int> &structure_temp,
                      FILE *fout) const {
  vector<int> num(topo->m);
  for (int i = 0; i < topo->m; i++)
    num[i] = i;
  int maxNum = topo->m;
  // structure_temp.clear();
  vector<int> structure_temp1;
  fprintf(fout, "%d\n", topoNum);
  showConstruction(done, num, maxNum, structure_temp1);
  bool check[100][200] = {false};
  bool nofind = false;
  bool valid = true;
  int t;
  if (structure_temp1.size() < 24 * divNum) {
    nofind = true;
  }
  if (nofind == false) {
    for (int i = 0; i < divNum; i++) {
      for (int j = 0; j < 24; j++) {
        t = structure_temp1[i * 24 + j];
        check[i][t] = true;
      }
    }
    for (int i = 0; i < divNum; i++)
      for (int j = 0; j < i; j++) {
        int same = 0;
        for (int k = 0; k < 200; k++)
          if (check[i][k] && check[j][k])
            same++;
        if (same == 3 || same > 4) {
          valid = false;
        }
      }
  }
  if (nofind) {
    valid = false;
  }
  for (int i = 0; i < divNum; i++) {
    for (int j = 0; j < 24; j++) {
      fprintf(fout, "%d ", structure_temp1[i * 24 + j]);
    }
    fprintf(fout, "\n");
  }
  fprintf(fout, "\n");
  return valid;
}

bool Polyhedron::available() const {
  if (topo == NULL)
    return false;
  if (topo->n > 40) {
    return false;
  }

  return true;
}

Polyhedron::Polyhedron(const SeperatedPolyhedron &sP1,
                       const SeperatedPolyhedron &sP2) {
  // sp1����Ҫ��,sp2�ǵ�����������
  Topology topology;
  topology.n = sP1.p2.topo.n + sP2.p2.topo.n;
  topology.m = topology.n + 2;
  divNum = sP1.divNum + sP2.divNum;
  topology.cellnum = divNum;

  topology.graphF.assign(topology.n * 4, 0);
  topology.graphP.assign(topology.n * 4, 0);
  topology.edge_valence.assign(topology.n * 4, 1);

  vector<PointType> mapping(sP2.p2.topo.m, -1);
  for (int i = 0; i < sP1.p1.topo.m; i++)
    if (sP2.mapping[i] != -1)
      mapping[sP2.mapping[i]] = sP1.mapping[i];
  int maxPoint = sP1.p2.topo.m;
  for (int i = 0; i < sP2.p2.topo.m; i++)
    if (mapping[i] == -1)
      mapping[i] = maxPoint++;
  for (int i = 0; i < sP1.p2.topo.n * 4; i++)
    topology.graphP[i] = sP1.p2.topo.graphP[i];
  for (int i = 0; i < sP2.p2.topo.n * 4; i++)
    topology.graphP[i + sP1.p2.topo.n * 4] = mapping[sP2.p2.topo.graphP[i]];
  map<pair<PointType, PointType>, int> mm;
  for (int i = 0; i < sP1.p2.topo.n * 4; i++) {
    if (sP1.p2.topo.graphF[i] != -1) {
      topology.graphF[i] = sP1.p2.topo.graphF[i];
    } else {
      PointType x = sP1.p2.topo.graphP[i],
                y = sP1.p2.topo.graphP[(i & 3) == 3 ? i - 3 : i + 1];
      pair<PointType, PointType> pr;
      if (x < y)
        pr = make_pair(x, y);
      else
        pr = make_pair(y, x);
      mm[pr] = i;
    }
    topology.edge_valence[i] = sP1.p2.topo.edge_valence[i];
  }
  for (int i = 0; i < sP2.p2.topo.n * 4; i++)
    if (sP2.p2.topo.graphF[i] != -1) {
      topology.graphF[i + sP1.p2.topo.n * 4] =
          sP2.p2.topo.graphF[i] + sP1.p2.topo.n;
      topology.edge_valence[i + sP1.p2.topo.n * 4] = 1;
    } else {
      PointType x = mapping[sP2.p2.topo.graphP[i]],
                y = mapping[sP2.p2.topo.graphP[(i & 3) == 3 ? i - 3 : i + 1]];
      pair<PointType, PointType> pr;
      if (x < y)
        pr = make_pair(x, y);
      else
        pr = make_pair(y, x);
      if (mm.find(pr) == mm.end()) {
        printf("can not find correspond face\n");
      } else {
        int point = mm[pr];
        topology.graphF[i + sP1.p2.topo.n * 4] = point / 4;
        topology.graphF[point] = i / 4 + sP1.p2.topo.n;
        topology.edge_valence[i + sP1.p2.topo.n * 4] =
            topology.edge_valence[point];
      }
    }
  FaceType maxSame = -1;
  bool ok = true;
  vector<FaceType> rec(sP1.p2.topo.m, -1);
  for (int i = 0; i < sP2.p2.topo.n; i++) {
    for (int j = 0; j < 4; j++)
      if (mapping[sP2.p2.topo.graphP[i * 4 + j]] < sP1.p2.topo.m)
        rec[mapping[sP2.p2.topo.graphP[i * 4 + j]]] = i;
    for (unsigned int j = 0; j < sP1.p2.topo.forbidden.size() / 2; j++) {
      int same = 0;
      for (int k = 0; k < 2; k++)
        if (rec[sP1.p2.topo.forbidden[j * 2 + k]] == i)
          same++;
      if (same == 2)
        ok = false;
    }
  }
  if (!ok) {
    topoNum = -1;
    topo = NULL;
    return;
  }
  topology.forbidden = sP1.p2.topo.forbidden;
  for (int i = 0; i < sP2.p2.topo.forbidden.size(); i++)
    topology.forbidden.insert(topology.forbidden.end(),
                              mapping[sP2.p2.topo.forbidden[i]]);
  for (int i = 0; i < sP1.p1.topo.n; i++)
    for (int j = 0; j < 2; j++)
      if (sP1.mapping[sP1.p1.topo.graphP[i * 4 + j]] != -1 &&
          sP1.mapping[sP1.p1.topo.graphP[i * 4 + j + 2]] != -1) {
        topology.forbidden.insert(topology.forbidden.end(),
                                  sP1.mapping[sP1.p1.topo.graphP[i * 4 + j]]);
        topology.forbidden.insert(
            topology.forbidden.end(),
            sP1.mapping[sP1.p1.topo.graphP[i * 4 + j + 2]]);
      }
  vector<PointType> displace;
  topology.regular(displace);
  topoNum = topoSet.insert(topology);
  topo = topoSet.getTopo(topoNum);
  father1 = sP1.father;
  father2 = sP2.father;
  if (topoNum == 1) {
    bool same[18][18] = {false};
    for (int i = 0; i < topology.forbidden.size() / 2; i++) {
      same[topology.forbidden[i * 2]][topology.forbidden[i * 2 + 1]] = true;
      same[topology.forbidden[i * 2 + 1]][topology.forbidden[i * 2]] = true;
    }
    bool ok1 = true;
    if (same[1][9] || same[8][9] || same[12][9] || same[13][9])
      ok1 = false;
    if (same[1][8] || same[1][11] || same[2][13] || same[8][17] ||
        same[4][12] || same[11][17])
      ok1 = false;
    if (ok1 == false) {
      topoNum = -1;
      topo = NULL;
    }
  }
  father_root = sP1.father_root;
}
Polyhedron::~Polyhedron() {}

int Polyhedron::divSum() const { return divNum; }

void Polyhedron::bfs(int st, vector<int> &certain, vector<int> &connection,
                     int &status) const {
  static vector<FaceType> queue(100);
  int head = 0, tail = 1;
  bool unselected = false;
  queue[0] = st;
  connection[st] = st;
  while (head < tail) {
    FaceType face = queue[head];
    if (certain[face] < 0)
      unselected = true;
    for (int i = 0; i < 4; i++)
      if (connection[topo->graphF[face * 4 + i]] == -1 &&
          certain[topo->graphF[face * 4 + i]] < 2) {
        connection[topo->graphF[face * 4 + i]] = st;
        queue[tail++] = topo->graphF[face * 4 + i];
      }
    head++;
  }
  if (unselected)
    if (status == -1)
      status = st;
    else
      status = -2;
}

bool Polyhedron::seperate_judge(FaceType center, vector<int> select) const {
  //�ж���������Χ�������ߵĶ����Ƿ����Ҫ��
  for (int i = 0; i < 4; i++) {
    if (select[i] == 1) {
      if (topo->edge_valence[center * 4 + i] == 1 ||
          topo->edge_valence[center * 4 + i] > 4)
        return false;
    } else {
      if (topo->edge_valence[center * 4 + i] >= 4)
        return false;
    }
  }
  //�ж���Χ�ĸ�������
  for (int i = 0; i < 4; i++) {
    if (select[i] == 1) {
      char currentF = topo->graphF[center * 4 + i];
      for (int j = 0; j < 4; j++) {
        if (topo->graphF[currentF * 4 + j] == center &&
            topo->edge_valence[currentF * 4 + (j + 2) % 4] >= 4)
          return false;
        if (topo->graphF[currentF * 4 + j] ==
            topo->graphF[center * 4 + (i + 1) % 4]) {
          if (select[(i + 1) % 4] == 1) {
            if (topo->edge_valence[currentF * 4 + j] == 1 ||
                topo->edge_valence[currentF * 4 + j] > 4)
              return false;

          } else {
            if (topo->edge_valence[currentF * 4 + j] >= 4)
              return false;
          }
        }
        if (topo->graphF[currentF * 4 + j] ==
            topo->graphF[center * 4 + (i + 3) % 4]) {
          if (select[(i + 3) % 4] != 1) {
            if (topo->edge_valence[currentF * 4 + j] >= 4)
              return false;
          }
        }
      }
    }
  }
  return true;
}

SeperatedPolyhedron Polyhedron::separate(const vector<int> &certain) const {
  int n1 = 0, n2 = 0, s1 = 0, s2 = 0;
  vector<FaceType> mapF1(topo->n), mapF2(topo->n);
  vector<PointType> mapP1(topo->m), mapP2(topo->m);
  int mF1 = 0, mF2 = 0, mP1 = 0, mP2 = 0;
  vector<PointType> inMapP1(topo->m, -1), inMapP2(topo->m, -1);
  vector<FaceType> inMapF1(topo->n, -1), inMapF2(topo->n, -1);
  for (int i = 0; i < topo->n; i++) {
    if (certain[i] >= 2) {
      inMapF1[i] = mF1;
      mapF1[mF1++] = i;
      for (int j = 0; j < 4; j++)
        if (inMapP1[topo->graphP[i * 4 + j]] == -1) {
          inMapP1[topo->graphP[i * 4 + j]] = mP1;
          mapP1[mP1++] = topo->graphP[i * 4 + j];
        }
    } else if (certain[i] <= 1) { //ȷ����ѡ��δ����ѡ�����(������ȷ��ѡ���������)
      inMapF2[i] = mF2;
      mapF2[mF2++] = i;
      for (int j = 0; j < 4; j++)
        if (inMapP2[topo->graphP[i * 4 + j]] == -1) {
          inMapP2[topo->graphP[i * 4 + j]] = mP2;
          mapP2[mP2++] = topo->graphP[i * 4 + j];
        }
    } else
      printf("Some points have not been certained.\n");
  }
  SeperatedPolyhedron sP;
  Polygon p1, p2;
  sP.divNum = divNum;
  sP.n = topo->n;
  sP.m = topo->m;
  if (mF1 > 5) {
    p1.topo.n = 0;
    p2.topo.n = 0;
    sP.p1 = p1;
    sP.p2 = p2;
    return sP;
  }
  Topology topo1, topo2;
  topo1.n = mF1;
  topo1.m = mP1;
  topo1.graphF.assign(topo1.n * 4, -1);
  topo1.graphP.assign(topo1.n * 4, -1);
  topo1.edge_valence.assign(topo1.n * 4, 1);
  for (int i = 0; i < topo1.n; i++)
    for (int j = 0; j < 4; j++) {
      topo1.graphF[i * 4 + j] = inMapF1[topo->graphF[mapF1[i] * 4 + j]];
      topo1.edge_valence[i * 4 + j] = topo->edge_valence[mapF1[i] * 4 + j];
    }
  for (int i = 0; i < topo1.n; i++)
    for (int j = 0; j < 4; j++)
      topo1.graphP[i * 4 + j] = inMapP1[topo->graphP[mapF1[i] * 4 + j]];
  int dia = topo1.diameter();
  if (topo1.n == 4 && dia != 6 || topo1.n == 5 && dia != 9) {
    p1.topo.n = 0;
    p2.topo.n = 0;
    sP.p1 = p1;
    sP.p2 = p2;
    return sP;
  }

  topo2.n = mF2;
  topo2.m = mP2;
  topo2.graphF.assign(topo2.n * 4, -1);
  topo2.graphP.assign(topo2.n * 4, -1);
  topo2.edge_valence.assign(topo2.n * 4, 1);
  for (int i = 0; i < topo2.n; i++)
    for (int j = 0; j < 4; j++) {
      topo2.graphF[i * 4 + j] = inMapF2[topo->graphF[mapF2[i] * 4 + j]];
      topo2.edge_valence[i * 4 + j] = topo->edge_valence[mapF2[i] * 4 + j];
    }
  for (int i = 0; i < topo2.n; i++) {
    for (int j = 0; j < 4; j++) {
      if (topo2.graphF[i * 4 + j] == -1)
        topo2.edge_valence[i * 4 + j] += 1;
    }
  }
  for (int i = 0; i < topo2.n; i++)
    for (int j = 0; j < 4; j++)
      topo2.graphP[i * 4 + j] = inMapP2[topo->graphP[mapF2[i] * 4 + j]];

  if (topo1.n == 3) {
    // printf("need to stop\n");
  }
  for (unsigned int i = 0; i < topo->forbidden.size() / 2; i++) {
    if (inMapP1[topo->forbidden[i * 2]] != -1 &&
        inMapP1[topo->forbidden[i * 2 + 1]] != -1) {
      topo1.forbidden.insert(topo1.forbidden.end(),
                             inMapP1[topo->forbidden[i * 2]]);
      topo1.forbidden.insert(topo1.forbidden.end(),
                             inMapP1[topo->forbidden[i * 2 + 1]]);
    }
    if (inMapP2[topo->forbidden[i * 2]] != -1 &&
        inMapP2[topo->forbidden[i * 2 + 1]] != -1) {
      topo2.forbidden.insert(topo2.forbidden.end(),
                             inMapP2[topo->forbidden[i * 2]]);
      topo2.forbidden.insert(topo2.forbidden.end(),
                             inMapP2[topo->forbidden[i * 2 + 1]]);
    }
  }
  vector<PointType> displace;
  topo1.regular(displace);
  for (int i = 0; i < topo->m; i++)
    if (inMapP1[i] != -1)
      inMapP1[i] = displace[inMapP1[i]];
  topo2.regular(displace);
  for (int i = 0; i < topo->m; i++)
    if (inMapP2[i] != -1)
      inMapP2[i] = displace[inMapP2[i]];
  p1.topo = topo1;
  p2.topo = topo2;

  sP.p1 = p1;
  sP.p2 = p2;
  sP.father = topoNum;
  vector<PointType> mapping(p1.topo.m, -1);
  for (int i = 0; i < topo->m; i++)
    if (inMapP1[i] != -1 && inMapP2[i] != -1)
      mapping[inMapP1[i]] = inMapP2[i];
  sP.mapping = mapping;
  sP.m1.assign(topo->m, -1);
  sP.m2.assign(topo->m, -1);
  sP.m1 = inMapP1;
  sP.m2 = inMapP2;
  sP.father_root = certain;
  return sP;
}

struct ElementStack {
  int selectNum;    //Ŀǰѡ�ǵ�����
  int unselectNum;  //Ŀǰѡ�������
  int candidateSum; //Ŀǰ����ѡ����е�����
  int candidate;    //Ҫѡ�������
};
vector<SeperatedPolyhedron> Polyhedron::divide() const {
  //�������������Ϊ�����֣������ֱַ���ͨ���õ����зָ�
  //һ���ְ������Ϊ0���棬��һ���ֲ�����
  //��0���濪ʼ��չ�������б�֤�˲�����ͨ
  set<SeperatedPolyhedron> result;
  vector<int> certain(topo->n, 0);
  SeperatedPolyhedron sP;
  for (int kk = 0; kk < topo->n; kk++) {
    certain[kk] = 2;
    // 1����,lowһ��д����
    vector<int> select(4, 0);
    if (seperate_judge(kk, select)) {
      sP = separate(certain);
      if (sP.p1.topo.n * sP.p2.topo.n != 0) {
        set<SeperatedPolyhedron>::iterator it = result.find(sP);
        if (it == result.end())
          result.insert(sP);
      }
    }
    int stack[10];
    int top = -1, select_num = 0;
    while (1) {
      if (select_num < 4) {
        select[select_num] = 1;
        if (certain[topo->graphF[kk * 4 + select_num]] >= 0 &&
            seperate_judge(kk, select)) {
          stack[++top] = select_num;

          certain[topo->graphF[kk * 4 + select_num]] = 2;
          sP = separate(certain);
          if (sP.p1.topo.n * sP.p2.topo.n != 0) {
            set<SeperatedPolyhedron>::iterator it = result.find(sP);
            if (it == result.end())
              result.insert(sP);
          }
        } else {
          select[select_num] = 0;
        }
        select_num++;
      } else {
        if (top == -1) {
          break;
        }
        certain[topo->graphF[kk * 4 + stack[top]]] = 0;
        select[stack[top]] = 0;
        top--;
        if (top == -1) {
          break;
        }
        select_num = stack[top];
        certain[topo->graphF[kk * 4 + stack[top]]] = 0;
        select[stack[top]] = 0;
        top--;
        select_num++;
      }
    }
    certain[kk] = -2;
  }
  vector<SeperatedPolyhedron> divided(result.begin(), result.end());
  // printf("%-6d %-6d %-8d ", stopnum, result.size(), topoSet.maxSize());
  return divided;
}

vector<SeperatedPolyhedron> Polyhedron::divide_old() const {
  //�������������Ϊ�����֣������ֱַ���ͨ���õ����зָ�
  //һ���ְ������Ϊ0���棬��һ���ֲ�����
  //��0���濪ʼ��չ�������б�֤�˲�����ͨ
  set<SeperatedPolyhedron> result;
  vector<int> certain(topo->n,
                      0); //����0=δȷ�� 1=�����ѡ���� 2,3=ȷ��ѡ -1,-2=ȷ����ѡ
  vector<FaceType> select(topo->n, 0);   //������ȷ����
  vector<FaceType> unselect(topo->n, 0); //����ȷ����ѡ��
  vector<FaceType> selectQueue(topo->n, 0);
  vector<ElementStack> stack(topo->n);
  int selectSum = 1, unselectSum = 0;
  int stopnum = 0;
  // printf("%-3d %-3d %-3d ", topo->n, topo->m, divNum);
  for (int kk = 0; kk < topo->n; kk++) {
    certain[kk] = 2;
    select[0] = kk;
    int top = 0, tail = 0;
    for (int j = 0; j < 4; j++)
      if (certain[topo->graphF[kk * 4 + j]] == 0) {
        selectQueue[tail++] = topo->graphF[kk * 4 + j];
        certain[topo->graphF[kk * 4 + j]] = 1;
      }
    stack[top].selectNum = selectSum;
    stack[top].unselectNum = unselectSum;
    stack[top].candidate = -1;
    stack[top].candidateSum = tail;
    vector<int> connection(topo->n);
    SeperatedPolyhedron sP = separate(certain);
    result.insert(sP);
    while (top >= 0) {
      while (selectSum > stack[top].selectNum)
        certain[select[--selectSum]] -= 2;
      while (tail > stack[top].candidateSum)
        certain[selectQueue[--tail]] -= 1;
      if (stack[top].candidate != -1 &&
          certain[selectQueue[stack[top].candidate]] == 1) {
        certain[selectQueue[stack[top].candidate]] -= 2;
        unselect[unselectSum++] = selectQueue[stack[top].candidate];
        if (unselectSum == 0) {
          int check = -1;
          connection.assign(topo->n, -1);
          bfs(selectQueue[stack[top].candidate], certain, connection, check);
          for (int i = 0; i < topo->n; i++)
            if (connection[i] != selectQueue[stack[top].candidate] &&
                certain[i] < 2) {
              certain[i] += 2;
              select[selectSum++] = i;
            }
        }
      }
      while (stack[top].candidate < tail &&
             (stack[top].candidate < 0 ||
              certain[selectQueue[stack[top].candidate]] != 1))
        stack[top].candidate++;
      if (stack[top].candidate >= tail) {
        while (unselectSum > stack[top].unselectNum)
          certain[unselect[--unselectSum]] += 2;
        top--;
        continue;
      }
      if (top >= 5)
        continue;
      int selectedFace = selectQueue[stack[top].candidate];
      certain[selectedFace] += 2;
      select[selectSum++] = selectedFace;
      int check = -1, diff = 0;
      connection.assign(topo->n, -1);
      for (int i = 0; i < 4; i++)
        if (connection[topo->graphF[selectedFace * 4 + i]] == -1 &&
            certain[topo->graphF[selectedFace * 4 + i]] < 2) {
          bfs(topo->graphF[selectedFace * 4 + i], certain, connection, check);
          diff++;
        }
      if (check == -2)
        continue;
      if (check != -1) {
        for (int i = 0; i < topo->n; i++)
          if ((certain[i] == 0 || certain[i] == 1) && connection[i] != check) {
            certain[i] += 2;
            select[selectSum++] = i;
          }
      }
      for (int i = stack[top].selectNum; i < selectSum; i++)
        for (int j = 0; j < 4; j++)
          if (certain[topo->graphF[select[i] * 4 + j]] == 0) {
            selectQueue[tail++] = topo->graphF[select[i] * 4 + j];
            certain[topo->graphF[select[i] * 4 + j]] = 1;
          }
      if (check != -1 || diff == 1) {
        //Ԥ��͹���Լ춨��������
        stopnum++;
        SeperatedPolyhedron sP = separate(certain);
        if (sP.p1.topo.n * sP.p2.topo.n != 0) {
          set<SeperatedPolyhedron>::iterator it = result.find(sP);
          if (it == result.end())
            result.insert(sP);
          else
            continue;
        }
      }
      top++;
      stack[top].selectNum = selectSum;
      stack[top].unselectNum = unselectSum;
      stack[top].candidate = stack[top - 1].candidate;
      stack[top].candidateSum = tail;
    }
    certain[kk] = -2;
    unselect[unselectSum++] = kk;
    for (int i = 0; i < 4; i++)
      if (certain[topo->graphF[kk * 4 + i]] == 1)
        certain[topo->graphF[kk * 4 + i]] = 0;
  }
  vector<SeperatedPolyhedron> divided(result.begin(), result.end());
  // printf("%-6d %-6d %-8d ", stopnum, result.size(), topoSet.maxSize());
  return divided;
}

bool Polyhedron::operator<(const Polyhedron &topo2) const {
  return topoNum < topo2.topoNum;
}

bool Polyhedron::operator==(const Polyhedron &topo2) const {
  return topoNum == topo2.topoNum;
}

bool Polyhedron::operator!=(const Polyhedron &topo2) const {
  return topoNum != topo2.topoNum;
}
bool PolyhedronCompare::operator()(const Polyhedron &s1,
                                   const Polyhedron &s2) const {
  return (s1.divNum < s2.divNum) ||
         (s1.divNum == s2.divNum && s1.topo->n < s2.topo->n) ||
         (s1.divNum == s2.divNum && s1.topo->n == s2.topo->n &&
          s1.topoNum < s2.topoNum);
}

void Polyhedron::print(FILE *fout) const {
  fprintf(fout, "idnum=%d  divnum=%d  fatherid=%d\n", topoNum, divNum, father1);
  topo->print(fout);
  fprintf(fout, "father_root: %d ", father_root.size());
  for (int i = 0; i < father_root.size(); i++) {
    fprintf(fout, "%d ", father_root[i]);
  }
  fprintf(fout, "\n");
  fprintf(fout, "\n");
}

void Polyhedron::addDiameter() const {
  int dia = topo->diameter();
  rec[topo->n / 2 - 2][dia]++;
  if (Topology::maxD[topo->n / 2] >= dia &&
      rec[topo->n / 2 - 2][dia] >= Topology::asdf[topo->n / 2 - 2]) {
    Topology::maxD[topo->n / 2] = dia - 1;
    Topology::asdf[topo->n / 2 - 2] += Topology::asdf[topo->n / 2 - 2] / 10;
  }
}

void Polyhedron::showdia() const {
  FILE *fdia;
  fopen_s(&fdia, "diameter.txt", "w");
  int sum = 0;
  for (int i = 0; i < 20; i++) {
    fprintf(fdia, "n=%d\n", i * 2 + 4);
    for (int j = 0; j < 1000; j++)
      if (rec[i][j] > 0) {
        fprintf(fdia, "%d:%d ", j, rec[i][j]);
        sum += rec[i][j];
      }
    fprintf(fdia, "\n");
  }
  fprintf(fdia, "%d\n", sum);
  fclose(fdia);
}

Polyhedron Polyhedron::load(set<Polyhedron> &done) {
  FILE *fin;
  errno_t err = fopen_s(&fin, "result.txt", "r");
  char str[20];
  int idnum, fatherid, divnum, n, m;
  Polyhedron poly;
  while (fscanf(fin, "%s", str) != EOF) {
    sscanf(str + 6, "%d", &idnum);
    fscanf(fin, "%s", str);
    sscanf(str + 7, "%d", &divnum);
    fscanf(fin, "%s", str);
    sscanf(str + 9, "%d", &fatherid);
    fscanf(fin, "%d%d", &n, &m);
    // if (idnum == 445796) {
    // fatherid = 436425;
    // divnum = 18;
    //}
    Topology topology;
    topology.n = n;
    topology.m = m;
    topology.graphF.resize(n * 4);
    topology.graphP.resize(n * 4);
    int dd;
    for (int i = 0; i < n * 4; i++) {
      fscanf(fin, "%d", &dd);
      topology.graphF[i] = dd;
    }
    for (int i = 0; i < n * 4; i++) {
      fscanf(fin, "%d", &dd);
      topology.graphP[i] = dd;
    }
    fscanf(fin, "%s", str);
    fscanf(fin, "%d", &dd);
    poly.father_root.resize(dd);
    for (int i = 0; i < dd; i++) {
      fscanf(fin, "%d", &poly.father_root[i]);
    }
    if (topoSet.size <= idnum) {
      topoSet.size = idnum + 1;
      topoSet.list.resize(idnum + 1);
    }
    map<Topology, int>::iterator address;
    map<Topology, int>::value_type p(topology, idnum);
    pair<map<Topology, int>::iterator, bool> res = topoSet.content.insert(p);
    address = res.first;
    topoSet.list[idnum] = &address->first;
    poly.divNum = divnum;
    poly.topoNum = idnum;
    poly.topo = topoSet.getTopo(poly.topoNum);
    if (n != 6) {
      poly.father1 = fatherid;
      poly.father2 = 0;
    } else {
      poly.father1 = -1;
      poly.father2 = -1;
    }
    done.insert(poly);
    if (done.size() % 10000 == 0)
      printf("%d\n", done.size());
    if (idnum > 300000)
      break;
    /*if (done.size()==5800) {
            printf("%d\n", done.size());
            break;
    }*/
  }
  return poly;
}

int Polyhedron::load_custom() {
  FILE *fin;
  int n, m, div;
  Topology topology;
  fin = fopen("custom_in.txt", "r");
  fscanf(fin, "%d", &div);
  fscanf(fin, "%d", &n);
  fscanf(fin, "%d", &m);
  topology.n = n;
  topology.m = m;
  topology.graphF.resize(n * 4);
  topology.graphP.resize(n * 4);
  int dd;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 4; j++) {
      fscanf(fin, "%d", &dd);
      topology.graphP[i * 4 + j] = dd;
    }
  }
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 4; j++) {
      fscanf(fin, "%d", &dd);
      topology.graphF[i * 4 + j] = dd;
    }
  }
  topology.regular();
  // topology.regular();
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 4; j++) {
      printf("%d ", topology.graphP[i * 4 + j]);
    }
  }
  printf("\n");
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 4; j++) {
      printf("%d ", topology.graphF[i * 4 + j]);
    }
  }
  printf("\n");
  printf("%d\n", topology.diameter());
  printf("%d\n", topology.maxD[topology.n / 2]);
  map<Topology, int>::iterator it;
  it = topoSet.content.find(topology);
  if (it == topoSet.content.end()) {
    return 0;
  }
  return 1;
}

void Polyhedron::free() {
  Topology newt;
  newt.n = topo->n;
  newt.m = topo->m;
  newt.graphF = topo->graphF;
  topoSet.content.erase(topoSet.content.find(*topo));
  map<Topology, int>::value_type p(newt, topoNum);
  pair<map<Topology, int>::iterator, bool> res = topoSet.content.insert(p);
  topo = topoSet.list[topoNum] = &(res.first->first);
}