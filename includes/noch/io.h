// SPDX-FileCopyrightText: 2023 Neel Basu
// SPDX-License-Identifier: BSD-3-Clause

#ifndef NCH_IO_H
#define NCH_IO_H

#include <ostream>

#ifdef __SIZEOF_INT128__

inline std::ostream& operator<<(std::ostream& stream, unsigned __int128 value){
    unsigned __int128 upper = static_cast<unsigned __int128>(value >> 64);
    unsigned __int128 lower = static_cast<unsigned __int128>(value);

    std::ios_base::fmtflags f(stream.flags());
    stream << std::hex << upper << lower;
    stream.flags(f);
    return stream;
}

#endif

#endif // NCH_IO_H
