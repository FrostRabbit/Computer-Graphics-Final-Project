#include "camera.h"
#include<iostream>
#include <opengl_context.h>

using namespace std;
#define ANGLE_TO_RADIAN(x) (float)((x)*M_PI / 180.0f)
#define ANGLE_TO_RADIAN(x) (float)((x)*M_PI / 180.0f)
#define GRAVITY 1.0f / 3.0f
Camera::Camera(glm::vec3 _position)
    : position(_position),
      origin_position(_position),
      up(0, 1, 0),
      front(0, 0, -1),
      right(1, 0, 0),
      rotation_x(glm::identity<glm::quat>()),
      rotation_y(glm::identity<glm::quat>()),
      aim(0,0,-100),
      projectionMatrix(1),
      viewMatrix(1) {}

void Camera::initialize(float aspectRatio) {
  updateProjectionMatrix(aspectRatio);
  updateViewMatrix();
}

void Camera::move(GLFWwindow* window, float rotate_x,float rotate_y,glm::vec3 pos) {

    aim = pos + glm::vec3(0, rotate_y, 0);
    rotation_x = glm::angleAxis(ANGLE_TO_RADIAN(rotate_x), glm::vec3(0, 1, 0));
    rotation_y = glm::angleAxis(ANGLE_TO_RADIAN(rotate_y), glm::vec3(1, 0, 0));
    position = pos + glm::mat3_cast(rotation_x) * origin_position;
    updateViewMatrix();
}

void Camera::updateViewMatrix() {
  constexpr glm::vec3 original_front(0, 0, -1);
  constexpr glm::vec3 original_up(0, 1, 0);
  glm::mat4 r = glm::mat4_cast(this->rotation_x);
  glm::vec3 front = glm::normalize(r * glm::vec4(original_front, 1.0));
  glm::vec3 up = glm::normalize(r * glm::vec4(original_up, 1.0));
  glm::vec3 right = glm::normalize(glm::cross(up, front));
  this->front = front;
  this->up = up;
  this->right = -right;
  this->viewMatrix = glm::lookAt(this->position, this->aim, up);
}

void Camera::updateProjectionMatrix(float aspectRatio) {
  constexpr float FOV = glm::radians(45.0f);
  constexpr float zNear = 0.1f;
  constexpr float zFar = 100.0f;

  this->projectionMatrix = glm::perspective(FOV, aspectRatio, zNear, zFar);
}