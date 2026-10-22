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

#అన్తర్భావ "vim.h"
#అన్తర్భావ "version.h"

#అన్తర్భావ <lua.h>
#అన్తర్భావ <lualib.h>
#అన్తర్భావ <lauxlib.h>

#ఒకవేళ __STDC_VERSION__ >= 199901L
# define LUAV_INLINE అంతఃవరుస
#లేకపోతే
# define LUAV_INLINE
#అయితే_ముగింపు

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#ఒకవేళ defined(FEAT_LUA)

# define LUAVIM_CHUNKNAME "vim chunk"
# define LUAVIM_NAME "vim"
# define LUAVIM_EVALNAME "luaeval"
# define LUAVIM_EVALHEADER "local _A=select(1,...) return "

# ifdef LUA_RELEASE
#  define LUAVIM_సంస్కరణ LUA_RELEASE
# లేకపోతే
#  define LUAVIM_సంస్కరణ LUA_సంస్కరణ
# endif

రకనిర్వచనం buf_T *lua_V_బఫరు;
రకనిర్వచనం win_T *lua_V_కిటికీ;
రకనిర్వచనం dict_T *luaV_Dict;
రకనిర్వచనం list_T *luaV_List;
రకనిర్వచనం blob_T *luaV_Blob;
రకనిర్వచనం నిర్మాణం {
    char_u	*name;	// funcref
    dict_T	*self;	// selfdict
} luaV_Funcref;
రకనిర్వచనం పూర్ణాంకం (*msgfunc_T)(అక్షరం *);

రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం lua_funcref;    // ref to a lua func
    పూర్ణాంకం lua_tableref;   // ref to a lua table if metatable else LUA_NOREF. used
			// for __call
    lua_State *L;
} luaV_CFuncState;

స్థిర స్థిరరాశి అక్షరం LUAVIM_DICT[] = "dict";
స్థిర స్థిరరాశి అక్షరం LUAVIM_LIST[] = "list";
స్థిర స్థిరరాశి అక్షరం LUAVIM_BLOB[] = "blob";
స్థిర స్థిరరాశి అక్షరం LUAVIM_FUNCREF[] = "funcref";
స్థిర స్థిరరాశి అక్షరం LUAVIM_బఫరు[] = "buffer";
స్థిర స్థిరరాశి అక్షరం LUAVIM_కిటికీ[] = "window";
స్థిర స్థిరరాశి అక్షరం LUAVIM_FREE[] = "luaV_free";
స్థిర స్థిరరాశి అక్షరం LUAVIM_LUAEVAL[] = "luaV_luaeval";
స్థిర స్థిరరాశి అక్షరం LUAVIM_SETREF[] = "luaV_setref";

స్థిర స్థిరరాశి అక్షరం LUA___CALL[] = "__call";

// get/setudata manage references to vim userdata in a cache table through
// object pointers (light userdata). The cache table itself is retrieved
// from the registry.

స్థిర స్థిరరాశి అక్షరం LUAVIM_UDATA_CACHE[] = "luaV_udata_cache";

# define luaV_getfield(L, s) \
    lua_pushlightuserdata((L), (శూన్య *)(s)); \
    lua_rawget((L), LUA_REGISTRYINDEX)
# define luaV_checksandbox(L) \
    ఒకవేళ (sandbox) luaL_error((L), "not allowed in sandbox")
# define lua_V_సందేశం(L) luaV_msgfunc((L), (msgfunc_T) సందేశం)
# define luaV_emsg(L) luaV_msgfunc((L), (msgfunc_T) emsg)
# define luaV_checktypval(L, a, v, సందేశం) \
    చేయి { \
	ఒకవేళ (luaV_totypval(L, a, v) == FAIL) \
	    luaL_error(L, సందేశం ": cannot convert value"); \
    } అయితే (0)

స్థిర luaV_List *luaV_pushlist(lua_State *L, list_T *lis);
స్థిర luaV_Dict *luaV_pushdict(lua_State *L, dict_T *dic);
స్థిర luaV_Blob *luaV_pushblob(lua_State *L, blob_T *blo);
స్థిర luaV_Funcref *luaV_pushfuncref(lua_State *L, char_u *name);
స్థిర పూర్ణాంకం luaV_call_lua_func(పూర్ణాంకం argcount, typval_T *argvars, typval_T *rettv, శూన్య *state);
స్థిర శూన్య luaV_call_lua_func_free(శూన్య *state);

# ఒకవేళ LUA_సంస్కరణ_అంకె <= 501
#  define lua_V_రిజిస్టరు(L, l) lua_L_రిజిస్టరు(L, ఖాళీ, l)
#  define luaL_typeerror luaL_typerror
# లేకపోతే
#  define lua_V_రిజిస్టరు(L, l) luaL_setfuncs(L, l, 0)
# endif

# ifdef DYNAMIC_LUA

#  ifdef MSWIN
#   define ఎక్కించు_dll vim_ఎక్కించు_Lib
#   define చిహ్నం_from_dll Get_Proc_చిరునామా
#   define close_dll FreeLibrary
#   define ఎక్కించు_dll_error GetWin32Error
#  లేకపోతే
#   include <dlfcn.h>
#   define HANDLE శూన్య*
#   define ఎక్కించు_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#   define చిహ్నం_from_dll dlsym
#   define close_dll dlclose
#   define ఎక్కించు_dll_error dlerror
#  endif

// lauxlib
#  ఒకవేళ LUA_సంస్కరణ_అంకె <= 501
#   define lua_L_రిజిస్టరు dll_lua_L_రిజిస్టరు
#   define luaL_prepbuffer dll_luaL_prepbuffer
#   define luaL_openlib dll_luaL_openlib
#   define luaL_typerror dll_luaL_typerror
#   define luaL_loadfile dll_luaL_loadfile
#   define luaL_loadbuffer dll_luaL_loadbuffer
#  లేకపోతే
#   define luaL_prepbuffsize dll_luaL_prepbuffsize
#   define luaL_setfuncs dll_luaL_setfuncs
#   define luaL_loadfilex dll_luaL_loadfilex
#   define luaL_loadbufferx dll_luaL_loadbufferx
#   define luaL_argerror dll_luaL_argerror
#  endif
#  ఒకవేళ LUA_సంస్కరణ_అంకె >= 504
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
#  ఒకవేళ LUA_సంస్కరణ_అంకె <= 501
#   define lua_tonumber dll_lua_tonumber
#   define lua_tointeger dll_lua_tointeger
#   define lua_call dll_lua_call
#   define lua_pcall dll_lua_pcall
#  లేకపోతే
#   define lua_tonumberx dll_lua_tonumberx
#   define lua_tointegerx dll_lua_tointegerx
#   define lua_callk dll_lua_callk
#   define lua_pcallk dll_lua_pcallk
#   define lua_getglobal dll_lua_getglobal
#   define lua_setglobal dll_lua_setglobal
#  endif
#  ఒకవేళ LUA_సంస్కరణ_అంకె <= 502
#   define lua_replace dll_lua_replace
#   define lua_remove dll_lua_remove
#  endif
#  ఒకవేళ LUA_సంస్కరణ_అంకె >= 503
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
#  ఒకవేళ LUA_సంస్కరణ_అంకె >= 504
#   define lua_newuserdatauv dll_lua_newuserdatauv
#  లేకపోతే
#   define lua_newuserdata dll_lua_newuserdata
#  endif
#  define lua_getmetatable dll_lua_getmetatable
#  define lua_setfield dll_lua_setfield
#  define lua_rawset dll_lua_rawset
#  define lua_rawseti dll_lua_rawseti
#  define lua_setmetatable dll_lua_setmetatable
#  define lua_తదుపరి dll_lua_తదుపరి
// libs
#  define luaopen_పునాది dll_luaopen_పునాది
#  define luaopen_పట్టిక dll_luaopen_పట్టిక
#  define luaopen_string dll_luaopen_string
#  define luaopen_math dll_luaopen_math
#  define luaopen_io dll_luaopen_io
#  define luaopen_os dll_luaopen_os
#  define luaopen_package dll_luaopen_package
#  define luaopen_debug dll_luaopen_debug
#  ఒకవేళ LUA_సంస్కరణ_అంకె >= 505
#   define luaL_openselectedlibs dll_luaL_openselectedlibs
#  లేకపోతే
#   define luaL_openlibs dll_luaL_openlibs
#  endif

// lauxlib
#  ఒకవేళ LUA_సంస్కరణ_అంకె <= 501
శూన్య (*dll_lua_L_రిజిస్టరు) (lua_State *L, స్థిరరాశి అక్షరం *libname, స్థిరరాశి luaL_Reg *l);
అక్షరం *(*dll_luaL_prepbuffer) (lua_L_బఫరు *B);
శూన్య (*dll_luaL_openlib) (lua_State *L, స్థిరరాశి అక్షరం *libname, స్థిరరాశి luaL_Reg *l, పూర్ణాంకం nup);
పూర్ణాంకం (*dll_luaL_typerror) (lua_State *L, పూర్ణాంకం narg, స్థిరరాశి అక్షరం *tname);
పూర్ణాంకం (*dll_luaL_loadfile) (lua_State *L, స్థిరరాశి అక్షరం *filename);
పూర్ణాంకం (*dll_luaL_loadbuffer) (lua_State *L, స్థిరరాశి అక్షరం *buff, size_t sz, స్థిరరాశి అక్షరం *name);
#  లేకపోతే
అక్షరం *(*dll_luaL_prepbuffsize) (lua_L_బఫరు *B, size_t sz);
శూన్య (*dll_luaL_setfuncs) (lua_State *L, స్థిరరాశి luaL_Reg *l, పూర్ణాంకం nup);
పూర్ణాంకం (*dll_luaL_loadfilex) (lua_State *L, స్థిరరాశి అక్షరం *filename, స్థిరరాశి అక్షరం *బహుళకం);
పూర్ణాంకం (*dll_luaL_loadbufferx) (lua_State *L, స్థిరరాశి అక్షరం *buff, size_t sz, స్థిరరాశి అక్షరం *name, స్థిరరాశి అక్షరం *బహుళకం);
పూర్ణాంకం (*dll_luaL_argerror) (lua_State *L, పూర్ణాంకం numarg, స్థిరరాశి అక్షరం *extramsg);
#  endif
#  ఒకవేళ LUA_సంస్కరణ_అంకె >= 504
పూర్ణాంకం (*dll_luaL_typeerror) (lua_State *L, పూర్ణాంకం narg, స్థిరరాశి అక్షరం *tname);
#  endif
శూన్య (*dll_luaL_checkany) (lua_State *L, పూర్ణాంకం narg);
స్థిరరాశి అక్షరం *(*dll_luaL_checklstring) (lua_State *L, పూర్ణాంకం అంకె_Arg, size_t *l);
lua_Integer (*dll_luaL_checkinteger) (lua_State *L, పూర్ణాంకం అంకె_Arg);
lua_Integer (*dll_luaL_optinteger) (lua_State *L, పూర్ణాంకం nArg, lua_Integer def);
శూన్య (*dll_luaL_checktype) (lua_State *L, పూర్ణాంకం narg, పూర్ణాంకం t);
పూర్ణాంకం (*dll_luaL_error) (lua_State *L, స్థిరరాశి అక్షరం *fmt, ...);
lua_State *(*dll_luaL_newstate) (శూన్య);
శూన్య (*dll_luaL_buffinit) (lua_State *L, lua_L_బఫరు *B);
శూన్య (*dll_luaL_addlstring) (lua_L_బఫరు *B, స్థిరరాశి అక్షరం *s, size_t l);
శూన్య (*dll_luaL_pushresult) (lua_L_బఫరు *B);
పూర్ణాంకం (*dll_luaL_loadstring) (lua_State *L, స్థిరరాశి అక్షరం *s);
పూర్ణాంకం (*dll_luaL_ref) (lua_State *L, పూర్ణాంకం idx);
#  ఒకవేళ LUA_సంస్కరణ_అంకె <= 502
శూన్య (*dll_luaL_unref) (lua_State *L, పూర్ణాంకం idx, పూర్ణాంకం n);
#  లేకపోతే
శూన్య (*dll_luaL_unref) (lua_State *L, పూర్ణాంకం idx, lua_Integer n);
#  endif
// lua
#  ఒకవేళ LUA_సంస్కరణ_అంకె <= 501
lua_సంఖ్య (*dll_lua_tonumber) (lua_State *L, పూర్ణాంకం idx);
lua_Integer (*dll_lua_tointeger) (lua_State *L, పూర్ణాంకం idx);
శూన్య (*dll_lua_call) (lua_State *L, పూర్ణాంకం nargs, పూర్ణాంకం nresults);
పూర్ణాంకం (*dll_lua_pcall) (lua_State *L, పూర్ణాంకం nargs, పూర్ణాంకం nresults, పూర్ణాంకం errfunc);
#  లేకపోతే
lua_సంఖ్య (*dll_lua_tonumberx) (lua_State *L, పూర్ణాంకం idx, పూర్ణాంకం *isnum);
lua_Integer (*dll_lua_tointegerx) (lua_State *L, పూర్ణాంకం idx, పూర్ణాంకం *isnum);
శూన్య (*dll_lua_callk) (lua_State *L, పూర్ణాంకం nargs, పూర్ణాంకం nresults, పూర్ణాంకం ctx,
	lua_CFunction k);
పూర్ణాంకం (*dll_lua_pcallk) (lua_State *L, పూర్ణాంకం nargs, పూర్ణాంకం nresults, పూర్ణాంకం errfunc,
	పూర్ణాంకం ctx, lua_CFunction k);
శూన్య (*dll_lua_getglobal) (lua_State *L, స్థిరరాశి అక్షరం *var);
శూన్య (*dll_lua_setglobal) (lua_State *L, స్థిరరాశి అక్షరం *var);
#  endif
#  ఒకవేళ LUA_సంస్కరణ_అంకె <= 502
శూన్య (*dll_lua_replace) (lua_State *L, పూర్ణాంకం idx);
శూన్య (*dll_lua_remove) (lua_State *L, పూర్ణాంకం idx);
#  endif
#  ఒకవేళ LUA_సంస్కరణ_అంకె >= 503
శూన్య  (*dll_lua_rotate) (lua_State *L, పూర్ణాంకం idx, పూర్ణాంకం n);
శూన్య (*dll_lua_copy) (lua_State *L, పూర్ణాంకం fromidx, పూర్ణాంకం toidx);
#  endif
స్థిరరాశి అక్షరం *(*dll_lua_typename) (lua_State *L, పూర్ణాంకం tp);
శూన్య       (*dll_lua_close) (lua_State *L);
పూర్ణాంకం (*dll_lua_gettop) (lua_State *L);
శూన్య (*dll_lua_settop) (lua_State *L, పూర్ణాంకం idx);
శూన్య (*dll_lua_pushvalue) (lua_State *L, పూర్ణాంకం idx);
పూర్ణాంకం (*dll_lua_isnumber) (lua_State *L, పూర్ణాంకం idx);
పూర్ణాంకం (*dll_lua_isstring) (lua_State *L, పూర్ణాంకం idx);
పూర్ణాంకం (*dll_lua_type) (lua_State *L, పూర్ణాంకం idx);
పూర్ణాంకం (*dll_lua_rawequal) (lua_State *L, పూర్ణాంకం idx1, పూర్ణాంకం idx2);
పూర్ణాంకం (*dll_lua_toboolean) (lua_State *L, పూర్ణాంకం idx);
స్థిరరాశి అక్షరం *(*dll_lua_tolstring) (lua_State *L, పూర్ణాంకం idx, size_t *len);
శూన్య *(*dll_lua_touserdata) (lua_State *L, పూర్ణాంకం idx);
శూన్య (*dll_lua_pushnil) (lua_State *L);
శూన్య (*dll_lua_pushnumber) (lua_State *L, lua_సంఖ్య n);
శూన్య (*dll_lua_pushinteger) (lua_State *L, lua_Integer n);
శూన్య (*dll_lua_pushlstring) (lua_State *L, స్థిరరాశి అక్షరం *s, size_t l);
శూన్య (*dll_lua_pushstring) (lua_State *L, స్థిరరాశి అక్షరం *s);
స్థిరరాశి అక్షరం *(*dll_lua_pushfstring) (lua_State *L, స్థిరరాశి అక్షరం *fmt, ...);
శూన్య (*dll_lua_pushcclosure) (lua_State *L, lua_CFunction fn, పూర్ణాంకం n);
శూన్య (*dll_lua_pushboolean) (lua_State *L, పూర్ణాంకం b);
శూన్య (*dll_lua_pushlightuserdata) (lua_State *L, శూన్య *p);
శూన్య (*dll_lua_getfield) (lua_State *L, పూర్ణాంకం idx, స్థిరరాశి అక్షరం *k);
#  ఒకవేళ LUA_సంస్కరణ_అంకె <= 502
శూన్య (*dll_lua_rawget) (lua_State *L, పూర్ణాంకం idx);
శూన్య (*dll_lua_rawgeti) (lua_State *L, పూర్ణాంకం idx, పూర్ణాంకం n);
#  లేకపోతే
పూర్ణాంకం (*dll_lua_rawget) (lua_State *L, పూర్ణాంకం idx);
పూర్ణాంకం (*dll_lua_rawgeti) (lua_State *L, పూర్ణాంకం idx, lua_Integer n);
#  endif
శూన్య (*dll_lua_createtable) (lua_State *L, పూర్ణాంకం narr, పూర్ణాంకం nrec);
శూన్య (*dll_lua_settable) (lua_State *L, పూర్ణాంకం idx);
#  ఒకవేళ LUA_సంస్కరణ_అంకె >= 504
శూన్య *(*dll_lua_newuserdatauv) (lua_State *L, size_t sz, పూర్ణాంకం nuvalue);
#  లేకపోతే
శూన్య *(*dll_lua_newuserdata) (lua_State *L, size_t sz);
#  endif
పూర్ణాంకం (*dll_lua_getmetatable) (lua_State *L, పూర్ణాంకం objindex);
శూన్య (*dll_lua_setfield) (lua_State *L, పూర్ణాంకం idx, స్థిరరాశి అక్షరం *k);
శూన్య (*dll_lua_rawset) (lua_State *L, పూర్ణాంకం idx);
#  ఒకవేళ LUA_సంస్కరణ_అంకె <= 502
శూన్య (*dll_lua_rawseti) (lua_State *L, పూర్ణాంకం idx, పూర్ణాంకం n);
#  లేకపోతే
శూన్య (*dll_lua_rawseti) (lua_State *L, పూర్ణాంకం idx, lua_Integer n);
#  endif
పూర్ణాంకం (*dll_lua_setmetatable) (lua_State *L, పూర్ణాంకం objindex);
పూర్ణాంకం (*dll_lua_తదుపరి) (lua_State *L, పూర్ణాంకం idx);
// libs
పూర్ణాంకం (*dll_luaopen_పునాది) (lua_State *L);
పూర్ణాంకం (*dll_luaopen_పట్టిక) (lua_State *L);
పూర్ణాంకం (*dll_luaopen_string) (lua_State *L);
పూర్ణాంకం (*dll_luaopen_math) (lua_State *L);
పూర్ణాంకం (*dll_luaopen_io) (lua_State *L);
పూర్ణాంకం (*dll_luaopen_os) (lua_State *L);
పూర్ణాంకం (*dll_luaopen_package) (lua_State *L);
పూర్ణాంకం (*dll_luaopen_debug) (lua_State *L);
#  ఒకవేళ LUA_సంస్కరణ_అంకె >= 505
శూన్య (*dll_luaL_openselectedlibs) (lua_State *L, పూర్ణాంకం ఎక్కించు, పూర్ణాంకం preload);
#  లేకపోతే
శూన్య (*dll_luaL_openlibs) (lua_State *L);
#  endif

రకనిర్వచనం శూన్య **luaV_function;
రకనిర్వచనం నిర్మాణం {
    స్థిరరాశి అక్షరం *name;
    luaV_function func;
} luaV_Reg;

స్థిర స్థిరరాశి luaV_Reg luaV_dll[] = {
    // lauxlib
#  ఒకవేళ LUA_సంస్కరణ_అంకె <= 501
    {"luaL_register", (luaV_function) &dll_lua_L_రిజిస్టరు},
    {"luaL_prepbuffer", (luaV_function) &dll_luaL_prepbuffer},
    {"luaL_openlib", (luaV_function) &dll_luaL_openlib},
    {"luaL_typerror", (luaV_function) &dll_luaL_typerror},
    {"luaL_loadfile", (luaV_function) &dll_luaL_loadfile},
    {"luaL_loadbuffer", (luaV_function) &dll_luaL_loadbuffer},
#  లేకపోతే
    {"luaL_prepbuffsize", (luaV_function) &dll_luaL_prepbuffsize},
    {"luaL_setfuncs", (luaV_function) &dll_luaL_setfuncs},
    {"luaL_loadfilex", (luaV_function) &dll_luaL_loadfilex},
    {"luaL_loadbufferx", (luaV_function) &dll_luaL_loadbufferx},
    {"luaL_argerror", (luaV_function) &dll_luaL_argerror},
#  endif
#  ఒకవేళ LUA_సంస్కరణ_అంకె >= 504
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
#  ఒకవేళ LUA_సంస్కరణ_అంకె <= 501
    {"lua_tonumber", (luaV_function) &dll_lua_tonumber},
    {"lua_tointeger", (luaV_function) &dll_lua_tointeger},
    {"lua_call", (luaV_function) &dll_lua_call},
    {"lua_pcall", (luaV_function) &dll_lua_pcall},
#  లేకపోతే
    {"lua_tonumberx", (luaV_function) &dll_lua_tonumberx},
    {"lua_tointegerx", (luaV_function) &dll_lua_tointegerx},
    {"lua_callk", (luaV_function) &dll_lua_callk},
    {"lua_pcallk", (luaV_function) &dll_lua_pcallk},
    {"lua_getglobal", (luaV_function) &dll_lua_getglobal},
    {"lua_setglobal", (luaV_function) &dll_lua_setglobal},
#  endif
#  ఒకవేళ LUA_సంస్కరణ_అంకె <= 502
    {"lua_replace", (luaV_function) &dll_lua_replace},
    {"lua_remove", (luaV_function) &dll_lua_remove},
#  endif
#  ఒకవేళ LUA_సంస్కరణ_అంకె >= 503
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
#  ఒకవేళ LUA_సంస్కరణ_అంకె >= 504
    {"lua_newuserdatauv", (luaV_function) &dll_lua_newuserdatauv},
#  లేకపోతే
    {"lua_newuserdata", (luaV_function) &dll_lua_newuserdata},
#  endif
    {"lua_getmetatable", (luaV_function) &dll_lua_getmetatable},
    {"lua_setfield", (luaV_function) &dll_lua_setfield},
    {"lua_rawset", (luaV_function) &dll_lua_rawset},
    {"lua_rawseti", (luaV_function) &dll_lua_rawseti},
    {"lua_setmetatable", (luaV_function) &dll_lua_setmetatable},
    {"lua_next", (luaV_function) &dll_lua_తదుపరి},
    // libs
    {"luaopen_base", (luaV_function) &dll_luaopen_పునాది},
    {"luaopen_table", (luaV_function) &dll_luaopen_పట్టిక},
    {"luaopen_string", (luaV_function) &dll_luaopen_string},
    {"luaopen_math", (luaV_function) &dll_luaopen_math},
    {"luaopen_io", (luaV_function) &dll_luaopen_io},
    {"luaopen_os", (luaV_function) &dll_luaopen_os},
    {"luaopen_package", (luaV_function) &dll_luaopen_package},
    {"luaopen_debug", (luaV_function) &dll_luaopen_debug},
#  ఒకవేళ LUA_సంస్కరణ_అంకె >= 505
    {"luaL_openselectedlibs", (luaV_function) &dll_luaL_openselectedlibs},
#  లేకపోతే
    {"luaL_openlibs", (luaV_function) &dll_luaL_openlibs},
#  endif
    {ఖాళీ, ఖాళీ}
};

స్థిర HANDLE hinstLua = ఖాళీ;

    స్థిర పూర్ణాంకం
lua_link_init(అక్షరం *libname, పూర్ణాంకం verbose)
{
    స్థిరరాశి luaV_Reg *reg;
    ఒకవేళ (hinstLua) తిరిగి OK;
    hinstLua = ఎక్కించు_dll(libname);
    ఒకవేళ (!hinstLua)
    {
	ఒకవేళ (verbose)
	    semsg(_(e_could_not_ఎక్కించు_library_str_str), libname, ఎక్కించు_dll_error());
	తిరిగి FAIL;
    }
    కొరకు (reg = luaV_dll; reg->func; reg++)
    {
	ఒకవేళ ((*reg->func = చిహ్నం_from_dll(hinstLua, reg->name)) == ఖాళీ)
	{
	    close_dll(hinstLua);
	    hinstLua = 0;
	    ఒకవేళ (verbose)
		semsg(_(e_could_not_ఎక్కించు_library_function_str), reg->name);
	    తిరిగి FAIL;
	}
    }
    తిరిగి OK;
}
# endif // DYNAMIC_LUA

# ఒకవేళ defined(DYNAMIC_LUA)
    పూర్ణాంకం
lua_enabled(పూర్ణాంకం verbose)
{
    తిరిగి lua_link_init((అక్షరం *)p_luadll, verbose) == OK;
}
# endif

# ఒకవేళ LUA_సంస్కరణ_అంకె > 501 && LUA_సంస్కరణ_అంకె < 504
    స్థిర పూర్ణాంకం
luaL_typeerror(lua_State *L, పూర్ణాంకం narg, స్థిరరాశి అక్షరం *tname)
{
    స్థిరరాశి అక్షరం *సందేశం = lua_pushfstring(L, "%s expected, got %s",
	    tname, luaL_typename(L, narg));
    తిరిగి luaL_argerror(L, narg, సందేశం);
}
# endif

    స్థిర LUAV_INLINE శూన్య
luaV_getudata(lua_State *L, శూన్య *v)
{
    lua_pushlightuserdata(L, (శూన్య *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // now the cache table is at the top of the stack
    lua_pushlightuserdata(L, v);
    lua_rawget(L, -2);
    lua_remove(L, -2);  // remove the cache table from the stack
}

    స్థిర LUAV_INLINE శూన్య
luaV_setudata(lua_State *L, శూన్య *v)
{
    lua_pushlightuserdata(L, (శూన్య *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // cache table is at -1
    lua_pushlightuserdata(L, v);       // ...now at -2
    lua_pushvalue(L, -3);	       // copy the userdata (cache at -3)
    lua_rawset(L, -3);		       // consumes two stack items
    lua_తీయి(L, 1);		       // and remove the cache table
}

// =======   Internal   =======

    స్థిర శూన్య
luaV_newmetatable(lua_State *L, స్థిరరాశి అక్షరం *tname)
{
    lua_newtable(L);
    lua_pushlightuserdata(L, (శూన్య *) tname);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
}

    స్థిర శూన్య *
luaV_toudata(lua_State *L, పూర్ణాంకం ud, స్థిరరాశి అక్షరం *tname)
{
    శూన్య *p = lua_touserdata(L, ud);

    ఒకవేళ (p == ఖాళీ)
	తిరిగి ఖాళీ;

    // value is userdata
    ఒకవేళ (lua_getmetatable(L, ud)) // does it have a metatable?
    {
	luaV_getfield(L, tname); // get metatable
	ఒకవేళ (lua_rawequal(L, -1, -2)) // MTs match?
	{
	    lua_తీయి(L, 2); // MTs
	    తిరిగి p;
	}
    }

    తిరిగి ఖాళీ;
}

    స్థిర శూన్య *
luaV_checkcache(lua_State *L, శూన్య *p)
{
    luaV_getudata(L, p);
    ఒకవేళ (lua_isnil(L, -1)) luaL_error(L, "invalid object");
    lua_తీయి(L, 1);
    తిరిగి p;
}

# define luaV_unbox(L,luatyp,ud) (*((luatyp *) lua_touserdata((L),(ud))))

# define luaV_checkvalid(L,luatyp,ud) \
    luaV_checkcache((L), (శూన్య *) luaV_unbox((L),luatyp,(ud)))

    స్థిర శూన్య *
luaV_checkudata(lua_State *L, పూర్ణాంకం ud, స్థిరరాశి అక్షరం *tname)
{
    శూన్య *p = luaV_toudata(L, ud, tname);
    ఒకవేళ (p == ఖాళీ) luaL_typeerror(L, ud, tname);
    తిరిగి p;
}

    స్థిర శూన్య
luaV_pushtypval(lua_State *L, typval_T *tv)
{
    ఒకవేళ (tv == ఖాళీ)
    {
	lua_pushnil(L);
	తిరిగి;
    }
    ఎంపిక (tv->v_type)
    {
	స్థితి VAR_STRING:
	    lua_pushstring(L, tv->vval.v_string == ఖాళీ
					    ? "" : (అక్షరం *)tv->vval.v_string);
	    ఆపు;
	స్థితి VAR_సంఖ్య:
	    lua_pushinteger(L, (పూర్ణాంకం) tv->vval.v_సంఖ్య);
	    ఆపు;
	స్థితి VAR_FLOAT:
	    lua_pushnumber(L, (lua_సంఖ్య) tv->vval.v_float);
	    ఆపు;
	స్థితి VAR_LIST:
	    luaV_pushlist(L, tv->vval.v_list);
	    ఆపు;
	స్థితి VAR_DICT:
	    luaV_pushdict(L, tv->vval.v_dict);
	    ఆపు;
	స్థితి VAR_BOOL:
	స్థితి VAR_SPECIAL:
	    ఒకవేళ (tv->vval.v_సంఖ్య <= VVAL_TRUE)
		lua_pushboolean(L, (పూర్ణాంకం) tv->vval.v_సంఖ్య);
	    లేకపోతే
		lua_pushnil(L);
	    ఆపు;
	స్థితి VAR_FUNC:
	    luaV_pushfuncref(L, tv->vval.v_string);
	    ఆపు;
	స్థితి VAR_PARTIAL:
	    // TODO: handle partial arguments
	    luaV_pushfuncref(L, partial_name(tv->vval.v_partial));
	    ఆపు;

	స్థితి VAR_BLOB:
	    luaV_pushblob(L, tv->vval.v_blob);
	    ఆపు;
	అప్రమేయ:
	    lua_pushnil(L);
    }
}

/*
 * Converts lua value at 'pos' to typval 'tv'.
 * Returns OK or FAIL.
 */
    స్థిర పూర్ణాంకం
luaV_totypval(lua_State *L, పూర్ణాంకం pos, typval_T *tv)
{
    పూర్ణాంకం స్థితి_2 = OK;

    tv->v_తాళం = 0;

    ఎంపిక (lua_type(L, pos))
    {
	స్థితి LUA_TBOOLEAN:
	    tv->v_type = VAR_BOOL;
	    tv->vval.v_సంఖ్య = (varnumber_T) lua_toboolean(L, pos);
	    ఆపు;
	స్థితి LUA_TNIL:
	    tv->v_type = VAR_SPECIAL;
	    tv->vval.v_సంఖ్య = VVAL_NULL;
	    ఆపు;
	స్థితి LUA_TSTRING:
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strsave((char_u *) lua_tostring(L, pos));
	    ఆపు;
	స్థితి LUA_TNUMBER:
	{
	    స్థిరరాశి lua_సంఖ్య n = lua_tonumber(L, pos);

	    ఒకవేళ (n > (lua_సంఖ్య)INT64_MAX || n < (lua_సంఖ్య)INT64_MIN
		    || ((lua_సంఖ్య)((varnumber_T)n)) != n)
	    {
		tv->v_type = VAR_FLOAT;
		tv->vval.v_float = (float_T)n;
	    }
	    లేకపోతే
	    {
		tv->v_type = VAR_సంఖ్య;
		tv->vval.v_సంఖ్య = (varnumber_T)n;
	    }
	}
	    ఆపు;
	స్థితి LUA_TFUNCTION:
	{
	    char_u *name;
	    luaV_CFuncState *state;

	    lua_pushvalue(L, pos);
	    state = ALLOC_CLEAR_ONE(luaV_CFuncState);
	    state->lua_funcref = luaL_ref(L, LUA_REGISTRYINDEX);
	    state->L = L;
	    state->lua_tableref = LUA_NOREF;
	    name = రిజిస్టరు_cfunc(&luaV_call_lua_func,
					      &luaV_call_lua_func_free, state);
	    tv->v_type = VAR_FUNC;
	    tv->vval.v_string = vim_strsave(name);
	    ఆపు;
	}
	స్థితి LUA_TTABLE:
	{
	    పూర్ణాంకం lua_tableref;

	    lua_pushvalue(L, pos);
	    lua_tableref = luaL_ref(L, LUA_REGISTRYINDEX);
	    ఒకవేళ (lua_getmetatable(L, pos))
	    {
		lua_getfield(L, -1, LUA___CALL);
		ఒకవేళ (lua_isfunction(L, -1))
		{
		    char_u *name;
		    పూర్ణాంకం lua_funcref = luaL_ref(L, LUA_REGISTRYINDEX);
		    luaV_CFuncState *state = ALLOC_CLEAR_ONE(luaV_CFuncState);

		    state->lua_funcref = lua_funcref;
		    state->L = L;
		    state->lua_tableref = lua_tableref;
		    name = రిజిస్టరు_cfunc(&luaV_call_lua_func,
					      &luaV_call_lua_func_free, state);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(name);
		    ఆపు;
		}
	    }
	    tv->v_type = VAR_సంఖ్య;
	    tv->vval.v_సంఖ్య = 0;
	    స్థితి_2 = FAIL;
	    ఆపు;
	}
	స్థితి LUA_TUSERDATA:
	{
	    శూన్య *p = lua_touserdata(L, pos);

	    ఒకవేళ (lua_getmetatable(L, pos)) // has metatable?
	    {
		// check list
		luaV_getfield(L, LUAVIM_LIST);
		ఒకవేళ (lua_rawequal(L, -1, -2))
		{
		    tv->v_type = VAR_LIST;
		    tv->vval.v_list = *((luaV_List *) p);
		    ++tv->vval.v_list->lv_refcount;
		    lua_తీయి(L, 2); // MTs
		    ఆపు;
		}
		// check dict
		luaV_getfield(L, LUAVIM_DICT);
		ఒకవేళ (lua_rawequal(L, -1, -3))
		{
		    tv->v_type = VAR_DICT;
		    tv->vval.v_dict = *((luaV_Dict *) p);
		    ++tv->vval.v_dict->dv_refcount;
		    lua_తీయి(L, 3); // MTs
		    ఆపు;
		}
		// check blob
		luaV_getfield(L, LUAVIM_BLOB);
		ఒకవేళ (lua_rawequal(L, -1, -4))
		{
		    tv->v_type = VAR_BLOB;
		    tv->vval.v_blob = *((luaV_Blob *) p);
		    ++tv->vval.v_blob->bv_refcount;
		    lua_తీయి(L, 4); // MTs
		    ఆపు;
		}
		// check funcref
		luaV_getfield(L, LUAVIM_FUNCREF);
		ఒకవేళ (lua_rawequal(L, -1, -5))
		{
		    luaV_Funcref *f = (luaV_Funcref *) p;

		    func_ref(f->name);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(f->name);
		    lua_తీయి(L, 5); // MTs
		    ఆపు;
		}
		lua_తీయి(L, 4); // MTs
	    }
	}
	// FALLTHROUGH
	అప్రమేయ:
	    tv->v_type = VAR_సంఖ్య;
	    tv->vval.v_సంఖ్య = 0;
	    స్థితి_2 = FAIL;
    }
    తిరిగి స్థితి_2;
}

/*
 * similar to luaL_addlstring, but replaces \0 with \n if toline and
 * \n with \0 otherwise
 */
    స్థిర శూన్య
luaV_addlstring(lua_L_బఫరు *b, స్థిరరాశి అక్షరం *s, size_t l, పూర్ణాంకం toline)
{
    అయితే (l--)
    {
	ఒకవేళ (*s == '\0' && toline)
	    luaL_addchar(b, '\n');
	లేకపోతే ఒకవేళ (*s == '\n' && !toline)
	    luaL_addchar(b, '\0');
	లేకపోతే
	    luaL_addchar(b, *s);
	s++;
    }
}

    స్థిర శూన్య
luaV_pushline(lua_State *L, buf_T *buf, linenr_T n)
{
    స్థిరరాశి అక్షరం *s = (స్థిరరాశి అక్షరం *) ml_get_buf(buf, n, FALSE);
    lua_L_బఫరు b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, అక్షరశ్రేణి_పొడవు(s), 0);
    luaL_pushresult(&b);
}

    స్థిర char_u *
luaV_toline(lua_State *L, పూర్ణాంకం pos)
{
    size_t l;
    స్థిరరాశి అక్షరం *s = lua_tolstring(L, pos, &l);

    lua_L_బఫరు b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 1);
    luaL_pushresult(&b);
    తిరిగి (char_u *) lua_tostring(L, -1);
}

/*
 * pops a string s from the top of the stack and calls mf(t) for pieces t of
 * s separated by newlines
 */
    స్థిర శూన్య
luaV_msgfunc(lua_State *L, msgfunc_T mf)
{
    lua_L_బఫరు b;
    size_t l;
    స్థిరరాశి అక్షరం *p, *s = lua_tolstring(L, -1, &l);
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 0);
    luaL_pushresult(&b);
    // break string
    p = s = lua_tolstring(L, -1, &l);
    అయితే (l--)
    {
	ఒకవేళ (*p++ == '\0') // break?
	{
	    mf((అక్షరం *)s);
	    s = p;
	}
    }
    mf((అక్షరం *)s);
    lua_తీయి(L, 2); // original and modified strings
}

# define luaV_newtype(typ,tname,luatyp,luatname) \
	స్థిర luatyp * \
    luaV_new##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = (luatyp *) lua_newuserdata(L, పరిమాణం(luatyp)); \
	*o = obj; \
	luaV_setudata(L, obj); /* cache[obj] = udata */ \
	luaV_getfield(L, luatname); \
	lua_setmetatable(L, -2); \
	తిరిగి o; \
    }

# define luaV_pushtype(typ,tname,luatyp) \
	స్థిర luatyp * \
    lua_V_నెట్టు##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = ఖాళీ; \
	ఒకవేళ (obj == ఖాళీ) \
	    lua_pushnil(L); \
	లేకపోతే \
	{ \
	    luaV_getudata(L, obj); \
	    ఒకవేళ (lua_isnil(L, -1)) /* not interned? */ \
	    { \
		lua_తీయి(L, 1); \
		o = luaV_new##tname(L, obj); \
	    } \
	    లేకపోతే \
		o = (luatyp *) lua_touserdata(L, -1); \
	} \
	తిరిగి o; \
    }

# define luaV_type_tostring(tname,luatname) \
	స్థిర పూర్ణాంకం \
    luaV_##tname##_tostring(lua_State *L) \
    { \
	lua_pushfstring(L, "%s: %p", luatname, lua_touserdata(L, 1)); \
	తిరిగి 1; \
    }

// =======   List type   =======

    స్థిర luaV_List *
luaV_newlist(lua_State *L, list_T *lis)
{
    luaV_List *l = (luaV_List *) lua_newuserdata(L, పరిమాణం(luaV_List));
    *l = lis;
    lis->lv_refcount++; // reference in Lua
    luaV_setudata(L, lis); // cache[lis] = udata
    luaV_getfield(L, LUAVIM_LIST);
    lua_setmetatable(L, -2);
    తిరిగి l;
}

luaV_pushtype(list_T, list, luaV_List)
luaV_type_tostring(list, LUAVIM_LIST)

    స్థిర పూర్ణాంకం
luaV_list_len(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushinteger(L, (పూర్ణాంకం) list_len(l));
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
luaV_list_iter(lua_State *L)
{
    listitem_T *li = (listitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    ఒకవేళ (li == ఖాళీ) తిరిగి 0;
    luaV_pushtypval(L, &li->li_tv);
    lua_pushlightuserdata(L, (శూన్య *) li->li_తదుపరి);
    lua_replace(L, lua_upvalueindex(1));
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
luaV_list_call(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushlightuserdata(L, (శూన్య *) l->lv_మొదటి);
    lua_pushcclosure(L, luaV_list_iter, 1);
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
lua_V_list_సూచీ(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    ఒకవేళ (lua_isnumber(L, 2)) // list item?
    {
	దీర్ఘ n = (దీర్ఘ) luaL_checkinteger(L, 2);
	listitem_T *li;

	// Lua array index starts with 1 while Vim uses 0, subtract 1 to
	// normalize.
	n -= 1;
	li = list_కనుగొను(l, n);
	ఒకవేళ (li == ఖాళీ)
	    lua_pushnil(L);
	లేకపోతే
	    luaV_pushtypval(L, &li->li_tv);
    }
    లేకపోతే ఒకవేళ (lua_isstring(L, 2)) // method?
    {
	స్థిరరాశి అక్షరం *s = lua_tostring(L, 2);
	ఒకవేళ (అక్షరశ్రేణి_పోలిక(s, "add", 3) == 0
		|| అక్షరశ్రేణి_పోలిక(s, "insert", 6) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	లేకపోతే
	    lua_pushnil(L);
    }
    లేకపోతే
	lua_pushnil(L);
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
luaV_list_newindex(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    దీర్ఘ n = (దీర్ఘ) luaL_checkinteger(L, 2);
    listitem_T *li;

    // Lua array index starts with 1 while Vim uses 0, subtract 1 to normalize.
    n -= 1;

    ఒకవేళ (l->lv_తాళం)
	luaL_error(L, "list is locked");
    li = list_కనుగొను(l, n);
    ఒకవేళ (li == ఖాళీ)
    {
	ఒకవేళ (!lua_isnil(L, 3))
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "inserting list item");
	    ఒకవేళ (list_చొప్పించు_tv(l, &v, li) == FAIL)
		luaL_error(L, "failed to add item to list");
	    clear_tv(&v);
	}
    }
    లేకపోతే
    {
	ఒకవేళ (lua_isnil(L, 3)) // remove?
	{
	    vimlist_remove(l, li, li);
	    listitem_free(l, li);
	}
	లేకపోతే
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "setting list item");
	    clear_tv(&li->li_tv);
	    li->li_tv = v;
	}
    }
    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
lua_V_list_కూడు(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (శూన్య *) *lis);
    typval_T v;
    ఒకవేళ (l->lv_తాళం)
	luaL_error(L, "list is locked");
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "adding list item");
    ఒకవేళ (list_append_tv(l, &v) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
lua_V_list_చొప్పించు(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (శూన్య *) *lis);
    దీర్ఘ pos = (దీర్ఘ) luaL_optinteger(L, 3, 0);
    listitem_T *li = ఖాళీ;
    typval_T v;
    ఒకవేళ (l->lv_తాళం)
	luaL_error(L, "list is locked");
    ఒకవేళ (pos < l->lv_len)
    {
	li = list_కనుగొను(l, pos);
	ఒకవేళ (li == ఖాళీ)
	    luaL_error(L, "invalid position");
    }
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "inserting list item");
    ఒకవేళ (list_చొప్పించు_tv(l, &v, li) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    తిరిగి 1;
}

స్థిర స్థిరరాశి luaL_Reg luaV_List_mt[] = {
    {"__tostring", luaV_list_tostring},
    {"__len", luaV_list_len},
    {"__call", luaV_list_call},
    {"__index", lua_V_list_సూచీ},
    {"__newindex", luaV_list_newindex},
    {"add", lua_V_list_కూడు},
    {"insert", lua_V_list_చొప్పించు},
    {ఖాళీ, ఖాళీ}
};


// =======   Dict type   =======

    స్థిర luaV_Dict *
luaV_newdict(lua_State *L, dict_T *dic)
{
    luaV_Dict *d = (luaV_Dict *) lua_newuserdata(L, పరిమాణం(luaV_Dict));
    *d = dic;
    dic->dv_refcount++; // reference in Lua
    luaV_setudata(L, dic); // cache[dic] = udata
    luaV_getfield(L, LUAVIM_DICT);
    lua_setmetatable(L, -2);
    తిరిగి d;
}

luaV_pushtype(dict_T, dict, luaV_Dict)
luaV_type_tostring(dict, LUAVIM_DICT)

    స్థిర పూర్ణాంకం
luaV_dict_len(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    lua_pushinteger(L, (పూర్ణాంకం) dict_len(d));
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
luaV_dict_iter(lua_State *L వాడని)
{
# ifdef FEAT_EVAL
    hashitem_T *hi = (hashitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    పూర్ణాంకం n = lua_tointeger(L, lua_upvalueindex(2));
    dictitem_T *di;
    ఒకవేళ (n <= 0) తిరిగి 0;
    అయితే (HASHITEM_ఖాళీలు(hi)) hi++;
    di = dict_lookup(hi);
    lua_pushstring(L, (అక్షరం *) hi->hi_కీలకం);
    luaV_pushtypval(L, &di->di_tv);
    lua_pushlightuserdata(L, (శూన్య *) (hi + 1));
    lua_replace(L, lua_upvalueindex(1));
    lua_pushinteger(L, n - 1);
    lua_replace(L, lua_upvalueindex(2));
    తిరిగి 2;
# లేకపోతే
    తిరిగి 0;
# endif
}

    స్థిర పూర్ణాంకం
luaV_dict_call(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    hashtab_T *ht = &d->dv_hashtab;
    lua_pushlightuserdata(L, (శూన్య *) ht->ht_array);
    lua_pushinteger(L, ht->ht_వాడిన); // # remaining items
    lua_pushcclosure(L, luaV_dict_iter, 2);
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
lua_V_dict_సూచీ(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *కీలకం = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di = dict_కనుగొను(d, కీలకం, -1);

    ఒకవేళ (di == ఖాళీ)
    {
	lua_pushnil(L);
	తిరిగి 1;
    }

    luaV_pushtypval(L, &di->di_tv);
    ఒకవేళ (di->di_tv.v_type == VAR_FUNC) // funcref?
    {
	luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, -1);
	f->self = d; // keep "self" reference
	d->dv_refcount++;
    }

    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
luaV_dict_newindex(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *కీలకం = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di;
    typval_T tv;

    ఒకవేళ (d->dv_తాళం)
	luaL_error(L, "dict is locked");
    ఒకవేళ (కీలకం == ఖాళీ)
	తిరిగి 0;
    ఒకవేళ (*కీలకం == NUL)
	luaL_error(L, "empty key");
    ఒకవేళ (!lua_isnil(L, 3)) // read value?
    {
	luaV_checktypval(L, 3, &tv, "setting dict item");
	ఒకవేళ (d->dv_scope == VAR_DEF_SCOPE && tv.v_type == VAR_FUNC)
	{
	    clear_tv(&tv);
	    luaL_error(L, "cannot assign funcref to builtin scope");
	}
    }
    di = dict_కనుగొను(d, కీలకం, -1);
    ఒకవేళ (di == ఖాళీ) // non-existing key?
    {
	ఒకవేళ (lua_isnil(L, 3))
	    తిరిగి 0;
	di = dictitem_alloc(కీలకం);
	ఒకవేళ (di == ఖాళీ)
	{
	    clear_tv(&tv);
	    తిరిగి 0;
	}
	ఒకవేళ (dict_కూడు(d, di) == FAIL)
	{
	    vim_free(di);
	    clear_tv(&tv);
	    తిరిగి 0;
	}
    }
    లేకపోతే
	clear_tv(&di->di_tv);
    ఒకవేళ (lua_isnil(L, 3)) // remove?
    {
	hashitem_T *hi = hash_కనుగొను(&d->dv_hashtab, di->di_కీలకం);
	hash_remove(&d->dv_hashtab, hi, "Lua new index");
	dictitem_free(di);
    }
    లేకపోతే
	di->di_tv = tv;
    తిరిగి 0;
}

స్థిర స్థిరరాశి luaL_Reg luaV_Dict_mt[] = {
    {"__tostring", luaV_dict_tostring},
    {"__len", luaV_dict_len},
    {"__call", luaV_dict_call},
    {"__index", lua_V_dict_సూచీ},
    {"__newindex", luaV_dict_newindex},
    {ఖాళీ, ఖాళీ}
};


// =======   Blob type   =======

    స్థిర luaV_Blob *
luaV_newblob(lua_State *L, blob_T *blo)
{
    luaV_Blob *b = (luaV_Blob *) lua_newuserdata(L, పరిమాణం(luaV_Blob));
    *b = blo;
    blo->bv_refcount++; // reference in Lua
    luaV_setudata(L, blo); // cache[blo] = udata
    luaV_getfield(L, LUAVIM_BLOB);
    lua_setmetatable(L, -2);
    తిరిగి b;
}

luaV_pushtype(blob_T, blob, luaV_Blob)
luaV_type_tostring(blob, LUAVIM_BLOB)

    స్థిర పూర్ణాంకం
luaV_blob_gc(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    blob_unref(b);
    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
luaV_blob_len(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    lua_pushinteger(L, (పూర్ణాంకం) blob_len(b));
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
lua_V_blob_సూచీ(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    ఒకవేళ (lua_isnumber(L, 2))
    {
	పూర్ణాంకం idx = luaL_checkinteger(L, 2);
	ఒకవేళ (idx < 0)
	    // Negative index is relative to the end.
	    idx += (పూర్ణాంకం) blob_len(b);
	ఒకవేళ (idx >= 0 && idx < blob_len(b))
	    lua_pushnumber(L, (lua_సంఖ్య) blob_get(b, idx));
	లేకపోతే
	    lua_pushnil(L);
    }
    లేకపోతే ఒకవేళ (lua_isstring(L, 2))
    {
	స్థిరరాశి అక్షరం *s = lua_tostring(L, 2);
	ఒకవేళ (అక్షరశ్రేణి_పోలిక(s, "add", 3) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	లేకపోతే
	    lua_pushnil(L);
    }
    లేకపోతే
	lua_pushnil(L);
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
luaV_blob_newindex(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    ఒకవేళ (b->bv_తాళం)
	luaL_error(L, "blob is locked");

    ఒకవేళ (!lua_isnumber(L, 2))
	తిరిగి 0;

    దీర్ఘ len = blob_len(b);
    పూర్ణాంకం idx = luaL_checkinteger(L, 2);
    పూర్ణాంకం val = luaL_checkinteger(L, 3);
    ఒకవేళ (idx < 0)
	// Negative index is relative to the end.
	idx += (పూర్ణాంకం) len;
    ఒకవేళ ((idx >= 0 && idx < len)
	    || (idx == len && ga_grow(&b->bv_ga, 1) == OK))
    {
	blob_సమితి(b, idx, (char_u) val);
	ఒకవేళ (idx == len)
	    ++b->bv_ga.ga_len;
    }
    లేకపోతే
	luaL_error(L, "index out of range");

    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
lua_V_blob_కూడు(lua_State *L)
{
    luaV_Blob *blo = luaV_checkudata(L, 1, LUAVIM_BLOB);
    blob_T *b = (blob_T *) luaV_checkcache(L, (శూన్య *) *blo);
    ఒకవేళ (b->bv_తాళం)
	luaL_error(L, "blob is locked");
    lua_settop(L, 2);
    ఒకవేళ (!lua_isstring(L, 2))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 2));
    లేకపోతే
    {
	size_t i, l = 0;
	స్థిరరాశి అక్షరం *s = lua_tolstring(L, 2, &l);

	ఒకవేళ (ga_grow(&b->bv_ga, (పూర్ణాంకం)l) == OK)
	    కొరకు (i = 0; i < l; ++i)
		ga_append(&b->bv_ga, s[i]);
    }
    lua_settop(L, 1);
    తిరిగి 1;
}

స్థిర స్థిరరాశి luaL_Reg luaV_Blob_mt[] = {
    {"__tostring", luaV_blob_tostring},
    {"__gc", luaV_blob_gc},
    {"__len", luaV_blob_len},
    {"__index", lua_V_blob_సూచీ},
    {"__newindex", luaV_blob_newindex},
    {"add", lua_V_blob_కూడు},
    {ఖాళీ, ఖాళీ}
};


// =======   Funcref type   =======

    స్థిర luaV_Funcref *
luaV_newfuncref(lua_State *L, char_u *name)
{
    luaV_Funcref *f = (luaV_Funcref *)lua_newuserdata(L, పరిమాణం(luaV_Funcref));

    ఒకవేళ (name != ఖాళీ)
    {
	func_ref(name);
	f->name = vim_strsave(name);
    }
    f->self = ఖాళీ;
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_setmetatable(L, -2);
    తిరిగి f;
}

    స్థిర luaV_Funcref *
luaV_pushfuncref(lua_State *L, char_u *name)
{
    తిరిగి luaV_newfuncref(L, name);
}


luaV_type_tostring(funcref, LUAVIM_FUNCREF)

    స్థిర పూర్ణాంకం
luaV_funcref_gc(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    func_unref(f->name);
    vim_free(f->name);
    // NOTE: Don't call "dict_unref(f->self)", because the dict of "f->self"
    // will be (or has been already) freed by Vim's garbage collection.
    తిరిగి 0;
}

// equivalent to string(funcref)
    స్థిర పూర్ణాంకం
luaV_funcref_len(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    lua_pushstring(L, (స్థిరరాశి అక్షరం *) f->name);
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
luaV_funcref_call(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);
    పూర్ణాంకం i, n = lua_gettop(L) - 1; // #args
    పూర్ణాంకం స్థితి_2 = FAIL;
    typval_T వాదనలు;
    typval_T rettv;

    వాదనలు.v_type = VAR_LIST;
    వాదనలు.vval.v_list = list_alloc();
    rettv.v_type = VAR_UNKNOWN; // as in clear_tv
    ఒకవేళ (వాదనలు.vval.v_list != ఖాళీ)
    {
	typval_T v;

	కొరకు (i = 0; i < n; i++)
	{
	    luaV_checktypval(L, i + 2, &v, "calling funcref");
	    list_append_tv(వాదనలు.vval.v_list, &v);
	    clear_tv(&v);
	}
	స్థితి_2 = func_call(f->name, &వాదనలు, ఖాళీ, f->self, &rettv);
	ఒకవేళ (స్థితి_2 == OK)
	    luaV_pushtypval(L, &rettv);
	clear_tv(&వాదనలు);
	clear_tv(&rettv);
    }
    ఒకవేళ (స్థితి_2 != OK)
	luaL_error(L, "cannot call funcref");
    తిరిగి 1;
}

స్థిర స్థిరరాశి luaL_Reg luaV_Funcref_mt[] = {
    {"__tostring", luaV_funcref_tostring},
    {"__gc", luaV_funcref_gc},
    {"__len", luaV_funcref_len},
    {"__call", luaV_funcref_call},
    {ఖాళీ, ఖాళీ}
};


// =======   Buffer type   =======

luaV_newtype(buf_T, బఫరు, lua_V_బఫరు, LUAVIM_బఫరు)
luaV_pushtype(buf_T, బఫరు, lua_V_బఫరు)
luaV_type_tostring(బఫరు, LUAVIM_బఫరు)

    స్థిర పూర్ణాంకం
lua_V_బఫరు_len(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, lua_V_బఫరు, 1);
    lua_pushinteger(L, b->b_ml.ml_పంక్తి_లెక్క);
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
lua_V_బఫరు_call(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, lua_V_బఫరు, 1);
    lua_settop(L, 1);
    సమితి_curbuf(b, DOBUF_విభజించు);
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
lua_V_బఫరు_సూచీ(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, lua_V_బఫరు, 1);
    linenr_T n = (linenr_T) lua_tointeger(L, 2);
    ఒకవేళ (n > 0 && n <= b->b_ml.ml_పంక్తి_లెక్క)
	luaV_pushline(L, b, n);
    లేకపోతే ఒకవేళ (lua_isstring(L, 2))
    {
	స్థిరరాశి అక్షరం *s = lua_tostring(L, 2);
	ఒకవేళ (అక్షరశ్రేణి_పోలిక(s, "name", 4) == 0)
	    lua_pushstring(L, (b->b_sfname == ఖాళీ)
					? "" : (అక్షరం *) b->b_sfname);
	లేకపోతే ఒకవేళ (అక్షరశ్రేణి_పోలిక(s, "fname", 5) == 0)
	    lua_pushstring(L, (b->b_ffname == ఖాళీ)
					? "" : (అక్షరం *) b->b_ffname);
	లేకపోతే ఒకవేళ (అక్షరశ్రేణి_పోలిక(s, "number", 6) == 0)
	    lua_pushinteger(L, b->b_fnum);
	// methods
	లేకపోతే ఒకవేళ (అక్షరశ్రేణి_పోలిక(s,   "insert", 6) == 0
		|| అక్షరశ్రేణి_పోలిక(s, "next", 4) == 0
		|| అక్షరశ్రేణి_పోలిక(s, "previous", 8) == 0
		|| అక్షరశ్రేణి_పోలిక(s, "isvalid", 7) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	లేకపోతే
	    lua_pushnil(L);
    }
    లేకపోతే
	lua_pushnil(L);
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
lua_V_బఫరు_newindex(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, lua_V_బఫరు, 1);
    linenr_T n = (linenr_T) luaL_checkinteger(L, 2);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    ఒకవేళ (n < 1 || n > b->b_ml.ml_పంక్తి_లెక్క)
	luaL_error(L, "invalid line number");
    ఒకవేళ (lua_isnil(L, 3)) // delete line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	ఒకవేళ (u_savedel(n, 1L) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	లేకపోతే ఒకవేళ (ml_delete(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot delete line");
	}
	లేకపోతే
	{
	    deleted_పంక్తులు_mark(n, 1L);
	    ఒకవేళ (b == curwin->w_బఫరు) // fix cursor in current window?
	    {
		ఒకవేళ (curwin->w_cursor.lnum >= n)
		{
		    ఒకవేళ (curwin->w_cursor.lnum > n)
		    {
			curwin->w_cursor.lnum -= 1;
			check_cursor_నిలువు();
		    }
		    లేకపోతే
			check_cursor();
		    మారింది_cline_bef_curs();
		}
		invalidate_botline();
	    }
	}
	curbuf = buf;
    }
    లేకపోతే ఒకవేళ (lua_isstring(L, 3)) // update line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	ఒకవేళ (u_savesub(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	లేకపోతే ఒకవేళ (ml_replace(n, luaV_toline(L, 3), TRUE) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot replace line");
	}
	లేకపోతే
	    మారింది_బైట్లు(n, 0);
	curbuf = buf;
	ఒకవేళ (b == curwin->w_బఫరు)
	    check_cursor_నిలువు();
    }
    లేకపోతే
	luaL_error(L, "wrong argument to change line");
    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
lua_V_బఫరు_చొప్పించు(lua_State *L)
{
    lua_V_బఫరు *lb = luaV_checkudata(L, 1, LUAVIM_బఫరు);
    buf_T *b = (buf_T *) luaV_checkcache(L, (శూన్య *) *lb);
    linenr_T చివరి = b->b_ml.ml_పంక్తి_లెక్క;
    linenr_T n = (linenr_T) luaL_optinteger(L, 3, చివరి);
    buf_T *buf;
    luaL_checktype(L, 2, LUA_TSTRING);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    // fix insertion line
    ఒకవేళ (n < 0) n = 0;
    ఒకవేళ (n > చివరి) n = చివరి;
    // insert
    buf = curbuf;
    curbuf = b;
    ఒకవేళ (u_save(n, n + 1) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot save undo information");
    }
    లేకపోతే ఒకవేళ (ml_append(n, luaV_toline(L, 2), 0, FALSE) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot insert line");
    }
    లేకపోతే
	appended_పంక్తులు_mark(n, 1L);
    curbuf = buf;
    update_screen(UPD_VALID);
    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
lua_V_బఫరు_తదుపరి(lua_State *L)
{
    lua_V_బఫరు *b = luaV_checkudata(L, 1, LUAVIM_బఫరు);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (శూన్య *) *b);
    luaV_pushbuffer(L, buf->b_తదుపరి);
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
lua_V_బఫరు_మునుపటి(lua_State *L)
{
    lua_V_బఫరు *b = luaV_checkudata(L, 1, LUAVIM_బఫరు);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (శూన్య *) *b);
    luaV_pushbuffer(L, buf->b_prev);
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
lua_V_బఫరు_isvalid(lua_State *L)
{
    lua_V_బఫరు *b = luaV_checkudata(L, 1, LUAVIM_బఫరు);
    luaV_getudata(L, *b);
    lua_pushboolean(L, !lua_isnil(L, -1));
    తిరిగి 1;
}

స్థిర స్థిరరాశి luaL_Reg lua_V_బఫరు_mt[] = {
    {"__tostring", lua_V_బఫరు_tostring},
    {"__len", lua_V_బఫరు_len},
    {"__call", lua_V_బఫరు_call},
    {"__index", lua_V_బఫరు_సూచీ},
    {"__newindex", lua_V_బఫరు_newindex},
    {"insert", lua_V_బఫరు_చొప్పించు},
    {"next", lua_V_బఫరు_తదుపరి},
    {"previous", lua_V_బఫరు_మునుపటి},
    {"isvalid", lua_V_బఫరు_isvalid},
    {ఖాళీ, ఖాళీ}
};


// =======   Window type   =======

luaV_newtype(win_T, కిటికీ_2, lua_V_కిటికీ, LUAVIM_కిటికీ)
luaV_pushtype(win_T, కిటికీ_2, lua_V_కిటికీ)
luaV_type_tostring(కిటికీ_2, LUAVIM_కిటికీ)

    స్థిర పూర్ణాంకం
lua_V_కిటికీ_call(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, lua_V_కిటికీ, 1);
    lua_settop(L, 1);
    win_goto(w);
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
lua_V_కిటికీ_సూచీ(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, lua_V_కిటికీ, 1);
    స్థిరరాశి అక్షరం *s = luaL_checkstring(L, 2);
    ఒకవేళ (అక్షరశ్రేణి_పోలిక(s, "buffer", 6) == 0)
	luaV_pushbuffer(L, w->w_బఫరు);
    లేకపోతే ఒకవేళ (అక్షరశ్రేణి_పోలిక(s, "line", 4) == 0)
	lua_pushinteger(L, w->w_cursor.lnum);
    లేకపోతే ఒకవేళ (అక్షరశ్రేణి_పోలిక(s, "col", 3) == 0)
	lua_pushinteger(L, w->w_cursor.నిలువు + 1);
    లేకపోతే ఒకవేళ (అక్షరశ్రేణి_పోలిక(s, "width", 5) == 0)
	lua_pushinteger(L, w->w_వెడల్పు);
    లేకపోతే ఒకవేళ (అక్షరశ్రేణి_పోలిక(s, "height", 6) == 0)
	lua_pushinteger(L, w->w_ఎత్తు);
    // methods
    లేకపోతే ఒకవేళ (అక్షరశ్రేణి_పోలిక(s,   "next", 4) == 0
	    || అక్షరశ్రేణి_పోలిక(s, "previous", 8) == 0
	    || అక్షరశ్రేణి_పోలిక(s, "isvalid", 7) == 0)
    {
	lua_getmetatable(L, 1);
	lua_getfield(L, -1, s);
    }
    లేకపోతే
	lua_pushnil(L);
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
lua_V_కిటికీ_newindex(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, lua_V_కిటికీ, 1);
    స్థిరరాశి అక్షరం *s = luaL_checkstring(L, 2);
    పూర్ణాంకం v = luaL_checkinteger(L, 3);
    ఒకవేళ (అక్షరశ్రేణి_పోలిక(s, "line", 4) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	ఒకవేళ (v < 1 || v > w->w_బఫరు->b_ml.ml_పంక్తి_లెక్క)
	    luaL_error(L, "line out of range");
	w->w_cursor.lnum = v;
	update_screen(UPD_VALID);
    }
    లేకపోతే ఒకవేళ (అక్షరశ్రేణి_పోలిక(s, "col", 3) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	w->w_cursor.నిలువు = v - 1;
	w->w_సమితి_curswant = true;
	update_screen(UPD_VALID);
    }
    లేకపోతే ఒకవేళ (అక్షరశ్రేణి_పోలిక(s, "width", 5) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setwidth(v);
	curwin = win;
    }
    లేకపోతే ఒకవేళ (అక్షరశ్రేణి_పోలిక(s, "height", 6) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setheight(v);
	curwin = win;
    }
    లేకపోతే
	luaL_error(L, "invalid window property: `%s'", s);
    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
lua_V_కిటికీ_తదుపరి(lua_State *L)
{
    lua_V_కిటికీ *w = luaV_checkudata(L, 1, LUAVIM_కిటికీ);
    win_T *win = (win_T *) luaV_checkcache(L, (శూన్య *) *w);
    luaV_pushwindow(L, win->w_తదుపరి);
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
lua_V_కిటికీ_మునుపటి(lua_State *L)
{
    lua_V_కిటికీ *w = luaV_checkudata(L, 1, LUAVIM_కిటికీ);
    win_T *win = (win_T *) luaV_checkcache(L, (శూన్య *) *w);
    luaV_pushwindow(L, win->w_prev);
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
lua_V_కిటికీ_isvalid(lua_State *L)
{
    lua_V_కిటికీ *w = luaV_checkudata(L, 1, LUAVIM_కిటికీ);
    luaV_getudata(L, *w);
    lua_pushboolean(L, !lua_isnil(L, -1));
    తిరిగి 1;
}

స్థిర స్థిరరాశి luaL_Reg lua_V_కిటికీ_mt[] = {
    {"__tostring", lua_V_కిటికీ_tostring},
    {"__call", lua_V_కిటికీ_call},
    {"__index", lua_V_కిటికీ_సూచీ},
    {"__newindex", lua_V_కిటికీ_newindex},
    {"next", lua_V_కిటికీ_తదుపరి},
    {"previous", lua_V_కిటికీ_మునుపటి},
    {"isvalid", lua_V_కిటికీ_isvalid},
    {ఖాళీ, ఖాళీ}
};


// =======   Vim module   =======

    స్థిర పూర్ణాంకం
luaV_print(lua_State *L)
{
    పూర్ణాంకం		i, n = lua_gettop(L); // nargs
    స్థిరరాశి అక్షరం	*s;
    size_t	l;
    garray_T	సందేశం_ga;

    ga_init2(&సందేశం_ga, 1, 128);
    lua_getglobal(L, "tostring");
    కొరకు (i = 1; i <= n; i++)
    {
	lua_pushvalue(L, -1); // tostring
	lua_pushvalue(L, i); // arg
	lua_call(L, 1, 1);
	s = lua_tolstring(L, -1, &l);
	ఒకవేళ (s == ఖాళీ)
	    తిరిగి luaL_error(L, "cannot convert to string");
	ఒకవేళ (i > 1)
	    ga_append(&సందేశం_ga, ' '); // use space instead of tab
	ga_concat_len(&సందేశం_ga, (char_u *)s, l);
	lua_తీయి(L, 1);
    }
    // Replace any "\n" with "\0"
    కొరకు (i = 0; i < సందేశం_ga.ga_len; i++)
	ఒకవేళ (((అక్షరం *)సందేశం_ga.ga_దత్తాంశం)[i] == '\n')
	    ((అక్షరం *)సందేశం_ga.ga_దత్తాంశం)[i] = '\0';
    lua_pushlstring(L, సందేశం_ga.ga_దత్తాంశం, సందేశం_ga.ga_len);
    ఒకవేళ (!got_int)
	lua_V_సందేశం(L);

    ga_clear(&సందేశం_ga);
    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
luaV_debug(lua_State *L)
{
    lua_settop(L, 0);
    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "eval");
    lua_remove(L, -2); // vim.eval at position 1
    కొరకు (;;)
    {
	స్థిరరాశి అక్షరం *ఆదానం;
	size_t l;
	lua_pushvalue(L, 1); // vim.eval
	lua_pushliteral(L, "input('lua_debug> ')");
	lua_call(L, 1, 1); // return string
	ఆదానం = lua_tolstring(L, -1, &l);
	ఒకవేళ (l == 0 || strcmp(ఆదానం, "cont") == 0)
	    తిరిగి 0;
	సందేశం_putchar('\n'); // avoid outputting on input line
	ఒకవేళ (luaL_loadbuffer(L, ఆదానం, l, "=(debug command)")
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
	lua_settop(L, 1); // remove eventual returns, but keep vim.eval
    }
}

    స్థిర dict_T *
luaV_get_var_scope(lua_State *L)
{
    స్థిరరాశి అక్షరం	*scope = luaL_checkstring(L, 1);
    dict_T	*dict = ఖాళీ;

    ఒకవేళ (STRICMP((అక్షరం *)scope, "g") == 0)
	dict = get_globvar_dict();
    లేకపోతే ఒకవేళ (STRICMP((అక్షరం *)scope, "v") == 0)
	dict = get_vimvar_dict();
    లేకపోతే ఒకవేళ (STRICMP((అక్షరం *)scope, "b") == 0)
	dict = curbuf->b_vars;
    లేకపోతే ఒకవేళ (STRICMP((అక్షరం *)scope, "w") == 0)
	dict = curwin->w_vars;
    లేకపోతే ఒకవేళ (STRICMP((అక్షరం *)scope, "t") == 0)
	dict = curtab->tp_vars;
    లేకపోతే
    {
	luaL_error(L, "invalid scope %s", scope);
	తిరిగి ఖాళీ;
    }

    తిరిగి dict;
}

    స్థిర పూర్ణాంకం
luaV_setvar(lua_State *L)
{
    dict_T	*dict;
    dictitem_T	*di;
    size_t	len;
    అక్షరం	*name;
    పూర్ణాంకం		del;
    అక్షరం	*error = ఖాళీ;

    name = (అక్షరం *)luaL_checklstring(L, 3, &len);
    del = (lua_gettop(L) < 4) || lua_isnil(L, 4);

    dict = luaV_get_var_scope(L);
    ఒకవేళ (dict == ఖాళీ)
	తిరిగి 0;

    di = dict_కనుగొను(dict, (char_u *)name, (పూర్ణాంకం)len);
    ఒకవేళ (di != ఖాళీ)
    {
	ఒకవేళ (di->di_flags & DI_FLAGS_RO)
	    error = "variable is read-only";
	లేకపోతే ఒకవేళ (di->di_flags & DI_FLAGS_తాళం)
	    error = "variable is locked";
	లేకపోతే ఒకవేళ (del && di->di_flags & DI_FLAGS_FIX)
	    error = "variable is fixed";
	ఒకవేళ (error != ఖాళీ)
	    తిరిగి luaL_error(L, error);
    }
    లేకపోతే ఒకవేళ (dict->dv_తాళం)
	తిరిగి luaL_error(L, "Dictionary is locked");

    ఒకవేళ (del)
    {
	// Delete the key
	ఒకవేళ (di == ఖాళీ)
	    // Doesn't exist, nothing to do
	    తిరిగి 0;
	// Delete the entry
	dictitem_remove(dict, di, "Lua delete variable");
    }
    లేకపోతే
    {
	// Update the key
	typval_T	tv;

	// Convert the lua value to a Vim script type in the temporary variable
	lua_pushvalue(L, 4);
	ఒకవేళ (luaV_totypval(L, -1, &tv) == FAIL)
	    తిరిగి luaL_error(L, "Couldn't convert lua value");

	ఒకవేళ (di == ఖాళీ)
	{
	    // Need to create an entry
	    di = dictitem_alloc((char_u *)name);
	    ఒకవేళ (di == ఖాళీ)
	    {
		clear_tv(&tv);
		తిరిగి 0;
	    }
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	    ఒకవేళ (dict_కూడు(dict, di) == FAIL)
	    {
		dictitem_free(di);
		clear_tv(&tv);
		తిరిగి luaL_error(L, "Couldn't add to dictionary");
	    }
	}
	లేకపోతే
	{
	    పూర్ణాంకం type_error = FALSE;
	    ఒకవేళ (dict == get_vimvar_dict()
	       && !before_సమితి_vvar((char_u *)name, di, &tv, TRUE, &type_error))
	    {
		clear_tv(&tv);
		ఒకవేళ (type_error)
		    తిరిగి luaL_error(L,
				"Setting v:%s to value with wrong type", name);
		తిరిగి 0;
	    }
	    // Clear the old value
	    clear_tv(&di->di_tv);
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	}

	// Clear the temporary variable
	clear_tv(&tv);
    }

    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
luaV_getvar(lua_State *L)
{
    dict_T	*dict = luaV_get_var_scope(L);
    size_t	len;
    స్థిరరాశి అక్షరం	*name = luaL_checklstring(L, 3, &len);
    dictitem_T	*di = dict_కనుగొను(dict, (char_u *)name, (పూర్ణాంకం)len);

    ఒకవేళ (di == ఖాళీ)
	తిరిగి 0;  // nil

    luaV_pushtypval(L, &di->di_tv);
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
luaV_command(lua_State *L)
{
    char_u  *s = vim_strsave((char_u *)luaL_checkstring(L, 1));

    execute_cmds_from_string(s);
    vim_free(s);
    update_screen(UPD_VALID);
    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
luaV_eval(lua_State *L)
{
    typval_T *tv = eval_సమాసం((char_u *) luaL_checkstring(L, 1), ఖాళీ);

    ఒకవేళ (tv == ఖాళీ) luaL_error(L, "invalid expression");
    luaV_pushtypval(L, tv);
    free_tv(tv);
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
luaV_beep(lua_State *L వాడని)
{
    vim_beep(BO_LANG);
    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
lua_V_పంక్తి(lua_State *L)
{
    luaV_pushline(L, curbuf, curwin->w_cursor.lnum);
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
luaV_list(lua_State *L)
{
    list_T *l;
    పూర్ణాంకం initarg = !lua_isnoneornil(L, 1);

    ఒకవేళ (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    l = list_alloc();
    ఒకవేళ (l == ఖాళీ)
    {
	lua_pushnil(L);
	తిరిగి 1;
    }

    luaV_newlist(L, l);
    ఒకవేళ (!initarg)
	తిరిగి 1;

    // traverse table to init list
    పూర్ణాంకం notnil, i = 0;
    typval_T v;
    చేయి
    {
	lua_rawgeti(L, 1, ++i);
	notnil = !lua_isnil(L, -1);
	ఒకవేళ (notnil)
	{
	    luaV_checktypval(L, -1, &v, "vim.list");
	    list_append_tv(l, &v);
	    clear_tv(&v);
	}
	lua_తీయి(L, 1); //విలువ
    } అయితే (notnil);

    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
luaV_dict(lua_State *L)
{
    dict_T *d;
    పూర్ణాంకం initarg = !lua_isnoneornil(L, 1);

    ఒకవేళ (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    d = dict_alloc();
    ఒకవేళ (d == ఖాళీ)
    {
	lua_pushnil(L);
	తిరిగి 1;
    }

    luaV_newdict(L, d);
    ఒకవేళ (!initarg)
	తిరిగి 1;

    // traverse table to init dict
    lua_pushnil(L);
    అయితే (lua_తదుపరి(L, 1))
    {
	char_u *కీలకం;
	dictitem_T *di;
	typval_T v;

	lua_pushvalue(L, -2); // dup key in case it's a number
	కీలకం = (char_u *) lua_tostring(L, -1);
	ఒకవేళ (కీలకం == ఖాళీ)
	{
	    lua_pushnil(L);
	    తిరిగి 1;
	}
	ఒకవేళ (*కీలకం == NUL)
	    luaL_error(L, "table has empty key");
	luaV_checktypval(L, -2, &v, "vim.dict"); //విలువ
	di = dictitem_alloc(కీలకం);
	ఒకవేళ (di == ఖాళీ || dict_కూడు(d, di) == FAIL)
	{
	    vim_free(di);
	    lua_pushnil(L);
	    తిరిగి 1;
	}
	di->di_tv = v;
	lua_తీయి(L, 2); // key copy and value
    }

    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
luaV_blob(lua_State *L)
{
    blob_T *b;
    పూర్ణాంకం initarg = !lua_isnoneornil(L, 1);

    ఒకవేళ (initarg && !lua_isstring(L, 1))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 1));

    b = blob_alloc();
    ఒకవేళ (b == ఖాళీ)
    {
	lua_pushnil(L);
	తిరిగి 1;
    }

    luaV_newblob(L, b);
    ఒకవేళ (!initarg)
	తిరిగి 1;

    // traverse table to init blob
    size_t i, l = 0;
    స్థిరరాశి అక్షరం *s = lua_tolstring(L, 1, &l);

    ఒకవేళ (ga_grow(&b->bv_ga, (పూర్ణాంకం)l) == OK)
	కొరకు (i = 0; i < l; ++i)
	    ga_append(&b->bv_ga, s[i]);

    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
luaV_funcref(lua_State *L)
{
    స్థిరరాశి అక్షరం *name = luaL_checkstring(L, 1);
    // note: not checking if function exists (needs function_exists)
    ఒకవేళ (name == ఖాళీ || *name == NUL || VIM_ISDIGIT(*name))
	luaL_error(L, "invalid function name: %s", name);
    luaV_newfuncref(L, (char_u *) name);
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
lua_V_బఫరు_2(lua_State *L)
{
    buf_T *buf;
    ఒకవేళ (lua_isstring(L, 1)) // get by number or name?
    {
	ఒకవేళ (lua_isnumber(L, 1)) // by number?
	{
	    పూర్ణాంకం n = lua_tointeger(L, 1);
	    FOR_ALL_BUFFERS(buf)
		ఒకవేళ (buf->b_fnum == n) ఆపు;
	}
	లేకపోతే // by name
	{
	    size_t l;
	    స్థిరరాశి అక్షరం *s = lua_tolstring(L, 1, &l);
	    FOR_ALL_BUFFERS(buf)
	    {
		ఒకవేళ (buf->b_ffname == ఖాళీ || buf->b_sfname == ఖాళీ)
		{
		    ఒకవేళ (l == 0) ఆపు;
		}
		లేకపోతే ఒకవేళ (అక్షరశ్రేణి_పోలిక(s, (అక్షరం *)buf->b_ffname, l) == 0
			|| అక్షరశ్రేణి_పోలిక(s, (అక్షరం *)buf->b_sfname, l) == 0)
		    ఆపు;
	    }
	}
    }
    లేకపోతే
	buf = (lua_toboolean(L, 1)) ? firstbuf : curbuf; // first buffer?
    luaV_pushbuffer(L, buf);
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
lua_V_కిటికీ_2(lua_State *L)
{
    win_T *win;
    ఒకవేళ (lua_isnumber(L, 1)) // get by number?
    {
	పూర్ణాంకం n = lua_tointeger(L, 1);
	కొరకు (win = firstwin; win != ఖాళీ; win = win->w_తదుపరి, n--)
	    ఒకవేళ (n == 1) ఆపు;
    }
    లేకపోతే
	win = (lua_toboolean(L, 1)) ? firstwin : curwin; // first window?
    luaV_pushwindow(L, win);
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
luaV_open(lua_State *L)
{
    char_u *s = ఖాళీ;
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    ఒకవేళ (lua_isstring(L, 1)) s = (char_u *) lua_tostring(L, 1);
    luaV_pushbuffer(L, buflist_new(s, ఖాళీ, 1L, BLN_LISTED));
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
luaV_type(lua_State *L)
{
    luaL_checkany(L, 1);
    ఒకవేళ (lua_type(L, 1) == LUA_TUSERDATA) // check vim udata?
    {
	lua_settop(L, 1);
	ఒకవేళ (lua_getmetatable(L, 1))
	{
	    luaV_getfield(L, LUAVIM_LIST);
	    ఒకవేళ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "list");
		తిరిగి 1;
	    }
	    luaV_getfield(L, LUAVIM_DICT);
	    ఒకవేళ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "dict");
		తిరిగి 1;
	    }
	    luaV_getfield(L, LUAVIM_BLOB);
	    ఒకవేళ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "blob");
		తిరిగి 1;
	    }
	    luaV_getfield(L, LUAVIM_FUNCREF);
	    ఒకవేళ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "funcref");
		తిరిగి 1;
	    }
	    luaV_getfield(L, LUAVIM_బఫరు);
	    ఒకవేళ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "buffer");
		తిరిగి 1;
	    }
	    luaV_getfield(L, LUAVIM_కిటికీ);
	    ఒకవేళ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "window");
		తిరిగి 1;
	    }
	}
    }
    lua_pushstring(L, luaL_typename(L, 1)); // fallback
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
luaV_call(lua_State *L)
{
    పూర్ణాంకం		argc = lua_gettop(L) - 1;
    size_t	funcname_len;
    char_u	*funcname;
    అక్షరం	*error = ఖాళీ;
    typval_T	rettv;
    typval_T	argv[MAX_FUNC_వాదనలు + 1];
    పూర్ణాంకం		i = 0;

    ఒకవేళ (argc > MAX_FUNC_వాదనలు)
	తిరిగి luaL_error(L, "Function called with too many arguments");

    funcname = (char_u *)luaL_checklstring(L, 1, &funcname_len);

    కొరకు (; i < argc; i++)
    {
	ఒకవేళ (luaV_totypval(L, i + 2, &argv[i]) == FAIL)
	{
	    error = "lua: cannot convert value";
	    వెళ్ళు free_vim_వాదనలు;
	}
    }

    argv[argc].v_type = VAR_UNKNOWN;

    ఒకవేళ (call_vim_function(funcname, argc, argv, &rettv) == FAIL)
    {
	error = "lua: call_vim_function failed";
	వెళ్ళు free_vim_వాదనలు;
    }

    luaV_pushtypval(L, &rettv);
    clear_tv(&rettv);

free_vim_వాదనలు:
    అయితే (i > 0)
	clear_tv(&argv[--i]);

    ఒకవేళ (error == ఖాళీ)
	తిరిగి 1;
    లేకపోతే
	తిరిగి luaL_error(L, error);
}

/*
 * Return the Vim version as a Lua table
 */
    స్థిర పూర్ణాంకం
lua_V_సంస్కరణ(lua_State *L)
{
    lua_newtable(L);
    lua_pushstring(L, "major");
    lua_pushinteger(L, VIM_సంస్కరణ_MAJOR);
    lua_settable(L, -3);
    lua_pushstring(L, "minor");
    lua_pushinteger(L, VIM_సంస్కరణ_MINOR);
    lua_settable(L, -3);
    lua_pushstring(L, "patch");
    lua_pushinteger(L, highest_patch());
    lua_settable(L, -3);
    తిరిగి 1;
}

స్థిర స్థిరరాశి luaL_Reg luaV_module[] = {
    {"command", luaV_command},
    {"eval", luaV_eval},
    {"beep", luaV_beep},
    {"line", lua_V_పంక్తి},
    {"list", luaV_list},
    {"dict", luaV_dict},
    {"blob", luaV_blob},
    {"funcref", luaV_funcref},
    {"buffer", lua_V_బఫరు_2},
    {"window", lua_V_కిటికీ_2},
    {"open", luaV_open},
    {"type", luaV_type},
    {"call", luaV_call},
    {"_getvar", luaV_getvar},
    {"_setvar", luaV_setvar},
    {"version", lua_V_సంస్కరణ},
    {"lua_version", ఖాళీ},
    {ఖాళీ, ఖాళీ}
};

/*
 * for freeing list, dict, buffer and window objects; lightuserdata as arg
 */
    స్థిర పూర్ణాంకం
luaV_free(lua_State *L)
{
    lua_pushnil(L);
    luaV_setudata(L, lua_touserdata(L, 1));
    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
luaV_luaeval(lua_State *L)
{
    lua_L_బఫరు b;
    size_t l;
    స్థిరరాశి అక్షరం *str = lua_tolstring(L, 1, &l);
    typval_T *arg = (typval_T *) lua_touserdata(L, 2);
    typval_T *rettv = (typval_T *) lua_touserdata(L, 3);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, LUAVIM_EVALHEADER, పరిమాణం(LUAVIM_EVALHEADER) - 1);
    luaL_addlstring(&b, str, l);
    luaL_pushresult(&b);
    str = lua_tolstring(L, -1, &l);
    ఒకవేళ (luaL_loadbuffer(L, str, l, LUAVIM_EVALNAME)) // compile error?
    {
	luaV_emsg(L);
	తిరిగి 0;
    }
    luaV_pushtypval(L, arg);
    ఒకవేళ (lua_pcall(L, 1, 1, 0)) // running error?
    {
	luaV_emsg(L);
	తిరిగి 0;
    }
    ఒకవేళ (luaV_totypval(L, -1, rettv) == FAIL)
	emsg("luaeval: cannot convert value");
    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
luaV_setref(lua_State *L)
{
    పూర్ణాంకం copyID = lua_tointeger(L, 1);
    పూర్ణాంకం abort = FALSE;

    lua_pushlightuserdata(L, (శూన్య *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // the cache table

    luaV_getfield(L, LUAVIM_LIST);
    luaV_getfield(L, LUAVIM_DICT);
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_pushnil(L);
    // traverse cache table
    అయితే (!abort && lua_తదుపరి(L, 2) != 0)
    {
	lua_getmetatable(L, -1);
	ఒకవేళ (lua_rawequal(L, -1, 3)) // list?
	{
	    list_T *l = (list_T *)lua_touserdata(L, 6); //కీలకం

	    abort = సమితి_ref_లోపల_list(l, copyID);
	}
	లేకపోతే ఒకవేళ (lua_rawequal(L, -1, 4)) // dict?
	{
	    dict_T *d = (dict_T *)lua_touserdata(L, 6); //కీలకం

	    abort = సమితి_ref_లోపల_dict(d, copyID);
	}
	లేకపోతే ఒకవేళ (lua_rawequal(L, -1, 5)) // funcref?
	{
	    luaV_Funcref *f = (luaV_Funcref *)lua_touserdata(L, 6); //కీలకం

	    abort = సమితి_ref_లోపల_dict(f->self, copyID);
	}
	lua_తీయి(L, 2); // metatable and value
    }
    lua_pushinteger(L, abort);
    తిరిగి 1;
}

    స్థిర పూర్ణాంకం
luaV_pushversion(lua_State *L)
{
    పూర్ణాంకం major = 0;
    పూర్ణాంకం minor = 0;
    పూర్ణాంకం patch = 0;
    అక్షరం s[16];

    అక్షరశ్రేణి_చదువు(LUAVIM_సంస్కరణ, "Lua %d.%d.%d", &major, &minor, &patch);
    vim_snprintf(s, పరిమాణం(s), "%d.%d.%d", major, minor, patch);
    lua_pushstring(L, s);
    తిరిగి 0;
}

# define LUA_VIM_FN_సంకేతం \
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

# define LUA_VIM_అమరిక_VARIABLE_DICTS \
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

    స్థిర పూర్ణాంకం
luaopen_vim(lua_State *L)
{
    lua_newtable(L);  // cache table
    lua_newtable(L);  // cache table's metatable
    lua_pushstring(L, "v");
    lua_setfield(L, -2, "__mode");
    lua_setmetatable(L, -2); // cache is weak-valued
    // put the cache table in the registry for luaV_get/setudata()
    lua_pushlightuserdata(L, (శూన్య *) LUAVIM_UDATA_CACHE);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
    lua_తీయి(L, 1);  // we don't need the cache table here anymore
    // print
    lua_pushcfunction(L, luaV_print);
    lua_setglobal(L, "print");
    // debug.debug
    lua_getglobal(L, "debug");
    lua_pushcfunction(L, luaV_debug);
    lua_setfield(L, -2, "debug");
    lua_తీయి(L, 1);
    // free
    lua_pushlightuserdata(L, (శూన్య *) LUAVIM_FREE);
    lua_pushcfunction(L, luaV_free);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // luaeval
    lua_pushlightuserdata(L, (శూన్య *) LUAVIM_LUAEVAL);
    lua_pushcfunction(L, luaV_luaeval);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // setref
    lua_pushlightuserdata(L, (శూన్య *) LUAVIM_SETREF);
    lua_pushcfunction(L, luaV_setref);
    lua_rawset(L, LUA_REGISTRYINDEX);
    //రిజిస్టరు
    luaV_newmetatable(L, LUAVIM_LIST);
    lua_V_రిజిస్టరు(L, luaV_List_mt);
    lua_తీయి(L, 1);
    luaV_newmetatable(L, LUAVIM_DICT);
    lua_V_రిజిస్టరు(L, luaV_Dict_mt);
    lua_తీయి(L, 1);
    luaV_newmetatable(L, LUAVIM_BLOB);
    lua_V_రిజిస్టరు(L, luaV_Blob_mt);
    lua_తీయి(L, 1);
    luaV_newmetatable(L, LUAVIM_FUNCREF);
    lua_V_రిజిస్టరు(L, luaV_Funcref_mt);
    lua_తీయి(L, 1);
    luaV_newmetatable(L, LUAVIM_బఫరు);
    lua_V_రిజిస్టరు(L, lua_V_బఫరు_mt);
    lua_తీయి(L, 1);
    luaV_newmetatable(L, LUAVIM_కిటికీ);
    lua_V_రిజిస్టరు(L, lua_V_కిటికీ_mt);
    lua_తీయి(L, 1);
    lua_newtable(L); // vim table
    lua_V_రిజిస్టరు(L, luaV_module);
    luaV_pushversion(L);
    lua_setfield(L, -2, "lua_version");
    lua_setglobal(L, LUAVIM_NAME);
    // custom code
    (శూన్య)luaL_dostring(L, LUA_VIM_FN_సంకేతం);
    (శూన్య)luaL_dostring(L, LUA_VIM_UPDATE_PACKAGE_PATHS);
    (శూన్య)luaL_dostring(L, LUA_VIM_అమరిక_VARIABLE_DICTS);

    lua_getglobal(L, LUAVIM_NAME);
    lua_getfield(L, -1, "_update_package_paths");

    ఒకవేళ (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);

    తిరిగి 0;
}

    స్థిర lua_State *
luaV_newstate(శూన్య)
{
    lua_State *L = luaL_newstate();
# ఒకవేళ LUA_సంస్కరణ_అంకె >= 505
    luaL_openselectedlibs(L, ~0, 0); // core libs
# లేకపోతే
    luaL_openlibs(L); // core libs
# endif
    lua_pushcfunction(L, luaopen_vim); // vim
    lua_call(L, 0, 0);
    తిరిగి L;
}

    స్థిర శూన్య
luaV_setrange(lua_State *L, పూర్ణాంకం line1, పూర్ణాంకం line2)
{
    lua_getglobal(L, LUAVIM_NAME);
    lua_pushinteger(L, line1);
    lua_setfield(L, -2, "firstline");
    lua_pushinteger(L, line2);
    lua_setfield(L, -2, "lastline");
    lua_తీయి(L, 1); // vim table
}


// =======   Interface   =======

స్థిర lua_State *L = ఖాళీ;

    స్థిర పూర్ణాంకం
lua_isopen(శూన్య)
{
    తిరిగి L != ఖాళీ;
}

    స్థిర పూర్ణాంకం
lua_init(శూన్య)
{
    ఒకవేళ (lua_isopen())
	తిరిగి OK;

# ifdef DYNAMIC_LUA
    ఒకవేళ (!lua_enabled(TRUE))
    {
	emsg(_("Lua library cannot be loaded."));
	తిరిగి FAIL;
    }
# endif
    L = luaV_newstate();

    తిరిగి OK;
}

    శూన్య
lua_చివర(శూన్య)
{
    ఒకవేళ (!lua_isopen())
	తిరిగి;

    lua_close(L);
    L = ఖాళీ;
}

/*
 * ex commands
 */
    శూన్య
ex_lua(exarg_T *eap)
{
    అక్షరం *script = (అక్షరం *)script_get(eap, eap->arg);

    ఒకవేళ (!eap->skip && lua_init() == OK)
    {
	అక్షరం *s = script != ఖాళీ ? script : (అక్షరం *)eap->arg;

	luaV_setrange(L, eap->line1, eap->line2);
	ఒకవేళ (luaL_loadbuffer(L, s, అక్షరశ్రేణి_పొడవు(s), LUAVIM_CHUNKNAME)
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
    ఒకవేళ (script != ఖాళీ)
	vim_free(script);
}

    శూన్య
ex_luado(exarg_T *eap)
{
    linenr_T l;
    స్థిరరాశి అక్షరం *s = (స్థిరరాశి అక్షరం *) eap->arg;
    lua_L_బఫరు b;
    size_t len;
    buf_T *was_curbuf = curbuf;

    ఒకవేళ (lua_init() == FAIL) తిరిగి;
    ఒకవేళ (u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
    {
	emsg(_("cannot save undo information"));
	తిరిగి;
    }
    luaV_setrange(L, eap->line1, eap->line2);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, "return function(line, linenr) ", 30); //శీర్షిక
    luaL_addlstring(&b, s, అక్షరశ్రేణి_పొడవు(s));
    luaL_addlstring(&b, " end", 4); // footer
    luaL_pushresult(&b);
    s = lua_tolstring(L, -1, &len);
    ఒకవేళ (luaL_loadbuffer(L, s, len, LUAVIM_CHUNKNAME))
    {
	luaV_emsg(L);
	lua_తీయి(L, 1); // function body
	తిరిగి;
    }
    lua_call(L, 0, 1);
    lua_replace(L, -2); // function -> body
    కొరకు (l = eap->line1; l <= eap->line2; l++)
    {
	// Check the line number, the command may have deleted lines.
	ఒకవేళ (l > curbuf->b_ml.ml_పంక్తి_లెక్క)
	    ఆపు;

	lua_pushvalue(L, -1); // function
	luaV_pushline(L, curbuf, l); // current line as arg
	lua_pushinteger(L, l); // current line number as arg
	ఒకవేళ (lua_pcall(L, 2, 1, 0))
	{
	    luaV_emsg(L);
	    ఆపు;
	}

	// Catch the command switching to another buffer.
	// Check the line number, the command may have deleted lines.
	ఒకవేళ (curbuf != was_curbuf || l > curbuf->b_ml.ml_పంక్తి_లెక్క)
	    ఆపు;

	ఒకవేళ (lua_isstring(L, -1)) // update line?
	{
# ifdef HAVE_SANDBOX
	    luaV_checksandbox(L);
# endif
	    ml_replace(l, luaV_toline(L, -1), TRUE);
	    మారింది_బైట్లు(l, 0);
	    lua_తీయి(L, 1); // result from luaV_toline
	}
	lua_తీయి(L, 1); //పంక్తి
    }
    lua_తీయి(L, 1); // function
    check_cursor();
    update_screen(UPD_NOT_VALID);
}

    శూన్య
ex_luafile(exarg_T *eap)
{
    ఒకవేళ (lua_init() == FAIL)
	తిరిగి;
    ఒకవేళ (!eap->skip)
    {
	luaV_setrange(L, eap->line1, eap->line2);
	ఒకవేళ (luaL_loadfile(L, (అక్షరం *) eap->arg) || lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
}

# define luaV_freetype(typ,tname) \
	శూన్య \
    lua_##tname##_free(typ *o) \
    { \
	ఒకవేళ (!lua_isopen()) తిరిగి; \
	luaV_getfield(L, LUAVIM_FREE); \
	lua_pushlightuserdata(L, (శూన్య *) o); \
	lua_call(L, 1, 0); \
    }

luaV_freetype(buf_T, బఫరు)
luaV_freetype(win_T, కిటికీ_2)

    శూన్య
do_luaeval(char_u *str, typval_T *arg, typval_T *rettv)
{
    lua_init();
    luaV_getfield(L, LUAVIM_LUAEVAL);
    lua_pushstring(L, (అక్షరం *) str);
    lua_pushlightuserdata(L, (శూన్య *) arg);
    lua_pushlightuserdata(L, (శూన్య *) rettv);
    lua_call(L, 3, 0);
}

    పూర్ణాంకం
సమితి_ref_లోపల_lua(పూర్ణాంకం copyID)
{
    పూర్ణాంకం aborted = 0;

    ఒకవేళ (!lua_isopen())
	తిరిగి 0;

    luaV_getfield(L, LUAVIM_SETREF);
    // call the function with 1 arg, getting 1 result back
    lua_pushinteger(L, copyID);
    lua_call(L, 1, 1);
    // get the result
    aborted = lua_tointeger(L, -1);
    // pop result off the stack
    lua_తీయి(L, 1);

    తిరిగి aborted;
}

    శూన్య
update_package_paths_లోపల_lua(శూన్య)
{
    ఒకవేళ (!lua_isopen())
	తిరిగి;

    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "_update_package_paths");

    ఒకవేళ (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);
}

/*
 * Native C function callback
 */
    స్థిర పూర్ణాంకం
luaV_call_lua_func(
	పూర్ణాంకం	 argcount,
	typval_T *argvars,
	typval_T *rettv,
	శూన్య	 *state)
{
    పూర్ణాంకం i;
    పూర్ణాంకం luaargcount = argcount;
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_funcref);

    ఒకవేళ (funcstate->lua_tableref != LUA_NOREF)
    {
	// First arg for metatable __call method is a table
	luaargcount += 1;
	lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    }

    కొరకు (i = 0; i < argcount; ++i)
	luaV_pushtypval(funcstate->L, &argvars[i]);

    ఒకవేళ (lua_pcall(funcstate->L, luaargcount, 1, 0))
    {
	luaV_emsg(funcstate->L);
	తిరిగి (పూర్ణాంకం)FCERR_OTHER;
    }

    luaV_checktypval(funcstate->L, -1, rettv, "get return value");
    తిరిగి (పూర్ణాంకం)FCERR_NONE;
}

/*
 * Free up any lua references held by the func state.
 */
    స్థిర శూన్య
luaV_call_lua_func_free(శూన్య *state)
{
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_funcref);
    funcstate->L = ఖాళీ;
    ఒకవేళ (funcstate->lua_tableref != LUA_NOREF)
	luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    VIM_CLEAR(funcstate);
}

#అయితే_ముగింపు
