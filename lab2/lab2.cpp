/*
 * ECEE 2160 Lab Assignment 2 main file
 *
 * Author:  Brian Schubert
 * Date:    2020-07-16
 *
 * References
 * ==========
 *
 *  - https://en.cppreference.com/w/cpp/algorithm/find
 *  - https://en.cppreference.com/w/cpp/iterator/advance
 *  - https://en.cppreference.com/w/cpp/container/multiset
 *  - https://en.cppreference.com/w/cpp/language/pointer
 *  - https://en.cppreference.com/w/cpp/utility/functional/less
 *
 */

#include "linked_list.h"

#include <algorithm>    // for std::find
#include <array>        // for std::array (used in menu)
#include <functional>   // for std::less
#include <iostream>     // for std::cout, std::cin
#include <limits>       // for std::numeric_limits
#include <set>          // for std::multiset (used for sorting list)
#include <string_view>  // for std::string_view



// For access to string view literals
using std::string_view_literals::operator ""sv;

// Using anonymous namespace to given symbols internal linkage.
namespace {

/// Program menu from lab instructions.
constexpr auto PRELAB_MENU = std::array{
    "Add a person"sv,
    "Find a person"sv,
    "Remove a person"sv,
    "Print the list"sv,
    "Sort the list"sv,
    "Exit"sv,
};

/**
 * Person structure from the instructor-provided personList.cpp file.
 */
struct Person {
    /// This person's unique identifier.
    int id;
    /// This person's age.
    int age;
    /// This person's name.
    std::string name;
};

// We use aggregate initialization for Person in run_list_interactive.
static_assert(std::is_aggregate<Person>::value, "Person is expected to be an aggregate");

/**
 * Runs a continuous interactive program that allows the user to manipulate
 * the provided list.
 *
 * @param list The list to be interacted with.
 */
void run_list_interactive(LinkedList<Person>& list);

/**
 * Prints the specified prompt to the standard output and reads a T value
 * from the standard input.
 *
 * If the user provides an invalid T value, the standard input will be cleared
 * and the user will be reprompted.
 *
 * @tparam T The type of the object to read.
 * @param prompt The message to be displayed to the user.
 * @return User provided T value.
 */
template<typename T>
T prompt_user(std::string_view prompt);

/**
 * Output stream operator for Person.
 */
std::ostream& operator<<(std::ostream& out, const Person& p);

/**
 * Output stream operator for LinkedList<Person>.
 *
 * We accept list as a non-const reference since we did not implement
 * constant iterators for this lab.
 */
std::ostream& operator<<(std::ostream& out, /*const*/ LinkedList<Person>& list);

/**
 * Sorts the given list based on the ordering described by the given
 * comparison function.
 *
 * We accept list as a non-const reference since we did not implement
 * constant iterators for this lab.
 *
 * @tparam T List content type.
 * @param list List to be sorted
 * @param comparison_func Function object that returns `true` if the first
 *                        argument is less than the second argument.
 * @returns Sorted list.
 */
template<typename T, typename Compare>
LinkedList<T> sorted_list(/*const*/ LinkedList<T>& list, Compare comparison_func);

/**
 * Helper function for creating lambdas that compare two T instances
 * based on a member variable designated by a member pointer.
 *
 * Two lambdas of this form are used in the extra credit portion of this
 * lab, so we decided to lift this pattern into a constexpr function.
 *
 * Automatic return type deduction is used since the return value is of
 * a unique type for the lambda expression.
 *
 * We decided to further generalize this function by also templating it over
 * the function used to compare the members variables. This style was inspired
 * by inspecting the source for std::multiset.
 *
 * The template parameter Compare is defaulted to the class std::less<Member>
 * to match the convention used by STL algorithms (e.g. std::sort,
 * std::multiset).
 *
 * @tparam T The type of values that the produced function will compare.
 * @tparam Member The type of the member variable of T that the produced function
 *           will compare.
 * @tparam Compare The type of the comparison function used to compare the selected
 *           member variables.
 * @param member The member pointer.
 * @return Function object for comparing T instances based on a member variable.
 */
template<typename T, typename Member, typename Compare = std::less<Member>>
constexpr auto make_compare_by_member(const Member T::* member, Compare compare = Compare())
{
    return [member, compare](const T& lhs, const T& rhs) -> bool {
        return compare(lhs.*member, rhs.*member);
    };
}

} // end namespace

int main()
{
    // Default construct list.
    LinkedList<Person> list{};

    run_list_interactive(list);
}


// Internal definitions.
namespace {
void run_list_interactive(LinkedList<Person>& list)
{
    // Lambda to generate ID's for each person added to the list.
    const auto person_id_generator = []() {
        static int last_id{0};
        return ++last_id;
    };

    while (true) {
        // Print the menu from the lab instructions.
        std::cout << "====== Menu ======\n";
        for (std::size_t i{0}; i < PRELAB_MENU.size(); ++i) {
            std::cout << (i + 1) << ". " << PRELAB_MENU[i] << '\n';
        }
        std::cout << '\n';

        // Prompt user for menu selection.
        const auto user_selection = prompt_user<int>("Select an option: ");

        switch (user_selection) {
            case 1: { // Add a person.
                const auto name = prompt_user<std::string>("Enter the person's name: ");
                const auto age = prompt_user<int>("Enter the person's age: ");

                // A move overload is not not implemented for LinkedList::push_front,
                // so this call results in a deep copy of the temporary.
                // However, since the user-provided names are likely going to be
                // very short, moves and copies should be equivalent, owing to
                // short string optimization in the GNU C++ STL.
                list.push_front(Person{ // Aggregate initialize Person.
                    person_id_generator(),
                    age,
                    name
                });
                break;
            }
            case 2: { // Find a person.
                const auto needle_id = prompt_user<int>("Enter the ID to search for: ");

                // Search for person in O(n) time.
                const auto loc = std::find_if(list.begin(), list.end(),
                    [needle_id](const Person& p) { return p.id == needle_id; }
                );

                if (loc != list.end()) {
                    std::cout << "Found " << *loc << '\n';
                } else {
                    std::cout << "No person with ID=" << needle_id << " found.\n";
                }

                break;
            }
            case 3: { // Remove a person.
                const auto needle_id = prompt_user<int>("Enter the ID of the person to delete: ");

                // Search for person in O(n) time.
                const auto loc = std::find_if(list.begin(), list.end(),
                    [needle_id](const Person& p) { return p.id == needle_id; }
                );

                if (loc != list.end()) {
                    // Unfortunately, we can only remove elements that appear after
                    // an iterator, so we need to manual acquire an iterator to
                    // the element one slot prior.
                    auto removal_iter = list.before_begin();
                    // Advance the iterator to just before the element to remove.
                    // Runs in O(n) time.
                    std::advance(removal_iter, std::distance(list.begin(), loc));
                    list.remove_after(removal_iter);
                } else {
                    std::cout << "No person with ID=" << needle_id << " found.\n";
                }

                break;
            }
            case 4: { // Print the list.
                std::cout << list << '\n';
                break;
            }
            case 5: { // Sort the list
                std::cout << "Sort by\n 0) Name\n 1) Age\n";
                // Only the strings "0" and "1" are accepted as valid bool
                // inputs, so the user cannot select an invalid menu option.
                const auto selection = prompt_user<bool>("Selection: ");

                if (selection == 0) {
                    // Sort the list by comparing names.
                    // We move the new list into "list" by invoking the move assignment
                    // operator. The old list, and all of its elements, will now be owned by
                    // the temporary and will be destroyed at the end of this statement.
                    list = sorted_list(list, make_compare_by_member(&Person::name));
                } else { // selection == 1
                    // Sort the list by comparing ages.
                    // See comment on move assignment in other branch.
                    list = sorted_list(list, make_compare_by_member(&Person::age));
                }
                break;
            }
            case 6: { // Exit
                std::cout << "Exiting...\n";
                return;
            }
            default: {
                // Print and immediately flush error message.
                std::cout
                    << "Invalid selection - selection must be an integer from [1,"
                    << PRELAB_MENU.size() << ']' << std::endl;
            }
        }

        std::cout << '\n';

    }
}

template<typename T>
T prompt_user(const std::string_view prompt)
{
    T user_selection;
    while (true) {
        std::cout << prompt;
        // operator>>(std::istream&, T) returns false on input failure.
        // We assume EOF will not be reached.
        if (!(std::cin >> user_selection)) {
            // Print and immediately flush error message.
            std::cout << "Invalid input" << std::endl;
            // Clear the error
            std::cin.clear();
            // Empty the input stream until a newline character is found.
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        return user_selection;
    }
}

std::ostream& operator<<(std::ostream& out, const Person& p)
{
    out << "Person { id=" << p.id
        << ", name=\"" << p.name
        << "\", age=" << p.age << '}';

    return out;
}

std::ostream& operator<<(std::ostream& out, /*const*/ LinkedList<Person>& list)
{
    for (const auto& elem : list) {
        out << " - " << elem << '\n';
    }
    return out;
}

template<typename T, typename Compare>
LinkedList<T> sorted_list(/*const*/ LinkedList<T>& list, Compare comparison_func)
{
    // Use std::multiset with the given comparison function to construct a
    // binary tree based on the element orderings. For sufficiently long lists,
    // this should be faster than placing all of the list entries in a vector
    // and then sorting (though have not we run any benchmarks to test this).
    const std::multiset<T, Compare> binary_tree(list.begin(), list.end(), comparison_func);

    // Copy the sorted elements into a new list using the range constructor.
    LinkedList<T> new_list(binary_tree.cbegin(), binary_tree.cend());

    return new_list;
}
} // end namespace
