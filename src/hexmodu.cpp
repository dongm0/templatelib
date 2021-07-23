#include "hexmodu.hpp"

namespace {
using namespace std;
bool ReWriteModu(vector<Byte> &cur_nbh_v, vector<Byte> &cur_nbh_c,
                 const vector<Byte> &m_nbh_v, const vector<Byte> &m_nbh_c,
                 const vector<Byte> &best_nbh_v,
                 const vector<Byte> &best_nbh_c) {

  return true;
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
  cur_nbh_v.swap(best_nbh_v);
  cur_nbh_c.swap(best_nbh_c);
}
