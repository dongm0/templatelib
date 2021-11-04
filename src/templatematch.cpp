#include "database.hpp"
#include "global_datatype.h"
#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

struct {
  unordered_multimap<StoreSurface, size_t> index;
  vector<StoreModu> data;
} DataStore;

struct meshLocal {
  vector<Byte> m_nbhf;
  vector<Byte> m_nbhv;
  vector<Byte> m_nbhf_valence;
  vector<double> m_nbhf_angle;

  vector<int> v_mapping;
} local;

void readDataBase() {}
pair<bool, vector<StoreModu>> getModuFromSurface(const StoreSurface &sf) {}

HexModu getEntireModu(const StoreModu &storemodu) {}

float calculateMatch(const HexModu &modu, meshLocal &input) {}

int main() { readDataBase(); }