/**
 * This file is part of prototype.
 *
 * (c) 2013 Nicolas Hillegeer <nicolas@hillegeer.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with the source code.
 */

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "util.h"

lua_State *gLua;

static void wfScriptLoadLibraries(lua_State *lua);

void wfScriptInit(void) {
    gLua = luaL_newstate();

    ERROR_EXIT(gLua == NULL, 0, "could not initialize the lua interpreter");

    wfScriptLoadLibraries(gLua);

    trace("succesfully loaded lua, initial memory usage: %d kb\n", wfScriptMemUsed());

    // char *script = loadfile("./game/hello.lua");
    // trace("loading script: %s\n", script);
    // zfree(script);

    int error = luaL_loadfile(gLua, "./game/hello.lua") || lua_pcall(gLua, 0, LUA_MULTRET, 0);
    ERROR_HANDLE(error != 0, error, "could not load lua script: %s", lua_tostring(gLua, -1));

    trace("initialization script ran, memory usage: %d kb\n", wfScriptMemUsed());

    return;
error:
    /* pop error message from the stack */
    lua_pop(gLua, 1);

    /* always clean up after self */
    lua_close(gLua);

    exit(1);
}

void wfScriptDestroy(void) {
    if (gLua) {
        lua_close(gLua);
    }
}

static void wfScriptLoadLibraries(lua_State *lua) {
    /* just open all libraries for now, over time we should */
    luaL_openlibs(lua);
}

int wfScriptMemUsed(void) {
    return lua_gc(gLua, LUA_GCCOUNT, 0);
}

const char *wfScriptVersion(void) {
    static char buffer[WF_STATIC_STRING_BUFSIZE];

    /* start a barebones lua interpreter and get the version number */
    lua_State *lua = luaL_newstate();
    {
        /**
         * load base library (and remove it, see linit.c for vanilla lua, lib_init.c for luajit)
         *
         * (for reference) lua 5.2 style:
         *
         * luaL_requiref(lua, "_G", luaopen_base, 1);
         * lua_pop(lua, 1);
         */
        lua_pushcfunction(lua, luaopen_base);
        lua_pushstring(lua, "");

        /**
         * call the C function luaopen_base, we specify that we
         * don't want the result (so we don't have to pop it either)
         */
        lua_call(lua, 1, 0);

        /* get version */
        lua_getglobal(lua, "_VERSION");
        snprintf(buffer, WF_STATIC_STRING_BUFSIZE, "Lua compiled against "  LUA_RELEASE " (running with %s)", lua_tostring(lua, -1));
    }
    lua_close(lua);

    return buffer;
    // return "luajit";
}
