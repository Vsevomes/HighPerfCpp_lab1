#pragma once

#include <memory>
#include <string>

class AVLNode {
 public:
  int key_;
  std::string value_;
  int height_;
  std::unique_ptr<AVLNode> left_;
  std::unique_ptr<AVLNode> right_;

  AVLNode(int k, const std::string &v)
      : key_(k), value_(v), height_(1), left_(nullptr), right_(nullptr) {}
};