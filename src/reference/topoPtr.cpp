#include "topoPtr.h"
#include "topology.h"
TopoPtr::TopoPtr() { size = 0; }

int TopoPtr::insert(const Topology &topo) {
  map<Topology, int>::iterator address;
  address = content.find(topo);
  if (address == content.end()) {
    if (!topo.available())
      return -1;
    map<Topology, int>::value_type p(topo, size++);
    pair<map<Topology, int>::iterator, bool> res = content.insert(p);
    if (res.second == false) {
      printf("insert topology failed\n");
      --size;
      return -1;
    }
    address = res.first;
    list.insert(list.end(), &address->first);
    return size - 1;
  } else
    return address->second;
}
const Topology *TopoPtr::getTopo(int topoNum) {
  if (topoNum >= 0)
    return list[topoNum];
  else
    return NULL;
}
int TopoPtr::maxSize() { return size; }