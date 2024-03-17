#include "gl_helper.h"
#include <fstream>
#include <iostream>
#include <string>
#include <array>

using namespace std;
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::string readFile(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return "";
    }

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

GLuint quickCreateProgram(const char* vert_shader_filename, const char* frag_shader_filename) {
  GLuint vert = createShader(vert_shader_filename, GL_VERTEX_SHADER);

  if (vert == 0) return 0;

  GLuint frag = createShader(frag_shader_filename, GL_FRAGMENT_SHADER);

  if (frag == 0) {
    glDeleteShader(vert);
    return 0;
  }

  GLuint prog = createProgram(vert, frag);
  if (prog == 0) {
    glDeleteShader(vert);
    glDeleteShader(frag);
    return 0;
  }
  return prog;
}


GLuint createShader(const char* filename, GLenum type) {
  // Read shader code
  char* buffer = 0;
  long length;
  std::ifstream infile(filename, std::ios::binary);
  if (!infile.is_open()) {
    std::cout << "Open file fail: " << filename << std::endl;
    return 0;
  }
  infile.seekg(0, std::ios::end);
  length = (long)infile.tellg();
  infile.seekg(0, std::ios::beg);
  buffer = (char*)malloc(length + 1);
  buffer[length] = 0;
  if (buffer == NULL) {
    std::cout << "Allocate memory fail" << std::endl;
    infile.close();
    return 0;
  }
  infile.read(buffer, length);
  infile.close();

  // Compile shader
  int success;
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, (const GLchar**)&buffer, 0);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(shader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    glDeleteShader(shader);
    shader = 0;
  };

  free(buffer);
  return shader;
}

GLuint createProgram(GLuint vert, GLuint frag) {
  // shader Program
  GLuint prog = glCreateProgram();
  glAttachShader(prog, vert);
  glAttachShader(prog, frag);
  glLinkProgram(prog);
  // print linking errors if any
  int success;
  glGetProgramiv(prog, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(prog, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    glDeleteProgram(prog);
    return 0;
  }

  // Always detach shaders after a successful link.
  glDetachShader(prog, vert);
  glDetachShader(prog, frag);
  return prog;
}

GLuint createTexture(const char* filename) {
  GLuint texture;
  int width, height, nrChannels;
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
  if (data == NULL) {
    std::cout << "Failed to load texture " << filename << std::endl;
  }

  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(data);

  return texture;
}
constexpr GLenum getColorFormat(int channels) {
  switch (channels) {
    case 1:
      return GL_RED;
    case 2:
      return GL_RG;
    case 3:
      return GL_RGB;
    case 4:
      return GL_RGBA;
    default:
      std::cout << "Unknown color format!" << std::endl;
      std::cout << "Guess color: RGB." << std::endl;
      return GL_RGB;
  }
}
void skyboxfromFile(const char* posx, const char* negx, const char* posy, const char* negy, const char* posz,
                    const char* negz, bool flip) {
  std::array<const char*, 6> filenames{posx, negx, posy, negy, posz, negz};
  int width, height, nChannels;
  stbi_set_flip_vertically_on_load(flip);

  for (int i = 0; i < 6; ++i) {
    stbi_uc* data = stbi_load(filenames[i], &width, &height, &nChannels, STBI_default);
    if (data == nullptr) {std::cout<<"Failed to load texture file"<<std::endl;};
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, getColorFormat(nChannels),
                 GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
  }
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
 }

GLuint loadCubemap(vector<const GLchar*> faces, bool flip) {
  GLuint textureID;
  glGenTextures(1, &textureID);
  int width, height, nChannels;
  stbi_set_flip_vertically_on_load(flip);
  for (int i = 0; i < 6; ++i) {
    stbi_uc* data = stbi_load(faces[i], &width, &height, &nChannels, STBI_default);
    if (data == nullptr) { cout << "Failed to load texture file" << endl;}
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, getColorFormat(nChannels),
                 GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);
  }
  glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

  return textureID;
}
