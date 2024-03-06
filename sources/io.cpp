#include "noch/io.h"
#include <ostream>

#ifdef __SIZEOF_INT128__

// https://stackoverflow.com/a/72651639
// Write the 128-bit integer val to out, with a minus sign if decimal and neg
// is true. Obey all of the ostream settings of out for integer display: octal
// or hexadecimal, upper case letters, plus sign, fill character and width, and
// fill placement.
static void out128(std::ostream& out, __uint128_t val, int neg) {
    // Note if the number is zero. (No hex or octal prefix in this case.)
    auto zero = val == 0;

    // Note if upper-case letters requested.
    auto state = out.flags();
    auto upper = (state & std::ios_base::uppercase) != 0;

    // Set base for digits.
    unsigned base = state & std::ios_base::hex ? 16 :
                    state & std::ios_base::oct ? 8 :
                    10;

    // Space for digits and prefix. Generate digits starting at the end of the
    // string, going backwards. num will be the digit string. Terminate it.
    char str[47];
    auto end = str + sizeof(str), num = end;
    *--num = 0;

    // Compute and place digits in base base.
    do {
        char dig = val % base;
        val /= base;
        dig += dig < 10 ? '0' : (upper ? 'A' : 'a') - 10;
        *--num = dig;
    } while (val);

    // Prepend octal number with a zero if requested.
    if (state & std::ios_base::showbase && base == 8 && !zero)
        *--num = '0';

    // pre will be the prefix string. Terminate it.
    auto pre = num;
    *--pre = 0;

    // Put a plus or minus sign in the prefix as appropriate.
    if (base == 10) {
        if (neg)
            *--pre = '-';
        else if (state & std::ios_base::showpos)
            *--pre = '+';
    }

    // Prefix a hexadecimal number if requested.
    else if (state & std::ios_base::showbase && base == 16 && !zero) {
        *--pre = upper ? 'X' : 'x';
        *--pre = '0';
    }

    // Compute the number of pad characters and get the fill character.
    auto len = (num - pre) + (end - num) - 2;
    auto pad = out.width();
    out.width(0);
    pad = pad > len ? pad - len : 0;
    char fill = out.fill();

    // Put the padding before prefix if neither left nor internal requested.
    if (!(state & (std::ios_base::internal | std::ios_base::left)))
        while (pad) {
            out << fill;
            pad--;
        }

    // Write prefix.
    out << pre;

    // Put the padding between the prefix and the digits if requested.
    if (state & std::ios_base::internal)
        while (pad) {
            out << fill;
            pad--;
        }

    // Write digits.
    out << num;

    // Put number to the left of padding, if requested.
    if (state & std::ios_base::left)
        while (pad) {
            out << fill;
            pad--;
        }
}

// Overload << for an unsigned 128-bit integer.
std::ostream& operator<<(std::ostream& out, __uint128_t val) {
    out128(out, val, 0);
    return out;
}

// Overload << for a signed 128-bit integer. Negation of the most negative
// signed value gives the correct unsigned absolute value.
std::ostream& operator<<(std::ostream& out, __int128_t val) {
    auto state = out.flags();
    if (val < 0 && !(state & (std::ios_base::hex | std::ios_base::oct)))
        out128(out, (__uint128_t)-val, 1);
    else
        out128(out, (__uint128_t)val, 0);
    return out;
}

#endif // __SIZEOF_INT128__
