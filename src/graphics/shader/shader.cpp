#include "shader.hpp"

#include <string>
#include <vector>
#include <optional>
#include <iostream>

namespace graphics::shader {

std::optional<std::string> get_shader_error(GLuint shader) {
  GLint isCompiled;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

  if (isCompiled == GL_FALSE) {
    GLint maxLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

    // The maxLength includes the NULL character
    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(shader, maxLength, &maxLength, errorLog.data());

    return std::string(errorLog.data());
  } else {
    return std::nullopt;
  }  
}

GLuint compile_program(const char* vert_src, const char* frag_src) {
  const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(vertex_shader, 1, &vert_src, NULL);
  glCompileShader(vertex_shader);

  const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &frag_src, NULL);
  glCompileShader(fragment_shader);

  // OpenGL error checks
  if (auto vert_error = get_shader_error(vertex_shader)) {
    std::cout << "vertex shader error: " << *vert_error << "\n";
  }
  if (auto frag_error = get_shader_error(fragment_shader)) {
    std::cout << "fragment shader error: " << *frag_error << "\n";
  }

  const GLuint program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  return program;
}


static const char* vertex_shader_text =
"#version 420\n"
"uniform mat4 MVP;\n"
"in vec3 vCol;\n"
"in vec2 vPos;\n"
"out vec3 color;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 0.0, 1.0);\n"
"    color = vCol;\n"
"}\n";

static const char* fragment_shader_text =
"#version 330\n"
"in vec3 color;\n"
"out vec4 fragment;\n"
"void main()\n"
"{\n"
"    fragment = vec4(color, 1.0);\n"
"}\n";

} // namespace graphics::shader
