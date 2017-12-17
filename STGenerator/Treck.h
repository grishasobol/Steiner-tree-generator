#pragma once
#include "Primitives.h"
#include <cassert>

using namespace std;

class Treck {
protected:
  unsigned distance;
public:
  Treck(unsigned distance) : distance(distance) {}
  virtual ~Treck() = default;
  virtual void release(STree& tree) const = 0;
  unsigned get_distance() {
    return distance;
  }
  virtual bool isAngle() {
    return false;
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
  void release(STree& tree) const override {
    tree.add_edge(make_shared<Edge>(p1, p2));
  }
};

class PPAngle : public PPTreck {
  bool orentation = true;
public:
  PPAngle(const Point& p1, const Point& p2) : PPTreck(p1, p2) {
    assert(p1.x != p2.x && p1.y != p2.y && "Couldn't create angle treck between points");
  }
  void release(STree& tree) const override {
    shared_ptr<Point> p;
    if (orentation)
      p = make_shared<Point>(p1.x, p2.y);
    else
      p = make_shared<Point>(p2.x, p1.y);
    tree.add_point(p);
    tree.add_edge(make_shared<Edge>(p1, *p.get()));
    tree.add_edge(make_shared<Edge>(p2, *p.get()));
  }
  bool isAngle() override {
    return true;
  }
  void reverse() {
    orentation = !orentation;
  }
};

class PETreck : public Treck {
  const Edge& e;
  const Point& p;
public:
  PETreck(const Point& p, const Edge& e) : Treck(e.get_distance(p)), e(e), p(p) {
    assert(e.liesOpposite(p) && "Cannot creat treck because point doesn't lies opposite edge");
  }
  void release(STree& tree) const override {
    int x, y;
    if (e.isHorizontal) {
      x = p.x;
      y = e.p1->y;
    }
    else {
      x = e.p1->x;
      y = p.y;
    }
    auto sp = make_shared<Point>(x, y);
    tree.add_point(sp);
    tree.add_edge(make_shared<Edge>(p, *sp.get()));
  }
};

unique_ptr<Treck> make_treck(const Point& p1, const Point& p2);
unique_ptr<Treck> make_treck(const Point& p, const Edge& e);
unique_ptr<Treck> make_treck(const Edge& e1, const Edge& e2);
unique_ptr<Treck> make_treck(const STree& t1, const STree& t2);