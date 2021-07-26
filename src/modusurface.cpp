#include "modusurface.hpp"
#include "global_datatype.h"
#include <queue>

namespace {
bool ReWriteSurface(Byte st, Byte npos, vector<Byte> &cur_nbh_f,
                    vector<Byte> &mapping, const vector<Byte> &m_nbh_f,
                    const vector<Byte> &best_nbh_f) {
  cur_nbh_f.clear();
  cur_nbh_f.reserve(m_nbh_f.size());
  queue<pair<Byte, Byte>> bfsqueue;
  mapping.assign(m_nbh_f.size() / 4, -1);
  bfsqueue.push({st, npos});
  int fnum = 0;
  mapping[st] = fnum;
  while (!bfsqueue.empty()) {
    auto [cur_f, cur_nf_pos] = bfsqueue.front();
    bfsqueue.pop();
    for (int i = 0; i < 4; ++i) {
      auto _next_f = m_nbh_f[cur_f * 4 + (cur_nf_pos + i) % 4];
      if (mapping[_next_f] == -1) {
        mapping[_next_f] = fnum++;
        int n2opos = -1;
        for (int j = 0; j < 4; ++j) {
          if (m_nbh_f[_next_f * 4 + j] == cur_f) {
            n2opos = j;
            break;
          }
        }
        bfsqueue.push({_next_f, n2opos});
      }
      cur_nbh_f.push_back(mapping[_next_f]);
    }
    if (cur_nbh_f >= best_nbh_f) {
      return false;
    }
  }
  return true;
}
}; // namespace

void ModuSurface::Regular() {
  vector<Byte> cur_nbh_f(m_nbh_f);
  vector<Byte> best_nbh_f(m_nbh_f);
  vector<Byte> cur_mapping(m_size);
  vector<Byte> best_mapping(m_size);
  for (int i = 0; i < m_size; ++i) {
    best_mapping[i] = i;
  }
  for (size_t i = 0; i < m_size; ++i) {
    for (size_t pos = 0; pos < 4; ++pos) {
      if (ReWriteSurface(i, pos, cur_nbh_f, cur_mapping, m_nbh_f, best_nbh_f)) {
        best_nbh_f.swap(cur_nbh_f);
        best_mapping.swap(cur_mapping);
      }
    }
  }
  m_nbh_f.swap(best_nbh_f);
  for (size_t i = 0; i < m_mapping.size(); ++i) {
    if (m_mapping[i] != -1) {
      m_mapping[i] = best_mapping[m_mapping[i]];
    }
  }
  return;
}