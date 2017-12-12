//#include <vector>
//#include <set>
//#include <cmath>
//#include <algorithm>
//#include <tuple>
//#include <memory>
//
//using namespace std;
//
//
//
//
//
//class STree {
//public:
//  set<Point> points;
//  set<Segment> segments;
//  const unsigned N;
//  STree(unsigned N) : N(N) {}
//  void add(unsigned x, unsigned y) {
//    points.insert(Point(x, y));
//  }
//  void add(const Point& p1, const Point& p2) {
//    segments.insert(Segment(p1, p2));
//  }
//  unsigned get_distance(const STree& tree) const {
//    if (segments.empty)
//      return get_distance
//    const Segment& s1 = *segments.begin();
//    const Segment& s2 = *tree.segments.begin();
//    unsigned min = s1.get_distance(s2);
//    for (auto& seg1 : segments) {
//      for (auto& seg2 : tree.segments) {
//        auto distance = seg1.get_distance(seg2);
//        if (distance < min) {
//          min = distance;
//        }
//      }
//    }
//    return min;
//  }
//  void add(const STree& tree) {
//    const Segment& s1 = *segments.begin();
//    const Segment& s2 = *tree.segments.begin();
//    unsigned min = s1.get_distance(s2);
//    for (auto& seg1 : segments) {
//      for (auto& seg2 : tree.segments) {
//        auto distance = seg1.get_distance(seg2);
//        if (distance < min) {
//          std::tie();
//        }
//      }
//    }
//  }
//};

#include <iostream>
#include <cstdio>
#include "Primitives.h"

using namespace std;

int main() {
  cout << "TESTING" << endl;
  auto p1 = Point(10, 20);
  auto p2 = Point(45, 46);
  printf("(%d,%d)(%d,%d) DISTANCE = %d", p1.x, p1.y, p2.x, p2.y, PPAngle(p1, p2).get_distance());
  auto t = PPAngle(p1, p2).release();
  cout << endl << get<1>(t)[0].isHorizontal << " " << get<1>(t)[1].isHorizontal;
  auto tt = make_treck(p1, p2)->release();
  cout << endl << get<1>(tt)[0].isHorizontal << " " << get<1>(tt)[1].isHorizontal;
  getchar();
}