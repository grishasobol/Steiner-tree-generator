#include "Treck.h"

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
    if (p.get_distance(*e.p1) < p.get_distance(*e.p2)) {
      return make_treck(p, *e.p1);
    }
    else {
      return make_treck(p, *e.p2);
    }
  }
}

unique_ptr<Treck> make_treck(const Edge& e1, const Edge& e2) {
  unsigned e1_p1, e1_p2, e2_p1, e2_p2;
  tie(e1_p1, e1_p2, e2_p1, e2_p2) = make_tuple(e1.get_distance(*e2.p1), e1.get_distance(*e2.p1), e1.get_distance(*e2.p1), e1.get_distance(*e2.p1));
  auto list = { tie(e1_p1, *e2.p1, e1), tie(e1_p2, *e2.p2, e1), tie(e2_p1, *e1.p1, e2), tie(e2_p2, *e1.p2, e2) };
  auto min_elem = list.begin();
  for (auto& elem : list) {
    if (get<0>(*min_elem) > get<0>(elem)) {
      min_elem = &elem;
    }
  }
  return make_treck(get<1>(*min_elem), get<2>(*min_elem));
}

unique_ptr<Treck> make_treck(const STree& t1, const STree& t2) {
  if (t1.edges.empty() && t2.edges.empty()) {
    return make_treck(*t1.points.front().get(), *t2.points.front().get());
  }
  else if (!t1.edges.empty() && t2.edges.empty()) {
    auto& min_point = *t2.points.front().get();
    auto min_edge = t1.edges.front();
    auto min_distance = min_edge->get_distance(min_point);
    for (auto itr = ++t1.edges.begin(); itr != t1.edges.end(); ++itr) {
      auto e1 = *itr;
      if (min_distance > e1->get_distance(min_point)) {
        min_distance = e1->get_distance(min_point);
        min_edge = e1;
      }
    }
    return make_treck(min_point, *min_edge);
  }
  else if (t1.edges.empty() && !t2.edges.empty()) {
    return make_treck(t2, t1);
  }
  else {
    auto min_edge1 = *t1.edges.begin();
    auto min_edge2 = *t2.edges.begin();
    auto min_distance = min_edge1->get_distance(*min_edge2);
    for (auto itr1 = ++t1.edges.begin(); itr1 != t1.edges.end(); ++itr1) {
      for (auto itr2 = ++t2.edges.begin(); itr2 != t2.edges.end(); ++itr2) {
        auto e1 = *itr1;
        auto e2 = *itr2;
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