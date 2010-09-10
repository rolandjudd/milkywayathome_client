/* Copyright 2010 Matthew Arsenault, Travis Desell, Boleslaw
Szymanski, Heidi Newberg, Carlos Varela, Malik Magdon-Ismail and
Rensselaer Polytechnic Institute.

This file is part of Milkway@Home.

Milkyway@Home is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Milkyway@Home is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Milkyway@Home.  If not, see <http://www.gnu.org/licenses/>.
*/

#if !defined (_MILKYWAY_MATH_H_INSIDE_) && !defined(MILKYWAY_MATH_COMPILIATION)
#error "Only milkyway_math.h can be included directly."
#endif

#ifndef _MILKYWAY_MATH_FUNCTIONS_H_
#define _MILKYWAY_MATH_FUNCTIONS_H_

#ifdef __OPENCL_VERSION__
  #include "milkyway_math_functions_cl.h"
#else
  #include "milkyway_math_functions_standard.h"
#endif /* __OPENCL_VERSION__ */

#endif /* _MILKYWAY_MATH_FUNCTIONS_H_ */
