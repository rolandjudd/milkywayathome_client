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

#ifndef _SHOW_CL_TYPES_H_
#define _SHOW_CL_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <OpenCL/cl.h>
#include <OpenCL/cl_ext.h>

const char* showCLDeviceType(const cl_device_type x);
const char* showCLBuildStatus(const cl_build_status x);
const char* showCLInt(const cl_int x);

#ifdef __cplusplus
}
#endif

#endif /* _SHOW_CL_TYPES_H_ */

