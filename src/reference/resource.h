#pragma once 

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#include "cstdio"
#include "set" 
#include "map"
#include "vector"
#include "cstring" 
#include "algorithm"
#include "ctime"
#include "cstdlib"
class Topology;
class TopoPtr;
class Polyhedron;
class Polygon;
class SeperatedPolyhedron;
class PolyhedronCompare;
using namespace std;
typedef char PointType;
typedef char FaceType;
typedef char PolyhedronType;
const PolyhedronType CUBE = 0;
const PolyhedronType PYRAMID = 1;
const PolyhedronType CAP14 = 2;
#pragma once
