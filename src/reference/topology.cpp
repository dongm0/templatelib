#include "topology.h"
int Topology::maxD[30] = {0,   0,   0,   12,  0,   140, 140, 140, 140, 140,
                          140, 140, 140, 140, 136, 143, 156, 168, 182, 195,
                          0,   0,   0,   0,   0,   0,   0,   0};
// int Topology::maxD[30] = {
// 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
int Topology::asdf[30] = {
    10000, 10000, 10000, 20000, 20000, 20000, 20000, 30000, 40000, 50000,
    60000, 50000, 50000, 20000, 20000, 20000, 10000, 10000, 10000, 10000,
    10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000};
int maxface[18] = {6,  10, 14, 18, 22, 24, 26, 28, 28,
                   30, 30, 30, 30, 30, 30, 30, 30, 30};

Topology::Topology() { return; }

Topology::~Topology() { return; }

int Topology::pointSum() const { return m; }

int Topology::faceSum() const { return n; }

struct ElementQueue {
  int face, stx, sty;
};

void Topology::regular() {
  vector<PointType> displace;
  regular(displace);
}

void Topology::regular(vector<PointType> &displace) {
  if (n == 1) {
    displace.resize(4);
    for (int i = 0; i < 4; i++)
      displace[i] = i;
    return;
  }
  //考虑加入nowmappingF和nowGrapfF的对应，用来记录
  vector<PointType> bestGraphP, nowGraphP(n * 4);
  vector<FaceType> bestGraphF, nowGraphF(n * 4);
  vector<FaceType> bestSame, nowSame(n);
  vector<FaceType> bestMappingF, nowMappingF(n);
  vector<PointType> bestMappingP, nowMappingP(m);
  vector<PointType> bestForbidden, nowForbidden(forbidden.size());
  vector<ElementQueue> queue(n);
  vector<int> inqueueF(n, 0), inqueueP(m, 0);
  vector<char> bestValence(n * 4);
  vector<char> nowValence(n * 4);
  int sign = 0;
  int st, ed;
  for (int i = 0; i < n; i++)
    for (int j = 0; j < 4; j++)
      for (int k = 0; k <= 1; k++)
        if (graphF[i * 4 + j] != -1) {
          sign++;
          PointType pointMapping = 0;
          int com = (bestGraphF.size() == 0);
          st = 0;
          queue[st].face = i;
          queue[st].stx = graphP[i * 4 + (j + k) % 4];
          queue[st].sty = graphP[i * 4 + (j + 1 - k) % 4];
          inqueueF[i] = sign;
          nowMappingF[i] = 0;
          ed = 1;
          while (st < ed) {
            int x, y, direction, neibor;
            int face = queue[st].face, stx = queue[st].stx, sty = queue[st].sty;

            for (x = 0; x < 4; x++)
              if (stx == graphP[face * 4 + x])
                break;
            for (y = 0; y < 4; y++)
              if (sty == graphP[face * 4 + y])
                break;
            if ((x + 1) % 4 == y) {
              direction = 1;
              neibor = x;
            } else if ((y + 1) % 4 == x) {
              direction = -1;
              neibor = y;
            } else {
              printf("error in polygon regular\n");
            }
            for (int l = 0; l < 4; l++) {
              if (inqueueP[graphP[face * 4 + x]] != sign) {
                nowMappingP[graphP[face * 4 + x]] = pointMapping++;
                inqueueP[graphP[face * 4 + x]] = sign;
              }
              if ((graphF[face * 4 + neibor] != -1) &&
                  inqueueF[graphF[face * 4 + neibor]] != sign) {
                queue[ed].face = graphF[face * 4 + neibor];
                queue[ed].stx = graphP[face * 4 + x];
                queue[ed].sty = graphP[face * 4 + y];
                inqueueF[graphF[face * 4 + neibor]] = sign;
                nowMappingF[graphF[face * 4 + neibor]] = ed;
                ed++;
              }
              if (graphF[face * 4 + neibor] != -1) {
                nowGraphF[st * 4 + l] = nowMappingF[graphF[face * 4 + neibor]];
              } else
                nowGraphF[st * 4 + l] = -1;
              //
              nowValence[st * 4 + l] = edge_valence[face * 4 + neibor];
              //
              if (!com)
                if (nowGraphF[st * 4 + l] < bestGraphF[st * 4 + l])
                  com = 1;
                else if (nowGraphF[st * 4 + l] > bestGraphF[st * 4 + l])
                  com = -1;
              nowGraphP[st * 4 + l] = nowMappingP[graphP[face * 4 + x]];
              x = (x + direction + 4) % 4;
              y = (y + direction + 4) % 4;
              neibor = (neibor + direction + 4) % 4;
            }
            st++;
            if (com == -1)
              break;
          }
          /*
          if (com!=-1)
          for (unsigned int p = 0; p < forbidden.size()/2; p++) {
                  int x, y;
                  x = forbidden[p * 2];
                  y = forbidden[p * 2 + 1];
                  x = nowMappingP[x];
                  y = nowMappingP[y];
                  if (x > y) {
                          int tmp = x;
                          x = y;
                          y = tmp;
                  }
                  unsigned int q = p;
                  for (q = p; q > 0 && (nowForbidden[q * 2 - 2] > x ||
          nowForbidden[q * 2 - 2] == x && nowForbidden[q * 2 - 1] > y); q--) {
                          nowForbidden[q * 2] = nowForbidden[q * 2 - 2];
                          nowForbidden[q * 2 + 1] = nowForbidden[q * 2 - 1];
                  }
                  nowForbidden[q * 2] = x;
                  nowForbidden[q * 2 + 1] = y;
          }
          */
          if (com != -1)
            if (bestGraphF.size() == 0 || nowGraphF < bestGraphF ||
                nowGraphF == bestGraphF) {
              bestGraphF = nowGraphF;
              bestGraphP = nowGraphP;
              bestMappingP = nowMappingP;
              bestMappingF = nowMappingF;
              bestSame = nowSame;
              bestValence = nowValence;
            }
        }
  edge_valence = bestValence;
  graphP = bestGraphP;
  graphF = bestGraphF;
  displace = bestMappingP;
  // forbidden = bestForbidden;
  for (unsigned int i = 0; i < forbidden.size() / 2; i++) {
    int x, y;
    x = forbidden[i * 2];
    y = forbidden[i * 2 + 1];
    x = displace[x];
    y = displace[y];
    forbidden[i * 2] = x;
    forbidden[i * 2 + 1] = y;
  }
}

bool Topology::operator<(const Topology &topo2) const {
  if (n < topo2.n)
    return true;
  if (n > topo2.n)
    return false;
  if (m < topo2.m)
    return true;
  if (m > topo2.m)
    return false;
  for (int i = 0; i < n * 4; i++) {
    if (graphF[i] < topo2.graphF[i])
      return true;
    if (graphF[i] > topo2.graphF[i])
      return false;
  }
  /*
  if (forbidden < topo2.forbidden)
          return true;
  if (forbidden > topo2.forbidden)
          return false;
          */
  return false;
}

bool Topology::operator==(const Topology &topo2) const {
  if (n != topo2.n || m != topo2.m)
    return false;
  for (int i = 0; i < n * 4; i++)
    if (graphF[i] != topo2.graphF[i])
      return false;
  return true;
}

bool Topology::operator!=(const Topology &topo2) const {
  return !operator==(topo2);
}

void Topology::print(FILE *fout) const {
  fprintf(fout, "%d %d\n", n, m);
  for (int i = 0; i < n * 4; i++)
    fprintf(fout, "%d ", graphF[i]);
  fprintf(fout, "\n");
  for (int i = 0; i < n * 4; i++)
    fprintf(fout, "%d ", graphP[i]);
  fprintf(fout, "\n");
  for (int i = 0; i < n * 4; i++) {
    fprintf(fout, "%d ", edge_valence[i]);
  }
  fprintf(fout, "\n");
}

int Topology::diameter() const {
  int maxD = 0, head, tail;
  vector<int> dis(n), inque(n, -1), queue(n);
  for (int i = 0; i < n; i++) {
    dis[i] = 0;
    inque[i] = i;
    head = 0;
    tail = 1;
    queue[0] = i;
    while (head < tail) {
      int point = queue[head++];
      for (int j = 0; j < 4; j++)
        if (graphF[point * 4 + j] != -1 && inque[graphF[point * 4 + j]] != i) {
          inque[graphF[point * 4 + j]] = i;
          queue[tail++] = graphF[point * 4 + j];
          dis[graphF[point * 4 + j]] = dis[point] + 1;
        }
    }
    if (tail != n)
      maxD = 10000000;
    else
      maxD += dis[queue[n - 1]];
    // if (dis[queue[n - 1]] > maxD)
    // maxD = dis[queue[n - 1]];
  }
  return maxD;
}

bool Topology::available() const {
  /*if (n == 6) {
          print(stdout);
          system("pause");
  }*/
  for (int i = 0; i < n; i++)
    for (int j = 0; j < 3; j++)
      if (graphF[i * 4 + j] != -1)
        for (int k = j + 1; k < 4; k++)
          if (graphF[i * 4 + j] == graphF[i * 4 + k])
            return false;
  vector<PointType> rec(m, -1);
  int count;
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 4; j++)
      rec[graphP[i * 4 + j]] = i;
    for (int j = i + 1; j < n; j++) {
      count = 0;
      for (int k = 0; k < 4; k++)
        if (rec[graphP[j * 4 + k]] == i)
          count++;
      if (count > 2)
        return false;
      if (count == 2) {
        bool adj = false;
        for (int k = 0; k < 4; k++)
          if (graphF[i * 4 + k] == j)
            adj = true;
        if (!adj)
          return false;
      }
    }
  }
  // return true;
  // return diameter()<=maxD[n/2];
  return n <= maxface[cellnum - 1];
}