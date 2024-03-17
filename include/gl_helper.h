#pragma once

#include <glad/gl.h>
#include <vector>
using namespace std;
GLuint quickCreateProgram(const char* vert_shader_filename, const char* frag_shader_filename);

GLuint createShader(const char* filename, GLenum type);

GLuint createProgram(GLuint vert, GLuint frag);

GLuint createTexture(const char* filename);

constexpr GLenum getColorFormat(int channels);

void skyboxfromFile(const char* posx, const char* negx, const char* posy, const char* negy, const char* posz,
                    const char* negz, bool flip);

GLuint loadCubemap(vector<const GLchar*> faces,bool flip);

void checkGLError();