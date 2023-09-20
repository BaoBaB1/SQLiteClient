#pragma once


#include <glad/glad.h>
#include <GLFW/glfw3.h>

// #TODO: make destructor
class Window
{
public:
  virtual void show() = 0;
  virtual ~Window() { glfwDestroyWindow(m_window); }
  Window(int w, int h, const char* name);
  int width() const { return m_w; }
  int height() const { return m_h; }
  const char* name() const { return m_name; }
  const GLFWwindow* glfw_window() const { return m_window; }
protected:
  GLFWwindow* m_window;
  int m_w;
  int m_h; 
  const char* m_name;
};
