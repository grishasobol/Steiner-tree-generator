#pragma once
#include <vector>
#include <set>
#include <cmath>
#include <algorithm>
#include <tuple>
#include <memory>
#include <cassert>

using namespace std;

class Point {
public:
  int x;
  int y;
  Point(unsigned x, unsigned y) : x(x), y(y) {}
  unsigned get_distance(const Point& p) const {
    return abs(x - p.x) + abs(y - p.y);
  }
};

class Edge {
public:
  const Point& p1;
  const Point& p2;
  bool isHorizontal;
  Edge(const Point& p1, const Point& p2) : p1(p1), p2(p2)
  {
    isHorizontal = p1.y == p2.y;
    if (!isHorizontal)
      assert(p1.x != p2.y && "Edge could be done only between points on one line");
  }
  bool liesOpposite(const Point& p) const {
    if (p1.x < p.x < p2.x || p2.x < p.x < p1.x || p1.y < p.x < p2.y || p2.y < p.x < p1.y)
      return true;
    else
      return false;
  }
  unsigned get_distance(const Point& p) const {
    if (isHorizontal && (p1.x <= p.x <= p2.x || p2.x <= p.x <= p1.x)) {
      return abs(p1.y - p.y);
    }
    else if (p1.y <= p.y <= p2.y || p2.y <= p.y <= p1.y) {
      return abs(p1.x - p.x);
    }
    else {
      return min(p.get_distance(p1), p.get_distance(p2));
    }
  }
  unsigned get_distance(const Edge& s) const {
    return min({ get_distance(s.p1), get_distance(s.p2), s.get_distance(p1), s.get_distance(p2) });
  }
};

class STree {
public:
  vector<Point> points;
  vector<Edge> edges;
  STree(int x, int y) {
    points.push_back(Point(x, y));
  }
};

class Treck {
public:
  using treck_type = tuple<vector<Point>, vector<Edge>>;
protected:
  unsigned distance;
public:
  Treck(unsigned distance) : distance(distance) {}
  virtual ~Treck() = default;
  virtual treck_type release() = 0;
  unsigned get_distance() {
    return distance;
  }
};

class PPTreck : public Treck {
protected:
  const Point& p1;
  const Point& p2;
  PPTreck(const Point& p1, const Point& p2) : Treck(p1.get_distance(p2)), p1(p1), p2(p2) {}
};

class PPStraight : public PPTreck {
public:
  PPStraight(const Point& p1, const Point& p2) : PPTreck(p1, p2) {
    assert(p1.x == p2.x || p1.y == p2.y && "Couldn't create straight treck between points");
  }
  treck_type release() override {
    return make_tuple<vector<Point>, vector<Edge>>({}, { Edge(p1, p2) });
  }
};

class PPAngle : public PPTreck {
public:
  PPAngle(const Point& p1, const Point& p2) : PPTreck(p1, p2) {
    assert(p1.x != p2.x && p1.y != p2.y && "Couldn't create angle treck between points");
  }
  treck_type release() override {
    auto p = Point(p1.x, p2.y);
    return make_tuple<vector<Point>, vector<Edge>>({p}, { Edge(p1, p), Edge(p2, p) });
  }
};

class PETreck : public Treck {
  const Edge& e;
  const Point& p;
public:
  PETreck(const Point& p, const Edge& e) : Treck(e.get_distance(p)), e(e), p(p){
    assert( e.liesOpposite(p) && "Cannot creat treck because point doesn't lies opposite edge");
  }
  treck_type release() override {
    int x, y;
    if (e.isHorizontal) {
      x = p.x;
      y = e.p1.y;
    }
    else {
      x = e.p1.x;
      y = p.y;
    }
    Point ShniederPoint = Point(x, y);
    return make_tuple<vector<Point>, vector<Edge>>({ Point(x, y) }, { Edge(p, ShniederPoint)});
  }
};


unique_ptr<Treck> make_treck(const Point& p1, const Point& p2) {
  if (p1.x == p2.x || p1.y == p2.y) {
    return make_unique<PPStraight>(PPStraight(p1, p2));
  }
  else {
    return make_unique<PPAngle>(PPAngle(p1, p2));
  }
}

unique_ptr<Treck> make_treck(const Point& p, const Edge& e) {
  if (e.liesOpposite(p)) {
    return make_unique<PETreck>(PETreck(p, e));
  }
  else {
    if (p.get_distance(e.p1) < p.get_distance(e.p1)) {
      return make_treck(p, e.p1);
    }
    else {
      return make_treck(p, e.p2);
    }
  }
}

unique_ptr<Treck> make_treck(const Edge& e1, const Edge& e2) {
  unsigned e1_p1, e1_p2, e2_p1, e2_p2;
  tie(e1_p1, e1_p2, e2_p1, e2_p2) = make_tuple(e1.get_distance(e2.p1), e1.get_distance(e2.p1), e1.get_distance(e2.p1), e1.get_distance(e2.p1));
  auto list = { tie(e1_p1, e2.p1, e1), tie(e1_p2, e2.p2, e1), tie(e2_p1, e1.p1, e2), tie(e2_p2, e1.p2, e2) };
  auto distance = min(list);
  return make_treck(get<1>(distance), get<2>(distance));
}

unique_ptr<Treck> make_treck(const STree& t1, const STree& t2) {
  if (t1.edges.empty && t2.edges.empty) {
    return make_treck(t1.points.front(), t2.points.front());
  }
  else if (!t1.edges.empty && t2.edges.empty) {
    auto& min_point = t2.points.front();
    auto min_edge = &t1.edges.front();
    auto min_distance = min_edge->get_distance(min_point);
    for (auto e1 = ++t1.edges.begin(); e1 != t1.edges.end(); ++e1) {
      if (min_distance > e1->get_distance(min_point)) {
        min_distance = e1->get_distance(min_point);
        min_edge = &*e1;
      }
    }
    return make_treck(min_point, *min_edge);
  }
  else if (t1.edges.empty() && !t2.edges.empty()) {
    return make_treck(t2, t1);
  }
  else {
    auto min_edge1 = t1.edges.begin();
    auto min_edge2 = t2.edges.begin();
    auto min_distance = min_edge1->get_distance(*min_edge2);
    for (auto e1 = ++t1.edges.begin(); e1 != t1.edges.end(); ++e1) {
      for (auto e2 = ++t2.edges.begin(); e2 != t2.edges.end(); ++e2) {
        if (min_distance > e1->get_distance(*e2)) {
          min_distance = e1->get_distance(*e2);
          min_edge1 = e1;
          min_edge2 = e2;
        }
      }
    }
    return make_treck(*min_edge1, *min_edge2);
  }
}