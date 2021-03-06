/**
 * ECEE 2160 Lab Assignment 4 - Generic seven-segment display I/O.
 *
 * Author:  Brian Schubert
 * Date:    2020-08-03
 *
 */

#include <cstdlib>              // for std::abs
#include <cctype>               // for std::isdigit, std::isxdigit
#include <stdexcept>            // for std::out_of_range, std::domain_error

template<std::size_t N, typename Reg>
void SevenSegmentDisplay<N, Reg>::update_display()
{
    for (std::size_t i{0}; i < k_register_count; ++i) {
        m_register_io->write_register(m_register_offsets[i], m_register_values[i].reg);
    }
}

template<std::size_t N, typename Reg>
void SevenSegmentDisplay<N, Reg>::clear_all()
{
    // Set all displays to zero.
    m_register_values = {};
    update_display();
}

template<std::size_t N, typename Reg>
[[maybe_unused]]
void SevenSegmentDisplay<N, Reg>::clear_display(std::size_t index)
{
    if (index >= N) {
        throw std::out_of_range("index must not exceed seven-segment display range");
    }

    access_display_unchecked(index) = 0;
    update_display();
}

template<std::size_t N, typename Reg>
[[maybe_unused]]
void SevenSegmentDisplay<N, Reg>::write_display_character(std::size_t index, char character)
{
    if (index >= N) {
        throw std::out_of_range("index must not exceed seven-segment display range");
    }

    if (!std::isxdigit(character)) {
        throw std::domain_error("character cannot be shown on seven segment display");
    }

    // Translate character to its index in the character value map.
    // Note: we assume ASCII encoding for characters. In general, it is not
    // guaranteed that the characters 'A' through 'F' are consecutive.
    auto value_index = static_cast<std::size_t>(
        std::isdigit(character)
            ? (character - '0')
            : (character >= 'a') ? (character - 'a') : (character - 'A')
    );

    // Set the target display to the given value.
    access_display_unchecked(index) = display_config::character_values[value_index];

    update_display();
}

template<std::size_t N, typename Reg>
void SevenSegmentDisplay<N, Reg>::print_hex(int number)
{
    const auto[min_value, max_value] = display_range();
    constexpr static int display_base = 16;

    // Check that the passed value can be shown on the display.
    if (number > max_value) {
        throw std::domain_error("number exceeds maximum display value");
    }
    if (number < min_value) {
        throw std::domain_error("number less than minimum display value");
    }

    // Set all displays to blank values.
    m_register_values = {};

    int pending = std::abs(number);
    std::size_t current_display{0};

    // Write an unsigned representation of the value to the displays.
    while (pending != 0) {
        const auto next_digit = pending % display_base;
        pending /= display_base;

        access_display_unchecked(current_display)
            = display_config::character_values[next_digit];

        ++current_display;
    }

    // If the original number was negative, write a negative sign to the
    // right-most unused display.
    if (number < 0) {
        access_display_unchecked(current_display) = display_config::negative_sign;
    }

    update_display();
}
