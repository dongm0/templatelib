#include "hexmodu.hpp"
#include <algorithm>
#include <cassert>
#include <map>
#include <queue>
#include <set>
#include <unordered_set>

namespace {
using namespace std;

template <typename It>
It FindElementInLine(It begin, It end, It::value_type tar) {
  for (auto it = begin; it != end; ++it) {
    if (*it == tar) {
      return it;
    }
  }
  return end;
}

template <typename Container, size_t size_n>
array<Container::value_type, size_n> GetPartInLine(const Container &line,
                                                   size_t index) {
  array<Container::valur_type, size_n> res;
  for (int i = 0; i < size_n; ++i) {
    res[i] = line.at(index * size_n + i);
  }
  return res;
}

enum class FaceDir {
  XF = 0,
  XB = 1,
  YF = 2,
  YB = 3,
  ZF = 4,
  ZB = 5,
  INVALID = 6
};

FaceDir opposite_face[6] = {FaceDir::XB, FaceDir::XF, FaceDir::YB,
                            FaceDir::YF, FaceDir::ZB, FaceDir::ZF};

FaceDir adj_face[24] = {FaceDir::YF, FaceDir::ZF, FaceDir::YB, FaceDir::ZB,
                        FaceDir::YF, FaceDir::ZB, FaceDir::YB, FaceDir::ZF,
                        FaceDir::XF, FaceDir::ZB, FaceDir::XB, FaceDir::ZF,
                        FaceDir::XF, FaceDir::ZF, FaceDir::XB, FaceDir::ZB,
                        FaceDir::XF, FaceDir::YF, FaceDir::XB, FaceDir::YB,
                        FaceDir::XF, FaceDir::YB, FaceDir::XB, FaceDir::YF};

array<Byte, 4> FaceVertices(array<Byte, 8> cell, FaceDir f) {
  if (f == FaceDir::XF) {
    return {cell[2], cell[6], cell[5], cell[1]};
  } else if (f == FaceDir::XB) {
    return {cell[7], cell[3], cell[0], cell[4]};
  } else if (f == FaceDir::YF) {
    return {cell[6], cell[2], cell[3], cell[7]};
  } else if (f == FaceDir::YB) {
    return {cell[1], cell[5], cell[4], cell[0]};
  } else if (f == FaceDir::ZF) {
    return {cell[5], cell[6], cell[7], cell[4]};
  } else if (f == FaceDir::ZB) {
    return {cell[2], cell[1], cell[0], cell[3]};
  } else {
    // static_assert(false);
    return {-1, -1, -1, -1};
  }
}

pair<array<Byte, 8>, array<Byte, 6>>
ZrollCW(const pair<array<Byte, 8>, array<Byte, 6>> &ori) {
  auto [ori_v, ori_c] = ori;
  return {{ori_v[1], ori_v[2], ori_v[3], ori_v[0], ori_v[5], ori_v[6], ori_v[7],
           ori_v[4]},
          {ori_c[2], ori_c[3], ori_c[1], ori_c[0], ori_c[4], ori_c[5]}};
}
pair<array<Byte, 8>, array<Byte, 6>>
ZrollCCW(const pair<array<Byte, 8>, array<Byte, 6>> &ori) {
  auto [ori_v, ori_c] = ori;
  return {{ori_v[3], ori_v[0], ori_v[1], ori_v[2], ori_v[7], ori_v[4], ori_v[5],
           ori_v[6]},
          {ori_c[3], ori_c[2], ori_c[0], ori_c[1], ori_c[4], ori_c[5]}};
}
pair<array<Byte, 8>, array<Byte, 6>>
YrollCW(const pair<array<Byte, 8>, array<Byte, 6>> &ori) {
  auto [ori_v, ori_c] = ori;
  return {{ori_v[4], ori_v[0], ori_v[3], ori_v[7], ori_v[5], ori_v[1], ori_v[2],
           ori_v[6]},
          {ori_c[5], ori_c[4], ori_c[2], ori_c[3], ori_c[0], ori_c[1]}};
}
pair<array<Byte, 8>, array<Byte, 6>>
YrollCCW(const pair<array<Byte, 8>, array<Byte, 6>> &ori) {
  auto [ori_v, ori_c] = ori;
  return {{ori_v[1], ori_v[5], ori_v[6], ori_v[2], ori_v[0], ori_v[4], ori_v[7],
           ori_v[3]},
          {ori_c[4], ori_c[5], ori_c[2], ori_c[3], ori_c[1], ori_c[0]}};
}
pair<array<Byte, 8>, array<Byte, 6>>
XrollCW(const pair<array<Byte, 8>, array<Byte, 6>> &ori) {
  auto [ori_v, ori_c] = ori;
  return {{ori_v[3], ori_v[2], ori_v[6], ori_v[7], ori_v[0], ori_v[1], ori_v[5],
           ori_v[4]},
          {ori_c[0], ori_c[1], ori_c[4], ori_c[5], ori_c[3], ori_c[2]}};
}
pair<array<Byte, 8>, array<Byte, 6>>
XrollCCW(const pair<array<Byte, 8>, array<Byte, 6>> &ori) {
  auto [ori_v, ori_c] = ori;
  return {{ori_v[1], ori_v[2], ori_v[3], ori_v[0], ori_v[5], ori_v[6], ori_v[7],
           ori_v[4]},
          {ori_c[0], ori_c[1], ori_c[5], ori_c[4], ori_c[2], ori_c[3]}};
}

pair<array<Byte, 8>, array<Byte, 6>>
RePosCell(pair<array<Byte, 8>, array<Byte, 6>> &&ori, FaceDir xf, FaceDir yf) {
  if (xf == FaceDir::XF) {
    if (yf == FaceDir::YF) {
      return ori;
    } else if (yf == FaceDir::ZF) {
      return XrollCW(ori);
    } else if (yf == FaceDir::YB) {
      return XrollCCW(XrollCCW(ori));
    } else if (yf == FaceDir::ZB) {
      return XrollCCW(ori);
    } else {
      assert(false);
      return {{-1, -1, -1, -1, -1, -1, -1, -1}, {-1, -1, -1, -1, -1, -1}};
    }
  } else if (xf == FaceDir::XB) {
    if (yf == FaceDir::YF) {
      return YrollCCW(YrollCCW(ori));
    } else if (yf == FaceDir::ZB) {
      return YrollCCW(YrollCCW(XrollCCW(ori)));
    } else if (yf == FaceDir::YB) {
      return ZrollCCW(ZrollCCW(ori));
    } else if (yf == FaceDir::ZF) {
      return YrollCCW(YrollCCW(XrollCW(ori)));
    } else {
      assert(false);
      return {{-1, -1, -1, -1, -1, -1, -1, -1}, {-1, -1, -1, -1, -1, -1}};
    }
  } else if (xf == FaceDir::YF) {
    if (yf == FaceDir::XF) {
      return YrollCCW(YrollCCW(ZrollCCW(ori)));
    } else if (yf == FaceDir::ZB) {
      return YrollCCW(XrollCW(ori));
    } else if (yf == FaceDir::XB) {
      return ZrollCW(ori);
    } else if (yf == FaceDir::ZF) {
      return YrollCW(XrollCW(ori));
    } else {
      assert(false);
      return {{-1, -1, -1, -1, -1, -1, -1, -1}, {-1, -1, -1, -1, -1, -1}};
    }
  } else if (xf == FaceDir::YB) {
    if (yf == FaceDir::XF) {
      return ZrollCCW(ori);
    } else if (yf == FaceDir::ZF) {
      return YrollCCW(XrollCW(ori));
    } else if (yf == FaceDir::XB) {
      return YrollCCW(YrollCCW(ZrollCCW(ori)));
    } else if (yf == FaceDir::ZB) {
      return YrollCW(XrollCW(ori));
    } else {
      assert(false);
      return {{-1, -1, -1, -1, -1, -1, -1, -1}, {-1, -1, -1, -1, -1, -1}};
    }
  } else if (xf == FaceDir::ZF) {
    if (yf == FaceDir::XF) {
      return YrollCCW(ZrollCCW(ori));
    } else if (yf == FaceDir::YF) {
      return YrollCCW(ori);
    } else if (yf == FaceDir::XB) {
      return YrollCCW(ZrollCW(ori));
    } else if (yf == FaceDir::YB) {
      return YrollCCW(ZrollCW(ZrollCW(ori)));
    } else {
      assert(false);
      return {{-1, -1, -1, -1, -1, -1, -1, -1}, {-1, -1, -1, -1, -1, -1}};
    }
  } else if (xf == FaceDir::ZB) {
    if (yf == FaceDir::XF) {
      return YrollCW(ZrollCCW(ori));
    } else if (yf == FaceDir::YB) {
      return YrollCW(ZrollCCW(ZrollCCW(ori)));
    } else if (yf == FaceDir::XB) {
      return YrollCW(ZrollCW(ori));
    } else if (yf == FaceDir::YF) {
      return YrollCW(ori);
    } else {
      assert(false);
      return {{-1, -1, -1, -1, -1, -1, -1, -1}, {-1, -1, -1, -1, -1, -1}};
    }
  } else {
    assert(false);
    return {{-1, -1, -1, -1, -1, -1, -1, -1}, {-1, -1, -1, -1, -1, -1}};
  }
}

FaceDir NbhCDir(array<Byte, 6> cell, Byte nbh) {
  for (int i = 0; i < 6; ++i) {
    if (cell[i] == nbh) {
      return static_cast<FaceDir>(i);
    }
  }
  return static_cast<FaceDir>(6);
}

FaceDir OppositeFace(FaceDir f) {
  FaceDir res;
  res = opposite_face[static_cast<unsigned int>(f)];
  return res;
}

array<FaceDir, 4> AdjFace(FaceDir f) {
  auto _f = static_cast<unsigned int>(f);
  array<FaceDir, 4> res{adj_face[_f * 4 + 0], adj_face[_f * 4 + 1],
                        adj_face[_f * 4 + 2], adj_face[_f * 4 + 3]};
}

template <size_t N>
pair<bool, array<Byte, N>> IntersectEdge(array<Byte, 4> f1, array<Byte, 4> f2) {
  array<Byte, N> res;
  int res_size = 0;
  for (int i = 0; i < 4; ++i) {
    int tar1 = f2[i];
    for (int j = 0; j < 4; ++j) {
      if (f1[j] == tar1) {
        if (res_size >= N) {
          return {false, array<Byte, N>};
        }
        res[res_size++] = tar1;
        break;
      }
    }
  }
  if (ressize != N) {
    return {false, array<Byte, N>()};
  }
  return {true, res};
}

// c1 and c2 are neighbors, get corresponding pos in c2 of "pos" in c1
FaceDir NbhCPosDir(Byte c1, array<Byte, 6> c1_c, array<Byte, 8> c1_v, Byte c2,
                   array<Byte, 6> c2_c, array<Byte, 8> c2_v, FaceDir pos) {
  FaceDir c2inc1 = NbhCDir(c1_c, c2);
  FaceDir c1inc2 = NbhCDir(c2_c, c1);
  for (int i = 0; i < 4; ++i) {
    if (IntersectEdge<2>(FaceVertices(c2_v, AdjFace(c1inc2)[i]),
                         FaceVertices(c1_v, pos))
            .first) {
      return AdjFace(c1inc2)[i];
    }
  }
  assert(false);
  return static_cast<FaceDir>(6);
}

bool ReWriteSurface(Byte st, Byte npos, vector<Byte> &cur_nbh_f,
                    vector<Byte> &cur_nbh_v, vector<Byte> &mapping_f,
                    vector<Byte> &mapping_v, const vector<Byte> &m_nbh_f,
                    const vector<Byte> &m_nbh_v, const vector<Byte> &best_nbh_f,
                    const vector<Byte> &best_nbh_v) {
  cur_nbh_f.clear();
  cur_nbh_f.reserve(m_nbh_f.size());
  cur_nbh_v.clear();
  cur_nbh_v.reserve(m_nbh_v.size());
  queue<pair<Byte, Byte>> bfsqueue;
  bfsqueue.push({st, npos});
  int fnum = 0, vnum = 0;
  mapping_f[st] = fnum;
  while (!bfsqueue.empty()) {
    auto [cur_f, cur_nf_pos] = bfsqueue.front();
    bfsqueue.pop();
    for (int i = 0; i < 4; ++i) {
      auto _next_f = m_nbh_f[cur_f * 4 + (cur_nf_pos + i) % 4];
      auto _nbh_v = m_nbh_v[cur_f * 4 + (cur_nf_pos + i) % 4];
      if (mapping_f[_next_f] == -1) {
        mapping_f[_next_f] = fnum++;
        int n2opos =
            FindElementInLine(m_nbh_f.begin() + _next_f * 4,
                              m_nbh_f.begin() + _next_f * 4 + 4, cur_f) -
            m_nbh_f.begin() + _next_f * 4;
        bfsqueue.push({_next_f, n2opos});
      }
      if (mapping_v[_nbh_v] == -1) {
        mapping_v[_nbh_v] = vnum++;
      }
      cur_nbh_f.push_back(mapping_f[_next_f]);
      cur_nbh_v.push_back(mapping_v[_nbh_v]);
    }
    if (cur_nbh_f >= best_nbh_f) {
      return false;
    }
  }
  return true;
}

bool ReWriteModu(Byte st, FaceDir xf_dir, FaceDir yf_dir,
                 vector<Byte> &cur_nbh_v, vector<Byte> &cur_nbh_c,
                 const vector<Byte> &m_nbh_v, const vector<Byte> &m_nbh_c,
                 const vector<Byte> &best_nbh_v,
                 const vector<Byte> &best_nbh_c) {
  cur_nbh_c.clear();
  cur_nbh_v.clear();
  cur_nbh_c.reserve(m_nbh_c.size());
  cur_nbh_v.reserve(m_nbh_v.size());
  vector<Byte> mapping_v_o2n(*max_element(m_nbh_v.begin(), m_nbh_v.end()) + 1);
  vector<Byte> mapping_c_o2n(*max_element(m_nbh_c.begin(), m_nbh_c.end()) + 1);
  queue<tuple<Byte, FaceDir, FaceDir>> bfsqueue;
  bfsqueue.push({st, xf_dir, yf_dir});
  int cnum = 0, vnum = 0;
  mapping_c_o2n[st] = cnum++;
  while (!bfsqueue.empty()) {
    auto [cur_c, cur_xf_d, cur_yf_d] = bfsqueue.front();
    bfsqueue.pop();
    array<Byte, 8> cur_c_nbhv = GetPartInLine<vector<Byte>, 8>(m_nbh_v, cur_c);
    array<Byte, 6> cur_c_nbhc = GetPartInLine<vector<Byte>, 6>(m_nbh_c, cur_c);
    auto [ndir_v, ndir_c] =
        RePosCell({cur_c_nbhv, cur_c_nbhc}, cur_xf_d, cur_yf_d);
    for (int i = 0; i < 8; ++i) {
      if (mapping_v_o2n[ndir_v[i]] == -1) {
        mapping_v_o2n[ndir_v[i]] = vnum++;
      }
      cur_nbh_v.push_back(mapping_v_o2n[ndir_v[i]]);
    }
    FaceDir xfpos[6] = {FaceDir::YF, FaceDir::ZB, FaceDir::ZF,
                        FaceDir::XB, FaceDir::XF, FaceDir::YB};
    FaceDir yfpos[6] = {FaceDir::ZF, FaceDir::YB, FaceDir::XF,
                        FaceDir::ZB, FaceDir::YF, FaceDir::XB};
    for (int i = 0; i < 6; ++i) {
      auto next_c = ndir_c[i];
      if (mapping_c_o2n[next_c] == -1) {
        mapping_c_o2n[next_c] = cnum++;
        auto next_c_nbhv = GetPartInLine<vector<Byte>, 8>(m_nbh_v, next_c);
        auto next_c_nbhc = GetPartInLine<vector<Byte>, 6>(m_nbh_c, next_c);
        bfsqueue.push({next_c,
                       NbhCPosDir(cur_c, cur_c_nbhc, cur_c_nbhv, next_c,
                                  next_c_nbhc, next_c_nbhv, xfpos[i]),
                       NbhCPosDir(cur_c, cur_c_nbhc, cur_c_nbhv, next_c,
                                  next_c_nbhc, next_c_nbhv, yfpos[i])});
      }
      cur_nbh_c.push_back(mapping_c_o2n[next_c]);
    }
    // 这里的逻辑需要再思考
    if (cur_nbh_c > best_nbh_c) {
      return false;
    }
  }

  return true;
}
}; // namespace

void ModuSurface::Regular() {
  vector<Byte> cur_nbh_f(m_nbh_f);
  vector<Byte> cur_nbh_v(m_nbh_v);
  vector<Byte> best_nbh_f(m_nbh_f);
  vector<Byte> best_nbh_v(m_nbh_v);
  vector<Byte> cur_mapping_f(m_size_f);
  vector<Byte> cur_mapping_v(m_size_v);
  vector<Byte> best_mapping_f(m_size_f);
  vector<Byte> best_mapping_v(m_size_v);
  for (int i = 0; i < m_size_f; ++i) {
    best_mapping_f[i] = i;
  }
  for (int i = 0; i < m_size_v; ++i) {
    best_mapping_v[i] = i;
  }
  for (size_t i = 0; i < m_size_f; ++i) {
    for (size_t pos = 0; pos < 4; ++pos) {
      cur_mapping_f.assign(m_size_f, -1);
      cur_mapping_v.assign(m_size_v, -1);
      if (ReWriteSurface(i, pos, cur_nbh_f, cur_nbh_v, cur_mapping_f,
                         cur_mapping_v, m_nbh_f, m_nbh_v, best_nbh_f,
                         best_nbh_v)) {
        best_nbh_f.swap(cur_nbh_f);
        best_nbh_v.swap(cur_nbh_v);
        best_mapping_f.swap(cur_mapping_f);
        best_mapping_v.swap(cur_mapping_v);
      }
    }
  }
  m_nbh_f.swap(best_nbh_f);
  m_nbh_v.swap(best_nbh_v);
  for (size_t i = 0; i < m_mapping_f.size(); ++i) {
    if (m_mapping_f[i] != -1) {
      m_mapping_f[i] = best_mapping_f[m_mapping_f[i]];
    }
  }
  for (size_t i = 0; i < m_mapping_v.size(); ++i) {
    if (m_mapping_v[i] != -1) {
      m_mapping_v[i] = best_mapping_v[m_mapping_v[i]];
    }
  }
  return;
}

void HexModu::Regular() {
  vector<Byte> cur_nbh_v(m_nbh_v);
  vector<Byte> cur_nbh_c(m_nbh_c);
  vector<Byte> best_nbh_v(m_nbh_v);
  vector<Byte> best_nbh_c(m_nbh_c);
  for (size_t i = 0; i < m_size; ++i) {
    for (size_t posf = 0; posf < 6; ++posf) {
      for (size_t posd = 0; posd < 4; ++posd) {
        if (ReWriteModu(i, FaceDir(posf), AdjFace(FaceDir(posf))[posd],
                        cur_nbh_v, cur_nbh_c, m_nbh_v, m_nbh_c, best_nbh_v,
                        best_nbh_c)) {
          best_nbh_v.swap(cur_nbh_v);
          best_nbh_c.swap(cur_nbh_c);
        }
      }
    }
  }
  m_nbh_v.swap(best_nbh_v);
  m_nbh_c.swap(best_nbh_c);
}

tuple<Byte, Byte, Byte> HexModu::GetAdjSurface_(Byte cur_c, Byte cur_sf_dir,
                                                Byte next_sf_dir) {
  auto _cur_c = cur_c;
  auto _cur_sf_dir = FaceDir(cur_sf_dir), _next_sf_dir = FaceDir(next_sf_dir);
  auto next_c = m_nbh_c[cur_c * 6 + next_sf_dir];
  while (next_c != -1) {
    /*loop until meet surface*/
    auto c1_nbhv = GetPartInLine<vector<Byte>, 8>(m_nbh_v, _cur_c);
    auto c1_nbhc = GetPartInLine<vector<Byte>, 6>(m_nbh_c, _cur_c);
    auto c2_nbhv = GetPartInLine<vector<Byte>, 8>(m_nbh_v, next_c);
    auto c2_nbhc = GetPartInLine<vector<Byte>, 6>(m_nbh_c, next_c);
    _next_sf_dir = NbhCPosDir(_cur_c, c1_nbhc, c1_nbhv, next_c, c2_nbhc,
                              c2_nbhv, _cur_sf_dir);
    _cur_sf_dir = NbhCDir(c2_nbhc, _cur_c);
    _cur_c = next_c;
    next_c = m_nbh_c[_cur_c * 6 + next_sf_dir];
  }
  return make_tuple(_cur_c, static_cast<Byte>(_next_sf_dir),
                    static_cast<Byte>(_cur_sf_dir));
}

ModuSurface HexModu::Surface() {
  vector<Byte> surface_f;
  vector<Byte> surface_v;

  vector<Byte> mapping_f_h2s(m_size * 6, -1);
  vector<Byte> mapping_v_h2s;
  auto [st_cell, sf_dir, next_dir] = [&]() -> tuple<Byte, Byte, Byte> {
    Byte st_cell = -1, sf_dir = -1, next_dir = -1;
    for (int i = 0; i < m_size; ++i) {
      for (int j = 0; j < 6; ++j) {
        if (m_nbh_c[i * 6 + j] == -1) {
          st_cell = i, sf_dir = j,
          next_dir = static_cast<Byte>(AdjFace(FaceDir(j))[0]);
          return {st_cell, sf_dir, next_dir};
        }
      }
    }
    assert(false);
    return {-1, -1, -1};
  }();

  // bfs
  int fnum = 0, vnum = 0;
  queue<tuple<Byte, Byte, Byte>> bfsqueue;
  bfsqueue.push({st_cell, sf_dir, next_dir});
  while (!bfsqueue.empty()) {
    auto [cur_c, cur_sf_dir, cur_next_dir] = bfsqueue.front();
    bfsqueue.pop();
    auto dirs = AdjFace(FaceDir(cur_sf_dir));
    auto _tmp = GetPartInLine<decltype(m_nbh_v), 8>(m_nbh_v, cur_c);
    auto face_vertices = FaceVertices(_tmp, FaceDir(cur_sf_dir));
    auto _pos =
        FindElementInLine(dirs.begin(), dirs.end(), FaceDir(cur_next_dir)) -
        dirs.begin();
    for (int i = 0; i < 4; ++i) {
      auto [next_c, next_sf_dir, next_next_dir] = GetAdjSurface_(
          cur_c, cur_sf_dir, static_cast<Byte>(dirs[(_pos + i) % 4]));
      if (mapping_f_h2s[next_c * 6 + next_sf_dir] == -1) {
        mapping_f_h2s[next_c * 6 + next_sf_dir] = fnum++;
        bfsqueue.push({next_c, next_sf_dir, next_next_dir});
      }
      if (mapping_v_h2s[face_vertices[(_pos + i) % 4]] == -1) {
        mapping_v_h2s[face_vertices[(_pos + i) % 4]] = vnum++;
      }
      surface_f.push_back(mapping_f_h2s[next_c * 6 + next_sf_dir]);
      surface_v.push_back(mapping_v_h2s[face_vertices[(_pos + i) % 4]]);
    }
  }

  ModuSurface res;
  res.m_nbh_f.swap(surface_f);
  res.m_nbh_v.swap(surface_v);
  res.m_mapping_f.swap(mapping_f_h2s);
  res.m_mapping_v.swap(mapping_v_h2s);
  res.m_size_f = res.m_nbh_f.size() / 4;
  res.m_size_v = (*max_element(res.m_nbh_v.begin(), res.m_nbh_v.end())) + 1;
  res.Regular();
  return res;
}

vector<pair<SFCase, vector<Byte>>> HexModu::EnumerateAllSFcase() {
  ModuSurface sf = Surface();
  map<vector<Byte>, pair<SFCase, vector<Byte>>> recording;
  unordered_set<pair<Byte, Byte>> diagonals;
  // calculate all diagonals
  auto SFCaseAt = [&](Byte c, Byte pos_in_c) {
    // All surface case at surface face m_nbh_c[c*6+pos_in_c],
    // m_nbh_c[c*6+pos_in_c] is center

    // inquire whether 4 corners connected
    Byte sf_center = sf.m_mapping_f[c * 6 + pos_in_c];
    array<Byte, 4> local_nbh_f = {0, 1, 2, 3};
    for_each(local_nbh_f.begin(), local_nbh_f.end(),
             [&](decltype(local_nbh_f)::value_type &i) {
               i = sf.m_nbh_f[sf_center * 4 + i];
             });
    array<bool, 4> corn_conn = {true, true, true, true};
    for (int i = 0; i < 4; ++i) {
      int pos = FindElementInLine(sf.m_nbh_f.begin() + local_nbh_f[i] * 4,
                                  sf.m_nbh_f.begin() + local_nbh_f[i] * 4 + 4,
                                  sf_center) -
                sf.m_nbh_f.begin() + local_nbh_f[i] * 4;
      if (sf.m_nbh_f[local_nbh_f[i] * 4 + (pos + 3) % 4] !=
          local_nbh_f[(i + 1) % 4]) {
        corn_conn[i] = false;
      }
    }

    // for convenience
    auto _findpos = [&](Byte arg1, Byte arg2) {
      return FindElementInLine(sf.m_nbh_f.begin() + arg1 * 4,
                               sf.m_nbh_f.begin() + arg1 * 4 + 4, arg2) -
             sf.m_nbh_f.begin() + arg1 * 4;
    };
    auto _findvertex = [&](Byte f, Byte anchor, Byte pos) {
      return sf.m_nbh_f[f * 4 + (anchor + pos) % 4];
    };
    auto _diagpair = [&](Byte p1, Byte p2) {
      return make_pair(min(p1, p2), max(p1, p2));
    };

    vector<pair<SFCase, vector<Byte>>> res;
    // logic here is very confusing,
    // TODO:: explain logic in a markdown file
    auto checkSFCase = [&](int iter_time, SFCase sfcase,
                           const vector<Byte> &corn,
                           const vector<bool> &corn_val, vector<Byte> faces,
                           const vector<vector<Byte>> &diag_tests) {
      // add avaliable SurFace Case at sf_center to "res"
      vector<Byte> local_nbh_f_copy = {sf_center};
      local_nbh_f_copy.insert(local_nbh_f_copy.end(), local_nbh_f.begin(),
                              local_nbh_f.end());

      for (int i = 0; i < iter_time; ++i) {
        bool pass = true;
        for (int j = 0; j < corn_val.size(); ++j) {
          if (corn_conn[(i + corn[j]) % 4] == corn_val[j]) {
            pass = false;
            break;
          }
        }
        if (!pass) {
          continue;
        }
        vector<Byte> pos;
        pos.push_back(_findpos(sf_center, local_nbh_f[(i + faces[0]) % 4]));
        for (int j = 0; j < faces.size(); ++j) {
          pos.push_back(_findpos(local_nbh_f[(i + faces[j]) % 4], sf_center));
        }
        vector<pair<Byte, Byte>> _diagpairs;
        for (auto x : diag_tests) {
          auto _f0 =
              x[0] == 0 ? sf_center : local_nbh_f[(i + faces[x[0] - 1]) % 4];
          auto _f1 =
              x[2] == 0 ? sf_center : local_nbh_f[(i + faces[x[2] - 1]) % 4];
          _diagpairs.push_back(_diagpair(_findvertex(_f0, pos[x[0]], x[1]),
                                         _findvertex(_f1, pos[x[2]], x[3])));
        }
        for (auto x : _diagpairs) {
          if (diagonals.find(x) != diagonals.end()) {
            pass = false;
            break;
          }
        }
        if (!pass) {
          continue;
        }
        res.push_back({sfcase, faces});
      }
    };

    // F1
    res.push_back({SFCase::F1, {sf_center}});
    // F2
    checkSFCase(4, SFCase::F2, {0, 3}, {true, true}, {0},
                {{0, 3, 1, 3}, {0, 2, 1, 2}});
    // preserve for check
    /*
    for (int i = 0; i < 4; ++i) {
      if (corn_conn[i] || corn_conn[(i + 3) % 4]) {
        continue;
      }
      auto pos1 = _findpos(sf_center, local_nbh_f[i]);
      auto pos2 = _findpos(local_nbh_f[i], sf_center);
      pair test1 = _diagpair(_findvertex(sf_center, pos1, 3),
                             _findvertex(local_nbh_f[i], pos2, 3));
      pair test2 = _diagpair(_findvertex(sf_center, pos1, 2),
                             _findvertex(local_nbh_f[i], pos2, 2));
      if (diagonals.find(test1) != diagonals.end() ||
          diagonals.find(test2) != diagonals.end()) {
        continue;
      }
      res.push_back({SFCase::F2, {sf_center, local_nbh_f[i]}});
    }
    */
    // F3_1
    checkSFCase(2, SFCase::F3_1, {0, 1, 2, 3}, {true, true, true, true}, {0, 2},
                {{0, 3, 1, 3},
                 {0, 2, 1, 2},
                 {0, 0, 2, 2},
                 {0, 1, 2, 3},
                 {1, 2, 2, 2},
                 {1, 3, 2, 3}});
    /*
    for (int i = 0; i < 2; ++i) {
      if (corn_conn[i] || corn_conn[(i + 1) % 4] || corn_conn[(i + 2) % 4] ||
          corn_conn[(i + 3) % 4]) {
        continue;
      }
      auto pos1 = _findpos(sf_center, local_nbh_f[i]);
      auto pos2 = _findpos(local_nbh_f[i], sf_center);
      auto pos3 = _findpos(local_nbh_f[(i + 2) % 4], sf_center);
      pair test1 = _diagpair(_findvertex(sf_center, pos1, 3),
                             _findvertex(local_nbh_f[i], pos2, 3));
      pair test2 = _diagpair(_findvertex(sf_center, pos1, 2),
                             _findvertex(local_nbh_f[i], pos2, 2));
      pair test3 = _diagpair(_findvertex(sf_center, pos1, 0),
                             _findvertex(local_nbh_f[(i + 2) % 4], pos3, 2));
      pair test4 = _diagpair(_findvertex(sf_center, pos1, 1),
                             _findvertex(local_nbh_f[(i + 2) % 4], pos3, 3));
      pair test5 = _diagpair(_findvertex(local_nbh_f[i], pos2, 2),
                             _findvertex(local_nbh_f[(i + 2) % 4], pos3, 2));
      pair test6 = _diagpair(_findvertex(local_nbh_f[i], pos2, 3),
                             _findvertex(local_nbh_f[(i + 2) % 4], pos3, 3));
      if (diagonals.find(test1) != diagonals.end() ||
          diagonals.find(test2) != diagonals.end() ||
          diagonals.find(test3) != diagonals.end() ||
          diagonals.find(test4) != diagonals.end() ||
          diagonals.find(test5) != diagonals.end() ||
          diagonals.find(test6) != diagonals.end()) {
        continue;
      }
      res.push_back({SFCase::F3_1,
                     {sf_center, local_nbh_f[i], local_nbh_f[(i + 2) % 4]}});
    }
    */
    // F3_2
    checkSFCase(4, SFCase::F3_2, {0, 1, 3}, {false, true, true}, {0, 1},
                {{0, 3, 1, 3}, {0, 2, 1, 2}, {0, 0, 2, 3}, {1, 2, 2, 3}});
    // F4
    checkSFCase(4, SFCase::F4, {0, 1, 2, 3}, {false, false, true, true},
                {0, 1, 2},
                {{0, 3, 1, 3},
                 {0, 2, 1, 2},
                 {0, 0, 3, 2},
                 {0, 1, 3, 3},
                 {1, 3, 3, 3},
                 {1, 2, 3, 2}});
    // F5
    checkSFCase(1, SFCase::F5, {0, 1, 2, 3}, {false, false, false, false},
                {0, 1, 2, 3},
                {{0, 3, 1, 3},
                 {0, 2, 1, 2},
                 {0, 0, 3, 2},
                 {0, 1, 3, 3},
                 {1, 3, 3, 3},
                 {1, 2, 3, 2}});
    return res;
  };
  for (int i = 0; i < m_size; ++i) {
    for (int j = 0; j < 6; ++i) {
      if (m_nbh_c[i * 6 + j] == -1) {
        auto sfcases = SFCaseAt(i, j);
        for (auto x : sfcases) {
          auto _tmp = x.second;
          sort(_tmp.begin(), _tmp.end());
          if (recording.find(_tmp) == recording.end()) {
            recording[_tmp] = x;
          }
        }
      }
    }
  }
  vector<pair<SFCase, vector<Byte>>> res;
  for (auto &x : recording) {
    res.push_back(std::move(x.second));
  }
  return res;
}

HexModu HexModu::AddHexAt(const ModuSurface &surface,
                          const pair<SFCase, vector<Byte>> &sfc) {
  HexModu res(*this);
  int vnum = surface.m_mapping_v.size();
  res.m_size += 1;
  Byte ncell = res.m_size - 1;
  res.m_nbh_c.insert(m_nbh_c.end(), {-1, -1, -1, -1, -1, -1});
  res.m_nbh_v.insert(m_nbh_v.end(), {-1, -1, -1, -1, -1, -1, -1, -1});
  res.m_cell_sheet.insert(m_cell_sheet.end(), {-1, -1, -1, -1, -1, -1});

  auto _it = FindElementInLine(surface.m_mapping_f.begin(),
                               surface.m_mapping_f.end(), sfc.second[0]);
  Byte base_cell_num = (_it - surface.m_mapping_f.begin()) / 6;
  Byte base_cell_pos = (_it - surface.m_mapping_f.begin()) % 6;
  res.m_nbh_c[base_cell_num * 6 + base_cell_pos] = ncell;
  res.m_nbh_c[ncell * 6 + 5] = base_cell_num;
  auto _tmpface =
      FaceVertices(GetPartInLine<decltype(m_nbh_v), 8>(m_nbh_v, base_cell_num),
                   FaceDir(base_cell_pos));
  res.m_nbh_v[ncell * 8 + 0] = _tmpface[3];
  res.m_nbh_v[ncell * 8 + 1] = _tmpface[0];
  res.m_nbh_v[ncell * 8 + 2] = _tmpface[1];
  res.m_nbh_v[ncell * 8 + 3] = _tmpface[2];

  FaceDir xfpos[6] = {FaceDir::YF, FaceDir::ZB, FaceDir::ZF,
                      FaceDir::XB, FaceDir::XF, FaceDir::YB};
  FaceDir yfpos[6] = {FaceDir::ZF, FaceDir::YB, FaceDir::XF,
                      FaceDir::ZB, FaceDir::YF, FaceDir::XB};

  auto adj_fs = AdjFace(FaceDir(base_cell_pos));
  for (int i = 0; i < 4; ++i) {
    auto [next_c, next_sf_pos, next_next_pos] = GetAdjSurface_(
        base_cell_num, base_cell_pos, static_cast<Byte>(adj_fs[i]));
    if (FindElementInLine(sfc.second.begin(), sfc.second.end(),
                          surface.m_mapping_f[next_c * 6 + next_sf_pos]) !=
        sfc.second.end()) {
      assert(res.m_nbh_c[next_c * 6 + next_sf_pos] == -1);
      res.m_nbh_c[next_c * 6 + next_sf_pos] = ncell;
      res.m_nbh_c[ncell * 6 + i] = next_c;

      auto sf2 = surface.m_mapping_f[next_c * 6 + next_next_pos];
      auto sf1 = surface.m_mapping_f[base_cell_num * 6 + base_cell_pos];

      auto _pos =
          FindElementInLine(surface.m_mapping_f.begin() + sf2 * 4,
                            surface.m_mapping_f.begin() + sf2 * 4 + 4, sf1) -
          surface.m_mapping_f.begin() + sf2 * 4;
      res.m_nbh_v[ncell * 8 + 4 + i] =
          FindElementInLine(surface.m_mapping_v.begin(),
                            surface.m_mapping_v.end(),
                            surface.m_mapping_v[sf2 * 4 + 2]) -
          surface.m_mapping_v.begin();
      res.m_nbh_v[ncell * 8 + 4 + (i + 1) % 4] =
          FindElementInLine(surface.m_mapping_v.begin(),
                            surface.m_mapping_v.end(),
                            surface.m_mapping_v[sf2 * 4 + 3]) -
          surface.m_mapping_v.begin();
    }
    for (int i = 0; i < 4; ++i) {
      if (res.m_nbh_v[ncell * 8 + 4 + i] == -1) {
        res.m_nbh_v[ncell * 8 + 4 + i] = vnum++;
      }
    }

    // sheet information update
    for (auto d :
         AdjFace(NbhCDir(GetPartInLine<decltype(m_nbh_c), 6>(m_nbh_c, ncell),
                         sfc.second[0]))) {
      auto bc = sfc.second[0];
      auto d_in_base =
          NbhCPosDir(ncell, GetPartInLine<decltype(m_nbh_c), 6>(m_nbh_c, ncell),
                     GetPartInLine<decltype(m_nbh_v), 8>(m_nbh_v, ncell), bc,
                     GetPartInLine<decltype(m_nbh_c), 6>(m_nbh_c, bc),
                     GetPartInLine<decltype(m_nbh_v), 8>(m_nbh_v, bc), d);
      res.m_cell_sheet[ncell * 6 + static_cast<Byte>(d)] =
          m_cell_sheet[bc * 6 + static_cast<Byte>(d_in_base)];
    }

    // possibly new sheet and sheet merge
    Byte nsheet_num = m_sheet.size();
    vector<Byte> sheet_tobe_merged;
    for (auto d :
         AdjFace(NbhCDir(GetPartInLine<decltype(m_nbh_c), 6>(m_nbh_c, ncell),
                         sfc.second[0]))) {
      if (auto _nbhc = res.m_nbh_c[ncell * 6 + static_cast<Byte>(d)];
          _nbhc != -1) {
        auto d_in_nbhc = NbhCPosDir(
            ncell, GetPartInLine<decltype(m_nbh_c), 6>(m_nbh_c, ncell),
            GetPartInLine<decltype(m_nbh_v), 8>(m_nbh_v, ncell), _nbhc,
            GetPartInLine<decltype(m_nbh_c), 6>(m_nbh_c, _nbhc),
            GetPartInLine<decltype(m_nbh_v), 8>(m_nbh_v, _nbhc), FaceDir(4));
        Byte sheet = m_cell_sheet[_nbhc * 6 + static_cast<Byte>(d_in_nbhc)];
        if (sheet > nsheet_num) {
          if (FindElementInLine(sheet_tobe_merged.begin(),
                                sheet_tobe_merged.end(),
                                sheet) == sheet_tobe_merged.end()) {
            sheet_tobe_merged.push_back(sheet);
          }
        } else {
          nsheet_num = sheet;
        }
      }
    }
    if (nsheet_num == m_sheet.size()) {
      res.m_sheet.push_back({0, 0});
    }
    res.m_cell_sheet[ncell * 6 + 4] = nsheet_num;
    res.m_cell_sheet[ncell * 6 + 5] = nsheet_num;
    for (auto &x : res.m_cell_sheet) {
      if (FindElementInLine(sheet_tobe_merged.begin(), sheet_tobe_merged.end(),
                            x) != sheet_tobe_merged.end()) {
        x = nsheet_num;
      }
    }

    // delete merges sheets
    for (int i = 0; i < sheet_tobe_merged.size(); ++i) {
      int sheet_m1 = sheet_tobe_merged[i];
      int sheet = m_sheet.size() - 1 - i;
      for (auto &x : m_cell_sheet) {
        if (x == sheet)
          x = sheet_m1;
      }
      swap(m_sheet[sheet_m1], m_sheet[sheet]);
    }
    for (int i = 0; i < sheet_tobe_merged.size(); ++i) {
      m_sheet.pop_back();
    }

    // TODO: update sheet singularty
    }

  res.Regular();
  return res;
}