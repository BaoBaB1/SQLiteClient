#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "MainWindow.h"
#include "Session.h"
#include "../core/UiRenderer.h"
#include "../models/Customer.h"
#include "../models/Order.h"
#include "../models/OrderProduct.h"
#include "../models/Supplier.h"
#include "../models/Product.h"

void MainWindow::show()
{
  gladLoadGL();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard ;     // Enable Keyboard Controls
  ImGui_ImplGlfw_InitForOpenGL(m_window, true);
  ImGui_ImplOpenGL3_Init("#version 430");
  UiRenderer renderer;
  while (!glfwWindowShouldClose(m_window))
  {
    glfwPollEvents();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    int width, height;
    glfwGetWindowSize(m_window, &width, &height);
    // offset from menu bar 
    const float navbar_offset = 20.f;
    ImGui::SetNextWindowPos(ImVec2(0, navbar_offset));
    ImGui::SetNextWindowSize(ImVec2((float)width, height - navbar_offset));
    ImGui::Begin("Tables", nullptr, ImGuiWindowFlags_NoCollapse);
    renderer.render_menu_bar();
    if (ImGui::CollapsingHeader("Customers"))
      renderer.render_table<Customer>("Customers");
    if (ImGui::CollapsingHeader("Orders"))
      renderer.render_table<Order>("Orders");
    if (ImGui::CollapsingHeader("Suppliers"))
      renderer.render_table<Supplier>("Suppliers");
    if (ImGui::CollapsingHeader("Products"))
      renderer.render_table<Product>("Products");
    if (ImGui::CollapsingHeader("Order products"))
      renderer.render_table<OrderProduct>("Order products");
    ImGui::End();
    ImGui::Render();
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(m_window);
  }
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
  glfwTerminate();
}
