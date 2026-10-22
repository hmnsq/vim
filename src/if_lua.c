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

#ಆದರೆ __STDC_VERSION__ >= 199901L
# define LUAV_INLINE ಸಾಲೊಳಗೆ
#ಇಲ್ಲದಿದ್ದರೆ
# define LUAV_INLINE
#ಆದರೆ_ಅಂತ್ಯ

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#ಆದರೆ defined(FEAT_LUA)

# define LUAVIM_CHUNKNAME "vim chunk"
# define LUAVIM_NAME "vim"
# define LUAVIM_EVALNAME "luaeval"
# define LUAVIM_EVALHEADER "local _A=select(1,...) return "

# ifdef LUA_RELEASE
#  define LUAVIM_ಆವೃತ್ತಿ LUA_RELEASE
# ಇಲ್ಲದಿದ್ದರೆ
#  define LUAVIM_ಆವೃತ್ತಿ LUA_ಆವೃತ್ತಿ
# endif

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ buf_T *lua_V_ಬಫರ್;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ win_T *lua_V_ಕಿಟಕಿ;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ dict_T *luaV_Dict;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ list_T *luaV_List;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ blob_T *luaV_Blob;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    char_u	*name;	// funcref
    dict_T	*self;	// selfdict
} luaV_Funcref;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*msgfunc_T)(ಅಕ್ಷರ *);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ lua_funcref;    // ref to a lua func
    ಪೂರ್ಣಾಂಕ lua_tableref;   // ref to a lua table if metatable else LUA_NOREF. used
			// for __call
    lua_State *L;
} luaV_CFuncState;

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ LUAVIM_DICT[] = "dict";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ LUAVIM_LIST[] = "list";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ LUAVIM_BLOB[] = "blob";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ LUAVIM_FUNCREF[] = "funcref";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ LUAVIM_ಬಫರ್[] = "buffer";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ LUAVIM_ಕಿಟಕಿ[] = "window";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ LUAVIM_FREE[] = "luaV_free";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ LUAVIM_LUAEVAL[] = "luaV_luaeval";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ LUAVIM_SETREF[] = "luaV_setref";

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ LUA___CALL[] = "__call";

// get/setudata manage references to vim userdata in a cache table through
// object pointers (light userdata). The cache table itself is retrieved
// from the registry.

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ LUAVIM_UDATA_CACHE[] = "luaV_udata_cache";

# define luaV_getfield(L, s) \
    lua_pushlightuserdata((L), (ಶೂನ್ಯ *)(s)); \
    lua_rawget((L), LUA_REGISTRYINDEX)
# define luaV_checksandbox(L) \
    ಆದರೆ (sandbox) luaL_error((L), "not allowed in sandbox")
# define lua_V_ಸಂದೇಶ(L) luaV_msgfunc((L), (msgfunc_T) ಸಂದೇಶ)
# define luaV_emsg(L) luaV_msgfunc((L), (msgfunc_T) emsg)
# define luaV_checktypval(L, a, v, ಸಂದೇಶ) \
    ಮಾಡು { \
	ಆದರೆ (luaV_totypval(L, a, v) == FAIL) \
	    luaL_error(L, ಸಂದೇಶ ": cannot convert value"); \
    } ಆಗ (0)

ಸ್ಥಿರ luaV_List *luaV_pushlist(lua_State *L, list_T *lis);
ಸ್ಥಿರ luaV_Dict *luaV_pushdict(lua_State *L, dict_T *dic);
ಸ್ಥಿರ luaV_Blob *luaV_pushblob(lua_State *L, blob_T *blo);
ಸ್ಥಿರ luaV_Funcref *luaV_pushfuncref(lua_State *L, char_u *name);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ luaV_call_lua_func(ಪೂರ್ಣಾಂಕ argcount, typval_T *argvars, typval_T *rettv, ಶೂನ್ಯ *state);
ಸ್ಥಿರ ಶೂನ್ಯ luaV_call_lua_func_free(ಶೂನ್ಯ *state);

# ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ <= 501
#  define lua_V_ರಿಜಿಸ್ಟರ್(L, l) lua_L_ರಿಜಿಸ್ಟರ್(L, ಖಾಲಿ, l)
#  define luaL_typeerror luaL_typerror
# ಇಲ್ಲದಿದ್ದರೆ
#  define lua_V_ರಿಜಿಸ್ಟರ್(L, l) luaL_setfuncs(L, l, 0)
# endif

# ifdef DYNAMIC_LUA

#  ifdef MSWIN
#   define ತುಂಬು_dll vim_ತುಂಬು_Lib
#   define ಚಿಹ್ನೆ_from_dll Get_Proc_ವಿಳಾಸ
#   define close_dll FreeLibrary
#   define ತುಂಬು_dll_error GetWin32Error
#  ಇಲ್ಲದಿದ್ದರೆ
#   include <dlfcn.h>
#   define HANDLE ಶೂನ್ಯ*
#   define ತುಂಬು_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#   define ಚಿಹ್ನೆ_from_dll dlsym
#   define close_dll dlclose
#   define ತುಂಬು_dll_error dlerror
#  endif

// lauxlib
#  ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ <= 501
#   define lua_L_ರಿಜಿಸ್ಟರ್ dll_lua_L_ರಿಜಿಸ್ಟರ್
#   define luaL_prepbuffer dll_luaL_prepbuffer
#   define luaL_openlib dll_luaL_openlib
#   define luaL_typerror dll_luaL_typerror
#   define luaL_loadfile dll_luaL_loadfile
#   define luaL_loadbuffer dll_luaL_loadbuffer
#  ಇಲ್ಲದಿದ್ದರೆ
#   define luaL_prepbuffsize dll_luaL_prepbuffsize
#   define luaL_setfuncs dll_luaL_setfuncs
#   define luaL_loadfilex dll_luaL_loadfilex
#   define luaL_loadbufferx dll_luaL_loadbufferx
#   define luaL_argerror dll_luaL_argerror
#  endif
#  ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ >= 504
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
#  ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ <= 501
#   define lua_tonumber dll_lua_tonumber
#   define lua_tointeger dll_lua_tointeger
#   define lua_call dll_lua_call
#   define lua_pcall dll_lua_pcall
#  ಇಲ್ಲದಿದ್ದರೆ
#   define lua_tonumberx dll_lua_tonumberx
#   define lua_tointegerx dll_lua_tointegerx
#   define lua_callk dll_lua_callk
#   define lua_pcallk dll_lua_pcallk
#   define lua_getglobal dll_lua_getglobal
#   define lua_setglobal dll_lua_setglobal
#  endif
#  ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ <= 502
#   define lua_replace dll_lua_replace
#   define lua_remove dll_lua_remove
#  endif
#  ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ >= 503
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
#  ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ >= 504
#   define lua_newuserdatauv dll_lua_newuserdatauv
#  ಇಲ್ಲದಿದ್ದರೆ
#   define lua_newuserdata dll_lua_newuserdata
#  endif
#  define lua_getmetatable dll_lua_getmetatable
#  define lua_setfield dll_lua_setfield
#  define lua_rawset dll_lua_rawset
#  define lua_rawseti dll_lua_rawseti
#  define lua_setmetatable dll_lua_setmetatable
#  define lua_ಮುಂದಿನ dll_lua_ಮುಂದಿನ
// libs
#  define luaopen_ತಳ dll_luaopen_ತಳ
#  define luaopen_ಕೋಷ್ಟಕ dll_luaopen_ಕೋಷ್ಟಕ
#  define luaopen_string dll_luaopen_string
#  define luaopen_math dll_luaopen_math
#  define luaopen_io dll_luaopen_io
#  define luaopen_os dll_luaopen_os
#  define luaopen_package dll_luaopen_package
#  define luaopen_debug dll_luaopen_debug
#  ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ >= 505
#   define luaL_openselectedlibs dll_luaL_openselectedlibs
#  ಇಲ್ಲದಿದ್ದರೆ
#   define luaL_openlibs dll_luaL_openlibs
#  endif

// lauxlib
#  ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ <= 501
ಶೂನ್ಯ (*dll_lua_L_ರಿಜಿಸ್ಟರ್) (lua_State *L, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *libname, ಸ್ಥಿರಾಂಕ luaL_Reg *l);
ಅಕ್ಷರ *(*dll_luaL_prepbuffer) (lua_L_ಬಫರ್ *B);
ಶೂನ್ಯ (*dll_luaL_openlib) (lua_State *L, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *libname, ಸ್ಥಿರಾಂಕ luaL_Reg *l, ಪೂರ್ಣಾಂಕ nup);
ಪೂರ್ಣಾಂಕ (*dll_luaL_typerror) (lua_State *L, ಪೂರ್ಣಾಂಕ narg, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tname);
ಪೂರ್ಣಾಂಕ (*dll_luaL_loadfile) (lua_State *L, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename);
ಪೂರ್ಣಾಂಕ (*dll_luaL_loadbuffer) (lua_State *L, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buff, size_t sz, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);
#  ಇಲ್ಲದಿದ್ದರೆ
ಅಕ್ಷರ *(*dll_luaL_prepbuffsize) (lua_L_ಬಫರ್ *B, size_t sz);
ಶೂನ್ಯ (*dll_luaL_setfuncs) (lua_State *L, ಸ್ಥಿರಾಂಕ luaL_Reg *l, ಪೂರ್ಣಾಂಕ nup);
ಪೂರ್ಣಾಂಕ (*dll_luaL_loadfilex) (lua_State *L, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ);
ಪೂರ್ಣಾಂಕ (*dll_luaL_loadbufferx) (lua_State *L, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buff, size_t sz, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಹುಲಕ);
ಪೂರ್ಣಾಂಕ (*dll_luaL_argerror) (lua_State *L, ಪೂರ್ಣಾಂಕ numarg, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extramsg);
#  endif
#  ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ >= 504
ಪೂರ್ಣಾಂಕ (*dll_luaL_typeerror) (lua_State *L, ಪೂರ್ಣಾಂಕ narg, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tname);
#  endif
ಶೂನ್ಯ (*dll_luaL_checkany) (lua_State *L, ಪೂರ್ಣಾಂಕ narg);
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *(*dll_luaL_checklstring) (lua_State *L, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_Arg, size_t *l);
lua_Integer (*dll_luaL_checkinteger) (lua_State *L, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_Arg);
lua_Integer (*dll_luaL_optinteger) (lua_State *L, ಪೂರ್ಣಾಂಕ nArg, lua_Integer def);
ಶೂನ್ಯ (*dll_luaL_checktype) (lua_State *L, ಪೂರ್ಣಾಂಕ narg, ಪೂರ್ಣಾಂಕ t);
ಪೂರ್ಣಾಂಕ (*dll_luaL_error) (lua_State *L, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...);
lua_State *(*dll_luaL_newstate) (ಶೂನ್ಯ);
ಶೂನ್ಯ (*dll_luaL_buffinit) (lua_State *L, lua_L_ಬಫರ್ *B);
ಶೂನ್ಯ (*dll_luaL_addlstring) (lua_L_ಬಫರ್ *B, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t l);
ಶೂನ್ಯ (*dll_luaL_pushresult) (lua_L_ಬಫರ್ *B);
ಪೂರ್ಣಾಂಕ (*dll_luaL_loadstring) (lua_State *L, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s);
ಪೂರ್ಣಾಂಕ (*dll_luaL_ref) (lua_State *L, ಪೂರ್ಣಾಂಕ idx);
#  ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ <= 502
ಶೂನ್ಯ (*dll_luaL_unref) (lua_State *L, ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ n);
#  ಇಲ್ಲದಿದ್ದರೆ
ಶೂನ್ಯ (*dll_luaL_unref) (lua_State *L, ಪೂರ್ಣಾಂಕ idx, lua_Integer n);
#  endif
// lua
#  ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ <= 501
lua_ಸಂಖ್ಯೆ (*dll_lua_tonumber) (lua_State *L, ಪೂರ್ಣಾಂಕ idx);
lua_Integer (*dll_lua_tointeger) (lua_State *L, ಪೂರ್ಣಾಂಕ idx);
ಶೂನ್ಯ (*dll_lua_call) (lua_State *L, ಪೂರ್ಣಾಂಕ nargs, ಪೂರ್ಣಾಂಕ nresults);
ಪೂರ್ಣಾಂಕ (*dll_lua_pcall) (lua_State *L, ಪೂರ್ಣಾಂಕ nargs, ಪೂರ್ಣಾಂಕ nresults, ಪೂರ್ಣಾಂಕ errfunc);
#  ಇಲ್ಲದಿದ್ದರೆ
lua_ಸಂಖ್ಯೆ (*dll_lua_tonumberx) (lua_State *L, ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ *isnum);
lua_Integer (*dll_lua_tointegerx) (lua_State *L, ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ *isnum);
ಶೂನ್ಯ (*dll_lua_callk) (lua_State *L, ಪೂರ್ಣಾಂಕ nargs, ಪೂರ್ಣಾಂಕ nresults, ಪೂರ್ಣಾಂಕ ctx,
	lua_CFunction k);
ಪೂರ್ಣಾಂಕ (*dll_lua_pcallk) (lua_State *L, ಪೂರ್ಣಾಂಕ nargs, ಪೂರ್ಣಾಂಕ nresults, ಪೂರ್ಣಾಂಕ errfunc,
	ಪೂರ್ಣಾಂಕ ctx, lua_CFunction k);
ಶೂನ್ಯ (*dll_lua_getglobal) (lua_State *L, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *var);
ಶೂನ್ಯ (*dll_lua_setglobal) (lua_State *L, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *var);
#  endif
#  ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ <= 502
ಶೂನ್ಯ (*dll_lua_replace) (lua_State *L, ಪೂರ್ಣಾಂಕ idx);
ಶೂನ್ಯ (*dll_lua_remove) (lua_State *L, ಪೂರ್ಣಾಂಕ idx);
#  endif
#  ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ >= 503
ಶೂನ್ಯ  (*dll_lua_rotate) (lua_State *L, ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ n);
ಶೂನ್ಯ (*dll_lua_copy) (lua_State *L, ಪೂರ್ಣಾಂಕ fromidx, ಪೂರ್ಣಾಂಕ toidx);
#  endif
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *(*dll_lua_typename) (lua_State *L, ಪೂರ್ಣಾಂಕ tp);
ಶೂನ್ಯ       (*dll_lua_close) (lua_State *L);
ಪೂರ್ಣಾಂಕ (*dll_lua_gettop) (lua_State *L);
ಶೂನ್ಯ (*dll_lua_settop) (lua_State *L, ಪೂರ್ಣಾಂಕ idx);
ಶೂನ್ಯ (*dll_lua_pushvalue) (lua_State *L, ಪೂರ್ಣಾಂಕ idx);
ಪೂರ್ಣಾಂಕ (*dll_lua_isnumber) (lua_State *L, ಪೂರ್ಣಾಂಕ idx);
ಪೂರ್ಣಾಂಕ (*dll_lua_isstring) (lua_State *L, ಪೂರ್ಣಾಂಕ idx);
ಪೂರ್ಣಾಂಕ (*dll_lua_type) (lua_State *L, ಪೂರ್ಣಾಂಕ idx);
ಪೂರ್ಣಾಂಕ (*dll_lua_rawequal) (lua_State *L, ಪೂರ್ಣಾಂಕ idx1, ಪೂರ್ಣಾಂಕ idx2);
ಪೂರ್ಣಾಂಕ (*dll_lua_toboolean) (lua_State *L, ಪೂರ್ಣಾಂಕ idx);
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *(*dll_lua_tolstring) (lua_State *L, ಪೂರ್ಣಾಂಕ idx, size_t *len);
ಶೂನ್ಯ *(*dll_lua_touserdata) (lua_State *L, ಪೂರ್ಣಾಂಕ idx);
ಶೂನ್ಯ (*dll_lua_pushnil) (lua_State *L);
ಶೂನ್ಯ (*dll_lua_pushnumber) (lua_State *L, lua_ಸಂಖ್ಯೆ n);
ಶೂನ್ಯ (*dll_lua_pushinteger) (lua_State *L, lua_Integer n);
ಶೂನ್ಯ (*dll_lua_pushlstring) (lua_State *L, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t l);
ಶೂನ್ಯ (*dll_lua_pushstring) (lua_State *L, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s);
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *(*dll_lua_pushfstring) (lua_State *L, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...);
ಶೂನ್ಯ (*dll_lua_pushcclosure) (lua_State *L, lua_CFunction fn, ಪೂರ್ಣಾಂಕ n);
ಶೂನ್ಯ (*dll_lua_pushboolean) (lua_State *L, ಪೂರ್ಣಾಂಕ b);
ಶೂನ್ಯ (*dll_lua_pushlightuserdata) (lua_State *L, ಶೂನ್ಯ *p);
ಶೂನ್ಯ (*dll_lua_getfield) (lua_State *L, ಪೂರ್ಣಾಂಕ idx, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *k);
#  ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ <= 502
ಶೂನ್ಯ (*dll_lua_rawget) (lua_State *L, ಪೂರ್ಣಾಂಕ idx);
ಶೂನ್ಯ (*dll_lua_rawgeti) (lua_State *L, ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ n);
#  ಇಲ್ಲದಿದ್ದರೆ
ಪೂರ್ಣಾಂಕ (*dll_lua_rawget) (lua_State *L, ಪೂರ್ಣಾಂಕ idx);
ಪೂರ್ಣಾಂಕ (*dll_lua_rawgeti) (lua_State *L, ಪೂರ್ಣಾಂಕ idx, lua_Integer n);
#  endif
ಶೂನ್ಯ (*dll_lua_createtable) (lua_State *L, ಪೂರ್ಣಾಂಕ narr, ಪೂರ್ಣಾಂಕ nrec);
ಶೂನ್ಯ (*dll_lua_settable) (lua_State *L, ಪೂರ್ಣಾಂಕ idx);
#  ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ >= 504
ಶೂನ್ಯ *(*dll_lua_newuserdatauv) (lua_State *L, size_t sz, ಪೂರ್ಣಾಂಕ nuvalue);
#  ಇಲ್ಲದಿದ್ದರೆ
ಶೂನ್ಯ *(*dll_lua_newuserdata) (lua_State *L, size_t sz);
#  endif
ಪೂರ್ಣಾಂಕ (*dll_lua_getmetatable) (lua_State *L, ಪೂರ್ಣಾಂಕ objindex);
ಶೂನ್ಯ (*dll_lua_setfield) (lua_State *L, ಪೂರ್ಣಾಂಕ idx, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *k);
ಶೂನ್ಯ (*dll_lua_rawset) (lua_State *L, ಪೂರ್ಣಾಂಕ idx);
#  ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ <= 502
ಶೂನ್ಯ (*dll_lua_rawseti) (lua_State *L, ಪೂರ್ಣಾಂಕ idx, ಪೂರ್ಣಾಂಕ n);
#  ಇಲ್ಲದಿದ್ದರೆ
ಶೂನ್ಯ (*dll_lua_rawseti) (lua_State *L, ಪೂರ್ಣಾಂಕ idx, lua_Integer n);
#  endif
ಪೂರ್ಣಾಂಕ (*dll_lua_setmetatable) (lua_State *L, ಪೂರ್ಣಾಂಕ objindex);
ಪೂರ್ಣಾಂಕ (*dll_lua_ಮುಂದಿನ) (lua_State *L, ಪೂರ್ಣಾಂಕ idx);
// libs
ಪೂರ್ಣಾಂಕ (*dll_luaopen_ತಳ) (lua_State *L);
ಪೂರ್ಣಾಂಕ (*dll_luaopen_ಕೋಷ್ಟಕ) (lua_State *L);
ಪೂರ್ಣಾಂಕ (*dll_luaopen_string) (lua_State *L);
ಪೂರ್ಣಾಂಕ (*dll_luaopen_math) (lua_State *L);
ಪೂರ್ಣಾಂಕ (*dll_luaopen_io) (lua_State *L);
ಪೂರ್ಣಾಂಕ (*dll_luaopen_os) (lua_State *L);
ಪೂರ್ಣಾಂಕ (*dll_luaopen_package) (lua_State *L);
ಪೂರ್ಣಾಂಕ (*dll_luaopen_debug) (lua_State *L);
#  ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ >= 505
ಶೂನ್ಯ (*dll_luaL_openselectedlibs) (lua_State *L, ಪೂರ್ಣಾಂಕ ತುಂಬು, ಪೂರ್ಣಾಂಕ preload);
#  ಇಲ್ಲದಿದ್ದರೆ
ಶೂನ್ಯ (*dll_luaL_openlibs) (lua_State *L);
#  endif

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ **luaV_function;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name;
    luaV_function func;
} luaV_Reg;

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ luaV_Reg luaV_dll[] = {
    // lauxlib
#  ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ <= 501
    {"luaL_register", (luaV_function) &dll_lua_L_ರಿಜಿಸ್ಟರ್},
    {"luaL_prepbuffer", (luaV_function) &dll_luaL_prepbuffer},
    {"luaL_openlib", (luaV_function) &dll_luaL_openlib},
    {"luaL_typerror", (luaV_function) &dll_luaL_typerror},
    {"luaL_loadfile", (luaV_function) &dll_luaL_loadfile},
    {"luaL_loadbuffer", (luaV_function) &dll_luaL_loadbuffer},
#  ಇಲ್ಲದಿದ್ದರೆ
    {"luaL_prepbuffsize", (luaV_function) &dll_luaL_prepbuffsize},
    {"luaL_setfuncs", (luaV_function) &dll_luaL_setfuncs},
    {"luaL_loadfilex", (luaV_function) &dll_luaL_loadfilex},
    {"luaL_loadbufferx", (luaV_function) &dll_luaL_loadbufferx},
    {"luaL_argerror", (luaV_function) &dll_luaL_argerror},
#  endif
#  ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ >= 504
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
#  ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ <= 501
    {"lua_tonumber", (luaV_function) &dll_lua_tonumber},
    {"lua_tointeger", (luaV_function) &dll_lua_tointeger},
    {"lua_call", (luaV_function) &dll_lua_call},
    {"lua_pcall", (luaV_function) &dll_lua_pcall},
#  ಇಲ್ಲದಿದ್ದರೆ
    {"lua_tonumberx", (luaV_function) &dll_lua_tonumberx},
    {"lua_tointegerx", (luaV_function) &dll_lua_tointegerx},
    {"lua_callk", (luaV_function) &dll_lua_callk},
    {"lua_pcallk", (luaV_function) &dll_lua_pcallk},
    {"lua_getglobal", (luaV_function) &dll_lua_getglobal},
    {"lua_setglobal", (luaV_function) &dll_lua_setglobal},
#  endif
#  ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ <= 502
    {"lua_replace", (luaV_function) &dll_lua_replace},
    {"lua_remove", (luaV_function) &dll_lua_remove},
#  endif
#  ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ >= 503
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
#  ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ >= 504
    {"lua_newuserdatauv", (luaV_function) &dll_lua_newuserdatauv},
#  ಇಲ್ಲದಿದ್ದರೆ
    {"lua_newuserdata", (luaV_function) &dll_lua_newuserdata},
#  endif
    {"lua_getmetatable", (luaV_function) &dll_lua_getmetatable},
    {"lua_setfield", (luaV_function) &dll_lua_setfield},
    {"lua_rawset", (luaV_function) &dll_lua_rawset},
    {"lua_rawseti", (luaV_function) &dll_lua_rawseti},
    {"lua_setmetatable", (luaV_function) &dll_lua_setmetatable},
    {"lua_next", (luaV_function) &dll_lua_ಮುಂದಿನ},
    // libs
    {"luaopen_base", (luaV_function) &dll_luaopen_ತಳ},
    {"luaopen_table", (luaV_function) &dll_luaopen_ಕೋಷ್ಟಕ},
    {"luaopen_string", (luaV_function) &dll_luaopen_string},
    {"luaopen_math", (luaV_function) &dll_luaopen_math},
    {"luaopen_io", (luaV_function) &dll_luaopen_io},
    {"luaopen_os", (luaV_function) &dll_luaopen_os},
    {"luaopen_package", (luaV_function) &dll_luaopen_package},
    {"luaopen_debug", (luaV_function) &dll_luaopen_debug},
#  ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ >= 505
    {"luaL_openselectedlibs", (luaV_function) &dll_luaL_openselectedlibs},
#  ಇಲ್ಲದಿದ್ದರೆ
    {"luaL_openlibs", (luaV_function) &dll_luaL_openlibs},
#  endif
    {ಖಾಲಿ, ಖಾಲಿ}
};

ಸ್ಥಿರ HANDLE hinstLua = ಖಾಲಿ;

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_link_init(ಅಕ್ಷರ *libname, ಪೂರ್ಣಾಂಕ verbose)
{
    ಸ್ಥಿರಾಂಕ luaV_Reg *reg;
    ಆದರೆ (hinstLua) ಹಿಂತಿರುಗಿ OK;
    hinstLua = ತುಂಬು_dll(libname);
    ಆದರೆ (!hinstLua)
    {
	ಆದರೆ (verbose)
	    semsg(_(e_could_not_ತುಂಬು_library_str_str), libname, ತುಂಬು_dll_error());
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಗಾಗಿ (reg = luaV_dll; reg->func; reg++)
    {
	ಆದರೆ ((*reg->func = ಚಿಹ್ನೆ_from_dll(hinstLua, reg->name)) == ಖಾಲಿ)
	{
	    close_dll(hinstLua);
	    hinstLua = 0;
	    ಆದರೆ (verbose)
		semsg(_(e_could_not_ತುಂಬು_library_function_str), reg->name);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    ಹಿಂತಿರುಗಿ OK;
}
# endif // DYNAMIC_LUA

# ಆದರೆ defined(DYNAMIC_LUA)
    ಪೂರ್ಣಾಂಕ
lua_enabled(ಪೂರ್ಣಾಂಕ verbose)
{
    ಹಿಂತಿರುಗಿ lua_link_init((ಅಕ್ಷರ *)p_luadll, verbose) == OK;
}
# endif

# ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ > 501 && LUA_ಆವೃತ್ತಿ_ಅಂಕೆ < 504
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaL_typeerror(lua_State *L, ಪೂರ್ಣಾಂಕ narg, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tname)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಂದೇಶ = lua_pushfstring(L, "%s expected, got %s",
	    tname, luaL_typename(L, narg));
    ಹಿಂತಿರುಗಿ luaL_argerror(L, narg, ಸಂದೇಶ);
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
luaV_newmetatable(lua_State *L, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tname)
{
    lua_newtable(L);
    lua_pushlightuserdata(L, (ಶೂನ್ಯ *) tname);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
}

    ಸ್ಥಿರ ಶೂನ್ಯ *
luaV_toudata(lua_State *L, ಪೂರ್ಣಾಂಕ ud, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tname)
{
    ಶೂನ್ಯ *p = lua_touserdata(L, ud);

    ಆದರೆ (p == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // value is userdata
    ಆದರೆ (lua_getmetatable(L, ud)) // does it have a metatable?
    {
	luaV_getfield(L, tname); // get metatable
	ಆದರೆ (lua_rawequal(L, -1, -2)) // MTs match?
	{
	    lua_ತೆಗೆ(L, 2); // MTs
	    ಹಿಂತಿರುಗಿ p;
	}
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ *
luaV_checkcache(lua_State *L, ಶೂನ್ಯ *p)
{
    luaV_getudata(L, p);
    ಆದರೆ (lua_isnil(L, -1)) luaL_error(L, "invalid object");
    lua_ತೆಗೆ(L, 1);
    ಹಿಂತಿರುಗಿ p;
}

# define luaV_unbox(L,luatyp,ud) (*((luatyp *) lua_touserdata((L),(ud))))

# define luaV_checkvalid(L,luatyp,ud) \
    luaV_checkcache((L), (ಶೂನ್ಯ *) luaV_unbox((L),luatyp,(ud)))

    ಸ್ಥಿರ ಶೂನ್ಯ *
luaV_checkudata(lua_State *L, ಪೂರ್ಣಾಂಕ ud, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tname)
{
    ಶೂನ್ಯ *p = luaV_toudata(L, ud, tname);
    ಆದರೆ (p == ಖಾಲಿ) luaL_typeerror(L, ud, tname);
    ಹಿಂತಿರುಗಿ p;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
luaV_pushtypval(lua_State *L, typval_T *tv)
{
    ಆದರೆ (tv == ಖಾಲಿ)
    {
	lua_pushnil(L);
	ಹಿಂತಿರುಗಿ;
    }
    ಆಯ್ಕೆ (tv->v_type)
    {
	ಸ್ಥಿತಿ VAR_STRING:
	    lua_pushstring(L, tv->vval.v_string == ಖಾಲಿ
					    ? "" : (ಅಕ್ಷರ *)tv->vval.v_string);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	    lua_pushinteger(L, (ಪೂರ್ಣಾಂಕ) tv->vval.v_ಸಂಖ್ಯೆ);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_FLOAT:
	    lua_pushnumber(L, (lua_ಸಂಖ್ಯೆ) tv->vval.v_float);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_LIST:
	    luaV_pushlist(L, tv->vval.v_list);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_DICT:
	    luaV_pushdict(L, tv->vval.v_dict);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_BOOL:
	ಸ್ಥಿತಿ VAR_SPECIAL:
	    ಆದರೆ (tv->vval.v_ಸಂಖ್ಯೆ <= VVAL_TRUE)
		lua_pushboolean(L, (ಪೂರ್ಣಾಂಕ) tv->vval.v_ಸಂಖ್ಯೆ);
	    ಇಲ್ಲದಿದ್ದರೆ
		lua_pushnil(L);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_FUNC:
	    luaV_pushfuncref(L, tv->vval.v_string);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_PARTIAL:
	    // TODO: handle partial arguments
	    luaV_pushfuncref(L, partial_name(tv->vval.v_partial));
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_BLOB:
	    luaV_pushblob(L, tv->vval.v_blob);
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    lua_pushnil(L);
    }
}

/*
 * Converts lua value at 'pos' to typval 'tv'.
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_totypval(lua_State *L, ಪೂರ್ಣಾಂಕ pos, typval_T *tv)
{
    ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2 = OK;

    tv->v_ಬೀಗ = 0;

    ಆಯ್ಕೆ (lua_type(L, pos))
    {
	ಸ್ಥಿತಿ LUA_TBOOLEAN:
	    tv->v_type = VAR_BOOL;
	    tv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T) lua_toboolean(L, pos);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ LUA_TNIL:
	    tv->v_type = VAR_SPECIAL;
	    tv->vval.v_ಸಂಖ್ಯೆ = VVAL_NULL;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ LUA_TSTRING:
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strsave((char_u *) lua_tostring(L, pos));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ LUA_TNUMBER:
	{
	    ಸ್ಥಿರಾಂಕ lua_ಸಂಖ್ಯೆ n = lua_tonumber(L, pos);

	    ಆದರೆ (n > (lua_ಸಂಖ್ಯೆ)INT64_MAX || n < (lua_ಸಂಖ್ಯೆ)INT64_MIN
		    || ((lua_ಸಂಖ್ಯೆ)((varnumber_T)n)) != n)
	    {
		tv->v_type = VAR_FLOAT;
		tv->vval.v_float = (float_T)n;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		tv->v_type = VAR_ಸಂಖ್ಯೆ;
		tv->vval.v_ಸಂಖ್ಯೆ = (varnumber_T)n;
	    }
	}
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ LUA_TFUNCTION:
	{
	    char_u *name;
	    luaV_CFuncState *state;

	    lua_pushvalue(L, pos);
	    state = ALLOC_CLEAR_ONE(luaV_CFuncState);
	    state->lua_funcref = luaL_ref(L, LUA_REGISTRYINDEX);
	    state->L = L;
	    state->lua_tableref = LUA_NOREF;
	    name = ರಿಜಿಸ್ಟರ್_cfunc(&luaV_call_lua_func,
					      &luaV_call_lua_func_free, state);
	    tv->v_type = VAR_FUNC;
	    tv->vval.v_string = vim_strsave(name);
	    ನಿಲ್ಲಿಸು;
	}
	ಸ್ಥಿತಿ LUA_TTABLE:
	{
	    ಪೂರ್ಣಾಂಕ lua_tableref;

	    lua_pushvalue(L, pos);
	    lua_tableref = luaL_ref(L, LUA_REGISTRYINDEX);
	    ಆದರೆ (lua_getmetatable(L, pos))
	    {
		lua_getfield(L, -1, LUA___CALL);
		ಆದರೆ (lua_isfunction(L, -1))
		{
		    char_u *name;
		    ಪೂರ್ಣಾಂಕ lua_funcref = luaL_ref(L, LUA_REGISTRYINDEX);
		    luaV_CFuncState *state = ALLOC_CLEAR_ONE(luaV_CFuncState);

		    state->lua_funcref = lua_funcref;
		    state->L = L;
		    state->lua_tableref = lua_tableref;
		    name = ರಿಜಿಸ್ಟರ್_cfunc(&luaV_call_lua_func,
					      &luaV_call_lua_func_free, state);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(name);
		    ನಿಲ್ಲಿಸು;
		}
	    }
	    tv->v_type = VAR_ಸಂಖ್ಯೆ;
	    tv->vval.v_ಸಂಖ್ಯೆ = 0;
	    ಸ್ಥಿತಿ_2 = FAIL;
	    ನಿಲ್ಲಿಸು;
	}
	ಸ್ಥಿತಿ LUA_TUSERDATA:
	{
	    ಶೂನ್ಯ *p = lua_touserdata(L, pos);

	    ಆದರೆ (lua_getmetatable(L, pos)) // has metatable?
	    {
		// check list
		luaV_getfield(L, LUAVIM_LIST);
		ಆದರೆ (lua_rawequal(L, -1, -2))
		{
		    tv->v_type = VAR_LIST;
		    tv->vval.v_list = *((luaV_List *) p);
		    ++tv->vval.v_list->lv_refcount;
		    lua_ತೆಗೆ(L, 2); // MTs
		    ನಿಲ್ಲಿಸು;
		}
		// check dict
		luaV_getfield(L, LUAVIM_DICT);
		ಆದರೆ (lua_rawequal(L, -1, -3))
		{
		    tv->v_type = VAR_DICT;
		    tv->vval.v_dict = *((luaV_Dict *) p);
		    ++tv->vval.v_dict->dv_refcount;
		    lua_ತೆಗೆ(L, 3); // MTs
		    ನಿಲ್ಲಿಸು;
		}
		// check blob
		luaV_getfield(L, LUAVIM_BLOB);
		ಆದರೆ (lua_rawequal(L, -1, -4))
		{
		    tv->v_type = VAR_BLOB;
		    tv->vval.v_blob = *((luaV_Blob *) p);
		    ++tv->vval.v_blob->bv_refcount;
		    lua_ತೆಗೆ(L, 4); // MTs
		    ನಿಲ್ಲಿಸು;
		}
		// check funcref
		luaV_getfield(L, LUAVIM_FUNCREF);
		ಆದರೆ (lua_rawequal(L, -1, -5))
		{
		    luaV_Funcref *f = (luaV_Funcref *) p;

		    func_ref(f->name);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(f->name);
		    lua_ತೆಗೆ(L, 5); // MTs
		    ನಿಲ್ಲಿಸು;
		}
		lua_ತೆಗೆ(L, 4); // MTs
	    }
	}
	// FALLTHROUGH
	ಪೂರ್ವನಿಯೋಜಿತ:
	    tv->v_type = VAR_ಸಂಖ್ಯೆ;
	    tv->vval.v_ಸಂಖ್ಯೆ = 0;
	    ಸ್ಥಿತಿ_2 = FAIL;
    }
    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

/*
 * similar to luaL_addlstring, but replaces \0 with \n if toline and
 * \n with \0 otherwise
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
luaV_addlstring(lua_L_ಬಫರ್ *b, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, size_t l, ಪೂರ್ಣಾಂಕ toline)
{
    ಆಗ (l--)
    {
	ಆದರೆ (*s == '\0' && toline)
	    luaL_addchar(b, '\n');
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s == '\n' && !toline)
	    luaL_addchar(b, '\0');
	ಇಲ್ಲದಿದ್ದರೆ
	    luaL_addchar(b, *s);
	s++;
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
luaV_pushline(lua_State *L, buf_T *buf, linenr_T n)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *) ml_get_buf(buf, n, FALSE);
    lua_L_ಬಫರ್ b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s), 0);
    luaL_pushresult(&b);
}

    ಸ್ಥಿರ char_u *
luaV_toline(lua_State *L, ಪೂರ್ಣಾಂಕ pos)
{
    size_t l;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = lua_tolstring(L, pos, &l);

    lua_L_ಬಫರ್ b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 1);
    luaL_pushresult(&b);
    ಹಿಂತಿರುಗಿ (char_u *) lua_tostring(L, -1);
}

/*
 * pops a string s from the top of the stack and calls mf(t) for pieces t of
 * s separated by newlines
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
luaV_msgfunc(lua_State *L, msgfunc_T mf)
{
    lua_L_ಬಫರ್ b;
    size_t l;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, *s = lua_tolstring(L, -1, &l);
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 0);
    luaL_pushresult(&b);
    // break string
    p = s = lua_tolstring(L, -1, &l);
    ಆಗ (l--)
    {
	ಆದರೆ (*p++ == '\0') // break?
	{
	    mf((ಅಕ್ಷರ *)s);
	    s = p;
	}
    }
    mf((ಅಕ್ಷರ *)s);
    lua_ತೆಗೆ(L, 2); // original and modified strings
}

# define luaV_newtype(typ,tname,luatyp,luatname) \
	ಸ್ಥಿರ luatyp * \
    luaV_new##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = (luatyp *) lua_newuserdata(L, ಗಾತ್ರ(luatyp)); \
	*o = obj; \
	luaV_setudata(L, obj); /* cache[obj] = udata */ \
	luaV_getfield(L, luatname); \
	lua_setmetatable(L, -2); \
	ಹಿಂತಿರುಗಿ o; \
    }

# define luaV_pushtype(typ,tname,luatyp) \
	ಸ್ಥಿರ luatyp * \
    lua_V_ತಳ್ಳು##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = ಖಾಲಿ; \
	ಆದರೆ (obj == ಖಾಲಿ) \
	    lua_pushnil(L); \
	ಇಲ್ಲದಿದ್ದರೆ \
	{ \
	    luaV_getudata(L, obj); \
	    ಆದರೆ (lua_isnil(L, -1)) /* not interned? */ \
	    { \
		lua_ತೆಗೆ(L, 1); \
		o = luaV_new##tname(L, obj); \
	    } \
	    ಇಲ್ಲದಿದ್ದರೆ \
		o = (luatyp *) lua_touserdata(L, -1); \
	} \
	ಹಿಂತಿರುಗಿ o; \
    }

# define luaV_type_tostring(tname,luatname) \
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ \
    luaV_##tname##_tostring(lua_State *L) \
    { \
	lua_pushfstring(L, "%s: %p", luatname, lua_touserdata(L, 1)); \
	ಹಿಂತಿರುಗಿ 1; \
    }

// =======   List type   =======

    ಸ್ಥಿರ luaV_List *
luaV_newlist(lua_State *L, list_T *lis)
{
    luaV_List *l = (luaV_List *) lua_newuserdata(L, ಗಾತ್ರ(luaV_List));
    *l = lis;
    lis->lv_refcount++; // reference in Lua
    luaV_setudata(L, lis); // cache[lis] = udata
    luaV_getfield(L, LUAVIM_LIST);
    lua_setmetatable(L, -2);
    ಹಿಂತಿರುಗಿ l;
}

luaV_pushtype(list_T, list, luaV_List)
luaV_type_tostring(list, LUAVIM_LIST)

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_list_len(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushinteger(L, (ಪೂರ್ಣಾಂಕ) list_len(l));
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_list_iter(lua_State *L)
{
    listitem_T *li = (listitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    ಆದರೆ (li == ಖಾಲಿ) ಹಿಂತಿರುಗಿ 0;
    luaV_pushtypval(L, &li->li_tv);
    lua_pushlightuserdata(L, (ಶೂನ್ಯ *) li->li_ಮುಂದಿನ);
    lua_replace(L, lua_upvalueindex(1));
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_list_call(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushlightuserdata(L, (ಶೂನ್ಯ *) l->lv_ಮೊದಲ);
    lua_pushcclosure(L, luaV_list_iter, 1);
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_V_list_ಸೂಚ್ಯಂಕ(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    ಆದರೆ (lua_isnumber(L, 2)) // list item?
    {
	ದೀರ್ಘ n = (ದೀರ್ಘ) luaL_checkinteger(L, 2);
	listitem_T *li;

	// Lua array index starts with 1 while Vim uses 0, subtract 1 to
	// normalize.
	n -= 1;
	li = list_ಕಂಡುಹಿಡಿ(l, n);
	ಆದರೆ (li == ಖಾಲಿ)
	    lua_pushnil(L);
	ಇಲ್ಲದಿದ್ದರೆ
	    luaV_pushtypval(L, &li->li_tv);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lua_isstring(L, 2)) // method?
    {
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = lua_tostring(L, 2);
	ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, "add", 3) == 0
		|| ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, "insert", 6) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    lua_pushnil(L);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	lua_pushnil(L);
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_list_newindex(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    ದೀರ್ಘ n = (ದೀರ್ಘ) luaL_checkinteger(L, 2);
    listitem_T *li;

    // Lua array index starts with 1 while Vim uses 0, subtract 1 to normalize.
    n -= 1;

    ಆದರೆ (l->lv_ಬೀಗ)
	luaL_error(L, "list is locked");
    li = list_ಕಂಡುಹಿಡಿ(l, n);
    ಆದರೆ (li == ಖಾಲಿ)
    {
	ಆದರೆ (!lua_isnil(L, 3))
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "inserting list item");
	    ಆದರೆ (list_ಸೇರಿಸು_tv(l, &v, li) == FAIL)
		luaL_error(L, "failed to add item to list");
	    clear_tv(&v);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (lua_isnil(L, 3)) // remove?
	{
	    vimlist_remove(l, li, li);
	    listitem_free(l, li);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "setting list item");
	    clear_tv(&li->li_tv);
	    li->li_tv = v;
	}
    }
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_V_list_ಕೂಡಿಸು(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (ಶೂನ್ಯ *) *lis);
    typval_T v;
    ಆದರೆ (l->lv_ಬೀಗ)
	luaL_error(L, "list is locked");
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "adding list item");
    ಆದರೆ (list_append_tv(l, &v) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_V_list_ಸೇರಿಸು(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (ಶೂನ್ಯ *) *lis);
    ದೀರ್ಘ pos = (ದೀರ್ಘ) luaL_optinteger(L, 3, 0);
    listitem_T *li = ಖಾಲಿ;
    typval_T v;
    ಆದರೆ (l->lv_ಬೀಗ)
	luaL_error(L, "list is locked");
    ಆದರೆ (pos < l->lv_len)
    {
	li = list_ಕಂಡುಹಿಡಿ(l, pos);
	ಆದರೆ (li == ಖಾಲಿ)
	    luaL_error(L, "invalid position");
    }
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "inserting list item");
    ಆದರೆ (list_ಸೇರಿಸು_tv(l, &v, li) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ luaL_Reg luaV_List_mt[] = {
    {"__tostring", luaV_list_tostring},
    {"__len", luaV_list_len},
    {"__call", luaV_list_call},
    {"__index", lua_V_list_ಸೂಚ್ಯಂಕ},
    {"__newindex", luaV_list_newindex},
    {"add", lua_V_list_ಕೂಡಿಸು},
    {"insert", lua_V_list_ಸೇರಿಸು},
    {ಖಾಲಿ, ಖಾಲಿ}
};


// =======   Dict type   =======

    ಸ್ಥಿರ luaV_Dict *
luaV_newdict(lua_State *L, dict_T *dic)
{
    luaV_Dict *d = (luaV_Dict *) lua_newuserdata(L, ಗಾತ್ರ(luaV_Dict));
    *d = dic;
    dic->dv_refcount++; // reference in Lua
    luaV_setudata(L, dic); // cache[dic] = udata
    luaV_getfield(L, LUAVIM_DICT);
    lua_setmetatable(L, -2);
    ಹಿಂತಿರುಗಿ d;
}

luaV_pushtype(dict_T, dict, luaV_Dict)
luaV_type_tostring(dict, LUAVIM_DICT)

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_dict_len(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    lua_pushinteger(L, (ಪೂರ್ಣಾಂಕ) dict_len(d));
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_dict_iter(lua_State *L ಬಳಸದ)
{
# ifdef FEAT_EVAL
    hashitem_T *hi = (hashitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    ಪೂರ್ಣಾಂಕ n = lua_tointeger(L, lua_upvalueindex(2));
    dictitem_T *di;
    ಆದರೆ (n <= 0) ಹಿಂತಿರುಗಿ 0;
    ಆಗ (HASHITEM_ಖಾಲಿಗಳು(hi)) hi++;
    di = dict_lookup(hi);
    lua_pushstring(L, (ಅಕ್ಷರ *) hi->hi_ಕೀಲಿ);
    luaV_pushtypval(L, &di->di_tv);
    lua_pushlightuserdata(L, (ಶೂನ್ಯ *) (hi + 1));
    lua_replace(L, lua_upvalueindex(1));
    lua_pushinteger(L, n - 1);
    lua_replace(L, lua_upvalueindex(2));
    ಹಿಂತಿರುಗಿ 2;
# ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ 0;
# endif
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_dict_call(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    hashtab_T *ht = &d->dv_hashtab;
    lua_pushlightuserdata(L, (ಶೂನ್ಯ *) ht->ht_array);
    lua_pushinteger(L, ht->ht_ಬಳಸಿದ); // # remaining items
    lua_pushcclosure(L, luaV_dict_iter, 2);
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_V_dict_ಸೂಚ್ಯಂಕ(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *ಕೀಲಿ = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di = dict_ಕಂಡುಹಿಡಿ(d, ಕೀಲಿ, -1);

    ಆದರೆ (di == ಖಾಲಿ)
    {
	lua_pushnil(L);
	ಹಿಂತಿರುಗಿ 1;
    }

    luaV_pushtypval(L, &di->di_tv);
    ಆದರೆ (di->di_tv.v_type == VAR_FUNC) // funcref?
    {
	luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, -1);
	f->self = d; // keep "self" reference
	d->dv_refcount++;
    }

    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_dict_newindex(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *ಕೀಲಿ = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di;
    typval_T tv;

    ಆದರೆ (d->dv_ಬೀಗ)
	luaL_error(L, "dict is locked");
    ಆದರೆ (ಕೀಲಿ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;
    ಆದರೆ (*ಕೀಲಿ == NUL)
	luaL_error(L, "empty key");
    ಆದರೆ (!lua_isnil(L, 3)) // read value?
    {
	luaV_checktypval(L, 3, &tv, "setting dict item");
	ಆದರೆ (d->dv_scope == VAR_DEF_SCOPE && tv.v_type == VAR_FUNC)
	{
	    clear_tv(&tv);
	    luaL_error(L, "cannot assign funcref to builtin scope");
	}
    }
    di = dict_ಕಂಡುಹಿಡಿ(d, ಕೀಲಿ, -1);
    ಆದರೆ (di == ಖಾಲಿ) // non-existing key?
    {
	ಆದರೆ (lua_isnil(L, 3))
	    ಹಿಂತಿರುಗಿ 0;
	di = dictitem_alloc(ಕೀಲಿ);
	ಆದರೆ (di == ಖಾಲಿ)
	{
	    clear_tv(&tv);
	    ಹಿಂತಿರುಗಿ 0;
	}
	ಆದರೆ (dict_ಕೂಡಿಸು(d, di) == FAIL)
	{
	    vim_free(di);
	    clear_tv(&tv);
	    ಹಿಂತಿರುಗಿ 0;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	clear_tv(&di->di_tv);
    ಆದರೆ (lua_isnil(L, 3)) // remove?
    {
	hashitem_T *hi = hash_ಕಂಡುಹಿಡಿ(&d->dv_hashtab, di->di_ಕೀಲಿ);
	hash_remove(&d->dv_hashtab, hi, "Lua new index");
	dictitem_free(di);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	di->di_tv = tv;
    ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ luaL_Reg luaV_Dict_mt[] = {
    {"__tostring", luaV_dict_tostring},
    {"__len", luaV_dict_len},
    {"__call", luaV_dict_call},
    {"__index", lua_V_dict_ಸೂಚ್ಯಂಕ},
    {"__newindex", luaV_dict_newindex},
    {ಖಾಲಿ, ಖಾಲಿ}
};


// =======   Blob type   =======

    ಸ್ಥಿರ luaV_Blob *
luaV_newblob(lua_State *L, blob_T *blo)
{
    luaV_Blob *b = (luaV_Blob *) lua_newuserdata(L, ಗಾತ್ರ(luaV_Blob));
    *b = blo;
    blo->bv_refcount++; // reference in Lua
    luaV_setudata(L, blo); // cache[blo] = udata
    luaV_getfield(L, LUAVIM_BLOB);
    lua_setmetatable(L, -2);
    ಹಿಂತಿರುಗಿ b;
}

luaV_pushtype(blob_T, blob, luaV_Blob)
luaV_type_tostring(blob, LUAVIM_BLOB)

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_blob_gc(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    blob_unref(b);
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_blob_len(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    lua_pushinteger(L, (ಪೂರ್ಣಾಂಕ) blob_len(b));
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_V_blob_ಸೂಚ್ಯಂಕ(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    ಆದರೆ (lua_isnumber(L, 2))
    {
	ಪೂರ್ಣಾಂಕ idx = luaL_checkinteger(L, 2);
	ಆದರೆ (idx < 0)
	    // Negative index is relative to the end.
	    idx += (ಪೂರ್ಣಾಂಕ) blob_len(b);
	ಆದರೆ (idx >= 0 && idx < blob_len(b))
	    lua_pushnumber(L, (lua_ಸಂಖ್ಯೆ) blob_get(b, idx));
	ಇಲ್ಲದಿದ್ದರೆ
	    lua_pushnil(L);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lua_isstring(L, 2))
    {
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = lua_tostring(L, 2);
	ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, "add", 3) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    lua_pushnil(L);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	lua_pushnil(L);
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_blob_newindex(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    ಆದರೆ (b->bv_ಬೀಗ)
	luaL_error(L, "blob is locked");

    ಆದರೆ (!lua_isnumber(L, 2))
	ಹಿಂತಿರುಗಿ 0;

    ದೀರ್ಘ len = blob_len(b);
    ಪೂರ್ಣಾಂಕ idx = luaL_checkinteger(L, 2);
    ಪೂರ್ಣಾಂಕ val = luaL_checkinteger(L, 3);
    ಆದರೆ (idx < 0)
	// Negative index is relative to the end.
	idx += (ಪೂರ್ಣಾಂಕ) len;
    ಆದರೆ ((idx >= 0 && idx < len)
	    || (idx == len && ga_grow(&b->bv_ga, 1) == OK))
    {
	blob_ಗಣ(b, idx, (char_u) val);
	ಆದರೆ (idx == len)
	    ++b->bv_ga.ga_len;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	luaL_error(L, "index out of range");

    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_V_blob_ಕೂಡಿಸು(lua_State *L)
{
    luaV_Blob *blo = luaV_checkudata(L, 1, LUAVIM_BLOB);
    blob_T *b = (blob_T *) luaV_checkcache(L, (ಶೂನ್ಯ *) *blo);
    ಆದರೆ (b->bv_ಬೀಗ)
	luaL_error(L, "blob is locked");
    lua_settop(L, 2);
    ಆದರೆ (!lua_isstring(L, 2))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 2));
    ಇಲ್ಲದಿದ್ದರೆ
    {
	size_t i, l = 0;
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = lua_tolstring(L, 2, &l);

	ಆದರೆ (ga_grow(&b->bv_ga, (ಪೂರ್ಣಾಂಕ)l) == OK)
	    ಗಾಗಿ (i = 0; i < l; ++i)
		ga_append(&b->bv_ga, s[i]);
    }
    lua_settop(L, 1);
    ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ luaL_Reg luaV_Blob_mt[] = {
    {"__tostring", luaV_blob_tostring},
    {"__gc", luaV_blob_gc},
    {"__len", luaV_blob_len},
    {"__index", lua_V_blob_ಸೂಚ್ಯಂಕ},
    {"__newindex", luaV_blob_newindex},
    {"add", lua_V_blob_ಕೂಡಿಸು},
    {ಖಾಲಿ, ಖಾಲಿ}
};


// =======   Funcref type   =======

    ಸ್ಥಿರ luaV_Funcref *
luaV_newfuncref(lua_State *L, char_u *name)
{
    luaV_Funcref *f = (luaV_Funcref *)lua_newuserdata(L, ಗಾತ್ರ(luaV_Funcref));

    ಆದರೆ (name != ಖಾಲಿ)
    {
	func_ref(name);
	f->name = vim_strsave(name);
    }
    f->self = ಖಾಲಿ;
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_setmetatable(L, -2);
    ಹಿಂತಿರುಗಿ f;
}

    ಸ್ಥಿರ luaV_Funcref *
luaV_pushfuncref(lua_State *L, char_u *name)
{
    ಹಿಂತಿರುಗಿ luaV_newfuncref(L, name);
}


luaV_type_tostring(funcref, LUAVIM_FUNCREF)

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_funcref_gc(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    func_unref(f->name);
    vim_free(f->name);
    // NOTE: Don't call "dict_unref(f->self)", because the dict of "f->self"
    // will be (or has been already) freed by Vim's garbage collection.
    ಹಿಂತಿರುಗಿ 0;
}

// equivalent to string(funcref)
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_funcref_len(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    lua_pushstring(L, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *) f->name);
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_funcref_call(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);
    ಪೂರ್ಣಾಂಕ i, n = lua_gettop(L) - 1; // #args
    ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2 = FAIL;
    typval_T ವಾದಗಳು;
    typval_T rettv;

    ವಾದಗಳು.v_type = VAR_LIST;
    ವಾದಗಳು.vval.v_list = list_alloc();
    rettv.v_type = VAR_UNKNOWN; // as in clear_tv
    ಆದರೆ (ವಾದಗಳು.vval.v_list != ಖಾಲಿ)
    {
	typval_T v;

	ಗಾಗಿ (i = 0; i < n; i++)
	{
	    luaV_checktypval(L, i + 2, &v, "calling funcref");
	    list_append_tv(ವಾದಗಳು.vval.v_list, &v);
	    clear_tv(&v);
	}
	ಸ್ಥಿತಿ_2 = func_call(f->name, &ವಾದಗಳು, ಖಾಲಿ, f->self, &rettv);
	ಆದರೆ (ಸ್ಥಿತಿ_2 == OK)
	    luaV_pushtypval(L, &rettv);
	clear_tv(&ವಾದಗಳು);
	clear_tv(&rettv);
    }
    ಆದರೆ (ಸ್ಥಿತಿ_2 != OK)
	luaL_error(L, "cannot call funcref");
    ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ luaL_Reg luaV_Funcref_mt[] = {
    {"__tostring", luaV_funcref_tostring},
    {"__gc", luaV_funcref_gc},
    {"__len", luaV_funcref_len},
    {"__call", luaV_funcref_call},
    {ಖಾಲಿ, ಖಾಲಿ}
};


// =======   Buffer type   =======

luaV_newtype(buf_T, ಬಫರ್, lua_V_ಬಫರ್, LUAVIM_ಬಫರ್)
luaV_pushtype(buf_T, ಬಫರ್, lua_V_ಬಫರ್)
luaV_type_tostring(ಬಫರ್, LUAVIM_ಬಫರ್)

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_V_ಬಫರ್_len(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, lua_V_ಬಫರ್, 1);
    lua_pushinteger(L, b->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ);
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_V_ಬಫರ್_call(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, lua_V_ಬಫರ್, 1);
    lua_settop(L, 1);
    ಗಣ_curbuf(b, DOBUF_ವಿಭಜಿಸು);
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_V_ಬಫರ್_ಸೂಚ್ಯಂಕ(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, lua_V_ಬಫರ್, 1);
    linenr_T n = (linenr_T) lua_tointeger(L, 2);
    ಆದರೆ (n > 0 && n <= b->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	luaV_pushline(L, b, n);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lua_isstring(L, 2))
    {
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = lua_tostring(L, 2);
	ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, "name", 4) == 0)
	    lua_pushstring(L, (b->b_sfname == ಖಾಲಿ)
					? "" : (ಅಕ್ಷರ *) b->b_sfname);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, "fname", 5) == 0)
	    lua_pushstring(L, (b->b_ffname == ಖಾಲಿ)
					? "" : (ಅಕ್ಷರ *) b->b_ffname);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, "number", 6) == 0)
	    lua_pushinteger(L, b->b_fnum);
	// methods
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s,   "insert", 6) == 0
		|| ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, "next", 4) == 0
		|| ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, "previous", 8) == 0
		|| ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, "isvalid", 7) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    lua_pushnil(L);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	lua_pushnil(L);
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_V_ಬಫರ್_newindex(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, lua_V_ಬಫರ್, 1);
    linenr_T n = (linenr_T) luaL_checkinteger(L, 2);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    ಆದರೆ (n < 1 || n > b->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	luaL_error(L, "invalid line number");
    ಆದರೆ (lua_isnil(L, 3)) // delete line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	ಆದರೆ (u_savedel(n, 1L) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ml_delete(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot delete line");
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    deleted_ಸಾಲುಗಳು_mark(n, 1L);
	    ಆದರೆ (b == curwin->w_ಬಫರ್) // fix cursor in current window?
	    {
		ಆದರೆ (curwin->w_cursor.lnum >= n)
		{
		    ಆದರೆ (curwin->w_cursor.lnum > n)
		    {
			curwin->w_cursor.lnum -= 1;
			check_cursor_ಲಂಬಸಾಲು();
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			check_cursor();
		    ಬದಲಾಯಿತು_cline_bef_curs();
		}
		invalidate_botline();
	    }
	}
	curbuf = buf;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lua_isstring(L, 3)) // update line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	ಆದರೆ (u_savesub(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ml_replace(n, luaV_toline(L, 3), TRUE) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot replace line");
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಬದಲಾಯಿತು_ಬೈಟುಗಳು(n, 0);
	curbuf = buf;
	ಆದರೆ (b == curwin->w_ಬಫರ್)
	    check_cursor_ಲಂಬಸಾಲು();
    }
    ಇಲ್ಲದಿದ್ದರೆ
	luaL_error(L, "wrong argument to change line");
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_V_ಬಫರ್_ಸೇರಿಸು(lua_State *L)
{
    lua_V_ಬಫರ್ *lb = luaV_checkudata(L, 1, LUAVIM_ಬಫರ್);
    buf_T *b = (buf_T *) luaV_checkcache(L, (ಶೂನ್ಯ *) *lb);
    linenr_T ಕೊನೆಯ = b->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ;
    linenr_T n = (linenr_T) luaL_optinteger(L, 3, ಕೊನೆಯ);
    buf_T *buf;
    luaL_checktype(L, 2, LUA_TSTRING);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    // fix insertion line
    ಆದರೆ (n < 0) n = 0;
    ಆದರೆ (n > ಕೊನೆಯ) n = ಕೊನೆಯ;
    // insert
    buf = curbuf;
    curbuf = b;
    ಆದರೆ (u_save(n, n + 1) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot save undo information");
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ml_append(n, luaV_toline(L, 2), 0, FALSE) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot insert line");
    }
    ಇಲ್ಲದಿದ್ದರೆ
	appended_ಸಾಲುಗಳು_mark(n, 1L);
    curbuf = buf;
    update_screen(UPD_VALID);
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_V_ಬಫರ್_ಮುಂದಿನ(lua_State *L)
{
    lua_V_ಬಫರ್ *b = luaV_checkudata(L, 1, LUAVIM_ಬಫರ್);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (ಶೂನ್ಯ *) *b);
    luaV_pushbuffer(L, buf->b_ಮುಂದಿನ);
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_V_ಬಫರ್_ಹಿಂದಿನ(lua_State *L)
{
    lua_V_ಬಫರ್ *b = luaV_checkudata(L, 1, LUAVIM_ಬಫರ್);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (ಶೂನ್ಯ *) *b);
    luaV_pushbuffer(L, buf->b_prev);
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_V_ಬಫರ್_isvalid(lua_State *L)
{
    lua_V_ಬಫರ್ *b = luaV_checkudata(L, 1, LUAVIM_ಬಫರ್);
    luaV_getudata(L, *b);
    lua_pushboolean(L, !lua_isnil(L, -1));
    ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ luaL_Reg lua_V_ಬಫರ್_mt[] = {
    {"__tostring", lua_V_ಬಫರ್_tostring},
    {"__len", lua_V_ಬಫರ್_len},
    {"__call", lua_V_ಬಫರ್_call},
    {"__index", lua_V_ಬಫರ್_ಸೂಚ್ಯಂಕ},
    {"__newindex", lua_V_ಬಫರ್_newindex},
    {"insert", lua_V_ಬಫರ್_ಸೇರಿಸು},
    {"next", lua_V_ಬಫರ್_ಮುಂದಿನ},
    {"previous", lua_V_ಬಫರ್_ಹಿಂದಿನ},
    {"isvalid", lua_V_ಬಫರ್_isvalid},
    {ಖಾಲಿ, ಖಾಲಿ}
};


// =======   Window type   =======

luaV_newtype(win_T, ಕಿಟಕಿ_2, lua_V_ಕಿಟಕಿ, LUAVIM_ಕಿಟಕಿ)
luaV_pushtype(win_T, ಕಿಟಕಿ_2, lua_V_ಕಿಟಕಿ)
luaV_type_tostring(ಕಿಟಕಿ_2, LUAVIM_ಕಿಟಕಿ)

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_V_ಕಿಟಕಿ_call(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, lua_V_ಕಿಟಕಿ, 1);
    lua_settop(L, 1);
    win_goto(w);
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_V_ಕಿಟಕಿ_ಸೂಚ್ಯಂಕ(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, lua_V_ಕಿಟಕಿ, 1);
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = luaL_checkstring(L, 2);
    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, "buffer", 6) == 0)
	luaV_pushbuffer(L, w->w_ಬಫರ್);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, "line", 4) == 0)
	lua_pushinteger(L, w->w_cursor.lnum);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, "col", 3) == 0)
	lua_pushinteger(L, w->w_cursor.ಲಂಬಸಾಲು + 1);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, "width", 5) == 0)
	lua_pushinteger(L, w->w_ಅಗಲ);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, "height", 6) == 0)
	lua_pushinteger(L, w->w_ಎತ್ತರ);
    // methods
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s,   "next", 4) == 0
	    || ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, "previous", 8) == 0
	    || ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, "isvalid", 7) == 0)
    {
	lua_getmetatable(L, 1);
	lua_getfield(L, -1, s);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	lua_pushnil(L);
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_V_ಕಿಟಕಿ_newindex(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, lua_V_ಕಿಟಕಿ, 1);
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = luaL_checkstring(L, 2);
    ಪೂರ್ಣಾಂಕ v = luaL_checkinteger(L, 3);
    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, "line", 4) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	ಆದರೆ (v < 1 || v > w->w_ಬಫರ್->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    luaL_error(L, "line out of range");
	w->w_cursor.lnum = v;
	update_screen(UPD_VALID);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, "col", 3) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	w->w_cursor.ಲಂಬಸಾಲು = v - 1;
	w->w_ಗಣ_curswant = true;
	update_screen(UPD_VALID);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, "width", 5) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setwidth(v);
	curwin = win;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, "height", 6) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setheight(v);
	curwin = win;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	luaL_error(L, "invalid window property: `%s'", s);
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_V_ಕಿಟಕಿ_ಮುಂದಿನ(lua_State *L)
{
    lua_V_ಕಿಟಕಿ *w = luaV_checkudata(L, 1, LUAVIM_ಕಿಟಕಿ);
    win_T *win = (win_T *) luaV_checkcache(L, (ಶೂನ್ಯ *) *w);
    luaV_pushwindow(L, win->w_ಮುಂದಿನ);
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_V_ಕಿಟಕಿ_ಹಿಂದಿನ(lua_State *L)
{
    lua_V_ಕಿಟಕಿ *w = luaV_checkudata(L, 1, LUAVIM_ಕಿಟಕಿ);
    win_T *win = (win_T *) luaV_checkcache(L, (ಶೂನ್ಯ *) *w);
    luaV_pushwindow(L, win->w_prev);
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_V_ಕಿಟಕಿ_isvalid(lua_State *L)
{
    lua_V_ಕಿಟಕಿ *w = luaV_checkudata(L, 1, LUAVIM_ಕಿಟಕಿ);
    luaV_getudata(L, *w);
    lua_pushboolean(L, !lua_isnil(L, -1));
    ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ luaL_Reg lua_V_ಕಿಟಕಿ_mt[] = {
    {"__tostring", lua_V_ಕಿಟಕಿ_tostring},
    {"__call", lua_V_ಕಿಟಕಿ_call},
    {"__index", lua_V_ಕಿಟಕಿ_ಸೂಚ್ಯಂಕ},
    {"__newindex", lua_V_ಕಿಟಕಿ_newindex},
    {"next", lua_V_ಕಿಟಕಿ_ಮುಂದಿನ},
    {"previous", lua_V_ಕಿಟಕಿ_ಹಿಂದಿನ},
    {"isvalid", lua_V_ಕಿಟಕಿ_isvalid},
    {ಖಾಲಿ, ಖಾಲಿ}
};


// =======   Vim module   =======

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_print(lua_State *L)
{
    ಪೂರ್ಣಾಂಕ		i, n = lua_gettop(L); // nargs
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*s;
    size_t	l;
    garray_T	ಸಂದೇಶ_ga;

    ga_init2(&ಸಂದೇಶ_ga, 1, 128);
    lua_getglobal(L, "tostring");
    ಗಾಗಿ (i = 1; i <= n; i++)
    {
	lua_pushvalue(L, -1); // tostring
	lua_pushvalue(L, i); // arg
	lua_call(L, 1, 1);
	s = lua_tolstring(L, -1, &l);
	ಆದರೆ (s == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ luaL_error(L, "cannot convert to string");
	ಆದರೆ (i > 1)
	    ga_append(&ಸಂದೇಶ_ga, ' '); // use space instead of tab
	ga_concat_len(&ಸಂದೇಶ_ga, (char_u *)s, l);
	lua_ತೆಗೆ(L, 1);
    }
    // Replace any "\n" with "\0"
    ಗಾಗಿ (i = 0; i < ಸಂದೇಶ_ga.ga_len; i++)
	ಆದರೆ (((ಅಕ್ಷರ *)ಸಂದೇಶ_ga.ga_ದತ್ತಾಂಶ)[i] == '\n')
	    ((ಅಕ್ಷರ *)ಸಂದೇಶ_ga.ga_ದತ್ತಾಂಶ)[i] = '\0';
    lua_pushlstring(L, ಸಂದೇಶ_ga.ga_ದತ್ತಾಂಶ, ಸಂದೇಶ_ga.ga_len);
    ಆದರೆ (!got_int)
	lua_V_ಸಂದೇಶ(L);

    ga_clear(&ಸಂದೇಶ_ga);
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_debug(lua_State *L)
{
    lua_settop(L, 0);
    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "eval");
    lua_remove(L, -2); // vim.eval at position 1
    ಗಾಗಿ (;;)
    {
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳಹರಿವು;
	size_t l;
	lua_pushvalue(L, 1); // vim.eval
	lua_pushliteral(L, "input('lua_debug> ')");
	lua_call(L, 1, 1); // return string
	ಒಳಹರಿವು = lua_tolstring(L, -1, &l);
	ಆದರೆ (l == 0 || strcmp(ಒಳಹರಿವು, "cont") == 0)
	    ಹಿಂತಿರುಗಿ 0;
	ಸಂದೇಶ_putchar('\n'); // avoid outputting on input line
	ಆದರೆ (luaL_loadbuffer(L, ಒಳಹರಿವು, l, "=(debug command)")
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
	lua_settop(L, 1); // remove eventual returns, but keep vim.eval
    }
}

    ಸ್ಥಿರ dict_T *
luaV_get_var_scope(lua_State *L)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*scope = luaL_checkstring(L, 1);
    dict_T	*dict = ಖಾಲಿ;

    ಆದರೆ (STRICMP((ಅಕ್ಷರ *)scope, "g") == 0)
	dict = get_globvar_dict();
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP((ಅಕ್ಷರ *)scope, "v") == 0)
	dict = get_vimvar_dict();
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP((ಅಕ್ಷರ *)scope, "b") == 0)
	dict = curbuf->b_vars;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP((ಅಕ್ಷರ *)scope, "w") == 0)
	dict = curwin->w_vars;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRICMP((ಅಕ್ಷರ *)scope, "t") == 0)
	dict = curtab->tp_vars;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	luaL_error(L, "invalid scope %s", scope);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ dict;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_setvar(lua_State *L)
{
    dict_T	*dict;
    dictitem_T	*di;
    size_t	len;
    ಅಕ್ಷರ	*name;
    ಪೂರ್ಣಾಂಕ		del;
    ಅಕ್ಷರ	*error = ಖಾಲಿ;

    name = (ಅಕ್ಷರ *)luaL_checklstring(L, 3, &len);
    del = (lua_gettop(L) < 4) || lua_isnil(L, 4);

    dict = luaV_get_var_scope(L);
    ಆದರೆ (dict == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)name, (ಪೂರ್ಣಾಂಕ)len);
    ಆದರೆ (di != ಖಾಲಿ)
    {
	ಆದರೆ (di->di_flags & DI_FLAGS_RO)
	    error = "variable is read-only";
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (di->di_flags & DI_FLAGS_ಬೀಗ)
	    error = "variable is locked";
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (del && di->di_flags & DI_FLAGS_FIX)
	    error = "variable is fixed";
	ಆದರೆ (error != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ luaL_error(L, error);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dict->dv_ಬೀಗ)
	ಹಿಂತಿರುಗಿ luaL_error(L, "Dictionary is locked");

    ಆದರೆ (del)
    {
	// Delete the key
	ಆದರೆ (di == ಖಾಲಿ)
	    // Doesn't exist, nothing to do
	    ಹಿಂತಿರುಗಿ 0;
	// Delete the entry
	dictitem_remove(dict, di, "Lua delete variable");
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Update the key
	typval_T	tv;

	// Convert the lua value to a Vim script type in the temporary variable
	lua_pushvalue(L, 4);
	ಆದರೆ (luaV_totypval(L, -1, &tv) == FAIL)
	    ಹಿಂತಿರುಗಿ luaL_error(L, "Couldn't convert lua value");

	ಆದರೆ (di == ಖಾಲಿ)
	{
	    // Need to create an entry
	    di = dictitem_alloc((char_u *)name);
	    ಆದರೆ (di == ಖಾಲಿ)
	    {
		clear_tv(&tv);
		ಹಿಂತಿರುಗಿ 0;
	    }
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	    ಆದರೆ (dict_ಕೂಡಿಸು(dict, di) == FAIL)
	    {
		dictitem_free(di);
		clear_tv(&tv);
		ಹಿಂತಿರುಗಿ luaL_error(L, "Couldn't add to dictionary");
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ type_error = FALSE;
	    ಆದರೆ (dict == get_vimvar_dict()
	       && !before_ಗಣ_vvar((char_u *)name, di, &tv, TRUE, &type_error))
	    {
		clear_tv(&tv);
		ಆದರೆ (type_error)
		    ಹಿಂತಿರುಗಿ luaL_error(L,
				"Setting v:%s to value with wrong type", name);
		ಹಿಂತಿರುಗಿ 0;
	    }
	    // Clear the old value
	    clear_tv(&di->di_tv);
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	}

	// Clear the temporary variable
	clear_tv(&tv);
    }

    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_getvar(lua_State *L)
{
    dict_T	*dict = luaV_get_var_scope(L);
    size_t	len;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*name = luaL_checklstring(L, 3, &len);
    dictitem_T	*di = dict_ಕಂಡುಹಿಡಿ(dict, (char_u *)name, (ಪೂರ್ಣಾಂಕ)len);

    ಆದರೆ (di == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;  // nil

    luaV_pushtypval(L, &di->di_tv);
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_command(lua_State *L)
{
    char_u  *s = vim_strsave((char_u *)luaL_checkstring(L, 1));

    execute_cmds_from_string(s);
    vim_free(s);
    update_screen(UPD_VALID);
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_eval(lua_State *L)
{
    typval_T *tv = eval_ಅಭಿವ್ಯಕ್ತಿ((char_u *) luaL_checkstring(L, 1), ಖಾಲಿ);

    ಆದರೆ (tv == ಖಾಲಿ) luaL_error(L, "invalid expression");
    luaV_pushtypval(L, tv);
    free_tv(tv);
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_beep(lua_State *L ಬಳಸದ)
{
    vim_beep(BO_LANG);
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_V_ಸಾಲು(lua_State *L)
{
    luaV_pushline(L, curbuf, curwin->w_cursor.lnum);
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_list(lua_State *L)
{
    list_T *l;
    ಪೂರ್ಣಾಂಕ initarg = !lua_isnoneornil(L, 1);

    ಆದರೆ (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    l = list_alloc();
    ಆದರೆ (l == ಖಾಲಿ)
    {
	lua_pushnil(L);
	ಹಿಂತಿರುಗಿ 1;
    }

    luaV_newlist(L, l);
    ಆದರೆ (!initarg)
	ಹಿಂತಿರುಗಿ 1;

    // traverse table to init list
    ಪೂರ್ಣಾಂಕ notnil, i = 0;
    typval_T v;
    ಮಾಡು
    {
	lua_rawgeti(L, 1, ++i);
	notnil = !lua_isnil(L, -1);
	ಆದರೆ (notnil)
	{
	    luaV_checktypval(L, -1, &v, "vim.list");
	    list_append_tv(l, &v);
	    clear_tv(&v);
	}
	lua_ತೆಗೆ(L, 1); //ಮೌಲ್ಯ
    } ಆಗ (notnil);

    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_dict(lua_State *L)
{
    dict_T *d;
    ಪೂರ್ಣಾಂಕ initarg = !lua_isnoneornil(L, 1);

    ಆದರೆ (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    d = dict_alloc();
    ಆದರೆ (d == ಖಾಲಿ)
    {
	lua_pushnil(L);
	ಹಿಂತಿರುಗಿ 1;
    }

    luaV_newdict(L, d);
    ಆದರೆ (!initarg)
	ಹಿಂತಿರುಗಿ 1;

    // traverse table to init dict
    lua_pushnil(L);
    ಆಗ (lua_ಮುಂದಿನ(L, 1))
    {
	char_u *ಕೀಲಿ;
	dictitem_T *di;
	typval_T v;

	lua_pushvalue(L, -2); // dup key in case it's a number
	ಕೀಲಿ = (char_u *) lua_tostring(L, -1);
	ಆದರೆ (ಕೀಲಿ == ಖಾಲಿ)
	{
	    lua_pushnil(L);
	    ಹಿಂತಿರುಗಿ 1;
	}
	ಆದರೆ (*ಕೀಲಿ == NUL)
	    luaL_error(L, "table has empty key");
	luaV_checktypval(L, -2, &v, "vim.dict"); //ಮೌಲ್ಯ
	di = dictitem_alloc(ಕೀಲಿ);
	ಆದರೆ (di == ಖಾಲಿ || dict_ಕೂಡಿಸು(d, di) == FAIL)
	{
	    vim_free(di);
	    lua_pushnil(L);
	    ಹಿಂತಿರುಗಿ 1;
	}
	di->di_tv = v;
	lua_ತೆಗೆ(L, 2); // key copy and value
    }

    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_blob(lua_State *L)
{
    blob_T *b;
    ಪೂರ್ಣಾಂಕ initarg = !lua_isnoneornil(L, 1);

    ಆದರೆ (initarg && !lua_isstring(L, 1))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 1));

    b = blob_alloc();
    ಆದರೆ (b == ಖಾಲಿ)
    {
	lua_pushnil(L);
	ಹಿಂತಿರುಗಿ 1;
    }

    luaV_newblob(L, b);
    ಆದರೆ (!initarg)
	ಹಿಂತಿರುಗಿ 1;

    // traverse table to init blob
    size_t i, l = 0;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = lua_tolstring(L, 1, &l);

    ಆದರೆ (ga_grow(&b->bv_ga, (ಪೂರ್ಣಾಂಕ)l) == OK)
	ಗಾಗಿ (i = 0; i < l; ++i)
	    ga_append(&b->bv_ga, s[i]);

    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_funcref(lua_State *L)
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name = luaL_checkstring(L, 1);
    // note: not checking if function exists (needs function_exists)
    ಆದರೆ (name == ಖಾಲಿ || *name == NUL || VIM_ISDIGIT(*name))
	luaL_error(L, "invalid function name: %s", name);
    luaV_newfuncref(L, (char_u *) name);
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_V_ಬಫರ್_2(lua_State *L)
{
    buf_T *buf;
    ಆದರೆ (lua_isstring(L, 1)) // get by number or name?
    {
	ಆದರೆ (lua_isnumber(L, 1)) // by number?
	{
	    ಪೂರ್ಣಾಂಕ n = lua_tointeger(L, 1);
	    FOR_ALL_BUFFERS(buf)
		ಆದರೆ (buf->b_fnum == n) ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ // by name
	{
	    size_t l;
	    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = lua_tolstring(L, 1, &l);
	    FOR_ALL_BUFFERS(buf)
	    {
		ಆದರೆ (buf->b_ffname == ಖಾಲಿ || buf->b_sfname == ಖಾಲಿ)
		{
		    ಆದರೆ (l == 0) ನಿಲ್ಲಿಸು;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, (ಅಕ್ಷರ *)buf->b_ffname, l) == 0
			|| ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(s, (ಅಕ್ಷರ *)buf->b_sfname, l) == 0)
		    ನಿಲ್ಲಿಸು;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	buf = (lua_toboolean(L, 1)) ? firstbuf : curbuf; // first buffer?
    luaV_pushbuffer(L, buf);
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_V_ಕಿಟಕಿ_2(lua_State *L)
{
    win_T *win;
    ಆದರೆ (lua_isnumber(L, 1)) // get by number?
    {
	ಪೂರ್ಣಾಂಕ n = lua_tointeger(L, 1);
	ಗಾಗಿ (win = firstwin; win != ಖಾಲಿ; win = win->w_ಮುಂದಿನ, n--)
	    ಆದರೆ (n == 1) ನಿಲ್ಲಿಸು;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	win = (lua_toboolean(L, 1)) ? firstwin : curwin; // first window?
    luaV_pushwindow(L, win);
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_open(lua_State *L)
{
    char_u *s = ಖಾಲಿ;
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    ಆದರೆ (lua_isstring(L, 1)) s = (char_u *) lua_tostring(L, 1);
    luaV_pushbuffer(L, buflist_new(s, ಖಾಲಿ, 1L, BLN_LISTED));
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_type(lua_State *L)
{
    luaL_checkany(L, 1);
    ಆದರೆ (lua_type(L, 1) == LUA_TUSERDATA) // check vim udata?
    {
	lua_settop(L, 1);
	ಆದರೆ (lua_getmetatable(L, 1))
	{
	    luaV_getfield(L, LUAVIM_LIST);
	    ಆದರೆ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "list");
		ಹಿಂತಿರುಗಿ 1;
	    }
	    luaV_getfield(L, LUAVIM_DICT);
	    ಆದರೆ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "dict");
		ಹಿಂತಿರುಗಿ 1;
	    }
	    luaV_getfield(L, LUAVIM_BLOB);
	    ಆದರೆ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "blob");
		ಹಿಂತಿರುಗಿ 1;
	    }
	    luaV_getfield(L, LUAVIM_FUNCREF);
	    ಆದರೆ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "funcref");
		ಹಿಂತಿರುಗಿ 1;
	    }
	    luaV_getfield(L, LUAVIM_ಬಫರ್);
	    ಆದರೆ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "buffer");
		ಹಿಂತಿರುಗಿ 1;
	    }
	    luaV_getfield(L, LUAVIM_ಕಿಟಕಿ);
	    ಆದರೆ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "window");
		ಹಿಂತಿರುಗಿ 1;
	    }
	}
    }
    lua_pushstring(L, luaL_typename(L, 1)); // fallback
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_call(lua_State *L)
{
    ಪೂರ್ಣಾಂಕ		argc = lua_gettop(L) - 1;
    size_t	funcname_len;
    char_u	*funcname;
    ಅಕ್ಷರ	*error = ಖಾಲಿ;
    typval_T	rettv;
    typval_T	argv[MAX_FUNC_ವಾದಗಳು + 1];
    ಪೂರ್ಣಾಂಕ		i = 0;

    ಆದರೆ (argc > MAX_FUNC_ವಾದಗಳು)
	ಹಿಂತಿರುಗಿ luaL_error(L, "Function called with too many arguments");

    funcname = (char_u *)luaL_checklstring(L, 1, &funcname_len);

    ಗಾಗಿ (; i < argc; i++)
    {
	ಆದರೆ (luaV_totypval(L, i + 2, &argv[i]) == FAIL)
	{
	    error = "lua: cannot convert value";
	    ಹೋಗು free_vim_ವಾದಗಳು;
	}
    }

    argv[argc].v_type = VAR_UNKNOWN;

    ಆದರೆ (call_vim_function(funcname, argc, argv, &rettv) == FAIL)
    {
	error = "lua: call_vim_function failed";
	ಹೋಗು free_vim_ವಾದಗಳು;
    }

    luaV_pushtypval(L, &rettv);
    clear_tv(&rettv);

free_vim_ವಾದಗಳು:
    ಆಗ (i > 0)
	clear_tv(&argv[--i]);

    ಆದರೆ (error == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 1;
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ luaL_error(L, error);
}

/*
 * Return the Vim version as a Lua table
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_V_ಆವೃತ್ತಿ(lua_State *L)
{
    lua_newtable(L);
    lua_pushstring(L, "major");
    lua_pushinteger(L, VIM_ಆವೃತ್ತಿ_MAJOR);
    lua_settable(L, -3);
    lua_pushstring(L, "minor");
    lua_pushinteger(L, VIM_ಆವೃತ್ತಿ_MINOR);
    lua_settable(L, -3);
    lua_pushstring(L, "patch");
    lua_pushinteger(L, highest_patch());
    lua_settable(L, -3);
    ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ luaL_Reg luaV_module[] = {
    {"command", luaV_command},
    {"eval", luaV_eval},
    {"beep", luaV_beep},
    {"line", lua_V_ಸಾಲು},
    {"list", luaV_list},
    {"dict", luaV_dict},
    {"blob", luaV_blob},
    {"funcref", luaV_funcref},
    {"buffer", lua_V_ಬಫರ್_2},
    {"window", lua_V_ಕಿಟಕಿ_2},
    {"open", luaV_open},
    {"type", luaV_type},
    {"call", luaV_call},
    {"_getvar", luaV_getvar},
    {"_setvar", luaV_setvar},
    {"version", lua_V_ಆವೃತ್ತಿ},
    {"lua_version", ಖಾಲಿ},
    {ಖಾಲಿ, ಖಾಲಿ}
};

/*
 * for freeing list, dict, buffer and window objects; lightuserdata as arg
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_free(lua_State *L)
{
    lua_pushnil(L);
    luaV_setudata(L, lua_touserdata(L, 1));
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_luaeval(lua_State *L)
{
    lua_L_ಬಫರ್ b;
    size_t l;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str = lua_tolstring(L, 1, &l);
    typval_T *arg = (typval_T *) lua_touserdata(L, 2);
    typval_T *rettv = (typval_T *) lua_touserdata(L, 3);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, LUAVIM_EVALHEADER, ಗಾತ್ರ(LUAVIM_EVALHEADER) - 1);
    luaL_addlstring(&b, str, l);
    luaL_pushresult(&b);
    str = lua_tolstring(L, -1, &l);
    ಆದರೆ (luaL_loadbuffer(L, str, l, LUAVIM_EVALNAME)) // compile error?
    {
	luaV_emsg(L);
	ಹಿಂತಿರುಗಿ 0;
    }
    luaV_pushtypval(L, arg);
    ಆದರೆ (lua_pcall(L, 1, 1, 0)) // running error?
    {
	luaV_emsg(L);
	ಹಿಂತಿರುಗಿ 0;
    }
    ಆದರೆ (luaV_totypval(L, -1, rettv) == FAIL)
	emsg("luaeval: cannot convert value");
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_setref(lua_State *L)
{
    ಪೂರ್ಣಾಂಕ copyID = lua_tointeger(L, 1);
    ಪೂರ್ಣಾಂಕ abort = FALSE;

    lua_pushlightuserdata(L, (ಶೂನ್ಯ *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // the cache table

    luaV_getfield(L, LUAVIM_LIST);
    luaV_getfield(L, LUAVIM_DICT);
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_pushnil(L);
    // traverse cache table
    ಆಗ (!abort && lua_ಮುಂದಿನ(L, 2) != 0)
    {
	lua_getmetatable(L, -1);
	ಆದರೆ (lua_rawequal(L, -1, 3)) // list?
	{
	    list_T *l = (list_T *)lua_touserdata(L, 6); //ಕೀಲಿ

	    abort = ಗಣ_ref_ಒಳ_list(l, copyID);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lua_rawequal(L, -1, 4)) // dict?
	{
	    dict_T *d = (dict_T *)lua_touserdata(L, 6); //ಕೀಲಿ

	    abort = ಗಣ_ref_ಒಳ_dict(d, copyID);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lua_rawequal(L, -1, 5)) // funcref?
	{
	    luaV_Funcref *f = (luaV_Funcref *)lua_touserdata(L, 6); //ಕೀಲಿ

	    abort = ಗಣ_ref_ಒಳ_dict(f->self, copyID);
	}
	lua_ತೆಗೆ(L, 2); // metatable and value
    }
    lua_pushinteger(L, abort);
    ಹಿಂತಿರುಗಿ 1;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_pushversion(lua_State *L)
{
    ಪೂರ್ಣಾಂಕ major = 0;
    ಪೂರ್ಣಾಂಕ minor = 0;
    ಪೂರ್ಣಾಂಕ patch = 0;
    ಅಕ್ಷರ s[16];

    ಅಕ್ಷರಸರಣಿ_ಓದು(LUAVIM_ಆವೃತ್ತಿ, "Lua %d.%d.%d", &major, &minor, &patch);
    vim_snprintf(s, ಗಾತ್ರ(s), "%d.%d.%d", major, minor, patch);
    lua_pushstring(L, s);
    ಹಿಂತಿರುಗಿ 0;
}

# define LUA_VIM_FN_ಸಂಕೇತ \
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

# define LUA_VIM_ಸಿದ್ಧತೆ_VARIABLE_DICTS \
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

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
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
    //ರಿಜಿಸ್ಟರ್
    luaV_newmetatable(L, LUAVIM_LIST);
    lua_V_ರಿಜಿಸ್ಟರ್(L, luaV_List_mt);
    lua_ತೆಗೆ(L, 1);
    luaV_newmetatable(L, LUAVIM_DICT);
    lua_V_ರಿಜಿಸ್ಟರ್(L, luaV_Dict_mt);
    lua_ತೆಗೆ(L, 1);
    luaV_newmetatable(L, LUAVIM_BLOB);
    lua_V_ರಿಜಿಸ್ಟರ್(L, luaV_Blob_mt);
    lua_ತೆಗೆ(L, 1);
    luaV_newmetatable(L, LUAVIM_FUNCREF);
    lua_V_ರಿಜಿಸ್ಟರ್(L, luaV_Funcref_mt);
    lua_ತೆಗೆ(L, 1);
    luaV_newmetatable(L, LUAVIM_ಬಫರ್);
    lua_V_ರಿಜಿಸ್ಟರ್(L, lua_V_ಬಫರ್_mt);
    lua_ತೆಗೆ(L, 1);
    luaV_newmetatable(L, LUAVIM_ಕಿಟಕಿ);
    lua_V_ರಿಜಿಸ್ಟರ್(L, lua_V_ಕಿಟಕಿ_mt);
    lua_ತೆಗೆ(L, 1);
    lua_newtable(L); // vim table
    lua_V_ರಿಜಿಸ್ಟರ್(L, luaV_module);
    luaV_pushversion(L);
    lua_setfield(L, -2, "lua_version");
    lua_setglobal(L, LUAVIM_NAME);
    // custom code
    (ಶೂನ್ಯ)luaL_dostring(L, LUA_VIM_FN_ಸಂಕೇತ);
    (ಶೂನ್ಯ)luaL_dostring(L, LUA_VIM_UPDATE_PACKAGE_PATHS);
    (ಶೂನ್ಯ)luaL_dostring(L, LUA_VIM_ಸಿದ್ಧತೆ_VARIABLE_DICTS);

    lua_getglobal(L, LUAVIM_NAME);
    lua_getfield(L, -1, "_update_package_paths");

    ಆದರೆ (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);

    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ lua_State *
luaV_newstate(ಶೂನ್ಯ)
{
    lua_State *L = luaL_newstate();
# ಆದರೆ LUA_ಆವೃತ್ತಿ_ಅಂಕೆ >= 505
    luaL_openselectedlibs(L, ~0, 0); // core libs
# ಇಲ್ಲದಿದ್ದರೆ
    luaL_openlibs(L); // core libs
# endif
    lua_pushcfunction(L, luaopen_vim); // vim
    lua_call(L, 0, 0);
    ಹಿಂತಿರುಗಿ L;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
luaV_setrange(lua_State *L, ಪೂರ್ಣಾಂಕ line1, ಪೂರ್ಣಾಂಕ line2)
{
    lua_getglobal(L, LUAVIM_NAME);
    lua_pushinteger(L, line1);
    lua_setfield(L, -2, "firstline");
    lua_pushinteger(L, line2);
    lua_setfield(L, -2, "lastline");
    lua_ತೆಗೆ(L, 1); // vim table
}


// =======   Interface   =======

ಸ್ಥಿರ lua_State *L = ಖಾಲಿ;

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_isopen(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ L != ಖಾಲಿ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lua_init(ಶೂನ್ಯ)
{
    ಆದರೆ (lua_isopen())
	ಹಿಂತಿರುಗಿ OK;

# ifdef DYNAMIC_LUA
    ಆದರೆ (!lua_enabled(TRUE))
    {
	emsg(_("Lua library cannot be loaded."));
	ಹಿಂತಿರುಗಿ FAIL;
    }
# endif
    L = luaV_newstate();

    ಹಿಂತಿರುಗಿ OK;
}

    ಶೂನ್ಯ
lua_ಕೊನೆ(ಶೂನ್ಯ)
{
    ಆದರೆ (!lua_isopen())
	ಹಿಂತಿರುಗಿ;

    lua_close(L);
    L = ಖಾಲಿ;
}

/*
 * ex commands
 */
    ಶೂನ್ಯ
ex_lua(exarg_T *eap)
{
    ಅಕ್ಷರ *script = (ಅಕ್ಷರ *)script_get(eap, eap->arg);

    ಆದರೆ (!eap->skip && lua_init() == OK)
    {
	ಅಕ್ಷರ *s = script != ಖಾಲಿ ? script : (ಅಕ್ಷರ *)eap->arg;

	luaV_setrange(L, eap->line1, eap->line2);
	ಆದರೆ (luaL_loadbuffer(L, s, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s), LUAVIM_CHUNKNAME)
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
    ಆದರೆ (script != ಖಾಲಿ)
	vim_free(script);
}

    ಶೂನ್ಯ
ex_luado(exarg_T *eap)
{
    linenr_T l;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *) eap->arg;
    lua_L_ಬಫರ್ b;
    size_t len;
    buf_T *was_curbuf = curbuf;

    ಆದರೆ (lua_init() == FAIL) ಹಿಂತಿರುಗಿ;
    ಆದರೆ (u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
    {
	emsg(_("cannot save undo information"));
	ಹಿಂತಿರುಗಿ;
    }
    luaV_setrange(L, eap->line1, eap->line2);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, "return function(line, linenr) ", 30); //ಶೀರ್ಷಿಕೆ
    luaL_addlstring(&b, s, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s));
    luaL_addlstring(&b, " end", 4); // footer
    luaL_pushresult(&b);
    s = lua_tolstring(L, -1, &len);
    ಆದರೆ (luaL_loadbuffer(L, s, len, LUAVIM_CHUNKNAME))
    {
	luaV_emsg(L);
	lua_ತೆಗೆ(L, 1); // function body
	ಹಿಂತಿರುಗಿ;
    }
    lua_call(L, 0, 1);
    lua_replace(L, -2); // function -> body
    ಗಾಗಿ (l = eap->line1; l <= eap->line2; l++)
    {
	// Check the line number, the command may have deleted lines.
	ಆದರೆ (l > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ನಿಲ್ಲಿಸು;

	lua_pushvalue(L, -1); // function
	luaV_pushline(L, curbuf, l); // current line as arg
	lua_pushinteger(L, l); // current line number as arg
	ಆದರೆ (lua_pcall(L, 2, 1, 0))
	{
	    luaV_emsg(L);
	    ನಿಲ್ಲಿಸು;
	}

	// Catch the command switching to another buffer.
	// Check the line number, the command may have deleted lines.
	ಆದರೆ (curbuf != was_curbuf || l > curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ)
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (lua_isstring(L, -1)) // update line?
	{
# ifdef HAVE_SANDBOX
	    luaV_checksandbox(L);
# endif
	    ml_replace(l, luaV_toline(L, -1), TRUE);
	    ಬದಲಾಯಿತು_ಬೈಟುಗಳು(l, 0);
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
    ಆದರೆ (lua_init() == FAIL)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (!eap->skip)
    {
	luaV_setrange(L, eap->line1, eap->line2);
	ಆದರೆ (luaL_loadfile(L, (ಅಕ್ಷರ *) eap->arg) || lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
}

# define luaV_freetype(typ,tname) \
	ಶೂನ್ಯ \
    lua_##tname##_free(typ *o) \
    { \
	ಆದರೆ (!lua_isopen()) ಹಿಂತಿರುಗಿ; \
	luaV_getfield(L, LUAVIM_FREE); \
	lua_pushlightuserdata(L, (ಶೂನ್ಯ *) o); \
	lua_call(L, 1, 0); \
    }

luaV_freetype(buf_T, ಬಫರ್)
luaV_freetype(win_T, ಕಿಟಕಿ_2)

    ಶೂನ್ಯ
do_luaeval(char_u *str, typval_T *arg, typval_T *rettv)
{
    lua_init();
    luaV_getfield(L, LUAVIM_LUAEVAL);
    lua_pushstring(L, (ಅಕ್ಷರ *) str);
    lua_pushlightuserdata(L, (ಶೂನ್ಯ *) arg);
    lua_pushlightuserdata(L, (ಶೂನ್ಯ *) rettv);
    lua_call(L, 3, 0);
}

    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_lua(ಪೂರ್ಣಾಂಕ copyID)
{
    ಪೂರ್ಣಾಂಕ aborted = 0;

    ಆದರೆ (!lua_isopen())
	ಹಿಂತಿರುಗಿ 0;

    luaV_getfield(L, LUAVIM_SETREF);
    // call the function with 1 arg, getting 1 result back
    lua_pushinteger(L, copyID);
    lua_call(L, 1, 1);
    // get the result
    aborted = lua_tointeger(L, -1);
    // pop result off the stack
    lua_ತೆಗೆ(L, 1);

    ಹಿಂತಿರುಗಿ aborted;
}

    ಶೂನ್ಯ
update_package_paths_ಒಳ_lua(ಶೂನ್ಯ)
{
    ಆದರೆ (!lua_isopen())
	ಹಿಂತಿರುಗಿ;

    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "_update_package_paths");

    ಆದರೆ (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);
}

/*
 * Native C function callback
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
luaV_call_lua_func(
	ಪೂರ್ಣಾಂಕ	 argcount,
	typval_T *argvars,
	typval_T *rettv,
	ಶೂನ್ಯ	 *state)
{
    ಪೂರ್ಣಾಂಕ i;
    ಪೂರ್ಣಾಂಕ luaargcount = argcount;
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_funcref);

    ಆದರೆ (funcstate->lua_tableref != LUA_NOREF)
    {
	// First arg for metatable __call method is a table
	luaargcount += 1;
	lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    }

    ಗಾಗಿ (i = 0; i < argcount; ++i)
	luaV_pushtypval(funcstate->L, &argvars[i]);

    ಆದರೆ (lua_pcall(funcstate->L, luaargcount, 1, 0))
    {
	luaV_emsg(funcstate->L);
	ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)FCERR_OTHER;
    }

    luaV_checktypval(funcstate->L, -1, rettv, "get return value");
    ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)FCERR_NONE;
}

/*
 * Free up any lua references held by the func state.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
luaV_call_lua_func_free(ಶೂನ್ಯ *state)
{
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_funcref);
    funcstate->L = ಖಾಲಿ;
    ಆದರೆ (funcstate->lua_tableref != LUA_NOREF)
	luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    VIM_CLEAR(funcstate);
}

#ಆದರೆ_ಅಂತ್ಯ
