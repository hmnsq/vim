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

#شامل "vim.h"
#شامل "version.h"

#شامل <lua.h>
#شامل <lualib.h>
#شامل <lauxlib.h>

#اگر __STDC_VERSION__ >= 199901L
# define LUAV_INLINE اندرونی
#ورنہ
# define LUAV_INLINE
#اگر_ختم

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#اگر defined(FEAT_LUA)

# define LUAVIM_CHUNKNAME "vim chunk"
# define LUAVIM_NAME "vim"
# define LUAVIM_EVALNAME "luaeval"
# define LUAVIM_EVALHEADER "local _A=select(1,...) return "

# ifdef LUA_RELEASE
#  define LUAVIM_نسخہ LUA_RELEASE
# ورنہ
#  define LUAVIM_نسخہ LUA_نسخہ
# endif

قسمتعریف buf_T *lua_V_بفر;
قسمتعریف win_T *lua_V_کھڑکی;
قسمتعریف dict_T *luaV_Dict;
قسمتعریف list_T *luaV_List;
قسمتعریف blob_T *luaV_Blob;
قسمتعریف ساخت {
    char_u	*name;	// funcref
    dict_T	*self;	// selfdict
} luaV_Funcref;
قسمتعریف صحیح (*msgfunc_T)(حرف *);

قسمتعریف ساخت {
    صحیح lua_funcref;    // ref to a lua func
    صحیح lua_tableref;   // ref to a lua table if metatable else LUA_NOREF. used
			// for __call
    lua_State *L;
} luaV_CFuncState;

ساکن ثابت حرف LUAVIM_DICT[] = "dict";
ساکن ثابت حرف LUAVIM_LIST[] = "list";
ساکن ثابت حرف LUAVIM_BLOB[] = "blob";
ساکن ثابت حرف LUAVIM_FUNCREF[] = "funcref";
ساکن ثابت حرف LUAVIM_بفر[] = "buffer";
ساکن ثابت حرف LUAVIM_کھڑکی[] = "window";
ساکن ثابت حرف LUAVIM_FREE[] = "luaV_free";
ساکن ثابت حرف LUAVIM_LUAEVAL[] = "luaV_luaeval";
ساکن ثابت حرف LUAVIM_SETREF[] = "luaV_setref";

ساکن ثابت حرف LUA___CALL[] = "__call";

// get/setudata manage references to vim userdata in a cache table through
// object pointers (light userdata). The cache table itself is retrieved
// from the registry.

ساکن ثابت حرف LUAVIM_UDATA_CACHE[] = "luaV_udata_cache";

# define luaV_getfield(L, s) \
    lua_pushlightuserdata((L), (خالی *)(s)); \
    lua_rawget((L), LUA_REGISTRYINDEX)
# define luaV_checksandbox(L) \
    اگر (sandbox) luaL_error((L), "not allowed in sandbox")
# define lua_V_پیغام(L) luaV_msgfunc((L), (msgfunc_T) پیغام)
# define luaV_emsg(L) luaV_msgfunc((L), (msgfunc_T) emsg)
# define luaV_checktypval(L, a, v, پیغام) \
    کرو { \
	اگر (luaV_totypval(L, a, v) == FAIL) \
	    luaL_error(L, پیغام ": cannot convert value"); \
    } جب (0)

ساکن luaV_List *luaV_pushlist(lua_State *L, list_T *lis);
ساکن luaV_Dict *luaV_pushdict(lua_State *L, dict_T *dic);
ساکن luaV_Blob *luaV_pushblob(lua_State *L, blob_T *blo);
ساکن luaV_Funcref *luaV_pushfuncref(lua_State *L, char_u *name);
ساکن صحیح luaV_call_lua_func(صحیح argcount, typval_T *argvars, typval_T *rettv, خالی *state);
ساکن خالی luaV_call_lua_func_free(خالی *state);

# اگر LUA_نسخہ_عدد <= 501
#  define lua_V_رجسٹر(L, l) lua_L_رجسٹر(L, تہی, l)
#  define luaL_typeerror luaL_typerror
# ورنہ
#  define lua_V_رجسٹر(L, l) luaL_setfuncs(L, l, 0)
# endif

# ifdef DYNAMIC_LUA

#  ifdef MSWIN
#   define بھرو_dll vim_بھرو_Lib
#   define علامت_from_dll Get_Proc_پتہ
#   define close_dll FreeLibrary
#   define بھرو_dll_error GetWin32Error
#  ورنہ
#   include <dlfcn.h>
#   define HANDLE خالی*
#   define بھرو_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#   define علامت_from_dll dlsym
#   define close_dll dlclose
#   define بھرو_dll_error dlerror
#  endif

// lauxlib
#  اگر LUA_نسخہ_عدد <= 501
#   define lua_L_رجسٹر dll_lua_L_رجسٹر
#   define luaL_prepbuffer dll_luaL_prepbuffer
#   define luaL_openlib dll_luaL_openlib
#   define luaL_typerror dll_luaL_typerror
#   define luaL_loadfile dll_luaL_loadfile
#   define luaL_loadbuffer dll_luaL_loadbuffer
#  ورنہ
#   define luaL_prepbuffsize dll_luaL_prepbuffsize
#   define luaL_setfuncs dll_luaL_setfuncs
#   define luaL_loadfilex dll_luaL_loadfilex
#   define luaL_loadbufferx dll_luaL_loadbufferx
#   define luaL_argerror dll_luaL_argerror
#  endif
#  اگر LUA_نسخہ_عدد >= 504
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
#  اگر LUA_نسخہ_عدد <= 501
#   define lua_tonumber dll_lua_tonumber
#   define lua_tointeger dll_lua_tointeger
#   define lua_call dll_lua_call
#   define lua_pcall dll_lua_pcall
#  ورنہ
#   define lua_tonumberx dll_lua_tonumberx
#   define lua_tointegerx dll_lua_tointegerx
#   define lua_callk dll_lua_callk
#   define lua_pcallk dll_lua_pcallk
#   define lua_getglobal dll_lua_getglobal
#   define lua_setglobal dll_lua_setglobal
#  endif
#  اگر LUA_نسخہ_عدد <= 502
#   define lua_replace dll_lua_replace
#   define lua_remove dll_lua_remove
#  endif
#  اگر LUA_نسخہ_عدد >= 503
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
#  اگر LUA_نسخہ_عدد >= 504
#   define lua_newuserdatauv dll_lua_newuserdatauv
#  ورنہ
#   define lua_newuserdata dll_lua_newuserdata
#  endif
#  define lua_getmetatable dll_lua_getmetatable
#  define lua_setfield dll_lua_setfield
#  define lua_rawset dll_lua_rawset
#  define lua_rawseti dll_lua_rawseti
#  define lua_setmetatable dll_lua_setmetatable
#  define lua_اگلا dll_lua_اگلا
// libs
#  define luaopen_بنیاد dll_luaopen_بنیاد
#  define luaopen_جدول dll_luaopen_جدول
#  define luaopen_string dll_luaopen_string
#  define luaopen_math dll_luaopen_math
#  define luaopen_io dll_luaopen_io
#  define luaopen_os dll_luaopen_os
#  define luaopen_package dll_luaopen_package
#  define luaopen_debug dll_luaopen_debug
#  اگر LUA_نسخہ_عدد >= 505
#   define luaL_openselectedlibs dll_luaL_openselectedlibs
#  ورنہ
#   define luaL_openlibs dll_luaL_openlibs
#  endif

// lauxlib
#  اگر LUA_نسخہ_عدد <= 501
خالی (*dll_lua_L_رجسٹر) (lua_State *L, ثابت حرف *libname, ثابت luaL_Reg *l);
حرف *(*dll_luaL_prepbuffer) (lua_L_بفر *B);
خالی (*dll_luaL_openlib) (lua_State *L, ثابت حرف *libname, ثابت luaL_Reg *l, صحیح nup);
صحیح (*dll_luaL_typerror) (lua_State *L, صحیح narg, ثابت حرف *tname);
صحیح (*dll_luaL_loadfile) (lua_State *L, ثابت حرف *filename);
صحیح (*dll_luaL_loadbuffer) (lua_State *L, ثابت حرف *buff, size_t sz, ثابت حرف *name);
#  ورنہ
حرف *(*dll_luaL_prepbuffsize) (lua_L_بفر *B, size_t sz);
خالی (*dll_luaL_setfuncs) (lua_State *L, ثابت luaL_Reg *l, صحیح nup);
صحیح (*dll_luaL_loadfilex) (lua_State *L, ثابت حرف *filename, ثابت حرف *منوال);
صحیح (*dll_luaL_loadbufferx) (lua_State *L, ثابت حرف *buff, size_t sz, ثابت حرف *name, ثابت حرف *منوال);
صحیح (*dll_luaL_argerror) (lua_State *L, صحیح numarg, ثابت حرف *extramsg);
#  endif
#  اگر LUA_نسخہ_عدد >= 504
صحیح (*dll_luaL_typeerror) (lua_State *L, صحیح narg, ثابت حرف *tname);
#  endif
خالی (*dll_luaL_checkany) (lua_State *L, صحیح narg);
ثابت حرف *(*dll_luaL_checklstring) (lua_State *L, صحیح عدد_Arg, size_t *l);
lua_Integer (*dll_luaL_checkinteger) (lua_State *L, صحیح عدد_Arg);
lua_Integer (*dll_luaL_optinteger) (lua_State *L, صحیح nArg, lua_Integer def);
خالی (*dll_luaL_checktype) (lua_State *L, صحیح narg, صحیح t);
صحیح (*dll_luaL_error) (lua_State *L, ثابت حرف *fmt, ...);
lua_State *(*dll_luaL_newstate) (خالی);
خالی (*dll_luaL_buffinit) (lua_State *L, lua_L_بفر *B);
خالی (*dll_luaL_addlstring) (lua_L_بفر *B, ثابت حرف *s, size_t l);
خالی (*dll_luaL_pushresult) (lua_L_بفر *B);
صحیح (*dll_luaL_loadstring) (lua_State *L, ثابت حرف *s);
صحیح (*dll_luaL_ref) (lua_State *L, صحیح idx);
#  اگر LUA_نسخہ_عدد <= 502
خالی (*dll_luaL_unref) (lua_State *L, صحیح idx, صحیح n);
#  ورنہ
خالی (*dll_luaL_unref) (lua_State *L, صحیح idx, lua_Integer n);
#  endif
// lua
#  اگر LUA_نسخہ_عدد <= 501
lua_نمبر (*dll_lua_tonumber) (lua_State *L, صحیح idx);
lua_Integer (*dll_lua_tointeger) (lua_State *L, صحیح idx);
خالی (*dll_lua_call) (lua_State *L, صحیح nargs, صحیح nresults);
صحیح (*dll_lua_pcall) (lua_State *L, صحیح nargs, صحیح nresults, صحیح errfunc);
#  ورنہ
lua_نمبر (*dll_lua_tonumberx) (lua_State *L, صحیح idx, صحیح *isnum);
lua_Integer (*dll_lua_tointegerx) (lua_State *L, صحیح idx, صحیح *isnum);
خالی (*dll_lua_callk) (lua_State *L, صحیح nargs, صحیح nresults, صحیح ctx,
	lua_CFunction k);
صحیح (*dll_lua_pcallk) (lua_State *L, صحیح nargs, صحیح nresults, صحیح errfunc,
	صحیح ctx, lua_CFunction k);
خالی (*dll_lua_getglobal) (lua_State *L, ثابت حرف *var);
خالی (*dll_lua_setglobal) (lua_State *L, ثابت حرف *var);
#  endif
#  اگر LUA_نسخہ_عدد <= 502
خالی (*dll_lua_replace) (lua_State *L, صحیح idx);
خالی (*dll_lua_remove) (lua_State *L, صحیح idx);
#  endif
#  اگر LUA_نسخہ_عدد >= 503
خالی  (*dll_lua_rotate) (lua_State *L, صحیح idx, صحیح n);
خالی (*dll_lua_copy) (lua_State *L, صحیح fromidx, صحیح toidx);
#  endif
ثابت حرف *(*dll_lua_typename) (lua_State *L, صحیح tp);
خالی       (*dll_lua_close) (lua_State *L);
صحیح (*dll_lua_gettop) (lua_State *L);
خالی (*dll_lua_settop) (lua_State *L, صحیح idx);
خالی (*dll_lua_pushvalue) (lua_State *L, صحیح idx);
صحیح (*dll_lua_isnumber) (lua_State *L, صحیح idx);
صحیح (*dll_lua_isstring) (lua_State *L, صحیح idx);
صحیح (*dll_lua_type) (lua_State *L, صحیح idx);
صحیح (*dll_lua_rawequal) (lua_State *L, صحیح idx1, صحیح idx2);
صحیح (*dll_lua_toboolean) (lua_State *L, صحیح idx);
ثابت حرف *(*dll_lua_tolstring) (lua_State *L, صحیح idx, size_t *len);
خالی *(*dll_lua_touserdata) (lua_State *L, صحیح idx);
خالی (*dll_lua_pushnil) (lua_State *L);
خالی (*dll_lua_pushnumber) (lua_State *L, lua_نمبر n);
خالی (*dll_lua_pushinteger) (lua_State *L, lua_Integer n);
خالی (*dll_lua_pushlstring) (lua_State *L, ثابت حرف *s, size_t l);
خالی (*dll_lua_pushstring) (lua_State *L, ثابت حرف *s);
ثابت حرف *(*dll_lua_pushfstring) (lua_State *L, ثابت حرف *fmt, ...);
خالی (*dll_lua_pushcclosure) (lua_State *L, lua_CFunction fn, صحیح n);
خالی (*dll_lua_pushboolean) (lua_State *L, صحیح b);
خالی (*dll_lua_pushlightuserdata) (lua_State *L, خالی *p);
خالی (*dll_lua_getfield) (lua_State *L, صحیح idx, ثابت حرف *k);
#  اگر LUA_نسخہ_عدد <= 502
خالی (*dll_lua_rawget) (lua_State *L, صحیح idx);
خالی (*dll_lua_rawgeti) (lua_State *L, صحیح idx, صحیح n);
#  ورنہ
صحیح (*dll_lua_rawget) (lua_State *L, صحیح idx);
صحیح (*dll_lua_rawgeti) (lua_State *L, صحیح idx, lua_Integer n);
#  endif
خالی (*dll_lua_createtable) (lua_State *L, صحیح narr, صحیح nrec);
خالی (*dll_lua_settable) (lua_State *L, صحیح idx);
#  اگر LUA_نسخہ_عدد >= 504
خالی *(*dll_lua_newuserdatauv) (lua_State *L, size_t sz, صحیح nuvalue);
#  ورنہ
خالی *(*dll_lua_newuserdata) (lua_State *L, size_t sz);
#  endif
صحیح (*dll_lua_getmetatable) (lua_State *L, صحیح objindex);
خالی (*dll_lua_setfield) (lua_State *L, صحیح idx, ثابت حرف *k);
خالی (*dll_lua_rawset) (lua_State *L, صحیح idx);
#  اگر LUA_نسخہ_عدد <= 502
خالی (*dll_lua_rawseti) (lua_State *L, صحیح idx, صحیح n);
#  ورنہ
خالی (*dll_lua_rawseti) (lua_State *L, صحیح idx, lua_Integer n);
#  endif
صحیح (*dll_lua_setmetatable) (lua_State *L, صحیح objindex);
صحیح (*dll_lua_اگلا) (lua_State *L, صحیح idx);
// libs
صحیح (*dll_luaopen_بنیاد) (lua_State *L);
صحیح (*dll_luaopen_جدول) (lua_State *L);
صحیح (*dll_luaopen_string) (lua_State *L);
صحیح (*dll_luaopen_math) (lua_State *L);
صحیح (*dll_luaopen_io) (lua_State *L);
صحیح (*dll_luaopen_os) (lua_State *L);
صحیح (*dll_luaopen_package) (lua_State *L);
صحیح (*dll_luaopen_debug) (lua_State *L);
#  اگر LUA_نسخہ_عدد >= 505
خالی (*dll_luaL_openselectedlibs) (lua_State *L, صحیح بھرو, صحیح preload);
#  ورنہ
خالی (*dll_luaL_openlibs) (lua_State *L);
#  endif

قسمتعریف خالی **luaV_function;
قسمتعریف ساخت {
    ثابت حرف *name;
    luaV_function func;
} luaV_Reg;

ساکن ثابت luaV_Reg luaV_dll[] = {
    // lauxlib
#  اگر LUA_نسخہ_عدد <= 501
    {"luaL_register", (luaV_function) &dll_lua_L_رجسٹر},
    {"luaL_prepbuffer", (luaV_function) &dll_luaL_prepbuffer},
    {"luaL_openlib", (luaV_function) &dll_luaL_openlib},
    {"luaL_typerror", (luaV_function) &dll_luaL_typerror},
    {"luaL_loadfile", (luaV_function) &dll_luaL_loadfile},
    {"luaL_loadbuffer", (luaV_function) &dll_luaL_loadbuffer},
#  ورنہ
    {"luaL_prepbuffsize", (luaV_function) &dll_luaL_prepbuffsize},
    {"luaL_setfuncs", (luaV_function) &dll_luaL_setfuncs},
    {"luaL_loadfilex", (luaV_function) &dll_luaL_loadfilex},
    {"luaL_loadbufferx", (luaV_function) &dll_luaL_loadbufferx},
    {"luaL_argerror", (luaV_function) &dll_luaL_argerror},
#  endif
#  اگر LUA_نسخہ_عدد >= 504
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
#  اگر LUA_نسخہ_عدد <= 501
    {"lua_tonumber", (luaV_function) &dll_lua_tonumber},
    {"lua_tointeger", (luaV_function) &dll_lua_tointeger},
    {"lua_call", (luaV_function) &dll_lua_call},
    {"lua_pcall", (luaV_function) &dll_lua_pcall},
#  ورنہ
    {"lua_tonumberx", (luaV_function) &dll_lua_tonumberx},
    {"lua_tointegerx", (luaV_function) &dll_lua_tointegerx},
    {"lua_callk", (luaV_function) &dll_lua_callk},
    {"lua_pcallk", (luaV_function) &dll_lua_pcallk},
    {"lua_getglobal", (luaV_function) &dll_lua_getglobal},
    {"lua_setglobal", (luaV_function) &dll_lua_setglobal},
#  endif
#  اگر LUA_نسخہ_عدد <= 502
    {"lua_replace", (luaV_function) &dll_lua_replace},
    {"lua_remove", (luaV_function) &dll_lua_remove},
#  endif
#  اگر LUA_نسخہ_عدد >= 503
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
#  اگر LUA_نسخہ_عدد >= 504
    {"lua_newuserdatauv", (luaV_function) &dll_lua_newuserdatauv},
#  ورنہ
    {"lua_newuserdata", (luaV_function) &dll_lua_newuserdata},
#  endif
    {"lua_getmetatable", (luaV_function) &dll_lua_getmetatable},
    {"lua_setfield", (luaV_function) &dll_lua_setfield},
    {"lua_rawset", (luaV_function) &dll_lua_rawset},
    {"lua_rawseti", (luaV_function) &dll_lua_rawseti},
    {"lua_setmetatable", (luaV_function) &dll_lua_setmetatable},
    {"lua_next", (luaV_function) &dll_lua_اگلا},
    // libs
    {"luaopen_base", (luaV_function) &dll_luaopen_بنیاد},
    {"luaopen_table", (luaV_function) &dll_luaopen_جدول},
    {"luaopen_string", (luaV_function) &dll_luaopen_string},
    {"luaopen_math", (luaV_function) &dll_luaopen_math},
    {"luaopen_io", (luaV_function) &dll_luaopen_io},
    {"luaopen_os", (luaV_function) &dll_luaopen_os},
    {"luaopen_package", (luaV_function) &dll_luaopen_package},
    {"luaopen_debug", (luaV_function) &dll_luaopen_debug},
#  اگر LUA_نسخہ_عدد >= 505
    {"luaL_openselectedlibs", (luaV_function) &dll_luaL_openselectedlibs},
#  ورنہ
    {"luaL_openlibs", (luaV_function) &dll_luaL_openlibs},
#  endif
    {تہی, تہی}
};

ساکن HANDLE hinstLua = تہی;

    ساکن صحیح
lua_link_init(حرف *libname, صحیح verbose)
{
    ثابت luaV_Reg *reg;
    اگر (hinstLua) واپس OK;
    hinstLua = بھرو_dll(libname);
    اگر (!hinstLua)
    {
	اگر (verbose)
	    semsg(_(e_could_not_بھرو_library_str_str), libname, بھرو_dll_error());
	واپس FAIL;
    }
    برائے (reg = luaV_dll; reg->func; reg++)
    {
	اگر ((*reg->func = علامت_from_dll(hinstLua, reg->name)) == تہی)
	{
	    close_dll(hinstLua);
	    hinstLua = 0;
	    اگر (verbose)
		semsg(_(e_could_not_بھرو_library_function_str), reg->name);
	    واپس FAIL;
	}
    }
    واپس OK;
}
# endif // DYNAMIC_LUA

# اگر defined(DYNAMIC_LUA)
    صحیح
lua_enabled(صحیح verbose)
{
    واپس lua_link_init((حرف *)p_luadll, verbose) == OK;
}
# endif

# اگر LUA_نسخہ_عدد > 501 && LUA_نسخہ_عدد < 504
    ساکن صحیح
luaL_typeerror(lua_State *L, صحیح narg, ثابت حرف *tname)
{
    ثابت حرف *پیغام = lua_pushfstring(L, "%s expected, got %s",
	    tname, luaL_typename(L, narg));
    واپس luaL_argerror(L, narg, پیغام);
}
# endif

    ساکن LUAV_INLINE خالی
luaV_getudata(lua_State *L, خالی *v)
{
    lua_pushlightuserdata(L, (خالی *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // now the cache table is at the top of the stack
    lua_pushlightuserdata(L, v);
    lua_rawget(L, -2);
    lua_remove(L, -2);  // remove the cache table from the stack
}

    ساکن LUAV_INLINE خالی
luaV_setudata(lua_State *L, خالی *v)
{
    lua_pushlightuserdata(L, (خالی *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // cache table is at -1
    lua_pushlightuserdata(L, v);       // ...now at -2
    lua_pushvalue(L, -3);	       // copy the userdata (cache at -3)
    lua_rawset(L, -3);		       // consumes two stack items
    lua_نکالو(L, 1);		       // and remove the cache table
}

// =======   Internal   =======

    ساکن خالی
luaV_newmetatable(lua_State *L, ثابت حرف *tname)
{
    lua_newtable(L);
    lua_pushlightuserdata(L, (خالی *) tname);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
}

    ساکن خالی *
luaV_toudata(lua_State *L, صحیح ud, ثابت حرف *tname)
{
    خالی *p = lua_touserdata(L, ud);

    اگر (p == تہی)
	واپس تہی;

    // value is userdata
    اگر (lua_getmetatable(L, ud)) // does it have a metatable?
    {
	luaV_getfield(L, tname); // get metatable
	اگر (lua_rawequal(L, -1, -2)) // MTs match?
	{
	    lua_نکالو(L, 2); // MTs
	    واپس p;
	}
    }

    واپس تہی;
}

    ساکن خالی *
luaV_checkcache(lua_State *L, خالی *p)
{
    luaV_getudata(L, p);
    اگر (lua_isnil(L, -1)) luaL_error(L, "invalid object");
    lua_نکالو(L, 1);
    واپس p;
}

# define luaV_unbox(L,luatyp,ud) (*((luatyp *) lua_touserdata((L),(ud))))

# define luaV_checkvalid(L,luatyp,ud) \
    luaV_checkcache((L), (خالی *) luaV_unbox((L),luatyp,(ud)))

    ساکن خالی *
luaV_checkudata(lua_State *L, صحیح ud, ثابت حرف *tname)
{
    خالی *p = luaV_toudata(L, ud, tname);
    اگر (p == تہی) luaL_typeerror(L, ud, tname);
    واپس p;
}

    ساکن خالی
luaV_pushtypval(lua_State *L, typval_T *tv)
{
    اگر (tv == تہی)
    {
	lua_pushnil(L);
	واپس;
    }
    انتخاب (tv->v_type)
    {
	حالت VAR_STRING:
	    lua_pushstring(L, tv->vval.v_string == تہی
					    ? "" : (حرف *)tv->vval.v_string);
	    توڑو;
	حالت VAR_نمبر:
	    lua_pushinteger(L, (صحیح) tv->vval.v_نمبر);
	    توڑو;
	حالت VAR_FLOAT:
	    lua_pushnumber(L, (lua_نمبر) tv->vval.v_float);
	    توڑو;
	حالت VAR_LIST:
	    luaV_pushlist(L, tv->vval.v_list);
	    توڑو;
	حالت VAR_DICT:
	    luaV_pushdict(L, tv->vval.v_dict);
	    توڑو;
	حالت VAR_BOOL:
	حالت VAR_SPECIAL:
	    اگر (tv->vval.v_نمبر <= VVAL_TRUE)
		lua_pushboolean(L, (صحیح) tv->vval.v_نمبر);
	    ورنہ
		lua_pushnil(L);
	    توڑو;
	حالت VAR_FUNC:
	    luaV_pushfuncref(L, tv->vval.v_string);
	    توڑو;
	حالت VAR_PARTIAL:
	    // TODO: handle partial arguments
	    luaV_pushfuncref(L, partial_name(tv->vval.v_partial));
	    توڑو;

	حالت VAR_BLOB:
	    luaV_pushblob(L, tv->vval.v_blob);
	    توڑو;
	طےشدہ:
	    lua_pushnil(L);
    }
}

/*
 * Converts lua value at 'pos' to typval 'tv'.
 * Returns OK or FAIL.
 */
    ساکن صحیح
luaV_totypval(lua_State *L, صحیح pos, typval_T *tv)
{
    صحیح حالت_2 = OK;

    tv->v_تالا = 0;

    انتخاب (lua_type(L, pos))
    {
	حالت LUA_TBOOLEAN:
	    tv->v_type = VAR_BOOL;
	    tv->vval.v_نمبر = (varnumber_T) lua_toboolean(L, pos);
	    توڑو;
	حالت LUA_TNIL:
	    tv->v_type = VAR_SPECIAL;
	    tv->vval.v_نمبر = VVAL_NULL;
	    توڑو;
	حالت LUA_TSTRING:
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strsave((char_u *) lua_tostring(L, pos));
	    توڑو;
	حالت LUA_TNUMBER:
	{
	    ثابت lua_نمبر n = lua_tonumber(L, pos);

	    اگر (n > (lua_نمبر)INT64_MAX || n < (lua_نمبر)INT64_MIN
		    || ((lua_نمبر)((varnumber_T)n)) != n)
	    {
		tv->v_type = VAR_FLOAT;
		tv->vval.v_float = (float_T)n;
	    }
	    ورنہ
	    {
		tv->v_type = VAR_نمبر;
		tv->vval.v_نمبر = (varnumber_T)n;
	    }
	}
	    توڑو;
	حالت LUA_TFUNCTION:
	{
	    char_u *name;
	    luaV_CFuncState *state;

	    lua_pushvalue(L, pos);
	    state = ALLOC_CLEAR_ONE(luaV_CFuncState);
	    state->lua_funcref = luaL_ref(L, LUA_REGISTRYINDEX);
	    state->L = L;
	    state->lua_tableref = LUA_NOREF;
	    name = رجسٹر_cfunc(&luaV_call_lua_func,
					      &luaV_call_lua_func_free, state);
	    tv->v_type = VAR_FUNC;
	    tv->vval.v_string = vim_strsave(name);
	    توڑو;
	}
	حالت LUA_TTABLE:
	{
	    صحیح lua_tableref;

	    lua_pushvalue(L, pos);
	    lua_tableref = luaL_ref(L, LUA_REGISTRYINDEX);
	    اگر (lua_getmetatable(L, pos))
	    {
		lua_getfield(L, -1, LUA___CALL);
		اگر (lua_isfunction(L, -1))
		{
		    char_u *name;
		    صحیح lua_funcref = luaL_ref(L, LUA_REGISTRYINDEX);
		    luaV_CFuncState *state = ALLOC_CLEAR_ONE(luaV_CFuncState);

		    state->lua_funcref = lua_funcref;
		    state->L = L;
		    state->lua_tableref = lua_tableref;
		    name = رجسٹر_cfunc(&luaV_call_lua_func,
					      &luaV_call_lua_func_free, state);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(name);
		    توڑو;
		}
	    }
	    tv->v_type = VAR_نمبر;
	    tv->vval.v_نمبر = 0;
	    حالت_2 = FAIL;
	    توڑو;
	}
	حالت LUA_TUSERDATA:
	{
	    خالی *p = lua_touserdata(L, pos);

	    اگر (lua_getmetatable(L, pos)) // has metatable?
	    {
		// check list
		luaV_getfield(L, LUAVIM_LIST);
		اگر (lua_rawequal(L, -1, -2))
		{
		    tv->v_type = VAR_LIST;
		    tv->vval.v_list = *((luaV_List *) p);
		    ++tv->vval.v_list->lv_refcount;
		    lua_نکالو(L, 2); // MTs
		    توڑو;
		}
		// check dict
		luaV_getfield(L, LUAVIM_DICT);
		اگر (lua_rawequal(L, -1, -3))
		{
		    tv->v_type = VAR_DICT;
		    tv->vval.v_dict = *((luaV_Dict *) p);
		    ++tv->vval.v_dict->dv_refcount;
		    lua_نکالو(L, 3); // MTs
		    توڑو;
		}
		// check blob
		luaV_getfield(L, LUAVIM_BLOB);
		اگر (lua_rawequal(L, -1, -4))
		{
		    tv->v_type = VAR_BLOB;
		    tv->vval.v_blob = *((luaV_Blob *) p);
		    ++tv->vval.v_blob->bv_refcount;
		    lua_نکالو(L, 4); // MTs
		    توڑو;
		}
		// check funcref
		luaV_getfield(L, LUAVIM_FUNCREF);
		اگر (lua_rawequal(L, -1, -5))
		{
		    luaV_Funcref *f = (luaV_Funcref *) p;

		    func_ref(f->name);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(f->name);
		    lua_نکالو(L, 5); // MTs
		    توڑو;
		}
		lua_نکالو(L, 4); // MTs
	    }
	}
	// FALLTHROUGH
	طےشدہ:
	    tv->v_type = VAR_نمبر;
	    tv->vval.v_نمبر = 0;
	    حالت_2 = FAIL;
    }
    واپس حالت_2;
}

/*
 * similar to luaL_addlstring, but replaces \0 with \n if toline and
 * \n with \0 otherwise
 */
    ساکن خالی
luaV_addlstring(lua_L_بفر *b, ثابت حرف *s, size_t l, صحیح toline)
{
    جب (l--)
    {
	اگر (*s == '\0' && toline)
	    luaL_addchar(b, '\n');
	ورنہ اگر (*s == '\n' && !toline)
	    luaL_addchar(b, '\0');
	ورنہ
	    luaL_addchar(b, *s);
	s++;
    }
}

    ساکن خالی
luaV_pushline(lua_State *L, buf_T *buf, linenr_T n)
{
    ثابت حرف *s = (ثابت حرف *) ml_get_buf(buf, n, FALSE);
    lua_L_بفر b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, لڑی_لمبائی(s), 0);
    luaL_pushresult(&b);
}

    ساکن char_u *
luaV_toline(lua_State *L, صحیح pos)
{
    size_t l;
    ثابت حرف *s = lua_tolstring(L, pos, &l);

    lua_L_بفر b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 1);
    luaL_pushresult(&b);
    واپس (char_u *) lua_tostring(L, -1);
}

/*
 * pops a string s from the top of the stack and calls mf(t) for pieces t of
 * s separated by newlines
 */
    ساکن خالی
luaV_msgfunc(lua_State *L, msgfunc_T mf)
{
    lua_L_بفر b;
    size_t l;
    ثابت حرف *p, *s = lua_tolstring(L, -1, &l);
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 0);
    luaL_pushresult(&b);
    // break string
    p = s = lua_tolstring(L, -1, &l);
    جب (l--)
    {
	اگر (*p++ == '\0') // break?
	{
	    mf((حرف *)s);
	    s = p;
	}
    }
    mf((حرف *)s);
    lua_نکالو(L, 2); // original and modified strings
}

# define luaV_newtype(typ,tname,luatyp,luatname) \
	ساکن luatyp * \
    luaV_new##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = (luatyp *) lua_newuserdata(L, حجم(luatyp)); \
	*o = obj; \
	luaV_setudata(L, obj); /* cache[obj] = udata */ \
	luaV_getfield(L, luatname); \
	lua_setmetatable(L, -2); \
	واپس o; \
    }

# define luaV_pushtype(typ,tname,luatyp) \
	ساکن luatyp * \
    lua_V_ڈالو##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = تہی; \
	اگر (obj == تہی) \
	    lua_pushnil(L); \
	ورنہ \
	{ \
	    luaV_getudata(L, obj); \
	    اگر (lua_isnil(L, -1)) /* not interned? */ \
	    { \
		lua_نکالو(L, 1); \
		o = luaV_new##tname(L, obj); \
	    } \
	    ورنہ \
		o = (luatyp *) lua_touserdata(L, -1); \
	} \
	واپس o; \
    }

# define luaV_type_tostring(tname,luatname) \
	ساکن صحیح \
    luaV_##tname##_tostring(lua_State *L) \
    { \
	lua_pushfstring(L, "%s: %p", luatname, lua_touserdata(L, 1)); \
	واپس 1; \
    }

// =======   List type   =======

    ساکن luaV_List *
luaV_newlist(lua_State *L, list_T *lis)
{
    luaV_List *l = (luaV_List *) lua_newuserdata(L, حجم(luaV_List));
    *l = lis;
    lis->lv_refcount++; // reference in Lua
    luaV_setudata(L, lis); // cache[lis] = udata
    luaV_getfield(L, LUAVIM_LIST);
    lua_setmetatable(L, -2);
    واپس l;
}

luaV_pushtype(list_T, list, luaV_List)
luaV_type_tostring(list, LUAVIM_LIST)

    ساکن صحیح
luaV_list_len(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushinteger(L, (صحیح) list_len(l));
    واپس 1;
}

    ساکن صحیح
luaV_list_iter(lua_State *L)
{
    listitem_T *li = (listitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    اگر (li == تہی) واپس 0;
    luaV_pushtypval(L, &li->li_tv);
    lua_pushlightuserdata(L, (خالی *) li->li_اگلا);
    lua_replace(L, lua_upvalueindex(1));
    واپس 1;
}

    ساکن صحیح
luaV_list_call(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushlightuserdata(L, (خالی *) l->lv_پہلا);
    lua_pushcclosure(L, luaV_list_iter, 1);
    واپس 1;
}

    ساکن صحیح
lua_V_list_اشاریہ(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    اگر (lua_isnumber(L, 2)) // list item?
    {
	طویل n = (طویل) luaL_checkinteger(L, 2);
	listitem_T *li;

	// Lua array index starts with 1 while Vim uses 0, subtract 1 to
	// normalize.
	n -= 1;
	li = list_ڈھونڈو(l, n);
	اگر (li == تہی)
	    lua_pushnil(L);
	ورنہ
	    luaV_pushtypval(L, &li->li_tv);
    }
    ورنہ اگر (lua_isstring(L, 2)) // method?
    {
	ثابت حرف *s = lua_tostring(L, 2);
	اگر (لڑی_موازنہ(s, "add", 3) == 0
		|| لڑی_موازنہ(s, "insert", 6) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	ورنہ
	    lua_pushnil(L);
    }
    ورنہ
	lua_pushnil(L);
    واپس 1;
}

    ساکن صحیح
luaV_list_newindex(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    طویل n = (طویل) luaL_checkinteger(L, 2);
    listitem_T *li;

    // Lua array index starts with 1 while Vim uses 0, subtract 1 to normalize.
    n -= 1;

    اگر (l->lv_تالا)
	luaL_error(L, "list is locked");
    li = list_ڈھونڈو(l, n);
    اگر (li == تہی)
    {
	اگر (!lua_isnil(L, 3))
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "inserting list item");
	    اگر (list_داخل_کرو_tv(l, &v, li) == FAIL)
		luaL_error(L, "failed to add item to list");
	    clear_tv(&v);
	}
    }
    ورنہ
    {
	اگر (lua_isnil(L, 3)) // remove?
	{
	    vimlist_remove(l, li, li);
	    listitem_free(l, li);
	}
	ورنہ
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "setting list item");
	    clear_tv(&li->li_tv);
	    li->li_tv = v;
	}
    }
    واپس 0;
}

    ساکن صحیح
lua_V_list_جوڑو(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (خالی *) *lis);
    typval_T v;
    اگر (l->lv_تالا)
	luaL_error(L, "list is locked");
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "adding list item");
    اگر (list_append_tv(l, &v) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    واپس 1;
}

    ساکن صحیح
lua_V_list_داخل_کرو(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (خالی *) *lis);
    طویل pos = (طویل) luaL_optinteger(L, 3, 0);
    listitem_T *li = تہی;
    typval_T v;
    اگر (l->lv_تالا)
	luaL_error(L, "list is locked");
    اگر (pos < l->lv_len)
    {
	li = list_ڈھونڈو(l, pos);
	اگر (li == تہی)
	    luaL_error(L, "invalid position");
    }
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "inserting list item");
    اگر (list_داخل_کرو_tv(l, &v, li) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    واپس 1;
}

ساکن ثابت luaL_Reg luaV_List_mt[] = {
    {"__tostring", luaV_list_tostring},
    {"__len", luaV_list_len},
    {"__call", luaV_list_call},
    {"__index", lua_V_list_اشاریہ},
    {"__newindex", luaV_list_newindex},
    {"add", lua_V_list_جوڑو},
    {"insert", lua_V_list_داخل_کرو},
    {تہی, تہی}
};


// =======   Dict type   =======

    ساکن luaV_Dict *
luaV_newdict(lua_State *L, dict_T *dic)
{
    luaV_Dict *d = (luaV_Dict *) lua_newuserdata(L, حجم(luaV_Dict));
    *d = dic;
    dic->dv_refcount++; // reference in Lua
    luaV_setudata(L, dic); // cache[dic] = udata
    luaV_getfield(L, LUAVIM_DICT);
    lua_setmetatable(L, -2);
    واپس d;
}

luaV_pushtype(dict_T, dict, luaV_Dict)
luaV_type_tostring(dict, LUAVIM_DICT)

    ساکن صحیح
luaV_dict_len(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    lua_pushinteger(L, (صحیح) dict_len(d));
    واپس 1;
}

    ساکن صحیح
luaV_dict_iter(lua_State *L غیر_مستعمل)
{
# ifdef FEAT_EVAL
    hashitem_T *hi = (hashitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    صحیح n = lua_tointeger(L, lua_upvalueindex(2));
    dictitem_T *di;
    اگر (n <= 0) واپس 0;
    جب (HASHITEM_خالی_خانے(hi)) hi++;
    di = dict_lookup(hi);
    lua_pushstring(L, (حرف *) hi->hi_کلید);
    luaV_pushtypval(L, &di->di_tv);
    lua_pushlightuserdata(L, (خالی *) (hi + 1));
    lua_replace(L, lua_upvalueindex(1));
    lua_pushinteger(L, n - 1);
    lua_replace(L, lua_upvalueindex(2));
    واپس 2;
# ورنہ
    واپس 0;
# endif
}

    ساکن صحیح
luaV_dict_call(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    hashtab_T *ht = &d->dv_hashtab;
    lua_pushlightuserdata(L, (خالی *) ht->ht_array);
    lua_pushinteger(L, ht->ht_مستعمل); // # remaining items
    lua_pushcclosure(L, luaV_dict_iter, 2);
    واپس 1;
}

    ساکن صحیح
lua_V_dict_اشاریہ(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *کلید = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di = dict_ڈھونڈو(d, کلید, -1);

    اگر (di == تہی)
    {
	lua_pushnil(L);
	واپس 1;
    }

    luaV_pushtypval(L, &di->di_tv);
    اگر (di->di_tv.v_type == VAR_FUNC) // funcref?
    {
	luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, -1);
	f->self = d; // keep "self" reference
	d->dv_refcount++;
    }

    واپس 1;
}

    ساکن صحیح
luaV_dict_newindex(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *کلید = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di;
    typval_T tv;

    اگر (d->dv_تالا)
	luaL_error(L, "dict is locked");
    اگر (کلید == تہی)
	واپس 0;
    اگر (*کلید == NUL)
	luaL_error(L, "empty key");
    اگر (!lua_isnil(L, 3)) // read value?
    {
	luaV_checktypval(L, 3, &tv, "setting dict item");
	اگر (d->dv_scope == VAR_DEF_SCOPE && tv.v_type == VAR_FUNC)
	{
	    clear_tv(&tv);
	    luaL_error(L, "cannot assign funcref to builtin scope");
	}
    }
    di = dict_ڈھونڈو(d, کلید, -1);
    اگر (di == تہی) // non-existing key?
    {
	اگر (lua_isnil(L, 3))
	    واپس 0;
	di = dictitem_alloc(کلید);
	اگر (di == تہی)
	{
	    clear_tv(&tv);
	    واپس 0;
	}
	اگر (dict_جوڑو(d, di) == FAIL)
	{
	    vim_free(di);
	    clear_tv(&tv);
	    واپس 0;
	}
    }
    ورنہ
	clear_tv(&di->di_tv);
    اگر (lua_isnil(L, 3)) // remove?
    {
	hashitem_T *hi = hash_ڈھونڈو(&d->dv_hashtab, di->di_کلید);
	hash_remove(&d->dv_hashtab, hi, "Lua new index");
	dictitem_free(di);
    }
    ورنہ
	di->di_tv = tv;
    واپس 0;
}

ساکن ثابت luaL_Reg luaV_Dict_mt[] = {
    {"__tostring", luaV_dict_tostring},
    {"__len", luaV_dict_len},
    {"__call", luaV_dict_call},
    {"__index", lua_V_dict_اشاریہ},
    {"__newindex", luaV_dict_newindex},
    {تہی, تہی}
};


// =======   Blob type   =======

    ساکن luaV_Blob *
luaV_newblob(lua_State *L, blob_T *blo)
{
    luaV_Blob *b = (luaV_Blob *) lua_newuserdata(L, حجم(luaV_Blob));
    *b = blo;
    blo->bv_refcount++; // reference in Lua
    luaV_setudata(L, blo); // cache[blo] = udata
    luaV_getfield(L, LUAVIM_BLOB);
    lua_setmetatable(L, -2);
    واپس b;
}

luaV_pushtype(blob_T, blob, luaV_Blob)
luaV_type_tostring(blob, LUAVIM_BLOB)

    ساکن صحیح
luaV_blob_gc(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    blob_unref(b);
    واپس 0;
}

    ساکن صحیح
luaV_blob_len(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    lua_pushinteger(L, (صحیح) blob_len(b));
    واپس 1;
}

    ساکن صحیح
lua_V_blob_اشاریہ(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    اگر (lua_isnumber(L, 2))
    {
	صحیح idx = luaL_checkinteger(L, 2);
	اگر (idx < 0)
	    // Negative index is relative to the end.
	    idx += (صحیح) blob_len(b);
	اگر (idx >= 0 && idx < blob_len(b))
	    lua_pushnumber(L, (lua_نمبر) blob_get(b, idx));
	ورنہ
	    lua_pushnil(L);
    }
    ورنہ اگر (lua_isstring(L, 2))
    {
	ثابت حرف *s = lua_tostring(L, 2);
	اگر (لڑی_موازنہ(s, "add", 3) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	ورنہ
	    lua_pushnil(L);
    }
    ورنہ
	lua_pushnil(L);
    واپس 1;
}

    ساکن صحیح
luaV_blob_newindex(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    اگر (b->bv_تالا)
	luaL_error(L, "blob is locked");

    اگر (!lua_isnumber(L, 2))
	واپس 0;

    طویل len = blob_len(b);
    صحیح idx = luaL_checkinteger(L, 2);
    صحیح val = luaL_checkinteger(L, 3);
    اگر (idx < 0)
	// Negative index is relative to the end.
	idx += (صحیح) len;
    اگر ((idx >= 0 && idx < len)
	    || (idx == len && ga_grow(&b->bv_ga, 1) == OK))
    {
	blob_سیٹ(b, idx, (char_u) val);
	اگر (idx == len)
	    ++b->bv_ga.ga_len;
    }
    ورنہ
	luaL_error(L, "index out of range");

    واپس 0;
}

    ساکن صحیح
lua_V_blob_جوڑو(lua_State *L)
{
    luaV_Blob *blo = luaV_checkudata(L, 1, LUAVIM_BLOB);
    blob_T *b = (blob_T *) luaV_checkcache(L, (خالی *) *blo);
    اگر (b->bv_تالا)
	luaL_error(L, "blob is locked");
    lua_settop(L, 2);
    اگر (!lua_isstring(L, 2))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 2));
    ورنہ
    {
	size_t i, l = 0;
	ثابت حرف *s = lua_tolstring(L, 2, &l);

	اگر (ga_grow(&b->bv_ga, (صحیح)l) == OK)
	    برائے (i = 0; i < l; ++i)
		ga_append(&b->bv_ga, s[i]);
    }
    lua_settop(L, 1);
    واپس 1;
}

ساکن ثابت luaL_Reg luaV_Blob_mt[] = {
    {"__tostring", luaV_blob_tostring},
    {"__gc", luaV_blob_gc},
    {"__len", luaV_blob_len},
    {"__index", lua_V_blob_اشاریہ},
    {"__newindex", luaV_blob_newindex},
    {"add", lua_V_blob_جوڑو},
    {تہی, تہی}
};


// =======   Funcref type   =======

    ساکن luaV_Funcref *
luaV_newfuncref(lua_State *L, char_u *name)
{
    luaV_Funcref *f = (luaV_Funcref *)lua_newuserdata(L, حجم(luaV_Funcref));

    اگر (name != تہی)
    {
	func_ref(name);
	f->name = vim_strsave(name);
    }
    f->self = تہی;
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_setmetatable(L, -2);
    واپس f;
}

    ساکن luaV_Funcref *
luaV_pushfuncref(lua_State *L, char_u *name)
{
    واپس luaV_newfuncref(L, name);
}


luaV_type_tostring(funcref, LUAVIM_FUNCREF)

    ساکن صحیح
luaV_funcref_gc(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    func_unref(f->name);
    vim_free(f->name);
    // NOTE: Don't call "dict_unref(f->self)", because the dict of "f->self"
    // will be (or has been already) freed by Vim's garbage collection.
    واپس 0;
}

// equivalent to string(funcref)
    ساکن صحیح
luaV_funcref_len(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    lua_pushstring(L, (ثابت حرف *) f->name);
    واپس 1;
}

    ساکن صحیح
luaV_funcref_call(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);
    صحیح i, n = lua_gettop(L) - 1; // #args
    صحیح حالت_2 = FAIL;
    typval_T دلائل;
    typval_T rettv;

    دلائل.v_type = VAR_LIST;
    دلائل.vval.v_list = list_alloc();
    rettv.v_type = VAR_UNKNOWN; // as in clear_tv
    اگر (دلائل.vval.v_list != تہی)
    {
	typval_T v;

	برائے (i = 0; i < n; i++)
	{
	    luaV_checktypval(L, i + 2, &v, "calling funcref");
	    list_append_tv(دلائل.vval.v_list, &v);
	    clear_tv(&v);
	}
	حالت_2 = func_call(f->name, &دلائل, تہی, f->self, &rettv);
	اگر (حالت_2 == OK)
	    luaV_pushtypval(L, &rettv);
	clear_tv(&دلائل);
	clear_tv(&rettv);
    }
    اگر (حالت_2 != OK)
	luaL_error(L, "cannot call funcref");
    واپس 1;
}

ساکن ثابت luaL_Reg luaV_Funcref_mt[] = {
    {"__tostring", luaV_funcref_tostring},
    {"__gc", luaV_funcref_gc},
    {"__len", luaV_funcref_len},
    {"__call", luaV_funcref_call},
    {تہی, تہی}
};


// =======   Buffer type   =======

luaV_newtype(buf_T, بفر, lua_V_بفر, LUAVIM_بفر)
luaV_pushtype(buf_T, بفر, lua_V_بفر)
luaV_type_tostring(بفر, LUAVIM_بفر)

    ساکن صحیح
lua_V_بفر_len(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, lua_V_بفر, 1);
    lua_pushinteger(L, b->b_ml.ml_سطر_گنتی);
    واپس 1;
}

    ساکن صحیح
lua_V_بفر_call(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, lua_V_بفر, 1);
    lua_settop(L, 1);
    سیٹ_curbuf(b, DOBUF_بانٹو);
    واپس 1;
}

    ساکن صحیح
lua_V_بفر_اشاریہ(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, lua_V_بفر, 1);
    linenr_T n = (linenr_T) lua_tointeger(L, 2);
    اگر (n > 0 && n <= b->b_ml.ml_سطر_گنتی)
	luaV_pushline(L, b, n);
    ورنہ اگر (lua_isstring(L, 2))
    {
	ثابت حرف *s = lua_tostring(L, 2);
	اگر (لڑی_موازنہ(s, "name", 4) == 0)
	    lua_pushstring(L, (b->b_sfname == تہی)
					? "" : (حرف *) b->b_sfname);
	ورنہ اگر (لڑی_موازنہ(s, "fname", 5) == 0)
	    lua_pushstring(L, (b->b_ffname == تہی)
					? "" : (حرف *) b->b_ffname);
	ورنہ اگر (لڑی_موازنہ(s, "number", 6) == 0)
	    lua_pushinteger(L, b->b_fnum);
	// methods
	ورنہ اگر (لڑی_موازنہ(s,   "insert", 6) == 0
		|| لڑی_موازنہ(s, "next", 4) == 0
		|| لڑی_موازنہ(s, "previous", 8) == 0
		|| لڑی_موازنہ(s, "isvalid", 7) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	ورنہ
	    lua_pushnil(L);
    }
    ورنہ
	lua_pushnil(L);
    واپس 1;
}

    ساکن صحیح
lua_V_بفر_newindex(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, lua_V_بفر, 1);
    linenr_T n = (linenr_T) luaL_checkinteger(L, 2);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    اگر (n < 1 || n > b->b_ml.ml_سطر_گنتی)
	luaL_error(L, "invalid line number");
    اگر (lua_isnil(L, 3)) // delete line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	اگر (u_savedel(n, 1L) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	ورنہ اگر (ml_delete(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot delete line");
	}
	ورنہ
	{
	    deleted_سطریں_mark(n, 1L);
	    اگر (b == curwin->w_بفر) // fix cursor in current window?
	    {
		اگر (curwin->w_cursor.lnum >= n)
		{
		    اگر (curwin->w_cursor.lnum > n)
		    {
			curwin->w_cursor.lnum -= 1;
			check_cursor_ستون();
		    }
		    ورنہ
			check_cursor();
		    بدلا_cline_bef_curs();
		}
		invalidate_botline();
	    }
	}
	curbuf = buf;
    }
    ورنہ اگر (lua_isstring(L, 3)) // update line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	اگر (u_savesub(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	ورنہ اگر (ml_replace(n, luaV_toline(L, 3), TRUE) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot replace line");
	}
	ورنہ
	    بدلا_بائٹیں(n, 0);
	curbuf = buf;
	اگر (b == curwin->w_بفر)
	    check_cursor_ستون();
    }
    ورنہ
	luaL_error(L, "wrong argument to change line");
    واپس 0;
}

    ساکن صحیح
lua_V_بفر_داخل_کرو(lua_State *L)
{
    lua_V_بفر *lb = luaV_checkudata(L, 1, LUAVIM_بفر);
    buf_T *b = (buf_T *) luaV_checkcache(L, (خالی *) *lb);
    linenr_T آخری = b->b_ml.ml_سطر_گنتی;
    linenr_T n = (linenr_T) luaL_optinteger(L, 3, آخری);
    buf_T *buf;
    luaL_checktype(L, 2, LUA_TSTRING);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    // fix insertion line
    اگر (n < 0) n = 0;
    اگر (n > آخری) n = آخری;
    // insert
    buf = curbuf;
    curbuf = b;
    اگر (u_save(n, n + 1) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot save undo information");
    }
    ورنہ اگر (ml_append(n, luaV_toline(L, 2), 0, FALSE) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot insert line");
    }
    ورنہ
	appended_سطریں_mark(n, 1L);
    curbuf = buf;
    update_screen(UPD_VALID);
    واپس 0;
}

    ساکن صحیح
lua_V_بفر_اگلا(lua_State *L)
{
    lua_V_بفر *b = luaV_checkudata(L, 1, LUAVIM_بفر);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (خالی *) *b);
    luaV_pushbuffer(L, buf->b_اگلا);
    واپس 1;
}

    ساکن صحیح
lua_V_بفر_پچھلا(lua_State *L)
{
    lua_V_بفر *b = luaV_checkudata(L, 1, LUAVIM_بفر);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (خالی *) *b);
    luaV_pushbuffer(L, buf->b_پچھلا);
    واپس 1;
}

    ساکن صحیح
lua_V_بفر_isvalid(lua_State *L)
{
    lua_V_بفر *b = luaV_checkudata(L, 1, LUAVIM_بفر);
    luaV_getudata(L, *b);
    lua_pushboolean(L, !lua_isnil(L, -1));
    واپس 1;
}

ساکن ثابت luaL_Reg lua_V_بفر_mt[] = {
    {"__tostring", lua_V_بفر_tostring},
    {"__len", lua_V_بفر_len},
    {"__call", lua_V_بفر_call},
    {"__index", lua_V_بفر_اشاریہ},
    {"__newindex", lua_V_بفر_newindex},
    {"insert", lua_V_بفر_داخل_کرو},
    {"next", lua_V_بفر_اگلا},
    {"previous", lua_V_بفر_پچھلا},
    {"isvalid", lua_V_بفر_isvalid},
    {تہی, تہی}
};


// =======   Window type   =======

luaV_newtype(win_T, کھڑکی_2, lua_V_کھڑکی, LUAVIM_کھڑکی)
luaV_pushtype(win_T, کھڑکی_2, lua_V_کھڑکی)
luaV_type_tostring(کھڑکی_2, LUAVIM_کھڑکی)

    ساکن صحیح
lua_V_کھڑکی_call(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, lua_V_کھڑکی, 1);
    lua_settop(L, 1);
    win_goto(w);
    واپس 1;
}

    ساکن صحیح
lua_V_کھڑکی_اشاریہ(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, lua_V_کھڑکی, 1);
    ثابت حرف *s = luaL_checkstring(L, 2);
    اگر (لڑی_موازنہ(s, "buffer", 6) == 0)
	luaV_pushbuffer(L, w->w_بفر);
    ورنہ اگر (لڑی_موازنہ(s, "line", 4) == 0)
	lua_pushinteger(L, w->w_cursor.lnum);
    ورنہ اگر (لڑی_موازنہ(s, "col", 3) == 0)
	lua_pushinteger(L, w->w_cursor.ستون + 1);
    ورنہ اگر (لڑی_موازنہ(s, "width", 5) == 0)
	lua_pushinteger(L, w->w_چوڑائی);
    ورنہ اگر (لڑی_موازنہ(s, "height", 6) == 0)
	lua_pushinteger(L, w->w_اونچائی);
    // methods
    ورنہ اگر (لڑی_موازنہ(s,   "next", 4) == 0
	    || لڑی_موازنہ(s, "previous", 8) == 0
	    || لڑی_موازنہ(s, "isvalid", 7) == 0)
    {
	lua_getmetatable(L, 1);
	lua_getfield(L, -1, s);
    }
    ورنہ
	lua_pushnil(L);
    واپس 1;
}

    ساکن صحیح
lua_V_کھڑکی_newindex(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, lua_V_کھڑکی, 1);
    ثابت حرف *s = luaL_checkstring(L, 2);
    صحیح v = luaL_checkinteger(L, 3);
    اگر (لڑی_موازنہ(s, "line", 4) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	اگر (v < 1 || v > w->w_بفر->b_ml.ml_سطر_گنتی)
	    luaL_error(L, "line out of range");
	w->w_cursor.lnum = v;
	update_screen(UPD_VALID);
    }
    ورنہ اگر (لڑی_موازنہ(s, "col", 3) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	w->w_cursor.ستون = v - 1;
	w->w_سیٹ_curswant = true;
	update_screen(UPD_VALID);
    }
    ورنہ اگر (لڑی_موازنہ(s, "width", 5) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setwidth(v);
	curwin = win;
    }
    ورنہ اگر (لڑی_موازنہ(s, "height", 6) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setheight(v);
	curwin = win;
    }
    ورنہ
	luaL_error(L, "invalid window property: `%s'", s);
    واپس 0;
}

    ساکن صحیح
lua_V_کھڑکی_اگلا(lua_State *L)
{
    lua_V_کھڑکی *w = luaV_checkudata(L, 1, LUAVIM_کھڑکی);
    win_T *win = (win_T *) luaV_checkcache(L, (خالی *) *w);
    luaV_pushwindow(L, win->w_اگلا);
    واپس 1;
}

    ساکن صحیح
lua_V_کھڑکی_پچھلا(lua_State *L)
{
    lua_V_کھڑکی *w = luaV_checkudata(L, 1, LUAVIM_کھڑکی);
    win_T *win = (win_T *) luaV_checkcache(L, (خالی *) *w);
    luaV_pushwindow(L, win->w_پچھلا);
    واپس 1;
}

    ساکن صحیح
lua_V_کھڑکی_isvalid(lua_State *L)
{
    lua_V_کھڑکی *w = luaV_checkudata(L, 1, LUAVIM_کھڑکی);
    luaV_getudata(L, *w);
    lua_pushboolean(L, !lua_isnil(L, -1));
    واپس 1;
}

ساکن ثابت luaL_Reg lua_V_کھڑکی_mt[] = {
    {"__tostring", lua_V_کھڑکی_tostring},
    {"__call", lua_V_کھڑکی_call},
    {"__index", lua_V_کھڑکی_اشاریہ},
    {"__newindex", lua_V_کھڑکی_newindex},
    {"next", lua_V_کھڑکی_اگلا},
    {"previous", lua_V_کھڑکی_پچھلا},
    {"isvalid", lua_V_کھڑکی_isvalid},
    {تہی, تہی}
};


// =======   Vim module   =======

    ساکن صحیح
luaV_print(lua_State *L)
{
    صحیح		i, n = lua_gettop(L); // nargs
    ثابت حرف	*s;
    size_t	l;
    garray_T	پیغام_ga;

    ga_init2(&پیغام_ga, 1, 128);
    lua_getglobal(L, "tostring");
    برائے (i = 1; i <= n; i++)
    {
	lua_pushvalue(L, -1); // tostring
	lua_pushvalue(L, i); // arg
	lua_call(L, 1, 1);
	s = lua_tolstring(L, -1, &l);
	اگر (s == تہی)
	    واپس luaL_error(L, "cannot convert to string");
	اگر (i > 1)
	    ga_append(&پیغام_ga, ' '); // use space instead of tab
	ga_concat_len(&پیغام_ga, (char_u *)s, l);
	lua_نکالو(L, 1);
    }
    // Replace any "\n" with "\0"
    برائے (i = 0; i < پیغام_ga.ga_len; i++)
	اگر (((حرف *)پیغام_ga.ga_ڈیٹا)[i] == '\n')
	    ((حرف *)پیغام_ga.ga_ڈیٹا)[i] = '\0';
    lua_pushlstring(L, پیغام_ga.ga_ڈیٹا, پیغام_ga.ga_len);
    اگر (!got_int)
	lua_V_پیغام(L);

    ga_clear(&پیغام_ga);
    واپس 0;
}

    ساکن صحیح
luaV_debug(lua_State *L)
{
    lua_settop(L, 0);
    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "eval");
    lua_remove(L, -2); // vim.eval at position 1
    برائے (;;)
    {
	ثابت حرف *داخلہ;
	size_t l;
	lua_pushvalue(L, 1); // vim.eval
	lua_pushliteral(L, "input('lua_debug> ')");
	lua_call(L, 1, 1); // return string
	داخلہ = lua_tolstring(L, -1, &l);
	اگر (l == 0 || strcmp(داخلہ, "cont") == 0)
	    واپس 0;
	پیغام_putchar('\n'); // avoid outputting on input line
	اگر (luaL_loadbuffer(L, داخلہ, l, "=(debug command)")
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
	lua_settop(L, 1); // remove eventual returns, but keep vim.eval
    }
}

    ساکن dict_T *
luaV_get_var_scope(lua_State *L)
{
    ثابت حرف	*scope = luaL_checkstring(L, 1);
    dict_T	*dict = تہی;

    اگر (STRICMP((حرف *)scope, "g") == 0)
	dict = get_globvar_dict();
    ورنہ اگر (STRICMP((حرف *)scope, "v") == 0)
	dict = get_vimvar_dict();
    ورنہ اگر (STRICMP((حرف *)scope, "b") == 0)
	dict = curbuf->b_vars;
    ورنہ اگر (STRICMP((حرف *)scope, "w") == 0)
	dict = curwin->w_vars;
    ورنہ اگر (STRICMP((حرف *)scope, "t") == 0)
	dict = curtab->tp_vars;
    ورنہ
    {
	luaL_error(L, "invalid scope %s", scope);
	واپس تہی;
    }

    واپس dict;
}

    ساکن صحیح
luaV_setvar(lua_State *L)
{
    dict_T	*dict;
    dictitem_T	*di;
    size_t	len;
    حرف	*name;
    صحیح		del;
    حرف	*error = تہی;

    name = (حرف *)luaL_checklstring(L, 3, &len);
    del = (lua_gettop(L) < 4) || lua_isnil(L, 4);

    dict = luaV_get_var_scope(L);
    اگر (dict == تہی)
	واپس 0;

    di = dict_ڈھونڈو(dict, (char_u *)name, (صحیح)len);
    اگر (di != تہی)
    {
	اگر (di->di_flags & DI_FLAGS_RO)
	    error = "variable is read-only";
	ورنہ اگر (di->di_flags & DI_FLAGS_تالا)
	    error = "variable is locked";
	ورنہ اگر (del && di->di_flags & DI_FLAGS_FIX)
	    error = "variable is fixed";
	اگر (error != تہی)
	    واپس luaL_error(L, error);
    }
    ورنہ اگر (dict->dv_تالا)
	واپس luaL_error(L, "Dictionary is locked");

    اگر (del)
    {
	// Delete the key
	اگر (di == تہی)
	    // Doesn't exist, nothing to do
	    واپس 0;
	// Delete the entry
	dictitem_remove(dict, di, "Lua delete variable");
    }
    ورنہ
    {
	// Update the key
	typval_T	tv;

	// Convert the lua value to a Vim script type in the temporary variable
	lua_pushvalue(L, 4);
	اگر (luaV_totypval(L, -1, &tv) == FAIL)
	    واپس luaL_error(L, "Couldn't convert lua value");

	اگر (di == تہی)
	{
	    // Need to create an entry
	    di = dictitem_alloc((char_u *)name);
	    اگر (di == تہی)
	    {
		clear_tv(&tv);
		واپس 0;
	    }
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	    اگر (dict_جوڑو(dict, di) == FAIL)
	    {
		dictitem_free(di);
		clear_tv(&tv);
		واپس luaL_error(L, "Couldn't add to dictionary");
	    }
	}
	ورنہ
	{
	    صحیح type_error = FALSE;
	    اگر (dict == get_vimvar_dict()
	       && !before_سیٹ_vvar((char_u *)name, di, &tv, TRUE, &type_error))
	    {
		clear_tv(&tv);
		اگر (type_error)
		    واپس luaL_error(L,
				"Setting v:%s to value with wrong type", name);
		واپس 0;
	    }
	    // Clear the old value
	    clear_tv(&di->di_tv);
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	}

	// Clear the temporary variable
	clear_tv(&tv);
    }

    واپس 0;
}

    ساکن صحیح
luaV_getvar(lua_State *L)
{
    dict_T	*dict = luaV_get_var_scope(L);
    size_t	len;
    ثابت حرف	*name = luaL_checklstring(L, 3, &len);
    dictitem_T	*di = dict_ڈھونڈو(dict, (char_u *)name, (صحیح)len);

    اگر (di == تہی)
	واپس 0;  // nil

    luaV_pushtypval(L, &di->di_tv);
    واپس 1;
}

    ساکن صحیح
luaV_command(lua_State *L)
{
    char_u  *s = vim_strsave((char_u *)luaL_checkstring(L, 1));

    execute_cmds_from_string(s);
    vim_free(s);
    update_screen(UPD_VALID);
    واپس 0;
}

    ساکن صحیح
luaV_eval(lua_State *L)
{
    typval_T *tv = eval_اظہاریہ((char_u *) luaL_checkstring(L, 1), تہی);

    اگر (tv == تہی) luaL_error(L, "invalid expression");
    luaV_pushtypval(L, tv);
    free_tv(tv);
    واپس 1;
}

    ساکن صحیح
luaV_beep(lua_State *L غیر_مستعمل)
{
    vim_beep(BO_LANG);
    واپس 0;
}

    ساکن صحیح
lua_V_سطر(lua_State *L)
{
    luaV_pushline(L, curbuf, curwin->w_cursor.lnum);
    واپس 1;
}

    ساکن صحیح
luaV_list(lua_State *L)
{
    list_T *l;
    صحیح initarg = !lua_isnoneornil(L, 1);

    اگر (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    l = list_alloc();
    اگر (l == تہی)
    {
	lua_pushnil(L);
	واپس 1;
    }

    luaV_newlist(L, l);
    اگر (!initarg)
	واپس 1;

    // traverse table to init list
    صحیح notnil, i = 0;
    typval_T v;
    کرو
    {
	lua_rawgeti(L, 1, ++i);
	notnil = !lua_isnil(L, -1);
	اگر (notnil)
	{
	    luaV_checktypval(L, -1, &v, "vim.list");
	    list_append_tv(l, &v);
	    clear_tv(&v);
	}
	lua_نکالو(L, 1); //قدر
    } جب (notnil);

    واپس 1;
}

    ساکن صحیح
luaV_dict(lua_State *L)
{
    dict_T *d;
    صحیح initarg = !lua_isnoneornil(L, 1);

    اگر (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    d = dict_alloc();
    اگر (d == تہی)
    {
	lua_pushnil(L);
	واپس 1;
    }

    luaV_newdict(L, d);
    اگر (!initarg)
	واپس 1;

    // traverse table to init dict
    lua_pushnil(L);
    جب (lua_اگلا(L, 1))
    {
	char_u *کلید;
	dictitem_T *di;
	typval_T v;

	lua_pushvalue(L, -2); // dup key in case it's a number
	کلید = (char_u *) lua_tostring(L, -1);
	اگر (کلید == تہی)
	{
	    lua_pushnil(L);
	    واپس 1;
	}
	اگر (*کلید == NUL)
	    luaL_error(L, "table has empty key");
	luaV_checktypval(L, -2, &v, "vim.dict"); //قدر
	di = dictitem_alloc(کلید);
	اگر (di == تہی || dict_جوڑو(d, di) == FAIL)
	{
	    vim_free(di);
	    lua_pushnil(L);
	    واپس 1;
	}
	di->di_tv = v;
	lua_نکالو(L, 2); // key copy and value
    }

    واپس 1;
}

    ساکن صحیح
luaV_blob(lua_State *L)
{
    blob_T *b;
    صحیح initarg = !lua_isnoneornil(L, 1);

    اگر (initarg && !lua_isstring(L, 1))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 1));

    b = blob_alloc();
    اگر (b == تہی)
    {
	lua_pushnil(L);
	واپس 1;
    }

    luaV_newblob(L, b);
    اگر (!initarg)
	واپس 1;

    // traverse table to init blob
    size_t i, l = 0;
    ثابت حرف *s = lua_tolstring(L, 1, &l);

    اگر (ga_grow(&b->bv_ga, (صحیح)l) == OK)
	برائے (i = 0; i < l; ++i)
	    ga_append(&b->bv_ga, s[i]);

    واپس 1;
}

    ساکن صحیح
luaV_funcref(lua_State *L)
{
    ثابت حرف *name = luaL_checkstring(L, 1);
    // note: not checking if function exists (needs function_exists)
    اگر (name == تہی || *name == NUL || VIM_ISDIGIT(*name))
	luaL_error(L, "invalid function name: %s", name);
    luaV_newfuncref(L, (char_u *) name);
    واپس 1;
}

    ساکن صحیح
lua_V_بفر_2(lua_State *L)
{
    buf_T *buf;
    اگر (lua_isstring(L, 1)) // get by number or name?
    {
	اگر (lua_isnumber(L, 1)) // by number?
	{
	    صحیح n = lua_tointeger(L, 1);
	    FOR_ALL_BUFFERS(buf)
		اگر (buf->b_fnum == n) توڑو;
	}
	ورنہ // by name
	{
	    size_t l;
	    ثابت حرف *s = lua_tolstring(L, 1, &l);
	    FOR_ALL_BUFFERS(buf)
	    {
		اگر (buf->b_ffname == تہی || buf->b_sfname == تہی)
		{
		    اگر (l == 0) توڑو;
		}
		ورنہ اگر (لڑی_موازنہ(s, (حرف *)buf->b_ffname, l) == 0
			|| لڑی_موازنہ(s, (حرف *)buf->b_sfname, l) == 0)
		    توڑو;
	    }
	}
    }
    ورنہ
	buf = (lua_toboolean(L, 1)) ? firstbuf : curbuf; // first buffer?
    luaV_pushbuffer(L, buf);
    واپس 1;
}

    ساکن صحیح
lua_V_کھڑکی_2(lua_State *L)
{
    win_T *win;
    اگر (lua_isnumber(L, 1)) // get by number?
    {
	صحیح n = lua_tointeger(L, 1);
	برائے (win = firstwin; win != تہی; win = win->w_اگلا, n--)
	    اگر (n == 1) توڑو;
    }
    ورنہ
	win = (lua_toboolean(L, 1)) ? firstwin : curwin; // first window?
    luaV_pushwindow(L, win);
    واپس 1;
}

    ساکن صحیح
luaV_open(lua_State *L)
{
    char_u *s = تہی;
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    اگر (lua_isstring(L, 1)) s = (char_u *) lua_tostring(L, 1);
    luaV_pushbuffer(L, buflist_new(s, تہی, 1L, BLN_LISTED));
    واپس 1;
}

    ساکن صحیح
luaV_type(lua_State *L)
{
    luaL_checkany(L, 1);
    اگر (lua_type(L, 1) == LUA_TUSERDATA) // check vim udata?
    {
	lua_settop(L, 1);
	اگر (lua_getmetatable(L, 1))
	{
	    luaV_getfield(L, LUAVIM_LIST);
	    اگر (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "list");
		واپس 1;
	    }
	    luaV_getfield(L, LUAVIM_DICT);
	    اگر (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "dict");
		واپس 1;
	    }
	    luaV_getfield(L, LUAVIM_BLOB);
	    اگر (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "blob");
		واپس 1;
	    }
	    luaV_getfield(L, LUAVIM_FUNCREF);
	    اگر (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "funcref");
		واپس 1;
	    }
	    luaV_getfield(L, LUAVIM_بفر);
	    اگر (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "buffer");
		واپس 1;
	    }
	    luaV_getfield(L, LUAVIM_کھڑکی);
	    اگر (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "window");
		واپس 1;
	    }
	}
    }
    lua_pushstring(L, luaL_typename(L, 1)); // fallback
    واپس 1;
}

    ساکن صحیح
luaV_call(lua_State *L)
{
    صحیح		argc = lua_gettop(L) - 1;
    size_t	funcname_len;
    char_u	*funcname;
    حرف	*error = تہی;
    typval_T	rettv;
    typval_T	argv[MAX_FUNC_دلائل + 1];
    صحیح		i = 0;

    اگر (argc > MAX_FUNC_دلائل)
	واپس luaL_error(L, "Function called with too many arguments");

    funcname = (char_u *)luaL_checklstring(L, 1, &funcname_len);

    برائے (; i < argc; i++)
    {
	اگر (luaV_totypval(L, i + 2, &argv[i]) == FAIL)
	{
	    error = "lua: cannot convert value";
	    جاؤ free_vim_دلائل;
	}
    }

    argv[argc].v_type = VAR_UNKNOWN;

    اگر (call_vim_function(funcname, argc, argv, &rettv) == FAIL)
    {
	error = "lua: call_vim_function failed";
	جاؤ free_vim_دلائل;
    }

    luaV_pushtypval(L, &rettv);
    clear_tv(&rettv);

free_vim_دلائل:
    جب (i > 0)
	clear_tv(&argv[--i]);

    اگر (error == تہی)
	واپس 1;
    ورنہ
	واپس luaL_error(L, error);
}

/*
 * Return the Vim version as a Lua table
 */
    ساکن صحیح
lua_V_نسخہ(lua_State *L)
{
    lua_newtable(L);
    lua_pushstring(L, "major");
    lua_pushinteger(L, VIM_نسخہ_MAJOR);
    lua_settable(L, -3);
    lua_pushstring(L, "minor");
    lua_pushinteger(L, VIM_نسخہ_MINOR);
    lua_settable(L, -3);
    lua_pushstring(L, "patch");
    lua_pushinteger(L, highest_patch());
    lua_settable(L, -3);
    واپس 1;
}

ساکن ثابت luaL_Reg luaV_module[] = {
    {"command", luaV_command},
    {"eval", luaV_eval},
    {"beep", luaV_beep},
    {"line", lua_V_سطر},
    {"list", luaV_list},
    {"dict", luaV_dict},
    {"blob", luaV_blob},
    {"funcref", luaV_funcref},
    {"buffer", lua_V_بفر_2},
    {"window", lua_V_کھڑکی_2},
    {"open", luaV_open},
    {"type", luaV_type},
    {"call", luaV_call},
    {"_getvar", luaV_getvar},
    {"_setvar", luaV_setvar},
    {"version", lua_V_نسخہ},
    {"lua_version", تہی},
    {تہی, تہی}
};

/*
 * for freeing list, dict, buffer and window objects; lightuserdata as arg
 */
    ساکن صحیح
luaV_free(lua_State *L)
{
    lua_pushnil(L);
    luaV_setudata(L, lua_touserdata(L, 1));
    واپس 0;
}

    ساکن صحیح
luaV_luaeval(lua_State *L)
{
    lua_L_بفر b;
    size_t l;
    ثابت حرف *str = lua_tolstring(L, 1, &l);
    typval_T *arg = (typval_T *) lua_touserdata(L, 2);
    typval_T *rettv = (typval_T *) lua_touserdata(L, 3);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, LUAVIM_EVALHEADER, حجم(LUAVIM_EVALHEADER) - 1);
    luaL_addlstring(&b, str, l);
    luaL_pushresult(&b);
    str = lua_tolstring(L, -1, &l);
    اگر (luaL_loadbuffer(L, str, l, LUAVIM_EVALNAME)) // compile error?
    {
	luaV_emsg(L);
	واپس 0;
    }
    luaV_pushtypval(L, arg);
    اگر (lua_pcall(L, 1, 1, 0)) // running error?
    {
	luaV_emsg(L);
	واپس 0;
    }
    اگر (luaV_totypval(L, -1, rettv) == FAIL)
	emsg("luaeval: cannot convert value");
    واپس 0;
}

    ساکن صحیح
luaV_setref(lua_State *L)
{
    صحیح copyID = lua_tointeger(L, 1);
    صحیح abort = FALSE;

    lua_pushlightuserdata(L, (خالی *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // the cache table

    luaV_getfield(L, LUAVIM_LIST);
    luaV_getfield(L, LUAVIM_DICT);
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_pushnil(L);
    // traverse cache table
    جب (!abort && lua_اگلا(L, 2) != 0)
    {
	lua_getmetatable(L, -1);
	اگر (lua_rawequal(L, -1, 3)) // list?
	{
	    list_T *l = (list_T *)lua_touserdata(L, 6); //کلید

	    abort = سیٹ_ref_اندر_list(l, copyID);
	}
	ورنہ اگر (lua_rawequal(L, -1, 4)) // dict?
	{
	    dict_T *d = (dict_T *)lua_touserdata(L, 6); //کلید

	    abort = سیٹ_ref_اندر_dict(d, copyID);
	}
	ورنہ اگر (lua_rawequal(L, -1, 5)) // funcref?
	{
	    luaV_Funcref *f = (luaV_Funcref *)lua_touserdata(L, 6); //کلید

	    abort = سیٹ_ref_اندر_dict(f->self, copyID);
	}
	lua_نکالو(L, 2); // metatable and value
    }
    lua_pushinteger(L, abort);
    واپس 1;
}

    ساکن صحیح
luaV_pushversion(lua_State *L)
{
    صحیح major = 0;
    صحیح minor = 0;
    صحیح patch = 0;
    حرف s[16];

    لڑی_سے_پڑھو(LUAVIM_نسخہ, "Lua %d.%d.%d", &major, &minor, &patch);
    vim_snprintf(s, حجم(s), "%d.%d.%d", major, minor, patch);
    lua_pushstring(L, s);
    واپس 0;
}

# define LUA_VIM_FN_رمز \
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

# define LUA_VIM_تیاری_VARIABLE_DICTS \
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

    ساکن صحیح
luaopen_vim(lua_State *L)
{
    lua_newtable(L);  // cache table
    lua_newtable(L);  // cache table's metatable
    lua_pushstring(L, "v");
    lua_setfield(L, -2, "__mode");
    lua_setmetatable(L, -2); // cache is weak-valued
    // put the cache table in the registry for luaV_get/setudata()
    lua_pushlightuserdata(L, (خالی *) LUAVIM_UDATA_CACHE);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
    lua_نکالو(L, 1);  // we don't need the cache table here anymore
    // print
    lua_pushcfunction(L, luaV_print);
    lua_setglobal(L, "print");
    // debug.debug
    lua_getglobal(L, "debug");
    lua_pushcfunction(L, luaV_debug);
    lua_setfield(L, -2, "debug");
    lua_نکالو(L, 1);
    // free
    lua_pushlightuserdata(L, (خالی *) LUAVIM_FREE);
    lua_pushcfunction(L, luaV_free);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // luaeval
    lua_pushlightuserdata(L, (خالی *) LUAVIM_LUAEVAL);
    lua_pushcfunction(L, luaV_luaeval);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // setref
    lua_pushlightuserdata(L, (خالی *) LUAVIM_SETREF);
    lua_pushcfunction(L, luaV_setref);
    lua_rawset(L, LUA_REGISTRYINDEX);
    //رجسٹر
    luaV_newmetatable(L, LUAVIM_LIST);
    lua_V_رجسٹر(L, luaV_List_mt);
    lua_نکالو(L, 1);
    luaV_newmetatable(L, LUAVIM_DICT);
    lua_V_رجسٹر(L, luaV_Dict_mt);
    lua_نکالو(L, 1);
    luaV_newmetatable(L, LUAVIM_BLOB);
    lua_V_رجسٹر(L, luaV_Blob_mt);
    lua_نکالو(L, 1);
    luaV_newmetatable(L, LUAVIM_FUNCREF);
    lua_V_رجسٹر(L, luaV_Funcref_mt);
    lua_نکالو(L, 1);
    luaV_newmetatable(L, LUAVIM_بفر);
    lua_V_رجسٹر(L, lua_V_بفر_mt);
    lua_نکالو(L, 1);
    luaV_newmetatable(L, LUAVIM_کھڑکی);
    lua_V_رجسٹر(L, lua_V_کھڑکی_mt);
    lua_نکالو(L, 1);
    lua_newtable(L); // vim table
    lua_V_رجسٹر(L, luaV_module);
    luaV_pushversion(L);
    lua_setfield(L, -2, "lua_version");
    lua_setglobal(L, LUAVIM_NAME);
    // custom code
    (خالی)luaL_dostring(L, LUA_VIM_FN_رمز);
    (خالی)luaL_dostring(L, LUA_VIM_UPDATE_PACKAGE_PATHS);
    (خالی)luaL_dostring(L, LUA_VIM_تیاری_VARIABLE_DICTS);

    lua_getglobal(L, LUAVIM_NAME);
    lua_getfield(L, -1, "_update_package_paths");

    اگر (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);

    واپس 0;
}

    ساکن lua_State *
luaV_newstate(خالی)
{
    lua_State *L = luaL_newstate();
# اگر LUA_نسخہ_عدد >= 505
    luaL_openselectedlibs(L, ~0, 0); // core libs
# ورنہ
    luaL_openlibs(L); // core libs
# endif
    lua_pushcfunction(L, luaopen_vim); // vim
    lua_call(L, 0, 0);
    واپس L;
}

    ساکن خالی
luaV_setrange(lua_State *L, صحیح line1, صحیح line2)
{
    lua_getglobal(L, LUAVIM_NAME);
    lua_pushinteger(L, line1);
    lua_setfield(L, -2, "firstline");
    lua_pushinteger(L, line2);
    lua_setfield(L, -2, "lastline");
    lua_نکالو(L, 1); // vim table
}


// =======   Interface   =======

ساکن lua_State *L = تہی;

    ساکن صحیح
lua_isopen(خالی)
{
    واپس L != تہی;
}

    ساکن صحیح
lua_init(خالی)
{
    اگر (lua_isopen())
	واپس OK;

# ifdef DYNAMIC_LUA
    اگر (!lua_enabled(TRUE))
    {
	emsg(_("Lua library cannot be loaded."));
	واپس FAIL;
    }
# endif
    L = luaV_newstate();

    واپس OK;
}

    خالی
lua_انتہا(خالی)
{
    اگر (!lua_isopen())
	واپس;

    lua_close(L);
    L = تہی;
}

/*
 * ex commands
 */
    خالی
ex_lua(exarg_T *eap)
{
    حرف *script = (حرف *)script_get(eap, eap->arg);

    اگر (!eap->skip && lua_init() == OK)
    {
	حرف *s = script != تہی ? script : (حرف *)eap->arg;

	luaV_setrange(L, eap->line1, eap->line2);
	اگر (luaL_loadbuffer(L, s, لڑی_لمبائی(s), LUAVIM_CHUNKNAME)
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
    اگر (script != تہی)
	vim_free(script);
}

    خالی
ex_luado(exarg_T *eap)
{
    linenr_T l;
    ثابت حرف *s = (ثابت حرف *) eap->arg;
    lua_L_بفر b;
    size_t len;
    buf_T *was_curbuf = curbuf;

    اگر (lua_init() == FAIL) واپس;
    اگر (u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
    {
	emsg(_("cannot save undo information"));
	واپس;
    }
    luaV_setrange(L, eap->line1, eap->line2);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, "return function(line, linenr) ", 30); //سرخی
    luaL_addlstring(&b, s, لڑی_لمبائی(s));
    luaL_addlstring(&b, " end", 4); // footer
    luaL_pushresult(&b);
    s = lua_tolstring(L, -1, &len);
    اگر (luaL_loadbuffer(L, s, len, LUAVIM_CHUNKNAME))
    {
	luaV_emsg(L);
	lua_نکالو(L, 1); // function body
	واپس;
    }
    lua_call(L, 0, 1);
    lua_replace(L, -2); // function -> body
    برائے (l = eap->line1; l <= eap->line2; l++)
    {
	// Check the line number, the command may have deleted lines.
	اگر (l > curbuf->b_ml.ml_سطر_گنتی)
	    توڑو;

	lua_pushvalue(L, -1); // function
	luaV_pushline(L, curbuf, l); // current line as arg
	lua_pushinteger(L, l); // current line number as arg
	اگر (lua_pcall(L, 2, 1, 0))
	{
	    luaV_emsg(L);
	    توڑو;
	}

	// Catch the command switching to another buffer.
	// Check the line number, the command may have deleted lines.
	اگر (curbuf != was_curbuf || l > curbuf->b_ml.ml_سطر_گنتی)
	    توڑو;

	اگر (lua_isstring(L, -1)) // update line?
	{
# ifdef HAVE_SANDBOX
	    luaV_checksandbox(L);
# endif
	    ml_replace(l, luaV_toline(L, -1), TRUE);
	    بدلا_بائٹیں(l, 0);
	    lua_نکالو(L, 1); // result from luaV_toline
	}
	lua_نکالو(L, 1); //سطر
    }
    lua_نکالو(L, 1); // function
    check_cursor();
    update_screen(UPD_NOT_VALID);
}

    خالی
ex_luafile(exarg_T *eap)
{
    اگر (lua_init() == FAIL)
	واپس;
    اگر (!eap->skip)
    {
	luaV_setrange(L, eap->line1, eap->line2);
	اگر (luaL_loadfile(L, (حرف *) eap->arg) || lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
}

# define luaV_freetype(typ,tname) \
	خالی \
    lua_##tname##_free(typ *o) \
    { \
	اگر (!lua_isopen()) واپس; \
	luaV_getfield(L, LUAVIM_FREE); \
	lua_pushlightuserdata(L, (خالی *) o); \
	lua_call(L, 1, 0); \
    }

luaV_freetype(buf_T, بفر)
luaV_freetype(win_T, کھڑکی_2)

    خالی
do_luaeval(char_u *str, typval_T *arg, typval_T *rettv)
{
    lua_init();
    luaV_getfield(L, LUAVIM_LUAEVAL);
    lua_pushstring(L, (حرف *) str);
    lua_pushlightuserdata(L, (خالی *) arg);
    lua_pushlightuserdata(L, (خالی *) rettv);
    lua_call(L, 3, 0);
}

    صحیح
سیٹ_ref_اندر_lua(صحیح copyID)
{
    صحیح aborted = 0;

    اگر (!lua_isopen())
	واپس 0;

    luaV_getfield(L, LUAVIM_SETREF);
    // call the function with 1 arg, getting 1 result back
    lua_pushinteger(L, copyID);
    lua_call(L, 1, 1);
    // get the result
    aborted = lua_tointeger(L, -1);
    // pop result off the stack
    lua_نکالو(L, 1);

    واپس aborted;
}

    خالی
update_package_paths_اندر_lua(خالی)
{
    اگر (!lua_isopen())
	واپس;

    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "_update_package_paths");

    اگر (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);
}

/*
 * Native C function callback
 */
    ساکن صحیح
luaV_call_lua_func(
	صحیح	 argcount,
	typval_T *argvars,
	typval_T *rettv,
	خالی	 *state)
{
    صحیح i;
    صحیح luaargcount = argcount;
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_funcref);

    اگر (funcstate->lua_tableref != LUA_NOREF)
    {
	// First arg for metatable __call method is a table
	luaargcount += 1;
	lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    }

    برائے (i = 0; i < argcount; ++i)
	luaV_pushtypval(funcstate->L, &argvars[i]);

    اگر (lua_pcall(funcstate->L, luaargcount, 1, 0))
    {
	luaV_emsg(funcstate->L);
	واپس (صحیح)FCERR_OTHER;
    }

    luaV_checktypval(funcstate->L, -1, rettv, "get return value");
    واپس (صحیح)FCERR_NONE;
}

/*
 * Free up any lua references held by the func state.
 */
    ساکن خالی
luaV_call_lua_func_free(خالی *state)
{
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_funcref);
    funcstate->L = تہی;
    اگر (funcstate->lua_tableref != LUA_NOREF)
	luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    VIM_CLEAR(funcstate);
}

#اگر_ختم
