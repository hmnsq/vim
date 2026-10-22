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

#ಅನ್ತರ್ಭಾವ "vim.h"
#ಅನ್ತರ್ಭಾವ "version.h"

#ಅನ್ತರ್ಭಾವ <lua.h>
#ಅನ್ತರ್ಭಾವ <lualib.h>
#ಅನ್ತರ್ಭಾವ <lauxlib.h>

#ಯದಿ __STDC_VERSION__ >= 199901L
# define LUAV_INLINE ಅನ್ತಃಸ್ಥ
#ಅನ್ಯಥಾ
# define LUAV_INLINE
#endif

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#ಯದಿ defined(FEAT_LUA)

# define LUAVIM_CHUNKNAME "vim chunk"
# define LUAVIM_NAME "vim"
# define LUAVIM_EVALNAME "luaeval"
# define LUAVIM_EVALHEADER "local _A=select(1,...) return "

# ifdef LUA_RELEASE
#  define LUAVIM_VERSION LUA_RELEASE
# ಅನ್ಯಥಾ
#  define LUAVIM_VERSION LUA_VERSION
# endif

ಪ್ರಕಾರನಿರ್ದೇಶ buf_T *luaV_Buffer;
ಪ್ರಕಾರನಿರ್ದೇಶ win_T *luaV_Window;
ಪ್ರಕಾರನಿರ್ದೇಶ dict_T *luaV_Dict;
ಪ್ರಕಾರನಿರ್ದೇಶ list_T *luaV_List;
ಪ್ರಕಾರನಿರ್ದೇಶ blob_T *luaV_Blob;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    char_u	*name;	// funcref
    dict_T	*self;	// selfdict
} luaV_Funcref;
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*msgfunc_T)(ವರ್ಣ *);

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಪೂರ್ಣಾಙ್ಕ lua_funcref;    // ref to a lua func
    ಪೂರ್ಣಾಙ್ಕ lua_tableref;   // ref to a lua table if metatable else LUA_NOREF. used
			// for __call
    lua_State *L;
} luaV_CFuncState;

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ LUAVIM_DICT[] = "dict";
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ LUAVIM_LIST[] = "list";
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ LUAVIM_BLOB[] = "blob";
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ LUAVIM_FUNCREF[] = "funcref";
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ LUAVIM_BUFFER[] = "buffer";
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ LUAVIM_WINDOW[] = "window";
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ LUAVIM_FREE[] = "luaV_free";
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ LUAVIM_LUAEVAL[] = "luaV_luaeval";
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ LUAVIM_SETREF[] = "luaV_setref";

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ LUA___CALL[] = "__call";

// get/setudata manage references to vim userdata in a cache table through
// object pointers (light userdata). The cache table itself is retrieved
// from the registry.

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ LUAVIM_UDATA_CACHE[] = "luaV_udata_cache";

# define luaV_getfield(L, s) \
    lua_pushlightuserdata((L), (ಶೂನ್ಯ *)(s)); \
    lua_rawget((L), LUA_REGISTRYINDEX)
# define luaV_checksandbox(L) \
    ಯದಿ (sandbox) luaL_error((L), "not allowed in sandbox")
# define lua_V_ಸಂದೇಶ(L) luaV_msgfunc((L), (msgfunc_T) ಸಂದೇಶ)
# define luaV_emsg(L) luaV_msgfunc((L), (msgfunc_T) emsg)
# define luaV_checktypval(L, a, v, ಸಂದೇಶ) \
    ಕುರು { \
	ಯದಿ (luaV_totypval(L, a, v) == FAIL) \
	    luaL_error(L, ಸಂದೇಶ ": cannot convert value"); \
    } ಯಾವತ್ (0)

ಸ್ಥಿರ luaV_List *luaV_pushlist(lua_State *L, list_T *lis);
ಸ್ಥಿರ luaV_Dict *luaV_pushdict(lua_State *L, dict_T *dic);
ಸ್ಥಿರ luaV_Blob *luaV_pushblob(lua_State *L, blob_T *blo);
ಸ್ಥಿರ luaV_Funcref *luaV_pushfuncref(lua_State *L, char_u *name);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ luaV_call_lua_func(ಪೂರ್ಣಾಙ್ಕ argcount, typval_T *argvars, typval_T *rettv, ಶೂನ್ಯ *state);
ಸ್ಥಿರ ಶೂನ್ಯ luaV_call_lua_func_free(ಶೂನ್ಯ *state);

# ಯದಿ LUA_VERSION_ಅಂಕೆ <= 501
#  define luaV_register(L, l) luaL_register(L, ರಿಕ್ತ, l)
#  define luaL_typeerror luaL_typerror
# ಅನ್ಯಥಾ
#  define luaV_register(L, l) luaL_setfuncs(L, l, 0)
# endif

# ifdef DYNAMIC_LUA

#  ifdef MSWIN
#   define load_dll vimLoadLib
#   define ಚಿಹ್ನೆ_from_dll GetProcAddress
#   define close_dll FreeLibrary
#   define load_dll_error GetWin32Error
#  ಅನ್ಯಥಾ
#   include <dlfcn.h>
#   define HANDLE ಶೂನ್ಯ*
#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#   define ಚಿಹ್ನೆ_from_dll dlsym
#   define close_dll dlclose
#   define load_dll_error dlerror
#  endif

// lauxlib
#  ಯದಿ LUA_VERSION_ಅಂಕೆ <= 501
#   define luaL_register dll_luaL_register
#   define luaL_prepbuffer dll_luaL_prepbuffer
#   define luaL_openlib dll_luaL_openlib
#   define luaL_typerror dll_luaL_typerror
#   define luaL_loadfile dll_luaL_loadfile
#   define luaL_loadbuffer dll_luaL_loadbuffer
#  ಅನ್ಯಥಾ
#   define luaL_prepbuffsize dll_luaL_prepbuffsize
#   define luaL_setfuncs dll_luaL_setfuncs
#   define luaL_loadfilex dll_luaL_loadfilex
#   define luaL_loadbufferx dll_luaL_loadbufferx
#   define luaL_argerror dll_luaL_argerror
#  endif
#  ಯದಿ LUA_VERSION_ಅಂಕೆ >= 504
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
#  ಯದಿ LUA_VERSION_ಅಂಕೆ <= 501
#   define lua_tonumber dll_lua_tonumber
#   define lua_tointeger dll_lua_tointeger
#   define lua_call dll_lua_call
#   define lua_pcall dll_lua_pcall
#  ಅನ್ಯಥಾ
#   define lua_tonumberx dll_lua_tonumberx
#   define lua_tointegerx dll_lua_tointegerx
#   define lua_callk dll_lua_callk
#   define lua_pcallk dll_lua_pcallk
#   define lua_getglobal dll_lua_getglobal
#   define lua_setglobal dll_lua_setglobal
#  endif
#  ಯದಿ LUA_VERSION_ಅಂಕೆ <= 502
#   define lua_replace dll_lua_replace
#   define lua_remove dll_lua_remove
#  endif
#  ಯದಿ LUA_VERSION_ಅಂಕೆ >= 503
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
#  ಯದಿ LUA_VERSION_ಅಂಕೆ >= 504
#   define lua_newuserdatauv dll_lua_newuserdatauv
#  ಅನ್ಯಥಾ
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
#  ಯದಿ LUA_VERSION_ಅಂಕೆ >= 505
#   define luaL_openselectedlibs dll_luaL_openselectedlibs
#  ಅನ್ಯಥಾ
#   define luaL_openlibs dll_luaL_openlibs
#  endif

// lauxlib
#  ಯದಿ LUA_VERSION_ಅಂಕೆ <= 501
ಶೂನ್ಯ (*dll_luaL_register) (lua_State *L, ಧ್ರುವ ವರ್ಣ *libname, ಧ್ರುವ luaL_Reg *l);
ವರ್ಣ *(*dll_luaL_prepbuffer) (luaL_Buffer *B);
ಶೂನ್ಯ (*dll_luaL_openlib) (lua_State *L, ಧ್ರುವ ವರ್ಣ *libname, ಧ್ರುವ luaL_Reg *l, ಪೂರ್ಣಾಙ್ಕ nup);
ಪೂರ್ಣಾಙ್ಕ (*dll_luaL_typerror) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ narg, ಧ್ರುವ ವರ್ಣ *tname);
ಪೂರ್ಣಾಙ್ಕ (*dll_luaL_loadfile) (lua_State *L, ಧ್ರುವ ವರ್ಣ *filename);
ಪೂರ್ಣಾಙ್ಕ (*dll_luaL_loadbuffer) (lua_State *L, ಧ್ರುವ ವರ್ಣ *buff, size_t sz, ಧ್ರುವ ವರ್ಣ *name);
#  ಅನ್ಯಥಾ
ವರ್ಣ *(*dll_luaL_prepbuffsize) (luaL_Buffer *B, size_t sz);
ಶೂನ್ಯ (*dll_luaL_setfuncs) (lua_State *L, ಧ್ರುವ luaL_Reg *l, ಪೂರ್ಣಾಙ್ಕ nup);
ಪೂರ್ಣಾಙ್ಕ (*dll_luaL_loadfilex) (lua_State *L, ಧ್ರುವ ವರ್ಣ *filename, ಧ್ರುವ ವರ್ಣ *mode);
ಪೂರ್ಣಾಙ್ಕ (*dll_luaL_loadbufferx) (lua_State *L, ಧ್ರುವ ವರ್ಣ *buff, size_t sz, ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ವರ್ಣ *mode);
ಪೂರ್ಣಾಙ್ಕ (*dll_luaL_argerror) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ numarg, ಧ್ರುವ ವರ್ಣ *extramsg);
#  endif
#  ಯದಿ LUA_VERSION_ಅಂಕೆ >= 504
ಪೂರ್ಣಾಙ್ಕ (*dll_luaL_typeerror) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ narg, ಧ್ರುವ ವರ್ಣ *tname);
#  endif
ಶೂನ್ಯ (*dll_luaL_checkany) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ narg);
ಧ್ರುವ ವರ್ಣ *(*dll_luaL_checklstring) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_Arg, size_t *l);
lua_Integer (*dll_luaL_checkinteger) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_Arg);
lua_Integer (*dll_luaL_optinteger) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ nArg, lua_Integer def);
ಶೂನ್ಯ (*dll_luaL_checktype) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ narg, ಪೂರ್ಣಾಙ್ಕ t);
ಪೂರ್ಣಾಙ್ಕ (*dll_luaL_error) (lua_State *L, ಧ್ರುವ ವರ್ಣ *fmt, ...);
lua_State *(*dll_luaL_newstate) (ಶೂನ್ಯ);
ಶೂನ್ಯ (*dll_luaL_buffinit) (lua_State *L, luaL_Buffer *B);
ಶೂನ್ಯ (*dll_luaL_addlstring) (luaL_Buffer *B, ಧ್ರುವ ವರ್ಣ *s, size_t l);
ಶೂನ್ಯ (*dll_luaL_pushresult) (luaL_Buffer *B);
ಪೂರ್ಣಾಙ್ಕ (*dll_luaL_loadstring) (lua_State *L, ಧ್ರುವ ವರ್ಣ *s);
ಪೂರ್ಣಾಙ್ಕ (*dll_luaL_ref) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx);
#  ಯದಿ LUA_VERSION_ಅಂಕೆ <= 502
ಶೂನ್ಯ (*dll_luaL_unref) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ n);
#  ಅನ್ಯಥಾ
ಶೂನ್ಯ (*dll_luaL_unref) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx, lua_Integer n);
#  endif
// lua
#  ಯದಿ LUA_VERSION_ಅಂಕೆ <= 501
lua_ಸಂಖ್ಯೆ (*dll_lua_tonumber) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx);
lua_Integer (*dll_lua_tointeger) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx);
ಶೂನ್ಯ (*dll_lua_call) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ nargs, ಪೂರ್ಣಾಙ್ಕ nresults);
ಪೂರ್ಣಾಙ್ಕ (*dll_lua_pcall) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ nargs, ಪೂರ್ಣಾಙ್ಕ nresults, ಪೂರ್ಣಾಙ್ಕ errfunc);
#  ಅನ್ಯಥಾ
lua_ಸಂಖ್ಯೆ (*dll_lua_tonumberx) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ *isnum);
lua_Integer (*dll_lua_tointegerx) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ *isnum);
ಶೂನ್ಯ (*dll_lua_callk) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ nargs, ಪೂರ್ಣಾಙ್ಕ nresults, ಪೂರ್ಣಾಙ್ಕ ctx,
	lua_CFunction k);
ಪೂರ್ಣಾಙ್ಕ (*dll_lua_pcallk) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ nargs, ಪೂರ್ಣಾಙ್ಕ nresults, ಪೂರ್ಣಾಙ್ಕ errfunc,
	ಪೂರ್ಣಾಙ್ಕ ctx, lua_CFunction k);
ಶೂನ್ಯ (*dll_lua_getglobal) (lua_State *L, ಧ್ರುವ ವರ್ಣ *var);
ಶೂನ್ಯ (*dll_lua_setglobal) (lua_State *L, ಧ್ರುವ ವರ್ಣ *var);
#  endif
#  ಯದಿ LUA_VERSION_ಅಂಕೆ <= 502
ಶೂನ್ಯ (*dll_lua_replace) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx);
ಶೂನ್ಯ (*dll_lua_remove) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx);
#  endif
#  ಯದಿ LUA_VERSION_ಅಂಕೆ >= 503
ಶೂನ್ಯ  (*dll_lua_rotate) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ n);
ಶೂನ್ಯ (*dll_lua_copy) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ fromidx, ಪೂರ್ಣಾಙ್ಕ toidx);
#  endif
ಧ್ರುವ ವರ್ಣ *(*dll_lua_typename) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ tp);
ಶೂನ್ಯ       (*dll_lua_close) (lua_State *L);
ಪೂರ್ಣಾಙ್ಕ (*dll_lua_gettop) (lua_State *L);
ಶೂನ್ಯ (*dll_lua_settop) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx);
ಶೂನ್ಯ (*dll_lua_pushvalue) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx);
ಪೂರ್ಣಾಙ್ಕ (*dll_lua_isnumber) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx);
ಪೂರ್ಣಾಙ್ಕ (*dll_lua_isstring) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx);
ಪೂರ್ಣಾಙ್ಕ (*dll_lua_type) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx);
ಪೂರ್ಣಾಙ್ಕ (*dll_lua_rawequal) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx1, ಪೂರ್ಣಾಙ್ಕ idx2);
ಪೂರ್ಣಾಙ್ಕ (*dll_lua_toboolean) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx);
ಧ್ರುವ ವರ್ಣ *(*dll_lua_tolstring) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx, size_t *len);
ಶೂನ್ಯ *(*dll_lua_touserdata) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx);
ಶೂನ್ಯ (*dll_lua_pushnil) (lua_State *L);
ಶೂನ್ಯ (*dll_lua_pushnumber) (lua_State *L, lua_ಸಂಖ್ಯೆ n);
ಶೂನ್ಯ (*dll_lua_pushinteger) (lua_State *L, lua_Integer n);
ಶೂನ್ಯ (*dll_lua_pushlstring) (lua_State *L, ಧ್ರುವ ವರ್ಣ *s, size_t l);
ಶೂನ್ಯ (*dll_lua_pushstring) (lua_State *L, ಧ್ರುವ ವರ್ಣ *s);
ಧ್ರುವ ವರ್ಣ *(*dll_lua_pushfstring) (lua_State *L, ಧ್ರುವ ವರ್ಣ *fmt, ...);
ಶೂನ್ಯ (*dll_lua_pushcclosure) (lua_State *L, lua_CFunction fn, ಪೂರ್ಣಾಙ್ಕ n);
ಶೂನ್ಯ (*dll_lua_pushboolean) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ b);
ಶೂನ್ಯ (*dll_lua_pushlightuserdata) (lua_State *L, ಶೂನ್ಯ *p);
ಶೂನ್ಯ (*dll_lua_getfield) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx, ಧ್ರುವ ವರ್ಣ *k);
#  ಯದಿ LUA_VERSION_ಅಂಕೆ <= 502
ಶೂನ್ಯ (*dll_lua_rawget) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx);
ಶೂನ್ಯ (*dll_lua_rawgeti) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ n);
#  ಅನ್ಯಥಾ
ಪೂರ್ಣಾಙ್ಕ (*dll_lua_rawget) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx);
ಪೂರ್ಣಾಙ್ಕ (*dll_lua_rawgeti) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx, lua_Integer n);
#  endif
ಶೂನ್ಯ (*dll_lua_createtable) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ narr, ಪೂರ್ಣಾಙ್ಕ nrec);
ಶೂನ್ಯ (*dll_lua_settable) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx);
#  ಯದಿ LUA_VERSION_ಅಂಕೆ >= 504
ಶೂನ್ಯ *(*dll_lua_newuserdatauv) (lua_State *L, size_t sz, ಪೂರ್ಣಾಙ್ಕ nuvalue);
#  ಅನ್ಯಥಾ
ಶೂನ್ಯ *(*dll_lua_newuserdata) (lua_State *L, size_t sz);
#  endif
ಪೂರ್ಣಾಙ್ಕ (*dll_lua_getmetatable) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ objindex);
ಶೂನ್ಯ (*dll_lua_setfield) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx, ಧ್ರುವ ವರ್ಣ *k);
ಶೂನ್ಯ (*dll_lua_rawset) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx);
#  ಯದಿ LUA_VERSION_ಅಂಕೆ <= 502
ಶೂನ್ಯ (*dll_lua_rawseti) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx, ಪೂರ್ಣಾಙ್ಕ n);
#  ಅನ್ಯಥಾ
ಶೂನ್ಯ (*dll_lua_rawseti) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx, lua_Integer n);
#  endif
ಪೂರ್ಣಾಙ್ಕ (*dll_lua_setmetatable) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ objindex);
ಪೂರ್ಣಾಙ್ಕ (*dll_lua_next) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ idx);
// libs
ಪೂರ್ಣಾಙ್ಕ (*dll_luaopen_base) (lua_State *L);
ಪೂರ್ಣಾಙ್ಕ (*dll_luaopen_table) (lua_State *L);
ಪೂರ್ಣಾಙ್ಕ (*dll_luaopen_string) (lua_State *L);
ಪೂರ್ಣಾಙ್ಕ (*dll_luaopen_math) (lua_State *L);
ಪೂರ್ಣಾಙ್ಕ (*dll_luaopen_io) (lua_State *L);
ಪೂರ್ಣಾಙ್ಕ (*dll_luaopen_os) (lua_State *L);
ಪೂರ್ಣಾಙ್ಕ (*dll_luaopen_package) (lua_State *L);
ಪೂರ್ಣಾಙ್ಕ (*dll_luaopen_debug) (lua_State *L);
#  ಯದಿ LUA_VERSION_ಅಂಕೆ >= 505
ಶೂನ್ಯ (*dll_luaL_openselectedlibs) (lua_State *L, ಪೂರ್ಣಾಙ್ಕ load, ಪೂರ್ಣಾಙ್ಕ preload);
#  ಅನ್ಯಥಾ
ಶೂನ್ಯ (*dll_luaL_openlibs) (lua_State *L);
#  endif

ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ **luaV_function;
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಧ್ರುವ ವರ್ಣ *name;
    luaV_function func;
} luaV_Reg;

ಸ್ಥಿರ ಧ್ರುವ luaV_Reg luaV_dll[] = {
    // lauxlib
#  ಯದಿ LUA_VERSION_ಅಂಕೆ <= 501
    {"luaL_register", (luaV_function) &dll_luaL_register},
    {"luaL_prepbuffer", (luaV_function) &dll_luaL_prepbuffer},
    {"luaL_openlib", (luaV_function) &dll_luaL_openlib},
    {"luaL_typerror", (luaV_function) &dll_luaL_typerror},
    {"luaL_loadfile", (luaV_function) &dll_luaL_loadfile},
    {"luaL_loadbuffer", (luaV_function) &dll_luaL_loadbuffer},
#  ಅನ್ಯಥಾ
    {"luaL_prepbuffsize", (luaV_function) &dll_luaL_prepbuffsize},
    {"luaL_setfuncs", (luaV_function) &dll_luaL_setfuncs},
    {"luaL_loadfilex", (luaV_function) &dll_luaL_loadfilex},
    {"luaL_loadbufferx", (luaV_function) &dll_luaL_loadbufferx},
    {"luaL_argerror", (luaV_function) &dll_luaL_argerror},
#  endif
#  ಯದಿ LUA_VERSION_ಅಂಕೆ >= 504
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
#  ಯದಿ LUA_VERSION_ಅಂಕೆ <= 501
    {"lua_tonumber", (luaV_function) &dll_lua_tonumber},
    {"lua_tointeger", (luaV_function) &dll_lua_tointeger},
    {"lua_call", (luaV_function) &dll_lua_call},
    {"lua_pcall", (luaV_function) &dll_lua_pcall},
#  ಅನ್ಯಥಾ
    {"lua_tonumberx", (luaV_function) &dll_lua_tonumberx},
    {"lua_tointegerx", (luaV_function) &dll_lua_tointegerx},
    {"lua_callk", (luaV_function) &dll_lua_callk},
    {"lua_pcallk", (luaV_function) &dll_lua_pcallk},
    {"lua_getglobal", (luaV_function) &dll_lua_getglobal},
    {"lua_setglobal", (luaV_function) &dll_lua_setglobal},
#  endif
#  ಯದಿ LUA_VERSION_ಅಂಕೆ <= 502
    {"lua_replace", (luaV_function) &dll_lua_replace},
    {"lua_remove", (luaV_function) &dll_lua_remove},
#  endif
#  ಯದಿ LUA_VERSION_ಅಂಕೆ >= 503
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
#  ಯದಿ LUA_VERSION_ಅಂಕೆ >= 504
    {"lua_newuserdatauv", (luaV_function) &dll_lua_newuserdatauv},
#  ಅನ್ಯಥಾ
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
#  ಯದಿ LUA_VERSION_ಅಂಕೆ >= 505
    {"luaL_openselectedlibs", (luaV_function) &dll_luaL_openselectedlibs},
#  ಅನ್ಯಥಾ
    {"luaL_openlibs", (luaV_function) &dll_luaL_openlibs},
#  endif
    {ರಿಕ್ತ, ರಿಕ್ತ}
};

ಸ್ಥಿರ HANDLE hinstLua = ರಿಕ್ತ;

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
lua_link_init(ವರ್ಣ *libname, ಪೂರ್ಣಾಙ್ಕ verbose)
{
    ಧ್ರುವ luaV_Reg *reg;
    ಯದಿ (hinstLua) ಪ್ರತ್ಯಾಗಮ OK;
    hinstLua = load_dll(libname);
    ಯದಿ (!hinstLua)
    {
	ಯದಿ (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತಿ (reg = luaV_dll; reg->func; reg++)
    {
	ಯದಿ ((*reg->func = ಚಿಹ್ನೆ_from_dll(hinstLua, reg->name)) == ರಿಕ್ತ)
	{
	    close_dll(hinstLua);
	    hinstLua = 0;
	    ಯದಿ (verbose)
		semsg(_(e_could_not_load_library_function_str), reg->name);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}
# endif // DYNAMIC_LUA

# ಯದಿ defined(DYNAMIC_LUA)
    ಪೂರ್ಣಾಙ್ಕ
lua_enabled(ಪೂರ್ಣಾಙ್ಕ verbose)
{
    ಪ್ರತ್ಯಾಗಮ lua_link_init((ವರ್ಣ *)p_luadll, verbose) == OK;
}
# endif

# ಯದಿ LUA_VERSION_ಅಂಕೆ > 501 && LUA_VERSION_ಅಂಕೆ < 504
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaL_typeerror(lua_State *L, ಪೂರ್ಣಾಙ್ಕ narg, ಧ್ರುವ ವರ್ಣ *tname)
{
    ಧ್ರುವ ವರ್ಣ *ಸಂದೇಶ = lua_pushfstring(L, "%s expected, got %s",
	    tname, luaL_typename(L, narg));
    ಪ್ರತ್ಯಾಗಮ luaL_argerror(L, narg, ಸಂದೇಶ);
}
# endif

    ಸ್ಥಿರ LUAV_INLINE ಶೂನ್ಯ
luaV_getudata(lua_State *L, ಶೂನ್ಯ *v)
{
    lua_pushlightuserdata(L, (ಶೂನ್ಯ *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // now the cache table is at the top of the stack
    lua_pushlightuserdata(L, v);
    lua_rawget(L, -2);
    lua_remove(L, -2);  // remove the cache table from the stack
}

    ಸ್ಥಿರ LUAV_INLINE ಶೂನ್ಯ
luaV_setudata(lua_State *L, ಶೂನ್ಯ *v)
{
    lua_pushlightuserdata(L, (ಶೂನ್ಯ *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // cache table is at -1
    lua_pushlightuserdata(L, v);       // ...now at -2
    lua_pushvalue(L, -3);	       // copy the userdata (cache at -3)
    lua_rawset(L, -3);		       // consumes two stack items
    lua_ತೆಗೆ(L, 1);		       // and remove the cache table
}

// =======   Internal   =======

    ಸ್ಥಿರ ಶೂನ್ಯ
luaV_newmetatable(lua_State *L, ಧ್ರುವ ವರ್ಣ *tname)
{
    lua_newtable(L);
    lua_pushlightuserdata(L, (ಶೂನ್ಯ *) tname);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
}

    ಸ್ಥಿರ ಶೂನ್ಯ *
luaV_toudata(lua_State *L, ಪೂರ್ಣಾಙ್ಕ ud, ಧ್ರುವ ವರ್ಣ *tname)
{
    ಶೂನ್ಯ *p = lua_touserdata(L, ud);

    ಯದಿ (p == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // value is userdata
    ಯದಿ (lua_getmetatable(L, ud)) // does it have a metatable?
    {
	luaV_getfield(L, tname); // get metatable
	ಯದಿ (lua_rawequal(L, -1, -2)) // MTs match?
	{
	    lua_ತೆಗೆ(L, 2); // MTs
	    ಪ್ರತ್ಯಾಗಮ p;
	}
    }

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ *
luaV_checkcache(lua_State *L, ಶೂನ್ಯ *p)
{
    luaV_getudata(L, p);
    ಯದಿ (lua_isnil(L, -1)) luaL_error(L, "invalid object");
    lua_ತೆಗೆ(L, 1);
    ಪ್ರತ್ಯಾಗಮ p;
}

# define luaV_unbox(L,luatyp,ud) (*((luatyp *) lua_touserdata((L),(ud))))

# define luaV_checkvalid(L,luatyp,ud) \
    luaV_checkcache((L), (ಶೂನ್ಯ *) luaV_unbox((L),luatyp,(ud)))

    ಸ್ಥಿರ ಶೂನ್ಯ *
luaV_checkudata(lua_State *L, ಪೂರ್ಣಾಙ್ಕ ud, ಧ್ರುವ ವರ್ಣ *tname)
{
    ಶೂನ್ಯ *p = luaV_toudata(L, ud, tname);
    ಯದಿ (p == ರಿಕ್ತ) luaL_typeerror(L, ud, tname);
    ಪ್ರತ್ಯಾಗಮ p;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
luaV_pushtypval(lua_State *L, typval_T *tv)
{
    ಯದಿ (tv == ರಿಕ್ತ)
    {
	lua_pushnil(L);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ವಿಕಲ್ಪ (tv->v_type)
    {
	ಪಕ್ಷ VAR_STRING:
	    lua_pushstring(L, tv->vval.v_string == ರಿಕ್ತ
					    ? "" : (ವರ್ಣ *)tv->vval.v_string);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	    lua_pushinteger(L, (ಪೂರ್ಣಾಙ್ಕ) tv->vval.v_ಸಂಖ್ಯೆ);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_FLOAT:
	    lua_pushnumber(L, (lua_ಸಂಖ್ಯೆ) tv->vval.v_float);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_LIST:
	    luaV_pushlist(L, tv->vval.v_list);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_DICT:
	    luaV_pushdict(L, tv->vval.v_dict);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_BOOL:
	ಪಕ್ಷ VAR_SPECIAL:
	    ಯದಿ (tv->vval.v_ಸಂಖ್ಯೆ <= VVAL_TRUE)
		lua_pushboolean(L, (ಪೂರ್ಣಾಙ್ಕ) tv->vval.v_ಸಂಖ್ಯೆ);
	    ಅನ್ಯಥಾ
		lua_pushnil(L);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_FUNC:
	    luaV_pushfuncref(L, tv->vval.v_string);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_PARTIAL:
	    // TODO: handle partial arguments
	    luaV_pushfuncref(L, partial_name(tv->vval.v_partial));
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_BLOB:
	    luaV_pushblob(L, tv->vval.v_blob);
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    lua_pushnil(L);
    }
}

/*
 * Converts lua value at 'pos' to typval 'tv'.
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_totypval(lua_State *L, ಪೂರ್ಣಾಙ್ಕ pos, typval_T *tv)
{
    ಪೂರ್ಣಾಙ್ಕ status = OK;

    tv->v_lock = 0;

    ವಿಕಲ್ಪ (lua_type(L, pos))
    {
	ಪಕ್ಷ LUA_TBOOLEAN:
	    tv->v_type = VAR_BOOL;
	    tv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T) lua_toboolean(L, pos);
	    ಭಙ್ಗ;
	ಪಕ್ಷ LUA_TNIL:
	    tv->v_type = VAR_SPECIAL;
	    tv->vval.v_ಸಂಖ್ಯೆ = VVAL_NULL;
	    ಭಙ್ಗ;
	ಪಕ್ಷ LUA_TSTRING:
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strsave((char_u *) lua_tostring(L, pos));
	    ಭಙ್ಗ;
	ಪಕ್ಷ LUA_TNUMBER:
	{
	    ಧ್ರುವ lua_ಸಂಖ್ಯೆ n = lua_tonumber(L, pos);

	    ಯದಿ (n > (lua_ಸಂಖ್ಯೆ)INT64_MAX || n < (lua_ಸಂಖ್ಯೆ)INT64_MIN
		    || ((lua_ಸಂಖ್ಯೆ)((varnumber_T)n)) != n)
	    {
		tv->v_type = VAR_FLOAT;
		tv->vval.v_float = (float_T)n;
	    }
	    ಅನ್ಯಥಾ
	    {
		tv->v_type = VAR_ಸಂಖ್ಯೆ;
		tv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)n;
	    }
	}
	    ಭಙ್ಗ;
	ಪಕ್ಷ LUA_TFUNCTION:
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
	    ಭಙ್ಗ;
	}
	ಪಕ್ಷ LUA_TTABLE:
	{
	    ಪೂರ್ಣಾಙ್ಕ lua_tableref;

	    lua_pushvalue(L, pos);
	    lua_tableref = luaL_ref(L, LUA_REGISTRYINDEX);
	    ಯದಿ (lua_getmetatable(L, pos))
	    {
		lua_getfield(L, -1, LUA___CALL);
		ಯದಿ (lua_isfunction(L, -1))
		{
		    char_u *name;
		    ಪೂರ್ಣಾಙ್ಕ lua_funcref = luaL_ref(L, LUA_REGISTRYINDEX);
		    luaV_CFuncState *state = ALLOC_CLEAR_ONE(luaV_CFuncState);

		    state->lua_funcref = lua_funcref;
		    state->L = L;
		    state->lua_tableref = lua_tableref;
		    name = register_cfunc(&luaV_call_lua_func,
					      &luaV_call_lua_func_free, state);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(name);
		    ಭಙ್ಗ;
		}
	    }
	    tv->v_type = VAR_ಸಂಖ್ಯೆ;
	    tv->vval.v_ಸಂಖ್ಯೆ = 0;
	    status = FAIL;
	    ಭಙ್ಗ;
	}
	ಪಕ್ಷ LUA_TUSERDATA:
	{
	    ಶೂನ್ಯ *p = lua_touserdata(L, pos);

	    ಯದಿ (lua_getmetatable(L, pos)) // has metatable?
	    {
		// check list
		luaV_getfield(L, LUAVIM_LIST);
		ಯದಿ (lua_rawequal(L, -1, -2))
		{
		    tv->v_type = VAR_LIST;
		    tv->vval.v_list = *((luaV_List *) p);
		    ++tv->vval.v_list->lv_refcount;
		    lua_ತೆಗೆ(L, 2); // MTs
		    ಭಙ್ಗ;
		}
		// check dict
		luaV_getfield(L, LUAVIM_DICT);
		ಯದಿ (lua_rawequal(L, -1, -3))
		{
		    tv->v_type = VAR_DICT;
		    tv->vval.v_dict = *((luaV_Dict *) p);
		    ++tv->vval.v_dict->dv_refcount;
		    lua_ತೆಗೆ(L, 3); // MTs
		    ಭಙ್ಗ;
		}
		// check blob
		luaV_getfield(L, LUAVIM_BLOB);
		ಯದಿ (lua_rawequal(L, -1, -4))
		{
		    tv->v_type = VAR_BLOB;
		    tv->vval.v_blob = *((luaV_Blob *) p);
		    ++tv->vval.v_blob->bv_refcount;
		    lua_ತೆಗೆ(L, 4); // MTs
		    ಭಙ್ಗ;
		}
		// check funcref
		luaV_getfield(L, LUAVIM_FUNCREF);
		ಯದಿ (lua_rawequal(L, -1, -5))
		{
		    luaV_Funcref *f = (luaV_Funcref *) p;

		    func_ref(f->name);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(f->name);
		    lua_ತೆಗೆ(L, 5); // MTs
		    ಭಙ್ಗ;
		}
		lua_ತೆಗೆ(L, 4); // MTs
	    }
	}
	// FALLTHROUGH
	ಸ್ವಭಾವ:
	    tv->v_type = VAR_ಸಂಖ್ಯೆ;
	    tv->vval.v_ಸಂಖ್ಯೆ = 0;
	    status = FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ status;
}

/*
 * similar to luaL_addlstring, but replaces \0 with \n if toline and
 * \n with \0 otherwise
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
luaV_addlstring(luaL_Buffer *b, ಧ್ರುವ ವರ್ಣ *s, size_t l, ಪೂರ್ಣಾಙ್ಕ toline)
{
    ಯಾವತ್ (l--)
    {
	ಯದಿ (*s == '\0' && toline)
	    luaL_addchar(b, '\n');
	ಅನ್ಯಥಾ ಯದಿ (*s == '\n' && !toline)
	    luaL_addchar(b, '\0');
	ಅನ್ಯಥಾ
	    luaL_addchar(b, *s);
	s++;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
luaV_pushline(lua_State *L, buf_T *buf, linenr_T n)
{
    ಧ್ರುವ ವರ್ಣ *s = (ಧ್ರುವ ವರ್ಣ *) ml_get_buf(buf, n, FALSE);
    luaL_Buffer b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, strlen(s), 0);
    luaL_pushresult(&b);
}

    ಸ್ಥಿರ char_u *
luaV_toline(lua_State *L, ಪೂರ್ಣಾಙ್ಕ pos)
{
    size_t l;
    ಧ್ರುವ ವರ್ಣ *s = lua_tolstring(L, pos, &l);

    luaL_Buffer b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 1);
    luaL_pushresult(&b);
    ಪ್ರತ್ಯಾಗಮ (char_u *) lua_tostring(L, -1);
}

/*
 * pops a string s from the top of the stack and calls mf(t) for pieces t of
 * s separated by newlines
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
luaV_msgfunc(lua_State *L, msgfunc_T mf)
{
    luaL_Buffer b;
    size_t l;
    ಧ್ರುವ ವರ್ಣ *p, *s = lua_tolstring(L, -1, &l);
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 0);
    luaL_pushresult(&b);
    // break string
    p = s = lua_tolstring(L, -1, &l);
    ಯಾವತ್ (l--)
    {
	ಯದಿ (*p++ == '\0') // break?
	{
	    mf((ವರ್ಣ *)s);
	    s = p;
	}
    }
    mf((ವರ್ಣ *)s);
    lua_ತೆಗೆ(L, 2); // original and modified strings
}

# define luaV_newtype(typ,tname,luatyp,luatname) \
	ಸ್ಥಿರ luatyp * \
    luaV_new##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = (luatyp *) lua_newuserdata(L, ಪರಿಮಾಣ(luatyp)); \
	*o = obj; \
	luaV_setudata(L, obj); /* cache[obj] = udata */ \
	luaV_getfield(L, luatname); \
	lua_setmetatable(L, -2); \
	ಪ್ರತ್ಯಾಗಮ o; \
    }

# define luaV_pushtype(typ,tname,luatyp) \
	ಸ್ಥಿರ luatyp * \
    lua_V_ತಳ್ಳು##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = ರಿಕ್ತ; \
	ಯದಿ (obj == ರಿಕ್ತ) \
	    lua_pushnil(L); \
	ಅನ್ಯಥಾ \
	{ \
	    luaV_getudata(L, obj); \
	    ಯದಿ (lua_isnil(L, -1)) /* not interned? */ \
	    { \
		lua_ತೆಗೆ(L, 1); \
		o = luaV_new##tname(L, obj); \
	    } \
	    ಅನ್ಯಥಾ \
		o = (luatyp *) lua_touserdata(L, -1); \
	} \
	ಪ್ರತ್ಯಾಗಮ o; \
    }

# define luaV_type_tostring(tname,luatname) \
	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ \
    luaV_##tname##_tostring(lua_State *L) \
    { \
	lua_pushfstring(L, "%s: %p", luatname, lua_touserdata(L, 1)); \
	ಪ್ರತ್ಯಾಗಮ 1; \
    }

// =======   List type   =======

    ಸ್ಥಿರ luaV_List *
luaV_newlist(lua_State *L, list_T *lis)
{
    luaV_List *l = (luaV_List *) lua_newuserdata(L, ಪರಿಮಾಣ(luaV_List));
    *l = lis;
    lis->lv_refcount++; // reference in Lua
    luaV_setudata(L, lis); // cache[lis] = udata
    luaV_getfield(L, LUAVIM_LIST);
    lua_setmetatable(L, -2);
    ಪ್ರತ್ಯಾಗಮ l;
}

luaV_pushtype(list_T, list, luaV_List)
luaV_type_tostring(list, LUAVIM_LIST)

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_list_len(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushinteger(L, (ಪೂರ್ಣಾಙ್ಕ) list_len(l));
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_list_iter(lua_State *L)
{
    listitem_T *li = (listitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    ಯದಿ (li == ರಿಕ್ತ) ಪ್ರತ್ಯಾಗಮ 0;
    luaV_pushtypval(L, &li->li_tv);
    lua_pushlightuserdata(L, (ಶೂನ್ಯ *) li->li_next);
    lua_replace(L, lua_upvalueindex(1));
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_list_call(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushlightuserdata(L, (ಶೂನ್ಯ *) l->lv_first);
    lua_pushcclosure(L, luaV_list_iter, 1);
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_list_index(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    ಯದಿ (lua_isnumber(L, 2)) // list item?
    {
	long n = (long) luaL_checkinteger(L, 2);
	listitem_T *li;

	// Lua array index starts with 1 while Vim uses 0, subtract 1 to
	// normalize.
	n -= 1;
	li = list_find(l, n);
	ಯದಿ (li == ರಿಕ್ತ)
	    lua_pushnil(L);
	ಅನ್ಯಥಾ
	    luaV_pushtypval(L, &li->li_tv);
    }
    ಅನ್ಯಥಾ ಯದಿ (lua_isstring(L, 2)) // method?
    {
	ಧ್ರುವ ವರ್ಣ *s = lua_tostring(L, 2);
	ಯದಿ (strncmp(s, "add", 3) == 0
		|| strncmp(s, "insert", 6) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	ಅನ್ಯಥಾ
	    lua_pushnil(L);
    }
    ಅನ್ಯಥಾ
	lua_pushnil(L);
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_list_newindex(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    long n = (long) luaL_checkinteger(L, 2);
    listitem_T *li;

    // Lua array index starts with 1 while Vim uses 0, subtract 1 to normalize.
    n -= 1;

    ಯದಿ (l->lv_lock)
	luaL_error(L, "list is locked");
    li = list_find(l, n);
    ಯದಿ (li == ರಿಕ್ತ)
    {
	ಯದಿ (!lua_isnil(L, 3))
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "inserting list item");
	    ಯದಿ (list_ಸೇರಿಸು_tv(l, &v, li) == FAIL)
		luaL_error(L, "failed to add item to list");
	    clear_tv(&v);
	}
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (lua_isnil(L, 3)) // remove?
	{
	    vimlist_remove(l, li, li);
	    listitem_free(l, li);
	}
	ಅನ್ಯಥಾ
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "setting list item");
	    clear_tv(&li->li_tv);
	    li->li_tv = v;
	}
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_list_add(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (ಶೂನ್ಯ *) *lis);
    typval_T v;
    ಯದಿ (l->lv_lock)
	luaL_error(L, "list is locked");
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "adding list item");
    ಯದಿ (list_append_tv(l, &v) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
lua_V_list_ಸೇರಿಸು(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (ಶೂನ್ಯ *) *lis);
    long pos = (long) luaL_optinteger(L, 3, 0);
    listitem_T *li = ರಿಕ್ತ;
    typval_T v;
    ಯದಿ (l->lv_lock)
	luaL_error(L, "list is locked");
    ಯದಿ (pos < l->lv_len)
    {
	li = list_find(l, pos);
	ಯದಿ (li == ರಿಕ್ತ)
	    luaL_error(L, "invalid position");
    }
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "inserting list item");
    ಯದಿ (list_ಸೇರಿಸು_tv(l, &v, li) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಧ್ರುವ luaL_Reg luaV_List_mt[] = {
    {"__tostring", luaV_list_tostring},
    {"__len", luaV_list_len},
    {"__call", luaV_list_call},
    {"__index", luaV_list_index},
    {"__newindex", luaV_list_newindex},
    {"add", luaV_list_add},
    {"insert", lua_V_list_ಸೇರಿಸು},
    {ರಿಕ್ತ, ರಿಕ್ತ}
};


// =======   Dict type   =======

    ಸ್ಥಿರ luaV_Dict *
luaV_newdict(lua_State *L, dict_T *dic)
{
    luaV_Dict *d = (luaV_Dict *) lua_newuserdata(L, ಪರಿಮಾಣ(luaV_Dict));
    *d = dic;
    dic->dv_refcount++; // reference in Lua
    luaV_setudata(L, dic); // cache[dic] = udata
    luaV_getfield(L, LUAVIM_DICT);
    lua_setmetatable(L, -2);
    ಪ್ರತ್ಯಾಗಮ d;
}

luaV_pushtype(dict_T, dict, luaV_Dict)
luaV_type_tostring(dict, LUAVIM_DICT)

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_dict_len(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    lua_pushinteger(L, (ಪೂರ್ಣಾಙ್ಕ) dict_len(d));
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_dict_iter(lua_State *L UNUSED)
{
# ifdef FEAT_EVAL
    hashitem_T *hi = (hashitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    ಪೂರ್ಣಾಙ್ಕ n = lua_tointeger(L, lua_upvalueindex(2));
    dictitem_T *di;
    ಯದಿ (n <= 0) ಪ್ರತ್ಯಾಗಮ 0;
    ಯಾವತ್ (HASHITEM_EMPTY(hi)) hi++;
    di = dict_lookup(hi);
    lua_pushstring(L, (ವರ್ಣ *) hi->hi_ಕೀಲಿ);
    luaV_pushtypval(L, &di->di_tv);
    lua_pushlightuserdata(L, (ಶೂನ್ಯ *) (hi + 1));
    lua_replace(L, lua_upvalueindex(1));
    lua_pushinteger(L, n - 1);
    lua_replace(L, lua_upvalueindex(2));
    ಪ್ರತ್ಯಾಗಮ 2;
# ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ 0;
# endif
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_dict_call(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    hashtab_T *ht = &d->dv_hashtab;
    lua_pushlightuserdata(L, (ಶೂನ್ಯ *) ht->ht_array);
    lua_pushinteger(L, ht->ht_used); // # remaining items
    lua_pushcclosure(L, luaV_dict_iter, 2);
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_dict_index(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *ಕೀಲಿ = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di = dict_find(d, ಕೀಲಿ, -1);

    ಯದಿ (di == ರಿಕ್ತ)
    {
	lua_pushnil(L);
	ಪ್ರತ್ಯಾಗಮ 1;
    }

    luaV_pushtypval(L, &di->di_tv);
    ಯದಿ (di->di_tv.v_type == VAR_FUNC) // funcref?
    {
	luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, -1);
	f->self = d; // keep "self" reference
	d->dv_refcount++;
    }

    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_dict_newindex(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *ಕೀಲಿ = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di;
    typval_T tv;

    ಯದಿ (d->dv_lock)
	luaL_error(L, "dict is locked");
    ಯದಿ (ಕೀಲಿ == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;
    ಯದಿ (*ಕೀಲಿ == NUL)
	luaL_error(L, "empty key");
    ಯದಿ (!lua_isnil(L, 3)) // read value?
    {
	luaV_checktypval(L, 3, &tv, "setting dict item");
	ಯದಿ (d->dv_scope == VAR_DEF_SCOPE && tv.v_type == VAR_FUNC)
	{
	    clear_tv(&tv);
	    luaL_error(L, "cannot assign funcref to builtin scope");
	}
    }
    di = dict_find(d, ಕೀಲಿ, -1);
    ಯದಿ (di == ರಿಕ್ತ) // non-existing key?
    {
	ಯದಿ (lua_isnil(L, 3))
	    ಪ್ರತ್ಯಾಗಮ 0;
	di = dictitem_alloc(ಕೀಲಿ);
	ಯದಿ (di == ರಿಕ್ತ)
	{
	    clear_tv(&tv);
	    ಪ್ರತ್ಯಾಗಮ 0;
	}
	ಯದಿ (dict_add(d, di) == FAIL)
	{
	    vim_free(di);
	    clear_tv(&tv);
	    ಪ್ರತ್ಯಾಗಮ 0;
	}
    }
    ಅನ್ಯಥಾ
	clear_tv(&di->di_tv);
    ಯದಿ (lua_isnil(L, 3)) // remove?
    {
	hashitem_T *hi = hash_find(&d->dv_hashtab, di->di_ಕೀಲಿ);
	hash_remove(&d->dv_hashtab, hi, "Lua new index");
	dictitem_free(di);
    }
    ಅನ್ಯಥಾ
	di->di_tv = tv;
    ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಧ್ರುವ luaL_Reg luaV_Dict_mt[] = {
    {"__tostring", luaV_dict_tostring},
    {"__len", luaV_dict_len},
    {"__call", luaV_dict_call},
    {"__index", luaV_dict_index},
    {"__newindex", luaV_dict_newindex},
    {ರಿಕ್ತ, ರಿಕ್ತ}
};


// =======   Blob type   =======

    ಸ್ಥಿರ luaV_Blob *
luaV_newblob(lua_State *L, blob_T *blo)
{
    luaV_Blob *b = (luaV_Blob *) lua_newuserdata(L, ಪರಿಮಾಣ(luaV_Blob));
    *b = blo;
    blo->bv_refcount++; // reference in Lua
    luaV_setudata(L, blo); // cache[blo] = udata
    luaV_getfield(L, LUAVIM_BLOB);
    lua_setmetatable(L, -2);
    ಪ್ರತ್ಯಾಗಮ b;
}

luaV_pushtype(blob_T, blob, luaV_Blob)
luaV_type_tostring(blob, LUAVIM_BLOB)

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_blob_gc(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    blob_unref(b);
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_blob_len(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    lua_pushinteger(L, (ಪೂರ್ಣಾಙ್ಕ) blob_len(b));
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_blob_index(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    ಯದಿ (lua_isnumber(L, 2))
    {
	ಪೂರ್ಣಾಙ್ಕ idx = luaL_checkinteger(L, 2);
	ಯದಿ (idx < 0)
	    // Negative index is relative to the end.
	    idx += (ಪೂರ್ಣಾಙ್ಕ) blob_len(b);
	ಯದಿ (idx >= 0 && idx < blob_len(b))
	    lua_pushnumber(L, (lua_ಸಂಖ್ಯೆ) blob_get(b, idx));
	ಅನ್ಯಥಾ
	    lua_pushnil(L);
    }
    ಅನ್ಯಥಾ ಯದಿ (lua_isstring(L, 2))
    {
	ಧ್ರುವ ವರ್ಣ *s = lua_tostring(L, 2);
	ಯದಿ (strncmp(s, "add", 3) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	ಅನ್ಯಥಾ
	    lua_pushnil(L);
    }
    ಅನ್ಯಥಾ
	lua_pushnil(L);
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_blob_newindex(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    ಯದಿ (b->bv_lock)
	luaL_error(L, "blob is locked");

    ಯದಿ (!lua_isnumber(L, 2))
	ಪ್ರತ್ಯಾಗಮ 0;

    long len = blob_len(b);
    ಪೂರ್ಣಾಙ್ಕ idx = luaL_checkinteger(L, 2);
    ಪೂರ್ಣಾಙ್ಕ val = luaL_checkinteger(L, 3);
    ಯದಿ (idx < 0)
	// Negative index is relative to the end.
	idx += (ಪೂರ್ಣಾಙ್ಕ) len;
    ಯದಿ ((idx >= 0 && idx < len)
	    || (idx == len && ga_grow(&b->bv_ga, 1) == OK))
    {
	blob_set(b, idx, (char_u) val);
	ಯದಿ (idx == len)
	    ++b->bv_ga.ga_len;
    }
    ಅನ್ಯಥಾ
	luaL_error(L, "index out of range");

    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_blob_add(lua_State *L)
{
    luaV_Blob *blo = luaV_checkudata(L, 1, LUAVIM_BLOB);
    blob_T *b = (blob_T *) luaV_checkcache(L, (ಶೂನ್ಯ *) *blo);
    ಯದಿ (b->bv_lock)
	luaL_error(L, "blob is locked");
    lua_settop(L, 2);
    ಯದಿ (!lua_isstring(L, 2))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 2));
    ಅನ್ಯಥಾ
    {
	size_t i, l = 0;
	ಧ್ರುವ ವರ್ಣ *s = lua_tolstring(L, 2, &l);

	ಯದಿ (ga_grow(&b->bv_ga, (ಪೂರ್ಣಾಙ್ಕ)l) == OK)
	    ಪ್ರತಿ (i = 0; i < l; ++i)
		ga_append(&b->bv_ga, s[i]);
    }
    lua_settop(L, 1);
    ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಧ್ರುವ luaL_Reg luaV_Blob_mt[] = {
    {"__tostring", luaV_blob_tostring},
    {"__gc", luaV_blob_gc},
    {"__len", luaV_blob_len},
    {"__index", luaV_blob_index},
    {"__newindex", luaV_blob_newindex},
    {"add", luaV_blob_add},
    {ರಿಕ್ತ, ರಿಕ್ತ}
};


// =======   Funcref type   =======

    ಸ್ಥಿರ luaV_Funcref *
luaV_newfuncref(lua_State *L, char_u *name)
{
    luaV_Funcref *f = (luaV_Funcref *)lua_newuserdata(L, ಪರಿಮಾಣ(luaV_Funcref));

    ಯದಿ (name != ರಿಕ್ತ)
    {
	func_ref(name);
	f->name = vim_strsave(name);
    }
    f->self = ರಿಕ್ತ;
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_setmetatable(L, -2);
    ಪ್ರತ್ಯಾಗಮ f;
}

    ಸ್ಥಿರ luaV_Funcref *
luaV_pushfuncref(lua_State *L, char_u *name)
{
    ಪ್ರತ್ಯಾಗಮ luaV_newfuncref(L, name);
}


luaV_type_tostring(funcref, LUAVIM_FUNCREF)

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_funcref_gc(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    func_unref(f->name);
    vim_free(f->name);
    // NOTE: Don't call "dict_unref(f->self)", because the dict of "f->self"
    // will be (or has been already) freed by Vim's garbage collection.
    ಪ್ರತ್ಯಾಗಮ 0;
}

// equivalent to string(funcref)
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_funcref_len(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    lua_pushstring(L, (ಧ್ರುವ ವರ್ಣ *) f->name);
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_funcref_call(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);
    ಪೂರ್ಣಾಙ್ಕ i, n = lua_gettop(L) - 1; // #args
    ಪೂರ್ಣಾಙ್ಕ status = FAIL;
    typval_T args;
    typval_T rettv;

    args.v_type = VAR_LIST;
    args.vval.v_list = list_alloc();
    rettv.v_type = VAR_UNKNOWN; // as in clear_tv
    ಯದಿ (args.vval.v_list != ರಿಕ್ತ)
    {
	typval_T v;

	ಪ್ರತಿ (i = 0; i < n; i++)
	{
	    luaV_checktypval(L, i + 2, &v, "calling funcref");
	    list_append_tv(args.vval.v_list, &v);
	    clear_tv(&v);
	}
	status = func_call(f->name, &args, ರಿಕ್ತ, f->self, &rettv);
	ಯದಿ (status == OK)
	    luaV_pushtypval(L, &rettv);
	clear_tv(&args);
	clear_tv(&rettv);
    }
    ಯದಿ (status != OK)
	luaL_error(L, "cannot call funcref");
    ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಧ್ರುವ luaL_Reg luaV_Funcref_mt[] = {
    {"__tostring", luaV_funcref_tostring},
    {"__gc", luaV_funcref_gc},
    {"__len", luaV_funcref_len},
    {"__call", luaV_funcref_call},
    {ರಿಕ್ತ, ರಿಕ್ತ}
};


// =======   Buffer type   =======

luaV_newtype(buf_T, buffer, luaV_Buffer, LUAVIM_BUFFER)
luaV_pushtype(buf_T, buffer, luaV_Buffer)
luaV_type_tostring(buffer, LUAVIM_BUFFER)

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_buffer_len(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    lua_pushinteger(L, b->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_buffer_call(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    lua_settop(L, 1);
    set_curbuf(b, DOBUF_SPLIT);
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_buffer_index(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    linenr_T n = (linenr_T) lua_tointeger(L, 2);
    ಯದಿ (n > 0 && n <= b->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	luaV_pushline(L, b, n);
    ಅನ್ಯಥಾ ಯದಿ (lua_isstring(L, 2))
    {
	ಧ್ರುವ ವರ್ಣ *s = lua_tostring(L, 2);
	ಯದಿ (strncmp(s, "name", 4) == 0)
	    lua_pushstring(L, (b->b_sfname == ರಿಕ್ತ)
					? "" : (ವರ್ಣ *) b->b_sfname);
	ಅನ್ಯಥಾ ಯದಿ (strncmp(s, "fname", 5) == 0)
	    lua_pushstring(L, (b->b_ffname == ರಿಕ್ತ)
					? "" : (ವರ್ಣ *) b->b_ffname);
	ಅನ್ಯಥಾ ಯದಿ (strncmp(s, "number", 6) == 0)
	    lua_pushinteger(L, b->b_fnum);
	// methods
	ಅನ್ಯಥಾ ಯದಿ (strncmp(s,   "insert", 6) == 0
		|| strncmp(s, "next", 4) == 0
		|| strncmp(s, "previous", 8) == 0
		|| strncmp(s, "isvalid", 7) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	ಅನ್ಯಥಾ
	    lua_pushnil(L);
    }
    ಅನ್ಯಥಾ
	lua_pushnil(L);
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_buffer_newindex(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    linenr_T n = (linenr_T) luaL_checkinteger(L, 2);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    ಯದಿ (n < 1 || n > b->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	luaL_error(L, "invalid line number");
    ಯದಿ (lua_isnil(L, 3)) // delete line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	ಯದಿ (u_savedel(n, 1L) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	ಅನ್ಯಥಾ ಯದಿ (ml_delete(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot delete line");
	}
	ಅನ್ಯಥಾ
	{
	    deleted_ಸಾಲುಗಳು_mark(n, 1L);
	    ಯದಿ (b == curwin->w_buffer) // fix cursor in current window?
	    {
		ಯದಿ (curwin->w_cursor.lnum >= n)
		{
		    ಯದಿ (curwin->w_cursor.lnum > n)
		    {
			curwin->w_cursor.lnum -= 1;
			check_cursor_col();
		    }
		    ಅನ್ಯಥಾ
			check_cursor();
		    changed_cline_bef_curs();
		}
		invalidate_botline();
	    }
	}
	curbuf = buf;
    }
    ಅನ್ಯಥಾ ಯದಿ (lua_isstring(L, 3)) // update line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	ಯದಿ (u_savesub(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	ಅನ್ಯಥಾ ಯದಿ (ml_replace(n, luaV_toline(L, 3), TRUE) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot replace line");
	}
	ಅನ್ಯಥಾ
	    changed_bytes(n, 0);
	curbuf = buf;
	ಯದಿ (b == curwin->w_buffer)
	    check_cursor_col();
    }
    ಅನ್ಯಥಾ
	luaL_error(L, "wrong argument to change line");
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
lua_V_buffer_ಸೇರಿಸು(lua_State *L)
{
    luaV_Buffer *lb = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *b = (buf_T *) luaV_checkcache(L, (ಶೂನ್ಯ *) *lb);
    linenr_T last = b->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    linenr_T n = (linenr_T) luaL_optinteger(L, 3, last);
    buf_T *buf;
    luaL_checktype(L, 2, LUA_TSTRING);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    // fix insertion line
    ಯದಿ (n < 0) n = 0;
    ಯದಿ (n > last) n = last;
    // insert
    buf = curbuf;
    curbuf = b;
    ಯದಿ (u_save(n, n + 1) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot save undo information");
    }
    ಅನ್ಯಥಾ ಯದಿ (ml_append(n, luaV_toline(L, 2), 0, FALSE) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot insert line");
    }
    ಅನ್ಯಥಾ
	appended_ಸಾಲುಗಳು_mark(n, 1L);
    curbuf = buf;
    update_screen(UPD_VALID);
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_buffer_next(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (ಶೂನ್ಯ *) *b);
    luaV_pushbuffer(L, buf->b_next);
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_buffer_previous(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (ಶೂನ್ಯ *) *b);
    luaV_pushbuffer(L, buf->b_prev);
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_buffer_isvalid(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    luaV_getudata(L, *b);
    lua_pushboolean(L, !lua_isnil(L, -1));
    ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಧ್ರುವ luaL_Reg luaV_Buffer_mt[] = {
    {"__tostring", luaV_buffer_tostring},
    {"__len", luaV_buffer_len},
    {"__call", luaV_buffer_call},
    {"__index", luaV_buffer_index},
    {"__newindex", luaV_buffer_newindex},
    {"insert", lua_V_buffer_ಸೇರಿಸು},
    {"next", luaV_buffer_next},
    {"previous", luaV_buffer_previous},
    {"isvalid", luaV_buffer_isvalid},
    {ರಿಕ್ತ, ರಿಕ್ತ}
};


// =======   Window type   =======

luaV_newtype(win_T, window, luaV_Window, LUAVIM_WINDOW)
luaV_pushtype(win_T, window, luaV_Window)
luaV_type_tostring(window, LUAVIM_WINDOW)

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_window_call(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    lua_settop(L, 1);
    win_goto(w);
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_window_index(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    ಧ್ರುವ ವರ್ಣ *s = luaL_checkstring(L, 2);
    ಯದಿ (strncmp(s, "buffer", 6) == 0)
	luaV_pushbuffer(L, w->w_buffer);
    ಅನ್ಯಥಾ ಯದಿ (strncmp(s, "line", 4) == 0)
	lua_pushinteger(L, w->w_cursor.lnum);
    ಅನ್ಯಥಾ ಯದಿ (strncmp(s, "col", 3) == 0)
	lua_pushinteger(L, w->w_cursor.col + 1);
    ಅನ್ಯಥಾ ಯದಿ (strncmp(s, "width", 5) == 0)
	lua_pushinteger(L, w->w_width);
    ಅನ್ಯಥಾ ಯದಿ (strncmp(s, "height", 6) == 0)
	lua_pushinteger(L, w->w_height);
    // methods
    ಅನ್ಯಥಾ ಯದಿ (strncmp(s,   "next", 4) == 0
	    || strncmp(s, "previous", 8) == 0
	    || strncmp(s, "isvalid", 7) == 0)
    {
	lua_getmetatable(L, 1);
	lua_getfield(L, -1, s);
    }
    ಅನ್ಯಥಾ
	lua_pushnil(L);
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_window_newindex(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    ಧ್ರುವ ವರ್ಣ *s = luaL_checkstring(L, 2);
    ಪೂರ್ಣಾಙ್ಕ v = luaL_checkinteger(L, 3);
    ಯದಿ (strncmp(s, "line", 4) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	ಯದಿ (v < 1 || v > w->w_buffer->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    luaL_error(L, "line out of range");
	w->w_cursor.lnum = v;
	update_screen(UPD_VALID);
    }
    ಅನ್ಯಥಾ ಯದಿ (strncmp(s, "col", 3) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	w->w_cursor.col = v - 1;
	w->w_set_curswant = true;
	update_screen(UPD_VALID);
    }
    ಅನ್ಯಥಾ ಯದಿ (strncmp(s, "width", 5) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setwidth(v);
	curwin = win;
    }
    ಅನ್ಯಥಾ ಯದಿ (strncmp(s, "height", 6) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setheight(v);
	curwin = win;
    }
    ಅನ್ಯಥಾ
	luaL_error(L, "invalid window property: `%s'", s);
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_window_next(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    win_T *win = (win_T *) luaV_checkcache(L, (ಶೂನ್ಯ *) *w);
    luaV_pushwindow(L, win->w_next);
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_window_previous(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    win_T *win = (win_T *) luaV_checkcache(L, (ಶೂನ್ಯ *) *w);
    luaV_pushwindow(L, win->w_prev);
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_window_isvalid(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    luaV_getudata(L, *w);
    lua_pushboolean(L, !lua_isnil(L, -1));
    ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಧ್ರುವ luaL_Reg luaV_Window_mt[] = {
    {"__tostring", luaV_window_tostring},
    {"__call", luaV_window_call},
    {"__index", luaV_window_index},
    {"__newindex", luaV_window_newindex},
    {"next", luaV_window_next},
    {"previous", luaV_window_previous},
    {"isvalid", luaV_window_isvalid},
    {ರಿಕ್ತ, ರಿಕ್ತ}
};


// =======   Vim module   =======

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_print(lua_State *L)
{
    ಪೂರ್ಣಾಙ್ಕ		i, n = lua_gettop(L); // nargs
    ಧ್ರುವ ವರ್ಣ	*s;
    size_t	l;
    garray_T	ಸಂದೇಶ_ga;

    ga_init2(&ಸಂದೇಶ_ga, 1, 128);
    lua_getglobal(L, "tostring");
    ಪ್ರತಿ (i = 1; i <= n; i++)
    {
	lua_pushvalue(L, -1); // tostring
	lua_pushvalue(L, i); // arg
	lua_call(L, 1, 1);
	s = lua_tolstring(L, -1, &l);
	ಯದಿ (s == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ luaL_error(L, "cannot convert to string");
	ಯದಿ (i > 1)
	    ga_append(&ಸಂದೇಶ_ga, ' '); // use space instead of tab
	ga_concat_len(&ಸಂದೇಶ_ga, (char_u *)s, l);
	lua_ತೆಗೆ(L, 1);
    }
    // Replace any "\n" with "\0"
    ಪ್ರತಿ (i = 0; i < ಸಂದೇಶ_ga.ga_len; i++)
	ಯದಿ (((ವರ್ಣ *)ಸಂದೇಶ_ga.ga_data)[i] == '\n')
	    ((ವರ್ಣ *)ಸಂದೇಶ_ga.ga_data)[i] = '\0';
    lua_pushlstring(L, ಸಂದೇಶ_ga.ga_data, ಸಂದೇಶ_ga.ga_len);
    ಯದಿ (!got_int)
	lua_V_ಸಂದೇಶ(L);

    ga_clear(&ಸಂದೇಶ_ga);
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_debug(lua_State *L)
{
    lua_settop(L, 0);
    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "eval");
    lua_remove(L, -2); // vim.eval at position 1
    ಪ್ರತಿ (;;)
    {
	ಧ್ರುವ ವರ್ಣ *ಒಳಹರಿವು;
	size_t l;
	lua_pushvalue(L, 1); // vim.eval
	lua_pushliteral(L, "input('lua_debug> ')");
	lua_call(L, 1, 1); // return string
	ಒಳಹರಿವು = lua_tolstring(L, -1, &l);
	ಯದಿ (l == 0 || strcmp(ಒಳಹರಿವು, "cont") == 0)
	    ಪ್ರತ್ಯಾಗಮ 0;
	ಸಂದೇಶ_putchar('\n'); // avoid outputting on input line
	ಯದಿ (luaL_loadbuffer(L, ಒಳಹರಿವು, l, "=(debug command)")
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
	lua_settop(L, 1); // remove eventual returns, but keep vim.eval
    }
}

    ಸ್ಥಿರ dict_T *
luaV_get_var_scope(lua_State *L)
{
    ಧ್ರುವ ವರ್ಣ	*scope = luaL_checkstring(L, 1);
    dict_T	*dict = ರಿಕ್ತ;

    ಯದಿ (STRICMP((ವರ್ಣ *)scope, "g") == 0)
	dict = get_globvar_dict();
    ಅನ್ಯಥಾ ಯದಿ (STRICMP((ವರ್ಣ *)scope, "v") == 0)
	dict = get_vimvar_dict();
    ಅನ್ಯಥಾ ಯದಿ (STRICMP((ವರ್ಣ *)scope, "b") == 0)
	dict = curbuf->b_vars;
    ಅನ್ಯಥಾ ಯದಿ (STRICMP((ವರ್ಣ *)scope, "w") == 0)
	dict = curwin->w_vars;
    ಅನ್ಯಥಾ ಯದಿ (STRICMP((ವರ್ಣ *)scope, "t") == 0)
	dict = curtab->tp_vars;
    ಅನ್ಯಥಾ
    {
	luaL_error(L, "invalid scope %s", scope);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ dict;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_setvar(lua_State *L)
{
    dict_T	*dict;
    dictitem_T	*di;
    size_t	len;
    ವರ್ಣ	*name;
    ಪೂರ್ಣಾಙ್ಕ		del;
    ವರ್ಣ	*error = ರಿಕ್ತ;

    name = (ವರ್ಣ *)luaL_checklstring(L, 3, &len);
    del = (lua_gettop(L) < 4) || lua_isnil(L, 4);

    dict = luaV_get_var_scope(L);
    ಯದಿ (dict == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    di = dict_find(dict, (char_u *)name, (ಪೂರ್ಣಾಙ್ಕ)len);
    ಯದಿ (di != ರಿಕ್ತ)
    {
	ಯದಿ (di->di_flags & DI_FLAGS_RO)
	    error = "variable is read-only";
	ಅನ್ಯಥಾ ಯದಿ (di->di_flags & DI_FLAGS_LOCK)
	    error = "variable is locked";
	ಅನ್ಯಥಾ ಯದಿ (del && di->di_flags & DI_FLAGS_FIX)
	    error = "variable is fixed";
	ಯದಿ (error != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ luaL_error(L, error);
    }
    ಅನ್ಯಥಾ ಯದಿ (dict->dv_lock)
	ಪ್ರತ್ಯಾಗಮ luaL_error(L, "Dictionary is locked");

    ಯದಿ (del)
    {
	// Delete the key
	ಯದಿ (di == ರಿಕ್ತ)
	    // Doesn't exist, nothing to do
	    ಪ್ರತ್ಯಾಗಮ 0;
	// Delete the entry
	dictitem_remove(dict, di, "Lua delete variable");
    }
    ಅನ್ಯಥಾ
    {
	// Update the key
	typval_T	tv;

	// Convert the lua value to a Vim script type in the temporary variable
	lua_pushvalue(L, 4);
	ಯದಿ (luaV_totypval(L, -1, &tv) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ luaL_error(L, "Couldn't convert lua value");

	ಯದಿ (di == ರಿಕ್ತ)
	{
	    // Need to create an entry
	    di = dictitem_alloc((char_u *)name);
	    ಯದಿ (di == ರಿಕ್ತ)
	    {
		clear_tv(&tv);
		ಪ್ರತ್ಯಾಗಮ 0;
	    }
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	    ಯದಿ (dict_add(dict, di) == FAIL)
	    {
		dictitem_free(di);
		clear_tv(&tv);
		ಪ್ರತ್ಯಾಗಮ luaL_error(L, "Couldn't add to dictionary");
	    }
	}
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ type_error = FALSE;
	    ಯದಿ (dict == get_vimvar_dict()
	       && !before_set_vvar((char_u *)name, di, &tv, TRUE, &type_error))
	    {
		clear_tv(&tv);
		ಯದಿ (type_error)
		    ಪ್ರತ್ಯಾಗಮ luaL_error(L,
				"Setting v:%s to value with wrong type", name);
		ಪ್ರತ್ಯಾಗಮ 0;
	    }
	    // Clear the old value
	    clear_tv(&di->di_tv);
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	}

	// Clear the temporary variable
	clear_tv(&tv);
    }

    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_getvar(lua_State *L)
{
    dict_T	*dict = luaV_get_var_scope(L);
    size_t	len;
    ಧ್ರುವ ವರ್ಣ	*name = luaL_checklstring(L, 3, &len);
    dictitem_T	*di = dict_find(dict, (char_u *)name, (ಪೂರ್ಣಾಙ್ಕ)len);

    ಯದಿ (di == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;  // nil

    luaV_pushtypval(L, &di->di_tv);
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_command(lua_State *L)
{
    char_u  *s = vim_strsave((char_u *)luaL_checkstring(L, 1));

    execute_cmds_from_string(s);
    vim_free(s);
    update_screen(UPD_VALID);
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_eval(lua_State *L)
{
    typval_T *tv = eval_ಅಭಿವ್ಯಕ್ತಿ((char_u *) luaL_checkstring(L, 1), ರಿಕ್ತ);

    ಯದಿ (tv == ರಿಕ್ತ) luaL_error(L, "invalid expression");
    luaV_pushtypval(L, tv);
    free_tv(tv);
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_beep(lua_State *L UNUSED)
{
    vim_beep(BO_LANG);
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
lua_V_ಸಾಲು(lua_State *L)
{
    luaV_pushline(L, curbuf, curwin->w_cursor.lnum);
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_list(lua_State *L)
{
    list_T *l;
    ಪೂರ್ಣಾಙ್ಕ initarg = !lua_isnoneornil(L, 1);

    ಯದಿ (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    l = list_alloc();
    ಯದಿ (l == ರಿಕ್ತ)
    {
	lua_pushnil(L);
	ಪ್ರತ್ಯಾಗಮ 1;
    }

    luaV_newlist(L, l);
    ಯದಿ (!initarg)
	ಪ್ರತ್ಯಾಗಮ 1;

    // traverse table to init list
    ಪೂರ್ಣಾಙ್ಕ notnil, i = 0;
    typval_T v;
    ಕುರು
    {
	lua_rawgeti(L, 1, ++i);
	notnil = !lua_isnil(L, -1);
	ಯದಿ (notnil)
	{
	    luaV_checktypval(L, -1, &v, "vim.list");
	    list_append_tv(l, &v);
	    clear_tv(&v);
	}
	lua_ತೆಗೆ(L, 1); //ಮೌಲ್ಯ
    } ಯಾವತ್ (notnil);

    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_dict(lua_State *L)
{
    dict_T *d;
    ಪೂರ್ಣಾಙ್ಕ initarg = !lua_isnoneornil(L, 1);

    ಯದಿ (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    d = dict_alloc();
    ಯದಿ (d == ರಿಕ್ತ)
    {
	lua_pushnil(L);
	ಪ್ರತ್ಯಾಗಮ 1;
    }

    luaV_newdict(L, d);
    ಯದಿ (!initarg)
	ಪ್ರತ್ಯಾಗಮ 1;

    // traverse table to init dict
    lua_pushnil(L);
    ಯಾವತ್ (lua_next(L, 1))
    {
	char_u *ಕೀಲಿ;
	dictitem_T *di;
	typval_T v;

	lua_pushvalue(L, -2); // dup key in case it's a number
	ಕೀಲಿ = (char_u *) lua_tostring(L, -1);
	ಯದಿ (ಕೀಲಿ == ರಿಕ್ತ)
	{
	    lua_pushnil(L);
	    ಪ್ರತ್ಯಾಗಮ 1;
	}
	ಯದಿ (*ಕೀಲಿ == NUL)
	    luaL_error(L, "table has empty key");
	luaV_checktypval(L, -2, &v, "vim.dict"); //ಮೌಲ್ಯ
	di = dictitem_alloc(ಕೀಲಿ);
	ಯದಿ (di == ರಿಕ್ತ || dict_add(d, di) == FAIL)
	{
	    vim_free(di);
	    lua_pushnil(L);
	    ಪ್ರತ್ಯಾಗಮ 1;
	}
	di->di_tv = v;
	lua_ತೆಗೆ(L, 2); // key copy and value
    }

    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_blob(lua_State *L)
{
    blob_T *b;
    ಪೂರ್ಣಾಙ್ಕ initarg = !lua_isnoneornil(L, 1);

    ಯದಿ (initarg && !lua_isstring(L, 1))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 1));

    b = blob_alloc();
    ಯದಿ (b == ರಿಕ್ತ)
    {
	lua_pushnil(L);
	ಪ್ರತ್ಯಾಗಮ 1;
    }

    luaV_newblob(L, b);
    ಯದಿ (!initarg)
	ಪ್ರತ್ಯಾಗಮ 1;

    // traverse table to init blob
    size_t i, l = 0;
    ಧ್ರುವ ವರ್ಣ *s = lua_tolstring(L, 1, &l);

    ಯದಿ (ga_grow(&b->bv_ga, (ಪೂರ್ಣಾಙ್ಕ)l) == OK)
	ಪ್ರತಿ (i = 0; i < l; ++i)
	    ga_append(&b->bv_ga, s[i]);

    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_funcref(lua_State *L)
{
    ಧ್ರುವ ವರ್ಣ *name = luaL_checkstring(L, 1);
    // note: not checking if function exists (needs function_exists)
    ಯದಿ (name == ರಿಕ್ತ || *name == NUL || VIM_ISDIGIT(*name))
	luaL_error(L, "invalid function name: %s", name);
    luaV_newfuncref(L, (char_u *) name);
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_buffer(lua_State *L)
{
    buf_T *buf;
    ಯದಿ (lua_isstring(L, 1)) // get by number or name?
    {
	ಯದಿ (lua_isnumber(L, 1)) // by number?
	{
	    ಪೂರ್ಣಾಙ್ಕ n = lua_tointeger(L, 1);
	    FOR_ALL_BUFFERS(buf)
		ಯದಿ (buf->b_fnum == n) ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ // by name
	{
	    size_t l;
	    ಧ್ರುವ ವರ್ಣ *s = lua_tolstring(L, 1, &l);
	    FOR_ALL_BUFFERS(buf)
	    {
		ಯದಿ (buf->b_ffname == ರಿಕ್ತ || buf->b_sfname == ರಿಕ್ತ)
		{
		    ಯದಿ (l == 0) ಭಙ್ಗ;
		}
		ಅನ್ಯಥಾ ಯದಿ (strncmp(s, (ವರ್ಣ *)buf->b_ffname, l) == 0
			|| strncmp(s, (ವರ್ಣ *)buf->b_sfname, l) == 0)
		    ಭಙ್ಗ;
	    }
	}
    }
    ಅನ್ಯಥಾ
	buf = (lua_toboolean(L, 1)) ? firstbuf : curbuf; // first buffer?
    luaV_pushbuffer(L, buf);
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_window(lua_State *L)
{
    win_T *win;
    ಯದಿ (lua_isnumber(L, 1)) // get by number?
    {
	ಪೂರ್ಣಾಙ್ಕ n = lua_tointeger(L, 1);
	ಪ್ರತಿ (win = firstwin; win != ರಿಕ್ತ; win = win->w_next, n--)
	    ಯದಿ (n == 1) ಭಙ್ಗ;
    }
    ಅನ್ಯಥಾ
	win = (lua_toboolean(L, 1)) ? firstwin : curwin; // first window?
    luaV_pushwindow(L, win);
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_open(lua_State *L)
{
    char_u *s = ರಿಕ್ತ;
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    ಯದಿ (lua_isstring(L, 1)) s = (char_u *) lua_tostring(L, 1);
    luaV_pushbuffer(L, buflist_new(s, ರಿಕ್ತ, 1L, BLN_LISTED));
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_type(lua_State *L)
{
    luaL_checkany(L, 1);
    ಯದಿ (lua_type(L, 1) == LUA_TUSERDATA) // check vim udata?
    {
	lua_settop(L, 1);
	ಯದಿ (lua_getmetatable(L, 1))
	{
	    luaV_getfield(L, LUAVIM_LIST);
	    ಯದಿ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "list");
		ಪ್ರತ್ಯಾಗಮ 1;
	    }
	    luaV_getfield(L, LUAVIM_DICT);
	    ಯದಿ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "dict");
		ಪ್ರತ್ಯಾಗಮ 1;
	    }
	    luaV_getfield(L, LUAVIM_BLOB);
	    ಯದಿ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "blob");
		ಪ್ರತ್ಯಾಗಮ 1;
	    }
	    luaV_getfield(L, LUAVIM_FUNCREF);
	    ಯದಿ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "funcref");
		ಪ್ರತ್ಯಾಗಮ 1;
	    }
	    luaV_getfield(L, LUAVIM_BUFFER);
	    ಯದಿ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "buffer");
		ಪ್ರತ್ಯಾಗಮ 1;
	    }
	    luaV_getfield(L, LUAVIM_WINDOW);
	    ಯದಿ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "window");
		ಪ್ರತ್ಯಾಗಮ 1;
	    }
	}
    }
    lua_pushstring(L, luaL_typename(L, 1)); // fallback
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_call(lua_State *L)
{
    ಪೂರ್ಣಾಙ್ಕ		argc = lua_gettop(L) - 1;
    size_t	funcname_len;
    char_u	*funcname;
    ವರ್ಣ	*error = ರಿಕ್ತ;
    typval_T	rettv;
    typval_T	argv[MAX_FUNC_ARGS + 1];
    ಪೂರ್ಣಾಙ್ಕ		i = 0;

    ಯದಿ (argc > MAX_FUNC_ARGS)
	ಪ್ರತ್ಯಾಗಮ luaL_error(L, "Function called with too many arguments");

    funcname = (char_u *)luaL_checklstring(L, 1, &funcname_len);

    ಪ್ರತಿ (; i < argc; i++)
    {
	ಯದಿ (luaV_totypval(L, i + 2, &argv[i]) == FAIL)
	{
	    error = "lua: cannot convert value";
	    ಗಚ್ಛ free_vim_args;
	}
    }

    argv[argc].v_type = VAR_UNKNOWN;

    ಯದಿ (call_vim_function(funcname, argc, argv, &rettv) == FAIL)
    {
	error = "lua: call_vim_function failed";
	ಗಚ್ಛ free_vim_args;
    }

    luaV_pushtypval(L, &rettv);
    clear_tv(&rettv);

free_vim_args:
    ಯಾವತ್ (i > 0)
	clear_tv(&argv[--i]);

    ಯದಿ (error == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 1;
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ luaL_error(L, error);
}

/*
 * Return the Vim version as a Lua table
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
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
    ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ ಧ್ರುವ luaL_Reg luaV_module[] = {
    {"command", luaV_command},
    {"eval", luaV_eval},
    {"beep", luaV_beep},
    {"line", lua_V_ಸಾಲು},
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
    {"lua_version", ರಿಕ್ತ},
    {ರಿಕ್ತ, ರಿಕ್ತ}
};

/*
 * for freeing list, dict, buffer and window objects; lightuserdata as arg
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_free(lua_State *L)
{
    lua_pushnil(L);
    luaV_setudata(L, lua_touserdata(L, 1));
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_luaeval(lua_State *L)
{
    luaL_Buffer b;
    size_t l;
    ಧ್ರುವ ವರ್ಣ *str = lua_tolstring(L, 1, &l);
    typval_T *arg = (typval_T *) lua_touserdata(L, 2);
    typval_T *rettv = (typval_T *) lua_touserdata(L, 3);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, LUAVIM_EVALHEADER, ಪರಿಮಾಣ(LUAVIM_EVALHEADER) - 1);
    luaL_addlstring(&b, str, l);
    luaL_pushresult(&b);
    str = lua_tolstring(L, -1, &l);
    ಯದಿ (luaL_loadbuffer(L, str, l, LUAVIM_EVALNAME)) // compile error?
    {
	luaV_emsg(L);
	ಪ್ರತ್ಯಾಗಮ 0;
    }
    luaV_pushtypval(L, arg);
    ಯದಿ (lua_pcall(L, 1, 1, 0)) // running error?
    {
	luaV_emsg(L);
	ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಯದಿ (luaV_totypval(L, -1, rettv) == FAIL)
	emsg("luaeval: cannot convert value");
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_setref(lua_State *L)
{
    ಪೂರ್ಣಾಙ್ಕ copyID = lua_tointeger(L, 1);
    ಪೂರ್ಣಾಙ್ಕ abort = FALSE;

    lua_pushlightuserdata(L, (ಶೂನ್ಯ *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // the cache table

    luaV_getfield(L, LUAVIM_LIST);
    luaV_getfield(L, LUAVIM_DICT);
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_pushnil(L);
    // traverse cache table
    ಯಾವತ್ (!abort && lua_next(L, 2) != 0)
    {
	lua_getmetatable(L, -1);
	ಯದಿ (lua_rawequal(L, -1, 3)) // list?
	{
	    list_T *l = (list_T *)lua_touserdata(L, 6); //ಕೀಲಿ

	    abort = set_ref_in_list(l, copyID);
	}
	ಅನ್ಯಥಾ ಯದಿ (lua_rawequal(L, -1, 4)) // dict?
	{
	    dict_T *d = (dict_T *)lua_touserdata(L, 6); //ಕೀಲಿ

	    abort = set_ref_in_dict(d, copyID);
	}
	ಅನ್ಯಥಾ ಯದಿ (lua_rawequal(L, -1, 5)) // funcref?
	{
	    luaV_Funcref *f = (luaV_Funcref *)lua_touserdata(L, 6); //ಕೀಲಿ

	    abort = set_ref_in_dict(f->self, copyID);
	}
	lua_ತೆಗೆ(L, 2); // metatable and value
    }
    lua_pushinteger(L, abort);
    ಪ್ರತ್ಯಾಗಮ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_pushversion(lua_State *L)
{
    ಪೂರ್ಣಾಙ್ಕ major = 0;
    ಪೂರ್ಣಾಙ್ಕ minor = 0;
    ಪೂರ್ಣಾಙ್ಕ patch = 0;
    ವರ್ಣ s[16];

    sscanf(LUAVIM_VERSION, "Lua %d.%d.%d", &major, &minor, &patch);
    vim_snprintf(s, ಪರಿಮಾಣ(s), "%d.%d.%d", major, minor, patch);
    lua_pushstring(L, s);
    ಪ್ರತ್ಯಾಗಮ 0;
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

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaopen_vim(lua_State *L)
{
    lua_newtable(L);  // cache table
    lua_newtable(L);  // cache table's metatable
    lua_pushstring(L, "v");
    lua_setfield(L, -2, "__mode");
    lua_setmetatable(L, -2); // cache is weak-valued
    // put the cache table in the registry for luaV_get/setudata()
    lua_pushlightuserdata(L, (ಶೂನ್ಯ *) LUAVIM_UDATA_CACHE);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
    lua_ತೆಗೆ(L, 1);  // we don't need the cache table here anymore
    // print
    lua_pushcfunction(L, luaV_print);
    lua_setglobal(L, "print");
    // debug.debug
    lua_getglobal(L, "debug");
    lua_pushcfunction(L, luaV_debug);
    lua_setfield(L, -2, "debug");
    lua_ತೆಗೆ(L, 1);
    // free
    lua_pushlightuserdata(L, (ಶೂನ್ಯ *) LUAVIM_FREE);
    lua_pushcfunction(L, luaV_free);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // luaeval
    lua_pushlightuserdata(L, (ಶೂನ್ಯ *) LUAVIM_LUAEVAL);
    lua_pushcfunction(L, luaV_luaeval);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // setref
    lua_pushlightuserdata(L, (ಶೂನ್ಯ *) LUAVIM_SETREF);
    lua_pushcfunction(L, luaV_setref);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // register
    luaV_newmetatable(L, LUAVIM_LIST);
    luaV_register(L, luaV_List_mt);
    lua_ತೆಗೆ(L, 1);
    luaV_newmetatable(L, LUAVIM_DICT);
    luaV_register(L, luaV_Dict_mt);
    lua_ತೆಗೆ(L, 1);
    luaV_newmetatable(L, LUAVIM_BLOB);
    luaV_register(L, luaV_Blob_mt);
    lua_ತೆಗೆ(L, 1);
    luaV_newmetatable(L, LUAVIM_FUNCREF);
    luaV_register(L, luaV_Funcref_mt);
    lua_ತೆಗೆ(L, 1);
    luaV_newmetatable(L, LUAVIM_BUFFER);
    luaV_register(L, luaV_Buffer_mt);
    lua_ತೆಗೆ(L, 1);
    luaV_newmetatable(L, LUAVIM_WINDOW);
    luaV_register(L, luaV_Window_mt);
    lua_ತೆಗೆ(L, 1);
    lua_newtable(L); // vim table
    luaV_register(L, luaV_module);
    luaV_pushversion(L);
    lua_setfield(L, -2, "lua_version");
    lua_setglobal(L, LUAVIM_NAME);
    // custom code
    (ಶೂನ್ಯ)luaL_dostring(L, LUA_VIM_FN_CODE);
    (ಶೂನ್ಯ)luaL_dostring(L, LUA_VIM_UPDATE_PACKAGE_PATHS);
    (ಶೂನ್ಯ)luaL_dostring(L, LUA_VIM_SETUP_VARIABLE_DICTS);

    lua_getglobal(L, LUAVIM_NAME);
    lua_getfield(L, -1, "_update_package_paths");

    ಯದಿ (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);

    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ lua_State *
luaV_newstate(ಶೂನ್ಯ)
{
    lua_State *L = luaL_newstate();
# ಯದಿ LUA_VERSION_ಅಂಕೆ >= 505
    luaL_openselectedlibs(L, ~0, 0); // core libs
# ಅನ್ಯಥಾ
    luaL_openlibs(L); // core libs
# endif
    lua_pushcfunction(L, luaopen_vim); // vim
    lua_call(L, 0, 0);
    ಪ್ರತ್ಯಾಗಮ L;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
luaV_setrange(lua_State *L, ಪೂರ್ಣಾಙ್ಕ line1, ಪೂರ್ಣಾಙ್ಕ line2)
{
    lua_getglobal(L, LUAVIM_NAME);
    lua_pushinteger(L, line1);
    lua_setfield(L, -2, "firstline");
    lua_pushinteger(L, line2);
    lua_setfield(L, -2, "lastline");
    lua_ತೆಗೆ(L, 1); // vim table
}


// =======   Interface   =======

ಸ್ಥಿರ lua_State *L = ರಿಕ್ತ;

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
lua_isopen(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ L != ರಿಕ್ತ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
lua_init(ಶೂನ್ಯ)
{
    ಯದಿ (lua_isopen())
	ಪ್ರತ್ಯಾಗಮ OK;

# ifdef DYNAMIC_LUA
    ಯದಿ (!lua_enabled(TRUE))
    {
	emsg(_("Lua library cannot be loaded."));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
# endif
    L = luaV_newstate();

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಶೂನ್ಯ
lua_end(ಶೂನ್ಯ)
{
    ಯದಿ (!lua_isopen())
	ಪ್ರತ್ಯಾಗಮ;

    lua_close(L);
    L = ರಿಕ್ತ;
}

/*
 * ex commands
 */
    ಶೂನ್ಯ
ex_lua(exarg_T *eap)
{
    ವರ್ಣ *script = (ವರ್ಣ *)script_get(eap, eap->arg);

    ಯದಿ (!eap->skip && lua_init() == OK)
    {
	ವರ್ಣ *s = script != ರಿಕ್ತ ? script : (ವರ್ಣ *)eap->arg;

	luaV_setrange(L, eap->line1, eap->line2);
	ಯದಿ (luaL_loadbuffer(L, s, strlen(s), LUAVIM_CHUNKNAME)
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
    ಯದಿ (script != ರಿಕ್ತ)
	vim_free(script);
}

    ಶೂನ್ಯ
ex_luado(exarg_T *eap)
{
    linenr_T l;
    ಧ್ರುವ ವರ್ಣ *s = (ಧ್ರುವ ವರ್ಣ *) eap->arg;
    luaL_Buffer b;
    size_t len;
    buf_T *was_curbuf = curbuf;

    ಯದಿ (lua_init() == FAIL) ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
    {
	emsg(_("cannot save undo information"));
	ಪ್ರತ್ಯಾಗಮ;
    }
    luaV_setrange(L, eap->line1, eap->line2);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, "return function(line, linenr) ", 30); // header
    luaL_addlstring(&b, s, strlen(s));
    luaL_addlstring(&b, " end", 4); // footer
    luaL_pushresult(&b);
    s = lua_tolstring(L, -1, &len);
    ಯದಿ (luaL_loadbuffer(L, s, len, LUAVIM_CHUNKNAME))
    {
	luaV_emsg(L);
	lua_ತೆಗೆ(L, 1); // function body
	ಪ್ರತ್ಯಾಗಮ;
    }
    lua_call(L, 0, 1);
    lua_replace(L, -2); // function -> body
    ಪ್ರತಿ (l = eap->line1; l <= eap->line2; l++)
    {
	// Check the line number, the command may have deleted lines.
	ಯದಿ (l > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ಭಙ್ಗ;

	lua_pushvalue(L, -1); // function
	luaV_pushline(L, curbuf, l); // current line as arg
	lua_pushinteger(L, l); // current line number as arg
	ಯದಿ (lua_pcall(L, 2, 1, 0))
	{
	    luaV_emsg(L);
	    ಭಙ್ಗ;
	}

	// Catch the command switching to another buffer.
	// Check the line number, the command may have deleted lines.
	ಯದಿ (curbuf != was_curbuf || l > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ಭಙ್ಗ;

	ಯದಿ (lua_isstring(L, -1)) // update line?
	{
# ifdef HAVE_SANDBOX
	    luaV_checksandbox(L);
# endif
	    ml_replace(l, luaV_toline(L, -1), TRUE);
	    changed_bytes(l, 0);
	    lua_ತೆಗೆ(L, 1); // result from luaV_toline
	}
	lua_ತೆಗೆ(L, 1); //ಸಾಲು
    }
    lua_ತೆಗೆ(L, 1); // function
    check_cursor();
    update_screen(UPD_NOT_VALID);
}

    ಶೂನ್ಯ
ex_luafile(exarg_T *eap)
{
    ಯದಿ (lua_init() == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (!eap->skip)
    {
	luaV_setrange(L, eap->line1, eap->line2);
	ಯದಿ (luaL_loadfile(L, (ವರ್ಣ *) eap->arg) || lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
}

# define luaV_freetype(typ,tname) \
	ಶೂನ್ಯ \
    lua_##tname##_free(typ *o) \
    { \
	ಯದಿ (!lua_isopen()) ಪ್ರತ್ಯಾಗಮ; \
	luaV_getfield(L, LUAVIM_FREE); \
	lua_pushlightuserdata(L, (ಶೂನ್ಯ *) o); \
	lua_call(L, 1, 0); \
    }

luaV_freetype(buf_T, buffer)
luaV_freetype(win_T, window)

    ಶೂನ್ಯ
do_luaeval(char_u *str, typval_T *arg, typval_T *rettv)
{
    lua_init();
    luaV_getfield(L, LUAVIM_LUAEVAL);
    lua_pushstring(L, (ವರ್ಣ *) str);
    lua_pushlightuserdata(L, (ಶೂನ್ಯ *) arg);
    lua_pushlightuserdata(L, (ಶೂನ್ಯ *) rettv);
    lua_call(L, 3, 0);
}

    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_lua(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    ಪೂರ್ಣಾಙ್ಕ aborted = 0;

    ಯದಿ (!lua_isopen())
	ಪ್ರತ್ಯಾಗಮ 0;

    luaV_getfield(L, LUAVIM_SETREF);
    // call the function with 1 arg, getting 1 result back
    lua_pushinteger(L, copyID);
    lua_call(L, 1, 1);
    // get the result
    aborted = lua_tointeger(L, -1);
    // pop result off the stack
    lua_ತೆಗೆ(L, 1);

    ಪ್ರತ್ಯಾಗಮ aborted;
}

    ಶೂನ್ಯ
update_package_paths_in_lua(ಶೂನ್ಯ)
{
    ಯದಿ (!lua_isopen())
	ಪ್ರತ್ಯಾಗಮ;

    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "_update_package_paths");

    ಯದಿ (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);
}

/*
 * Native C function callback
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
luaV_call_lua_func(
	ಪೂರ್ಣಾಙ್ಕ	 argcount,
	typval_T *argvars,
	typval_T *rettv,
	ಶೂನ್ಯ	 *state)
{
    ಪೂರ್ಣಾಙ್ಕ i;
    ಪೂರ್ಣಾಙ್ಕ luaargcount = argcount;
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_funcref);

    ಯದಿ (funcstate->lua_tableref != LUA_NOREF)
    {
	// First arg for metatable __call method is a table
	luaargcount += 1;
	lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    }

    ಪ್ರತಿ (i = 0; i < argcount; ++i)
	luaV_pushtypval(funcstate->L, &argvars[i]);

    ಯದಿ (lua_pcall(funcstate->L, luaargcount, 1, 0))
    {
	luaV_emsg(funcstate->L);
	ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)FCERR_OTHER;
    }

    luaV_checktypval(funcstate->L, -1, rettv, "get return value");
    ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)FCERR_NONE;
}

/*
 * Free up any lua references held by the func state.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
luaV_call_lua_func_free(ಶೂನ್ಯ *state)
{
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_funcref);
    funcstate->L = ರಿಕ್ತ;
    ಯದಿ (funcstate->lua_tableref != LUA_NOREF)
	luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    VIM_CLEAR(funcstate);
}

#endif
