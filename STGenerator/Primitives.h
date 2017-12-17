#pragma once
#include <vector>
#include <set>
#include <cmath>
#include <algorithm>
#include <tuple>
#include <memory>
#include <cassert>
#include <ostream>
#include <iostream>
#include <numeric>

using namespace std;

class Point {
public:
  int x;
  int y;
  Point(unsigned x, unsigned y) : x(x), y(y) {}
  Point(const Point& p) {
    x = p.x;
    y = p.y;
    cout << "POINT_COPY" << endl;
  }
  Point& operator=(Point& p) {
    x = p.x;
    y = p.y;
    cout << "POINT_COPY" << endl;
  }
  unsigned get_distance(const Point& p) const {
    return abs(x - p.x) + abs(y - p.y);
  }
  friend ostream& operator<<(ostream& os, const Point& p) {
    return os << "<point x=\"" << p.x << "\" y=\"" << p.y << "\" layer=\"pins\" type=\"pin\"/>";
  }
};

class Edge {
public:
  const Point* p1;
  const Point* p2;
  bool isHorizontal;
  Edge(const Point& p1, const Point& p2) : p1(&p1), p2(&p2)
  {
    isHorizontal = p1.y == p2.y;
    if (!isHorizontal)
      assert(p1.x != p2.y && "Edge could be done only between points on one line");
  }
  bool liesOpposite(const Point& p) const {
    if ((p1->x < p.x && p.x < p2->x) || 
      (p2->x < p.x && p.x < p1->x) || 
      (p1->y < p.y && p.y < p2->y) || 
      (p2->y < p.y && p.y < p1->y))
      return true;
    else
      return false;
  }
  unsigned get_distance(const Point& p) const {
    if (isHorizontal && (p1->x <= p.x && p.x <= p2->x ||
      p2->x <= p.x && p.x <= p1->x)) {
      return abs(p1->y - p.y);
    }
    else if (p1->y <= p.y && p.y <= p2->y ||
      p2->y <= p.y && p.y <= p1->y) {
      return abs(p1->x - p.x);
    }
    else {
      return min(p.get_distance(*p1), p.get_distance(*p2));
    }
  }
  unsigned get_distance(const Edge& s) const {
    return min({ get_distance(*s.p1), get_distance(*s.p2), s.get_distance(*p1), s.get_distance(*p2) });
  }
  unsigned get_len() const {
    return p1->get_distance(*p2);
  }
  friend ostream& operator<<(ostream& os, const Edge& e) {
    return os << "<segment x1=\"" << e.p1->x << "\" y1=\"" << e.p1->y <<
      "\" x2=\"" << e.p2->x << "\" y2=\"" << e.p2->y << "\" layer=\"" << (e.isHorizontal ? "m2" : "m3") << "\"/>";
  }
};

class STree {
public:
  vector<shared_ptr<Point>> points;
  vector<shared_ptr<Edge>> edges;
  STree() {
    points = {};
    edges = {};
  }
  STree(int x, int y) {
    points.push_back(make_shared<Point>(x, y));
  }
  STree(const Point& p) : STree(p.x, p.y) {}
  //STree(const STree& tree) {
  //  cout << "COPY_TREE" << endl;
  //}
  void add_point(shared_ptr<Point> p) {
    points.push_back(p);
  }
  void add_edge(shared_ptr<Edge> e) {
    edges.push_back(e);
  }
  void add_tree(const STree& tree) {
    //auto treck = make_treck(*this, tree);
    //treck->release(*this);
    points.insert(points.end(), tree.points.begin(), tree.points.end());
    edges.insert(edges.end(), tree.edges.begin(), tree.edges.end());
  }
  friend ostream& operator<<(ostream& os, const STree& t) {
    auto max_x = max_element(t.points.begin(), t.points.end(), [](auto& p1, auto& p2) {return p1->x < p2->x; });
    auto n = max_x->get()->x;
    auto max_y = max_element(t.points.begin(), t.points.end(), [](auto& p1, auto& p2) {return p1->y < p2->y; });
    auto m = max_y->get()->y;
    //os << "<net " << "n=\"" << n << "\" m=\"" << m << "\">\n";
    os << "<net " << "grid_size=\"" << max(n,m) + 1 << "\">\n";
    for (auto& p : t.points) {
      os << *p.get() << "\n";
    }
    for (auto& e : t.edges) {
      os << *e.get() << "\n";
    }
    auto len = accumulate(t.edges.begin(), t.edges.end(), 0, [](const unsigned previous, const auto e) {return previous + e->get_len(); });
    os << "<length v=\"" << len << "\" />\n";
    return os << "</net>";
  }
};