#pragma once

template<typename T>
class Singleton
{
public:
  static T& instance()
  {
    static T t;
    return t;
  }
  Singleton(const Singleton& other) = delete;
  Singleton& operator=(const Singleton& other) = delete;
private:
  Singleton() = default;
};
