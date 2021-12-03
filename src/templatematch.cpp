#include "database.hpp"
#include "global_datatype.h"
#include <algorithm>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

struct {
  unordered_map<StoreSurface, vector<uint64_t>, SurfaceHash> index;
  // map<StoreSurface, vector<uint64_t>> index;
  vector<StoreModu> data;
} DataStore;

struct meshLocal {
  vector<Byte> m_nbhf;
  vector<Byte> m_nbhv;
  vector<Byte> m_nbhf_valence;
  vector<double> m_nbhf_angle;

  vector<int> v_mapping;
} local;

template <typename T> void readBin(std::istream &in, T *data, size_t len = 1) {
  int size = sizeof(T) / sizeof(char);
  in.read((char *)data, size * len);
}

template <typename T>
void writeBin(std::ofstream &out, T *data, size_t len = 1) {
  int size = sizeof(T) / sizeof(char);
  out.write((char *)(data), size * len);
}

void writeDataBin(const std::string &filename) {
  ofstream fout;
  fout.open(filename, ios::binary);
  uint64_t sf_num = DataStore.index.size();
  writeBin(fout, &sf_num);
  for (auto x : DataStore.index) {
    // sf
    Byte f = x.first.nbh_c.size() / 4;
    writeBin(fout, &f);
    writeBin(fout, x.first.nbh_v.data(), x.first.nbh_v.size());
    writeBin(fout, x.first.nbh_c.data(), x.first.nbh_c.size());
    // index
    uint64_t index_size = x.second.size();
    writeBin(fout, &index_size);
    writeBin(fout, x.second.data(), index_size);
  }

  uint64_t modu_num = DataStore.data.size();
  writeBin(fout, &modu_num);
  for (auto x : DataStore.data) {
    Byte c = x.nbh_c.size() / 6;
    writeBin(fout, &c);
    writeBin(fout, x.nbh_c.data(), x.nbh_c.size());
    writeBin(fout, x.nbh_v.data(), x.nbh_v.size());
  }
  fout.close();
}

void readDataBin(const std::string &filename) {
  ifstream fin;
  fin.open(filename, ios::binary);
  uint64_t sf_num = 0;
  readBin(fin, &sf_num);
  for (size_t i = 0; i < sf_num; ++i) {
    // sf
    StoreSurface ssf;
    Byte f = 0;
    readBin(fin, &f);
    ssf.nbh_c.resize((int)f * 4);
    ssf.nbh_v.resize((int)f * 4);
    readBin(fin, ssf.nbh_c.data(), ssf.nbh_c.size());
    readBin(fin, ssf.nbh_v.data(), ssf.nbh_v.size());
    // index
    uint64_t index_size = 0;
    readBin(fin, &index_size);
    vector<uint64_t> index(index_size);
    readBin(fin, index.data(), index_size);
    DataStore.index.insert(make_pair(ssf, index));
  }
  uint64_t modu_num = 0;
  readBin(fin, &modu_num);
  DataStore.data.resize(modu_num);
  for (size_t i = 0; i < modu_num; ++i) {
    StoreModu modu;
    Byte c = 0;
    readBin(fin, &c);
    modu.nbh_v.resize(c * 8);
    modu.nbh_c.resize(c * 6);
    readBin(fin, modu.nbh_c.data(), modu.nbh_c.size());
    readBin(fin, modu.nbh_v.data(), modu.nbh_v.size());
    DataStore.data[i] = modu;
  }
  fin.close();
}

void readDataBase(const std::string &filename) {
  ifstream fin;
  fin.open(filename);
  int tnum;
  fin >> tnum;
  for (int i = 0; i < tnum; ++i) {
    int sf_v, sf_c;
    fin >> sf_c >> sf_v;
    StoreSurface ssf;
    ssf.nbh_c.resize(sf_c * 4);
    ssf.nbh_v.resize(sf_c * 4);
    int tmp;
    for (int j = 0; j < sf_c * 4; ++j) {
      fin >> tmp;
      ssf.nbh_v[j] = tmp;
    }
    for (int j = 0; j < sf_c * 4; ++j) {
      fin >> tmp;
      ssf.nbh_c[j] = tmp;
    }

    int modu_v, modu_c;
    fin >> modu_c >> modu_v;
    StoreModu modu;
    modu.nbh_c.resize(modu_c * 6);
    modu.nbh_v.resize(modu_c * 8);
    for (int j = 0; j < modu_c * 6; ++j) {
      fin >> tmp;
      modu.nbh_c[j] = tmp;
    }
    for (int j = 0; j < modu_c * 8; ++j) {
      fin >> tmp;
      modu.nbh_v[j] = tmp;
    }
    DataStore.data.push_back(modu);
    if (DataStore.index.count(ssf) == 0)
      DataStore.index[ssf];
    DataStore.index[ssf].push_back(i);
  }
  fin.close();
}
// pair<bool, vector<StoreModu>> getModuFromSurface(const StoreSurface &sf) {}

// HexModu getEntireModu(const StoreModu &storemodu) {}

// float calculateMatch(const HexModu &modu, meshLocal &input) {}

void worker_trans_ascii2bin() {
  readDataBase("datastore_1.dat");
  writeDataBin("datastore_bin_withsingle.dat");
}

bool readssf(std::string filename, StoreSurface &ssf) {
  ssf.nbh_c.clear();
  ssf.nbh_v.clear();
  std::ifstream fin(filename);
  if (!fin) {
    return false;
  }
  int vnum, cnum;
  fin >> cnum >> vnum;
  ssf.nbh_c.resize(cnum * 4);
  ssf.nbh_v.resize(cnum * 4);
  for (int i = 0; i < cnum * 4; ++i) {
    int tmp;
    fin >> tmp;
    ssf.nbh_c[i] = tmp;
  }
  for (int i = 0; i < cnum * 4; ++i) {
    int tmp;
    fin >> tmp;
    ssf.nbh_v[i] = tmp;
  }
  return true;
}

int main() {
  StoreSurface ssf;
  std::string filepath("D:\\github\\livesu_data\\cpp_input\\");
  vector<string> dirs{"blade",  "buste",        "indorelax",    "kiss",
                      "kitten", "mechanical02", "mechanical05", "mechanical08",
                      "sculpt", "sphinx",       "teapot"};

  // auto sf = ssf.getEntireSurface();
  // sf.Regular();
  // auto ssf1 = StoreSurface(sf);
  // ssf1.Write(std::cout);
  int b = 0;

  readDataBin("datastore_bin_withsingle.dat");
  for (auto dir : dirs) {
    for (int i = 0; i < 20; ++i) {
      std::string filename = filepath + dir + std::string("\\") + to_string(i) +
                             std::string("_out.txt");
      if (readssf(filename, ssf)) {
        cout << dir << " " << i << " : ";
        auto sf = ssf.getEntireSurface();
        sf.Regular();
        auto ssf1 = StoreSurface(sf);
        cout << (DataStore.index.count(ssf1) != 0 ? DataStore.index[ssf1].size()
                                                  : 0)
             << endl;
        if (b == 0 && DataStore.index.count(ssf1) != 0) {
          b = 1;
          auto modu = DataStore.data.at(DataStore.index[ssf1][0]);
          modu.Write(cout);
        }
      }
    }
  }
}