#pragma once

#include <memory>
#include <optional>
#include <string>

#include "AVLNode.h"

class AVLMap {
 public:
  AVLMap() = default;
  ~AVLMap() = default;

  // disable copy, enable move
  AVLMap(const AVLMap &) = delete;
  AVLMap &operator=(const AVLMap &) = delete;
  AVLMap(AVLMap &&) = default;
  AVLMap &operator=(AVLMap &&) = default;

  bool insert(int key, const std::string &value);
  void upsert(int key, const std::string &value);
  bool erase(int key);

  std::optional<std::string> find(int key) const;
  bool contains(int key) const;
  
  void inorderPrint() const;

 private:
  std::unique_ptr<AVLNode> root_;

  static int height(const std::unique_ptr<AVLNode> &n);
  static void updateHeight(std::unique_ptr<AVLNode> &n);
  static int balanceFactor(const std::unique_ptr<AVLNode> &n);

  static void rotateRight(std::unique_ptr<AVLNode> &n);
  static void rotateLeft(std::unique_ptr<AVLNode> &n);
  static void rebalance(std::unique_ptr<AVLNode> &n);

  static void insertNode(std::unique_ptr<AVLNode> &n, int key,
                         const std::string &value, bool &inserted);
  static void upsertNode(std::unique_ptr<AVLNode> &n, int key,
                         const std::string &value);
  static void eraseNode(std::unique_ptr<AVLNode> &n, int key, bool &erased);

  static void popMin(std::unique_ptr<AVLNode> &n, int &out_key,
                     std::string &out_value);

  static std::optional<std::string> findNode(const std::unique_ptr<AVLNode> &n,
                                             int key);
  static bool containsNode(const std::unique_ptr<AVLNode> &n, int key);
  static void inorderPrintNode(const std::unique_ptr<AVLNode> &n);
};
