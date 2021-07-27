#include "hexmodu.hpp"
#include <algorithm>
#include <cassert>
#include <queue>

namespace {
using namespace std;

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

array<Byte, 4> FaceVertices(FaceDir f, array<Byte, 8> cell) {
  if (f == FaceDir::XF) {
    return {cell[1], cell[2], cell[5], cell[6]};
  } else if (f == FaceDir::XB) {
    return {cell[0], cell[3], cell[4], cell[7]};
  } else if (f == FaceDir::YF) {
    return {cell[2], cell[3], cell[6], cell[7]};
  } else if (f == FaceDir::YB) {
    return {cell[0], cell[1], cell[4], cell[5]};
  } else if (f == FaceDir::ZF) {
    return {cell[4], cell[5], cell[6], cell[7]};
  } else if (f == FaceDir::ZB) {
    return {cell[0], cell[1], cell[2], cell[3]};
  } else {
    // static_assert(false);
    return {-1, -1, -1, -1};
  }
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

FaceDir NbhCPosDir(Byte c1, array<Byte, 6> c1_c, array<Byte, 8> c1_v, Byte c2,
                   array<Byte, 6> c2_c, array<Byte, 8> c2_v, FaceDir pos) {
  FaceDir c2inc1 = NbhCDir(c1_c, c2);
  FaceDir c1inc2 = NbhCDir(c2_c, c1);
  for (int i = 0; i < 4; ++i) {
    if (IntersectEdge<2>(FaceVertices(AdjFace(c1inc2)[i], c2_v),
                         FaceVertices(pos, c1_v))
            .first) {
      return AdjFace(c1inc2)[i];
    }
  }
  assert(false);
  return static_cast<FaceDir>(6);
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
    array<Byte, 8> cur_c_nbhv = {
        m_nbh_v[cur_c * 8 + 0], m_nbh_v[cur_c * 8 + 1], m_nbh_v[cur_c * 8 + 2],
        m_nbh_v[cur_c * 8 + 3], m_nbh_v[cur_c * 8 + 4], m_nbh_v[cur_c * 8 + 5],
        m_nbh_v[cur_c * 8 + 6], m_nbh_v[cur_c * 8 + 7]};
    array<Byte, 6> cur_c_nbhc = {
        m_nbh_c[cur_c * 6 + 0], m_nbh_c[cur_c * 6 + 1], m_nbh_c[cur_c * 6 + 2],
        m_nbh_c[cur_c * 6 + 3], m_nbh_c[cur_c * 6 + 4], m_nbh_c[cur_c * 6 + 5]};
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
        array<Byte, 8> next_c_nbhv = {
            m_nbh_v[next_c * 8 + 0], m_nbh_v[next_c * 8 + 1],
            m_nbh_v[next_c * 8 + 2], m_nbh_v[next_c * 8 + 3],
            m_nbh_v[next_c * 8 + 4], m_nbh_v[next_c * 8 + 5],
            m_nbh_v[next_c * 8 + 6], m_nbh_v[next_c * 8 + 7]};
        array<Byte, 6> next_c_nbhc = {
            m_nbh_c[next_c * 6 + 0], m_nbh_c[next_c * 6 + 1],
            m_nbh_c[next_c * 6 + 2], m_nbh_c[next_c * 6 + 3],
            m_nbh_c[next_c * 6 + 4], m_nbh_c[next_c * 6 + 5]};
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

void SurfaceBFS(Byte cur_c, FaceDir cur_cf_dir, FaceDir st_nbhf_dir,
                const vector<Byte> &m_nbh_c, const vector<Byte> &m_nbh_v,
                vector<Byte> &mapping_f_m2s, vector<Byte> &surface_f) {
  int vnum = 0;
  queue<tuple<Byte, FaceDir, FaceDir>> bfsqueue;
  auto getNextdirs = [&](Byte cur_c, FaceDir cur_cf_dir, FaceDir st_nbhf_dir) {
    auto dirs = AdjFace(cur_cf_dir);
    array<FaceDir, 4> res;
    for (int i = 0; i < 4; ++i) {
      if (dirs[i] == st_nbhf_dir) {
        for (int ii = 0; ii < 4; ++ii) {
          res[ii] = dirs[(i + ii) % 4];
        }
        break;
      }
    }
    return res;
  };
  auto getAdjSurface = [&](Byte cur_c, FaceDir cur_cf_dir,
                           FaceDir next_nbh_dir) {
    auto _cur_c = cur_c;
    auto _cur_cf_dir = cur_cf_dir, _next_nbh_dir = next_nbh_dir;
    auto next_c = m_nbh_c[cur_c * 6 + static_cast<unsigned int>(next_nbh_dir)];
    while (next_c != -1) {
      /*loop until meet surface*/
      array<Byte, 8> c1_nbhv = {
          m_nbh_c[_cur_c * 8 + 0], m_nbh_c[_cur_c * 8 + 1],
          m_nbh_c[_cur_c * 8 + 2], m_nbh_c[_cur_c * 8 + 3],
          m_nbh_c[_cur_c * 8 + 4], m_nbh_c[_cur_c * 8 + 5],
          m_nbh_c[_cur_c * 8 + 6], m_nbh_c[_cur_c * 8 + 7]};
      array<Byte, 6> c1_nbhc = {
          m_nbh_v[_cur_c * 6 + 0], m_nbh_v[_cur_c * 6 + 1],
          m_nbh_v[_cur_c * 6 + 2], m_nbh_v[_cur_c * 6 + 3],
          m_nbh_v[_cur_c * 6 + 4], m_nbh_v[_cur_c * 6 + 5]};
      array<Byte, 8> c2_nbhv = {
          m_nbh_c[next_c * 8 + 0], m_nbh_c[next_c * 8 + 1],
          m_nbh_c[next_c * 8 + 2], m_nbh_c[next_c * 8 + 3],
          m_nbh_c[next_c * 8 + 4], m_nbh_c[next_c * 8 + 5],
          m_nbh_c[next_c * 8 + 6], m_nbh_c[next_c * 8 + 7]};
      array<Byte, 6> c2_nbhc = {
          m_nbh_v[next_c * 6 + 0], m_nbh_v[next_c * 6 + 1],
          m_nbh_v[next_c * 6 + 2], m_nbh_v[next_c * 6 + 3],
          m_nbh_v[next_c * 6 + 4], m_nbh_v[next_c * 6 + 5]};
      _next_nbh_dir = NbhCPosDir(_cur_c, c1_nbhc, c1_nbhv, next_c, c2_nbhc,
                                 c2_nbhv, _cur_cf_dir);
      _cur_cf_dir = NbhCDir(c2_nbhc, _cur_c);
      _cur_c = next_c;
      next_c = m_nbh_c[_cur_c * 6 + static_cast<unsigned int>(next_nbh_dir)];
    }
    return make_tuple(_cur_c, _cur_cf_dir, _next_nbh_dir);
  };
  bfsqueue.push({cur_c, cur_cf_dir, st_nbhf_dir});
  mapping_f_m2s[cur_c * 6 + static_cast<unsigned int>(cur_cf_dir)] = vnum++;
  while (!bfsqueue.empty()) {
    auto [_cur_c, _cur_cf_dir, _st_nbhf_dir] = bfsqueue.front();
    bfsqueue.pop();
    auto _d = getNextdirs(cur_c, cur_cf_dir, st_nbhf_dir);
    for (int i = 0; i < 4; ++i) {
      auto [_next_c, _next_cf_dir, _next_st_nbhf_dir] =
          getAdjSurface(cur_c, cur_cf_dir, _d[i]);
      if (mapping_f_m2s[_next_c * 6 +
                        static_cast<unsigned int>(_next_cf_dir)] == -1) {
        mapping_f_m2s[_next_c * 6 + static_cast<unsigned int>(_next_cf_dir)] =
            vnum++;
        bfsqueue.push({_next_c, _next_cf_dir, _next_st_nbhf_dir});
      }
      surface_f.push_back(
          mapping_f_m2s[_next_c * 6 + static_cast<unsigned int>(_next_cf_dir)]);
    }
  }
}
}; // namespace

void HexModu::Regular() {
  vector<Byte> cur_nbh_v(m_nbh_v);
  vector<Byte> cur_nbh_c(m_nbh_c);
  vector<Byte> best_nbh_v(m_nbh_v);
  vector<Byte> best_nbh_c(m_nbh_c);
  for (size_t i = 0; i < m_size; ++i) {
    for (size_t posf = 0; posf < 6; ++posf) {
      for (size_t posd = 0; posd < 4; ++posd) {
        if (ReWriteModu(i, static_cast<FaceDir>(posf),
                        AdjFace(static_cast<FaceDir>(posf))[posd], cur_nbh_v,
                        cur_nbh_c, m_nbh_v, m_nbh_c, best_nbh_v, best_nbh_c)) {
          best_nbh_v.swap(cur_nbh_v);
          best_nbh_c.swap(cur_nbh_c);
        }
      }
    }
  }
  m_nbh_v.swap(best_nbh_v);
  m_nbh_c.swap(best_nbh_c);
}

ModuSurface HexModu::Surface() {
  vector<Byte> surface_f;
  // vector<Byte> surface_v;

  vector<Byte> mapping_f_m2s(m_size * 6, -1);
  // vector<Byte> mapping_v_s2m;
  for (int i = 0; i < m_size; ++i) {
    for (int j = 0; j < 6; ++j) {
      if (m_nbh_c[i * 6 + j] == -1) {
        FaceDir cf = static_cast<FaceDir>(j);
        FaceDir next = AdjFace(cf)[0];
        SurfaceBFS(i, cf, next, m_nbh_c, m_nbh_v, mapping_f_m2s, surface_f);
        break;
      }
    }
  }
  ModuSurface res;
  res.m_nbh_f.swap(surface_f);
  res.m_mapping.swap(mapping_f_m2s);
  res.m_size = res.m_nbh_f.size() / 4;
  res.Regular();
  return res;
}