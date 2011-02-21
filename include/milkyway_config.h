/* Copyright 2010 Matthew Arsenault, Travis Desell, Dave Przybylo,
Nathan Cole, Boleslaw Szymanski, Heidi Newberg, Carlos Varela, Malik
Magdon-Ismail and Rensselaer Polytechnic Institute.

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

#ifndef _MILKYWAY_CONFIG_H_
#define _MILKYWAY_CONFIG_H_

#define BOINC_APPLICATION 0
#define BOINC_APP_GRAPHICS 0
#define DOUBLEPREC 1
#define MW_ENABLE_DEBUG 0
#define MILKYWAY_OPENCL 0

#define HAVE_EXP10 0
#define HAVE_SINCOS 0
#define HAVE_TGMATH_H 1

#if ENABLE_FDLIBM
/* These actually aren't in crlibm. FIXME: Check function exists in specific math library */
  #undef HAVE_EXP10
  #undef HAVE_SINCOS
  #undef HAVE_TGMATH_H
#endif

/* C99 Restrict.

   For MSVC, just defining restrict to __restrict doesn't work because
   its preprocessor is stupid, and for some reason replaces the
   restrict in __declspec(restrict) in the MSVC standard headers.
 */
#ifdef _MSC_VER
  #define RESTRICT __restrict
  #define FUNC_NAME __FUNCTION__
  #define inline __inline
#else
  #define RESTRICT restrict
  #define FUNC_NAME __func__
#endif /* _MSC_VER */


#ifndef _MSC_VER
  #if __GNUC__ >= 4 && __GNUC_MINOR__ >= 3
    #define HOT __attribute__((hot))
  #else
    #define HOT
  #endif

  #define CONST_F __attribute__((const))
  #define ALWAYS_INLINE __attribute__((always_inline))
#else
  #define HOT
  #define CONST_F
  #define ALWAYS_INLINE
#endif /* _MSC_VER */


/* Apple's GCC 4.2 has many features backported, so it counts as a newer GCC even though it isn't. */
#if defined(__APPLE__) && (__GNUC__ >= 4 && __GNUC_MINOR__ >= 2)
  #define _MW_NEW_APPLE_GCC
#endif

/* GCC 4.3 switched to C99 compliant inlining with -std=c99/gnu99 */
/* clang also reports itself as compatibile with GCC 4.2.1, and
 * llvm-gcc 4.2.1 also works. */
#if (__GNUC__ <= 4 && __GNUC_MINOR__ < 3) && !defined(_MW_NEW_APPLE_GCC) && !defined(__clang__) && !defined(__llvm__)
  #define OLD_GCC_EXTERNINLINE extern
#else
  #define OLD_GCC_EXTERNINLINE
#endif


#if DOUBLEPREC
  #define PRECSTRING "double"
#else
  #define PRECSTRING "float"
#endif

#if DISABLE_DENORMALS
  #define DENORMAL_STRING " no denormals"
#else
  #define DENORMAL_STRING ""
#endif


/* x86_64 seems to always give i386 even on OS X 64 */

#ifndef __OPENCL_VERSION__
/* FIXME: This would be totally wrong when using a universal binary */
#if defined(__i386__) || defined(_M_IX86)
  #define ARCH_STRING "x86"
#elif defined(__powerpc__) || defined(__ppc__) || defined(_M_PPC)
  #define ARCH_STRING "PPC"
#elif defined(__x86_64__) || defined(_M_X64)
  #define ARCH_STRING "x86_64"
#elif defined(__ppc64__)
  #define ARCH_STRING "PPC64"
#else
  #define ARCH_STRING "Unknown architecture"
  #warning "Unknown architecture!"
#endif


#if DOUBLEPREC
  #ifdef __SSE2__
    #define HAVE_SSE_MATHS 1
  #endif
#else
  #ifdef __SSE__
    #define HAVE_SSE_MATHS 1
  #endif
#endif /* DOUBLEPREC */

#if !HAVE_SSE_MATHS && defined(_WIN32) && !defined(_WIN64)
  /* 32 bit Windows without SSE2 */
  #define WINDOWS_USES_X87 1
#endif

#endif /* __OPENCL_VERSION__ */

#endif /* _MILKYWAY_CONFIG_H_ */
