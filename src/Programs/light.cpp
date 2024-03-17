#include <iostream>
#include "context.h"
#include "program.h"

bool LightProgram::load() {
  programId = quickCreateProgram(vertProgramFile, fragProgramFIle);
  int num_model = (int)ctx->models.size();
  VAO = new GLuint[num_model];

  glGenVertexArrays(num_model, VAO);
  for (int i = 0; i < num_model; i++) {
    glBindVertexArray(VAO[i]);

    Model* model = ctx->models[i];
    // combine positions, normals, textures to one vector
    std::vector<float> combined;
    for (int j = 0; j < model->numVertex; j++) {
      combined.push_back(model->positions[j * 3]);
      combined.push_back(model->positions[j * 3 + 1]);
      combined.push_back(model->positions[j * 3 + 2]);
      combined.push_back(model->normals[j * 3]);
      combined.push_back(model->normals[j * 3 + 1]);
      combined.push_back(model->normals[j * 3 + 2]);
      combined.push_back(model->texcoords[j * 2]);
      combined.push_back(model->texcoords[j * 2 + 1]);
    }
    // generate and bind VBO
    GLuint VBO[1];
    glGenBuffers(1, VBO);
    // pass data to buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * combined.size(), combined.data(), GL_STATIC_DRAW);
    // set attributes
    // position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    // texture
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  }
  return programId != 0;
}

void LightProgram::doMainLoop() {
  glUseProgram(programId);
  int obj_num = (int)ctx->objects.size();
  for (int i = 0; i < obj_num; i++) {
    int modelIndex = ctx->objects[i]->modelIndex;
    glBindVertexArray(VAO[modelIndex]);

    // pass variables to shader
    // camera
    Model* model = ctx->models[modelIndex];
    Camera* camera = ctx->camera;
    Object* object = ctx->objects[i];
    setMat4("Projection", camera->getProjectionMatrix());
    setMat4("ViewMatrix", camera->getViewMatrix());
    glm::mat4 modelMatrix = object->transformMatrix * model->modelMatrix;
    glm::mat4 modelNormalMatrix = glm::transpose(glm::inverse(modelMatrix));
    setMat4("ModelMatrix", glm::value_ptr(modelMatrix));
    setMat4("ModelNormalMatrix", glm::value_ptr(modelNormalMatrix));
    setVec3("viewPos", camera->getPosition());

    // texture
    setInt("ourTexture", 0);
    glBindTexture(GL_TEXTURE_2D, model->textures[ctx->objects[i]->textureIndex]);

    // material
    Material material = ctx->objects[i]->material;
    setVec3("material.ambient", glm::value_ptr(material.ambient));
    setVec3("material.diffuse", glm::value_ptr(material.diffuse));
    setVec3("material.specular", glm::value_ptr(material.specular));
    setFloat("material.shininess", material.shininess);

    // directional light
    setInt("dl.enable", ctx->directionLightEnable);
    setVec3("dl.direction", glm::value_ptr(ctx->directionLightDirection));
    setVec3("dl.lightColor", glm::value_ptr(ctx->directionLightColor));

    // point light
    setInt("pl.enable", ctx->pointLightEnable);
    setVec3("pl.position", glm::value_ptr(ctx->pointLightPosition));
    setVec3("pl.lightColor", glm::value_ptr(ctx->pointLightColor));
    setFloat("pl.constant", ctx->pointLightConstant);
    setFloat("pl.linear", ctx->pointLightLinear);
    setFloat("pl.quadratic", ctx->pointLightQuardratic);

    // spot light
    setInt("sl.enable", ctx->spotLightEnable);
    setVec3("sl.position", glm::value_ptr(ctx->spotLightPosition));
    setVec3("sl.direction", glm::value_ptr(ctx->spotLightDirection));
    setVec3("sl.lightColor", glm::value_ptr(ctx->spotLightColor));
    setFloat("sl.cutOff", ctx->spotLightCutOff);
    setFloat("sl.constant", ctx->spotLightConstant);
    setFloat("sl.linear", ctx->spotLightLinear);
    setFloat("sl.quadratic", ctx->spotLightQuardratic);

    glDrawArrays(model->drawMode, 0, model->numVertex);
  }
  glUseProgram(0);
}


bool skyboxProgram::load() {
  programId = quickCreateProgram(vertProgramFile, fragProgramFIle);

  GLfloat skyboxVertices[] = {// Positions
                              -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f,
                              1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f,

                              -1.0f, -1.0f, 1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  -1.0f,
                              -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f, 1.0f,

                              1.0f,  -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,
                              1.0f,  1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f,

                              -1.0f, -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,  1.0f,  1.0f,
                              1.0f,  1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f, -1.0f, 1.0f,

                              -1.0f, 1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  1.0f,
                              1.0f,  1.0f,  1.0f,  -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f,  -1.0f,

                              -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, -1.0f,
                              1.0f,  -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,  1.0f,  -1.0f, 1.0f};
  GLuint skyboxVBO[1];
  skyboxVAO = new GLuint[1];
  glGenVertexArrays(1, skyboxVAO);
  glGenBuffers(1, skyboxVBO);
  glBindVertexArray(skyboxVAO[0]);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO[0]);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
  glBindVertexArray(0);

  return programId != 0;
}

void skyboxProgram::doMainLoop() {
  // Check for linking errors
  GLint success;
  glGetProgramiv(programId, GL_LINK_STATUS, &success);
  if (!success) {
      GLchar infoLog[512];
      glGetProgramInfoLog(programId, 512, NULL, infoLog);
      std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }

  glDepthMask(GL_FALSE);
  glDepthFunc(GL_LEQUAL);
  glUseProgram(programId);
  Camera* camera = ctx->camera;
  setMat4("projection", camera->getProjectionMatrix());
  setMat4("view", camera->getViewMatrix());
  glBindVertexArray(skyboxVAO[0]);
  glActiveTexture(GL_TEXTURE0);
  setInt("skybox", 0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, ctx->skymap);
  glDrawArrays(GL_TRIANGLES, 0, 36);

  glBindVertexArray(0);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
  glUseProgram(0);
}