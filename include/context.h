#pragma once

#include <vector>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/vec3.hpp>

#include "model.h"
#include "camera.h"
#include "program.h"

// Global varaibles share between main.cpp and shader programs
class Context {
 public:
  // Index of program id in Context::programs vector 
  // which is currently use to render all objects
  int currentProgram = 0;

 public:
  std::vector<Program* > programs;
  std::vector<Model* > models;
  std::vector<Object* > objects;

 public:
  int directionLightEnable = 1;
  glm::vec3 directionLightDirection = glm::vec3(-0.3f, -0.5f, -0.2f);
  glm::vec3 directionLightColor = glm::vec3(0.6f, 0.6f, 0.6f);

  int pointLightEnable = 0;
  glm::vec3 pointLightPosition = glm::vec3(6, 3, 0);
  glm::vec3 pointLightColor = glm::vec3(1.0f, 0.0f, 0.0f);
  float pointLightConstant = 0.9f;
  float pointLightLinear = 0.027f;
  float pointLightQuardratic = 0.0128f;
  float _pointLightPosisionDegree = 0.0f;

  int spotLightEnable = 1;
  glm::vec3 spotLightPosition = glm::vec3(0, 0.5, -1.0);
  glm::vec3 spotLightDirection = glm::vec3(1.0f, 1.0f, 1.0f);
  glm::vec3 spotLightColor = glm::vec3(1.0f, 0.996f, 0.675f);
  float spotLightCutOff = glm::cos(glm::radians(20.0f));
  float spotLightConstant = 0.9f;
  float spotLightLinear = 0.014f;
  float spotLightQuardratic = 0.007f;

 public:
  Camera *camera = 0;
  GLFWwindow *window = 0;
  GLuint skymap = 0;
};