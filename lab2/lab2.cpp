/*
 * ECEE 2160 Lab Assignment 2 main file
 *
 * Author:  Brian Schubert
 * Date:    2020-07-16
 *
 */

#include "linked_list.h"

int main() {

    LinkedList<int> z{};

    auto it = z.begin();

    z.insert(it, 7);
}