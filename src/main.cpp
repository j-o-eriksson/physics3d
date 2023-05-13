#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/ext.hpp>
#include <glm/glm.hpp>
#include "../ext/glfw/deps/linmath.h"

#include <gl_utilities/cube_mesh.hpp>
#include <shader/phong_shader.hpp>
#include <particle.hpp>

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

static void error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window,
                         int key,
                         int scancode,
                         int action,
                         int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main(void) {
  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL Triangle", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, key_callback);

  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glfwSwapInterval(1);

  auto phong_shader = graphics::shader::phong_shader{};
  auto [phong_vao, matrix_buffer_object] =
      graphics::utilities::make_cube_mesh_arrays(1.f, 1.f, 1.f);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);
  glDepthRange(0.0f, 1.0f);

  while (!glfwWindowShouldClose(window)) {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    const float ratio = width / (float)height;
    glViewport(0, 0, width, height);
    glm::mat4 proj_matrix =
        glm::perspective(glm::pi<float>() * 60.f / 180.f, ratio, 0.1f, 100.f);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto matrix_buffer = std::vector<glm::mat4>{glm::mat4(1.f)};
    matrix_buffer[0][3] = glm::vec4{0.f, 0.f, -3.f, 1.f};
    graphics::utilities::update_matrix_buffer(matrix_buffer_object, matrix_buffer);

    phong_shader.bind();
    phong_shader.set_projection_matrix(proj_matrix);

    glBindVertexArray(phong_vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, matrix_buffer.size());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);

  glfwTerminate();
  exit(EXIT_SUCCESS);
}

//! [code]
