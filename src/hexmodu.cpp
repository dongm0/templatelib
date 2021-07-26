#include "hexmodu.hpp"
#include <algorithm>
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
  int cnum = 0;
  while (!bfsqueue.empty()) {
    auto [cur_c, cur_xf_d, cur_yf_d] = bfsqueue.front();
    bfsqueue.pop();
    for (int i = 0; i < 6; ++i) {
      auto next_c = m_nbh_c[cur_c * 6 + i];
      if ()
    }
  }

  return true;
}
array<Byte, 4> FaceVertices(FaceDir f) {
  if (f == FaceDir::XF) {
    return {1, 2, 5, 6};
  } else if (f == FaceDir::XB) {
    return {0, 3, 4, 7};
  } else if (f == FaceDir::YF) {
    return {2, 3, 6, 7};
  } else if (f == FaceDir::YB) {
    return {0, 1, 4, 5};
  } else if (f == FaceDir::ZF) {
    return {4, 5, 6, 7};
  } else if (f == FaceDir::ZB) {
    return {0, 1, 2, 3};
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
      auto [success, common_edge] = IntersectEdge<2>(
          FaceVertices(cur_cf_dir), FaceVertices(next_nbh_dir));
      // Face of cur_c in next_c
      FaceDir n2o_dir = [&](Byte nc, Byte oc) {
        for (int i = 0; i < 6; ++i) {
          if (m_nbh_c[nc * 6 + i] == oc) {
            return static_cast<FaceDir>(i);
          }
        }
        return static_cast<FaceDir>(6);
      }(next_c, _cur_c);
      // Face of next_next_c in next_c
      FaceDir n2nn_dir = [&](Byte nc, FaceDir n2o, array<Byte, 2> edge) {
        array<Byte, 2> edge_in_nc;
        for (int i = 0; i < 8; ++i) {
          if (m_nbh_v[nc * 8 + i] == edge[0]) {
            edge_in_nc[0] = i;
          } else if (m_nbh_v[nc * 8 + i] == edge[1]) {
            edge_in_nc[1] = i;
          }
        }
        auto adjs = AdjFace(n2o);
        for (int i = 0; i < 4; ++i) {
          auto success = [](array<Byte, 4> vs, Byte v1, Byte v2) {
            bool r1 = false, r2 = false;
            for (auto v : vs) {
              if (v == v1) {
                r1 == true;
              }
              if (v == v2) {
                r2 == true;
              }
            }
            return r1 && r2;
          }(FaceVertices(adjs[i]), edge[0], edge[1]);
          if (success) {
            return static_cast<FaceDir>(i);
          }
        }
        return static_cast<FaceDir>(6);
      }(next_c, n2o_dir, common_edge);
      _cur_c = next_c;
      _cur_cf_dir = n2o_dir;
      _next_nbh_dir = n2nn_dir;
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
        if (ReWriteModu(cur_nbh_v, cur_nbh_c, m_nbh_v, m_nbh_c, best_nbh_v,
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