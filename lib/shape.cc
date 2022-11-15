#include <iostream>

#include "shape.h"
#include <fmt/core.h>


Rectangle::Rectangle(int width, int height) : width_(width), height_(height) {}

int Rectangle::GetSize() const {
  // 직사각형의 넓이를 리턴한다.
  fmt::print("Width : {0} \n Height: {1}", width_, height_);
  return width_ * height_;
}