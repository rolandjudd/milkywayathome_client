/*
Copyright (C) 2011  Matthew Arsenault

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


#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "nbody_types.h"
#include "nbody_show.h"
#include "milkyway_lua.h"
#include "milkyway_util.h"
#include "nbody_lua_nbodyctx.h"
#include "nbody_lua_potential.h"
#include "nbody_lua_type_marshal.h"
#include "nbody_step.h"
#include "nbody_defaults.h"
#include "nbody_checkpoint.h"


NBodyState* checkNBodyState(lua_State* luaSt, int idx)
{
    return (NBodyState*) mw_checknamedudata(luaSt, idx, NBODYSTATE_TYPE);
}

NBodyState* toNBodyState(lua_State* luaSt, int idx)
{
    return (NBodyState*) mw_tonamedudata(luaSt, idx, NBODYSTATE_TYPE);
}

NBodyState* expectNBodyState(lua_State* luaSt, int idx)
{
    return (NBodyState*) expectType(luaSt, idx, NBODYSTATE_TYPE);
}

int pushNBodyState(lua_State* luaSt, const NBodyState* p)
{
    return pushType(luaSt, NBODYSTATE_TYPE, sizeof(NBodyState), (void*) p);
}

static int stepNBodyState(lua_State* luaSt)
{
    NBodyStatus rc;
    NBodyState* st;
    NBodyCtx ctx;

    if (lua_gettop(luaSt) != 3)
        return luaL_argerror(luaSt, 3, "Expected 3 arguments");

    st = checkNBodyState(luaSt, 1);
    ctx = *checkNBodyCtx(luaSt, 2);
    ctx.pot = *checkPotential(luaSt, 3);

    rc = stepSystem(&ctx, st);
    lua_pushstring(luaSt, showNBodyStatus(rc));
    return 1;
}

static int gcNBodyState(lua_State* luaSt)
{
    destroyNBodyState(checkNBodyState(luaSt, -1));
    return 0;
}

/* Receives arbitrary number of models and creates an initial state */
/* NBodyState.create(NBodyCtx ctx, Potential potential, ...) */
static int createNBodyState(lua_State* luaSt)
{
    Body* bodies;
    NBodyCtx ctx;
    unsigned int nbody;
    NBodyState st = EMPTY_NBODYSTATE;

    ctx = *checkNBodyCtx(luaSt, 1);
    ctx.pot = *checkPotential(luaSt, 2);
    bodies = readModels(luaSt, lua_gettop(luaSt) - 2, &nbody);
    if (!bodies)
        luaL_argerror(luaSt, 3, "Expected model tables");

    setInitialNBodyState(&st, &ctx, bodies, nbody);
    pushNBodyState(luaSt, &st);

    return 1;
}

static int sortBodiesNBodyState(lua_State* luaSt)
{
    NBodyState* st;

    if (lua_gettop(luaSt) != 1)
        luaL_argerror(luaSt, 1, "Expected 1 argument");

    st = checkNBodyState(luaSt, 1);
    sortBodies(st->bodytab, st->nbody);

    return 0;
}

static int luaWriteCheckpoint(lua_State* luaSt)
{
    NBodyState* st;
    const NBodyCtx* ctx;
    const char* tmpFile;

    st = checkNBodyState(luaSt, 1);
    ctx = checkNBodyCtx(luaSt, 2);

    assert(st->checkpointResolved == NULL);

    st->checkpointResolved = strdup(luaL_optstring(luaSt, 3, DEFAULT_CHECKPOINT_FILE));
    tmpFile = luaL_optstring(luaSt, 4, CHECKPOINT_TMP_FILE);

    if (writeCheckpointWithTmpFile(ctx, st, tmpFile))
        luaL_error(luaSt, "Error writing checkpoint");

    free(st->checkpointResolved);
    st->checkpointResolved = NULL;

    return 0;
}

static int luaCloneNBodyState(lua_State* luaSt)
{
    const NBodyState* oldSt;
    NBodyState newSt = EMPTY_NBODYSTATE;

    oldSt = checkNBodyState(luaSt, 1);
    cloneNBodyState(&newSt, oldSt);
    pushNBodyState(luaSt, &newSt);
    return 1;
}

/* Returns resumed context and state */
static int luaReadCheckpoint(lua_State* luaSt)
{
    NBodyCtx ctx = EMPTY_NBODYCTX;
    NBodyState st = EMPTY_NBODYSTATE;

    st.checkpointResolved = strdup(luaL_optstring(luaSt, 1, DEFAULT_CHECKPOINT_FILE));
    if (readCheckpoint(&ctx, &st))
        luaL_error(luaSt, "Error reading checkpoint '%s'", st.checkpointResolved);

    free(st.checkpointResolved);
    st.checkpointResolved = NULL;

    pushNBodyCtx(luaSt, &ctx);
    pushNBodyState(luaSt, &st);

    return 2;
}

static int eqNBodyState(lua_State* luaSt)
{
    lua_pushboolean(luaSt, equalNBodyState(checkNBodyState(luaSt, 1), checkNBodyState(luaSt, 2)));
    return 1;
}

static int toStringNBodyState(lua_State* luaSt)
{
    const NBodyState* st;
    char* buf;

    st = checkNBodyState(luaSt, 1);
    buf = showNBodyState(st);
    lua_pushstring(luaSt, buf);
    free(buf);

    return 1;
}


static const luaL_reg metaMethodsNBodyState[] =
{
    { "__gc",       gcNBodyState       },
    { "__eq",       eqNBodyState       },
    { "__tostring", toStringNBodyState },
    { NULL, NULL }
};

static const luaL_reg methodsNBodyState[] =
{
    { "create",          createNBodyState     },
    { "step",            stepNBodyState       },
    { "sortBodies",      sortBodiesNBodyState },
    { "clone",           luaCloneNBodyState   },
    { "writeCheckpoint", luaWriteCheckpoint   },
    { "readCheckpoint",  luaReadCheckpoint    },
    { NULL, NULL }
};

static const Xet_reg_pre gettersNBodyState[] =
{
    { NULL, NULL, 0 }
};

static const Xet_reg_pre settersNBodyState[] =
{
    { NULL, NULL, 0 }
};

int registerNBodyState(lua_State* luaSt)
{
    return registerStruct(luaSt,
                          NBODYSTATE_TYPE,
                          gettersNBodyState,
                          settersNBodyState,
                          metaMethodsNBodyState,
                          methodsNBodyState);
}

