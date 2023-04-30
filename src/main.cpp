#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp> 


const float cubeStrip[] = {
	1, 1, 0,
	0, 1, 0,
	1, 1, 1,
	0, 1, 1,
	0, 0, 1,
	0, 1, 0,
	0, 0, 0,
	1, 1, 0,
	1, 0, 0,
	1, 1, 1,
	1, 0, 1,
	0, 0, 1,
	1, 0, 0,
	0, 0, 0
};

constexpr int n_triangles = sizeof(cubeStrip) / sizeof(float) / 3;


// utility functions

// global variables


double prev_xpos = 0.0;
double prev_ypos = 0.0;
bool left_mouse_pressed = false;

void mouse_button_callback(GLFWwindow* window,
                           int button,
                           int action,
                           int mods) {
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);

  int width, height;
  glfwGetWindowSize(window, &width, &height);

  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    // check if press is within volume rendering box
    if (xpos > width / 2 && ypos < height / 2) {
      prev_xpos = xpos;
      prev_ypos = ypos;
      left_mouse_pressed = true;
    }

  } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
    left_mouse_pressed = false;
  }
}

int main(void) {
  std::cout << sizeof(GLubyte) << " " << sizeof(unsigned char) << "\n";
  glfwSetErrorCallback(error_callback);

  if (!glfwInit())
    exit(EXIT_FAILURE);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window =
      glfwCreateWindow(1280, 960, "msiXplore", NULL, NULL);
  if (!window) {
    glfwTerminate();
    exit(EXIT_FAILURE);
  }

  glfwSetKeyCallback(window, key_callback);
  glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetScrollCallback(window, scroll_callback);

  glfwMakeContextCurrent(window);
  gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  glfwSwapInterval(1);

  

  // Setup GL state
  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

  float t0 = (float)glfwGetTime();

  while (!glfwWindowShouldClose(window)) {
    auto t = (float)glfwGetTime();
    auto dt = t - t0;
    t0 = t;

    glfwPollEvents();    

    // OpenGL rendering code
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    const float ratio = width / (float)height;

    glViewport(0, 0, width, height);
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);    

    glm::mat4 p =
        glm::perspective(glm::pi<float>() * 60.f / 180.f, ratio, 0.1f, 100.f);



    glfwSwapBuffers(window);    

    // cap framerate
    if(float elapsed = (float)glfwGetTime() - t; elapsed < 1e-2) {
      auto s = static_cast<size_t>((1e-2 - elapsed) * 1000.f);
      std::this_thread::sleep_for(std::chrono::milliseconds(s));      
    }
  }

  glfwDestroyWindow(window);

  glfwTerminate();
}

//! [code]
