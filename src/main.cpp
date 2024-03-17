#include <algorithm>
#include <iostream>

#include <memory>
#include <vector>
#include <stack>

#include <GLFW/glfw3.h>
#define GLAD_GL_IMPLEMENTATION
#include <glad/gl.h>
#undef GLAD_GL_IMPLEMENTATION
#include <glm/glm.hpp>

#include <glm/ext/matrix_transform.hpp>

#include "camera.h"
#include "context.h"
#include "gl_helper.h"
#include "model.h"
#include "opengl_context.h"
#include "program.h"
#include "utils.h"

#define ANGLE_TO_RADIAN(x) (float)((x)*M_PI / 180.0f)
#define RADIAN_TO_ANGEL(x) (float)((x)*180.0f / M_PI)

#define CIRCLE_SEGMENT 64
#define Framerate 60.0f
#define ROTATE_SPEED 0.5f * Framerate
#define GRAVITY FLYING_SPEED / 6.0f
#define FLY_SPEED_MULTIPLY 3.0f
#define Angle_speed_press 0.6f * Framerate
#define Angle_speed_release 0.2f * Framerate
glm::vec3 movement(0.0,0.0,0.0);
glm::vec3 pos(1.0f);
glm::vec3 cube_pos(0, 5, 0);
GLfloat air_rotate;
GLfloat wing_rotate;
GLfloat pitch;
GLfloat yaw;
GLfloat wing_left_rx;
GLfloat wing_left_ry;
GLfloat wing_right_rx;
GLfloat wing_right_ry;
GLfloat joint_left_ry;
GLfloat joint_right_ry;
GLfloat engine_move;
GLfloat FLYING_SPEED = ROTATE_SPEED / 3.0f;
int score;
bool hold_space;
bool hold_left;
bool hold_right;
bool hold_up;
bool hold_down;
bool activate = false;
bool on_land;
float deltaTime;

void initOpenGL();
void resizeCallback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int, int action, int);

Context ctx;

Material mFlatwhite;
Material mShinyred;
Material mClearblue;
void drawCube(float size);
void loadMaterial() {
  mFlatwhite.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
  mFlatwhite.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
  mFlatwhite.specular = glm::vec3(0.0f, 0.0f, 0.0f);
  mFlatwhite.shininess = 10;

  mShinyred.ambient = glm::vec3(0.1985f, 0.0000f, 0.0000f);
  mShinyred.diffuse = glm::vec3(0.5921f, 0.0167f, 0.0000f);
  mShinyred.specular = glm::vec3(0.5973f, 0.2083f, 0.2083f);
  mShinyred.shininess = 100.0f;

  mClearblue.ambient = glm::vec3(0.0394f, 0.0394f, 0.3300f);
  mClearblue.diffuse = glm::vec3(0.1420f, 0.1420f, 0.9500f);
  mClearblue.specular = glm::vec3(0.1420f, 0.1420f, 0.9500f);
  mClearblue.shininess = 10;
}

void loadPrograms() {
  ctx.programs.push_back(new ExampleProgram(&ctx));
  ctx.programs.push_back(new BasicProgram(&ctx));
  ctx.programs.push_back(new LightProgram(&ctx));
  ctx.programs.push_back(new skyboxProgram(&ctx));

  for (auto iter = ctx.programs.begin(); iter != ctx.programs.end(); iter++) {
    if (!(*iter)->load()) {
      std::cout << "Load program fail, force terminate" << std::endl;
      exit(1);
    }
  }
  glUseProgram(0);
}

void loadModels() {

  Model* m = new Model();

  std::vector<Model*> models = Model::fromObjectFile("../assets/models/car/new_car_11.obj");
  for (int i = 0; i < 17; i++) {
    //left wing:12 right wing:9
    //left wing:13 right wing:14

    if (i == 13 || i == 14) {
      m = models[i];
      m->textures.push_back(createTexture("../assets/models/cube/texture.bmp"));
      m->modelMatrix = glm::scale(m->modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
      ctx.models.push_back(m);
    }  else if (i == 3 || i == 4 || i == 6 || i == 7) {
      m = models[i];
      m->textures.push_back(createTexture("../assets/models/car/textures/brakes_02.jpeg"));
      m->modelMatrix = glm::scale(m->modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
      ctx.models.push_back(m);
    } else if (i == 10 || i == 11) {  
      m = models[i];
      m->textures.push_back(createTexture("../assets/models/cube/gray.jpeg"));
      m->modelMatrix = glm::scale(m->modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
      ctx.models.push_back(m);
    } else if (i == 9 || i == 12) { 
      m = models[i];
      m->textures.push_back(createTexture("../assets/models/cube/texture.bmp"));
      m->modelMatrix = glm::scale(m->modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
      ctx.models.push_back(m);
    } else if (i == 15 || i == 16) {  
      m = models[i];
      m->textures.push_back(createTexture("../assets/models/cube/texture.bmp"));
      m->modelMatrix = glm::scale(m->modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
      ctx.models.push_back(m);
    } else {
      m = models[i];
      m->textures.push_back(createTexture("../assets/models/cube/gray.jpeg"));
      m->modelMatrix = glm::scale(m->modelMatrix, glm::vec3(0.2f, 0.2f, 0.2f));
      ctx.models.push_back(m);
    }
  }

  std::vector<float> p = {-1.0, 0.0, -100.0, -1.0, 0.0, 100.0, 1.0, 0.0, 100.0, 1.0, 0.0, -100.0};
  std::vector<float> n = {0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0, 0.0};
  std::vector<float> t = {0.0, 0.0, 0.0, 200.0, 2.0, 200.0, 2.0, 0.0};
  m = new Model();
  m->positions.insert(m->positions.end(), p.begin(), p.end());
  m->normals.insert(m->normals.end(), n.begin(), n.end());
  m->texcoords.insert(m->texcoords.end(), t.begin(), t.end());
  m->textures.push_back(createTexture("../assets/models/Wood_maps/road1.jpg"));
  m->modelMatrix = glm::scale(m->modelMatrix, glm::vec3(4.096f, 1.0f, 2.56f));
  m->numVertex = 4;
  m->drawMode = GL_QUADS;
  ctx.models.push_back(m);
}
void setupObjects() {
  for (int i = 0; i < 17; i++) {
    if (i == 10 || i == 11) {
      ctx.objects.push_back(new Object(i, glm::rotate(glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, 0, -2)),
                                                      ANGLE_TO_RADIAN(180), glm::vec3(0, 1, 0))));
      continue;
    }
     ctx.objects.push_back(new Object(i, glm::rotate(glm::translate(glm::identity<glm::mat4>(), glm::vec3(0, 0, 0)),ANGLE_TO_RADIAN(180), glm::vec3(0, 1, 0))));

  }
  ctx.objects.push_back(new Object(17, glm::scale(glm::translate(glm::identity<glm::mat4>(), glm::vec3(0.0, 0.0, 0.0)),glm::vec3(1.0,1.0,25.0))));
}

int main() {
  initOpenGL();
  GLFWwindow* window = OpenGLContext::getWindow();
  glfwSetWindowTitle(window, "final project");

  // Init Camera helper
  Camera camera(glm::vec3(0, 2.5, 6.5));
  camera.initialize(OpenGLContext::getAspectRatio());
  // Store camera as glfw global variable for callbacks use
  glfwSetWindowUserPointer(window, &camera);
  ctx.camera = &camera;
  ctx.window = window;
  vector<const GLchar*> faces;
  faces.push_back("../assets/texture/scene0/px.png");
  faces.push_back("../assets/texture/scene0/nx.png");
  faces.push_back("../assets/texture/scene0/py.png");
  faces.push_back("../assets/texture/scene0/ny.png");
  faces.push_back("../assets/texture/scene0/pz.png");
  faces.push_back("../assets/texture/scene0/nz.png");
  ctx.skymap = loadCubemap(faces,false);

  loadMaterial();
  loadModels();
  loadPrograms();
  setupObjects();

  float lastFrame = glfwGetTime();
  glm::vec3 lightaim(0, 0, -3);
  // Main rendering loop
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    // Polling events.
    glfwPollEvents();
    // Update camera position and view
    camera.move(window, air_rotate, pitch/10, movement);
    // GL_XXX_BIT can simply "OR" together to use.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    /// TO DO Enable DepthTest
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    ctx.spotLightDirection = glm::normalize(lightaim - ctx.spotLightPosition);
    ctx.pointLightPosition = glm::vec3(6 * glm::cos(glm::radians(ctx._pointLightPosisionDegree)), 3.0f,
                                       6 * glm::sin(glm::radians(ctx._pointLightPosisionDegree)));

    glm::mat4 r = glm::rotate(glm::mat4(1.0f), ANGLE_TO_RADIAN(air_rotate), glm::vec3(0.0, 1.0, 0.0));
    glm::vec3 front = glm::normalize(r * glm::vec4(0.0, 0.0, -1.0, 1.0));
    if (movement.y > 0) {
        on_land = false;
    } else {
        on_land = true;
    }
    if (on_land) {
      if (hold_space && hold_up && activate) {
        movement = movement + front * FLYING_SPEED * deltaTime;
        movement.y += FLYING_SPEED * deltaTime*2.0f;
      } else if (hold_up) {
          movement = movement + front * FLYING_SPEED * deltaTime*1.5f;
      }
    } else {
        if (hold_up) {
            movement = movement + front * FLYING_SPEED * deltaTime * 1.5f;
            movement.y -= GRAVITY * deltaTime*0.02f;
            if (hold_space) {
              movement = movement - front * FLYING_SPEED * deltaTime * 0.3f;
              movement.y += FLYING_SPEED * deltaTime*0.5f;
            }
        }
        else if (hold_down) {
            movement = movement + front * FLYING_SPEED * deltaTime * 0.8f;
            movement.y -= GRAVITY * deltaTime * 1.5f;
        } else if (hold_space) {
            movement = movement + front * FLYING_SPEED * deltaTime * 0.8f;
            movement.y += FLYING_SPEED * deltaTime * 0.5f;

        } else {
            movement = movement + front * FLYING_SPEED * deltaTime;
            movement.y -= GRAVITY * deltaTime;
        }
        if (activate == false) {
            movement.y -= GRAVITY * deltaTime*2.0f;
        }
    }


    float total_angle = std::abs(pitch) + std::abs(yaw);
    float pitch_ratio = std::abs(pitch) / total_angle;
    float yaw_ratio = std::abs(yaw) / total_angle;

    if(!hold_down&&!hold_space){
      if (pitch > Angle_speed_release * pitch_ratio*deltaTime) {
        pitch -= Angle_speed_release * pitch_ratio*deltaTime;
      } else if(pitch < -Angle_speed_release * pitch_ratio*deltaTime) {
        pitch += Angle_speed_release * pitch_ratio*deltaTime;
      } else {
        pitch = 0.0f;
      }
    }

    if (hold_left) {
      air_rotate += ROTATE_SPEED*deltaTime;
    }
    if (hold_right) {
      air_rotate -= ROTATE_SPEED*deltaTime;
    }

    if(!hold_left&&!hold_right){
      if (yaw > Angle_speed_release * yaw_ratio*deltaTime) {
        yaw -= Angle_speed_release * yaw_ratio*deltaTime;
      } else if (yaw < -Angle_speed_release * yaw_ratio*deltaTime) {
        yaw += Angle_speed_release * yaw_ratio*deltaTime;
      } else {
        yaw = 0.0f;
      }
    }
    if (air_rotate > 360.0f) {
      air_rotate -= 360.0f;
    }
    if (air_rotate < -360.0f) {
      air_rotate += 360.0f;
    }
    if (activate){
      if(joint_left_ry < 160.0f){
        joint_left_ry += ROTATE_SPEED*deltaTime*2;
        joint_right_ry += ROTATE_SPEED*deltaTime*2;
      }else if(wing_right_rx < 90.0f){
        wing_right_rx += ROTATE_SPEED*deltaTime;
        wing_left_rx += ROTATE_SPEED*deltaTime;
      }else if(wing_right_ry < 90.0f){
        wing_right_ry += ROTATE_SPEED*deltaTime;
        wing_left_ry += ROTATE_SPEED*deltaTime;
      }
      if (engine_move < 2.0f) {
        engine_move += ROTATE_SPEED * deltaTime * 0.01f;
      }
    } else if(joint_left_ry > 0.0f){
      if(wing_right_ry > 0.0f){
        wing_right_ry -= ROTATE_SPEED*deltaTime;
        wing_left_ry -= ROTATE_SPEED*deltaTime;
      }else if(wing_right_rx > 0.0f){
        wing_right_rx -= ROTATE_SPEED*deltaTime;
        wing_left_rx -= ROTATE_SPEED*deltaTime;
      }else if(joint_left_ry > 0.0f){
        joint_left_ry -= ROTATE_SPEED*deltaTime*2;
        joint_right_ry -= ROTATE_SPEED*deltaTime*2;
      }
      if (engine_move > 0.0f) {
        engine_move -= ROTATE_SPEED * deltaTime * 0.01f;
      }
    }
    std::stack<glm::mat4> matrixStack;

    r = glm::rotate(glm::mat4(1.0f), ANGLE_TO_RADIAN(air_rotate+yaw), glm::vec3(0.0, 1.0, 0.0));
    r = glm::rotate(r, ANGLE_TO_RADIAN(pitch), glm::vec3(1.0, 0.0, 0.0));
    glm::mat4 trans = glm::translate(glm::mat4(1.0f), movement);

    //move the car
    for(int i =0;i<17;i++){
          matrixStack.push(ctx.objects[i]->transformMatrix);
          ctx.objects[i]->transformMatrix = trans * (r * ctx.objects[i]->transformMatrix);
      if(i==9){
        ctx.objects[i]->transformMatrix = glm::translate(ctx.objects[i]->transformMatrix, glm::vec3(-0.61, 0.5, -0.675));
        ctx.objects[i]->transformMatrix = glm::rotate(ctx.objects[i]->transformMatrix, ANGLE_TO_RADIAN(wing_right_rx), glm::vec3(1.0, 0.0, 0.0));
        ctx.objects[i]->transformMatrix = glm::rotate(ctx.objects[i]->transformMatrix, ANGLE_TO_RADIAN(wing_right_ry), glm::vec3(0.0, 1.0, 0.0));
        ctx.objects[i]->transformMatrix = glm::translate(ctx.objects[i]->transformMatrix, glm::vec3(0.61, -0.5, 0.675));
      }
      if(i==12){
        ctx.objects[i]->transformMatrix = glm::translate(ctx.objects[i]->transformMatrix, glm::vec3(0.61, 0.5, -0.675));
        ctx.objects[i]->transformMatrix = glm::rotate(ctx.objects[i]->transformMatrix, ANGLE_TO_RADIAN(wing_right_rx), glm::vec3(1.0, 0.0, 0.0));
        ctx.objects[i]->transformMatrix = glm::rotate(ctx.objects[i]->transformMatrix, ANGLE_TO_RADIAN(wing_right_ry), glm::vec3(0.0, -1.0, 0.0));
        ctx.objects[i]->transformMatrix = glm::translate(ctx.objects[i]->transformMatrix, glm::vec3(-0.61, -0.5, 0.675));
      }
      if(i==13){
        ctx.objects[i]->transformMatrix = glm::translate(ctx.objects[i]->transformMatrix, glm::vec3(0.675, 0.5, -0.78));
        ctx.objects[i]->transformMatrix = glm::rotate(ctx.objects[i]->transformMatrix, ANGLE_TO_RADIAN(joint_left_ry), glm::vec3(0.0, 1.0, 0.0));
        ctx.objects[i]->transformMatrix = glm::translate(ctx.objects[i]->transformMatrix, glm::vec3(-0.675, -0.5, 0.78));
      }
      if(i==14){
        ctx.objects[i]->transformMatrix = glm::translate(ctx.objects[i]->transformMatrix, glm::vec3(-0.675, 0.5, -0.78));
        ctx.objects[i]->transformMatrix = glm::rotate(ctx.objects[i]->transformMatrix, ANGLE_TO_RADIAN(joint_right_ry), glm::vec3(0.0, -1.0, 0.0));
        ctx.objects[i]->transformMatrix = glm::translate(ctx.objects[i]->transformMatrix, glm::vec3(0.675, -0.5, 0.78));
      }
      if (i == 10 || i ==11) {
        ctx.objects[i]->transformMatrix =
            glm::translate(ctx.objects[i]->transformMatrix, glm::vec3(0.0, 0.0, -engine_move));
      }
    }
    lightaim = glm::vec3(trans * (r * glm::vec4(lightaim, 1.0)));
    ctx.spotLightPosition = glm::vec3(trans * (r * glm::vec4(ctx.spotLightPosition, 1.0)));
    ctx.spotLightDirection = glm::normalize(lightaim - ctx.spotLightPosition);
    ctx.programs[2]->doMainLoop();
    lightaim = glm::vec3(0, 0, -3);
    ctx.spotLightPosition = glm::vec3(0, 0.5, -1.0);

    for(int i =16;i>=0;i--){
          ctx.objects[i]->transformMatrix = matrixStack.top();
          matrixStack.pop();
    }

#ifdef __APPLE__
    // Some platform need explicit glFlush
    glFlush();
#endif
    glfwSwapBuffers(window);
  }
  return 0;
}

void keyCallback(GLFWwindow* window, int key, int, int action, int) {

  if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_REPEAT){
    hold_space = GL_TRUE;
    if (pitch < 15.0f && activate) {
      pitch += Angle_speed_press*0.8*deltaTime;
    }
  }
  if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_REPEAT){
    if (yaw < 15.0f) {
      yaw += Angle_speed_press*deltaTime;
    }
    hold_left = GL_TRUE;
  }
  if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_REPEAT){
    if (yaw > -15.0f){
      yaw -= Angle_speed_press*deltaTime;
    }
    hold_right = GL_TRUE;
  }
  if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_REPEAT){
    hold_up = GL_TRUE;
  }
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_REPEAT) {
    hold_down = GL_TRUE;
    if (pitch > -15.0f && movement.y > 0.0f) {
      pitch -= Angle_speed_press * 0.8 * deltaTime;
    }
  }
  if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
    activate = !activate;
  }

  if(action == GLFW_RELEASE) {
    switch (key) {
      case GLFW_KEY_SPACE:
        hold_space = GL_FALSE;
        break;
      case GLFW_KEY_A:
        hold_left = GL_FALSE;
        break;
      case GLFW_KEY_D:
        hold_right = GL_FALSE;
        break;
      case GLFW_KEY_W:
        hold_up = GL_FALSE;
        break;
      case GLFW_KEY_S:
        hold_down = GL_FALSE;
        break;
      case GLFW_KEY_LEFT_ALT:
        FLYING_SPEED = ROTATE_SPEED / 20.f;
        break;
    }
  }

  // Press ESC to close the window.
  if (key == GLFW_KEY_ESCAPE) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
    return;
  }
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_F9: {
        if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
          // Show the mouse cursor
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
          // Hide the mouse cursor
          glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        break;
      }
      case GLFW_KEY_1:
        ctx.currentProgram = 0;
        break;
      case GLFW_KEY_2:
        ctx.currentProgram = 1;
        break;
      case GLFW_KEY_3:
        ctx.currentProgram = 2;
        break;
      case GLFW_KEY_0:
        ctx.currentProgram = 3;
        break;
      case GLFW_KEY_4:
        ctx.directionLightEnable = !ctx.directionLightEnable;
        break;
      case GLFW_KEY_5:
        ctx.pointLightEnable = !ctx.pointLightEnable;
        break;
      case GLFW_KEY_F:
        ctx.spotLightEnable = !ctx.spotLightEnable;
        break;
      case GLFW_KEY_H:
        ctx.pointLightColor = glm::vec3(0.8f, 0.8f, 0.8f);
        break;
      case GLFW_KEY_J:
        ctx.pointLightColor = glm::vec3(1.0f, 0.0f, 0.0f);
        break;
      case GLFW_KEY_C:
        ctx.spotLightColor = glm::vec3(0.8f, 0.8f, 0.8f);
        break;
      case GLFW_KEY_V:
        ctx.spotLightColor = glm::vec3(0.0f, 1.0f, 0.98f);
        break;
      default:
        break;
    }
  }
}

void resizeCallback(GLFWwindow* window, int width, int height) {
  OpenGLContext::framebufferResizeCallback(window, width, height);
  auto ptr = static_cast<Camera*>(glfwGetWindowUserPointer(window));
  if (ptr) {
    ptr->updateProjectionMatrix(OpenGLContext::getAspectRatio());
  }
}

void initOpenGL() {
  // Initialize OpenGL context, details are wrapped in class.
#ifdef __APPLE__
  // MacOS need explicit request legacy support
  OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
#else
  OpenGLContext::createContext(21, GLFW_OPENGL_ANY_PROFILE);
//  OpenGLContext::createContext(43, GLFW_OPENGL_COMPAT_PROFILE);
#endif
  GLFWwindow* window = OpenGLContext::getWindow();
  glfwSetKeyCallback(window, keyCallback);
  glfwSetFramebufferSizeCallback(window, resizeCallback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#ifndef NDEBUG
  OpenGLContext::printSystemInfo();
  // This is useful if you want to debug your OpenGL API calls.
  OpenGLContext::enableDebugCallback();
#endif
}