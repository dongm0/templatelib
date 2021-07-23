#include "SeperatedPolyhedron.h"
#include "polygon.h"
#include "polyhedron.h"
#include "resource.h"
#include "topoPtr.h"
#include "topology.h"
set<Polyhedron> done;
set<Polyhedron, PolyhedronCompare> ready;
map<Polyhedron, set<Polyhedron, PolyhedronCompare>::iterator> location;
extern bool needcheck;
void addPolyhedron(const Polyhedron &combination) {
  if (!combination.available())
    return;
  set<Polyhedron>::iterator doneit = done.find(combination);
  if (doneit != done.end()) {
    /*
    if ((doneit->divSum()+combination.divSum())%2!=0) {
            FILE *fp = fopen("modu.txt","a+");
            combination.print(fp);
            doneit->print(fp);
            fclose(fp);
    }
    */
    return;
  }
  map<Polyhedron, set<Polyhedron, PolyhedronCompare>::iterator>::iterator
      modularity;
  modularity = location.find(combination);
  if (modularity == location.end()) {
    // combination.addDiameter();
    ready.insert(combination);
    location[combination] = ready.find(combination);
  } else {
    /*
    if ((combination.divSum() + modularity->first.divSum())%2!=0 &&
    combination.divSum() != modularity->first.divSum()) { FILE *fp =
    fopen("modu.txt","a+"); combination.print(fp); modularity->first.print(fp);
            fclose(fp);
    }
    */
    if (combination.divSum() < modularity->first.divSum()) {
      ready.erase(location[combination]);
      ready.insert(combination);
      location[combination] = ready.find(combination);
    }
  }
}
int work() {
  multimap<Polygon, SeperatedPolyhedron> part;
  Polyhedron cube(CUBE);
  addPolyhedron(cube);
  // Polyhedron pyramid(PYRAMID);
  vector<int> structure_temp;
  vector<SeperatedPolyhedron> seperated_cube = cube.divide_old();
  for (int i = 0; i < seperated_cube.size(); i++) {
    Polygon p1 = seperated_cube[i].poly(1);
    pair<Polygon, SeperatedPolyhedron> pair1(p1, seperated_cube[i]);
    if (seperated_cube[i].available())
      part.insert(pair1);
  }

  int kk = 0;
  FILE *fout, *fout1;
  errno_t er = fopen_s(&fout, "result.txt", "w");
  fopen_s(&fout1, "raw.txt", "w");
  if (er != 0) {
    printf("!!!!!!!!!!\n");
  }
  while (1) {
    kk++;
    // if (kk % 100 == 0) {
    //	cube.showdia();
    //	break;
    //}
    set<Polyhedron, PolyhedronCompare>::iterator it;
    if (ready.empty()) {
      printf("nothing in the queue\n");
      break;
    }
    it = ready.begin();
    Polyhedron poly = *it;
    //锟剿筹拷
    if (poly.divSum() > 18)
      break;
    poly.print(fout);
    vector<SeperatedPolyhedron> fragment = poly.divide();
    ready.erase(it);
    map<Polyhedron, set<Polyhedron, PolyhedronCompare>::iterator>::iterator
        loc = location.find(poly); //锟斤拷锟斤拷锟斤开始拼
    if (loc == location.end()) {
      printf("location not find1\n");
    } else
      location.erase(loc);
    /*
    for (unsigned int i = 0; i < fragment.size(); i++) {
            Polygon p1 = fragment[i].poly(1), p2 = fragment[i].poly(2);
            pair<Polygon, SeperatedPolyhedron> pair1(p1, fragment[i]);
            if (fragment[i].available())
                    part.insert(pair1);
    }
    */
    int count = 0;
    for (unsigned int i = 0; i < fragment.size(); i++) {
      Polygon p1 = fragment[i].poly(1), p2 = fragment[i].poly(2);
      pair<multimap<Polygon, SeperatedPolyhedron>::iterator,
           multimap<Polygon, SeperatedPolyhedron>::iterator>
          range;
      // multimap<Polygon, SeperatedPolyhedron>::iterator itBegin, itEnd;
      range = part.equal_range(p1);
      for (multimap<Polygon, SeperatedPolyhedron>::iterator itor = range.first;
           itor != range.second; itor++) {
        Polyhedron newPolyhedron(fragment[i], itor->second);
        addPolyhedron(newPolyhedron);
        count++;
      }
    }
    done.insert(poly);
    poly.show(done, structure_temp, fout1);
    printf("%-6d %d", kk, poly.divSum());
    printf("\n");
    // if (pyramid == poly) {
    // pyramid = poly;
    // break;
    //}
  }
  fclose(fout);
  fclose(fout1);
  // cube.showdia();
  // if (pyramid.divSum() != -1) {
  // printf("ok\n");
  // pyramid.show(done);
  //}
  // printf("锟揭碉拷锟斤拷%d\n", Polyhedron::load_custom());
  return 0;
}
void work2() {
  Polyhedron final = Polyhedron::load(done);
  // Polyhedron::load_custom();
  // final.show(done);
}
int main(int argc, char *argv[]) {
  long h, m, s;
  clock_t st = clock();
  work();
  clock_t ed = clock();
  s = (ed - st) / CLOCKS_PER_SEC;
  h = s / 3600;
  m = (s - s / 3600) / 60;
  s = s - h * 3600 - m * 60;
  printf("use time %d小时%d锟斤拷%d锟斤拷\n", h, m, s);
  system("pause");
  return 0;
}