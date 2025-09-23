#include "AVLMap.h"
#include <iostream>

int main() {
    AVLMap dict;

    dict.insert(10, "ten");
    dict.insert(20, "twenty");
    dict.insert(5, "five");
    dict.insert(15, "fifteen");

    std::cout << "After inserts: ";
    dict.inorderPrint();

    bool ok = dict.insert(10, "TEN");
    std::cout << "Insert existing key 10: " << (ok ? "inserted" : "not inserted") << "\n";

    dict.upsert(10, "TEN-updated");
    std::cout << "After upsert 10: ";
    dict.inorderPrint();

    dict.erase(15);
    std::cout << "After erase 15: ";
    dict.inorderPrint();

    auto val = dict.find(20);
    if (val) std::cout << "20 -> " << *val << "\n";

    return 0;
}
