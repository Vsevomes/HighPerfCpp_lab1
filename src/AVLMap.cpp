#include "AVLMap.h"

#include <algorithm>
#include <iostream>

bool AVLMap::insert(int key, const std::string &value) {
  bool inserted = false;
  insertNode(root_, key, value, inserted);
  return inserted;
}

void AVLMap::upsert(int key, const std::string &value) {
  upsertNode(root_, key, value);
}

bool AVLMap::erase(int key) {
  bool erased = false;
  eraseNode(root_, key, erased);
  return erased;
}

std::optional<std::string> AVLMap::find(int key) const {
  return findNode(root_, key);
}

bool AVLMap::contains(int key) const { return containsNode(root_, key); }

void AVLMap::inorderPrint() const {
  inorderPrintNode(root_);
  std::cout << "\n";
}

int AVLMap::height(const std::unique_ptr<AVLNode> &n) {
  return n ? n->height_ : 0;
}

void AVLMap::updateHeight(std::unique_ptr<AVLNode> &n) {
  if (n) n->height_ = 1 + std::max(height(n->left_), height(n->right_));
}

int AVLMap::balanceFactor(const std::unique_ptr<AVLNode> &n) {
  return n ? (height(n->left_) - height(n->right_)) : 0;
}

void AVLMap::rotateRight(std::unique_ptr<AVLNode> &n) {
  std::unique_ptr<AVLNode> L = std::move(n->left_);
  n->left_ = std::move(L->right_);
  L->right_ = std::move(n);
  n = std::move(L);
  updateHeight(n->right_);
  updateHeight(n);
}

void AVLMap::rotateLeft(std::unique_ptr<AVLNode> &n) {
  std::unique_ptr<AVLNode> R = std::move(n->right_);
  n->right_ = std::move(R->left_);
  R->left_ = std::move(n);
  n = std::move(R);
  updateHeight(n->left_);
  updateHeight(n);
}

void AVLMap::rebalance(std::unique_ptr<AVLNode> &n) {
  if (!n) return;
  updateHeight(n);
  int bf = balanceFactor(n);

  if (bf > 1) {
    if (balanceFactor(n->left_) < 0) {
      rotateLeft(n->left_);
    }
    rotateRight(n);
  } else if (bf < -1) {
    if (balanceFactor(n->right_) > 0) {
      rotateRight(n->right_);
    }
    rotateLeft(n);
  } else {
  }
}

void AVLMap::insertNode(std::unique_ptr<AVLNode> &n, int key,
                        const std::string &value, bool &inserted) {
  if (!n) {
    n = std::make_unique<AVLNode>(key, value);
    inserted = true;
    return;
  }
  if (key < n->key_) {
    insertNode(n->left_, key, value, inserted);
  } else if (key > n->key_) {
    insertNode(n->right_, key, value, inserted);
  } else {
    inserted = false;
    return;
  }
  rebalance(n);
}

void AVLMap::upsertNode(std::unique_ptr<AVLNode> &n, int key,
                        const std::string &value) {
  if (!n) {
    n = std::make_unique<AVLNode>(key, value);
    return;
  }
  if (key < n->key_) {
    upsertNode(n->left_, key, value);
  } else if (key > n->key_) {
    upsertNode(n->right_, key, value);
  } else {
    n->value_ = value;
    return;
  }
  rebalance(n);
}

void AVLMap::popMin(std::unique_ptr<AVLNode> &n, int &out_key,
                    std::string &out_value) {
  if (!n->left_) {
    out_key = n->key_;
    out_value = n->value_;
    n = std::move(n->right_);
    return;
  }
  popMin(n->left_, out_key, out_value);
  rebalance(n);
}

void AVLMap::eraseNode(std::unique_ptr<AVLNode> &n, int key, bool &erased) {
  if (!n) {
    erased = false;
    return;
  }
  if (key < n->key_) {
    eraseNode(n->left_, key, erased);
  } else if (key > n->key_) {
    eraseNode(n->right_, key, erased);
  } else {
    erased = true;
    if (!n->left_) {
      n = std::move(n->right_);
      return;
    } else if (!n->right_) {
      n = std::move(n->left_);
      return;
    } else {
      int succ_key;
      std::string succ_val;
      popMin(n->right_, succ_key, succ_val);
      n->key_ = succ_key;
      n->value_ = std::move(succ_val);
    }
  }
  rebalance(n);
}

std::optional<std::string> AVLMap::findNode(const std::unique_ptr<AVLNode> &n,
                                            int key) {
  if (!n) return std::nullopt;
  if (key < n->key_) return findNode(n->left_, key);
  if (key > n->key_) return findNode(n->right_, key);
  return n->value_;
}

bool AVLMap::containsNode(const std::unique_ptr<AVLNode> &n, int key) {
  if (!n) return false;
  if (key < n->key_) return containsNode(n->left_, key);
  if (key > n->key_) return containsNode(n->right_, key);
  return true;
}

void AVLMap::inorderPrintNode(const std::unique_ptr<AVLNode> &n) {
  if (!n) return;
  inorderPrintNode(n->left_);
  std::cout << "(" << n->key_ << " : " << n->value_ << ") ";
  inorderPrintNode(n->right_);
}
