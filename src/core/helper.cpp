/*
 * helper.cpp
 *
 * Copyright (c) 2010-2011 Thomas Hisch <thomas at opentech dot at>.
 *
 * This file is part of StepperControl.
 *
 * StepperControl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * StepperControl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with StepperControl.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "helper.hpp"

namespace helper {
    std::string binary( unsigned long n )
    {
        char     result[ 4 + 1 ];
        unsigned index  = 4;
        result[ index ] = '\0';

        do {
            result[ --index ] = '0' + (n & 1);
            n >>= 1;
        } while (index);

        return std::string(result);
    }

    bool string_contains(const string& str, const char c)
    {
        return find_if(str.begin(), str.end(),
                       [&](char sc) { return sc == c; }) != str.end();
    }
};
