/*
 * ECEE 2160 Lab Assignment 0 Utilities
 *
 * All of the utility functions defined in this header are templated, so no
 * implementation (.cpp) file is required.
 *
 * Author:  Brian Schubert
 * Date:    2020-07-01
 *
 * References
 * ==========
 *
 *  - http://www.cplusplus.com/reference/random/
 *  - http://www.cplusplus.com/reference/algorithm/
 *  - https://en.cppreference.com/w/cpp/named_req/ForwardIterator
 */

#ifndef ECEE_2160_LAB_REPORTS_LAB0_UTILS_H
#define ECEE_2160_LAB_REPORTS_LAB0_UTILS_H

#include <iostream>         // for std::ostream - we can't use iosfwd since this
                            // header includes definitions that write to ostream.
#include <string_view>      // for std::string_view
#include <utility>          // for std::swap

/**
 * Prints the elements of the given forward iterator to the given output
 * stream with elements separated by the given delimiter.
 *
 * The delimiter is taken as a std::string_view so that C-style strings,
 * std::string, and other string collection types can be use without requiring
 * manual conversions.
 *
 * This function is used in lab assignments 0.2 and 0.3.
 *
 * @tparam I Iterator type.
 * @param out Output stream to be written to.
 * @param iter Forward iterator indicating start location.
 * @param end Forward iterator indicating end location.
 * @param delim String view of output delimiter.
 */
template<class I>
void print_iter(std::ostream& out, I iter, I end, const std::string_view delim)
{
    // Make sure the received iterator isn't already at its end.
    if (iter == end) {
        return;
    }
    // Track the previously yielded element from the iterator while consuming.
    I prev{iter};

    // Advance the iterator and check to see if it has reached the end.
    while (++iter != end) {
        // If the end has not been reached, print the previous element with
        // a trailing delimiter.
        out << *prev << delim;
        // Update the previous element with the current element.
        prev = iter;
    }
    // Print the final element without a trailing delimiter.
    out << *prev;
}

/**
 * Sorts the given array of consecutive elements.
 *
 * This function implements the selection sort algorithm per the lab assignment
 * instructions, which occurs in O(1) space and O(n^2) time.
 *
 * This function is used in lab assignments 0.2 and 0.3.
 *
 * @tparam T Array content type.
 * @param values Mutable pointer to array contents.
 * @param size Array length.
 */
template<class T>
void selection_sort_array(T* values, std::size_t size)
{
    for (std::size_t i{0}; i < size; ++i) {
        // Store both the value and index of the  minimum unsorted element.
        T min_element{values[i]};
        std::size_t min_index{i};

        // Locate the index of the minimum element in the unsorted porition.
        for (std::size_t j{i + 1}; j < size; ++j) {
            if (values[j] < min_element) {
                min_element = values[j];
                min_index = j;
            }
        }

        // Swap the first unsorted element with the located minimum.
        std::swap(values[i], values[min_index]);
    }
}

#endif //ECEE_2160_LAB_REPORTS_LAB0_UTILS_H
