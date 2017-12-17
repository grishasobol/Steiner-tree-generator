#include <iostream>
#include <cstdio>
#include <fstream>
#include <string>
#include <map>

#include "Primitives.h"
#include "Treck.h"
#include "tinyxml\tinystr.h"
#include "tinyxml\tinyxml.h"

using namespace std;

vector<Point> parse_xml(const char* file_name) {
  vector<Point> points = {};
  TiXmlDocument doc(file_name);
  if(!doc.LoadFile())
    throw "Cannot import xml file: " + string(file_name);
  TiXmlElement *RootElement = doc.RootElement();
  for (auto xml_point = RootElement->FirstChildElement("point"); xml_point; xml_point = xml_point->NextSiblingElement("point")) {
    auto xml_x = xml_point->Attribute("x");
    auto xml_y = xml_point->Attribute("y");
    points.emplace_back(atoi(xml_x), atoi(xml_y));
  }
  return points;
}

void join_trees(STree& t1, const STree& t2) {
  make_treck(t1, t2)->release(t1);
  t1.add_tree(t2);
}

int main(int argc, char* argv[]) {
  //cout << "TESTING" << endl;
  //{
  //  auto p1 = Point(10, 20);
  //  auto p2 = Point(45, 46);
  //  auto p3 = Point(15, 25);
  //  auto p4 = Point(20, 33);
  //  cout << p1 << "\n" << p2 << endl;
  //  //  printf("(%d,%d)(%d,%d) DISTANCE = %d\n", p1.x, p1.y, p2.x, p2.y, PPAngle(p1, p2).get_distance());

  //  STree t1(p1);
  //  STree t2(p2);
  //  STree t3(p3);
  //  STree t4(p4);

  //  join_trees(t1, t2);
  //  join_trees(t3, t4);
  //  join_trees(t1, t3);

  //  cout << t1 << endl;
  //}

  char* input_file_name = (argc > 1) ? argv[1] : "points.xml";
  auto& points = parse_xml(input_file_name);
  vector<shared_ptr<STree>> trees;
  for (auto& point : points) {
    cout << point << endl;
    trees.push_back(make_shared<STree>(point));
  }
  map<pair<shared_ptr<STree>, shared_ptr<STree>>, unique_ptr<Treck>> trees_treck{};
  for (auto& tree1 : trees) {
    for (auto& tree2 : trees) {
      if (&tree1 == &tree2)
        continue;
      trees_treck.insert({make_pair(tree1, tree2), move(make_treck(*tree1, *tree2)) });
    }
  }

  while (trees.size() > 1) {
    auto min_treck = min_element(trees_treck.begin(), trees_treck.end(), [](auto& p1, auto& p2) {return p1.second->get_distance() < p2.second->get_distance(); });
    //cout << min_treck->second->get_distance();

    auto treck = min_treck->second.get();
    auto tree1 = min_treck->first.first;
    auto tree2 = min_treck->first.second;

    //cout << endl;
    //cout << *tree1 << endl << *tree2 << endl;

    if (treck->isAngle()) {
      auto angle_treck = static_cast<PPAngle*>(treck);
      STree treck_tree1, treck_tree2;
      angle_treck->release(treck_tree1);
      angle_treck->reverse();
      angle_treck->release(treck_tree2);
      unsigned first_distance = 0, second_distance = 0;
      for (auto& tree : trees) {
        first_distance += make_treck(*tree, treck_tree1)->get_distance();
        second_distance += make_treck(*tree, treck_tree2)->get_distance();
      }
      if (first_distance > second_distance) {
        tree1->add_tree(treck_tree2);
      }
      else {
        tree1->add_tree(treck_tree1);
      }
    }
    else {
      treck->release(*tree1);
    }
    tree1->add_tree(*tree2);
    //cout << "AFTER:" << endl;
    //cout << *tree1 << endl;
    trees.erase(find(trees.begin(), trees.end(), tree2));
    for (auto& tree : trees) {
      trees_treck.erase({ tree2, tree });
      trees_treck.erase({ tree, tree2 });
      if (tree == tree1)
        continue;
      trees_treck[make_pair(tree1, tree)] = move(make_treck(*tree, *tree1));
    }
  }

  char* output_file_name = (argc > 2) ? argv[2] : "out.xml";
  ofstream out_file(output_file_name);
  out_file << *trees.front() << endl;
  out_file.close();

  getchar();
}