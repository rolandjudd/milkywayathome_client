/*
Copyright 2008, 2009 Travis Desell, Dave Przybylo, Nathan Cole,
Boleslaw Szymanski, Heidi Newberg, Carlos Varela, Malik Magdon-Ismail
and Rensselaer Polytechnic Institute.

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


#ifndef _EVALUATION_STATE_H_
#define _EVALUATION_STATE_H_

#include <stdio.h>
#include "parameters.h"
#include "star_points.h"

typedef struct
{
    unsigned int mu_step, nu_step, r_step;
    unsigned int mu_steps, nu_steps, r_steps;
    double mu_min, nu_min, r_min;
    double mu_max, nu_max, r_max;
    double mu_step_size, nu_step_size, r_step_size;

    long min_calculation;
    long max_calculation;
    long current_calculation;

    unsigned int number_streams;
    double background_integral;
    double* stream_integrals;
} INTEGRAL_AREA;

typedef struct
{
    /* State for integral calculation. */
    INTEGRAL_AREA* integrals;
    double* ia_stream_integrals;  /* all of the stream_integrals used by INTEGRAL_AREAS */
    unsigned int number_integrals;
    unsigned int current_integral;
    unsigned int number_streams;

    double background_integral;
    double* stream_integrals;

    /* State for likelihood calculation. */
    unsigned int current_star_point;
    unsigned int total_stars;
    int num_zero;
    int bad_jacobians;
    double prob_sum;
} EVALUATION_STATE;

#define EMPTY_EVALUATION_STATE { NULL, NULL,    \
                                 0, 0, 0, \
                                 0.0, NULL, 0, 0, 0, 0, 0.0 }

void get_steps(INTEGRAL_AREA* ia, unsigned int* mu_step_current, unsigned int* nu_step_current, unsigned int* r_step_current);

void initialize_state(const ASTRONOMY_PARAMETERS* ap, const STAR_POINTS* sp, EVALUATION_STATE* es);
void free_state(EVALUATION_STATE* es);
void reset_evaluation_state(EVALUATION_STATE* es);

void fwrite_integral_area(FILE* file, INTEGRAL_AREA* ia);

int write_checkpoint(EVALUATION_STATE* es);
int read_checkpoint(EVALUATION_STATE* es);


#endif /* _EVALUATION_STATE_H_ */

