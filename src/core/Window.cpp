#include "Window.h"
#include <string>
#include <stdexcept>
#include <iostream>

static void setup_glfw_once();

Window::Window(int w, int h, const char* name) : m_w(w), m_h(h), m_name(name)
{
  setup_glfw_once();
  m_window = glfwCreateWindow(w, h, name, nullptr, nullptr);
  if (m_window == 0)
  {
    std::cerr << "glfwCreateWindow m_window == nullptr\n";
    throw std::runtime_error(std::string("Could not create window glfwCreateWindow") + name);
  }
  glfwMakeContextCurrent(m_window);
}

static void setup_glfw_once()
{
  static bool configured = false;
  if (!configured)
  {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    configured = true;
  }
}

