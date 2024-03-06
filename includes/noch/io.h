// SPDX-FileCopyrightText: 2023 Neel Basu
// SPDX-License-Identifier: BSD-3-Clause

#ifndef NCH_IO_H
#define NCH_IO_H

#include <cstdint>
#include <ostream>

#ifdef __SIZEOF_INT128__

// https://stackoverflow.com/a/72651639

std::ostream& operator<<(std::ostream& out, __uint128_t val);
std::ostream& operator<<(std::ostream& out, __int128_t val);

#endif // __SIZEOF_INT128__

#endif // NCH_IO_H
