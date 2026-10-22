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

#ضمن "vim.h"
#ضمن "version.h"

#ضمن <lua.h>
#ضمن <lualib.h>
#ضمن <lauxlib.h>

#إذا __STDC_VERSION__ >= 199901L
# define LUAV_INLINE مضمن
#وإلا
# define LUAV_INLINE
#د_که_پای

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#إذا defined(FEAT_LUA)

# define LUAVIM_CHUNKNAME "vim chunk"
# define LUAVIM_NAME "vim"
# define LUAVIM_EVALNAME "luaeval"
# define LUAVIM_EVALHEADER "local _A=select(1,...) return "

# ifdef LUA_RELEASE
#  define LUAVIM_نسخه LUA_RELEASE
# وإلا
#  define LUAVIM_نسخه LUA_نسخه
# endif

تعريفنوع buf_T *lua_V_بفر;
تعريفنوع win_T *lua_V_کړکۍ;
تعريفنوع dict_T *luaV_Dict;
تعريفنوع list_T *luaV_List;
تعريفنوع blob_T *luaV_Blob;
تعريفنوع بنية {
    char_u	*name;	// funcref
    dict_T	*self;	// selfdict
} luaV_Funcref;
تعريفنوع صحيح (*msgfunc_T)(حرف *);

تعريفنوع بنية {
    صحيح lua_funcref;    // ref to a lua func
    صحيح lua_tableref;   // ref to a lua table if metatable else LUA_NOREF. used
			// for __call
    lua_State *L;
} luaV_CFuncState;

ساكن ثابت حرف LUAVIM_DICT[] = "dict";
ساكن ثابت حرف LUAVIM_LIST[] = "list";
ساكن ثابت حرف LUAVIM_BLOB[] = "blob";
ساكن ثابت حرف LUAVIM_FUNCREF[] = "funcref";
ساكن ثابت حرف LUAVIM_بفر[] = "buffer";
ساكن ثابت حرف LUAVIM_کړکۍ[] = "window";
ساكن ثابت حرف LUAVIM_FREE[] = "luaV_free";
ساكن ثابت حرف LUAVIM_LUAEVAL[] = "luaV_luaeval";
ساكن ثابت حرف LUAVIM_SETREF[] = "luaV_setref";

ساكن ثابت حرف LUA___CALL[] = "__call";

// get/setudata manage references to vim userdata in a cache table through
// object pointers (light userdata). The cache table itself is retrieved
// from the registry.

ساكن ثابت حرف LUAVIM_UDATA_CACHE[] = "luaV_udata_cache";

# define luaV_getfield(L, s) \
    lua_pushlightuserdata((L), (فراغ *)(s)); \
    lua_rawget((L), LUA_REGISTRYINDEX)
# define luaV_checksandbox(L) \
    إذا (sandbox) luaL_error((L), "not allowed in sandbox")
# define lua_V_پیغام(L) luaV_msgfunc((L), (msgfunc_T) پیغام)
# define luaV_emsg(L) luaV_msgfunc((L), (msgfunc_T) emsg)
# define luaV_checktypval(L, a, v, پیغام) \
    افعل { \
	إذا (luaV_totypval(L, a, v) == FAIL) \
	    luaL_error(L, پیغام ": cannot convert value"); \
    } طالما (0)

ساكن luaV_List *luaV_pushlist(lua_State *L, list_T *lis);
ساكن luaV_Dict *luaV_pushdict(lua_State *L, dict_T *dic);
ساكن luaV_Blob *luaV_pushblob(lua_State *L, blob_T *blo);
ساكن luaV_Funcref *luaV_pushfuncref(lua_State *L, char_u *name);
ساكن صحيح luaV_call_lua_func(صحيح argcount, typval_T *argvars, typval_T *rettv, فراغ *state);
ساكن فراغ luaV_call_lua_func_free(فراغ *state);

# إذا LUA_نسخه_عدد <= 501
#  define lua_V_ثبتوونکی(L, l) lua_L_ثبتوونکی(L, عدم, l)
#  define luaL_typeerror luaL_typerror
# وإلا
#  define lua_V_ثبتوونکی(L, l) luaL_setfuncs(L, l, 0)
# endif

# ifdef DYNAMIC_LUA

#  ifdef MSWIN
#   define بارول_dll vim_بارول_Lib
#   define نښه_from_dll Get_Proc_پته
#   define close_dll FreeLibrary
#   define بارول_dll_error GetWin32Error
#  وإلا
#   include <dlfcn.h>
#   define HANDLE فراغ*
#   define بارول_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#   define نښه_from_dll dlsym
#   define close_dll dlclose
#   define بارول_dll_error dlerror
#  endif

// lauxlib
#  إذا LUA_نسخه_عدد <= 501
#   define lua_L_ثبتوونکی dll_lua_L_ثبتوونکی
#   define luaL_prepbuffer dll_luaL_prepbuffer
#   define luaL_openlib dll_luaL_openlib
#   define luaL_typerror dll_luaL_typerror
#   define luaL_loadfile dll_luaL_loadfile
#   define luaL_loadbuffer dll_luaL_loadbuffer
#  وإلا
#   define luaL_prepbuffsize dll_luaL_prepbuffsize
#   define luaL_setfuncs dll_luaL_setfuncs
#   define luaL_loadfilex dll_luaL_loadfilex
#   define luaL_loadbufferx dll_luaL_loadbufferx
#   define luaL_argerror dll_luaL_argerror
#  endif
#  إذا LUA_نسخه_عدد >= 504
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
#  إذا LUA_نسخه_عدد <= 501
#   define lua_tonumber dll_lua_tonumber
#   define lua_tointeger dll_lua_tointeger
#   define lua_call dll_lua_call
#   define lua_pcall dll_lua_pcall
#  وإلا
#   define lua_tonumberx dll_lua_tonumberx
#   define lua_tointegerx dll_lua_tointegerx
#   define lua_callk dll_lua_callk
#   define lua_pcallk dll_lua_pcallk
#   define lua_getglobal dll_lua_getglobal
#   define lua_setglobal dll_lua_setglobal
#  endif
#  إذا LUA_نسخه_عدد <= 502
#   define lua_replace dll_lua_replace
#   define lua_remove dll_lua_remove
#  endif
#  إذا LUA_نسخه_عدد >= 503
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
#  إذا LUA_نسخه_عدد >= 504
#   define lua_newuserdatauv dll_lua_newuserdatauv
#  وإلا
#   define lua_newuserdata dll_lua_newuserdata
#  endif
#  define lua_getmetatable dll_lua_getmetatable
#  define lua_setfield dll_lua_setfield
#  define lua_rawset dll_lua_rawset
#  define lua_rawseti dll_lua_rawseti
#  define lua_setmetatable dll_lua_setmetatable
#  define lua_بل dll_lua_بل
// libs
#  define luaopen_بنسټ dll_luaopen_بنسټ
#  define luaopen_جدول dll_luaopen_جدول
#  define luaopen_string dll_luaopen_string
#  define luaopen_math dll_luaopen_math
#  define luaopen_io dll_luaopen_io
#  define luaopen_os dll_luaopen_os
#  define luaopen_package dll_luaopen_package
#  define luaopen_debug dll_luaopen_debug
#  إذا LUA_نسخه_عدد >= 505
#   define luaL_openselectedlibs dll_luaL_openselectedlibs
#  وإلا
#   define luaL_openlibs dll_luaL_openlibs
#  endif

// lauxlib
#  إذا LUA_نسخه_عدد <= 501
فراغ (*dll_lua_L_ثبتوونکی) (lua_State *L, ثابت حرف *libname, ثابت luaL_Reg *l);
حرف *(*dll_luaL_prepbuffer) (lua_L_بفر *B);
فراغ (*dll_luaL_openlib) (lua_State *L, ثابت حرف *libname, ثابت luaL_Reg *l, صحيح nup);
صحيح (*dll_luaL_typerror) (lua_State *L, صحيح narg, ثابت حرف *tname);
صحيح (*dll_luaL_loadfile) (lua_State *L, ثابت حرف *filename);
صحيح (*dll_luaL_loadbuffer) (lua_State *L, ثابت حرف *buff, size_t sz, ثابت حرف *name);
#  وإلا
حرف *(*dll_luaL_prepbuffsize) (lua_L_بفر *B, size_t sz);
فراغ (*dll_luaL_setfuncs) (lua_State *L, ثابت luaL_Reg *l, صحيح nup);
صحيح (*dll_luaL_loadfilex) (lua_State *L, ثابت حرف *filename, ثابت حرف *ډېرپېښ);
صحيح (*dll_luaL_loadbufferx) (lua_State *L, ثابت حرف *buff, size_t sz, ثابت حرف *name, ثابت حرف *ډېرپېښ);
صحيح (*dll_luaL_argerror) (lua_State *L, صحيح numarg, ثابت حرف *extramsg);
#  endif
#  إذا LUA_نسخه_عدد >= 504
صحيح (*dll_luaL_typeerror) (lua_State *L, صحيح narg, ثابت حرف *tname);
#  endif
فراغ (*dll_luaL_checkany) (lua_State *L, صحيح narg);
ثابت حرف *(*dll_luaL_checklstring) (lua_State *L, صحيح عدد_Arg, size_t *l);
lua_Integer (*dll_luaL_checkinteger) (lua_State *L, صحيح عدد_Arg);
lua_Integer (*dll_luaL_optinteger) (lua_State *L, صحيح nArg, lua_Integer def);
فراغ (*dll_luaL_checktype) (lua_State *L, صحيح narg, صحيح t);
صحيح (*dll_luaL_error) (lua_State *L, ثابت حرف *fmt, ...);
lua_State *(*dll_luaL_newstate) (فراغ);
فراغ (*dll_luaL_buffinit) (lua_State *L, lua_L_بفر *B);
فراغ (*dll_luaL_addlstring) (lua_L_بفر *B, ثابت حرف *s, size_t l);
فراغ (*dll_luaL_pushresult) (lua_L_بفر *B);
صحيح (*dll_luaL_loadstring) (lua_State *L, ثابت حرف *s);
صحيح (*dll_luaL_ref) (lua_State *L, صحيح idx);
#  إذا LUA_نسخه_عدد <= 502
فراغ (*dll_luaL_unref) (lua_State *L, صحيح idx, صحيح n);
#  وإلا
فراغ (*dll_luaL_unref) (lua_State *L, صحيح idx, lua_Integer n);
#  endif
// lua
#  إذا LUA_نسخه_عدد <= 501
lua_شمېره (*dll_lua_tonumber) (lua_State *L, صحيح idx);
lua_Integer (*dll_lua_tointeger) (lua_State *L, صحيح idx);
فراغ (*dll_lua_call) (lua_State *L, صحيح nargs, صحيح nresults);
صحيح (*dll_lua_pcall) (lua_State *L, صحيح nargs, صحيح nresults, صحيح errfunc);
#  وإلا
lua_شمېره (*dll_lua_tonumberx) (lua_State *L, صحيح idx, صحيح *isnum);
lua_Integer (*dll_lua_tointegerx) (lua_State *L, صحيح idx, صحيح *isnum);
فراغ (*dll_lua_callk) (lua_State *L, صحيح nargs, صحيح nresults, صحيح ctx,
	lua_CFunction k);
صحيح (*dll_lua_pcallk) (lua_State *L, صحيح nargs, صحيح nresults, صحيح errfunc,
	صحيح ctx, lua_CFunction k);
فراغ (*dll_lua_getglobal) (lua_State *L, ثابت حرف *var);
فراغ (*dll_lua_setglobal) (lua_State *L, ثابت حرف *var);
#  endif
#  إذا LUA_نسخه_عدد <= 502
فراغ (*dll_lua_replace) (lua_State *L, صحيح idx);
فراغ (*dll_lua_remove) (lua_State *L, صحيح idx);
#  endif
#  إذا LUA_نسخه_عدد >= 503
فراغ  (*dll_lua_rotate) (lua_State *L, صحيح idx, صحيح n);
فراغ (*dll_lua_copy) (lua_State *L, صحيح fromidx, صحيح toidx);
#  endif
ثابت حرف *(*dll_lua_typename) (lua_State *L, صحيح tp);
فراغ       (*dll_lua_close) (lua_State *L);
صحيح (*dll_lua_gettop) (lua_State *L);
فراغ (*dll_lua_settop) (lua_State *L, صحيح idx);
فراغ (*dll_lua_pushvalue) (lua_State *L, صحيح idx);
صحيح (*dll_lua_isnumber) (lua_State *L, صحيح idx);
صحيح (*dll_lua_isstring) (lua_State *L, صحيح idx);
صحيح (*dll_lua_type) (lua_State *L, صحيح idx);
صحيح (*dll_lua_rawequal) (lua_State *L, صحيح idx1, صحيح idx2);
صحيح (*dll_lua_toboolean) (lua_State *L, صحيح idx);
ثابت حرف *(*dll_lua_tolstring) (lua_State *L, صحيح idx, size_t *len);
فراغ *(*dll_lua_touserdata) (lua_State *L, صحيح idx);
فراغ (*dll_lua_pushnil) (lua_State *L);
فراغ (*dll_lua_pushnumber) (lua_State *L, lua_شمېره n);
فراغ (*dll_lua_pushinteger) (lua_State *L, lua_Integer n);
فراغ (*dll_lua_pushlstring) (lua_State *L, ثابت حرف *s, size_t l);
فراغ (*dll_lua_pushstring) (lua_State *L, ثابت حرف *s);
ثابت حرف *(*dll_lua_pushfstring) (lua_State *L, ثابت حرف *fmt, ...);
فراغ (*dll_lua_pushcclosure) (lua_State *L, lua_CFunction fn, صحيح n);
فراغ (*dll_lua_pushboolean) (lua_State *L, صحيح b);
فراغ (*dll_lua_pushlightuserdata) (lua_State *L, فراغ *p);
فراغ (*dll_lua_getfield) (lua_State *L, صحيح idx, ثابت حرف *k);
#  إذا LUA_نسخه_عدد <= 502
فراغ (*dll_lua_rawget) (lua_State *L, صحيح idx);
فراغ (*dll_lua_rawgeti) (lua_State *L, صحيح idx, صحيح n);
#  وإلا
صحيح (*dll_lua_rawget) (lua_State *L, صحيح idx);
صحيح (*dll_lua_rawgeti) (lua_State *L, صحيح idx, lua_Integer n);
#  endif
فراغ (*dll_lua_createtable) (lua_State *L, صحيح narr, صحيح nrec);
فراغ (*dll_lua_settable) (lua_State *L, صحيح idx);
#  إذا LUA_نسخه_عدد >= 504
فراغ *(*dll_lua_newuserdatauv) (lua_State *L, size_t sz, صحيح nuvalue);
#  وإلا
فراغ *(*dll_lua_newuserdata) (lua_State *L, size_t sz);
#  endif
صحيح (*dll_lua_getmetatable) (lua_State *L, صحيح objindex);
فراغ (*dll_lua_setfield) (lua_State *L, صحيح idx, ثابت حرف *k);
فراغ (*dll_lua_rawset) (lua_State *L, صحيح idx);
#  إذا LUA_نسخه_عدد <= 502
فراغ (*dll_lua_rawseti) (lua_State *L, صحيح idx, صحيح n);
#  وإلا
فراغ (*dll_lua_rawseti) (lua_State *L, صحيح idx, lua_Integer n);
#  endif
صحيح (*dll_lua_setmetatable) (lua_State *L, صحيح objindex);
صحيح (*dll_lua_بل) (lua_State *L, صحيح idx);
// libs
صحيح (*dll_luaopen_بنسټ) (lua_State *L);
صحيح (*dll_luaopen_جدول) (lua_State *L);
صحيح (*dll_luaopen_string) (lua_State *L);
صحيح (*dll_luaopen_math) (lua_State *L);
صحيح (*dll_luaopen_io) (lua_State *L);
صحيح (*dll_luaopen_os) (lua_State *L);
صحيح (*dll_luaopen_package) (lua_State *L);
صحيح (*dll_luaopen_debug) (lua_State *L);
#  إذا LUA_نسخه_عدد >= 505
فراغ (*dll_luaL_openselectedlibs) (lua_State *L, صحيح بارول, صحيح preload);
#  وإلا
فراغ (*dll_luaL_openlibs) (lua_State *L);
#  endif

تعريفنوع فراغ **luaV_function;
تعريفنوع بنية {
    ثابت حرف *name;
    luaV_function func;
} luaV_Reg;

ساكن ثابت luaV_Reg luaV_dll[] = {
    // lauxlib
#  إذا LUA_نسخه_عدد <= 501
    {"luaL_register", (luaV_function) &dll_lua_L_ثبتوونکی},
    {"luaL_prepbuffer", (luaV_function) &dll_luaL_prepbuffer},
    {"luaL_openlib", (luaV_function) &dll_luaL_openlib},
    {"luaL_typerror", (luaV_function) &dll_luaL_typerror},
    {"luaL_loadfile", (luaV_function) &dll_luaL_loadfile},
    {"luaL_loadbuffer", (luaV_function) &dll_luaL_loadbuffer},
#  وإلا
    {"luaL_prepbuffsize", (luaV_function) &dll_luaL_prepbuffsize},
    {"luaL_setfuncs", (luaV_function) &dll_luaL_setfuncs},
    {"luaL_loadfilex", (luaV_function) &dll_luaL_loadfilex},
    {"luaL_loadbufferx", (luaV_function) &dll_luaL_loadbufferx},
    {"luaL_argerror", (luaV_function) &dll_luaL_argerror},
#  endif
#  إذا LUA_نسخه_عدد >= 504
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
#  إذا LUA_نسخه_عدد <= 501
    {"lua_tonumber", (luaV_function) &dll_lua_tonumber},
    {"lua_tointeger", (luaV_function) &dll_lua_tointeger},
    {"lua_call", (luaV_function) &dll_lua_call},
    {"lua_pcall", (luaV_function) &dll_lua_pcall},
#  وإلا
    {"lua_tonumberx", (luaV_function) &dll_lua_tonumberx},
    {"lua_tointegerx", (luaV_function) &dll_lua_tointegerx},
    {"lua_callk", (luaV_function) &dll_lua_callk},
    {"lua_pcallk", (luaV_function) &dll_lua_pcallk},
    {"lua_getglobal", (luaV_function) &dll_lua_getglobal},
    {"lua_setglobal", (luaV_function) &dll_lua_setglobal},
#  endif
#  إذا LUA_نسخه_عدد <= 502
    {"lua_replace", (luaV_function) &dll_lua_replace},
    {"lua_remove", (luaV_function) &dll_lua_remove},
#  endif
#  إذا LUA_نسخه_عدد >= 503
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
#  إذا LUA_نسخه_عدد >= 504
    {"lua_newuserdatauv", (luaV_function) &dll_lua_newuserdatauv},
#  وإلا
    {"lua_newuserdata", (luaV_function) &dll_lua_newuserdata},
#  endif
    {"lua_getmetatable", (luaV_function) &dll_lua_getmetatable},
    {"lua_setfield", (luaV_function) &dll_lua_setfield},
    {"lua_rawset", (luaV_function) &dll_lua_rawset},
    {"lua_rawseti", (luaV_function) &dll_lua_rawseti},
    {"lua_setmetatable", (luaV_function) &dll_lua_setmetatable},
    {"lua_next", (luaV_function) &dll_lua_بل},
    // libs
    {"luaopen_base", (luaV_function) &dll_luaopen_بنسټ},
    {"luaopen_table", (luaV_function) &dll_luaopen_جدول},
    {"luaopen_string", (luaV_function) &dll_luaopen_string},
    {"luaopen_math", (luaV_function) &dll_luaopen_math},
    {"luaopen_io", (luaV_function) &dll_luaopen_io},
    {"luaopen_os", (luaV_function) &dll_luaopen_os},
    {"luaopen_package", (luaV_function) &dll_luaopen_package},
    {"luaopen_debug", (luaV_function) &dll_luaopen_debug},
#  إذا LUA_نسخه_عدد >= 505
    {"luaL_openselectedlibs", (luaV_function) &dll_luaL_openselectedlibs},
#  وإلا
    {"luaL_openlibs", (luaV_function) &dll_luaL_openlibs},
#  endif
    {عدم, عدم}
};

ساكن HANDLE hinstLua = عدم;

    ساكن صحيح
lua_link_init(حرف *libname, صحيح verbose)
{
    ثابت luaV_Reg *reg;
    إذا (hinstLua) أرجع OK;
    hinstLua = بارول_dll(libname);
    إذا (!hinstLua)
    {
	إذا (verbose)
	    semsg(_(e_could_not_بارول_library_str_str), libname, بارول_dll_error());
	أرجع FAIL;
    }
    لكل (reg = luaV_dll; reg->func; reg++)
    {
	إذا ((*reg->func = نښه_from_dll(hinstLua, reg->name)) == عدم)
	{
	    close_dll(hinstLua);
	    hinstLua = 0;
	    إذا (verbose)
		semsg(_(e_could_not_بارول_library_function_str), reg->name);
	    أرجع FAIL;
	}
    }
    أرجع OK;
}
# endif // DYNAMIC_LUA

# إذا defined(DYNAMIC_LUA)
    صحيح
lua_enabled(صحيح verbose)
{
    أرجع lua_link_init((حرف *)p_luadll, verbose) == OK;
}
# endif

# إذا LUA_نسخه_عدد > 501 && LUA_نسخه_عدد < 504
    ساكن صحيح
luaL_typeerror(lua_State *L, صحيح narg, ثابت حرف *tname)
{
    ثابت حرف *پیغام = lua_pushfstring(L, "%s expected, got %s",
	    tname, luaL_typename(L, narg));
    أرجع luaL_argerror(L, narg, پیغام);
}
# endif

    ساكن LUAV_INLINE فراغ
luaV_getudata(lua_State *L, فراغ *v)
{
    lua_pushlightuserdata(L, (فراغ *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // now the cache table is at the top of the stack
    lua_pushlightuserdata(L, v);
    lua_rawget(L, -2);
    lua_remove(L, -2);  // remove the cache table from the stack
}

    ساكن LUAV_INLINE فراغ
luaV_setudata(lua_State *L, فراغ *v)
{
    lua_pushlightuserdata(L, (فراغ *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // cache table is at -1
    lua_pushlightuserdata(L, v);       // ...now at -2
    lua_pushvalue(L, -3);	       // copy the userdata (cache at -3)
    lua_rawset(L, -3);		       // consumes two stack items
    lua_راوباسه(L, 1);		       // and remove the cache table
}

// =======   Internal   =======

    ساكن فراغ
luaV_newmetatable(lua_State *L, ثابت حرف *tname)
{
    lua_newtable(L);
    lua_pushlightuserdata(L, (فراغ *) tname);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
}

    ساكن فراغ *
luaV_toudata(lua_State *L, صحيح ud, ثابت حرف *tname)
{
    فراغ *p = lua_touserdata(L, ud);

    إذا (p == عدم)
	أرجع عدم;

    // value is userdata
    إذا (lua_getmetatable(L, ud)) // does it have a metatable?
    {
	luaV_getfield(L, tname); // get metatable
	إذا (lua_rawequal(L, -1, -2)) // MTs match?
	{
	    lua_راوباسه(L, 2); // MTs
	    أرجع p;
	}
    }

    أرجع عدم;
}

    ساكن فراغ *
luaV_checkcache(lua_State *L, فراغ *p)
{
    luaV_getudata(L, p);
    إذا (lua_isnil(L, -1)) luaL_error(L, "invalid object");
    lua_راوباسه(L, 1);
    أرجع p;
}

# define luaV_unbox(L,luatyp,ud) (*((luatyp *) lua_touserdata((L),(ud))))

# define luaV_checkvalid(L,luatyp,ud) \
    luaV_checkcache((L), (فراغ *) luaV_unbox((L),luatyp,(ud)))

    ساكن فراغ *
luaV_checkudata(lua_State *L, صحيح ud, ثابت حرف *tname)
{
    فراغ *p = luaV_toudata(L, ud, tname);
    إذا (p == عدم) luaL_typeerror(L, ud, tname);
    أرجع p;
}

    ساكن فراغ
luaV_pushtypval(lua_State *L, typval_T *tv)
{
    إذا (tv == عدم)
    {
	lua_pushnil(L);
	أرجع;
    }
    بدل (tv->v_type)
    {
	حالة VAR_STRING:
	    lua_pushstring(L, tv->vval.v_string == عدم
					    ? "" : (حرف *)tv->vval.v_string);
	    اكسر;
	حالة VAR_شمېره:
	    lua_pushinteger(L, (صحيح) tv->vval.v_شمېره);
	    اكسر;
	حالة VAR_FLOAT:
	    lua_pushnumber(L, (lua_شمېره) tv->vval.v_float);
	    اكسر;
	حالة VAR_LIST:
	    luaV_pushlist(L, tv->vval.v_list);
	    اكسر;
	حالة VAR_DICT:
	    luaV_pushdict(L, tv->vval.v_dict);
	    اكسر;
	حالة VAR_BOOL:
	حالة VAR_SPECIAL:
	    إذا (tv->vval.v_شمېره <= VVAL_TRUE)
		lua_pushboolean(L, (صحيح) tv->vval.v_شمېره);
	    وإلا
		lua_pushnil(L);
	    اكسر;
	حالة VAR_FUNC:
	    luaV_pushfuncref(L, tv->vval.v_string);
	    اكسر;
	حالة VAR_PARTIAL:
	    // TODO: handle partial arguments
	    luaV_pushfuncref(L, partial_name(tv->vval.v_partial));
	    اكسر;

	حالة VAR_BLOB:
	    luaV_pushblob(L, tv->vval.v_blob);
	    اكسر;
	افتراضي:
	    lua_pushnil(L);
    }
}

/*
 * Converts lua value at 'pos' to typval 'tv'.
 * Returns OK or FAIL.
 */
    ساكن صحيح
luaV_totypval(lua_State *L, صحيح pos, typval_T *tv)
{
    صحيح حالت = OK;

    tv->v_قلف = 0;

    بدل (lua_type(L, pos))
    {
	حالة LUA_TBOOLEAN:
	    tv->v_type = VAR_BOOL;
	    tv->vval.v_شمېره = (varnumber_T) lua_toboolean(L, pos);
	    اكسر;
	حالة LUA_TNIL:
	    tv->v_type = VAR_SPECIAL;
	    tv->vval.v_شمېره = VVAL_NULL;
	    اكسر;
	حالة LUA_TSTRING:
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strsave((char_u *) lua_tostring(L, pos));
	    اكسر;
	حالة LUA_TNUMBER:
	{
	    ثابت lua_شمېره n = lua_tonumber(L, pos);

	    إذا (n > (lua_شمېره)INT64_MAX || n < (lua_شمېره)INT64_MIN
		    || ((lua_شمېره)((varnumber_T)n)) != n)
	    {
		tv->v_type = VAR_FLOAT;
		tv->vval.v_float = (float_T)n;
	    }
	    وإلا
	    {
		tv->v_type = VAR_شمېره;
		tv->vval.v_شمېره = (varnumber_T)n;
	    }
	}
	    اكسر;
	حالة LUA_TFUNCTION:
	{
	    char_u *name;
	    luaV_CFuncState *state;

	    lua_pushvalue(L, pos);
	    state = ALLOC_CLEAR_ONE(luaV_CFuncState);
	    state->lua_funcref = luaL_ref(L, LUA_REGISTRYINDEX);
	    state->L = L;
	    state->lua_tableref = LUA_NOREF;
	    name = ثبتوونکی_cfunc(&luaV_call_lua_func,
					      &luaV_call_lua_func_free, state);
	    tv->v_type = VAR_FUNC;
	    tv->vval.v_string = vim_strsave(name);
	    اكسر;
	}
	حالة LUA_TTABLE:
	{
	    صحيح lua_tableref;

	    lua_pushvalue(L, pos);
	    lua_tableref = luaL_ref(L, LUA_REGISTRYINDEX);
	    إذا (lua_getmetatable(L, pos))
	    {
		lua_getfield(L, -1, LUA___CALL);
		إذا (lua_isfunction(L, -1))
		{
		    char_u *name;
		    صحيح lua_funcref = luaL_ref(L, LUA_REGISTRYINDEX);
		    luaV_CFuncState *state = ALLOC_CLEAR_ONE(luaV_CFuncState);

		    state->lua_funcref = lua_funcref;
		    state->L = L;
		    state->lua_tableref = lua_tableref;
		    name = ثبتوونکی_cfunc(&luaV_call_lua_func,
					      &luaV_call_lua_func_free, state);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(name);
		    اكسر;
		}
	    }
	    tv->v_type = VAR_شمېره;
	    tv->vval.v_شمېره = 0;
	    حالت = FAIL;
	    اكسر;
	}
	حالة LUA_TUSERDATA:
	{
	    فراغ *p = lua_touserdata(L, pos);

	    إذا (lua_getmetatable(L, pos)) // has metatable?
	    {
		// check list
		luaV_getfield(L, LUAVIM_LIST);
		إذا (lua_rawequal(L, -1, -2))
		{
		    tv->v_type = VAR_LIST;
		    tv->vval.v_list = *((luaV_List *) p);
		    ++tv->vval.v_list->lv_refcount;
		    lua_راوباسه(L, 2); // MTs
		    اكسر;
		}
		// check dict
		luaV_getfield(L, LUAVIM_DICT);
		إذا (lua_rawequal(L, -1, -3))
		{
		    tv->v_type = VAR_DICT;
		    tv->vval.v_dict = *((luaV_Dict *) p);
		    ++tv->vval.v_dict->dv_refcount;
		    lua_راوباسه(L, 3); // MTs
		    اكسر;
		}
		// check blob
		luaV_getfield(L, LUAVIM_BLOB);
		إذا (lua_rawequal(L, -1, -4))
		{
		    tv->v_type = VAR_BLOB;
		    tv->vval.v_blob = *((luaV_Blob *) p);
		    ++tv->vval.v_blob->bv_refcount;
		    lua_راوباسه(L, 4); // MTs
		    اكسر;
		}
		// check funcref
		luaV_getfield(L, LUAVIM_FUNCREF);
		إذا (lua_rawequal(L, -1, -5))
		{
		    luaV_Funcref *f = (luaV_Funcref *) p;

		    func_ref(f->name);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(f->name);
		    lua_راوباسه(L, 5); // MTs
		    اكسر;
		}
		lua_راوباسه(L, 4); // MTs
	    }
	}
	// FALLTHROUGH
	افتراضي:
	    tv->v_type = VAR_شمېره;
	    tv->vval.v_شمېره = 0;
	    حالت = FAIL;
    }
    أرجع حالت;
}

/*
 * similar to luaL_addlstring, but replaces \0 with \n if toline and
 * \n with \0 otherwise
 */
    ساكن فراغ
luaV_addlstring(lua_L_بفر *b, ثابت حرف *s, size_t l, صحيح toline)
{
    طالما (l--)
    {
	إذا (*s == '\0' && toline)
	    luaL_addchar(b, '\n');
	وإلا إذا (*s == '\n' && !toline)
	    luaL_addchar(b, '\0');
	وإلا
	    luaL_addchar(b, *s);
	s++;
    }
}

    ساكن فراغ
luaV_pushline(lua_State *L, buf_T *buf, linenr_T n)
{
    ثابت حرف *s = (ثابت حرف *) ml_get_buf(buf, n, FALSE);
    lua_L_بفر b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, مزي_اوږدوالی(s), 0);
    luaL_pushresult(&b);
}

    ساكن char_u *
luaV_toline(lua_State *L, صحيح pos)
{
    size_t l;
    ثابت حرف *s = lua_tolstring(L, pos, &l);

    lua_L_بفر b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 1);
    luaL_pushresult(&b);
    أرجع (char_u *) lua_tostring(L, -1);
}

/*
 * pops a string s from the top of the stack and calls mf(t) for pieces t of
 * s separated by newlines
 */
    ساكن فراغ
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
    طالما (l--)
    {
	إذا (*p++ == '\0') // break?
	{
	    mf((حرف *)s);
	    s = p;
	}
    }
    mf((حرف *)s);
    lua_راوباسه(L, 2); // original and modified strings
}

# define luaV_newtype(typ,tname,luatyp,luatname) \
	ساكن luatyp * \
    luaV_new##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = (luatyp *) lua_newuserdata(L, حجم(luatyp)); \
	*o = obj; \
	luaV_setudata(L, obj); /* cache[obj] = udata */ \
	luaV_getfield(L, luatname); \
	lua_setmetatable(L, -2); \
	أرجع o; \
    }

# define luaV_pushtype(typ,tname,luatyp) \
	ساكن luatyp * \
    lua_V_ټېل##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = عدم; \
	إذا (obj == عدم) \
	    lua_pushnil(L); \
	وإلا \
	{ \
	    luaV_getudata(L, obj); \
	    إذا (lua_isnil(L, -1)) /* not interned? */ \
	    { \
		lua_راوباسه(L, 1); \
		o = luaV_new##tname(L, obj); \
	    } \
	    وإلا \
		o = (luatyp *) lua_touserdata(L, -1); \
	} \
	أرجع o; \
    }

# define luaV_type_tostring(tname,luatname) \
	ساكن صحيح \
    luaV_##tname##_tostring(lua_State *L) \
    { \
	lua_pushfstring(L, "%s: %p", luatname, lua_touserdata(L, 1)); \
	أرجع 1; \
    }

// =======   List type   =======

    ساكن luaV_List *
luaV_newlist(lua_State *L, list_T *lis)
{
    luaV_List *l = (luaV_List *) lua_newuserdata(L, حجم(luaV_List));
    *l = lis;
    lis->lv_refcount++; // reference in Lua
    luaV_setudata(L, lis); // cache[lis] = udata
    luaV_getfield(L, LUAVIM_LIST);
    lua_setmetatable(L, -2);
    أرجع l;
}

luaV_pushtype(list_T, list, luaV_List)
luaV_type_tostring(list, LUAVIM_LIST)

    ساكن صحيح
luaV_list_len(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushinteger(L, (صحيح) list_len(l));
    أرجع 1;
}

    ساكن صحيح
luaV_list_iter(lua_State *L)
{
    listitem_T *li = (listitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    إذا (li == عدم) أرجع 0;
    luaV_pushtypval(L, &li->li_tv);
    lua_pushlightuserdata(L, (فراغ *) li->li_بل);
    lua_replace(L, lua_upvalueindex(1));
    أرجع 1;
}

    ساكن صحيح
luaV_list_call(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushlightuserdata(L, (فراغ *) l->lv_لومړی);
    lua_pushcclosure(L, luaV_list_iter, 1);
    أرجع 1;
}

    ساكن صحيح
lua_V_list_شاخص(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    إذا (lua_isnumber(L, 2)) // list item?
    {
	اوږد n = (اوږد) luaL_checkinteger(L, 2);
	listitem_T *li;

	// Lua array index starts with 1 while Vim uses 0, subtract 1 to
	// normalize.
	n -= 1;
	li = list_پیدا_کړه(l, n);
	إذا (li == عدم)
	    lua_pushnil(L);
	وإلا
	    luaV_pushtypval(L, &li->li_tv);
    }
    وإلا إذا (lua_isstring(L, 2)) // method?
    {
	ثابت حرف *s = lua_tostring(L, 2);
	إذا (مزي_پرتله(s, "add", 3) == 0
		|| مزي_پرتله(s, "insert", 6) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	وإلا
	    lua_pushnil(L);
    }
    وإلا
	lua_pushnil(L);
    أرجع 1;
}

    ساكن صحيح
luaV_list_newindex(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    اوږد n = (اوږد) luaL_checkinteger(L, 2);
    listitem_T *li;

    // Lua array index starts with 1 while Vim uses 0, subtract 1 to normalize.
    n -= 1;

    إذا (l->lv_قلف)
	luaL_error(L, "list is locked");
    li = list_پیدا_کړه(l, n);
    إذا (li == عدم)
    {
	إذا (!lua_isnil(L, 3))
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "inserting list item");
	    إذا (list_ورننباسه_tv(l, &v, li) == FAIL)
		luaL_error(L, "failed to add item to list");
	    clear_tv(&v);
	}
    }
    وإلا
    {
	إذا (lua_isnil(L, 3)) // remove?
	{
	    vimlist_remove(l, li, li);
	    listitem_free(l, li);
	}
	وإلا
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "setting list item");
	    clear_tv(&li->li_tv);
	    li->li_tv = v;
	}
    }
    أرجع 0;
}

    ساكن صحيح
lua_V_list_جمع_کړه(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (فراغ *) *lis);
    typval_T v;
    إذا (l->lv_قلف)
	luaL_error(L, "list is locked");
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "adding list item");
    إذا (list_append_tv(l, &v) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    أرجع 1;
}

    ساكن صحيح
lua_V_list_ورننباسه(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (فراغ *) *lis);
    اوږد pos = (اوږد) luaL_optinteger(L, 3, 0);
    listitem_T *li = عدم;
    typval_T v;
    إذا (l->lv_قلف)
	luaL_error(L, "list is locked");
    إذا (pos < l->lv_len)
    {
	li = list_پیدا_کړه(l, pos);
	إذا (li == عدم)
	    luaL_error(L, "invalid position");
    }
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "inserting list item");
    إذا (list_ورننباسه_tv(l, &v, li) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    أرجع 1;
}

ساكن ثابت luaL_Reg luaV_List_mt[] = {
    {"__tostring", luaV_list_tostring},
    {"__len", luaV_list_len},
    {"__call", luaV_list_call},
    {"__index", lua_V_list_شاخص},
    {"__newindex", luaV_list_newindex},
    {"add", lua_V_list_جمع_کړه},
    {"insert", lua_V_list_ورننباسه},
    {عدم, عدم}
};


// =======   Dict type   =======

    ساكن luaV_Dict *
luaV_newdict(lua_State *L, dict_T *dic)
{
    luaV_Dict *d = (luaV_Dict *) lua_newuserdata(L, حجم(luaV_Dict));
    *d = dic;
    dic->dv_refcount++; // reference in Lua
    luaV_setudata(L, dic); // cache[dic] = udata
    luaV_getfield(L, LUAVIM_DICT);
    lua_setmetatable(L, -2);
    أرجع d;
}

luaV_pushtype(dict_T, dict, luaV_Dict)
luaV_type_tostring(dict, LUAVIM_DICT)

    ساكن صحيح
luaV_dict_len(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    lua_pushinteger(L, (صحيح) dict_len(d));
    أرجع 1;
}

    ساكن صحيح
luaV_dict_iter(lua_State *L ناکارول_شوی)
{
# ifdef FEAT_EVAL
    hashitem_T *hi = (hashitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    صحيح n = lua_tointeger(L, lua_upvalueindex(2));
    dictitem_T *di;
    إذا (n <= 0) أرجع 0;
    طالما (HASHITEM_تشه(hi)) hi++;
    di = dict_lookup(hi);
    lua_pushstring(L, (حرف *) hi->hi_کلی);
    luaV_pushtypval(L, &di->di_tv);
    lua_pushlightuserdata(L, (فراغ *) (hi + 1));
    lua_replace(L, lua_upvalueindex(1));
    lua_pushinteger(L, n - 1);
    lua_replace(L, lua_upvalueindex(2));
    أرجع 2;
# وإلا
    أرجع 0;
# endif
}

    ساكن صحيح
luaV_dict_call(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    hashtab_T *ht = &d->dv_hashtab;
    lua_pushlightuserdata(L, (فراغ *) ht->ht_array);
    lua_pushinteger(L, ht->ht_کارول_شوی); // # remaining items
    lua_pushcclosure(L, luaV_dict_iter, 2);
    أرجع 1;
}

    ساكن صحيح
lua_V_dict_شاخص(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *کلی = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di = dict_پیدا_کړه(d, کلی, -1);

    إذا (di == عدم)
    {
	lua_pushnil(L);
	أرجع 1;
    }

    luaV_pushtypval(L, &di->di_tv);
    إذا (di->di_tv.v_type == VAR_FUNC) // funcref?
    {
	luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, -1);
	f->self = d; // keep "self" reference
	d->dv_refcount++;
    }

    أرجع 1;
}

    ساكن صحيح
luaV_dict_newindex(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *کلی = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di;
    typval_T tv;

    إذا (d->dv_قلف)
	luaL_error(L, "dict is locked");
    إذا (کلی == عدم)
	أرجع 0;
    إذا (*کلی == NUL)
	luaL_error(L, "empty key");
    إذا (!lua_isnil(L, 3)) // read value?
    {
	luaV_checktypval(L, 3, &tv, "setting dict item");
	إذا (d->dv_scope == VAR_DEF_SCOPE && tv.v_type == VAR_FUNC)
	{
	    clear_tv(&tv);
	    luaL_error(L, "cannot assign funcref to builtin scope");
	}
    }
    di = dict_پیدا_کړه(d, کلی, -1);
    إذا (di == عدم) // non-existing key?
    {
	إذا (lua_isnil(L, 3))
	    أرجع 0;
	di = dictitem_alloc(کلی);
	إذا (di == عدم)
	{
	    clear_tv(&tv);
	    أرجع 0;
	}
	إذا (dict_جمع_کړه(d, di) == FAIL)
	{
	    vim_free(di);
	    clear_tv(&tv);
	    أرجع 0;
	}
    }
    وإلا
	clear_tv(&di->di_tv);
    إذا (lua_isnil(L, 3)) // remove?
    {
	hashitem_T *hi = hash_پیدا_کړه(&d->dv_hashtab, di->di_کلی);
	hash_remove(&d->dv_hashtab, hi, "Lua new index");
	dictitem_free(di);
    }
    وإلا
	di->di_tv = tv;
    أرجع 0;
}

ساكن ثابت luaL_Reg luaV_Dict_mt[] = {
    {"__tostring", luaV_dict_tostring},
    {"__len", luaV_dict_len},
    {"__call", luaV_dict_call},
    {"__index", lua_V_dict_شاخص},
    {"__newindex", luaV_dict_newindex},
    {عدم, عدم}
};


// =======   Blob type   =======

    ساكن luaV_Blob *
luaV_newblob(lua_State *L, blob_T *blo)
{
    luaV_Blob *b = (luaV_Blob *) lua_newuserdata(L, حجم(luaV_Blob));
    *b = blo;
    blo->bv_refcount++; // reference in Lua
    luaV_setudata(L, blo); // cache[blo] = udata
    luaV_getfield(L, LUAVIM_BLOB);
    lua_setmetatable(L, -2);
    أرجع b;
}

luaV_pushtype(blob_T, blob, luaV_Blob)
luaV_type_tostring(blob, LUAVIM_BLOB)

    ساكن صحيح
luaV_blob_gc(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    blob_unref(b);
    أرجع 0;
}

    ساكن صحيح
luaV_blob_len(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    lua_pushinteger(L, (صحيح) blob_len(b));
    أرجع 1;
}

    ساكن صحيح
lua_V_blob_شاخص(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    إذا (lua_isnumber(L, 2))
    {
	صحيح idx = luaL_checkinteger(L, 2);
	إذا (idx < 0)
	    // Negative index is relative to the end.
	    idx += (صحيح) blob_len(b);
	إذا (idx >= 0 && idx < blob_len(b))
	    lua_pushnumber(L, (lua_شمېره) blob_get(b, idx));
	وإلا
	    lua_pushnil(L);
    }
    وإلا إذا (lua_isstring(L, 2))
    {
	ثابت حرف *s = lua_tostring(L, 2);
	إذا (مزي_پرتله(s, "add", 3) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	وإلا
	    lua_pushnil(L);
    }
    وإلا
	lua_pushnil(L);
    أرجع 1;
}

    ساكن صحيح
luaV_blob_newindex(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    إذا (b->bv_قلف)
	luaL_error(L, "blob is locked");

    إذا (!lua_isnumber(L, 2))
	أرجع 0;

    اوږد len = blob_len(b);
    صحيح idx = luaL_checkinteger(L, 2);
    صحيح val = luaL_checkinteger(L, 3);
    إذا (idx < 0)
	// Negative index is relative to the end.
	idx += (صحيح) len;
    إذا ((idx >= 0 && idx < len)
	    || (idx == len && ga_grow(&b->bv_ga, 1) == OK))
    {
	blob_سېټ(b, idx, (char_u) val);
	إذا (idx == len)
	    ++b->bv_ga.ga_len;
    }
    وإلا
	luaL_error(L, "index out of range");

    أرجع 0;
}

    ساكن صحيح
lua_V_blob_جمع_کړه(lua_State *L)
{
    luaV_Blob *blo = luaV_checkudata(L, 1, LUAVIM_BLOB);
    blob_T *b = (blob_T *) luaV_checkcache(L, (فراغ *) *blo);
    إذا (b->bv_قلف)
	luaL_error(L, "blob is locked");
    lua_settop(L, 2);
    إذا (!lua_isstring(L, 2))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 2));
    وإلا
    {
	size_t i, l = 0;
	ثابت حرف *s = lua_tolstring(L, 2, &l);

	إذا (ga_grow(&b->bv_ga, (صحيح)l) == OK)
	    لكل (i = 0; i < l; ++i)
		ga_append(&b->bv_ga, s[i]);
    }
    lua_settop(L, 1);
    أرجع 1;
}

ساكن ثابت luaL_Reg luaV_Blob_mt[] = {
    {"__tostring", luaV_blob_tostring},
    {"__gc", luaV_blob_gc},
    {"__len", luaV_blob_len},
    {"__index", lua_V_blob_شاخص},
    {"__newindex", luaV_blob_newindex},
    {"add", lua_V_blob_جمع_کړه},
    {عدم, عدم}
};


// =======   Funcref type   =======

    ساكن luaV_Funcref *
luaV_newfuncref(lua_State *L, char_u *name)
{
    luaV_Funcref *f = (luaV_Funcref *)lua_newuserdata(L, حجم(luaV_Funcref));

    إذا (name != عدم)
    {
	func_ref(name);
	f->name = vim_strsave(name);
    }
    f->self = عدم;
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_setmetatable(L, -2);
    أرجع f;
}

    ساكن luaV_Funcref *
luaV_pushfuncref(lua_State *L, char_u *name)
{
    أرجع luaV_newfuncref(L, name);
}


luaV_type_tostring(funcref, LUAVIM_FUNCREF)

    ساكن صحيح
luaV_funcref_gc(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    func_unref(f->name);
    vim_free(f->name);
    // NOTE: Don't call "dict_unref(f->self)", because the dict of "f->self"
    // will be (or has been already) freed by Vim's garbage collection.
    أرجع 0;
}

// equivalent to string(funcref)
    ساكن صحيح
luaV_funcref_len(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    lua_pushstring(L, (ثابت حرف *) f->name);
    أرجع 1;
}

    ساكن صحيح
luaV_funcref_call(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);
    صحيح i, n = lua_gettop(L) - 1; // #args
    صحيح حالت = FAIL;
    typval_T دلیلونه;
    typval_T rettv;

    دلیلونه.v_type = VAR_LIST;
    دلیلونه.vval.v_list = list_alloc();
    rettv.v_type = VAR_UNKNOWN; // as in clear_tv
    إذا (دلیلونه.vval.v_list != عدم)
    {
	typval_T v;

	لكل (i = 0; i < n; i++)
	{
	    luaV_checktypval(L, i + 2, &v, "calling funcref");
	    list_append_tv(دلیلونه.vval.v_list, &v);
	    clear_tv(&v);
	}
	حالت = func_call(f->name, &دلیلونه, عدم, f->self, &rettv);
	إذا (حالت == OK)
	    luaV_pushtypval(L, &rettv);
	clear_tv(&دلیلونه);
	clear_tv(&rettv);
    }
    إذا (حالت != OK)
	luaL_error(L, "cannot call funcref");
    أرجع 1;
}

ساكن ثابت luaL_Reg luaV_Funcref_mt[] = {
    {"__tostring", luaV_funcref_tostring},
    {"__gc", luaV_funcref_gc},
    {"__len", luaV_funcref_len},
    {"__call", luaV_funcref_call},
    {عدم, عدم}
};


// =======   Buffer type   =======

luaV_newtype(buf_T, بفر, lua_V_بفر, LUAVIM_بفر)
luaV_pushtype(buf_T, بفر, lua_V_بفر)
luaV_type_tostring(بفر, LUAVIM_بفر)

    ساكن صحيح
lua_V_بفر_len(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, lua_V_بفر, 1);
    lua_pushinteger(L, b->b_ml.ml_کرښه_شمېر);
    أرجع 1;
}

    ساكن صحيح
lua_V_بفر_call(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, lua_V_بفر, 1);
    lua_settop(L, 1);
    سېټ_curbuf(b, DOBUF_وېشل);
    أرجع 1;
}

    ساكن صحيح
lua_V_بفر_شاخص(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, lua_V_بفر, 1);
    linenr_T n = (linenr_T) lua_tointeger(L, 2);
    إذا (n > 0 && n <= b->b_ml.ml_کرښه_شمېر)
	luaV_pushline(L, b, n);
    وإلا إذا (lua_isstring(L, 2))
    {
	ثابت حرف *s = lua_tostring(L, 2);
	إذا (مزي_پرتله(s, "name", 4) == 0)
	    lua_pushstring(L, (b->b_sfname == عدم)
					? "" : (حرف *) b->b_sfname);
	وإلا إذا (مزي_پرتله(s, "fname", 5) == 0)
	    lua_pushstring(L, (b->b_ffname == عدم)
					? "" : (حرف *) b->b_ffname);
	وإلا إذا (مزي_پرتله(s, "number", 6) == 0)
	    lua_pushinteger(L, b->b_fnum);
	// methods
	وإلا إذا (مزي_پرتله(s,   "insert", 6) == 0
		|| مزي_پرتله(s, "next", 4) == 0
		|| مزي_پرتله(s, "previous", 8) == 0
		|| مزي_پرتله(s, "isvalid", 7) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	وإلا
	    lua_pushnil(L);
    }
    وإلا
	lua_pushnil(L);
    أرجع 1;
}

    ساكن صحيح
lua_V_بفر_newindex(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, lua_V_بفر, 1);
    linenr_T n = (linenr_T) luaL_checkinteger(L, 2);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    إذا (n < 1 || n > b->b_ml.ml_کرښه_شمېر)
	luaL_error(L, "invalid line number");
    إذا (lua_isnil(L, 3)) // delete line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	إذا (u_savedel(n, 1L) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	وإلا إذا (ml_delete(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot delete line");
	}
	وإلا
	{
	    deleted_کرښې_mark(n, 1L);
	    إذا (b == curwin->w_بفر) // fix cursor in current window?
	    {
		إذا (curwin->w_cursor.lnum >= n)
		{
		    إذا (curwin->w_cursor.lnum > n)
		    {
			curwin->w_cursor.lnum -= 1;
			check_cursor_ستون();
		    }
		    وإلا
			check_cursor();
		    بدل_شوی_cline_bef_curs();
		}
		invalidate_botline();
	    }
	}
	curbuf = buf;
    }
    وإلا إذا (lua_isstring(L, 3)) // update line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	إذا (u_savesub(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	وإلا إذا (ml_replace(n, luaV_toline(L, 3), TRUE) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot replace line");
	}
	وإلا
	    بدل_شوی_بایټونه(n, 0);
	curbuf = buf;
	إذا (b == curwin->w_بفر)
	    check_cursor_ستون();
    }
    وإلا
	luaL_error(L, "wrong argument to change line");
    أرجع 0;
}

    ساكن صحيح
lua_V_بفر_ورننباسه(lua_State *L)
{
    lua_V_بفر *lb = luaV_checkudata(L, 1, LUAVIM_بفر);
    buf_T *b = (buf_T *) luaV_checkcache(L, (فراغ *) *lb);
    linenr_T وروستی = b->b_ml.ml_کرښه_شمېر;
    linenr_T n = (linenr_T) luaL_optinteger(L, 3, وروستی);
    buf_T *buf;
    luaL_checktype(L, 2, LUA_TSTRING);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    // fix insertion line
    إذا (n < 0) n = 0;
    إذا (n > وروستی) n = وروستی;
    // insert
    buf = curbuf;
    curbuf = b;
    إذا (u_save(n, n + 1) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot save undo information");
    }
    وإلا إذا (ml_append(n, luaV_toline(L, 2), 0, FALSE) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot insert line");
    }
    وإلا
	appended_کرښې_mark(n, 1L);
    curbuf = buf;
    update_screen(UPD_VALID);
    أرجع 0;
}

    ساكن صحيح
lua_V_بفر_بل(lua_State *L)
{
    lua_V_بفر *b = luaV_checkudata(L, 1, LUAVIM_بفر);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (فراغ *) *b);
    luaV_pushbuffer(L, buf->b_بل);
    أرجع 1;
}

    ساكن صحيح
lua_V_بفر_پخوانی(lua_State *L)
{
    lua_V_بفر *b = luaV_checkudata(L, 1, LUAVIM_بفر);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (فراغ *) *b);
    luaV_pushbuffer(L, buf->b_prev);
    أرجع 1;
}

    ساكن صحيح
lua_V_بفر_isvalid(lua_State *L)
{
    lua_V_بفر *b = luaV_checkudata(L, 1, LUAVIM_بفر);
    luaV_getudata(L, *b);
    lua_pushboolean(L, !lua_isnil(L, -1));
    أرجع 1;
}

ساكن ثابت luaL_Reg lua_V_بفر_mt[] = {
    {"__tostring", lua_V_بفر_tostring},
    {"__len", lua_V_بفر_len},
    {"__call", lua_V_بفر_call},
    {"__index", lua_V_بفر_شاخص},
    {"__newindex", lua_V_بفر_newindex},
    {"insert", lua_V_بفر_ورننباسه},
    {"next", lua_V_بفر_بل},
    {"previous", lua_V_بفر_پخوانی},
    {"isvalid", lua_V_بفر_isvalid},
    {عدم, عدم}
};


// =======   Window type   =======

luaV_newtype(win_T, کړکۍ_2, lua_V_کړکۍ, LUAVIM_کړکۍ)
luaV_pushtype(win_T, کړکۍ_2, lua_V_کړکۍ)
luaV_type_tostring(کړکۍ_2, LUAVIM_کړکۍ)

    ساكن صحيح
lua_V_کړکۍ_call(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, lua_V_کړکۍ, 1);
    lua_settop(L, 1);
    win_goto(w);
    أرجع 1;
}

    ساكن صحيح
lua_V_کړکۍ_شاخص(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, lua_V_کړکۍ, 1);
    ثابت حرف *s = luaL_checkstring(L, 2);
    إذا (مزي_پرتله(s, "buffer", 6) == 0)
	luaV_pushbuffer(L, w->w_بفر);
    وإلا إذا (مزي_پرتله(s, "line", 4) == 0)
	lua_pushinteger(L, w->w_cursor.lnum);
    وإلا إذا (مزي_پرتله(s, "col", 3) == 0)
	lua_pushinteger(L, w->w_cursor.ستون + 1);
    وإلا إذا (مزي_پرتله(s, "width", 5) == 0)
	lua_pushinteger(L, w->w_پلنوالی);
    وإلا إذا (مزي_پرتله(s, "height", 6) == 0)
	lua_pushinteger(L, w->w_لوړوالی);
    // methods
    وإلا إذا (مزي_پرتله(s,   "next", 4) == 0
	    || مزي_پرتله(s, "previous", 8) == 0
	    || مزي_پرتله(s, "isvalid", 7) == 0)
    {
	lua_getmetatable(L, 1);
	lua_getfield(L, -1, s);
    }
    وإلا
	lua_pushnil(L);
    أرجع 1;
}

    ساكن صحيح
lua_V_کړکۍ_newindex(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, lua_V_کړکۍ, 1);
    ثابت حرف *s = luaL_checkstring(L, 2);
    صحيح v = luaL_checkinteger(L, 3);
    إذا (مزي_پرتله(s, "line", 4) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	إذا (v < 1 || v > w->w_بفر->b_ml.ml_کرښه_شمېر)
	    luaL_error(L, "line out of range");
	w->w_cursor.lnum = v;
	update_screen(UPD_VALID);
    }
    وإلا إذا (مزي_پرتله(s, "col", 3) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	w->w_cursor.ستون = v - 1;
	w->w_سېټ_curswant = true;
	update_screen(UPD_VALID);
    }
    وإلا إذا (مزي_پرتله(s, "width", 5) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setwidth(v);
	curwin = win;
    }
    وإلا إذا (مزي_پرتله(s, "height", 6) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setheight(v);
	curwin = win;
    }
    وإلا
	luaL_error(L, "invalid window property: `%s'", s);
    أرجع 0;
}

    ساكن صحيح
lua_V_کړکۍ_بل(lua_State *L)
{
    lua_V_کړکۍ *w = luaV_checkudata(L, 1, LUAVIM_کړکۍ);
    win_T *win = (win_T *) luaV_checkcache(L, (فراغ *) *w);
    luaV_pushwindow(L, win->w_بل);
    أرجع 1;
}

    ساكن صحيح
lua_V_کړکۍ_پخوانی(lua_State *L)
{
    lua_V_کړکۍ *w = luaV_checkudata(L, 1, LUAVIM_کړکۍ);
    win_T *win = (win_T *) luaV_checkcache(L, (فراغ *) *w);
    luaV_pushwindow(L, win->w_prev);
    أرجع 1;
}

    ساكن صحيح
lua_V_کړکۍ_isvalid(lua_State *L)
{
    lua_V_کړکۍ *w = luaV_checkudata(L, 1, LUAVIM_کړکۍ);
    luaV_getudata(L, *w);
    lua_pushboolean(L, !lua_isnil(L, -1));
    أرجع 1;
}

ساكن ثابت luaL_Reg lua_V_کړکۍ_mt[] = {
    {"__tostring", lua_V_کړکۍ_tostring},
    {"__call", lua_V_کړکۍ_call},
    {"__index", lua_V_کړکۍ_شاخص},
    {"__newindex", lua_V_کړکۍ_newindex},
    {"next", lua_V_کړکۍ_بل},
    {"previous", lua_V_کړکۍ_پخوانی},
    {"isvalid", lua_V_کړکۍ_isvalid},
    {عدم, عدم}
};


// =======   Vim module   =======

    ساكن صحيح
luaV_print(lua_State *L)
{
    صحيح		i, n = lua_gettop(L); // nargs
    ثابت حرف	*s;
    size_t	l;
    garray_T	پیغام_ga;

    ga_init2(&پیغام_ga, 1, 128);
    lua_getglobal(L, "tostring");
    لكل (i = 1; i <= n; i++)
    {
	lua_pushvalue(L, -1); // tostring
	lua_pushvalue(L, i); // arg
	lua_call(L, 1, 1);
	s = lua_tolstring(L, -1, &l);
	إذا (s == عدم)
	    أرجع luaL_error(L, "cannot convert to string");
	إذا (i > 1)
	    ga_append(&پیغام_ga, ' '); // use space instead of tab
	ga_concat_len(&پیغام_ga, (char_u *)s, l);
	lua_راوباسه(L, 1);
    }
    // Replace any "\n" with "\0"
    لكل (i = 0; i < پیغام_ga.ga_len; i++)
	إذا (((حرف *)پیغام_ga.ga_معلومات)[i] == '\n')
	    ((حرف *)پیغام_ga.ga_معلومات)[i] = '\0';
    lua_pushlstring(L, پیغام_ga.ga_معلومات, پیغام_ga.ga_len);
    إذا (!got_int)
	lua_V_پیغام(L);

    ga_clear(&پیغام_ga);
    أرجع 0;
}

    ساكن صحيح
luaV_debug(lua_State *L)
{
    lua_settop(L, 0);
    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "eval");
    lua_remove(L, -2); // vim.eval at position 1
    لكل (;;)
    {
	ثابت حرف *ننوت;
	size_t l;
	lua_pushvalue(L, 1); // vim.eval
	lua_pushliteral(L, "input('lua_debug> ')");
	lua_call(L, 1, 1); // return string
	ننوت = lua_tolstring(L, -1, &l);
	إذا (l == 0 || strcmp(ننوت, "cont") == 0)
	    أرجع 0;
	پیغام_putchar('\n'); // avoid outputting on input line
	إذا (luaL_loadbuffer(L, ننوت, l, "=(debug command)")
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
	lua_settop(L, 1); // remove eventual returns, but keep vim.eval
    }
}

    ساكن dict_T *
luaV_get_var_scope(lua_State *L)
{
    ثابت حرف	*scope = luaL_checkstring(L, 1);
    dict_T	*dict = عدم;

    إذا (STRICMP((حرف *)scope, "g") == 0)
	dict = get_globvar_dict();
    وإلا إذا (STRICMP((حرف *)scope, "v") == 0)
	dict = get_vimvar_dict();
    وإلا إذا (STRICMP((حرف *)scope, "b") == 0)
	dict = curbuf->b_vars;
    وإلا إذا (STRICMP((حرف *)scope, "w") == 0)
	dict = curwin->w_vars;
    وإلا إذا (STRICMP((حرف *)scope, "t") == 0)
	dict = curtab->tp_vars;
    وإلا
    {
	luaL_error(L, "invalid scope %s", scope);
	أرجع عدم;
    }

    أرجع dict;
}

    ساكن صحيح
luaV_setvar(lua_State *L)
{
    dict_T	*dict;
    dictitem_T	*di;
    size_t	len;
    حرف	*name;
    صحيح		del;
    حرف	*error = عدم;

    name = (حرف *)luaL_checklstring(L, 3, &len);
    del = (lua_gettop(L) < 4) || lua_isnil(L, 4);

    dict = luaV_get_var_scope(L);
    إذا (dict == عدم)
	أرجع 0;

    di = dict_پیدا_کړه(dict, (char_u *)name, (صحيح)len);
    إذا (di != عدم)
    {
	إذا (di->di_flags & DI_FLAGS_RO)
	    error = "variable is read-only";
	وإلا إذا (di->di_flags & DI_FLAGS_قلف)
	    error = "variable is locked";
	وإلا إذا (del && di->di_flags & DI_FLAGS_FIX)
	    error = "variable is fixed";
	إذا (error != عدم)
	    أرجع luaL_error(L, error);
    }
    وإلا إذا (dict->dv_قلف)
	أرجع luaL_error(L, "Dictionary is locked");

    إذا (del)
    {
	// Delete the key
	إذا (di == عدم)
	    // Doesn't exist, nothing to do
	    أرجع 0;
	// Delete the entry
	dictitem_remove(dict, di, "Lua delete variable");
    }
    وإلا
    {
	// Update the key
	typval_T	tv;

	// Convert the lua value to a Vim script type in the temporary variable
	lua_pushvalue(L, 4);
	إذا (luaV_totypval(L, -1, &tv) == FAIL)
	    أرجع luaL_error(L, "Couldn't convert lua value");

	إذا (di == عدم)
	{
	    // Need to create an entry
	    di = dictitem_alloc((char_u *)name);
	    إذا (di == عدم)
	    {
		clear_tv(&tv);
		أرجع 0;
	    }
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	    إذا (dict_جمع_کړه(dict, di) == FAIL)
	    {
		dictitem_free(di);
		clear_tv(&tv);
		أرجع luaL_error(L, "Couldn't add to dictionary");
	    }
	}
	وإلا
	{
	    صحيح type_error = FALSE;
	    إذا (dict == get_vimvar_dict()
	       && !before_سېټ_vvar((char_u *)name, di, &tv, TRUE, &type_error))
	    {
		clear_tv(&tv);
		إذا (type_error)
		    أرجع luaL_error(L,
				"Setting v:%s to value with wrong type", name);
		أرجع 0;
	    }
	    // Clear the old value
	    clear_tv(&di->di_tv);
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	}

	// Clear the temporary variable
	clear_tv(&tv);
    }

    أرجع 0;
}

    ساكن صحيح
luaV_getvar(lua_State *L)
{
    dict_T	*dict = luaV_get_var_scope(L);
    size_t	len;
    ثابت حرف	*name = luaL_checklstring(L, 3, &len);
    dictitem_T	*di = dict_پیدا_کړه(dict, (char_u *)name, (صحيح)len);

    إذا (di == عدم)
	أرجع 0;  // nil

    luaV_pushtypval(L, &di->di_tv);
    أرجع 1;
}

    ساكن صحيح
luaV_command(lua_State *L)
{
    char_u  *s = vim_strsave((char_u *)luaL_checkstring(L, 1));

    execute_cmds_from_string(s);
    vim_free(s);
    update_screen(UPD_VALID);
    أرجع 0;
}

    ساكن صحيح
luaV_eval(lua_State *L)
{
    typval_T *tv = eval_بیان((char_u *) luaL_checkstring(L, 1), عدم);

    إذا (tv == عدم) luaL_error(L, "invalid expression");
    luaV_pushtypval(L, tv);
    free_tv(tv);
    أرجع 1;
}

    ساكن صحيح
luaV_beep(lua_State *L ناکارول_شوی)
{
    vim_beep(BO_LANG);
    أرجع 0;
}

    ساكن صحيح
lua_V_کرښه(lua_State *L)
{
    luaV_pushline(L, curbuf, curwin->w_cursor.lnum);
    أرجع 1;
}

    ساكن صحيح
luaV_list(lua_State *L)
{
    list_T *l;
    صحيح initarg = !lua_isnoneornil(L, 1);

    إذا (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    l = list_alloc();
    إذا (l == عدم)
    {
	lua_pushnil(L);
	أرجع 1;
    }

    luaV_newlist(L, l);
    إذا (!initarg)
	أرجع 1;

    // traverse table to init list
    صحيح notnil, i = 0;
    typval_T v;
    افعل
    {
	lua_rawgeti(L, 1, ++i);
	notnil = !lua_isnil(L, -1);
	إذا (notnil)
	{
	    luaV_checktypval(L, -1, &v, "vim.list");
	    list_append_tv(l, &v);
	    clear_tv(&v);
	}
	lua_راوباسه(L, 1); //ارزښت
    } طالما (notnil);

    أرجع 1;
}

    ساكن صحيح
luaV_dict(lua_State *L)
{
    dict_T *d;
    صحيح initarg = !lua_isnoneornil(L, 1);

    إذا (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    d = dict_alloc();
    إذا (d == عدم)
    {
	lua_pushnil(L);
	أرجع 1;
    }

    luaV_newdict(L, d);
    إذا (!initarg)
	أرجع 1;

    // traverse table to init dict
    lua_pushnil(L);
    طالما (lua_بل(L, 1))
    {
	char_u *کلی;
	dictitem_T *di;
	typval_T v;

	lua_pushvalue(L, -2); // dup key in case it's a number
	کلی = (char_u *) lua_tostring(L, -1);
	إذا (کلی == عدم)
	{
	    lua_pushnil(L);
	    أرجع 1;
	}
	إذا (*کلی == NUL)
	    luaL_error(L, "table has empty key");
	luaV_checktypval(L, -2, &v, "vim.dict"); //ارزښت
	di = dictitem_alloc(کلی);
	إذا (di == عدم || dict_جمع_کړه(d, di) == FAIL)
	{
	    vim_free(di);
	    lua_pushnil(L);
	    أرجع 1;
	}
	di->di_tv = v;
	lua_راوباسه(L, 2); // key copy and value
    }

    أرجع 1;
}

    ساكن صحيح
luaV_blob(lua_State *L)
{
    blob_T *b;
    صحيح initarg = !lua_isnoneornil(L, 1);

    إذا (initarg && !lua_isstring(L, 1))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 1));

    b = blob_alloc();
    إذا (b == عدم)
    {
	lua_pushnil(L);
	أرجع 1;
    }

    luaV_newblob(L, b);
    إذا (!initarg)
	أرجع 1;

    // traverse table to init blob
    size_t i, l = 0;
    ثابت حرف *s = lua_tolstring(L, 1, &l);

    إذا (ga_grow(&b->bv_ga, (صحيح)l) == OK)
	لكل (i = 0; i < l; ++i)
	    ga_append(&b->bv_ga, s[i]);

    أرجع 1;
}

    ساكن صحيح
luaV_funcref(lua_State *L)
{
    ثابت حرف *name = luaL_checkstring(L, 1);
    // note: not checking if function exists (needs function_exists)
    إذا (name == عدم || *name == NUL || VIM_ISDIGIT(*name))
	luaL_error(L, "invalid function name: %s", name);
    luaV_newfuncref(L, (char_u *) name);
    أرجع 1;
}

    ساكن صحيح
lua_V_بفر_2(lua_State *L)
{
    buf_T *buf;
    إذا (lua_isstring(L, 1)) // get by number or name?
    {
	إذا (lua_isnumber(L, 1)) // by number?
	{
	    صحيح n = lua_tointeger(L, 1);
	    FOR_ALL_BUFFERS(buf)
		إذا (buf->b_fnum == n) اكسر;
	}
	وإلا // by name
	{
	    size_t l;
	    ثابت حرف *s = lua_tolstring(L, 1, &l);
	    FOR_ALL_BUFFERS(buf)
	    {
		إذا (buf->b_ffname == عدم || buf->b_sfname == عدم)
		{
		    إذا (l == 0) اكسر;
		}
		وإلا إذا (مزي_پرتله(s, (حرف *)buf->b_ffname, l) == 0
			|| مزي_پرتله(s, (حرف *)buf->b_sfname, l) == 0)
		    اكسر;
	    }
	}
    }
    وإلا
	buf = (lua_toboolean(L, 1)) ? firstbuf : curbuf; // first buffer?
    luaV_pushbuffer(L, buf);
    أرجع 1;
}

    ساكن صحيح
lua_V_کړکۍ_2(lua_State *L)
{
    win_T *win;
    إذا (lua_isnumber(L, 1)) // get by number?
    {
	صحيح n = lua_tointeger(L, 1);
	لكل (win = firstwin; win != عدم; win = win->w_بل, n--)
	    إذا (n == 1) اكسر;
    }
    وإلا
	win = (lua_toboolean(L, 1)) ? firstwin : curwin; // first window?
    luaV_pushwindow(L, win);
    أرجع 1;
}

    ساكن صحيح
luaV_open(lua_State *L)
{
    char_u *s = عدم;
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    إذا (lua_isstring(L, 1)) s = (char_u *) lua_tostring(L, 1);
    luaV_pushbuffer(L, buflist_new(s, عدم, 1L, BLN_LISTED));
    أرجع 1;
}

    ساكن صحيح
luaV_type(lua_State *L)
{
    luaL_checkany(L, 1);
    إذا (lua_type(L, 1) == LUA_TUSERDATA) // check vim udata?
    {
	lua_settop(L, 1);
	إذا (lua_getmetatable(L, 1))
	{
	    luaV_getfield(L, LUAVIM_LIST);
	    إذا (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "list");
		أرجع 1;
	    }
	    luaV_getfield(L, LUAVIM_DICT);
	    إذا (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "dict");
		أرجع 1;
	    }
	    luaV_getfield(L, LUAVIM_BLOB);
	    إذا (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "blob");
		أرجع 1;
	    }
	    luaV_getfield(L, LUAVIM_FUNCREF);
	    إذا (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "funcref");
		أرجع 1;
	    }
	    luaV_getfield(L, LUAVIM_بفر);
	    إذا (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "buffer");
		أرجع 1;
	    }
	    luaV_getfield(L, LUAVIM_کړکۍ);
	    إذا (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "window");
		أرجع 1;
	    }
	}
    }
    lua_pushstring(L, luaL_typename(L, 1)); // fallback
    أرجع 1;
}

    ساكن صحيح
luaV_call(lua_State *L)
{
    صحيح		argc = lua_gettop(L) - 1;
    size_t	funcname_len;
    char_u	*funcname;
    حرف	*error = عدم;
    typval_T	rettv;
    typval_T	argv[MAX_FUNC_دلیلونه + 1];
    صحيح		i = 0;

    إذا (argc > MAX_FUNC_دلیلونه)
	أرجع luaL_error(L, "Function called with too many arguments");

    funcname = (char_u *)luaL_checklstring(L, 1, &funcname_len);

    لكل (; i < argc; i++)
    {
	إذا (luaV_totypval(L, i + 2, &argv[i]) == FAIL)
	{
	    error = "lua: cannot convert value";
	    اذهب free_vim_دلیلونه;
	}
    }

    argv[argc].v_type = VAR_UNKNOWN;

    إذا (call_vim_function(funcname, argc, argv, &rettv) == FAIL)
    {
	error = "lua: call_vim_function failed";
	اذهب free_vim_دلیلونه;
    }

    luaV_pushtypval(L, &rettv);
    clear_tv(&rettv);

free_vim_دلیلونه:
    طالما (i > 0)
	clear_tv(&argv[--i]);

    إذا (error == عدم)
	أرجع 1;
    وإلا
	أرجع luaL_error(L, error);
}

/*
 * Return the Vim version as a Lua table
 */
    ساكن صحيح
lua_V_نسخه(lua_State *L)
{
    lua_newtable(L);
    lua_pushstring(L, "major");
    lua_pushinteger(L, VIM_نسخه_MAJOR);
    lua_settable(L, -3);
    lua_pushstring(L, "minor");
    lua_pushinteger(L, VIM_نسخه_MINOR);
    lua_settable(L, -3);
    lua_pushstring(L, "patch");
    lua_pushinteger(L, highest_patch());
    lua_settable(L, -3);
    أرجع 1;
}

ساكن ثابت luaL_Reg luaV_module[] = {
    {"command", luaV_command},
    {"eval", luaV_eval},
    {"beep", luaV_beep},
    {"line", lua_V_کرښه},
    {"list", luaV_list},
    {"dict", luaV_dict},
    {"blob", luaV_blob},
    {"funcref", luaV_funcref},
    {"buffer", lua_V_بفر_2},
    {"window", lua_V_کړکۍ_2},
    {"open", luaV_open},
    {"type", luaV_type},
    {"call", luaV_call},
    {"_getvar", luaV_getvar},
    {"_setvar", luaV_setvar},
    {"version", lua_V_نسخه},
    {"lua_version", عدم},
    {عدم, عدم}
};

/*
 * for freeing list, dict, buffer and window objects; lightuserdata as arg
 */
    ساكن صحيح
luaV_free(lua_State *L)
{
    lua_pushnil(L);
    luaV_setudata(L, lua_touserdata(L, 1));
    أرجع 0;
}

    ساكن صحيح
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
    إذا (luaL_loadbuffer(L, str, l, LUAVIM_EVALNAME)) // compile error?
    {
	luaV_emsg(L);
	أرجع 0;
    }
    luaV_pushtypval(L, arg);
    إذا (lua_pcall(L, 1, 1, 0)) // running error?
    {
	luaV_emsg(L);
	أرجع 0;
    }
    إذا (luaV_totypval(L, -1, rettv) == FAIL)
	emsg("luaeval: cannot convert value");
    أرجع 0;
}

    ساكن صحيح
luaV_setref(lua_State *L)
{
    صحيح copyID = lua_tointeger(L, 1);
    صحيح abort = FALSE;

    lua_pushlightuserdata(L, (فراغ *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // the cache table

    luaV_getfield(L, LUAVIM_LIST);
    luaV_getfield(L, LUAVIM_DICT);
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_pushnil(L);
    // traverse cache table
    طالما (!abort && lua_بل(L, 2) != 0)
    {
	lua_getmetatable(L, -1);
	إذا (lua_rawequal(L, -1, 3)) // list?
	{
	    list_T *l = (list_T *)lua_touserdata(L, 6); //کلی

	    abort = سېټ_ref_دننه_list(l, copyID);
	}
	وإلا إذا (lua_rawequal(L, -1, 4)) // dict?
	{
	    dict_T *d = (dict_T *)lua_touserdata(L, 6); //کلی

	    abort = سېټ_ref_دننه_dict(d, copyID);
	}
	وإلا إذا (lua_rawequal(L, -1, 5)) // funcref?
	{
	    luaV_Funcref *f = (luaV_Funcref *)lua_touserdata(L, 6); //کلی

	    abort = سېټ_ref_دننه_dict(f->self, copyID);
	}
	lua_راوباسه(L, 2); // metatable and value
    }
    lua_pushinteger(L, abort);
    أرجع 1;
}

    ساكن صحيح
luaV_pushversion(lua_State *L)
{
    صحيح major = 0;
    صحيح minor = 0;
    صحيح patch = 0;
    حرف s[16];

    له_مزي_ولوله(LUAVIM_نسخه, "Lua %d.%d.%d", &major, &minor, &patch);
    vim_snprintf(s, حجم(s), "%d.%d.%d", major, minor, patch);
    lua_pushstring(L, s);
    أرجع 0;
}

# define LUA_VIM_FN_کوډ \
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

    ساكن صحيح
luaopen_vim(lua_State *L)
{
    lua_newtable(L);  // cache table
    lua_newtable(L);  // cache table's metatable
    lua_pushstring(L, "v");
    lua_setfield(L, -2, "__mode");
    lua_setmetatable(L, -2); // cache is weak-valued
    // put the cache table in the registry for luaV_get/setudata()
    lua_pushlightuserdata(L, (فراغ *) LUAVIM_UDATA_CACHE);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
    lua_راوباسه(L, 1);  // we don't need the cache table here anymore
    // print
    lua_pushcfunction(L, luaV_print);
    lua_setglobal(L, "print");
    // debug.debug
    lua_getglobal(L, "debug");
    lua_pushcfunction(L, luaV_debug);
    lua_setfield(L, -2, "debug");
    lua_راوباسه(L, 1);
    // free
    lua_pushlightuserdata(L, (فراغ *) LUAVIM_FREE);
    lua_pushcfunction(L, luaV_free);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // luaeval
    lua_pushlightuserdata(L, (فراغ *) LUAVIM_LUAEVAL);
    lua_pushcfunction(L, luaV_luaeval);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // setref
    lua_pushlightuserdata(L, (فراغ *) LUAVIM_SETREF);
    lua_pushcfunction(L, luaV_setref);
    lua_rawset(L, LUA_REGISTRYINDEX);
    //ثبتوونکی
    luaV_newmetatable(L, LUAVIM_LIST);
    lua_V_ثبتوونکی(L, luaV_List_mt);
    lua_راوباسه(L, 1);
    luaV_newmetatable(L, LUAVIM_DICT);
    lua_V_ثبتوونکی(L, luaV_Dict_mt);
    lua_راوباسه(L, 1);
    luaV_newmetatable(L, LUAVIM_BLOB);
    lua_V_ثبتوونکی(L, luaV_Blob_mt);
    lua_راوباسه(L, 1);
    luaV_newmetatable(L, LUAVIM_FUNCREF);
    lua_V_ثبتوونکی(L, luaV_Funcref_mt);
    lua_راوباسه(L, 1);
    luaV_newmetatable(L, LUAVIM_بفر);
    lua_V_ثبتوونکی(L, lua_V_بفر_mt);
    lua_راوباسه(L, 1);
    luaV_newmetatable(L, LUAVIM_کړکۍ);
    lua_V_ثبتوونکی(L, lua_V_کړکۍ_mt);
    lua_راوباسه(L, 1);
    lua_newtable(L); // vim table
    lua_V_ثبتوونکی(L, luaV_module);
    luaV_pushversion(L);
    lua_setfield(L, -2, "lua_version");
    lua_setglobal(L, LUAVIM_NAME);
    // custom code
    (فراغ)luaL_dostring(L, LUA_VIM_FN_کوډ);
    (فراغ)luaL_dostring(L, LUA_VIM_UPDATE_PACKAGE_PATHS);
    (فراغ)luaL_dostring(L, LUA_VIM_تیاری_VARIABLE_DICTS);

    lua_getglobal(L, LUAVIM_NAME);
    lua_getfield(L, -1, "_update_package_paths");

    إذا (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);

    أرجع 0;
}

    ساكن lua_State *
luaV_newstate(فراغ)
{
    lua_State *L = luaL_newstate();
# إذا LUA_نسخه_عدد >= 505
    luaL_openselectedlibs(L, ~0, 0); // core libs
# وإلا
    luaL_openlibs(L); // core libs
# endif
    lua_pushcfunction(L, luaopen_vim); // vim
    lua_call(L, 0, 0);
    أرجع L;
}

    ساكن فراغ
luaV_setrange(lua_State *L, صحيح line1, صحيح line2)
{
    lua_getglobal(L, LUAVIM_NAME);
    lua_pushinteger(L, line1);
    lua_setfield(L, -2, "firstline");
    lua_pushinteger(L, line2);
    lua_setfield(L, -2, "lastline");
    lua_راوباسه(L, 1); // vim table
}


// =======   Interface   =======

ساكن lua_State *L = عدم;

    ساكن صحيح
lua_isopen(فراغ)
{
    أرجع L != عدم;
}

    ساكن صحيح
lua_init(فراغ)
{
    إذا (lua_isopen())
	أرجع OK;

# ifdef DYNAMIC_LUA
    إذا (!lua_enabled(TRUE))
    {
	emsg(_("Lua library cannot be loaded."));
	أرجع FAIL;
    }
# endif
    L = luaV_newstate();

    أرجع OK;
}

    فراغ
lua_پای_ټکی(فراغ)
{
    إذا (!lua_isopen())
	أرجع;

    lua_close(L);
    L = عدم;
}

/*
 * ex commands
 */
    فراغ
ex_lua(exarg_T *eap)
{
    حرف *script = (حرف *)script_get(eap, eap->arg);

    إذا (!eap->skip && lua_init() == OK)
    {
	حرف *s = script != عدم ? script : (حرف *)eap->arg;

	luaV_setrange(L, eap->line1, eap->line2);
	إذا (luaL_loadbuffer(L, s, مزي_اوږدوالی(s), LUAVIM_CHUNKNAME)
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
    إذا (script != عدم)
	vim_free(script);
}

    فراغ
ex_luado(exarg_T *eap)
{
    linenr_T l;
    ثابت حرف *s = (ثابت حرف *) eap->arg;
    lua_L_بفر b;
    size_t len;
    buf_T *was_curbuf = curbuf;

    إذا (lua_init() == FAIL) أرجع;
    إذا (u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
    {
	emsg(_("cannot save undo information"));
	أرجع;
    }
    luaV_setrange(L, eap->line1, eap->line2);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, "return function(line, linenr) ", 30); //سرلیک
    luaL_addlstring(&b, s, مزي_اوږدوالی(s));
    luaL_addlstring(&b, " end", 4); // footer
    luaL_pushresult(&b);
    s = lua_tolstring(L, -1, &len);
    إذا (luaL_loadbuffer(L, s, len, LUAVIM_CHUNKNAME))
    {
	luaV_emsg(L);
	lua_راوباسه(L, 1); // function body
	أرجع;
    }
    lua_call(L, 0, 1);
    lua_replace(L, -2); // function -> body
    لكل (l = eap->line1; l <= eap->line2; l++)
    {
	// Check the line number, the command may have deleted lines.
	إذا (l > curbuf->b_ml.ml_کرښه_شمېر)
	    اكسر;

	lua_pushvalue(L, -1); // function
	luaV_pushline(L, curbuf, l); // current line as arg
	lua_pushinteger(L, l); // current line number as arg
	إذا (lua_pcall(L, 2, 1, 0))
	{
	    luaV_emsg(L);
	    اكسر;
	}

	// Catch the command switching to another buffer.
	// Check the line number, the command may have deleted lines.
	إذا (curbuf != was_curbuf || l > curbuf->b_ml.ml_کرښه_شمېر)
	    اكسر;

	إذا (lua_isstring(L, -1)) // update line?
	{
# ifdef HAVE_SANDBOX
	    luaV_checksandbox(L);
# endif
	    ml_replace(l, luaV_toline(L, -1), TRUE);
	    بدل_شوی_بایټونه(l, 0);
	    lua_راوباسه(L, 1); // result from luaV_toline
	}
	lua_راوباسه(L, 1); //کرښه
    }
    lua_راوباسه(L, 1); // function
    check_cursor();
    update_screen(UPD_NOT_VALID);
}

    فراغ
ex_luafile(exarg_T *eap)
{
    إذا (lua_init() == FAIL)
	أرجع;
    إذا (!eap->skip)
    {
	luaV_setrange(L, eap->line1, eap->line2);
	إذا (luaL_loadfile(L, (حرف *) eap->arg) || lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
}

# define luaV_freetype(typ,tname) \
	فراغ \
    lua_##tname##_free(typ *o) \
    { \
	إذا (!lua_isopen()) أرجع; \
	luaV_getfield(L, LUAVIM_FREE); \
	lua_pushlightuserdata(L, (فراغ *) o); \
	lua_call(L, 1, 0); \
    }

luaV_freetype(buf_T, بفر)
luaV_freetype(win_T, کړکۍ_2)

    فراغ
do_luaeval(char_u *str, typval_T *arg, typval_T *rettv)
{
    lua_init();
    luaV_getfield(L, LUAVIM_LUAEVAL);
    lua_pushstring(L, (حرف *) str);
    lua_pushlightuserdata(L, (فراغ *) arg);
    lua_pushlightuserdata(L, (فراغ *) rettv);
    lua_call(L, 3, 0);
}

    صحيح
سېټ_ref_دننه_lua(صحيح copyID)
{
    صحيح aborted = 0;

    إذا (!lua_isopen())
	أرجع 0;

    luaV_getfield(L, LUAVIM_SETREF);
    // call the function with 1 arg, getting 1 result back
    lua_pushinteger(L, copyID);
    lua_call(L, 1, 1);
    // get the result
    aborted = lua_tointeger(L, -1);
    // pop result off the stack
    lua_راوباسه(L, 1);

    أرجع aborted;
}

    فراغ
update_package_paths_دننه_lua(فراغ)
{
    إذا (!lua_isopen())
	أرجع;

    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "_update_package_paths");

    إذا (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);
}

/*
 * Native C function callback
 */
    ساكن صحيح
luaV_call_lua_func(
	صحيح	 argcount,
	typval_T *argvars,
	typval_T *rettv,
	فراغ	 *state)
{
    صحيح i;
    صحيح luaargcount = argcount;
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_funcref);

    إذا (funcstate->lua_tableref != LUA_NOREF)
    {
	// First arg for metatable __call method is a table
	luaargcount += 1;
	lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    }

    لكل (i = 0; i < argcount; ++i)
	luaV_pushtypval(funcstate->L, &argvars[i]);

    إذا (lua_pcall(funcstate->L, luaargcount, 1, 0))
    {
	luaV_emsg(funcstate->L);
	أرجع (صحيح)FCERR_OTHER;
    }

    luaV_checktypval(funcstate->L, -1, rettv, "get return value");
    أرجع (صحيح)FCERR_NONE;
}

/*
 * Free up any lua references held by the func state.
 */
    ساكن فراغ
luaV_call_lua_func_free(فراغ *state)
{
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_funcref);
    funcstate->L = عدم;
    إذا (funcstate->lua_tableref != LUA_NOREF)
	luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    VIM_CLEAR(funcstate);
}

#د_که_پای
