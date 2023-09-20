#pragma once

#include "Window.h"
#include "Singleton.h"

class MainWindow : public Window
{
public:
  static MainWindow& instance() { return Singleton<MainWindow>::instance(); }
  friend class Singleton<MainWindow>;
  void show() override;
private:
  MainWindow() : Window(720, 480, "Main window") {}
};
