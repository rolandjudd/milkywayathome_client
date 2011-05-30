/*
Copyright (c) 2011 Matthew Arsenault
Copyright (c) 2011 Rensselaer Polytechnic Institute.

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

#include "nbody.h"
#include "nbody_priv.h"
#include "milkyway_util.h"
#include "nbody_grav.h"
#include "nbody_show.h"
#include "nbody_lua.h"
#include "milkyway_cpp_util.h"

#if USE_SHMEM
  #include <sys/shm.h>
#endif

#if USE_SHMEM

static void* createSharedMemory(key_t key, size_t size, int* shmIdOut)
{
    int shmId;
    void* p;

    shmId = shmget(key, size, IPC_CREAT | SHM_W | SHM_R);
    if (shmId < 0)
    {
        perror("Error getting shared memory");
        return NULL;
    }

    p = shmat(shmId, NULL, 0);
    if (!p || p == (void*) -1)
    {
        perror("Getting shared memory");
        return NULL;
    }

    memset(p, 0, size);

    if (shmIdOut)
        *shmIdOut = shmId;

    return p;
}

int createSharedScene(NBodyState* st, const char* inputFile)
{
    key_t key;
    size_t size;
    int shmId = -1;

    size = sizeof(scene_t) + st->nbody * sizeof(FloatPos);
    key = DEFAULT_SHMEM_KEY;
    //key = ftok(inputFile, getpid());
    if (key < 0)
    {
        key = DEFAULT_SHMEM_KEY;
        perror("Error getting key");
        return 1;
    }

    st->scene = (scene_t*) createSharedMemory(key, size, &shmId);
    if (!st->scene)
        return 1;

    st->shmId = shmId;
    st->scene->nbody = st->nbody;

    return 0;
}

int visualizerIsAttached(const NBodyState* st)
{
    struct shmid_ds buf;

    if (shmctl(st->shmId, IPC_STAT, &buf) < 0)
    {
        perror("Finding attached visualizers");
        return 0;
    }

    return buf.shm_nattch > 1;
}

#elif USE_BOINC_SHMEM

int createSharedScene(NBodyState* st, const char* inputFile)
{
    size_t size = sizeof(scene_t) + st->nbody * sizeof(FloatPos);

    st->scene = (scene_t*) mw_graphics_make_shmem(NBODY_BIN_NAME, (int) size);
    if (!st->scene)
    {
        warn("Failed to get shmem of size %d\n", (int) size);
        return 1;
    }

    memset(st->scene, 0, size);
    st->scene->nbody = st->nbody;

    return 0;
}

int visualizerIsAttached(const NBodyState* st)
{
    /* TODO */
    return 1;
}

#else

int createSharedScene(NBodyState* st, const char* inputFile)
{
    warn("Creating shared scene unimplemented for this system\n");
    return 0;
}

int visualizerIsAttached(const NBodyState* st)
{
    return 0;
}

#endif /* USE_SHMEM */


#ifndef _WIN32

void launchVisualizer(NBodyState* st)
{
    static const char* const argv[] = { NBODY_GRAPHICS_NAME, NULL };
    pid_t pid;
    char* path = NULL;
    char* newPath = NULL;

    if (!st->scene) /* If there's no scene to share, there's no point */
        return;

    pid = fork();
    if (pid != 0)  /* Parent */
        return;

    /* Child */

    /* Hack to close the shared memory access we inherit so we
     * don't count it when the visualizer actually opens it again */
    if (detachSharedScene(st))
    {
        warn("Error detaching child from shared");
        return;
    }

    /* Put places convenient for testing. Not essential, failure of
     * any of these is OK */
    path = getenv("PATH");
    if (!path)
    {
        perror("Error getting PATH");
    }
    else
    {
        if (asprintf(&newPath, ".:../bin/:%s", path) < 0)
        {
            perror("Appending to path");
        }
        else
        {
            if (setenv("PATH", newPath, TRUE) < 0)
            {
                perror("Error setting PATH");
            }
            free(newPath);
        }
    }

    if (execvp(argv[0], argv) < 0)
    {
        perror("Failed to launch visualizer");
    }
}

#else

void launchVisualizer(NBodyState* st)
{
    warn("Launching visualizer from main application not unimplemented on Windows\n");
}

#endif /* _WIN32 */


void updateDisplayedBodies(NBodyState* st)
{
    const Body* b;
    FloatPos* r;
    unsigned int i = 0;
    const unsigned int nbody = st->nbody;
    scene_t* scene = st->scene;

    if (!scene)
        return;

    r = scene->r;
    scene->usleepcount += scene->usleepdt;

    /* read data if not paused */
    if (scene->usleepcount >= scene->dt && (!scene->paused || scene->step == 1))
    {
        scene->usleepcount = 0.0;
        scene->step = 0;

      #ifdef _OPENMP
        #pragma omp parallel for private(i, b) schedule(static)
      #endif
        for (i = 0; i < nbody; ++i)
        {
            b = &st->bodytab[i];
            r[i].x = (float) X(Pos(b));
            r[i].y = (float) Y(Pos(b));
            r[i].z = (float) Z(Pos(b));
        }
    }

    scene->changed = TRUE;
}

