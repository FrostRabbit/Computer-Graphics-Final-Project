#include "model.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>

#include <glm/vec3.hpp>
using namespace std;

void face_parser(std::string str, int& v, int& vt, int& vn) {
  v = 0;
  vt = 0;
  vn = 0;
  int i = 0;
  for (; i < str.size() && str[i] != '/'; i++) {
    v = v * 10 + (int)(str[i] - '0');
  }
  i++;
  for (; i < str.size() && str[i] != '/'; i++) {
    vt = vt * 10 + (int)(str[i] - '0');
  }
  i++;
  for (; i < str.size() && str[i] != '/'; i++) {
    vn = vn * 10 + (int)(str[i] - '0');
  }
}

std::vector<Model*> Model::fromObjectFile(const char* obj_file) {
  std::ifstream ObjFile(obj_file);

  if (!ObjFile.is_open()) {
    std::cout << "Can't open File !" << std::endl;
    return std::vector<Model*>(); 
  }
  std::string line;
  std::string type;
  std::vector<float> v, vt, vn;
  std::vector<Model*> models;
  Model* m = new Model();
  while (std::getline(ObjFile, line)) {
    std::istringstream iss(line);
    iss >> type;
    if (type == "o") {
      while (std::getline(ObjFile, line)) {
        std::istringstream iss(line);
        iss >> type;
        if (type == "o") {
          // each vertex has 3 components
          m->numVertex = m->positions.size() / 3;
          models.push_back(m);
          m = new Model();
          continue;
        }
        if (type == "v") {
          glm::vec3 vertex;
          iss >> vertex.x >> vertex.y >> vertex.z;
          v.push_back(vertex.x);
          v.push_back(vertex.y);
          v.push_back(vertex.z);
        } else if (type == "vn") {
          glm::vec3 normal;
          iss >> normal.x >> normal.y >> normal.z;
          vn.push_back(normal.x);
          vn.push_back(normal.y);
          vn.push_back(normal.z);
        } else if (type == "vt") {
          glm::vec2 texcoord;
          glm::vec3 texcoord3;
          iss >> texcoord.x >> texcoord.y;
          vt.push_back(texcoord.x);
          vt.push_back(texcoord.y);
        } else if (type == "f") {
          for (int i = 0; i < 3; i++) {
            std::string p;
            iss >> p;
            int vi, vti, vni;
            face_parser(p, vi, vti, vni);
            m->positions.push_back(v[(vi - 1) * 3]);
            m->positions.push_back(v[(vi - 1) * 3 + 1]);
            m->positions.push_back(v[(vi - 1) * 3 + 2]);
            m->texcoords.push_back(vt[(vti - 1) * 2]);
            m->texcoords.push_back(vt[(vti - 1) * 2 + 1]);
            m->normals.push_back(vn[(vni - 1) * 3]);
            m->normals.push_back(vn[(vni - 1) * 3 + 1]);
            m->normals.push_back(vn[(vni - 1) * 3 + 2]);
            m->numVertex++;
          }
        }
      }
    }
  }
  models.push_back(m);
  if (ObjFile.is_open()) ObjFile.close();
  return models;
}