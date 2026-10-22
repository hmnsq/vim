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
# define LUAV_INLINE درونخطی
#وگرنه
# define LUAV_INLINE
#endif

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#اگر defined(FEAT_LUA)

# define LUAVIM_CHUNKNAME "vim chunk"
# define LUAVIM_NAME "vim"
# define LUAVIM_EVALNAME "luaeval"
# define LUAVIM_EVALHEADER "local _A=select(1,...) return "

# ifdef LUA_RELEASE
#  define LUAVIM_VERSION LUA_RELEASE
# وگرنه
#  define LUAVIM_VERSION LUA_VERSION
# endif

تعریفنوع buf_T *luaV_Buffer;
تعریفنوع win_T *luaV_Window;
تعریفنوع dict_T *luaV_Dict;
تعریفنوع list_T *luaV_List;
تعریفنوع blob_T *luaV_Blob;
تعریفنوع ساختار {
    char_u	*name;	// funcref
    dict_T	*self;	// selfdict
} luaV_Funcref;
تعریفنوع صحیح (*msgfunc_T)(نویسه *);

تعریفنوع ساختار {
    صحیح lua_funcref;    // ref to a lua func
    صحیح lua_tableref;   // ref to a lua table if metatable else LUA_NOREF. used
			// for __call
    lua_State *L;
} luaV_CFuncState;

ایستا ثابت نویسه LUAVIM_DICT[] = "dict";
ایستا ثابت نویسه LUAVIM_LIST[] = "list";
ایستا ثابت نویسه LUAVIM_BLOB[] = "blob";
ایستا ثابت نویسه LUAVIM_FUNCREF[] = "funcref";
ایستا ثابت نویسه LUAVIM_BUFFER[] = "buffer";
ایستا ثابت نویسه LUAVIM_WINDOW[] = "window";
ایستا ثابت نویسه LUAVIM_FREE[] = "luaV_free";
ایستا ثابت نویسه LUAVIM_LUAEVAL[] = "luaV_luaeval";
ایستا ثابت نویسه LUAVIM_SETREF[] = "luaV_setref";

ایستا ثابت نویسه LUA___CALL[] = "__call";

// get/setudata manage references to vim userdata in a cache table through
// object pointers (light userdata). The cache table itself is retrieved
// from the registry.

ایستا ثابت نویسه LUAVIM_UDATA_CACHE[] = "luaV_udata_cache";

# define luaV_getfield(L, s) \
    lua_pushlightuserdata((L), (تهی *)(s)); \
    lua_rawget((L), LUA_REGISTRYINDEX)
# define luaV_checksandbox(L) \
    اگر (sandbox) luaL_error((L), "not allowed in sandbox")
# define lua_V_پیام(L) luaV_msgfunc((L), (msgfunc_T) پیام)
# define luaV_emsg(L) luaV_msgfunc((L), (msgfunc_T) emsg)
# define luaV_checktypval(L, a, v, پیام) \
    انجامبده { \
	اگر (luaV_totypval(L, a, v) == FAIL) \
	    luaL_error(L, پیام ": cannot convert value"); \
    } وقتی (0)

ایستا luaV_List *luaV_pushlist(lua_State *L, list_T *lis);
ایستا luaV_Dict *luaV_pushdict(lua_State *L, dict_T *dic);
ایستا luaV_Blob *luaV_pushblob(lua_State *L, blob_T *blo);
ایستا luaV_Funcref *luaV_pushfuncref(lua_State *L, char_u *name);
ایستا صحیح luaV_call_lua_func(صحیح argcount, typval_T *argvars, typval_T *rettv, تهی *state);
ایستا تهی luaV_call_lua_func_free(تهی *state);

# اگر LUA_VERSION_رقم <= 501
#  define luaV_register(L, l) luaL_register(L, پوچ, l)
#  define luaL_typeerror luaL_typerror
# وگرنه
#  define luaV_register(L, l) luaL_setfuncs(L, l, 0)
# endif

# ifdef DYNAMIC_LUA

#  ifdef MSWIN
#   define load_dll vimLoadLib
#   define نماد_from_dll GetProcAddress
#   define close_dll FreeLibrary
#   define load_dll_error GetWin32Error
#  وگرنه
#   include <dlfcn.h>
#   define HANDLE تهی*
#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#   define نماد_from_dll dlsym
#   define close_dll dlclose
#   define load_dll_error dlerror
#  endif

// lauxlib
#  اگر LUA_VERSION_رقم <= 501
#   define luaL_register dll_luaL_register
#   define luaL_prepbuffer dll_luaL_prepbuffer
#   define luaL_openlib dll_luaL_openlib
#   define luaL_typerror dll_luaL_typerror
#   define luaL_loadfile dll_luaL_loadfile
#   define luaL_loadbuffer dll_luaL_loadbuffer
#  وگرنه
#   define luaL_prepbuffsize dll_luaL_prepbuffsize
#   define luaL_setfuncs dll_luaL_setfuncs
#   define luaL_loadfilex dll_luaL_loadfilex
#   define luaL_loadbufferx dll_luaL_loadbufferx
#   define luaL_argerror dll_luaL_argerror
#  endif
#  اگر LUA_VERSION_رقم >= 504
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
#  اگر LUA_VERSION_رقم <= 501
#   define lua_tonumber dll_lua_tonumber
#   define lua_tointeger dll_lua_tointeger
#   define lua_call dll_lua_call
#   define lua_pcall dll_lua_pcall
#  وگرنه
#   define lua_tonumberx dll_lua_tonumberx
#   define lua_tointegerx dll_lua_tointegerx
#   define lua_callk dll_lua_callk
#   define lua_pcallk dll_lua_pcallk
#   define lua_getglobal dll_lua_getglobal
#   define lua_setglobal dll_lua_setglobal
#  endif
#  اگر LUA_VERSION_رقم <= 502
#   define lua_replace dll_lua_replace
#   define lua_remove dll_lua_remove
#  endif
#  اگر LUA_VERSION_رقم >= 503
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
#  اگر LUA_VERSION_رقم >= 504
#   define lua_newuserdatauv dll_lua_newuserdatauv
#  وگرنه
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
#  اگر LUA_VERSION_رقم >= 505
#   define luaL_openselectedlibs dll_luaL_openselectedlibs
#  وگرنه
#   define luaL_openlibs dll_luaL_openlibs
#  endif

// lauxlib
#  اگر LUA_VERSION_رقم <= 501
تهی (*dll_luaL_register) (lua_State *L, ثابت نویسه *libname, ثابت luaL_Reg *l);
نویسه *(*dll_luaL_prepbuffer) (luaL_Buffer *B);
تهی (*dll_luaL_openlib) (lua_State *L, ثابت نویسه *libname, ثابت luaL_Reg *l, صحیح nup);
صحیح (*dll_luaL_typerror) (lua_State *L, صحیح narg, ثابت نویسه *tname);
صحیح (*dll_luaL_loadfile) (lua_State *L, ثابت نویسه *filename);
صحیح (*dll_luaL_loadbuffer) (lua_State *L, ثابت نویسه *buff, size_t sz, ثابت نویسه *name);
#  وگرنه
نویسه *(*dll_luaL_prepbuffsize) (luaL_Buffer *B, size_t sz);
تهی (*dll_luaL_setfuncs) (lua_State *L, ثابت luaL_Reg *l, صحیح nup);
صحیح (*dll_luaL_loadfilex) (lua_State *L, ثابت نویسه *filename, ثابت نویسه *mode);
صحیح (*dll_luaL_loadbufferx) (lua_State *L, ثابت نویسه *buff, size_t sz, ثابت نویسه *name, ثابت نویسه *mode);
صحیح (*dll_luaL_argerror) (lua_State *L, صحیح numarg, ثابت نویسه *extramsg);
#  endif
#  اگر LUA_VERSION_رقم >= 504
صحیح (*dll_luaL_typeerror) (lua_State *L, صحیح narg, ثابت نویسه *tname);
#  endif
تهی (*dll_luaL_checkany) (lua_State *L, صحیح narg);
ثابت نویسه *(*dll_luaL_checklstring) (lua_State *L, صحیح رقم_Arg, size_t *l);
lua_Integer (*dll_luaL_checkinteger) (lua_State *L, صحیح رقم_Arg);
lua_Integer (*dll_luaL_optinteger) (lua_State *L, صحیح nArg, lua_Integer def);
تهی (*dll_luaL_checktype) (lua_State *L, صحیح narg, صحیح t);
صحیح (*dll_luaL_error) (lua_State *L, ثابت نویسه *fmt, ...);
lua_State *(*dll_luaL_newstate) (تهی);
تهی (*dll_luaL_buffinit) (lua_State *L, luaL_Buffer *B);
تهی (*dll_luaL_addlstring) (luaL_Buffer *B, ثابت نویسه *s, size_t l);
تهی (*dll_luaL_pushresult) (luaL_Buffer *B);
صحیح (*dll_luaL_loadstring) (lua_State *L, ثابت نویسه *s);
صحیح (*dll_luaL_ref) (lua_State *L, صحیح idx);
#  اگر LUA_VERSION_رقم <= 502
تهی (*dll_luaL_unref) (lua_State *L, صحیح idx, صحیح n);
#  وگرنه
تهی (*dll_luaL_unref) (lua_State *L, صحیح idx, lua_Integer n);
#  endif
// lua
#  اگر LUA_VERSION_رقم <= 501
lua_عدد (*dll_lua_tonumber) (lua_State *L, صحیح idx);
lua_Integer (*dll_lua_tointeger) (lua_State *L, صحیح idx);
تهی (*dll_lua_call) (lua_State *L, صحیح nargs, صحیح nresults);
صحیح (*dll_lua_pcall) (lua_State *L, صحیح nargs, صحیح nresults, صحیح errfunc);
#  وگرنه
lua_عدد (*dll_lua_tonumberx) (lua_State *L, صحیح idx, صحیح *isnum);
lua_Integer (*dll_lua_tointegerx) (lua_State *L, صحیح idx, صحیح *isnum);
تهی (*dll_lua_callk) (lua_State *L, صحیح nargs, صحیح nresults, صحیح ctx,
	lua_CFunction k);
صحیح (*dll_lua_pcallk) (lua_State *L, صحیح nargs, صحیح nresults, صحیح errfunc,
	صحیح ctx, lua_CFunction k);
تهی (*dll_lua_getglobal) (lua_State *L, ثابت نویسه *var);
تهی (*dll_lua_setglobal) (lua_State *L, ثابت نویسه *var);
#  endif
#  اگر LUA_VERSION_رقم <= 502
تهی (*dll_lua_replace) (lua_State *L, صحیح idx);
تهی (*dll_lua_remove) (lua_State *L, صحیح idx);
#  endif
#  اگر LUA_VERSION_رقم >= 503
تهی  (*dll_lua_rotate) (lua_State *L, صحیح idx, صحیح n);
تهی (*dll_lua_copy) (lua_State *L, صحیح fromidx, صحیح toidx);
#  endif
ثابت نویسه *(*dll_lua_typename) (lua_State *L, صحیح tp);
تهی       (*dll_lua_close) (lua_State *L);
صحیح (*dll_lua_gettop) (lua_State *L);
تهی (*dll_lua_settop) (lua_State *L, صحیح idx);
تهی (*dll_lua_pushvalue) (lua_State *L, صحیح idx);
صحیح (*dll_lua_isnumber) (lua_State *L, صحیح idx);
صحیح (*dll_lua_isstring) (lua_State *L, صحیح idx);
صحیح (*dll_lua_type) (lua_State *L, صحیح idx);
صحیح (*dll_lua_rawequal) (lua_State *L, صحیح idx1, صحیح idx2);
صحیح (*dll_lua_toboolean) (lua_State *L, صحیح idx);
ثابت نویسه *(*dll_lua_tolstring) (lua_State *L, صحیح idx, size_t *len);
تهی *(*dll_lua_touserdata) (lua_State *L, صحیح idx);
تهی (*dll_lua_pushnil) (lua_State *L);
تهی (*dll_lua_pushnumber) (lua_State *L, lua_عدد n);
تهی (*dll_lua_pushinteger) (lua_State *L, lua_Integer n);
تهی (*dll_lua_pushlstring) (lua_State *L, ثابت نویسه *s, size_t l);
تهی (*dll_lua_pushstring) (lua_State *L, ثابت نویسه *s);
ثابت نویسه *(*dll_lua_pushfstring) (lua_State *L, ثابت نویسه *fmt, ...);
تهی (*dll_lua_pushcclosure) (lua_State *L, lua_CFunction fn, صحیح n);
تهی (*dll_lua_pushboolean) (lua_State *L, صحیح b);
تهی (*dll_lua_pushlightuserdata) (lua_State *L, تهی *p);
تهی (*dll_lua_getfield) (lua_State *L, صحیح idx, ثابت نویسه *k);
#  اگر LUA_VERSION_رقم <= 502
تهی (*dll_lua_rawget) (lua_State *L, صحیح idx);
تهی (*dll_lua_rawgeti) (lua_State *L, صحیح idx, صحیح n);
#  وگرنه
صحیح (*dll_lua_rawget) (lua_State *L, صحیح idx);
صحیح (*dll_lua_rawgeti) (lua_State *L, صحیح idx, lua_Integer n);
#  endif
تهی (*dll_lua_createtable) (lua_State *L, صحیح narr, صحیح nrec);
تهی (*dll_lua_settable) (lua_State *L, صحیح idx);
#  اگر LUA_VERSION_رقم >= 504
تهی *(*dll_lua_newuserdatauv) (lua_State *L, size_t sz, صحیح nuvalue);
#  وگرنه
تهی *(*dll_lua_newuserdata) (lua_State *L, size_t sz);
#  endif
صحیح (*dll_lua_getmetatable) (lua_State *L, صحیح objindex);
تهی (*dll_lua_setfield) (lua_State *L, صحیح idx, ثابت نویسه *k);
تهی (*dll_lua_rawset) (lua_State *L, صحیح idx);
#  اگر LUA_VERSION_رقم <= 502
تهی (*dll_lua_rawseti) (lua_State *L, صحیح idx, صحیح n);
#  وگرنه
تهی (*dll_lua_rawseti) (lua_State *L, صحیح idx, lua_Integer n);
#  endif
صحیح (*dll_lua_setmetatable) (lua_State *L, صحیح objindex);
صحیح (*dll_lua_next) (lua_State *L, صحیح idx);
// libs
صحیح (*dll_luaopen_base) (lua_State *L);
صحیح (*dll_luaopen_table) (lua_State *L);
صحیح (*dll_luaopen_string) (lua_State *L);
صحیح (*dll_luaopen_math) (lua_State *L);
صحیح (*dll_luaopen_io) (lua_State *L);
صحیح (*dll_luaopen_os) (lua_State *L);
صحیح (*dll_luaopen_package) (lua_State *L);
صحیح (*dll_luaopen_debug) (lua_State *L);
#  اگر LUA_VERSION_رقم >= 505
تهی (*dll_luaL_openselectedlibs) (lua_State *L, صحیح load, صحیح preload);
#  وگرنه
تهی (*dll_luaL_openlibs) (lua_State *L);
#  endif

تعریفنوع تهی **luaV_function;
تعریفنوع ساختار {
    ثابت نویسه *name;
    luaV_function func;
} luaV_Reg;

ایستا ثابت luaV_Reg luaV_dll[] = {
    // lauxlib
#  اگر LUA_VERSION_رقم <= 501
    {"luaL_register", (luaV_function) &dll_luaL_register},
    {"luaL_prepbuffer", (luaV_function) &dll_luaL_prepbuffer},
    {"luaL_openlib", (luaV_function) &dll_luaL_openlib},
    {"luaL_typerror", (luaV_function) &dll_luaL_typerror},
    {"luaL_loadfile", (luaV_function) &dll_luaL_loadfile},
    {"luaL_loadbuffer", (luaV_function) &dll_luaL_loadbuffer},
#  وگرنه
    {"luaL_prepbuffsize", (luaV_function) &dll_luaL_prepbuffsize},
    {"luaL_setfuncs", (luaV_function) &dll_luaL_setfuncs},
    {"luaL_loadfilex", (luaV_function) &dll_luaL_loadfilex},
    {"luaL_loadbufferx", (luaV_function) &dll_luaL_loadbufferx},
    {"luaL_argerror", (luaV_function) &dll_luaL_argerror},
#  endif
#  اگر LUA_VERSION_رقم >= 504
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
#  اگر LUA_VERSION_رقم <= 501
    {"lua_tonumber", (luaV_function) &dll_lua_tonumber},
    {"lua_tointeger", (luaV_function) &dll_lua_tointeger},
    {"lua_call", (luaV_function) &dll_lua_call},
    {"lua_pcall", (luaV_function) &dll_lua_pcall},
#  وگرنه
    {"lua_tonumberx", (luaV_function) &dll_lua_tonumberx},
    {"lua_tointegerx", (luaV_function) &dll_lua_tointegerx},
    {"lua_callk", (luaV_function) &dll_lua_callk},
    {"lua_pcallk", (luaV_function) &dll_lua_pcallk},
    {"lua_getglobal", (luaV_function) &dll_lua_getglobal},
    {"lua_setglobal", (luaV_function) &dll_lua_setglobal},
#  endif
#  اگر LUA_VERSION_رقم <= 502
    {"lua_replace", (luaV_function) &dll_lua_replace},
    {"lua_remove", (luaV_function) &dll_lua_remove},
#  endif
#  اگر LUA_VERSION_رقم >= 503
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
#  اگر LUA_VERSION_رقم >= 504
    {"lua_newuserdatauv", (luaV_function) &dll_lua_newuserdatauv},
#  وگرنه
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
#  اگر LUA_VERSION_رقم >= 505
    {"luaL_openselectedlibs", (luaV_function) &dll_luaL_openselectedlibs},
#  وگرنه
    {"luaL_openlibs", (luaV_function) &dll_luaL_openlibs},
#  endif
    {پوچ, پوچ}
};

ایستا HANDLE hinstLua = پوچ;

    ایستا صحیح
lua_link_init(نویسه *libname, صحیح verbose)
{
    ثابت luaV_Reg *reg;
    اگر (hinstLua) بازگشت OK;
    hinstLua = load_dll(libname);
    اگر (!hinstLua)
    {
	اگر (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	بازگشت FAIL;
    }
    برای (reg = luaV_dll; reg->func; reg++)
    {
	اگر ((*reg->func = نماد_from_dll(hinstLua, reg->name)) == پوچ)
	{
	    close_dll(hinstLua);
	    hinstLua = 0;
	    اگر (verbose)
		semsg(_(e_could_not_load_library_function_str), reg->name);
	    بازگشت FAIL;
	}
    }
    بازگشت OK;
}
# endif // DYNAMIC_LUA

# اگر defined(DYNAMIC_LUA)
    صحیح
lua_enabled(صحیح verbose)
{
    بازگشت lua_link_init((نویسه *)p_luadll, verbose) == OK;
}
# endif

# اگر LUA_VERSION_رقم > 501 && LUA_VERSION_رقم < 504
    ایستا صحیح
luaL_typeerror(lua_State *L, صحیح narg, ثابت نویسه *tname)
{
    ثابت نویسه *پیام = lua_pushfstring(L, "%s expected, got %s",
	    tname, luaL_typename(L, narg));
    بازگشت luaL_argerror(L, narg, پیام);
}
# endif

    ایستا LUAV_INLINE تهی
luaV_getudata(lua_State *L, تهی *v)
{
    lua_pushlightuserdata(L, (تهی *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // now the cache table is at the top of the stack
    lua_pushlightuserdata(L, v);
    lua_rawget(L, -2);
    lua_remove(L, -2);  // remove the cache table from the stack
}

    ایستا LUAV_INLINE تهی
luaV_setudata(lua_State *L, تهی *v)
{
    lua_pushlightuserdata(L, (تهی *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // cache table is at -1
    lua_pushlightuserdata(L, v);       // ...now at -2
    lua_pushvalue(L, -3);	       // copy the userdata (cache at -3)
    lua_rawset(L, -3);		       // consumes two stack items
    lua_برداشت(L, 1);		       // and remove the cache table
}

// =======   Internal   =======

    ایستا تهی
luaV_newmetatable(lua_State *L, ثابت نویسه *tname)
{
    lua_newtable(L);
    lua_pushlightuserdata(L, (تهی *) tname);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
}

    ایستا تهی *
luaV_toudata(lua_State *L, صحیح ud, ثابت نویسه *tname)
{
    تهی *p = lua_touserdata(L, ud);

    اگر (p == پوچ)
	بازگشت پوچ;

    // value is userdata
    اگر (lua_getmetatable(L, ud)) // does it have a metatable?
    {
	luaV_getfield(L, tname); // get metatable
	اگر (lua_rawequal(L, -1, -2)) // MTs match?
	{
	    lua_برداشت(L, 2); // MTs
	    بازگشت p;
	}
    }

    بازگشت پوچ;
}

    ایستا تهی *
luaV_checkcache(lua_State *L, تهی *p)
{
    luaV_getudata(L, p);
    اگر (lua_isnil(L, -1)) luaL_error(L, "invalid object");
    lua_برداشت(L, 1);
    بازگشت p;
}

# define luaV_unbox(L,luatyp,ud) (*((luatyp *) lua_touserdata((L),(ud))))

# define luaV_checkvalid(L,luatyp,ud) \
    luaV_checkcache((L), (تهی *) luaV_unbox((L),luatyp,(ud)))

    ایستا تهی *
luaV_checkudata(lua_State *L, صحیح ud, ثابت نویسه *tname)
{
    تهی *p = luaV_toudata(L, ud, tname);
    اگر (p == پوچ) luaL_typeerror(L, ud, tname);
    بازگشت p;
}

    ایستا تهی
luaV_pushtypval(lua_State *L, typval_T *tv)
{
    اگر (tv == پوچ)
    {
	lua_pushnil(L);
	بازگشت;
    }
    انتخاب (tv->v_type)
    {
	حالت VAR_STRING:
	    lua_pushstring(L, tv->vval.v_string == پوچ
					    ? "" : (نویسه *)tv->vval.v_string);
	    بشکن;
	حالت VAR_عدد:
	    lua_pushinteger(L, (صحیح) tv->vval.v_عدد);
	    بشکن;
	حالت VAR_FLOAT:
	    lua_pushnumber(L, (lua_عدد) tv->vval.v_float);
	    بشکن;
	حالت VAR_LIST:
	    luaV_pushlist(L, tv->vval.v_list);
	    بشکن;
	حالت VAR_DICT:
	    luaV_pushdict(L, tv->vval.v_dict);
	    بشکن;
	حالت VAR_BOOL:
	حالت VAR_SPECIAL:
	    اگر (tv->vval.v_عدد <= VVAL_TRUE)
		lua_pushboolean(L, (صحیح) tv->vval.v_عدد);
	    وگرنه
		lua_pushnil(L);
	    بشکن;
	حالت VAR_FUNC:
	    luaV_pushfuncref(L, tv->vval.v_string);
	    بشکن;
	حالت VAR_PARTIAL:
	    // TODO: handle partial arguments
	    luaV_pushfuncref(L, partial_name(tv->vval.v_partial));
	    بشکن;

	حالت VAR_BLOB:
	    luaV_pushblob(L, tv->vval.v_blob);
	    بشکن;
	پیشفرض:
	    lua_pushnil(L);
    }
}

/*
 * Converts lua value at 'pos' to typval 'tv'.
 * Returns OK or FAIL.
 */
    ایستا صحیح
luaV_totypval(lua_State *L, صحیح pos, typval_T *tv)
{
    صحیح status = OK;

    tv->v_lock = 0;

    انتخاب (lua_type(L, pos))
    {
	حالت LUA_TBOOLEAN:
	    tv->v_type = VAR_BOOL;
	    tv->vval.v_عدد = (varnumber_T) lua_toboolean(L, pos);
	    بشکن;
	حالت LUA_TNIL:
	    tv->v_type = VAR_SPECIAL;
	    tv->vval.v_عدد = VVAL_NULL;
	    بشکن;
	حالت LUA_TSTRING:
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strsave((char_u *) lua_tostring(L, pos));
	    بشکن;
	حالت LUA_TNUMBER:
	{
	    ثابت lua_عدد n = lua_tonumber(L, pos);

	    اگر (n > (lua_عدد)INT64_MAX || n < (lua_عدد)INT64_MIN
		    || ((lua_عدد)((varnumber_T)n)) != n)
	    {
		tv->v_type = VAR_FLOAT;
		tv->vval.v_float = (float_T)n;
	    }
	    وگرنه
	    {
		tv->v_type = VAR_عدد;
		tv->vval.v_عدد = (varnumber_T)n;
	    }
	}
	    بشکن;
	حالت LUA_TFUNCTION:
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
	    بشکن;
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
		    name = register_cfunc(&luaV_call_lua_func,
					      &luaV_call_lua_func_free, state);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(name);
		    بشکن;
		}
	    }
	    tv->v_type = VAR_عدد;
	    tv->vval.v_عدد = 0;
	    status = FAIL;
	    بشکن;
	}
	حالت LUA_TUSERDATA:
	{
	    تهی *p = lua_touserdata(L, pos);

	    اگر (lua_getmetatable(L, pos)) // has metatable?
	    {
		// check list
		luaV_getfield(L, LUAVIM_LIST);
		اگر (lua_rawequal(L, -1, -2))
		{
		    tv->v_type = VAR_LIST;
		    tv->vval.v_list = *((luaV_List *) p);
		    ++tv->vval.v_list->lv_refcount;
		    lua_برداشت(L, 2); // MTs
		    بشکن;
		}
		// check dict
		luaV_getfield(L, LUAVIM_DICT);
		اگر (lua_rawequal(L, -1, -3))
		{
		    tv->v_type = VAR_DICT;
		    tv->vval.v_dict = *((luaV_Dict *) p);
		    ++tv->vval.v_dict->dv_refcount;
		    lua_برداشت(L, 3); // MTs
		    بشکن;
		}
		// check blob
		luaV_getfield(L, LUAVIM_BLOB);
		اگر (lua_rawequal(L, -1, -4))
		{
		    tv->v_type = VAR_BLOB;
		    tv->vval.v_blob = *((luaV_Blob *) p);
		    ++tv->vval.v_blob->bv_refcount;
		    lua_برداشت(L, 4); // MTs
		    بشکن;
		}
		// check funcref
		luaV_getfield(L, LUAVIM_FUNCREF);
		اگر (lua_rawequal(L, -1, -5))
		{
		    luaV_Funcref *f = (luaV_Funcref *) p;

		    func_ref(f->name);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(f->name);
		    lua_برداشت(L, 5); // MTs
		    بشکن;
		}
		lua_برداشت(L, 4); // MTs
	    }
	}
	// FALLTHROUGH
	پیشفرض:
	    tv->v_type = VAR_عدد;
	    tv->vval.v_عدد = 0;
	    status = FAIL;
    }
    بازگشت status;
}

/*
 * similar to luaL_addlstring, but replaces \0 with \n if toline and
 * \n with \0 otherwise
 */
    ایستا تهی
luaV_addlstring(luaL_Buffer *b, ثابت نویسه *s, size_t l, صحیح toline)
{
    وقتی (l--)
    {
	اگر (*s == '\0' && toline)
	    luaL_addchar(b, '\n');
	وگرنه اگر (*s == '\n' && !toline)
	    luaL_addchar(b, '\0');
	وگرنه
	    luaL_addchar(b, *s);
	s++;
    }
}

    ایستا تهی
luaV_pushline(lua_State *L, buf_T *buf, linenr_T n)
{
    ثابت نویسه *s = (ثابت نویسه *) ml_get_buf(buf, n, FALSE);
    luaL_Buffer b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, strlen(s), 0);
    luaL_pushresult(&b);
}

    ایستا char_u *
luaV_toline(lua_State *L, صحیح pos)
{
    size_t l;
    ثابت نویسه *s = lua_tolstring(L, pos, &l);

    luaL_Buffer b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 1);
    luaL_pushresult(&b);
    بازگشت (char_u *) lua_tostring(L, -1);
}

/*
 * pops a string s from the top of the stack and calls mf(t) for pieces t of
 * s separated by newlines
 */
    ایستا تهی
luaV_msgfunc(lua_State *L, msgfunc_T mf)
{
    luaL_Buffer b;
    size_t l;
    ثابت نویسه *p, *s = lua_tolstring(L, -1, &l);
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 0);
    luaL_pushresult(&b);
    // break string
    p = s = lua_tolstring(L, -1, &l);
    وقتی (l--)
    {
	اگر (*p++ == '\0') // break?
	{
	    mf((نویسه *)s);
	    s = p;
	}
    }
    mf((نویسه *)s);
    lua_برداشت(L, 2); // original and modified strings
}

# define luaV_newtype(typ,tname,luatyp,luatname) \
	ایستا luatyp * \
    luaV_new##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = (luatyp *) lua_newuserdata(L, اندازه(luatyp)); \
	*o = obj; \
	luaV_setudata(L, obj); /* cache[obj] = udata */ \
	luaV_getfield(L, luatname); \
	lua_setmetatable(L, -2); \
	بازگشت o; \
    }

# define luaV_pushtype(typ,tname,luatyp) \
	ایستا luatyp * \
    lua_V_افزودن##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = پوچ; \
	اگر (obj == پوچ) \
	    lua_pushnil(L); \
	وگرنه \
	{ \
	    luaV_getudata(L, obj); \
	    اگر (lua_isnil(L, -1)) /* not interned? */ \
	    { \
		lua_برداشت(L, 1); \
		o = luaV_new##tname(L, obj); \
	    } \
	    وگرنه \
		o = (luatyp *) lua_touserdata(L, -1); \
	} \
	بازگشت o; \
    }

# define luaV_type_tostring(tname,luatname) \
	ایستا صحیح \
    luaV_##tname##_tostring(lua_State *L) \
    { \
	lua_pushfstring(L, "%s: %p", luatname, lua_touserdata(L, 1)); \
	بازگشت 1; \
    }

// =======   List type   =======

    ایستا luaV_List *
luaV_newlist(lua_State *L, list_T *lis)
{
    luaV_List *l = (luaV_List *) lua_newuserdata(L, اندازه(luaV_List));
    *l = lis;
    lis->lv_refcount++; // reference in Lua
    luaV_setudata(L, lis); // cache[lis] = udata
    luaV_getfield(L, LUAVIM_LIST);
    lua_setmetatable(L, -2);
    بازگشت l;
}

luaV_pushtype(list_T, list, luaV_List)
luaV_type_tostring(list, LUAVIM_LIST)

    ایستا صحیح
luaV_list_len(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushinteger(L, (صحیح) list_len(l));
    بازگشت 1;
}

    ایستا صحیح
luaV_list_iter(lua_State *L)
{
    listitem_T *li = (listitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    اگر (li == پوچ) بازگشت 0;
    luaV_pushtypval(L, &li->li_tv);
    lua_pushlightuserdata(L, (تهی *) li->li_next);
    lua_replace(L, lua_upvalueindex(1));
    بازگشت 1;
}

    ایستا صحیح
luaV_list_call(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushlightuserdata(L, (تهی *) l->lv_first);
    lua_pushcclosure(L, luaV_list_iter, 1);
    بازگشت 1;
}

    ایستا صحیح
luaV_list_index(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    اگر (lua_isnumber(L, 2)) // list item?
    {
	long n = (long) luaL_checkinteger(L, 2);
	listitem_T *li;

	// Lua array index starts with 1 while Vim uses 0, subtract 1 to
	// normalize.
	n -= 1;
	li = list_find(l, n);
	اگر (li == پوچ)
	    lua_pushnil(L);
	وگرنه
	    luaV_pushtypval(L, &li->li_tv);
    }
    وگرنه اگر (lua_isstring(L, 2)) // method?
    {
	ثابت نویسه *s = lua_tostring(L, 2);
	اگر (strncmp(s, "add", 3) == 0
		|| strncmp(s, "insert", 6) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	وگرنه
	    lua_pushnil(L);
    }
    وگرنه
	lua_pushnil(L);
    بازگشت 1;
}

    ایستا صحیح
luaV_list_newindex(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    long n = (long) luaL_checkinteger(L, 2);
    listitem_T *li;

    // Lua array index starts with 1 while Vim uses 0, subtract 1 to normalize.
    n -= 1;

    اگر (l->lv_lock)
	luaL_error(L, "list is locked");
    li = list_find(l, n);
    اگر (li == پوچ)
    {
	اگر (!lua_isnil(L, 3))
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "inserting list item");
	    اگر (list_درج_tv(l, &v, li) == FAIL)
		luaL_error(L, "failed to add item to list");
	    clear_tv(&v);
	}
    }
    وگرنه
    {
	اگر (lua_isnil(L, 3)) // remove?
	{
	    vimlist_remove(l, li, li);
	    listitem_free(l, li);
	}
	وگرنه
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "setting list item");
	    clear_tv(&li->li_tv);
	    li->li_tv = v;
	}
    }
    بازگشت 0;
}

    ایستا صحیح
luaV_list_add(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (تهی *) *lis);
    typval_T v;
    اگر (l->lv_lock)
	luaL_error(L, "list is locked");
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "adding list item");
    اگر (list_append_tv(l, &v) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    بازگشت 1;
}

    ایستا صحیح
lua_V_list_درج(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (تهی *) *lis);
    long pos = (long) luaL_optinteger(L, 3, 0);
    listitem_T *li = پوچ;
    typval_T v;
    اگر (l->lv_lock)
	luaL_error(L, "list is locked");
    اگر (pos < l->lv_len)
    {
	li = list_find(l, pos);
	اگر (li == پوچ)
	    luaL_error(L, "invalid position");
    }
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "inserting list item");
    اگر (list_درج_tv(l, &v, li) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    بازگشت 1;
}

ایستا ثابت luaL_Reg luaV_List_mt[] = {
    {"__tostring", luaV_list_tostring},
    {"__len", luaV_list_len},
    {"__call", luaV_list_call},
    {"__index", luaV_list_index},
    {"__newindex", luaV_list_newindex},
    {"add", luaV_list_add},
    {"insert", lua_V_list_درج},
    {پوچ, پوچ}
};


// =======   Dict type   =======

    ایستا luaV_Dict *
luaV_newdict(lua_State *L, dict_T *dic)
{
    luaV_Dict *d = (luaV_Dict *) lua_newuserdata(L, اندازه(luaV_Dict));
    *d = dic;
    dic->dv_refcount++; // reference in Lua
    luaV_setudata(L, dic); // cache[dic] = udata
    luaV_getfield(L, LUAVIM_DICT);
    lua_setmetatable(L, -2);
    بازگشت d;
}

luaV_pushtype(dict_T, dict, luaV_Dict)
luaV_type_tostring(dict, LUAVIM_DICT)

    ایستا صحیح
luaV_dict_len(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    lua_pushinteger(L, (صحیح) dict_len(d));
    بازگشت 1;
}

    ایستا صحیح
luaV_dict_iter(lua_State *L UNUSED)
{
# ifdef FEAT_EVAL
    hashitem_T *hi = (hashitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    صحیح n = lua_tointeger(L, lua_upvalueindex(2));
    dictitem_T *di;
    اگر (n <= 0) بازگشت 0;
    وقتی (HASHITEM_EMPTY(hi)) hi++;
    di = dict_lookup(hi);
    lua_pushstring(L, (نویسه *) hi->hi_کلید);
    luaV_pushtypval(L, &di->di_tv);
    lua_pushlightuserdata(L, (تهی *) (hi + 1));
    lua_replace(L, lua_upvalueindex(1));
    lua_pushinteger(L, n - 1);
    lua_replace(L, lua_upvalueindex(2));
    بازگشت 2;
# وگرنه
    بازگشت 0;
# endif
}

    ایستا صحیح
luaV_dict_call(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    hashtab_T *ht = &d->dv_hashtab;
    lua_pushlightuserdata(L, (تهی *) ht->ht_array);
    lua_pushinteger(L, ht->ht_used); // # remaining items
    lua_pushcclosure(L, luaV_dict_iter, 2);
    بازگشت 1;
}

    ایستا صحیح
luaV_dict_index(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *کلید = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di = dict_find(d, کلید, -1);

    اگر (di == پوچ)
    {
	lua_pushnil(L);
	بازگشت 1;
    }

    luaV_pushtypval(L, &di->di_tv);
    اگر (di->di_tv.v_type == VAR_FUNC) // funcref?
    {
	luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, -1);
	f->self = d; // keep "self" reference
	d->dv_refcount++;
    }

    بازگشت 1;
}

    ایستا صحیح
luaV_dict_newindex(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *کلید = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di;
    typval_T tv;

    اگر (d->dv_lock)
	luaL_error(L, "dict is locked");
    اگر (کلید == پوچ)
	بازگشت 0;
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
    di = dict_find(d, کلید, -1);
    اگر (di == پوچ) // non-existing key?
    {
	اگر (lua_isnil(L, 3))
	    بازگشت 0;
	di = dictitem_alloc(کلید);
	اگر (di == پوچ)
	{
	    clear_tv(&tv);
	    بازگشت 0;
	}
	اگر (dict_add(d, di) == FAIL)
	{
	    vim_free(di);
	    clear_tv(&tv);
	    بازگشت 0;
	}
    }
    وگرنه
	clear_tv(&di->di_tv);
    اگر (lua_isnil(L, 3)) // remove?
    {
	hashitem_T *hi = hash_find(&d->dv_hashtab, di->di_کلید);
	hash_remove(&d->dv_hashtab, hi, "Lua new index");
	dictitem_free(di);
    }
    وگرنه
	di->di_tv = tv;
    بازگشت 0;
}

ایستا ثابت luaL_Reg luaV_Dict_mt[] = {
    {"__tostring", luaV_dict_tostring},
    {"__len", luaV_dict_len},
    {"__call", luaV_dict_call},
    {"__index", luaV_dict_index},
    {"__newindex", luaV_dict_newindex},
    {پوچ, پوچ}
};


// =======   Blob type   =======

    ایستا luaV_Blob *
luaV_newblob(lua_State *L, blob_T *blo)
{
    luaV_Blob *b = (luaV_Blob *) lua_newuserdata(L, اندازه(luaV_Blob));
    *b = blo;
    blo->bv_refcount++; // reference in Lua
    luaV_setudata(L, blo); // cache[blo] = udata
    luaV_getfield(L, LUAVIM_BLOB);
    lua_setmetatable(L, -2);
    بازگشت b;
}

luaV_pushtype(blob_T, blob, luaV_Blob)
luaV_type_tostring(blob, LUAVIM_BLOB)

    ایستا صحیح
luaV_blob_gc(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    blob_unref(b);
    بازگشت 0;
}

    ایستا صحیح
luaV_blob_len(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    lua_pushinteger(L, (صحیح) blob_len(b));
    بازگشت 1;
}

    ایستا صحیح
luaV_blob_index(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    اگر (lua_isnumber(L, 2))
    {
	صحیح idx = luaL_checkinteger(L, 2);
	اگر (idx < 0)
	    // Negative index is relative to the end.
	    idx += (صحیح) blob_len(b);
	اگر (idx >= 0 && idx < blob_len(b))
	    lua_pushnumber(L, (lua_عدد) blob_get(b, idx));
	وگرنه
	    lua_pushnil(L);
    }
    وگرنه اگر (lua_isstring(L, 2))
    {
	ثابت نویسه *s = lua_tostring(L, 2);
	اگر (strncmp(s, "add", 3) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	وگرنه
	    lua_pushnil(L);
    }
    وگرنه
	lua_pushnil(L);
    بازگشت 1;
}

    ایستا صحیح
luaV_blob_newindex(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    اگر (b->bv_lock)
	luaL_error(L, "blob is locked");

    اگر (!lua_isnumber(L, 2))
	بازگشت 0;

    long len = blob_len(b);
    صحیح idx = luaL_checkinteger(L, 2);
    صحیح val = luaL_checkinteger(L, 3);
    اگر (idx < 0)
	// Negative index is relative to the end.
	idx += (صحیح) len;
    اگر ((idx >= 0 && idx < len)
	    || (idx == len && ga_grow(&b->bv_ga, 1) == OK))
    {
	blob_set(b, idx, (char_u) val);
	اگر (idx == len)
	    ++b->bv_ga.ga_len;
    }
    وگرنه
	luaL_error(L, "index out of range");

    بازگشت 0;
}

    ایستا صحیح
luaV_blob_add(lua_State *L)
{
    luaV_Blob *blo = luaV_checkudata(L, 1, LUAVIM_BLOB);
    blob_T *b = (blob_T *) luaV_checkcache(L, (تهی *) *blo);
    اگر (b->bv_lock)
	luaL_error(L, "blob is locked");
    lua_settop(L, 2);
    اگر (!lua_isstring(L, 2))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 2));
    وگرنه
    {
	size_t i, l = 0;
	ثابت نویسه *s = lua_tolstring(L, 2, &l);

	اگر (ga_grow(&b->bv_ga, (صحیح)l) == OK)
	    برای (i = 0; i < l; ++i)
		ga_append(&b->bv_ga, s[i]);
    }
    lua_settop(L, 1);
    بازگشت 1;
}

ایستا ثابت luaL_Reg luaV_Blob_mt[] = {
    {"__tostring", luaV_blob_tostring},
    {"__gc", luaV_blob_gc},
    {"__len", luaV_blob_len},
    {"__index", luaV_blob_index},
    {"__newindex", luaV_blob_newindex},
    {"add", luaV_blob_add},
    {پوچ, پوچ}
};


// =======   Funcref type   =======

    ایستا luaV_Funcref *
luaV_newfuncref(lua_State *L, char_u *name)
{
    luaV_Funcref *f = (luaV_Funcref *)lua_newuserdata(L, اندازه(luaV_Funcref));

    اگر (name != پوچ)
    {
	func_ref(name);
	f->name = vim_strsave(name);
    }
    f->self = پوچ;
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_setmetatable(L, -2);
    بازگشت f;
}

    ایستا luaV_Funcref *
luaV_pushfuncref(lua_State *L, char_u *name)
{
    بازگشت luaV_newfuncref(L, name);
}


luaV_type_tostring(funcref, LUAVIM_FUNCREF)

    ایستا صحیح
luaV_funcref_gc(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    func_unref(f->name);
    vim_free(f->name);
    // NOTE: Don't call "dict_unref(f->self)", because the dict of "f->self"
    // will be (or has been already) freed by Vim's garbage collection.
    بازگشت 0;
}

// equivalent to string(funcref)
    ایستا صحیح
luaV_funcref_len(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    lua_pushstring(L, (ثابت نویسه *) f->name);
    بازگشت 1;
}

    ایستا صحیح
luaV_funcref_call(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);
    صحیح i, n = lua_gettop(L) - 1; // #args
    صحیح status = FAIL;
    typval_T args;
    typval_T rettv;

    args.v_type = VAR_LIST;
    args.vval.v_list = list_alloc();
    rettv.v_type = VAR_UNKNOWN; // as in clear_tv
    اگر (args.vval.v_list != پوچ)
    {
	typval_T v;

	برای (i = 0; i < n; i++)
	{
	    luaV_checktypval(L, i + 2, &v, "calling funcref");
	    list_append_tv(args.vval.v_list, &v);
	    clear_tv(&v);
	}
	status = func_call(f->name, &args, پوچ, f->self, &rettv);
	اگر (status == OK)
	    luaV_pushtypval(L, &rettv);
	clear_tv(&args);
	clear_tv(&rettv);
    }
    اگر (status != OK)
	luaL_error(L, "cannot call funcref");
    بازگشت 1;
}

ایستا ثابت luaL_Reg luaV_Funcref_mt[] = {
    {"__tostring", luaV_funcref_tostring},
    {"__gc", luaV_funcref_gc},
    {"__len", luaV_funcref_len},
    {"__call", luaV_funcref_call},
    {پوچ, پوچ}
};


// =======   Buffer type   =======

luaV_newtype(buf_T, buffer, luaV_Buffer, LUAVIM_BUFFER)
luaV_pushtype(buf_T, buffer, luaV_Buffer)
luaV_type_tostring(buffer, LUAVIM_BUFFER)

    ایستا صحیح
luaV_buffer_len(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    lua_pushinteger(L, b->b_ml.ml_سطر_شمار);
    بازگشت 1;
}

    ایستا صحیح
luaV_buffer_call(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    lua_settop(L, 1);
    set_curbuf(b, DOBUF_SPLIT);
    بازگشت 1;
}

    ایستا صحیح
luaV_buffer_index(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    linenr_T n = (linenr_T) lua_tointeger(L, 2);
    اگر (n > 0 && n <= b->b_ml.ml_سطر_شمار)
	luaV_pushline(L, b, n);
    وگرنه اگر (lua_isstring(L, 2))
    {
	ثابت نویسه *s = lua_tostring(L, 2);
	اگر (strncmp(s, "name", 4) == 0)
	    lua_pushstring(L, (b->b_sfname == پوچ)
					? "" : (نویسه *) b->b_sfname);
	وگرنه اگر (strncmp(s, "fname", 5) == 0)
	    lua_pushstring(L, (b->b_ffname == پوچ)
					? "" : (نویسه *) b->b_ffname);
	وگرنه اگر (strncmp(s, "number", 6) == 0)
	    lua_pushinteger(L, b->b_fnum);
	// methods
	وگرنه اگر (strncmp(s,   "insert", 6) == 0
		|| strncmp(s, "next", 4) == 0
		|| strncmp(s, "previous", 8) == 0
		|| strncmp(s, "isvalid", 7) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	وگرنه
	    lua_pushnil(L);
    }
    وگرنه
	lua_pushnil(L);
    بازگشت 1;
}

    ایستا صحیح
luaV_buffer_newindex(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    linenr_T n = (linenr_T) luaL_checkinteger(L, 2);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    اگر (n < 1 || n > b->b_ml.ml_سطر_شمار)
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
	وگرنه اگر (ml_delete(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot delete line");
	}
	وگرنه
	{
	    deleted_سطرها_mark(n, 1L);
	    اگر (b == curwin->w_buffer) // fix cursor in current window?
	    {
		اگر (curwin->w_cursor.lnum >= n)
		{
		    اگر (curwin->w_cursor.lnum > n)
		    {
			curwin->w_cursor.lnum -= 1;
			check_cursor_col();
		    }
		    وگرنه
			check_cursor();
		    changed_cline_bef_curs();
		}
		invalidate_botline();
	    }
	}
	curbuf = buf;
    }
    وگرنه اگر (lua_isstring(L, 3)) // update line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	اگر (u_savesub(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	وگرنه اگر (ml_replace(n, luaV_toline(L, 3), TRUE) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot replace line");
	}
	وگرنه
	    changed_bytes(n, 0);
	curbuf = buf;
	اگر (b == curwin->w_buffer)
	    check_cursor_col();
    }
    وگرنه
	luaL_error(L, "wrong argument to change line");
    بازگشت 0;
}

    ایستا صحیح
lua_V_buffer_درج(lua_State *L)
{
    luaV_Buffer *lb = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *b = (buf_T *) luaV_checkcache(L, (تهی *) *lb);
    linenr_T last = b->b_ml.ml_سطر_شمار;
    linenr_T n = (linenr_T) luaL_optinteger(L, 3, last);
    buf_T *buf;
    luaL_checktype(L, 2, LUA_TSTRING);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    // fix insertion line
    اگر (n < 0) n = 0;
    اگر (n > last) n = last;
    // insert
    buf = curbuf;
    curbuf = b;
    اگر (u_save(n, n + 1) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot save undo information");
    }
    وگرنه اگر (ml_append(n, luaV_toline(L, 2), 0, FALSE) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot insert line");
    }
    وگرنه
	appended_سطرها_mark(n, 1L);
    curbuf = buf;
    update_screen(UPD_VALID);
    بازگشت 0;
}

    ایستا صحیح
luaV_buffer_next(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (تهی *) *b);
    luaV_pushbuffer(L, buf->b_next);
    بازگشت 1;
}

    ایستا صحیح
luaV_buffer_previous(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (تهی *) *b);
    luaV_pushbuffer(L, buf->b_prev);
    بازگشت 1;
}

    ایستا صحیح
luaV_buffer_isvalid(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    luaV_getudata(L, *b);
    lua_pushboolean(L, !lua_isnil(L, -1));
    بازگشت 1;
}

ایستا ثابت luaL_Reg luaV_Buffer_mt[] = {
    {"__tostring", luaV_buffer_tostring},
    {"__len", luaV_buffer_len},
    {"__call", luaV_buffer_call},
    {"__index", luaV_buffer_index},
    {"__newindex", luaV_buffer_newindex},
    {"insert", lua_V_buffer_درج},
    {"next", luaV_buffer_next},
    {"previous", luaV_buffer_previous},
    {"isvalid", luaV_buffer_isvalid},
    {پوچ, پوچ}
};


// =======   Window type   =======

luaV_newtype(win_T, window, luaV_Window, LUAVIM_WINDOW)
luaV_pushtype(win_T, window, luaV_Window)
luaV_type_tostring(window, LUAVIM_WINDOW)

    ایستا صحیح
luaV_window_call(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    lua_settop(L, 1);
    win_goto(w);
    بازگشت 1;
}

    ایستا صحیح
luaV_window_index(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    ثابت نویسه *s = luaL_checkstring(L, 2);
    اگر (strncmp(s, "buffer", 6) == 0)
	luaV_pushbuffer(L, w->w_buffer);
    وگرنه اگر (strncmp(s, "line", 4) == 0)
	lua_pushinteger(L, w->w_cursor.lnum);
    وگرنه اگر (strncmp(s, "col", 3) == 0)
	lua_pushinteger(L, w->w_cursor.col + 1);
    وگرنه اگر (strncmp(s, "width", 5) == 0)
	lua_pushinteger(L, w->w_width);
    وگرنه اگر (strncmp(s, "height", 6) == 0)
	lua_pushinteger(L, w->w_height);
    // methods
    وگرنه اگر (strncmp(s,   "next", 4) == 0
	    || strncmp(s, "previous", 8) == 0
	    || strncmp(s, "isvalid", 7) == 0)
    {
	lua_getmetatable(L, 1);
	lua_getfield(L, -1, s);
    }
    وگرنه
	lua_pushnil(L);
    بازگشت 1;
}

    ایستا صحیح
luaV_window_newindex(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    ثابت نویسه *s = luaL_checkstring(L, 2);
    صحیح v = luaL_checkinteger(L, 3);
    اگر (strncmp(s, "line", 4) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	اگر (v < 1 || v > w->w_buffer->b_ml.ml_سطر_شمار)
	    luaL_error(L, "line out of range");
	w->w_cursor.lnum = v;
	update_screen(UPD_VALID);
    }
    وگرنه اگر (strncmp(s, "col", 3) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	w->w_cursor.col = v - 1;
	w->w_set_curswant = true;
	update_screen(UPD_VALID);
    }
    وگرنه اگر (strncmp(s, "width", 5) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setwidth(v);
	curwin = win;
    }
    وگرنه اگر (strncmp(s, "height", 6) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setheight(v);
	curwin = win;
    }
    وگرنه
	luaL_error(L, "invalid window property: `%s'", s);
    بازگشت 0;
}

    ایستا صحیح
luaV_window_next(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    win_T *win = (win_T *) luaV_checkcache(L, (تهی *) *w);
    luaV_pushwindow(L, win->w_next);
    بازگشت 1;
}

    ایستا صحیح
luaV_window_previous(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    win_T *win = (win_T *) luaV_checkcache(L, (تهی *) *w);
    luaV_pushwindow(L, win->w_prev);
    بازگشت 1;
}

    ایستا صحیح
luaV_window_isvalid(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    luaV_getudata(L, *w);
    lua_pushboolean(L, !lua_isnil(L, -1));
    بازگشت 1;
}

ایستا ثابت luaL_Reg luaV_Window_mt[] = {
    {"__tostring", luaV_window_tostring},
    {"__call", luaV_window_call},
    {"__index", luaV_window_index},
    {"__newindex", luaV_window_newindex},
    {"next", luaV_window_next},
    {"previous", luaV_window_previous},
    {"isvalid", luaV_window_isvalid},
    {پوچ, پوچ}
};


// =======   Vim module   =======

    ایستا صحیح
luaV_print(lua_State *L)
{
    صحیح		i, n = lua_gettop(L); // nargs
    ثابت نویسه	*s;
    size_t	l;
    garray_T	پیام_ga;

    ga_init2(&پیام_ga, 1, 128);
    lua_getglobal(L, "tostring");
    برای (i = 1; i <= n; i++)
    {
	lua_pushvalue(L, -1); // tostring
	lua_pushvalue(L, i); // arg
	lua_call(L, 1, 1);
	s = lua_tolstring(L, -1, &l);
	اگر (s == پوچ)
	    بازگشت luaL_error(L, "cannot convert to string");
	اگر (i > 1)
	    ga_append(&پیام_ga, ' '); // use space instead of tab
	ga_concat_len(&پیام_ga, (char_u *)s, l);
	lua_برداشت(L, 1);
    }
    // Replace any "\n" with "\0"
    برای (i = 0; i < پیام_ga.ga_len; i++)
	اگر (((نویسه *)پیام_ga.ga_data)[i] == '\n')
	    ((نویسه *)پیام_ga.ga_data)[i] = '\0';
    lua_pushlstring(L, پیام_ga.ga_data, پیام_ga.ga_len);
    اگر (!got_int)
	lua_V_پیام(L);

    ga_clear(&پیام_ga);
    بازگشت 0;
}

    ایستا صحیح
luaV_debug(lua_State *L)
{
    lua_settop(L, 0);
    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "eval");
    lua_remove(L, -2); // vim.eval at position 1
    برای (;;)
    {
	ثابت نویسه *درون_داد;
	size_t l;
	lua_pushvalue(L, 1); // vim.eval
	lua_pushliteral(L, "input('lua_debug> ')");
	lua_call(L, 1, 1); // return string
	درون_داد = lua_tolstring(L, -1, &l);
	اگر (l == 0 || strcmp(درون_داد, "cont") == 0)
	    بازگشت 0;
	پیام_putchar('\n'); // avoid outputting on input line
	اگر (luaL_loadbuffer(L, درون_داد, l, "=(debug command)")
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
	lua_settop(L, 1); // remove eventual returns, but keep vim.eval
    }
}

    ایستا dict_T *
luaV_get_var_scope(lua_State *L)
{
    ثابت نویسه	*scope = luaL_checkstring(L, 1);
    dict_T	*dict = پوچ;

    اگر (STRICMP((نویسه *)scope, "g") == 0)
	dict = get_globvar_dict();
    وگرنه اگر (STRICMP((نویسه *)scope, "v") == 0)
	dict = get_vimvar_dict();
    وگرنه اگر (STRICMP((نویسه *)scope, "b") == 0)
	dict = curbuf->b_vars;
    وگرنه اگر (STRICMP((نویسه *)scope, "w") == 0)
	dict = curwin->w_vars;
    وگرنه اگر (STRICMP((نویسه *)scope, "t") == 0)
	dict = curtab->tp_vars;
    وگرنه
    {
	luaL_error(L, "invalid scope %s", scope);
	بازگشت پوچ;
    }

    بازگشت dict;
}

    ایستا صحیح
luaV_setvar(lua_State *L)
{
    dict_T	*dict;
    dictitem_T	*di;
    size_t	len;
    نویسه	*name;
    صحیح		del;
    نویسه	*error = پوچ;

    name = (نویسه *)luaL_checklstring(L, 3, &len);
    del = (lua_gettop(L) < 4) || lua_isnil(L, 4);

    dict = luaV_get_var_scope(L);
    اگر (dict == پوچ)
	بازگشت 0;

    di = dict_find(dict, (char_u *)name, (صحیح)len);
    اگر (di != پوچ)
    {
	اگر (di->di_flags & DI_FLAGS_RO)
	    error = "variable is read-only";
	وگرنه اگر (di->di_flags & DI_FLAGS_LOCK)
	    error = "variable is locked";
	وگرنه اگر (del && di->di_flags & DI_FLAGS_FIX)
	    error = "variable is fixed";
	اگر (error != پوچ)
	    بازگشت luaL_error(L, error);
    }
    وگرنه اگر (dict->dv_lock)
	بازگشت luaL_error(L, "Dictionary is locked");

    اگر (del)
    {
	// Delete the key
	اگر (di == پوچ)
	    // Doesn't exist, nothing to do
	    بازگشت 0;
	// Delete the entry
	dictitem_remove(dict, di, "Lua delete variable");
    }
    وگرنه
    {
	// Update the key
	typval_T	tv;

	// Convert the lua value to a Vim script type in the temporary variable
	lua_pushvalue(L, 4);
	اگر (luaV_totypval(L, -1, &tv) == FAIL)
	    بازگشت luaL_error(L, "Couldn't convert lua value");

	اگر (di == پوچ)
	{
	    // Need to create an entry
	    di = dictitem_alloc((char_u *)name);
	    اگر (di == پوچ)
	    {
		clear_tv(&tv);
		بازگشت 0;
	    }
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	    اگر (dict_add(dict, di) == FAIL)
	    {
		dictitem_free(di);
		clear_tv(&tv);
		بازگشت luaL_error(L, "Couldn't add to dictionary");
	    }
	}
	وگرنه
	{
	    صحیح type_error = FALSE;
	    اگر (dict == get_vimvar_dict()
	       && !before_set_vvar((char_u *)name, di, &tv, TRUE, &type_error))
	    {
		clear_tv(&tv);
		اگر (type_error)
		    بازگشت luaL_error(L,
				"Setting v:%s to value with wrong type", name);
		بازگشت 0;
	    }
	    // Clear the old value
	    clear_tv(&di->di_tv);
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	}

	// Clear the temporary variable
	clear_tv(&tv);
    }

    بازگشت 0;
}

    ایستا صحیح
luaV_getvar(lua_State *L)
{
    dict_T	*dict = luaV_get_var_scope(L);
    size_t	len;
    ثابت نویسه	*name = luaL_checklstring(L, 3, &len);
    dictitem_T	*di = dict_find(dict, (char_u *)name, (صحیح)len);

    اگر (di == پوچ)
	بازگشت 0;  // nil

    luaV_pushtypval(L, &di->di_tv);
    بازگشت 1;
}

    ایستا صحیح
luaV_command(lua_State *L)
{
    char_u  *s = vim_strsave((char_u *)luaL_checkstring(L, 1));

    execute_cmds_from_string(s);
    vim_free(s);
    update_screen(UPD_VALID);
    بازگشت 0;
}

    ایستا صحیح
luaV_eval(lua_State *L)
{
    typval_T *tv = eval_عبارت((char_u *) luaL_checkstring(L, 1), پوچ);

    اگر (tv == پوچ) luaL_error(L, "invalid expression");
    luaV_pushtypval(L, tv);
    free_tv(tv);
    بازگشت 1;
}

    ایستا صحیح
luaV_beep(lua_State *L UNUSED)
{
    vim_beep(BO_LANG);
    بازگشت 0;
}

    ایستا صحیح
lua_V_سطر(lua_State *L)
{
    luaV_pushline(L, curbuf, curwin->w_cursor.lnum);
    بازگشت 1;
}

    ایستا صحیح
luaV_list(lua_State *L)
{
    list_T *l;
    صحیح initarg = !lua_isnoneornil(L, 1);

    اگر (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    l = list_alloc();
    اگر (l == پوچ)
    {
	lua_pushnil(L);
	بازگشت 1;
    }

    luaV_newlist(L, l);
    اگر (!initarg)
	بازگشت 1;

    // traverse table to init list
    صحیح notnil, i = 0;
    typval_T v;
    انجامبده
    {
	lua_rawgeti(L, 1, ++i);
	notnil = !lua_isnil(L, -1);
	اگر (notnil)
	{
	    luaV_checktypval(L, -1, &v, "vim.list");
	    list_append_tv(l, &v);
	    clear_tv(&v);
	}
	lua_برداشت(L, 1); //مقدار
    } وقتی (notnil);

    بازگشت 1;
}

    ایستا صحیح
luaV_dict(lua_State *L)
{
    dict_T *d;
    صحیح initarg = !lua_isnoneornil(L, 1);

    اگر (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    d = dict_alloc();
    اگر (d == پوچ)
    {
	lua_pushnil(L);
	بازگشت 1;
    }

    luaV_newdict(L, d);
    اگر (!initarg)
	بازگشت 1;

    // traverse table to init dict
    lua_pushnil(L);
    وقتی (lua_next(L, 1))
    {
	char_u *کلید;
	dictitem_T *di;
	typval_T v;

	lua_pushvalue(L, -2); // dup key in case it's a number
	کلید = (char_u *) lua_tostring(L, -1);
	اگر (کلید == پوچ)
	{
	    lua_pushnil(L);
	    بازگشت 1;
	}
	اگر (*کلید == NUL)
	    luaL_error(L, "table has empty key");
	luaV_checktypval(L, -2, &v, "vim.dict"); //مقدار
	di = dictitem_alloc(کلید);
	اگر (di == پوچ || dict_add(d, di) == FAIL)
	{
	    vim_free(di);
	    lua_pushnil(L);
	    بازگشت 1;
	}
	di->di_tv = v;
	lua_برداشت(L, 2); // key copy and value
    }

    بازگشت 1;
}

    ایستا صحیح
luaV_blob(lua_State *L)
{
    blob_T *b;
    صحیح initarg = !lua_isnoneornil(L, 1);

    اگر (initarg && !lua_isstring(L, 1))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 1));

    b = blob_alloc();
    اگر (b == پوچ)
    {
	lua_pushnil(L);
	بازگشت 1;
    }

    luaV_newblob(L, b);
    اگر (!initarg)
	بازگشت 1;

    // traverse table to init blob
    size_t i, l = 0;
    ثابت نویسه *s = lua_tolstring(L, 1, &l);

    اگر (ga_grow(&b->bv_ga, (صحیح)l) == OK)
	برای (i = 0; i < l; ++i)
	    ga_append(&b->bv_ga, s[i]);

    بازگشت 1;
}

    ایستا صحیح
luaV_funcref(lua_State *L)
{
    ثابت نویسه *name = luaL_checkstring(L, 1);
    // note: not checking if function exists (needs function_exists)
    اگر (name == پوچ || *name == NUL || VIM_ISDIGIT(*name))
	luaL_error(L, "invalid function name: %s", name);
    luaV_newfuncref(L, (char_u *) name);
    بازگشت 1;
}

    ایستا صحیح
luaV_buffer(lua_State *L)
{
    buf_T *buf;
    اگر (lua_isstring(L, 1)) // get by number or name?
    {
	اگر (lua_isnumber(L, 1)) // by number?
	{
	    صحیح n = lua_tointeger(L, 1);
	    FOR_ALL_BUFFERS(buf)
		اگر (buf->b_fnum == n) بشکن;
	}
	وگرنه // by name
	{
	    size_t l;
	    ثابت نویسه *s = lua_tolstring(L, 1, &l);
	    FOR_ALL_BUFFERS(buf)
	    {
		اگر (buf->b_ffname == پوچ || buf->b_sfname == پوچ)
		{
		    اگر (l == 0) بشکن;
		}
		وگرنه اگر (strncmp(s, (نویسه *)buf->b_ffname, l) == 0
			|| strncmp(s, (نویسه *)buf->b_sfname, l) == 0)
		    بشکن;
	    }
	}
    }
    وگرنه
	buf = (lua_toboolean(L, 1)) ? firstbuf : curbuf; // first buffer?
    luaV_pushbuffer(L, buf);
    بازگشت 1;
}

    ایستا صحیح
luaV_window(lua_State *L)
{
    win_T *win;
    اگر (lua_isnumber(L, 1)) // get by number?
    {
	صحیح n = lua_tointeger(L, 1);
	برای (win = firstwin; win != پوچ; win = win->w_next, n--)
	    اگر (n == 1) بشکن;
    }
    وگرنه
	win = (lua_toboolean(L, 1)) ? firstwin : curwin; // first window?
    luaV_pushwindow(L, win);
    بازگشت 1;
}

    ایستا صحیح
luaV_open(lua_State *L)
{
    char_u *s = پوچ;
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    اگر (lua_isstring(L, 1)) s = (char_u *) lua_tostring(L, 1);
    luaV_pushbuffer(L, buflist_new(s, پوچ, 1L, BLN_LISTED));
    بازگشت 1;
}

    ایستا صحیح
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
		بازگشت 1;
	    }
	    luaV_getfield(L, LUAVIM_DICT);
	    اگر (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "dict");
		بازگشت 1;
	    }
	    luaV_getfield(L, LUAVIM_BLOB);
	    اگر (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "blob");
		بازگشت 1;
	    }
	    luaV_getfield(L, LUAVIM_FUNCREF);
	    اگر (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "funcref");
		بازگشت 1;
	    }
	    luaV_getfield(L, LUAVIM_BUFFER);
	    اگر (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "buffer");
		بازگشت 1;
	    }
	    luaV_getfield(L, LUAVIM_WINDOW);
	    اگر (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "window");
		بازگشت 1;
	    }
	}
    }
    lua_pushstring(L, luaL_typename(L, 1)); // fallback
    بازگشت 1;
}

    ایستا صحیح
luaV_call(lua_State *L)
{
    صحیح		argc = lua_gettop(L) - 1;
    size_t	funcname_len;
    char_u	*funcname;
    نویسه	*error = پوچ;
    typval_T	rettv;
    typval_T	argv[MAX_FUNC_ARGS + 1];
    صحیح		i = 0;

    اگر (argc > MAX_FUNC_ARGS)
	بازگشت luaL_error(L, "Function called with too many arguments");

    funcname = (char_u *)luaL_checklstring(L, 1, &funcname_len);

    برای (; i < argc; i++)
    {
	اگر (luaV_totypval(L, i + 2, &argv[i]) == FAIL)
	{
	    error = "lua: cannot convert value";
	    برو free_vim_args;
	}
    }

    argv[argc].v_type = VAR_UNKNOWN;

    اگر (call_vim_function(funcname, argc, argv, &rettv) == FAIL)
    {
	error = "lua: call_vim_function failed";
	برو free_vim_args;
    }

    luaV_pushtypval(L, &rettv);
    clear_tv(&rettv);

free_vim_args:
    وقتی (i > 0)
	clear_tv(&argv[--i]);

    اگر (error == پوچ)
	بازگشت 1;
    وگرنه
	بازگشت luaL_error(L, error);
}

/*
 * Return the Vim version as a Lua table
 */
    ایستا صحیح
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
    بازگشت 1;
}

ایستا ثابت luaL_Reg luaV_module[] = {
    {"command", luaV_command},
    {"eval", luaV_eval},
    {"beep", luaV_beep},
    {"line", lua_V_سطر},
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
    {"lua_version", پوچ},
    {پوچ, پوچ}
};

/*
 * for freeing list, dict, buffer and window objects; lightuserdata as arg
 */
    ایستا صحیح
luaV_free(lua_State *L)
{
    lua_pushnil(L);
    luaV_setudata(L, lua_touserdata(L, 1));
    بازگشت 0;
}

    ایستا صحیح
luaV_luaeval(lua_State *L)
{
    luaL_Buffer b;
    size_t l;
    ثابت نویسه *str = lua_tolstring(L, 1, &l);
    typval_T *arg = (typval_T *) lua_touserdata(L, 2);
    typval_T *rettv = (typval_T *) lua_touserdata(L, 3);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, LUAVIM_EVALHEADER, اندازه(LUAVIM_EVALHEADER) - 1);
    luaL_addlstring(&b, str, l);
    luaL_pushresult(&b);
    str = lua_tolstring(L, -1, &l);
    اگر (luaL_loadbuffer(L, str, l, LUAVIM_EVALNAME)) // compile error?
    {
	luaV_emsg(L);
	بازگشت 0;
    }
    luaV_pushtypval(L, arg);
    اگر (lua_pcall(L, 1, 1, 0)) // running error?
    {
	luaV_emsg(L);
	بازگشت 0;
    }
    اگر (luaV_totypval(L, -1, rettv) == FAIL)
	emsg("luaeval: cannot convert value");
    بازگشت 0;
}

    ایستا صحیح
luaV_setref(lua_State *L)
{
    صحیح copyID = lua_tointeger(L, 1);
    صحیح abort = FALSE;

    lua_pushlightuserdata(L, (تهی *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // the cache table

    luaV_getfield(L, LUAVIM_LIST);
    luaV_getfield(L, LUAVIM_DICT);
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_pushnil(L);
    // traverse cache table
    وقتی (!abort && lua_next(L, 2) != 0)
    {
	lua_getmetatable(L, -1);
	اگر (lua_rawequal(L, -1, 3)) // list?
	{
	    list_T *l = (list_T *)lua_touserdata(L, 6); //کلید

	    abort = set_ref_in_list(l, copyID);
	}
	وگرنه اگر (lua_rawequal(L, -1, 4)) // dict?
	{
	    dict_T *d = (dict_T *)lua_touserdata(L, 6); //کلید

	    abort = set_ref_in_dict(d, copyID);
	}
	وگرنه اگر (lua_rawequal(L, -1, 5)) // funcref?
	{
	    luaV_Funcref *f = (luaV_Funcref *)lua_touserdata(L, 6); //کلید

	    abort = set_ref_in_dict(f->self, copyID);
	}
	lua_برداشت(L, 2); // metatable and value
    }
    lua_pushinteger(L, abort);
    بازگشت 1;
}

    ایستا صحیح
luaV_pushversion(lua_State *L)
{
    صحیح major = 0;
    صحیح minor = 0;
    صحیح patch = 0;
    نویسه s[16];

    sscanf(LUAVIM_VERSION, "Lua %d.%d.%d", &major, &minor, &patch);
    vim_snprintf(s, اندازه(s), "%d.%d.%d", major, minor, patch);
    lua_pushstring(L, s);
    بازگشت 0;
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

    ایستا صحیح
luaopen_vim(lua_State *L)
{
    lua_newtable(L);  // cache table
    lua_newtable(L);  // cache table's metatable
    lua_pushstring(L, "v");
    lua_setfield(L, -2, "__mode");
    lua_setmetatable(L, -2); // cache is weak-valued
    // put the cache table in the registry for luaV_get/setudata()
    lua_pushlightuserdata(L, (تهی *) LUAVIM_UDATA_CACHE);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
    lua_برداشت(L, 1);  // we don't need the cache table here anymore
    // print
    lua_pushcfunction(L, luaV_print);
    lua_setglobal(L, "print");
    // debug.debug
    lua_getglobal(L, "debug");
    lua_pushcfunction(L, luaV_debug);
    lua_setfield(L, -2, "debug");
    lua_برداشت(L, 1);
    // free
    lua_pushlightuserdata(L, (تهی *) LUAVIM_FREE);
    lua_pushcfunction(L, luaV_free);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // luaeval
    lua_pushlightuserdata(L, (تهی *) LUAVIM_LUAEVAL);
    lua_pushcfunction(L, luaV_luaeval);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // setref
    lua_pushlightuserdata(L, (تهی *) LUAVIM_SETREF);
    lua_pushcfunction(L, luaV_setref);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // register
    luaV_newmetatable(L, LUAVIM_LIST);
    luaV_register(L, luaV_List_mt);
    lua_برداشت(L, 1);
    luaV_newmetatable(L, LUAVIM_DICT);
    luaV_register(L, luaV_Dict_mt);
    lua_برداشت(L, 1);
    luaV_newmetatable(L, LUAVIM_BLOB);
    luaV_register(L, luaV_Blob_mt);
    lua_برداشت(L, 1);
    luaV_newmetatable(L, LUAVIM_FUNCREF);
    luaV_register(L, luaV_Funcref_mt);
    lua_برداشت(L, 1);
    luaV_newmetatable(L, LUAVIM_BUFFER);
    luaV_register(L, luaV_Buffer_mt);
    lua_برداشت(L, 1);
    luaV_newmetatable(L, LUAVIM_WINDOW);
    luaV_register(L, luaV_Window_mt);
    lua_برداشت(L, 1);
    lua_newtable(L); // vim table
    luaV_register(L, luaV_module);
    luaV_pushversion(L);
    lua_setfield(L, -2, "lua_version");
    lua_setglobal(L, LUAVIM_NAME);
    // custom code
    (تهی)luaL_dostring(L, LUA_VIM_FN_CODE);
    (تهی)luaL_dostring(L, LUA_VIM_UPDATE_PACKAGE_PATHS);
    (تهی)luaL_dostring(L, LUA_VIM_SETUP_VARIABLE_DICTS);

    lua_getglobal(L, LUAVIM_NAME);
    lua_getfield(L, -1, "_update_package_paths");

    اگر (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);

    بازگشت 0;
}

    ایستا lua_State *
luaV_newstate(تهی)
{
    lua_State *L = luaL_newstate();
# اگر LUA_VERSION_رقم >= 505
    luaL_openselectedlibs(L, ~0, 0); // core libs
# وگرنه
    luaL_openlibs(L); // core libs
# endif
    lua_pushcfunction(L, luaopen_vim); // vim
    lua_call(L, 0, 0);
    بازگشت L;
}

    ایستا تهی
luaV_setrange(lua_State *L, صحیح line1, صحیح line2)
{
    lua_getglobal(L, LUAVIM_NAME);
    lua_pushinteger(L, line1);
    lua_setfield(L, -2, "firstline");
    lua_pushinteger(L, line2);
    lua_setfield(L, -2, "lastline");
    lua_برداشت(L, 1); // vim table
}


// =======   Interface   =======

ایستا lua_State *L = پوچ;

    ایستا صحیح
lua_isopen(تهی)
{
    بازگشت L != پوچ;
}

    ایستا صحیح
lua_init(تهی)
{
    اگر (lua_isopen())
	بازگشت OK;

# ifdef DYNAMIC_LUA
    اگر (!lua_enabled(TRUE))
    {
	emsg(_("Lua library cannot be loaded."));
	بازگشت FAIL;
    }
# endif
    L = luaV_newstate();

    بازگشت OK;
}

    تهی
lua_end(تهی)
{
    اگر (!lua_isopen())
	بازگشت;

    lua_close(L);
    L = پوچ;
}

/*
 * ex commands
 */
    تهی
ex_lua(exarg_T *eap)
{
    نویسه *script = (نویسه *)script_get(eap, eap->arg);

    اگر (!eap->skip && lua_init() == OK)
    {
	نویسه *s = script != پوچ ? script : (نویسه *)eap->arg;

	luaV_setrange(L, eap->line1, eap->line2);
	اگر (luaL_loadbuffer(L, s, strlen(s), LUAVIM_CHUNKNAME)
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
    اگر (script != پوچ)
	vim_free(script);
}

    تهی
ex_luado(exarg_T *eap)
{
    linenr_T l;
    ثابت نویسه *s = (ثابت نویسه *) eap->arg;
    luaL_Buffer b;
    size_t len;
    buf_T *was_curbuf = curbuf;

    اگر (lua_init() == FAIL) بازگشت;
    اگر (u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
    {
	emsg(_("cannot save undo information"));
	بازگشت;
    }
    luaV_setrange(L, eap->line1, eap->line2);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, "return function(line, linenr) ", 30); // header
    luaL_addlstring(&b, s, strlen(s));
    luaL_addlstring(&b, " end", 4); // footer
    luaL_pushresult(&b);
    s = lua_tolstring(L, -1, &len);
    اگر (luaL_loadbuffer(L, s, len, LUAVIM_CHUNKNAME))
    {
	luaV_emsg(L);
	lua_برداشت(L, 1); // function body
	بازگشت;
    }
    lua_call(L, 0, 1);
    lua_replace(L, -2); // function -> body
    برای (l = eap->line1; l <= eap->line2; l++)
    {
	// Check the line number, the command may have deleted lines.
	اگر (l > curbuf->b_ml.ml_سطر_شمار)
	    بشکن;

	lua_pushvalue(L, -1); // function
	luaV_pushline(L, curbuf, l); // current line as arg
	lua_pushinteger(L, l); // current line number as arg
	اگر (lua_pcall(L, 2, 1, 0))
	{
	    luaV_emsg(L);
	    بشکن;
	}

	// Catch the command switching to another buffer.
	// Check the line number, the command may have deleted lines.
	اگر (curbuf != was_curbuf || l > curbuf->b_ml.ml_سطر_شمار)
	    بشکن;

	اگر (lua_isstring(L, -1)) // update line?
	{
# ifdef HAVE_SANDBOX
	    luaV_checksandbox(L);
# endif
	    ml_replace(l, luaV_toline(L, -1), TRUE);
	    changed_bytes(l, 0);
	    lua_برداشت(L, 1); // result from luaV_toline
	}
	lua_برداشت(L, 1); //سطر
    }
    lua_برداشت(L, 1); // function
    check_cursor();
    update_screen(UPD_NOT_VALID);
}

    تهی
ex_luafile(exarg_T *eap)
{
    اگر (lua_init() == FAIL)
	بازگشت;
    اگر (!eap->skip)
    {
	luaV_setrange(L, eap->line1, eap->line2);
	اگر (luaL_loadfile(L, (نویسه *) eap->arg) || lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
}

# define luaV_freetype(typ,tname) \
	تهی \
    lua_##tname##_free(typ *o) \
    { \
	اگر (!lua_isopen()) بازگشت; \
	luaV_getfield(L, LUAVIM_FREE); \
	lua_pushlightuserdata(L, (تهی *) o); \
	lua_call(L, 1, 0); \
    }

luaV_freetype(buf_T, buffer)
luaV_freetype(win_T, window)

    تهی
do_luaeval(char_u *str, typval_T *arg, typval_T *rettv)
{
    lua_init();
    luaV_getfield(L, LUAVIM_LUAEVAL);
    lua_pushstring(L, (نویسه *) str);
    lua_pushlightuserdata(L, (تهی *) arg);
    lua_pushlightuserdata(L, (تهی *) rettv);
    lua_call(L, 3, 0);
}

    صحیح
set_ref_in_lua(صحیح copyID)
{
    صحیح aborted = 0;

    اگر (!lua_isopen())
	بازگشت 0;

    luaV_getfield(L, LUAVIM_SETREF);
    // call the function with 1 arg, getting 1 result back
    lua_pushinteger(L, copyID);
    lua_call(L, 1, 1);
    // get the result
    aborted = lua_tointeger(L, -1);
    // pop result off the stack
    lua_برداشت(L, 1);

    بازگشت aborted;
}

    تهی
update_package_paths_in_lua(تهی)
{
    اگر (!lua_isopen())
	بازگشت;

    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "_update_package_paths");

    اگر (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);
}

/*
 * Native C function callback
 */
    ایستا صحیح
luaV_call_lua_func(
	صحیح	 argcount,
	typval_T *argvars,
	typval_T *rettv,
	تهی	 *state)
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

    برای (i = 0; i < argcount; ++i)
	luaV_pushtypval(funcstate->L, &argvars[i]);

    اگر (lua_pcall(funcstate->L, luaargcount, 1, 0))
    {
	luaV_emsg(funcstate->L);
	بازگشت (صحیح)FCERR_OTHER;
    }

    luaV_checktypval(funcstate->L, -1, rettv, "get return value");
    بازگشت (صحیح)FCERR_NONE;
}

/*
 * Free up any lua references held by the func state.
 */
    ایستا تهی
luaV_call_lua_func_free(تهی *state)
{
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_funcref);
    funcstate->L = پوچ;
    اگر (funcstate->lua_tableref != LUA_NOREF)
	luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    VIM_CLEAR(funcstate);
}

#endif
