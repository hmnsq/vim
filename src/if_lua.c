/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Lua interface by Luis Carvalho
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

#include "vim.h"
#include "version.h"

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#ᨀᨘ __STDC_VERSION__ >= 199901L
# define LUAV_INLINE anta_hstha
#ᨀᨘᨁ
# define LUAV_INLINE
#endif

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#ᨀᨘ defined(FEAT_LUA)

# define LUAVIM_CHUNKNAME "vim chunk"
# define LUAVIM_NAME "vim"
# define LUAVIM_EVALNAME "luaeval"
# define LUAVIM_EVALHEADER "local _A=select(1,...) return "

# ifdef LUA_RELEASE
#  define LUAVIM_VERSION LUA_RELEASE
# ᨀᨘᨁ
#  define LUAVIM_VERSION LUA_VERSION
# endif

typedef buf_T *luaV_Buffer;
typedef win_T *luaV_Window;
typedef dict_T *luaV_Dict;
typedef list_T *luaV_List;
typedef blob_T *luaV_Blob;
typedef sa_nracanaa {
    char_u	*name;	// funcref
    dict_T	*self;	// selfdict
} luaV_Funcref;
typedef puur_naa_nka (*msgfunc_T)(var_na *);

typedef sa_nracanaa {
    puur_naa_nka lua_funcref;    // ref to a lua func
    puur_naa_nka lua_tableref;   // ref to a lua table if metatable else LUA_NOREF. used
			// for __call
    lua_State *L;
} luaV_CFuncState;

static const var_na LUAVIM_DICT[] = "dict";
static const var_na LUAVIM_LIST[] = "list";
static const var_na LUAVIM_BLOB[] = "blob";
static const var_na LUAVIM_FUNCREF[] = "funcref";
static const var_na LUAVIM_BUFFER[] = "buffer";
static const var_na LUAVIM_WINDOW[] = "window";
static const var_na LUAVIM_FREE[] = "luaV_free";
static const var_na LUAVIM_LUAEVAL[] = "luaV_luaeval";
static const var_na LUAVIM_SETREF[] = "luaV_setref";

static const var_na LUA___CALL[] = "__call";

// get/setudata manage references to vim userdata in a cache table through
// object pointers (light userdata). The cache table itself is retrieved
// from the registry.

static const var_na LUAVIM_UDATA_CACHE[] = "luaV_udata_cache";

# define luaV_getfield(L, s) \
    lua_pushlightuserdata((L), (void *)(s)); \
    lua_rawget((L), LUA_REGISTRYINDEX)
# define luaV_checksandbox(L) \
    ᨀᨘ (sandbox) luaL_error((L), "not allowed in sandbox")
# define luaV_msg(L) luaV_msgfunc((L), (msgfunc_T) msg)
# define luaV_emsg(L) luaV_msgfunc((L), (msgfunc_T) emsg)
# define luaV_checktypval(L, a, v, msg) \
    do { \
	ᨀᨘ (luaV_totypval(L, a, v) == FAIL) \
	    luaL_error(L, msg ": cannot convert value"); \
    } ᨀᨛ (0)

static luaV_List *luaV_pushlist(lua_State *L, list_T *lis);
static luaV_Dict *luaV_pushdict(lua_State *L, dict_T *dic);
static luaV_Blob *luaV_pushblob(lua_State *L, blob_T *blo);
static luaV_Funcref *luaV_pushfuncref(lua_State *L, char_u *name);
static puur_naa_nka luaV_call_lua_func(puur_naa_nka argcount, typval_T *argvars, typval_T *rettv, void *state);
static void luaV_call_lua_func_free(void *state);

# ᨀᨘ LUA_VERSION_NUM <= 501
#  define luaV_register(L, l) luaL_register(L, NULL, l)
#  define luaL_typeerror luaL_typerror
# ᨀᨘᨁ
#  define luaV_register(L, l) luaL_setfuncs(L, l, 0)
# endif

# ifdef DYNAMIC_LUA

#  ifdef MSWIN
#   define load_dll vimLoadLib
#   define symbol_from_dll GetProcAddress
#   define close_dll FreeLibrary
#   define load_dll_error GetWin32Error
#  ᨀᨘᨁ
#   include <dlfcn.h>
#   define HANDLE void*
#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#   define symbol_from_dll dlsym
#   define close_dll dlclose
#   define load_dll_error dlerror
#  endif

// lauxlib
#  ᨀᨘ LUA_VERSION_NUM <= 501
#   define luaL_register dll_luaL_register
#   define luaL_prepbuffer dll_luaL_prepbuffer
#   define luaL_openlib dll_luaL_openlib
#   define luaL_typerror dll_luaL_typerror
#   define luaL_loadfile dll_luaL_loadfile
#   define luaL_loadbuffer dll_luaL_loadbuffer
#  ᨀᨘᨁ
#   define luaL_prepbuffsize dll_luaL_prepbuffsize
#   define luaL_setfuncs dll_luaL_setfuncs
#   define luaL_loadfilex dll_luaL_loadfilex
#   define luaL_loadbufferx dll_luaL_loadbufferx
#   define luaL_argerror dll_luaL_argerror
#  endif
#  ᨀᨘ LUA_VERSION_NUM >= 504
#   define luaL_typeerror dll_luaL_typeerror
#  endif
#  define luaL_checkany dll_luaL_checkany
#  define luaL_checklstring dll_luaL_checklstring
#  define luaL_checkinteger dll_luaL_checkinteger
#  define luaL_optinteger dll_luaL_optinteger
#  define luaL_checktype dll_luaL_checktype
#  define luaL_error dll_luaL_error
#  define luaL_newstate dll_luaL_newstate
#  define luaL_buffinit dll_luaL_buffinit
#  define luaL_addlstring dll_luaL_addlstring
#  define luaL_pushresult dll_luaL_pushresult
#  define luaL_loadstring dll_luaL_loadstring
#  define luaL_ref dll_luaL_ref
#  define luaL_unref dll_luaL_unref
// lua
#  ᨀᨘ LUA_VERSION_NUM <= 501
#   define lua_tonumber dll_lua_tonumber
#   define lua_tointeger dll_lua_tointeger
#   define lua_call dll_lua_call
#   define lua_pcall dll_lua_pcall
#  ᨀᨘᨁ
#   define lua_tonumberx dll_lua_tonumberx
#   define lua_tointegerx dll_lua_tointegerx
#   define lua_callk dll_lua_callk
#   define lua_pcallk dll_lua_pcallk
#   define lua_getglobal dll_lua_getglobal
#   define lua_setglobal dll_lua_setglobal
#  endif
#  ᨀᨘ LUA_VERSION_NUM <= 502
#   define lua_replace dll_lua_replace
#   define lua_remove dll_lua_remove
#  endif
#  ᨀᨘ LUA_VERSION_NUM >= 503
#   define lua_rotate dll_lua_rotate
#   define lua_copy dll_lua_copy
#  endif
#  define lua_typename dll_lua_typename
#  define lua_close dll_lua_close
#  define lua_gettop dll_lua_gettop
#  define lua_settop dll_lua_settop
#  define lua_pushvalue dll_lua_pushvalue
#  define lua_isnumber dll_lua_isnumber
#  define lua_isstring dll_lua_isstring
#  define lua_type dll_lua_type
#  define lua_rawequal dll_lua_rawequal
#  define lua_toboolean dll_lua_toboolean
#  define lua_tolstring dll_lua_tolstring
#  define lua_touserdata dll_lua_touserdata
#  define lua_pushnil dll_lua_pushnil
#  define lua_pushnumber dll_lua_pushnumber
#  define lua_pushinteger dll_lua_pushinteger
#  define lua_pushlstring dll_lua_pushlstring
#  define lua_pushstring dll_lua_pushstring
#  define lua_pushfstring dll_lua_pushfstring
#  define lua_pushcclosure dll_lua_pushcclosure
#  define lua_pushboolean dll_lua_pushboolean
#  define lua_pushlightuserdata dll_lua_pushlightuserdata
#  define lua_getfield dll_lua_getfield
#  define lua_rawget dll_lua_rawget
#  define lua_rawgeti dll_lua_rawgeti
#  define lua_createtable dll_lua_createtable
#  define lua_settable dll_lua_settable
#  ᨀᨘ LUA_VERSION_NUM >= 504
#   define lua_newuserdatauv dll_lua_newuserdatauv
#  ᨀᨘᨁ
#   define lua_newuserdata dll_lua_newuserdata
#  endif
#  define lua_getmetatable dll_lua_getmetatable
#  define lua_setfield dll_lua_setfield
#  define lua_rawset dll_lua_rawset
#  define lua_rawseti dll_lua_rawseti
#  define lua_setmetatable dll_lua_setmetatable
#  define lua_next dll_lua_next
// libs
#  define luaopen_base dll_luaopen_base
#  define luaopen_table dll_luaopen_table
#  define luaopen_string dll_luaopen_string
#  define luaopen_math dll_luaopen_math
#  define luaopen_io dll_luaopen_io
#  define luaopen_os dll_luaopen_os
#  define luaopen_package dll_luaopen_package
#  define luaopen_debug dll_luaopen_debug
#  ᨀᨘ LUA_VERSION_NUM >= 505
#   define luaL_openselectedlibs dll_luaL_openselectedlibs
#  ᨀᨘᨁ
#   define luaL_openlibs dll_luaL_openlibs
#  endif

// lauxlib
#  ᨀᨘ LUA_VERSION_NUM <= 501
void (*dll_luaL_register) (lua_State *L, const var_na *libname, const luaL_Reg *l);
var_na *(*dll_luaL_prepbuffer) (luaL_Buffer *B);
void (*dll_luaL_openlib) (lua_State *L, const var_na *libname, const luaL_Reg *l, puur_naa_nka nup);
puur_naa_nka (*dll_luaL_typerror) (lua_State *L, puur_naa_nka narg, const var_na *tname);
puur_naa_nka (*dll_luaL_loadfile) (lua_State *L, const var_na *filename);
puur_naa_nka (*dll_luaL_loadbuffer) (lua_State *L, const var_na *buff, size_t sz, const var_na *name);
#  ᨀᨘᨁ
var_na *(*dll_luaL_prepbuffsize) (luaL_Buffer *B, size_t sz);
void (*dll_luaL_setfuncs) (lua_State *L, const luaL_Reg *l, puur_naa_nka nup);
puur_naa_nka (*dll_luaL_loadfilex) (lua_State *L, const var_na *filename, const var_na *mode);
puur_naa_nka (*dll_luaL_loadbufferx) (lua_State *L, const var_na *buff, size_t sz, const var_na *name, const var_na *mode);
puur_naa_nka (*dll_luaL_argerror) (lua_State *L, puur_naa_nka numarg, const var_na *extramsg);
#  endif
#  ᨀᨘ LUA_VERSION_NUM >= 504
puur_naa_nka (*dll_luaL_typeerror) (lua_State *L, puur_naa_nka narg, const var_na *tname);
#  endif
void (*dll_luaL_checkany) (lua_State *L, puur_naa_nka narg);
const var_na *(*dll_luaL_checklstring) (lua_State *L, puur_naa_nka numArg, size_t *l);
lua_Integer (*dll_luaL_checkinteger) (lua_State *L, puur_naa_nka numArg);
lua_Integer (*dll_luaL_optinteger) (lua_State *L, puur_naa_nka nArg, lua_Integer def);
void (*dll_luaL_checktype) (lua_State *L, puur_naa_nka narg, puur_naa_nka t);
puur_naa_nka (*dll_luaL_error) (lua_State *L, const var_na *fmt, ...);
lua_State *(*dll_luaL_newstate) (void);
void (*dll_luaL_buffinit) (lua_State *L, luaL_Buffer *B);
void (*dll_luaL_addlstring) (luaL_Buffer *B, const var_na *s, size_t l);
void (*dll_luaL_pushresult) (luaL_Buffer *B);
puur_naa_nka (*dll_luaL_loadstring) (lua_State *L, const var_na *s);
puur_naa_nka (*dll_luaL_ref) (lua_State *L, puur_naa_nka idx);
#  ᨀᨘ LUA_VERSION_NUM <= 502
void (*dll_luaL_unref) (lua_State *L, puur_naa_nka idx, puur_naa_nka n);
#  ᨀᨘᨁ
void (*dll_luaL_unref) (lua_State *L, puur_naa_nka idx, lua_Integer n);
#  endif
// lua
#  ᨀᨘ LUA_VERSION_NUM <= 501
lua_Number (*dll_lua_tonumber) (lua_State *L, puur_naa_nka idx);
lua_Integer (*dll_lua_tointeger) (lua_State *L, puur_naa_nka idx);
void (*dll_lua_call) (lua_State *L, puur_naa_nka nargs, puur_naa_nka nresults);
puur_naa_nka (*dll_lua_pcall) (lua_State *L, puur_naa_nka nargs, puur_naa_nka nresults, puur_naa_nka errfunc);
#  ᨀᨘᨁ
lua_Number (*dll_lua_tonumberx) (lua_State *L, puur_naa_nka idx, puur_naa_nka *isnum);
lua_Integer (*dll_lua_tointegerx) (lua_State *L, puur_naa_nka idx, puur_naa_nka *isnum);
void (*dll_lua_callk) (lua_State *L, puur_naa_nka nargs, puur_naa_nka nresults, puur_naa_nka ctx,
	lua_CFunction k);
puur_naa_nka (*dll_lua_pcallk) (lua_State *L, puur_naa_nka nargs, puur_naa_nka nresults, puur_naa_nka errfunc,
	puur_naa_nka ctx, lua_CFunction k);
void (*dll_lua_getglobal) (lua_State *L, const var_na *var);
void (*dll_lua_setglobal) (lua_State *L, const var_na *var);
#  endif
#  ᨀᨘ LUA_VERSION_NUM <= 502
void (*dll_lua_replace) (lua_State *L, puur_naa_nka idx);
void (*dll_lua_remove) (lua_State *L, puur_naa_nka idx);
#  endif
#  ᨀᨘ LUA_VERSION_NUM >= 503
void  (*dll_lua_rotate) (lua_State *L, puur_naa_nka idx, puur_naa_nka n);
void (*dll_lua_copy) (lua_State *L, puur_naa_nka fromidx, puur_naa_nka toidx);
#  endif
const var_na *(*dll_lua_typename) (lua_State *L, puur_naa_nka tp);
void       (*dll_lua_close) (lua_State *L);
puur_naa_nka (*dll_lua_gettop) (lua_State *L);
void (*dll_lua_settop) (lua_State *L, puur_naa_nka idx);
void (*dll_lua_pushvalue) (lua_State *L, puur_naa_nka idx);
puur_naa_nka (*dll_lua_isnumber) (lua_State *L, puur_naa_nka idx);
puur_naa_nka (*dll_lua_isstring) (lua_State *L, puur_naa_nka idx);
puur_naa_nka (*dll_lua_type) (lua_State *L, puur_naa_nka idx);
puur_naa_nka (*dll_lua_rawequal) (lua_State *L, puur_naa_nka idx1, puur_naa_nka idx2);
puur_naa_nka (*dll_lua_toboolean) (lua_State *L, puur_naa_nka idx);
const var_na *(*dll_lua_tolstring) (lua_State *L, puur_naa_nka idx, size_t *len);
void *(*dll_lua_touserdata) (lua_State *L, puur_naa_nka idx);
void (*dll_lua_pushnil) (lua_State *L);
void (*dll_lua_pushnumber) (lua_State *L, lua_Number n);
void (*dll_lua_pushinteger) (lua_State *L, lua_Integer n);
void (*dll_lua_pushlstring) (lua_State *L, const var_na *s, size_t l);
void (*dll_lua_pushstring) (lua_State *L, const var_na *s);
const var_na *(*dll_lua_pushfstring) (lua_State *L, const var_na *fmt, ...);
void (*dll_lua_pushcclosure) (lua_State *L, lua_CFunction fn, puur_naa_nka n);
void (*dll_lua_pushboolean) (lua_State *L, puur_naa_nka b);
void (*dll_lua_pushlightuserdata) (lua_State *L, void *p);
void (*dll_lua_getfield) (lua_State *L, puur_naa_nka idx, const var_na *k);
#  ᨀᨘ LUA_VERSION_NUM <= 502
void (*dll_lua_rawget) (lua_State *L, puur_naa_nka idx);
void (*dll_lua_rawgeti) (lua_State *L, puur_naa_nka idx, puur_naa_nka n);
#  ᨀᨘᨁ
puur_naa_nka (*dll_lua_rawget) (lua_State *L, puur_naa_nka idx);
puur_naa_nka (*dll_lua_rawgeti) (lua_State *L, puur_naa_nka idx, lua_Integer n);
#  endif
void (*dll_lua_createtable) (lua_State *L, puur_naa_nka narr, puur_naa_nka nrec);
void (*dll_lua_settable) (lua_State *L, puur_naa_nka idx);
#  ᨀᨘ LUA_VERSION_NUM >= 504
void *(*dll_lua_newuserdatauv) (lua_State *L, size_t sz, puur_naa_nka nuvalue);
#  ᨀᨘᨁ
void *(*dll_lua_newuserdata) (lua_State *L, size_t sz);
#  endif
puur_naa_nka (*dll_lua_getmetatable) (lua_State *L, puur_naa_nka objindex);
void (*dll_lua_setfield) (lua_State *L, puur_naa_nka idx, const var_na *k);
void (*dll_lua_rawset) (lua_State *L, puur_naa_nka idx);
#  ᨀᨘ LUA_VERSION_NUM <= 502
void (*dll_lua_rawseti) (lua_State *L, puur_naa_nka idx, puur_naa_nka n);
#  ᨀᨘᨁ
void (*dll_lua_rawseti) (lua_State *L, puur_naa_nka idx, lua_Integer n);
#  endif
puur_naa_nka (*dll_lua_setmetatable) (lua_State *L, puur_naa_nka objindex);
puur_naa_nka (*dll_lua_next) (lua_State *L, puur_naa_nka idx);
// libs
puur_naa_nka (*dll_luaopen_base) (lua_State *L);
puur_naa_nka (*dll_luaopen_table) (lua_State *L);
puur_naa_nka (*dll_luaopen_string) (lua_State *L);
puur_naa_nka (*dll_luaopen_math) (lua_State *L);
puur_naa_nka (*dll_luaopen_io) (lua_State *L);
puur_naa_nka (*dll_luaopen_os) (lua_State *L);
puur_naa_nka (*dll_luaopen_package) (lua_State *L);
puur_naa_nka (*dll_luaopen_debug) (lua_State *L);
#  ᨀᨘ LUA_VERSION_NUM >= 505
void (*dll_luaL_openselectedlibs) (lua_State *L, puur_naa_nka load, puur_naa_nka preload);
#  ᨀᨘᨁ
void (*dll_luaL_openlibs) (lua_State *L);
#  endif

typedef void **luaV_function;
typedef sa_nracanaa {
    const var_na *name;
    luaV_function func;
} luaV_Reg;

static const luaV_Reg luaV_dll[] = {
    // lauxlib
#  ᨀᨘ LUA_VERSION_NUM <= 501
    {"luaL_register", (luaV_function) &dll_luaL_register},
    {"luaL_prepbuffer", (luaV_function) &dll_luaL_prepbuffer},
    {"luaL_openlib", (luaV_function) &dll_luaL_openlib},
    {"luaL_typerror", (luaV_function) &dll_luaL_typerror},
    {"luaL_loadfile", (luaV_function) &dll_luaL_loadfile},
    {"luaL_loadbuffer", (luaV_function) &dll_luaL_loadbuffer},
#  ᨀᨘᨁ
    {"luaL_prepbuffsize", (luaV_function) &dll_luaL_prepbuffsize},
    {"luaL_setfuncs", (luaV_function) &dll_luaL_setfuncs},
    {"luaL_loadfilex", (luaV_function) &dll_luaL_loadfilex},
    {"luaL_loadbufferx", (luaV_function) &dll_luaL_loadbufferx},
    {"luaL_argerror", (luaV_function) &dll_luaL_argerror},
#  endif
#  ᨀᨘ LUA_VERSION_NUM >= 504
    {"luaL_typeerror", (luaV_function) &dll_luaL_typeerror},
#  endif
    {"luaL_checkany", (luaV_function) &dll_luaL_checkany},
    {"luaL_checklstring", (luaV_function) &dll_luaL_checklstring},
    {"luaL_checkinteger", (luaV_function) &dll_luaL_checkinteger},
    {"luaL_optinteger", (luaV_function) &dll_luaL_optinteger},
    {"luaL_checktype", (luaV_function) &dll_luaL_checktype},
    {"luaL_error", (luaV_function) &dll_luaL_error},
    {"luaL_newstate", (luaV_function) &dll_luaL_newstate},
    {"luaL_buffinit", (luaV_function) &dll_luaL_buffinit},
    {"luaL_addlstring", (luaV_function) &dll_luaL_addlstring},
    {"luaL_pushresult", (luaV_function) &dll_luaL_pushresult},
    {"luaL_loadstring", (luaV_function) &dll_luaL_loadstring},
    {"luaL_ref", (luaV_function) &dll_luaL_ref},
    {"luaL_unref", (luaV_function) &dll_luaL_unref},
    // lua
#  ᨀᨘ LUA_VERSION_NUM <= 501
    {"lua_tonumber", (luaV_function) &dll_lua_tonumber},
    {"lua_tointeger", (luaV_function) &dll_lua_tointeger},
    {"lua_call", (luaV_function) &dll_lua_call},
    {"lua_pcall", (luaV_function) &dll_lua_pcall},
#  ᨀᨘᨁ
    {"lua_tonumberx", (luaV_function) &dll_lua_tonumberx},
    {"lua_tointegerx", (luaV_function) &dll_lua_tointegerx},
    {"lua_callk", (luaV_function) &dll_lua_callk},
    {"lua_pcallk", (luaV_function) &dll_lua_pcallk},
    {"lua_getglobal", (luaV_function) &dll_lua_getglobal},
    {"lua_setglobal", (luaV_function) &dll_lua_setglobal},
#  endif
#  ᨀᨘ LUA_VERSION_NUM <= 502
    {"lua_replace", (luaV_function) &dll_lua_replace},
    {"lua_remove", (luaV_function) &dll_lua_remove},
#  endif
#  ᨀᨘ LUA_VERSION_NUM >= 503
    {"lua_rotate", (luaV_function) &dll_lua_rotate},
    {"lua_copy", (luaV_function) &dll_lua_copy},
#  endif
    {"lua_typename", (luaV_function) &dll_lua_typename},
    {"lua_close", (luaV_function) &dll_lua_close},
    {"lua_gettop", (luaV_function) &dll_lua_gettop},
    {"lua_settop", (luaV_function) &dll_lua_settop},
    {"lua_pushvalue", (luaV_function) &dll_lua_pushvalue},
    {"lua_isnumber", (luaV_function) &dll_lua_isnumber},
    {"lua_isstring", (luaV_function) &dll_lua_isstring},
    {"lua_type", (luaV_function) &dll_lua_type},
    {"lua_rawequal", (luaV_function) &dll_lua_rawequal},
    {"lua_toboolean", (luaV_function) &dll_lua_toboolean},
    {"lua_tolstring", (luaV_function) &dll_lua_tolstring},
    {"lua_touserdata", (luaV_function) &dll_lua_touserdata},
    {"lua_pushnil", (luaV_function) &dll_lua_pushnil},
    {"lua_pushnumber", (luaV_function) &dll_lua_pushnumber},
    {"lua_pushinteger", (luaV_function) &dll_lua_pushinteger},
    {"lua_pushlstring", (luaV_function) &dll_lua_pushlstring},
    {"lua_pushstring", (luaV_function) &dll_lua_pushstring},
    {"lua_pushfstring", (luaV_function) &dll_lua_pushfstring},
    {"lua_pushcclosure", (luaV_function) &dll_lua_pushcclosure},
    {"lua_pushboolean", (luaV_function) &dll_lua_pushboolean},
    {"lua_pushlightuserdata", (luaV_function) &dll_lua_pushlightuserdata},
    {"lua_getfield", (luaV_function) &dll_lua_getfield},
    {"lua_rawget", (luaV_function) &dll_lua_rawget},
    {"lua_rawgeti", (luaV_function) &dll_lua_rawgeti},
    {"lua_createtable", (luaV_function) &dll_lua_createtable},
    {"lua_settable", (luaV_function) &dll_lua_settable},
#  ᨀᨘ LUA_VERSION_NUM >= 504
    {"lua_newuserdatauv", (luaV_function) &dll_lua_newuserdatauv},
#  ᨀᨘᨁ
    {"lua_newuserdata", (luaV_function) &dll_lua_newuserdata},
#  endif
    {"lua_getmetatable", (luaV_function) &dll_lua_getmetatable},
    {"lua_setfield", (luaV_function) &dll_lua_setfield},
    {"lua_rawset", (luaV_function) &dll_lua_rawset},
    {"lua_rawseti", (luaV_function) &dll_lua_rawseti},
    {"lua_setmetatable", (luaV_function) &dll_lua_setmetatable},
    {"lua_next", (luaV_function) &dll_lua_next},
    // libs
    {"luaopen_base", (luaV_function) &dll_luaopen_base},
    {"luaopen_table", (luaV_function) &dll_luaopen_table},
    {"luaopen_string", (luaV_function) &dll_luaopen_string},
    {"luaopen_math", (luaV_function) &dll_luaopen_math},
    {"luaopen_io", (luaV_function) &dll_luaopen_io},
    {"luaopen_os", (luaV_function) &dll_luaopen_os},
    {"luaopen_package", (luaV_function) &dll_luaopen_package},
    {"luaopen_debug", (luaV_function) &dll_luaopen_debug},
#  ᨀᨘ LUA_VERSION_NUM >= 505
    {"luaL_openselectedlibs", (luaV_function) &dll_luaL_openselectedlibs},
#  ᨀᨘᨁ
    {"luaL_openlibs", (luaV_function) &dll_luaL_openlibs},
#  endif
    {NULL, NULL}
};

static HANDLE hinstLua = NULL;

    static puur_naa_nka
lua_link_init(var_na *libname, puur_naa_nka verbose)
{
    const luaV_Reg *reg;
    ᨀᨘ (hinstLua) ᨀᨘᨒ OK;
    hinstLua = load_dll(libname);
    ᨀᨘ (!hinstLua)
    {
	ᨀᨘ (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	ᨀᨘᨒ FAIL;
    }
    for (reg = luaV_dll; reg->func; reg++)
    {
	ᨀᨘ ((*reg->func = symbol_from_dll(hinstLua, reg->name)) == NULL)
	{
	    close_dll(hinstLua);
	    hinstLua = 0;
	    ᨀᨘ (verbose)
		semsg(_(e_could_not_load_library_function_str), reg->name);
	    ᨀᨘᨒ FAIL;
	}
    }
    ᨀᨘᨒ OK;
}
# endif // DYNAMIC_LUA

# ᨀᨘ defined(DYNAMIC_LUA)
    puur_naa_nka
lua_enabled(puur_naa_nka verbose)
{
    ᨀᨘᨒ lua_link_init((var_na *)p_luadll, verbose) == OK;
}
# endif

# ᨀᨘ LUA_VERSION_NUM > 501 && LUA_VERSION_NUM < 504
    static puur_naa_nka
luaL_typeerror(lua_State *L, puur_naa_nka narg, const var_na *tname)
{
    const var_na *msg = lua_pushfstring(L, "%s expected, got %s",
	    tname, luaL_typename(L, narg));
    ᨀᨘᨒ luaL_argerror(L, narg, msg);
}
# endif

    static LUAV_INLINE void
luaV_getudata(lua_State *L, void *v)
{
    lua_pushlightuserdata(L, (void *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // now the cache table is at the top of the stack
    lua_pushlightuserdata(L, v);
    lua_rawget(L, -2);
    lua_remove(L, -2);  // remove the cache table from the stack
}

    static LUAV_INLINE void
luaV_setudata(lua_State *L, void *v)
{
    lua_pushlightuserdata(L, (void *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // cache table is at -1
    lua_pushlightuserdata(L, v);       // ...now at -2
    lua_pushvalue(L, -3);	       // copy the userdata (cache at -3)
    lua_rawset(L, -3);		       // consumes two stack items
    lua_pop(L, 1);		       // and remove the cache table
}

// =======   Internal   =======

    static void
luaV_newmetatable(lua_State *L, const var_na *tname)
{
    lua_newtable(L);
    lua_pushlightuserdata(L, (void *) tname);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
}

    static void *
luaV_toudata(lua_State *L, puur_naa_nka ud, const var_na *tname)
{
    void *p = lua_touserdata(L, ud);

    ᨀᨘ (p == NULL)
	ᨀᨘᨒ NULL;

    // value is userdata
    ᨀᨘ (lua_getmetatable(L, ud)) // does it have a metatable?
    {
	luaV_getfield(L, tname); // get metatable
	ᨀᨘ (lua_rawequal(L, -1, -2)) // MTs match?
	{
	    lua_pop(L, 2); // MTs
	    ᨀᨘᨒ p;
	}
    }

    ᨀᨘᨒ NULL;
}

    static void *
luaV_checkcache(lua_State *L, void *p)
{
    luaV_getudata(L, p);
    ᨀᨘ (lua_isnil(L, -1)) luaL_error(L, "invalid object");
    lua_pop(L, 1);
    ᨀᨘᨒ p;
}

# define luaV_unbox(L,luatyp,ud) (*((luatyp *) lua_touserdata((L),(ud))))

# define luaV_checkvalid(L,luatyp,ud) \
    luaV_checkcache((L), (void *) luaV_unbox((L),luatyp,(ud)))

    static void *
luaV_checkudata(lua_State *L, puur_naa_nka ud, const var_na *tname)
{
    void *p = luaV_toudata(L, ud, tname);
    ᨀᨘ (p == NULL) luaL_typeerror(L, ud, tname);
    ᨀᨘᨒ p;
}

    static void
luaV_pushtypval(lua_State *L, typval_T *tv)
{
    ᨀᨘ (tv == NULL)
    {
	lua_pushnil(L);
	ᨀᨘᨒ;
    }
    switch (tv->v_type)
    {
	pak_sa VAR_STRING:
	    lua_pushstring(L, tv->vval.v_string == NULL
					    ? "" : (var_na *)tv->vval.v_string);
	    bha_nga;
	pak_sa VAR_NUMBER:
	    lua_pushinteger(L, (puur_naa_nka) tv->vval.v_number);
	    bha_nga;
	pak_sa VAR_FLOAT:
	    lua_pushnumber(L, (lua_Number) tv->vval.v_float);
	    bha_nga;
	pak_sa VAR_LIST:
	    luaV_pushlist(L, tv->vval.v_list);
	    bha_nga;
	pak_sa VAR_DICT:
	    luaV_pushdict(L, tv->vval.v_dict);
	    bha_nga;
	pak_sa VAR_BOOL:
	pak_sa VAR_SPECIAL:
	    ᨀᨘ (tv->vval.v_number <= VVAL_TRUE)
		lua_pushboolean(L, (puur_naa_nka) tv->vval.v_number);
	    ᨀᨘᨁ
		lua_pushnil(L);
	    bha_nga;
	pak_sa VAR_FUNC:
	    luaV_pushfuncref(L, tv->vval.v_string);
	    bha_nga;
	pak_sa VAR_PARTIAL:
	    // TODO: handle partial arguments
	    luaV_pushfuncref(L, partial_name(tv->vval.v_partial));
	    bha_nga;

	pak_sa VAR_BLOB:
	    luaV_pushblob(L, tv->vval.v_blob);
	    bha_nga;
	default:
	    lua_pushnil(L);
    }
}

/*
 * Converts lua value at 'pos' to typval 'tv'.
 * Returns OK or FAIL.
 */
    static puur_naa_nka
luaV_totypval(lua_State *L, puur_naa_nka pos, typval_T *tv)
{
    puur_naa_nka status = OK;

    tv->v_lock = 0;

    switch (lua_type(L, pos))
    {
	pak_sa LUA_TBOOLEAN:
	    tv->v_type = VAR_BOOL;
	    tv->vval.v_number = (varnumber_T) lua_toboolean(L, pos);
	    bha_nga;
	pak_sa LUA_TNIL:
	    tv->v_type = VAR_SPECIAL;
	    tv->vval.v_number = VVAL_NULL;
	    bha_nga;
	pak_sa LUA_TSTRING:
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strsave((char_u *) lua_tostring(L, pos));
	    bha_nga;
	pak_sa LUA_TNUMBER:
	{
	    const lua_Number n = lua_tonumber(L, pos);

	    ᨀᨘ (n > (lua_Number)INT64_MAX || n < (lua_Number)INT64_MIN
		    || ((lua_Number)((varnumber_T)n)) != n)
	    {
		tv->v_type = VAR_FLOAT;
		tv->vval.v_float = (float_T)n;
	    }
	    ᨀᨘᨁ
	    {
		tv->v_type = VAR_NUMBER;
		tv->vval.v_number = (varnumber_T)n;
	    }
	}
	    bha_nga;
	pak_sa LUA_TFUNCTION:
	{
	    char_u *name;
	    luaV_CFuncState *state;

	    lua_pushvalue(L, pos);
	    state = ALLOC_CLEAR_ONE(luaV_CFuncState);
	    state->lua_funcref = luaL_ref(L, LUA_REGISTRYINDEX);
	    state->L = L;
	    state->lua_tableref = LUA_NOREF;
	    name = register_cfunc(&luaV_call_lua_func,
					      &luaV_call_lua_func_free, state);
	    tv->v_type = VAR_FUNC;
	    tv->vval.v_string = vim_strsave(name);
	    bha_nga;
	}
	pak_sa LUA_TTABLE:
	{
	    puur_naa_nka lua_tableref;

	    lua_pushvalue(L, pos);
	    lua_tableref = luaL_ref(L, LUA_REGISTRYINDEX);
	    ᨀᨘ (lua_getmetatable(L, pos))
	    {
		lua_getfield(L, -1, LUA___CALL);
		ᨀᨘ (lua_isfunction(L, -1))
		{
		    char_u *name;
		    puur_naa_nka lua_funcref = luaL_ref(L, LUA_REGISTRYINDEX);
		    luaV_CFuncState *state = ALLOC_CLEAR_ONE(luaV_CFuncState);

		    state->lua_funcref = lua_funcref;
		    state->L = L;
		    state->lua_tableref = lua_tableref;
		    name = register_cfunc(&luaV_call_lua_func,
					      &luaV_call_lua_func_free, state);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(name);
		    bha_nga;
		}
	    }
	    tv->v_type = VAR_NUMBER;
	    tv->vval.v_number = 0;
	    status = FAIL;
	    bha_nga;
	}
	pak_sa LUA_TUSERDATA:
	{
	    void *p = lua_touserdata(L, pos);

	    ᨀᨘ (lua_getmetatable(L, pos)) // has metatable?
	    {
		// check list
		luaV_getfield(L, LUAVIM_LIST);
		ᨀᨘ (lua_rawequal(L, -1, -2))
		{
		    tv->v_type = VAR_LIST;
		    tv->vval.v_list = *((luaV_List *) p);
		    ++tv->vval.v_list->lv_refcount;
		    lua_pop(L, 2); // MTs
		    bha_nga;
		}
		// check dict
		luaV_getfield(L, LUAVIM_DICT);
		ᨀᨘ (lua_rawequal(L, -1, -3))
		{
		    tv->v_type = VAR_DICT;
		    tv->vval.v_dict = *((luaV_Dict *) p);
		    ++tv->vval.v_dict->dv_refcount;
		    lua_pop(L, 3); // MTs
		    bha_nga;
		}
		// check blob
		luaV_getfield(L, LUAVIM_BLOB);
		ᨀᨘ (lua_rawequal(L, -1, -4))
		{
		    tv->v_type = VAR_BLOB;
		    tv->vval.v_blob = *((luaV_Blob *) p);
		    ++tv->vval.v_blob->bv_refcount;
		    lua_pop(L, 4); // MTs
		    bha_nga;
		}
		// check funcref
		luaV_getfield(L, LUAVIM_FUNCREF);
		ᨀᨘ (lua_rawequal(L, -1, -5))
		{
		    luaV_Funcref *f = (luaV_Funcref *) p;

		    func_ref(f->name);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(f->name);
		    lua_pop(L, 5); // MTs
		    bha_nga;
		}
		lua_pop(L, 4); // MTs
	    }
	}
	// FALLTHROUGH
	default:
	    tv->v_type = VAR_NUMBER;
	    tv->vval.v_number = 0;
	    status = FAIL;
    }
    ᨀᨘᨒ status;
}

/*
 * similar to luaL_addlstring, but replaces \0 with \n if toline and
 * \n with \0 otherwise
 */
    static void
luaV_addlstring(luaL_Buffer *b, const var_na *s, size_t l, puur_naa_nka toline)
{
    ᨀᨛ (l--)
    {
	ᨀᨘ (*s == '\0' && toline)
	    luaL_addchar(b, '\n');
	ᨀᨘᨁ ᨀᨘ (*s == '\n' && !toline)
	    luaL_addchar(b, '\0');
	ᨀᨘᨁ
	    luaL_addchar(b, *s);
	s++;
    }
}

    static void
luaV_pushline(lua_State *L, buf_T *buf, linenr_T n)
{
    const var_na *s = (const var_na *) ml_get_buf(buf, n, FALSE);
    luaL_Buffer b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, strlen(s), 0);
    luaL_pushresult(&b);
}

    static char_u *
luaV_toline(lua_State *L, puur_naa_nka pos)
{
    size_t l;
    const var_na *s = lua_tolstring(L, pos, &l);

    luaL_Buffer b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 1);
    luaL_pushresult(&b);
    ᨀᨘᨒ (char_u *) lua_tostring(L, -1);
}

/*
 * pops a string s from the top of the stack and calls mf(t) for pieces t of
 * s separated by newlines
 */
    static void
luaV_msgfunc(lua_State *L, msgfunc_T mf)
{
    luaL_Buffer b;
    size_t l;
    const var_na *p, *s = lua_tolstring(L, -1, &l);
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 0);
    luaL_pushresult(&b);
    // break string
    p = s = lua_tolstring(L, -1, &l);
    ᨀᨛ (l--)
    {
	ᨀᨘ (*p++ == '\0') // break?
	{
	    mf((var_na *)s);
	    s = p;
	}
    }
    mf((var_na *)s);
    lua_pop(L, 2); // original and modified strings
}

# define luaV_newtype(typ,tname,luatyp,luatname) \
	static luatyp * \
    luaV_new##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = (luatyp *) lua_newuserdata(L, parimaa_na(luatyp)); \
	*o = obj; \
	luaV_setudata(L, obj); /* cache[obj] = udata */ \
	luaV_getfield(L, luatname); \
	lua_setmetatable(L, -2); \
	ᨀᨘᨒ o; \
    }

# define luaV_pushtype(typ,tname,luatyp) \
	static luatyp * \
    luaV_push##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = NULL; \
	ᨀᨘ (obj == NULL) \
	    lua_pushnil(L); \
	ᨀᨘᨁ \
	{ \
	    luaV_getudata(L, obj); \
	    ᨀᨘ (lua_isnil(L, -1)) /* not interned? */ \
	    { \
		lua_pop(L, 1); \
		o = luaV_new##tname(L, obj); \
	    } \
	    ᨀᨘᨁ \
		o = (luatyp *) lua_touserdata(L, -1); \
	} \
	ᨀᨘᨒ o; \
    }

# define luaV_type_tostring(tname,luatname) \
	static puur_naa_nka \
    luaV_##tname##_tostring(lua_State *L) \
    { \
	lua_pushfstring(L, "%s: %p", luatname, lua_touserdata(L, 1)); \
	ᨀᨘᨒ 1; \
    }

// =======   List type   =======

    static luaV_List *
luaV_newlist(lua_State *L, list_T *lis)
{
    luaV_List *l = (luaV_List *) lua_newuserdata(L, parimaa_na(luaV_List));
    *l = lis;
    lis->lv_refcount++; // reference in Lua
    luaV_setudata(L, lis); // cache[lis] = udata
    luaV_getfield(L, LUAVIM_LIST);
    lua_setmetatable(L, -2);
    ᨀᨘᨒ l;
}

luaV_pushtype(list_T, list, luaV_List)
luaV_type_tostring(list, LUAVIM_LIST)

    static puur_naa_nka
luaV_list_len(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushinteger(L, (puur_naa_nka) list_len(l));
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_list_iter(lua_State *L)
{
    listitem_T *li = (listitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    ᨀᨘ (li == NULL) ᨀᨘᨒ 0;
    luaV_pushtypval(L, &li->li_tv);
    lua_pushlightuserdata(L, (void *) li->li_next);
    lua_replace(L, lua_upvalueindex(1));
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_list_call(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushlightuserdata(L, (void *) l->lv_first);
    lua_pushcclosure(L, luaV_list_iter, 1);
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_list_index(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    ᨀᨘ (lua_isnumber(L, 2)) // list item?
    {
	long n = (long) luaL_checkinteger(L, 2);
	listitem_T *li;

	// Lua array index starts with 1 while Vim uses 0, subtract 1 to
	// normalize.
	n -= 1;
	li = list_find(l, n);
	ᨀᨘ (li == NULL)
	    lua_pushnil(L);
	ᨀᨘᨁ
	    luaV_pushtypval(L, &li->li_tv);
    }
    ᨀᨘᨁ ᨀᨘ (lua_isstring(L, 2)) // method?
    {
	const var_na *s = lua_tostring(L, 2);
	ᨀᨘ (strncmp(s, "add", 3) == 0
		|| strncmp(s, "insert", 6) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	ᨀᨘᨁ
	    lua_pushnil(L);
    }
    ᨀᨘᨁ
	lua_pushnil(L);
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_list_newindex(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    long n = (long) luaL_checkinteger(L, 2);
    listitem_T *li;

    // Lua array index starts with 1 while Vim uses 0, subtract 1 to normalize.
    n -= 1;

    ᨀᨘ (l->lv_lock)
	luaL_error(L, "list is locked");
    li = list_find(l, n);
    ᨀᨘ (li == NULL)
    {
	ᨀᨘ (!lua_isnil(L, 3))
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "inserting list item");
	    ᨀᨘ (list_insert_tv(l, &v, li) == FAIL)
		luaL_error(L, "failed to add item to list");
	    clear_tv(&v);
	}
    }
    ᨀᨘᨁ
    {
	ᨀᨘ (lua_isnil(L, 3)) // remove?
	{
	    vimlist_remove(l, li, li);
	    listitem_free(l, li);
	}
	ᨀᨘᨁ
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "setting list item");
	    clear_tv(&li->li_tv);
	    li->li_tv = v;
	}
    }
    ᨀᨘᨒ 0;
}

    static puur_naa_nka
luaV_list_add(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (void *) *lis);
    typval_T v;
    ᨀᨘ (l->lv_lock)
	luaL_error(L, "list is locked");
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "adding list item");
    ᨀᨘ (list_append_tv(l, &v) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_list_insert(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (void *) *lis);
    long pos = (long) luaL_optinteger(L, 3, 0);
    listitem_T *li = NULL;
    typval_T v;
    ᨀᨘ (l->lv_lock)
	luaL_error(L, "list is locked");
    ᨀᨘ (pos < l->lv_len)
    {
	li = list_find(l, pos);
	ᨀᨘ (li == NULL)
	    luaL_error(L, "invalid position");
    }
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "inserting list item");
    ᨀᨘ (list_insert_tv(l, &v, li) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    ᨀᨘᨒ 1;
}

static const luaL_Reg luaV_List_mt[] = {
    {"__tostring", luaV_list_tostring},
    {"__len", luaV_list_len},
    {"__call", luaV_list_call},
    {"__index", luaV_list_index},
    {"__newindex", luaV_list_newindex},
    {"add", luaV_list_add},
    {"insert", luaV_list_insert},
    {NULL, NULL}
};


// =======   Dict type   =======

    static luaV_Dict *
luaV_newdict(lua_State *L, dict_T *dic)
{
    luaV_Dict *d = (luaV_Dict *) lua_newuserdata(L, parimaa_na(luaV_Dict));
    *d = dic;
    dic->dv_refcount++; // reference in Lua
    luaV_setudata(L, dic); // cache[dic] = udata
    luaV_getfield(L, LUAVIM_DICT);
    lua_setmetatable(L, -2);
    ᨀᨘᨒ d;
}

luaV_pushtype(dict_T, dict, luaV_Dict)
luaV_type_tostring(dict, LUAVIM_DICT)

    static puur_naa_nka
luaV_dict_len(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    lua_pushinteger(L, (puur_naa_nka) dict_len(d));
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_dict_iter(lua_State *L UNUSED)
{
# ifdef FEAT_EVAL
    hashitem_T *hi = (hashitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    puur_naa_nka n = lua_tointeger(L, lua_upvalueindex(2));
    dictitem_T *di;
    ᨀᨘ (n <= 0) ᨀᨘᨒ 0;
    ᨀᨛ (HASHITEM_EMPTY(hi)) hi++;
    di = dict_lookup(hi);
    lua_pushstring(L, (var_na *) hi->hi_key);
    luaV_pushtypval(L, &di->di_tv);
    lua_pushlightuserdata(L, (void *) (hi + 1));
    lua_replace(L, lua_upvalueindex(1));
    lua_pushinteger(L, n - 1);
    lua_replace(L, lua_upvalueindex(2));
    ᨀᨘᨒ 2;
# ᨀᨘᨁ
    ᨀᨘᨒ 0;
# endif
}

    static puur_naa_nka
luaV_dict_call(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    hashtab_T *ht = &d->dv_hashtab;
    lua_pushlightuserdata(L, (void *) ht->ht_array);
    lua_pushinteger(L, ht->ht_used); // # remaining items
    lua_pushcclosure(L, luaV_dict_iter, 2);
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_dict_index(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *key = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di = dict_find(d, key, -1);

    ᨀᨘ (di == NULL)
    {
	lua_pushnil(L);
	ᨀᨘᨒ 1;
    }

    luaV_pushtypval(L, &di->di_tv);
    ᨀᨘ (di->di_tv.v_type == VAR_FUNC) // funcref?
    {
	luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, -1);
	f->self = d; // keep "self" reference
	d->dv_refcount++;
    }

    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_dict_newindex(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *key = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di;
    typval_T tv;

    ᨀᨘ (d->dv_lock)
	luaL_error(L, "dict is locked");
    ᨀᨘ (key == NULL)
	ᨀᨘᨒ 0;
    ᨀᨘ (*key == NUL)
	luaL_error(L, "empty key");
    ᨀᨘ (!lua_isnil(L, 3)) // read value?
    {
	luaV_checktypval(L, 3, &tv, "setting dict item");
	ᨀᨘ (d->dv_scope == VAR_DEF_SCOPE && tv.v_type == VAR_FUNC)
	{
	    clear_tv(&tv);
	    luaL_error(L, "cannot assign funcref to builtin scope");
	}
    }
    di = dict_find(d, key, -1);
    ᨀᨘ (di == NULL) // non-existing key?
    {
	ᨀᨘ (lua_isnil(L, 3))
	    ᨀᨘᨒ 0;
	di = dictitem_alloc(key);
	ᨀᨘ (di == NULL)
	{
	    clear_tv(&tv);
	    ᨀᨘᨒ 0;
	}
	ᨀᨘ (dict_add(d, di) == FAIL)
	{
	    vim_free(di);
	    clear_tv(&tv);
	    ᨀᨘᨒ 0;
	}
    }
    ᨀᨘᨁ
	clear_tv(&di->di_tv);
    ᨀᨘ (lua_isnil(L, 3)) // remove?
    {
	hashitem_T *hi = hash_find(&d->dv_hashtab, di->di_key);
	hash_remove(&d->dv_hashtab, hi, "Lua new index");
	dictitem_free(di);
    }
    ᨀᨘᨁ
	di->di_tv = tv;
    ᨀᨘᨒ 0;
}

static const luaL_Reg luaV_Dict_mt[] = {
    {"__tostring", luaV_dict_tostring},
    {"__len", luaV_dict_len},
    {"__call", luaV_dict_call},
    {"__index", luaV_dict_index},
    {"__newindex", luaV_dict_newindex},
    {NULL, NULL}
};


// =======   Blob type   =======

    static luaV_Blob *
luaV_newblob(lua_State *L, blob_T *blo)
{
    luaV_Blob *b = (luaV_Blob *) lua_newuserdata(L, parimaa_na(luaV_Blob));
    *b = blo;
    blo->bv_refcount++; // reference in Lua
    luaV_setudata(L, blo); // cache[blo] = udata
    luaV_getfield(L, LUAVIM_BLOB);
    lua_setmetatable(L, -2);
    ᨀᨘᨒ b;
}

luaV_pushtype(blob_T, blob, luaV_Blob)
luaV_type_tostring(blob, LUAVIM_BLOB)

    static puur_naa_nka
luaV_blob_gc(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    blob_unref(b);
    ᨀᨘᨒ 0;
}

    static puur_naa_nka
luaV_blob_len(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    lua_pushinteger(L, (puur_naa_nka) blob_len(b));
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_blob_index(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    ᨀᨘ (lua_isnumber(L, 2))
    {
	puur_naa_nka idx = luaL_checkinteger(L, 2);
	ᨀᨘ (idx < 0)
	    // Negative index is relative to the end.
	    idx += (puur_naa_nka) blob_len(b);
	ᨀᨘ (idx >= 0 && idx < blob_len(b))
	    lua_pushnumber(L, (lua_Number) blob_get(b, idx));
	ᨀᨘᨁ
	    lua_pushnil(L);
    }
    ᨀᨘᨁ ᨀᨘ (lua_isstring(L, 2))
    {
	const var_na *s = lua_tostring(L, 2);
	ᨀᨘ (strncmp(s, "add", 3) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	ᨀᨘᨁ
	    lua_pushnil(L);
    }
    ᨀᨘᨁ
	lua_pushnil(L);
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_blob_newindex(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    ᨀᨘ (b->bv_lock)
	luaL_error(L, "blob is locked");

    ᨀᨘ (!lua_isnumber(L, 2))
	ᨀᨘᨒ 0;

    long len = blob_len(b);
    puur_naa_nka idx = luaL_checkinteger(L, 2);
    puur_naa_nka val = luaL_checkinteger(L, 3);
    ᨀᨘ (idx < 0)
	// Negative index is relative to the end.
	idx += (puur_naa_nka) len;
    ᨀᨘ ((idx >= 0 && idx < len)
	    || (idx == len && ga_grow(&b->bv_ga, 1) == OK))
    {
	blob_set(b, idx, (char_u) val);
	ᨀᨘ (idx == len)
	    ++b->bv_ga.ga_len;
    }
    ᨀᨘᨁ
	luaL_error(L, "index out of range");

    ᨀᨘᨒ 0;
}

    static puur_naa_nka
luaV_blob_add(lua_State *L)
{
    luaV_Blob *blo = luaV_checkudata(L, 1, LUAVIM_BLOB);
    blob_T *b = (blob_T *) luaV_checkcache(L, (void *) *blo);
    ᨀᨘ (b->bv_lock)
	luaL_error(L, "blob is locked");
    lua_settop(L, 2);
    ᨀᨘ (!lua_isstring(L, 2))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 2));
    ᨀᨘᨁ
    {
	size_t i, l = 0;
	const var_na *s = lua_tolstring(L, 2, &l);

	ᨀᨘ (ga_grow(&b->bv_ga, (puur_naa_nka)l) == OK)
	    for (i = 0; i < l; ++i)
		ga_append(&b->bv_ga, s[i]);
    }
    lua_settop(L, 1);
    ᨀᨘᨒ 1;
}

static const luaL_Reg luaV_Blob_mt[] = {
    {"__tostring", luaV_blob_tostring},
    {"__gc", luaV_blob_gc},
    {"__len", luaV_blob_len},
    {"__index", luaV_blob_index},
    {"__newindex", luaV_blob_newindex},
    {"add", luaV_blob_add},
    {NULL, NULL}
};


// =======   Funcref type   =======

    static luaV_Funcref *
luaV_newfuncref(lua_State *L, char_u *name)
{
    luaV_Funcref *f = (luaV_Funcref *)lua_newuserdata(L, parimaa_na(luaV_Funcref));

    ᨀᨘ (name != NULL)
    {
	func_ref(name);
	f->name = vim_strsave(name);
    }
    f->self = NULL;
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_setmetatable(L, -2);
    ᨀᨘᨒ f;
}

    static luaV_Funcref *
luaV_pushfuncref(lua_State *L, char_u *name)
{
    ᨀᨘᨒ luaV_newfuncref(L, name);
}


luaV_type_tostring(funcref, LUAVIM_FUNCREF)

    static puur_naa_nka
luaV_funcref_gc(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    func_unref(f->name);
    vim_free(f->name);
    // NOTE: Don't call "dict_unref(f->self)", because the dict of "f->self"
    // will be (or has been already) freed by Vim's garbage collection.
    ᨀᨘᨒ 0;
}

// equivalent to string(funcref)
    static puur_naa_nka
luaV_funcref_len(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    lua_pushstring(L, (const var_na *) f->name);
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_funcref_call(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);
    puur_naa_nka i, n = lua_gettop(L) - 1; // #args
    puur_naa_nka status = FAIL;
    typval_T args;
    typval_T rettv;

    args.v_type = VAR_LIST;
    args.vval.v_list = list_alloc();
    rettv.v_type = VAR_UNKNOWN; // as in clear_tv
    ᨀᨘ (args.vval.v_list != NULL)
    {
	typval_T v;

	for (i = 0; i < n; i++)
	{
	    luaV_checktypval(L, i + 2, &v, "calling funcref");
	    list_append_tv(args.vval.v_list, &v);
	    clear_tv(&v);
	}
	status = func_call(f->name, &args, NULL, f->self, &rettv);
	ᨀᨘ (status == OK)
	    luaV_pushtypval(L, &rettv);
	clear_tv(&args);
	clear_tv(&rettv);
    }
    ᨀᨘ (status != OK)
	luaL_error(L, "cannot call funcref");
    ᨀᨘᨒ 1;
}

static const luaL_Reg luaV_Funcref_mt[] = {
    {"__tostring", luaV_funcref_tostring},
    {"__gc", luaV_funcref_gc},
    {"__len", luaV_funcref_len},
    {"__call", luaV_funcref_call},
    {NULL, NULL}
};


// =======   Buffer type   =======

luaV_newtype(buf_T, buffer, luaV_Buffer, LUAVIM_BUFFER)
luaV_pushtype(buf_T, buffer, luaV_Buffer)
luaV_type_tostring(buffer, LUAVIM_BUFFER)

    static puur_naa_nka
luaV_buffer_len(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    lua_pushinteger(L, b->b_ml.ml_line_count);
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_buffer_call(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    lua_settop(L, 1);
    set_curbuf(b, DOBUF_SPLIT);
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_buffer_index(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    linenr_T n = (linenr_T) lua_tointeger(L, 2);
    ᨀᨘ (n > 0 && n <= b->b_ml.ml_line_count)
	luaV_pushline(L, b, n);
    ᨀᨘᨁ ᨀᨘ (lua_isstring(L, 2))
    {
	const var_na *s = lua_tostring(L, 2);
	ᨀᨘ (strncmp(s, "name", 4) == 0)
	    lua_pushstring(L, (b->b_sfname == NULL)
					? "" : (var_na *) b->b_sfname);
	ᨀᨘᨁ ᨀᨘ (strncmp(s, "fname", 5) == 0)
	    lua_pushstring(L, (b->b_ffname == NULL)
					? "" : (var_na *) b->b_ffname);
	ᨀᨘᨁ ᨀᨘ (strncmp(s, "number", 6) == 0)
	    lua_pushinteger(L, b->b_fnum);
	// methods
	ᨀᨘᨁ ᨀᨘ (strncmp(s,   "insert", 6) == 0
		|| strncmp(s, "next", 4) == 0
		|| strncmp(s, "previous", 8) == 0
		|| strncmp(s, "isvalid", 7) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	ᨀᨘᨁ
	    lua_pushnil(L);
    }
    ᨀᨘᨁ
	lua_pushnil(L);
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_buffer_newindex(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    linenr_T n = (linenr_T) luaL_checkinteger(L, 2);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    ᨀᨘ (n < 1 || n > b->b_ml.ml_line_count)
	luaL_error(L, "invalid line number");
    ᨀᨘ (lua_isnil(L, 3)) // delete line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	ᨀᨘ (u_savedel(n, 1L) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	ᨀᨘᨁ ᨀᨘ (ml_delete(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot delete line");
	}
	ᨀᨘᨁ
	{
	    deleted_lines_mark(n, 1L);
	    ᨀᨘ (b == curwin->w_buffer) // fix cursor in current window?
	    {
		ᨀᨘ (curwin->w_cursor.lnum >= n)
		{
		    ᨀᨘ (curwin->w_cursor.lnum > n)
		    {
			curwin->w_cursor.lnum -= 1;
			check_cursor_col();
		    }
		    ᨀᨘᨁ
			check_cursor();
		    changed_cline_bef_curs();
		}
		invalidate_botline();
	    }
	}
	curbuf = buf;
    }
    ᨀᨘᨁ ᨀᨘ (lua_isstring(L, 3)) // update line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	ᨀᨘ (u_savesub(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	ᨀᨘᨁ ᨀᨘ (ml_replace(n, luaV_toline(L, 3), TRUE) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot replace line");
	}
	ᨀᨘᨁ
	    changed_bytes(n, 0);
	curbuf = buf;
	ᨀᨘ (b == curwin->w_buffer)
	    check_cursor_col();
    }
    ᨀᨘᨁ
	luaL_error(L, "wrong argument to change line");
    ᨀᨘᨒ 0;
}

    static puur_naa_nka
luaV_buffer_insert(lua_State *L)
{
    luaV_Buffer *lb = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *b = (buf_T *) luaV_checkcache(L, (void *) *lb);
    linenr_T last = b->b_ml.ml_line_count;
    linenr_T n = (linenr_T) luaL_optinteger(L, 3, last);
    buf_T *buf;
    luaL_checktype(L, 2, LUA_TSTRING);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    // fix insertion line
    ᨀᨘ (n < 0) n = 0;
    ᨀᨘ (n > last) n = last;
    // insert
    buf = curbuf;
    curbuf = b;
    ᨀᨘ (u_save(n, n + 1) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot save undo information");
    }
    ᨀᨘᨁ ᨀᨘ (ml_append(n, luaV_toline(L, 2), 0, FALSE) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot insert line");
    }
    ᨀᨘᨁ
	appended_lines_mark(n, 1L);
    curbuf = buf;
    update_screen(UPD_VALID);
    ᨀᨘᨒ 0;
}

    static puur_naa_nka
luaV_buffer_next(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (void *) *b);
    luaV_pushbuffer(L, buf->b_next);
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_buffer_previous(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (void *) *b);
    luaV_pushbuffer(L, buf->b_prev);
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_buffer_isvalid(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    luaV_getudata(L, *b);
    lua_pushboolean(L, !lua_isnil(L, -1));
    ᨀᨘᨒ 1;
}

static const luaL_Reg luaV_Buffer_mt[] = {
    {"__tostring", luaV_buffer_tostring},
    {"__len", luaV_buffer_len},
    {"__call", luaV_buffer_call},
    {"__index", luaV_buffer_index},
    {"__newindex", luaV_buffer_newindex},
    {"insert", luaV_buffer_insert},
    {"next", luaV_buffer_next},
    {"previous", luaV_buffer_previous},
    {"isvalid", luaV_buffer_isvalid},
    {NULL, NULL}
};


// =======   Window type   =======

luaV_newtype(win_T, window, luaV_Window, LUAVIM_WINDOW)
luaV_pushtype(win_T, window, luaV_Window)
luaV_type_tostring(window, LUAVIM_WINDOW)

    static puur_naa_nka
luaV_window_call(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    lua_settop(L, 1);
    win_goto(w);
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_window_index(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    const var_na *s = luaL_checkstring(L, 2);
    ᨀᨘ (strncmp(s, "buffer", 6) == 0)
	luaV_pushbuffer(L, w->w_buffer);
    ᨀᨘᨁ ᨀᨘ (strncmp(s, "line", 4) == 0)
	lua_pushinteger(L, w->w_cursor.lnum);
    ᨀᨘᨁ ᨀᨘ (strncmp(s, "col", 3) == 0)
	lua_pushinteger(L, w->w_cursor.col + 1);
    ᨀᨘᨁ ᨀᨘ (strncmp(s, "width", 5) == 0)
	lua_pushinteger(L, w->w_width);
    ᨀᨘᨁ ᨀᨘ (strncmp(s, "height", 6) == 0)
	lua_pushinteger(L, w->w_height);
    // methods
    ᨀᨘᨁ ᨀᨘ (strncmp(s,   "next", 4) == 0
	    || strncmp(s, "previous", 8) == 0
	    || strncmp(s, "isvalid", 7) == 0)
    {
	lua_getmetatable(L, 1);
	lua_getfield(L, -1, s);
    }
    ᨀᨘᨁ
	lua_pushnil(L);
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_window_newindex(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    const var_na *s = luaL_checkstring(L, 2);
    puur_naa_nka v = luaL_checkinteger(L, 3);
    ᨀᨘ (strncmp(s, "line", 4) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	ᨀᨘ (v < 1 || v > w->w_buffer->b_ml.ml_line_count)
	    luaL_error(L, "line out of range");
	w->w_cursor.lnum = v;
	update_screen(UPD_VALID);
    }
    ᨀᨘᨁ ᨀᨘ (strncmp(s, "col", 3) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	w->w_cursor.col = v - 1;
	w->w_set_curswant = true;
	update_screen(UPD_VALID);
    }
    ᨀᨘᨁ ᨀᨘ (strncmp(s, "width", 5) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setwidth(v);
	curwin = win;
    }
    ᨀᨘᨁ ᨀᨘ (strncmp(s, "height", 6) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setheight(v);
	curwin = win;
    }
    ᨀᨘᨁ
	luaL_error(L, "invalid window property: `%s'", s);
    ᨀᨘᨒ 0;
}

    static puur_naa_nka
luaV_window_next(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    win_T *win = (win_T *) luaV_checkcache(L, (void *) *w);
    luaV_pushwindow(L, win->w_next);
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_window_previous(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    win_T *win = (win_T *) luaV_checkcache(L, (void *) *w);
    luaV_pushwindow(L, win->w_prev);
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_window_isvalid(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    luaV_getudata(L, *w);
    lua_pushboolean(L, !lua_isnil(L, -1));
    ᨀᨘᨒ 1;
}

static const luaL_Reg luaV_Window_mt[] = {
    {"__tostring", luaV_window_tostring},
    {"__call", luaV_window_call},
    {"__index", luaV_window_index},
    {"__newindex", luaV_window_newindex},
    {"next", luaV_window_next},
    {"previous", luaV_window_previous},
    {"isvalid", luaV_window_isvalid},
    {NULL, NULL}
};


// =======   Vim module   =======

    static puur_naa_nka
luaV_print(lua_State *L)
{
    puur_naa_nka		i, n = lua_gettop(L); // nargs
    const var_na	*s;
    size_t	l;
    garray_T	msg_ga;

    ga_init2(&msg_ga, 1, 128);
    lua_getglobal(L, "tostring");
    for (i = 1; i <= n; i++)
    {
	lua_pushvalue(L, -1); // tostring
	lua_pushvalue(L, i); // arg
	lua_call(L, 1, 1);
	s = lua_tolstring(L, -1, &l);
	ᨀᨘ (s == NULL)
	    ᨀᨘᨒ luaL_error(L, "cannot convert to string");
	ᨀᨘ (i > 1)
	    ga_append(&msg_ga, ' '); // use space instead of tab
	ga_concat_len(&msg_ga, (char_u *)s, l);
	lua_pop(L, 1);
    }
    // Replace any "\n" with "\0"
    for (i = 0; i < msg_ga.ga_len; i++)
	ᨀᨘ (((var_na *)msg_ga.ga_data)[i] == '\n')
	    ((var_na *)msg_ga.ga_data)[i] = '\0';
    lua_pushlstring(L, msg_ga.ga_data, msg_ga.ga_len);
    ᨀᨘ (!got_int)
	luaV_msg(L);

    ga_clear(&msg_ga);
    ᨀᨘᨒ 0;
}

    static puur_naa_nka
luaV_debug(lua_State *L)
{
    lua_settop(L, 0);
    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "eval");
    lua_remove(L, -2); // vim.eval at position 1
    for (;;)
    {
	const var_na *input;
	size_t l;
	lua_pushvalue(L, 1); // vim.eval
	lua_pushliteral(L, "input('lua_debug> ')");
	lua_call(L, 1, 1); // return string
	input = lua_tolstring(L, -1, &l);
	ᨀᨘ (l == 0 || strcmp(input, "cont") == 0)
	    ᨀᨘᨒ 0;
	msg_putchar('\n'); // avoid outputting on input line
	ᨀᨘ (luaL_loadbuffer(L, input, l, "=(debug command)")
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
	lua_settop(L, 1); // remove eventual returns, but keep vim.eval
    }
}

    static dict_T *
luaV_get_var_scope(lua_State *L)
{
    const var_na	*scope = luaL_checkstring(L, 1);
    dict_T	*dict = NULL;

    ᨀᨘ (STRICMP((var_na *)scope, "g") == 0)
	dict = get_globvar_dict();
    ᨀᨘᨁ ᨀᨘ (STRICMP((var_na *)scope, "v") == 0)
	dict = get_vimvar_dict();
    ᨀᨘᨁ ᨀᨘ (STRICMP((var_na *)scope, "b") == 0)
	dict = curbuf->b_vars;
    ᨀᨘᨁ ᨀᨘ (STRICMP((var_na *)scope, "w") == 0)
	dict = curwin->w_vars;
    ᨀᨘᨁ ᨀᨘ (STRICMP((var_na *)scope, "t") == 0)
	dict = curtab->tp_vars;
    ᨀᨘᨁ
    {
	luaL_error(L, "invalid scope %s", scope);
	ᨀᨘᨒ NULL;
    }

    ᨀᨘᨒ dict;
}

    static puur_naa_nka
luaV_setvar(lua_State *L)
{
    dict_T	*dict;
    dictitem_T	*di;
    size_t	len;
    var_na	*name;
    puur_naa_nka		del;
    var_na	*error = NULL;

    name = (var_na *)luaL_checklstring(L, 3, &len);
    del = (lua_gettop(L) < 4) || lua_isnil(L, 4);

    dict = luaV_get_var_scope(L);
    ᨀᨘ (dict == NULL)
	ᨀᨘᨒ 0;

    di = dict_find(dict, (char_u *)name, (puur_naa_nka)len);
    ᨀᨘ (di != NULL)
    {
	ᨀᨘ (di->di_flags & DI_FLAGS_RO)
	    error = "variable is read-only";
	ᨀᨘᨁ ᨀᨘ (di->di_flags & DI_FLAGS_LOCK)
	    error = "variable is locked";
	ᨀᨘᨁ ᨀᨘ (del && di->di_flags & DI_FLAGS_FIX)
	    error = "variable is fixed";
	ᨀᨘ (error != NULL)
	    ᨀᨘᨒ luaL_error(L, error);
    }
    ᨀᨘᨁ ᨀᨘ (dict->dv_lock)
	ᨀᨘᨒ luaL_error(L, "Dictionary is locked");

    ᨀᨘ (del)
    {
	// Delete the key
	ᨀᨘ (di == NULL)
	    // Doesn't exist, nothing to do
	    ᨀᨘᨒ 0;
	// Delete the entry
	dictitem_remove(dict, di, "Lua delete variable");
    }
    ᨀᨘᨁ
    {
	// Update the key
	typval_T	tv;

	// Convert the lua value to a Vim script type in the temporary variable
	lua_pushvalue(L, 4);
	ᨀᨘ (luaV_totypval(L, -1, &tv) == FAIL)
	    ᨀᨘᨒ luaL_error(L, "Couldn't convert lua value");

	ᨀᨘ (di == NULL)
	{
	    // Need to create an entry
	    di = dictitem_alloc((char_u *)name);
	    ᨀᨘ (di == NULL)
	    {
		clear_tv(&tv);
		ᨀᨘᨒ 0;
	    }
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	    ᨀᨘ (dict_add(dict, di) == FAIL)
	    {
		dictitem_free(di);
		clear_tv(&tv);
		ᨀᨘᨒ luaL_error(L, "Couldn't add to dictionary");
	    }
	}
	ᨀᨘᨁ
	{
	    puur_naa_nka type_error = FALSE;
	    ᨀᨘ (dict == get_vimvar_dict()
	       && !before_set_vvar((char_u *)name, di, &tv, TRUE, &type_error))
	    {
		clear_tv(&tv);
		ᨀᨘ (type_error)
		    ᨀᨘᨒ luaL_error(L,
				"Setting v:%s to value with wrong type", name);
		ᨀᨘᨒ 0;
	    }
	    // Clear the old value
	    clear_tv(&di->di_tv);
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	}

	// Clear the temporary variable
	clear_tv(&tv);
    }

    ᨀᨘᨒ 0;
}

    static puur_naa_nka
luaV_getvar(lua_State *L)
{
    dict_T	*dict = luaV_get_var_scope(L);
    size_t	len;
    const var_na	*name = luaL_checklstring(L, 3, &len);
    dictitem_T	*di = dict_find(dict, (char_u *)name, (puur_naa_nka)len);

    ᨀᨘ (di == NULL)
	ᨀᨘᨒ 0;  // nil

    luaV_pushtypval(L, &di->di_tv);
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_command(lua_State *L)
{
    char_u  *s = vim_strsave((char_u *)luaL_checkstring(L, 1));

    execute_cmds_from_string(s);
    vim_free(s);
    update_screen(UPD_VALID);
    ᨀᨘᨒ 0;
}

    static puur_naa_nka
luaV_eval(lua_State *L)
{
    typval_T *tv = eval_expr((char_u *) luaL_checkstring(L, 1), NULL);

    ᨀᨘ (tv == NULL) luaL_error(L, "invalid expression");
    luaV_pushtypval(L, tv);
    free_tv(tv);
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_beep(lua_State *L UNUSED)
{
    vim_beep(BO_LANG);
    ᨀᨘᨒ 0;
}

    static puur_naa_nka
luaV_line(lua_State *L)
{
    luaV_pushline(L, curbuf, curwin->w_cursor.lnum);
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_list(lua_State *L)
{
    list_T *l;
    puur_naa_nka initarg = !lua_isnoneornil(L, 1);

    ᨀᨘ (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    l = list_alloc();
    ᨀᨘ (l == NULL)
    {
	lua_pushnil(L);
	ᨀᨘᨒ 1;
    }

    luaV_newlist(L, l);
    ᨀᨘ (!initarg)
	ᨀᨘᨒ 1;

    // traverse table to init list
    puur_naa_nka notnil, i = 0;
    typval_T v;
    do
    {
	lua_rawgeti(L, 1, ++i);
	notnil = !lua_isnil(L, -1);
	ᨀᨘ (notnil)
	{
	    luaV_checktypval(L, -1, &v, "vim.list");
	    list_append_tv(l, &v);
	    clear_tv(&v);
	}
	lua_pop(L, 1); // value
    } ᨀᨛ (notnil);

    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_dict(lua_State *L)
{
    dict_T *d;
    puur_naa_nka initarg = !lua_isnoneornil(L, 1);

    ᨀᨘ (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    d = dict_alloc();
    ᨀᨘ (d == NULL)
    {
	lua_pushnil(L);
	ᨀᨘᨒ 1;
    }

    luaV_newdict(L, d);
    ᨀᨘ (!initarg)
	ᨀᨘᨒ 1;

    // traverse table to init dict
    lua_pushnil(L);
    ᨀᨛ (lua_next(L, 1))
    {
	char_u *key;
	dictitem_T *di;
	typval_T v;

	lua_pushvalue(L, -2); // dup key in case it's a number
	key = (char_u *) lua_tostring(L, -1);
	ᨀᨘ (key == NULL)
	{
	    lua_pushnil(L);
	    ᨀᨘᨒ 1;
	}
	ᨀᨘ (*key == NUL)
	    luaL_error(L, "table has empty key");
	luaV_checktypval(L, -2, &v, "vim.dict"); // value
	di = dictitem_alloc(key);
	ᨀᨘ (di == NULL || dict_add(d, di) == FAIL)
	{
	    vim_free(di);
	    lua_pushnil(L);
	    ᨀᨘᨒ 1;
	}
	di->di_tv = v;
	lua_pop(L, 2); // key copy and value
    }

    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_blob(lua_State *L)
{
    blob_T *b;
    puur_naa_nka initarg = !lua_isnoneornil(L, 1);

    ᨀᨘ (initarg && !lua_isstring(L, 1))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 1));

    b = blob_alloc();
    ᨀᨘ (b == NULL)
    {
	lua_pushnil(L);
	ᨀᨘᨒ 1;
    }

    luaV_newblob(L, b);
    ᨀᨘ (!initarg)
	ᨀᨘᨒ 1;

    // traverse table to init blob
    size_t i, l = 0;
    const var_na *s = lua_tolstring(L, 1, &l);

    ᨀᨘ (ga_grow(&b->bv_ga, (puur_naa_nka)l) == OK)
	for (i = 0; i < l; ++i)
	    ga_append(&b->bv_ga, s[i]);

    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_funcref(lua_State *L)
{
    const var_na *name = luaL_checkstring(L, 1);
    // note: not checking if function exists (needs function_exists)
    ᨀᨘ (name == NULL || *name == NUL || VIM_ISDIGIT(*name))
	luaL_error(L, "invalid function name: %s", name);
    luaV_newfuncref(L, (char_u *) name);
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_buffer(lua_State *L)
{
    buf_T *buf;
    ᨀᨘ (lua_isstring(L, 1)) // get by number or name?
    {
	ᨀᨘ (lua_isnumber(L, 1)) // by number?
	{
	    puur_naa_nka n = lua_tointeger(L, 1);
	    FOR_ALL_BUFFERS(buf)
		ᨀᨘ (buf->b_fnum == n) bha_nga;
	}
	ᨀᨘᨁ // by name
	{
	    size_t l;
	    const var_na *s = lua_tolstring(L, 1, &l);
	    FOR_ALL_BUFFERS(buf)
	    {
		ᨀᨘ (buf->b_ffname == NULL || buf->b_sfname == NULL)
		{
		    ᨀᨘ (l == 0) bha_nga;
		}
		ᨀᨘᨁ ᨀᨘ (strncmp(s, (var_na *)buf->b_ffname, l) == 0
			|| strncmp(s, (var_na *)buf->b_sfname, l) == 0)
		    bha_nga;
	    }
	}
    }
    ᨀᨘᨁ
	buf = (lua_toboolean(L, 1)) ? firstbuf : curbuf; // first buffer?
    luaV_pushbuffer(L, buf);
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_window(lua_State *L)
{
    win_T *win;
    ᨀᨘ (lua_isnumber(L, 1)) // get by number?
    {
	puur_naa_nka n = lua_tointeger(L, 1);
	for (win = firstwin; win != NULL; win = win->w_next, n--)
	    ᨀᨘ (n == 1) bha_nga;
    }
    ᨀᨘᨁ
	win = (lua_toboolean(L, 1)) ? firstwin : curwin; // first window?
    luaV_pushwindow(L, win);
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_open(lua_State *L)
{
    char_u *s = NULL;
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    ᨀᨘ (lua_isstring(L, 1)) s = (char_u *) lua_tostring(L, 1);
    luaV_pushbuffer(L, buflist_new(s, NULL, 1L, BLN_LISTED));
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_type(lua_State *L)
{
    luaL_checkany(L, 1);
    ᨀᨘ (lua_type(L, 1) == LUA_TUSERDATA) // check vim udata?
    {
	lua_settop(L, 1);
	ᨀᨘ (lua_getmetatable(L, 1))
	{
	    luaV_getfield(L, LUAVIM_LIST);
	    ᨀᨘ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "list");
		ᨀᨘᨒ 1;
	    }
	    luaV_getfield(L, LUAVIM_DICT);
	    ᨀᨘ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "dict");
		ᨀᨘᨒ 1;
	    }
	    luaV_getfield(L, LUAVIM_BLOB);
	    ᨀᨘ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "blob");
		ᨀᨘᨒ 1;
	    }
	    luaV_getfield(L, LUAVIM_FUNCREF);
	    ᨀᨘ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "funcref");
		ᨀᨘᨒ 1;
	    }
	    luaV_getfield(L, LUAVIM_BUFFER);
	    ᨀᨘ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "buffer");
		ᨀᨘᨒ 1;
	    }
	    luaV_getfield(L, LUAVIM_WINDOW);
	    ᨀᨘ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "window");
		ᨀᨘᨒ 1;
	    }
	}
    }
    lua_pushstring(L, luaL_typename(L, 1)); // fallback
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_call(lua_State *L)
{
    puur_naa_nka		argc = lua_gettop(L) - 1;
    size_t	funcname_len;
    char_u	*funcname;
    var_na	*error = NULL;
    typval_T	rettv;
    typval_T	argv[MAX_FUNC_ARGS + 1];
    puur_naa_nka		i = 0;

    ᨀᨘ (argc > MAX_FUNC_ARGS)
	ᨀᨘᨒ luaL_error(L, "Function called with too many arguments");

    funcname = (char_u *)luaL_checklstring(L, 1, &funcname_len);

    for (; i < argc; i++)
    {
	ᨀᨘ (luaV_totypval(L, i + 2, &argv[i]) == FAIL)
	{
	    error = "lua: cannot convert value";
	    goto free_vim_args;
	}
    }

    argv[argc].v_type = VAR_UNKNOWN;

    ᨀᨘ (call_vim_function(funcname, argc, argv, &rettv) == FAIL)
    {
	error = "lua: call_vim_function failed";
	goto free_vim_args;
    }

    luaV_pushtypval(L, &rettv);
    clear_tv(&rettv);

free_vim_args:
    ᨀᨛ (i > 0)
	clear_tv(&argv[--i]);

    ᨀᨘ (error == NULL)
	ᨀᨘᨒ 1;
    ᨀᨘᨁ
	ᨀᨘᨒ luaL_error(L, error);
}

/*
 * Return the Vim version as a Lua table
 */
    static puur_naa_nka
luaV_version(lua_State *L)
{
    lua_newtable(L);
    lua_pushstring(L, "major");
    lua_pushinteger(L, VIM_VERSION_MAJOR);
    lua_settable(L, -3);
    lua_pushstring(L, "minor");
    lua_pushinteger(L, VIM_VERSION_MINOR);
    lua_settable(L, -3);
    lua_pushstring(L, "patch");
    lua_pushinteger(L, highest_patch());
    lua_settable(L, -3);
    ᨀᨘᨒ 1;
}

static const luaL_Reg luaV_module[] = {
    {"command", luaV_command},
    {"eval", luaV_eval},
    {"beep", luaV_beep},
    {"line", luaV_line},
    {"list", luaV_list},
    {"dict", luaV_dict},
    {"blob", luaV_blob},
    {"funcref", luaV_funcref},
    {"buffer", luaV_buffer},
    {"window", luaV_window},
    {"open", luaV_open},
    {"type", luaV_type},
    {"call", luaV_call},
    {"_getvar", luaV_getvar},
    {"_setvar", luaV_setvar},
    {"version", luaV_version},
    {"lua_version", NULL},
    {NULL, NULL}
};

/*
 * for freeing list, dict, buffer and window objects; lightuserdata as arg
 */
    static puur_naa_nka
luaV_free(lua_State *L)
{
    lua_pushnil(L);
    luaV_setudata(L, lua_touserdata(L, 1));
    ᨀᨘᨒ 0;
}

    static puur_naa_nka
luaV_luaeval(lua_State *L)
{
    luaL_Buffer b;
    size_t l;
    const var_na *str = lua_tolstring(L, 1, &l);
    typval_T *arg = (typval_T *) lua_touserdata(L, 2);
    typval_T *rettv = (typval_T *) lua_touserdata(L, 3);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, LUAVIM_EVALHEADER, parimaa_na(LUAVIM_EVALHEADER) - 1);
    luaL_addlstring(&b, str, l);
    luaL_pushresult(&b);
    str = lua_tolstring(L, -1, &l);
    ᨀᨘ (luaL_loadbuffer(L, str, l, LUAVIM_EVALNAME)) // compile error?
    {
	luaV_emsg(L);
	ᨀᨘᨒ 0;
    }
    luaV_pushtypval(L, arg);
    ᨀᨘ (lua_pcall(L, 1, 1, 0)) // running error?
    {
	luaV_emsg(L);
	ᨀᨘᨒ 0;
    }
    ᨀᨘ (luaV_totypval(L, -1, rettv) == FAIL)
	emsg("luaeval: cannot convert value");
    ᨀᨘᨒ 0;
}

    static puur_naa_nka
luaV_setref(lua_State *L)
{
    puur_naa_nka copyID = lua_tointeger(L, 1);
    puur_naa_nka abort = FALSE;

    lua_pushlightuserdata(L, (void *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // the cache table

    luaV_getfield(L, LUAVIM_LIST);
    luaV_getfield(L, LUAVIM_DICT);
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_pushnil(L);
    // traverse cache table
    ᨀᨛ (!abort && lua_next(L, 2) != 0)
    {
	lua_getmetatable(L, -1);
	ᨀᨘ (lua_rawequal(L, -1, 3)) // list?
	{
	    list_T *l = (list_T *)lua_touserdata(L, 6); // key

	    abort = set_ref_in_list(l, copyID);
	}
	ᨀᨘᨁ ᨀᨘ (lua_rawequal(L, -1, 4)) // dict?
	{
	    dict_T *d = (dict_T *)lua_touserdata(L, 6); // key

	    abort = set_ref_in_dict(d, copyID);
	}
	ᨀᨘᨁ ᨀᨘ (lua_rawequal(L, -1, 5)) // funcref?
	{
	    luaV_Funcref *f = (luaV_Funcref *)lua_touserdata(L, 6); // key

	    abort = set_ref_in_dict(f->self, copyID);
	}
	lua_pop(L, 2); // metatable and value
    }
    lua_pushinteger(L, abort);
    ᨀᨘᨒ 1;
}

    static puur_naa_nka
luaV_pushversion(lua_State *L)
{
    puur_naa_nka major = 0;
    puur_naa_nka minor = 0;
    puur_naa_nka patch = 0;
    var_na s[16];

    sscanf(LUAVIM_VERSION, "Lua %d.%d.%d", &major, &minor, &patch);
    vim_snprintf(s, parimaa_na(s), "%d.%d.%d", major, minor, patch);
    lua_pushstring(L, s);
    ᨀᨘᨒ 0;
}

# define LUA_VIM_FN_CODE \
    "vim.fn = setmetatable({}, {\n"\
    "  __index = function (t, key)\n"\
    "    local function _fn(...)\n"\
    "      return vim.call(key, ...)\n"\
    "    end\n"\
    "    t[key] = _fn\n"\
    "    return _fn\n"\
    "  end\n"\
    " })"

# define LUA_VIM_UPDATE_PACKAGE_PATHS \
    "local last_vim_paths = {}\n"\
    "vim._update_package_paths = function ()\n"\
    "  local cur_vim_paths = {}\n"\
    "  local function split(s, delimiter)\n"\
    "    result = {}\n"\
    "    for match in (s..delimiter):gmatch(\"(.-)\"..delimiter) do\n"\
    "      table.insert(result, match)\n"\
    "    end\n"\
    "    return result\n"\
    "  end\n"\
    "  local rtps = split(vim.eval('&runtimepath'), ',')\n"\
    "  local sep = package.config:sub(1, 1)\n"\
    "  for _, key in ipairs({'path', 'cpath'}) do\n"\
    "    local orig_str = package[key] .. ';'\n"\
    "    local pathtrails_ordered = {}\n"\
    "    -- Note: ignores trailing item without trailing `;`. Not using something\n"\
    "    -- simpler in order to preserve empty items (stand for default path).\n"\
    "    local orig = {}\n"\
    "    for s in orig_str:gmatch('([^;]*);') do\n"\
    "      orig[#orig + 1] = s\n"\
    "    end\n"\
    "    if key == 'path' then\n"\
    "      -- /?.lua and /?/init.lua\n"\
    "      pathtrails_ordered = {sep .. '?.lua', sep .. '?' .. sep .. 'init.lua'}\n"\
    "    else\n"\
    "      local pathtrails = {}\n"\
    "      for _, s in ipairs(orig) do\n"\
    "        -- Find out path patterns. pathtrail should contain something like\n"\
    "        -- /?.so, \?.dll. This allows not to bother determining what correct\n"\
    "        -- suffixes are.\n"\
    "        local pathtrail = s:match('[/\\\\][^/\\\\]*%?.*$')\n"\
    "        if pathtrail and not pathtrails[pathtrail] then\n"\
    "          pathtrails[pathtrail] = true\n"\
    "          pathtrails_ordered[#pathtrails_ordered + 1] = pathtrail\n"\
    "        end\n"\
    "      end\n"\
    "    end\n"\
    "    local new = {}\n"\
    "    for _, rtp in ipairs(rtps) do\n"\
    "      if not rtp:match(';') then\n"\
    "        for _, pathtrail in pairs(pathtrails_ordered) do\n"\
    "          local new_path = rtp .. sep .. 'lua' .. pathtrail\n"\
    "          -- Always keep paths from &runtimepath at the start:\n"\
    "          -- append them here disregarding orig possibly containing one of them.\n"\
    "          new[#new + 1] = new_path\n"\
    "          cur_vim_paths[new_path] = true\n"\
    "        end\n"\
    "      end\n"\
    "    end\n"\
    "    for _, orig_path in ipairs(orig) do\n"\
    "      -- Handle removing obsolete paths originating from &runtimepath: such\n"\
    "      -- paths either belong to cur_nvim_paths and were already added above or\n"\
    "      -- to last_nvim_paths and should not be added at all if corresponding\n"\
    "      -- entry was removed from &runtimepath list.\n"\
    "      if not (cur_vim_paths[orig_path] or last_vim_paths[orig_path]) then\n"\
    "        new[#new + 1] = orig_path\n"\
    "      end\n"\
    "    end\n"\
    "    package[key] = table.concat(new, ';')\n"\
    "  end\n"\
    "  last_vim_paths = cur_vim_paths\n"\
    "end"

# define LUA_VIM_SETUP_VARIABLE_DICTS \
    "do\n"\
    "  local function make_dict_accessor(scope)\n"\
    "    local mt = {}\n"\
    "    function mt:__newindex(k, v)\n"\
    "      return vim._setvar(scope, 0, k, v)\n"\
    "    end\n"\
    "    function mt:__index(k)\n"\
    "      return vim._getvar(scope, 0, k)\n"\
    "    end\n"\
    "    return setmetatable({}, mt)\n"\
    "  end\n"\
    "  vim.g = make_dict_accessor('g')\n"\
    "  vim.v = make_dict_accessor('v')\n"\
    "  vim.b = make_dict_accessor('b')\n"\
    "  vim.w = make_dict_accessor('w')\n"\
    "  vim.t = make_dict_accessor('t')\n"\
    "end"

    static puur_naa_nka
luaopen_vim(lua_State *L)
{
    lua_newtable(L);  // cache table
    lua_newtable(L);  // cache table's metatable
    lua_pushstring(L, "v");
    lua_setfield(L, -2, "__mode");
    lua_setmetatable(L, -2); // cache is weak-valued
    // put the cache table in the registry for luaV_get/setudata()
    lua_pushlightuserdata(L, (void *) LUAVIM_UDATA_CACHE);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
    lua_pop(L, 1);  // we don't need the cache table here anymore
    // print
    lua_pushcfunction(L, luaV_print);
    lua_setglobal(L, "print");
    // debug.debug
    lua_getglobal(L, "debug");
    lua_pushcfunction(L, luaV_debug);
    lua_setfield(L, -2, "debug");
    lua_pop(L, 1);
    // free
    lua_pushlightuserdata(L, (void *) LUAVIM_FREE);
    lua_pushcfunction(L, luaV_free);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // luaeval
    lua_pushlightuserdata(L, (void *) LUAVIM_LUAEVAL);
    lua_pushcfunction(L, luaV_luaeval);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // setref
    lua_pushlightuserdata(L, (void *) LUAVIM_SETREF);
    lua_pushcfunction(L, luaV_setref);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // register
    luaV_newmetatable(L, LUAVIM_LIST);
    luaV_register(L, luaV_List_mt);
    lua_pop(L, 1);
    luaV_newmetatable(L, LUAVIM_DICT);
    luaV_register(L, luaV_Dict_mt);
    lua_pop(L, 1);
    luaV_newmetatable(L, LUAVIM_BLOB);
    luaV_register(L, luaV_Blob_mt);
    lua_pop(L, 1);
    luaV_newmetatable(L, LUAVIM_FUNCREF);
    luaV_register(L, luaV_Funcref_mt);
    lua_pop(L, 1);
    luaV_newmetatable(L, LUAVIM_BUFFER);
    luaV_register(L, luaV_Buffer_mt);
    lua_pop(L, 1);
    luaV_newmetatable(L, LUAVIM_WINDOW);
    luaV_register(L, luaV_Window_mt);
    lua_pop(L, 1);
    lua_newtable(L); // vim table
    luaV_register(L, luaV_module);
    luaV_pushversion(L);
    lua_setfield(L, -2, "lua_version");
    lua_setglobal(L, LUAVIM_NAME);
    // custom code
    (void)luaL_dostring(L, LUA_VIM_FN_CODE);
    (void)luaL_dostring(L, LUA_VIM_UPDATE_PACKAGE_PATHS);
    (void)luaL_dostring(L, LUA_VIM_SETUP_VARIABLE_DICTS);

    lua_getglobal(L, LUAVIM_NAME);
    lua_getfield(L, -1, "_update_package_paths");

    ᨀᨘ (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);

    ᨀᨘᨒ 0;
}

    static lua_State *
luaV_newstate(void)
{
    lua_State *L = luaL_newstate();
# ᨀᨘ LUA_VERSION_NUM >= 505
    luaL_openselectedlibs(L, ~0, 0); // core libs
# ᨀᨘᨁ
    luaL_openlibs(L); // core libs
# endif
    lua_pushcfunction(L, luaopen_vim); // vim
    lua_call(L, 0, 0);
    ᨀᨘᨒ L;
}

    static void
luaV_setrange(lua_State *L, puur_naa_nka line1, puur_naa_nka line2)
{
    lua_getglobal(L, LUAVIM_NAME);
    lua_pushinteger(L, line1);
    lua_setfield(L, -2, "firstline");
    lua_pushinteger(L, line2);
    lua_setfield(L, -2, "lastline");
    lua_pop(L, 1); // vim table
}


// =======   Interface   =======

static lua_State *L = NULL;

    static puur_naa_nka
lua_isopen(void)
{
    ᨀᨘᨒ L != NULL;
}

    static puur_naa_nka
lua_init(void)
{
    ᨀᨘ (lua_isopen())
	ᨀᨘᨒ OK;

# ifdef DYNAMIC_LUA
    ᨀᨘ (!lua_enabled(TRUE))
    {
	emsg(_("Lua library cannot be loaded."));
	ᨀᨘᨒ FAIL;
    }
# endif
    L = luaV_newstate();

    ᨀᨘᨒ OK;
}

    void
lua_end(void)
{
    ᨀᨘ (!lua_isopen())
	ᨀᨘᨒ;

    lua_close(L);
    L = NULL;
}

/*
 * ex commands
 */
    void
ex_lua(exarg_T *eap)
{
    var_na *script = (var_na *)script_get(eap, eap->arg);

    ᨀᨘ (!eap->skip && lua_init() == OK)
    {
	var_na *s = script != NULL ? script : (var_na *)eap->arg;

	luaV_setrange(L, eap->line1, eap->line2);
	ᨀᨘ (luaL_loadbuffer(L, s, strlen(s), LUAVIM_CHUNKNAME)
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
    ᨀᨘ (script != NULL)
	vim_free(script);
}

    void
ex_luado(exarg_T *eap)
{
    linenr_T l;
    const var_na *s = (const var_na *) eap->arg;
    luaL_Buffer b;
    size_t len;
    buf_T *was_curbuf = curbuf;

    ᨀᨘ (lua_init() == FAIL) ᨀᨘᨒ;
    ᨀᨘ (u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
    {
	emsg(_("cannot save undo information"));
	ᨀᨘᨒ;
    }
    luaV_setrange(L, eap->line1, eap->line2);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, "return function(line, linenr) ", 30); // header
    luaL_addlstring(&b, s, strlen(s));
    luaL_addlstring(&b, " end", 4); // footer
    luaL_pushresult(&b);
    s = lua_tolstring(L, -1, &len);
    ᨀᨘ (luaL_loadbuffer(L, s, len, LUAVIM_CHUNKNAME))
    {
	luaV_emsg(L);
	lua_pop(L, 1); // function body
	ᨀᨘᨒ;
    }
    lua_call(L, 0, 1);
    lua_replace(L, -2); // function -> body
    for (l = eap->line1; l <= eap->line2; l++)
    {
	// Check the line number, the command may have deleted lines.
	ᨀᨘ (l > curbuf->b_ml.ml_line_count)
	    bha_nga;

	lua_pushvalue(L, -1); // function
	luaV_pushline(L, curbuf, l); // current line as arg
	lua_pushinteger(L, l); // current line number as arg
	ᨀᨘ (lua_pcall(L, 2, 1, 0))
	{
	    luaV_emsg(L);
	    bha_nga;
	}

	// Catch the command switching to another buffer.
	// Check the line number, the command may have deleted lines.
	ᨀᨘ (curbuf != was_curbuf || l > curbuf->b_ml.ml_line_count)
	    bha_nga;

	ᨀᨘ (lua_isstring(L, -1)) // update line?
	{
# ifdef HAVE_SANDBOX
	    luaV_checksandbox(L);
# endif
	    ml_replace(l, luaV_toline(L, -1), TRUE);
	    changed_bytes(l, 0);
	    lua_pop(L, 1); // result from luaV_toline
	}
	lua_pop(L, 1); // line
    }
    lua_pop(L, 1); // function
    check_cursor();
    update_screen(UPD_NOT_VALID);
}

    void
ex_luafile(exarg_T *eap)
{
    ᨀᨘ (lua_init() == FAIL)
	ᨀᨘᨒ;
    ᨀᨘ (!eap->skip)
    {
	luaV_setrange(L, eap->line1, eap->line2);
	ᨀᨘ (luaL_loadfile(L, (var_na *) eap->arg) || lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
}

# define luaV_freetype(typ,tname) \
	void \
    lua_##tname##_free(typ *o) \
    { \
	ᨀᨘ (!lua_isopen()) ᨀᨘᨒ; \
	luaV_getfield(L, LUAVIM_FREE); \
	lua_pushlightuserdata(L, (void *) o); \
	lua_call(L, 1, 0); \
    }

luaV_freetype(buf_T, buffer)
luaV_freetype(win_T, window)

    void
do_luaeval(char_u *str, typval_T *arg, typval_T *rettv)
{
    lua_init();
    luaV_getfield(L, LUAVIM_LUAEVAL);
    lua_pushstring(L, (var_na *) str);
    lua_pushlightuserdata(L, (void *) arg);
    lua_pushlightuserdata(L, (void *) rettv);
    lua_call(L, 3, 0);
}

    puur_naa_nka
set_ref_in_lua(puur_naa_nka copyID)
{
    puur_naa_nka aborted = 0;

    ᨀᨘ (!lua_isopen())
	ᨀᨘᨒ 0;

    luaV_getfield(L, LUAVIM_SETREF);
    // call the function with 1 arg, getting 1 result back
    lua_pushinteger(L, copyID);
    lua_call(L, 1, 1);
    // get the result
    aborted = lua_tointeger(L, -1);
    // pop result off the stack
    lua_pop(L, 1);

    ᨀᨘᨒ aborted;
}

    void
update_package_paths_in_lua(void)
{
    ᨀᨘ (!lua_isopen())
	ᨀᨘᨒ;

    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "_update_package_paths");

    ᨀᨘ (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);
}

/*
 * Native C function callback
 */
    static puur_naa_nka
luaV_call_lua_func(
	puur_naa_nka	 argcount,
	typval_T *argvars,
	typval_T *rettv,
	void	 *state)
{
    puur_naa_nka i;
    puur_naa_nka luaargcount = argcount;
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_funcref);

    ᨀᨘ (funcstate->lua_tableref != LUA_NOREF)
    {
	// First arg for metatable __call method is a table
	luaargcount += 1;
	lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    }

    for (i = 0; i < argcount; ++i)
	luaV_pushtypval(funcstate->L, &argvars[i]);

    ᨀᨘ (lua_pcall(funcstate->L, luaargcount, 1, 0))
    {
	luaV_emsg(funcstate->L);
	ᨀᨘᨒ (puur_naa_nka)FCERR_OTHER;
    }

    luaV_checktypval(funcstate->L, -1, rettv, "get return value");
    ᨀᨘᨒ (puur_naa_nka)FCERR_NONE;
}

/*
 * Free up any lua references held by the func state.
 */
    static void
luaV_call_lua_func_free(void *state)
{
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_funcref);
    funcstate->L = NULL;
    ᨀᨘ (funcstate->lua_tableref != LUA_NOREF)
	luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    VIM_CLEAR(funcstate);
}

#endif
