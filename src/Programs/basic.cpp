#include <iostream>

#include "context.h"
#include "program.h"

bool BasicProgram::load() {
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
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    // TextureCoordinate
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  }
  return programId != 0;
}

void BasicProgram::doMainLoop() {
    glUseProgram(programId);
    int obj_num = (int)ctx->objects.size();
    for (int i = 0; i < obj_num; i++) {
        int modelIndex = ctx->objects[i]->modelIndex;
        glBindVertexArray(VAO[modelIndex]);

        Model* model = ctx->models[modelIndex];
        const float* p = ctx->camera->getProjectionMatrix();
        GLint pmatLoc = glGetUniformLocation(programId, "Projection");
        glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, p);

        const float* v = ctx->camera->getViewMatrix();
        GLint vmatLoc = glGetUniformLocation(programId, "ViewMatrix");
        glUniformMatrix4fv(vmatLoc, 1, GL_FALSE, v);

        const float* m = glm::value_ptr(ctx->objects[i]->transformMatrix * model->modelMatrix);
        GLint mmatLoc = glGetUniformLocation(programId, "ModelMatrix");
        glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, m);

        glUniform1i(glGetUniformLocation(programId, "ourTexture"), 0);
        glBindTexture(GL_TEXTURE_2D, model->textures[ctx->objects[i]->textureIndex]);
        glDrawArrays(model->drawMode, 0, model->numVertex);
    }
  
    glUseProgram(0);
}