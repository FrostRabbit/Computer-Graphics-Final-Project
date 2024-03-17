#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
 public:
  Camera(glm::vec3 _position);
  void initialize(float aspectRatio);
  void move(GLFWwindow* window, float rotate_x, float rotate_y, glm::vec3 pos);
  void updateViewMatrix();
  void updateProjectionMatrix(float aspectRatio);

  const float* getProjectionMatrix() const { return glm::value_ptr(projectionMatrix); }
  const float* getViewMatrix() const { return glm::value_ptr(viewMatrix); }
  const float* getPosition() const { return glm::value_ptr(position); }

private:
  glm::vec3 position;
  glm::vec3 origin_position;
  glm::vec3 up;
  glm::vec3 front;
  glm::vec3 right;
  glm::vec3 aim;

  glm::quat rotation_x;
  glm::quat rotation_y;
  // TODO (optional): Change these values if your WASD or mouse move too slow or too fast.
  constexpr static float keyboardMoveSpeed = 0.1f;
  constexpr static float mouseMoveSpeed = 0.001f;

  // matrix
  glm::mat4 projectionMatrix;
  glm::mat4 viewMatrix;
};