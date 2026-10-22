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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "version.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <lua.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lualib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lauxlib.h>

#ਜੇ __STDC_VERSION__ >= 199901L
# define LUAV_INLINE ਸਤਰਅੰਦਰ
#ਨਹੀਂਤਾਂ
# define LUAV_INLINE
#ਜੇ_ਅੰਤ

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#ਜੇ defined(FEAT_LUA)

# define LUAVIM_CHUNKNAME "vim chunk"
# define LUAVIM_NAME "vim"
# define LUAVIM_EVALNAME "luaeval"
# define LUAVIM_EVALHEADER "local _A=select(1,...) return "

# ifdef LUA_RELEASE
#  define LUAVIM_ਸੰਸਕਰਣ LUA_RELEASE
# ਨਹੀਂਤਾਂ
#  define LUAVIM_ਸੰਸਕਰਣ LUA_ਸੰਸਕਰਣ
# endif

ਕਿਸਮਪਰਿਭਾਸ਼ਾ buf_T *lua_V_ਬਫਰ;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ win_T *lua_V_ਖਿੜਕੀ;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ dict_T *luaV_Dict;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ list_T *luaV_List;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ blob_T *luaV_Blob;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    char_u	*name;	// funcref
    dict_T	*self;	// selfdict
} luaV_Funcref;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*msgfunc_T)(ਅੱਖਰ *);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਪੂਰਨ lua_funcref;    // ref to a lua func
    ਪੂਰਨ lua_tableref;   // ref to a lua table if metatable else LUA_NOREF. used
			// for __call
    lua_State *L;
} luaV_CFuncState;

ਸਥਿਰ ਅਚਲ ਅੱਖਰ LUAVIM_DICT[] = "dict";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ LUAVIM_LIST[] = "list";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ LUAVIM_BLOB[] = "blob";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ LUAVIM_FUNCREF[] = "funcref";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ LUAVIM_ਬਫਰ[] = "buffer";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ LUAVIM_ਖਿੜਕੀ[] = "window";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ LUAVIM_FREE[] = "luaV_free";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ LUAVIM_LUAEVAL[] = "luaV_luaeval";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ LUAVIM_SETREF[] = "luaV_setref";

ਸਥਿਰ ਅਚਲ ਅੱਖਰ LUA___CALL[] = "__call";

// get/setudata manage references to vim userdata in a cache table through
// object pointers (light userdata). The cache table itself is retrieved
// from the registry.

ਸਥਿਰ ਅਚਲ ਅੱਖਰ LUAVIM_UDATA_CACHE[] = "luaV_udata_cache";

# define luaV_getfield(L, s) \
    lua_pushlightuserdata((L), (ਖਾਲੀ *)(s)); \
    lua_rawget((L), LUA_REGISTRYINDEX)
# define luaV_checksandbox(L) \
    ਜੇ (sandbox) luaL_error((L), "not allowed in sandbox")
# define lua_V_ਸੁਨੇਹਾ(L) luaV_msgfunc((L), (msgfunc_T) ਸੁਨੇਹਾ)
# define luaV_emsg(L) luaV_msgfunc((L), (msgfunc_T) emsg)
# define luaV_checktypval(L, a, v, ਸੁਨੇਹਾ) \
    ਕਰੋ { \
	ਜੇ (luaV_totypval(L, a, v) == FAIL) \
	    luaL_error(L, ਸੁਨੇਹਾ ": cannot convert value"); \
    } ਜਦੋਂ (0)

ਸਥਿਰ luaV_List *luaV_pushlist(lua_State *L, list_T *lis);
ਸਥਿਰ luaV_Dict *luaV_pushdict(lua_State *L, dict_T *dic);
ਸਥਿਰ luaV_Blob *luaV_pushblob(lua_State *L, blob_T *blo);
ਸਥਿਰ luaV_Funcref *luaV_pushfuncref(lua_State *L, char_u *name);
ਸਥਿਰ ਪੂਰਨ luaV_call_lua_func(ਪੂਰਨ argcount, typval_T *argvars, typval_T *rettv, ਖਾਲੀ *state);
ਸਥਿਰ ਖਾਲੀ luaV_call_lua_func_free(ਖਾਲੀ *state);

# ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ <= 501
#  define lua_V_ਰਜਿਸਟਰ(L, l) lua_L_ਰਜਿਸਟਰ(L, ਨਿਰਾ, l)
#  define luaL_typeerror luaL_typerror
# ਨਹੀਂਤਾਂ
#  define lua_V_ਰਜਿਸਟਰ(L, l) luaL_setfuncs(L, l, 0)
# endif

# ifdef DYNAMIC_LUA

#  ifdef MSWIN
#   define ਲੱਦੋ_dll vim_ਲੱਦੋ_Lib
#   define ਚਿੰਨ੍ਹ_from_dll Get_Proc_ਪਤਾ
#   define close_dll FreeLibrary
#   define ਲੱਦੋ_dll_error GetWin32Error
#  ਨਹੀਂਤਾਂ
#   include <dlfcn.h>
#   define HANDLE ਖਾਲੀ*
#   define ਲੱਦੋ_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#   define ਚਿੰਨ੍ਹ_from_dll dlsym
#   define close_dll dlclose
#   define ਲੱਦੋ_dll_error dlerror
#  endif

// lauxlib
#  ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ <= 501
#   define lua_L_ਰਜਿਸਟਰ dll_lua_L_ਰਜਿਸਟਰ
#   define luaL_prepbuffer dll_luaL_prepbuffer
#   define luaL_openlib dll_luaL_openlib
#   define luaL_typerror dll_luaL_typerror
#   define luaL_loadfile dll_luaL_loadfile
#   define luaL_loadbuffer dll_luaL_loadbuffer
#  ਨਹੀਂਤਾਂ
#   define luaL_prepbuffsize dll_luaL_prepbuffsize
#   define luaL_setfuncs dll_luaL_setfuncs
#   define luaL_loadfilex dll_luaL_loadfilex
#   define luaL_loadbufferx dll_luaL_loadbufferx
#   define luaL_argerror dll_luaL_argerror
#  endif
#  ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ >= 504
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
#  ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ <= 501
#   define lua_tonumber dll_lua_tonumber
#   define lua_tointeger dll_lua_tointeger
#   define lua_call dll_lua_call
#   define lua_pcall dll_lua_pcall
#  ਨਹੀਂਤਾਂ
#   define lua_tonumberx dll_lua_tonumberx
#   define lua_tointegerx dll_lua_tointegerx
#   define lua_callk dll_lua_callk
#   define lua_pcallk dll_lua_pcallk
#   define lua_getglobal dll_lua_getglobal
#   define lua_setglobal dll_lua_setglobal
#  endif
#  ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ <= 502
#   define lua_replace dll_lua_replace
#   define lua_remove dll_lua_remove
#  endif
#  ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ >= 503
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
#  ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ >= 504
#   define lua_newuserdatauv dll_lua_newuserdatauv
#  ਨਹੀਂਤਾਂ
#   define lua_newuserdata dll_lua_newuserdata
#  endif
#  define lua_getmetatable dll_lua_getmetatable
#  define lua_setfield dll_lua_setfield
#  define lua_rawset dll_lua_rawset
#  define lua_rawseti dll_lua_rawseti
#  define lua_setmetatable dll_lua_setmetatable
#  define lua_ਅਗਲਾ dll_lua_ਅਗਲਾ
// libs
#  define luaopen_ਅਧਾਰ dll_luaopen_ਅਧਾਰ
#  define luaopen_ਸਾਰਣੀ dll_luaopen_ਸਾਰਣੀ
#  define luaopen_string dll_luaopen_string
#  define luaopen_math dll_luaopen_math
#  define luaopen_io dll_luaopen_io
#  define luaopen_os dll_luaopen_os
#  define luaopen_package dll_luaopen_package
#  define luaopen_debug dll_luaopen_debug
#  ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ >= 505
#   define luaL_openselectedlibs dll_luaL_openselectedlibs
#  ਨਹੀਂਤਾਂ
#   define luaL_openlibs dll_luaL_openlibs
#  endif

// lauxlib
#  ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ <= 501
ਖਾਲੀ (*dll_lua_L_ਰਜਿਸਟਰ) (lua_State *L, ਅਚਲ ਅੱਖਰ *libname, ਅਚਲ luaL_Reg *l);
ਅੱਖਰ *(*dll_luaL_prepbuffer) (lua_L_ਬਫਰ *B);
ਖਾਲੀ (*dll_luaL_openlib) (lua_State *L, ਅਚਲ ਅੱਖਰ *libname, ਅਚਲ luaL_Reg *l, ਪੂਰਨ nup);
ਪੂਰਨ (*dll_luaL_typerror) (lua_State *L, ਪੂਰਨ narg, ਅਚਲ ਅੱਖਰ *tname);
ਪੂਰਨ (*dll_luaL_loadfile) (lua_State *L, ਅਚਲ ਅੱਖਰ *filename);
ਪੂਰਨ (*dll_luaL_loadbuffer) (lua_State *L, ਅਚਲ ਅੱਖਰ *buff, size_t sz, ਅਚਲ ਅੱਖਰ *name);
#  ਨਹੀਂਤਾਂ
ਅੱਖਰ *(*dll_luaL_prepbuffsize) (lua_L_ਬਫਰ *B, size_t sz);
ਖਾਲੀ (*dll_luaL_setfuncs) (lua_State *L, ਅਚਲ luaL_Reg *l, ਪੂਰਨ nup);
ਪੂਰਨ (*dll_luaL_loadfilex) (lua_State *L, ਅਚਲ ਅੱਖਰ *filename, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ);
ਪੂਰਨ (*dll_luaL_loadbufferx) (lua_State *L, ਅਚਲ ਅੱਖਰ *buff, size_t sz, ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਅੱਖਰ *ਬਹੁਲਕ);
ਪੂਰਨ (*dll_luaL_argerror) (lua_State *L, ਪੂਰਨ numarg, ਅਚਲ ਅੱਖਰ *extramsg);
#  endif
#  ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ >= 504
ਪੂਰਨ (*dll_luaL_typeerror) (lua_State *L, ਪੂਰਨ narg, ਅਚਲ ਅੱਖਰ *tname);
#  endif
ਖਾਲੀ (*dll_luaL_checkany) (lua_State *L, ਪੂਰਨ narg);
ਅਚਲ ਅੱਖਰ *(*dll_luaL_checklstring) (lua_State *L, ਪੂਰਨ ਅੰਕ_Arg, size_t *l);
lua_Integer (*dll_luaL_checkinteger) (lua_State *L, ਪੂਰਨ ਅੰਕ_Arg);
lua_Integer (*dll_luaL_optinteger) (lua_State *L, ਪੂਰਨ nArg, lua_Integer def);
ਖਾਲੀ (*dll_luaL_checktype) (lua_State *L, ਪੂਰਨ narg, ਪੂਰਨ t);
ਪੂਰਨ (*dll_luaL_error) (lua_State *L, ਅਚਲ ਅੱਖਰ *fmt, ...);
lua_State *(*dll_luaL_newstate) (ਖਾਲੀ);
ਖਾਲੀ (*dll_luaL_buffinit) (lua_State *L, lua_L_ਬਫਰ *B);
ਖਾਲੀ (*dll_luaL_addlstring) (lua_L_ਬਫਰ *B, ਅਚਲ ਅੱਖਰ *s, size_t l);
ਖਾਲੀ (*dll_luaL_pushresult) (lua_L_ਬਫਰ *B);
ਪੂਰਨ (*dll_luaL_loadstring) (lua_State *L, ਅਚਲ ਅੱਖਰ *s);
ਪੂਰਨ (*dll_luaL_ref) (lua_State *L, ਪੂਰਨ idx);
#  ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ <= 502
ਖਾਲੀ (*dll_luaL_unref) (lua_State *L, ਪੂਰਨ idx, ਪੂਰਨ n);
#  ਨਹੀਂਤਾਂ
ਖਾਲੀ (*dll_luaL_unref) (lua_State *L, ਪੂਰਨ idx, lua_Integer n);
#  endif
// lua
#  ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ <= 501
lua_ਸੰਖਿਆ (*dll_lua_tonumber) (lua_State *L, ਪੂਰਨ idx);
lua_Integer (*dll_lua_tointeger) (lua_State *L, ਪੂਰਨ idx);
ਖਾਲੀ (*dll_lua_call) (lua_State *L, ਪੂਰਨ nargs, ਪੂਰਨ nresults);
ਪੂਰਨ (*dll_lua_pcall) (lua_State *L, ਪੂਰਨ nargs, ਪੂਰਨ nresults, ਪੂਰਨ errfunc);
#  ਨਹੀਂਤਾਂ
lua_ਸੰਖਿਆ (*dll_lua_tonumberx) (lua_State *L, ਪੂਰਨ idx, ਪੂਰਨ *isnum);
lua_Integer (*dll_lua_tointegerx) (lua_State *L, ਪੂਰਨ idx, ਪੂਰਨ *isnum);
ਖਾਲੀ (*dll_lua_callk) (lua_State *L, ਪੂਰਨ nargs, ਪੂਰਨ nresults, ਪੂਰਨ ctx,
	lua_CFunction k);
ਪੂਰਨ (*dll_lua_pcallk) (lua_State *L, ਪੂਰਨ nargs, ਪੂਰਨ nresults, ਪੂਰਨ errfunc,
	ਪੂਰਨ ctx, lua_CFunction k);
ਖਾਲੀ (*dll_lua_getglobal) (lua_State *L, ਅਚਲ ਅੱਖਰ *var);
ਖਾਲੀ (*dll_lua_setglobal) (lua_State *L, ਅਚਲ ਅੱਖਰ *var);
#  endif
#  ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ <= 502
ਖਾਲੀ (*dll_lua_replace) (lua_State *L, ਪੂਰਨ idx);
ਖਾਲੀ (*dll_lua_remove) (lua_State *L, ਪੂਰਨ idx);
#  endif
#  ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ >= 503
ਖਾਲੀ  (*dll_lua_rotate) (lua_State *L, ਪੂਰਨ idx, ਪੂਰਨ n);
ਖਾਲੀ (*dll_lua_copy) (lua_State *L, ਪੂਰਨ fromidx, ਪੂਰਨ toidx);
#  endif
ਅਚਲ ਅੱਖਰ *(*dll_lua_typename) (lua_State *L, ਪੂਰਨ tp);
ਖਾਲੀ       (*dll_lua_close) (lua_State *L);
ਪੂਰਨ (*dll_lua_gettop) (lua_State *L);
ਖਾਲੀ (*dll_lua_settop) (lua_State *L, ਪੂਰਨ idx);
ਖਾਲੀ (*dll_lua_pushvalue) (lua_State *L, ਪੂਰਨ idx);
ਪੂਰਨ (*dll_lua_isnumber) (lua_State *L, ਪੂਰਨ idx);
ਪੂਰਨ (*dll_lua_isstring) (lua_State *L, ਪੂਰਨ idx);
ਪੂਰਨ (*dll_lua_type) (lua_State *L, ਪੂਰਨ idx);
ਪੂਰਨ (*dll_lua_rawequal) (lua_State *L, ਪੂਰਨ idx1, ਪੂਰਨ idx2);
ਪੂਰਨ (*dll_lua_toboolean) (lua_State *L, ਪੂਰਨ idx);
ਅਚਲ ਅੱਖਰ *(*dll_lua_tolstring) (lua_State *L, ਪੂਰਨ idx, size_t *len);
ਖਾਲੀ *(*dll_lua_touserdata) (lua_State *L, ਪੂਰਨ idx);
ਖਾਲੀ (*dll_lua_pushnil) (lua_State *L);
ਖਾਲੀ (*dll_lua_pushnumber) (lua_State *L, lua_ਸੰਖਿਆ n);
ਖਾਲੀ (*dll_lua_pushinteger) (lua_State *L, lua_Integer n);
ਖਾਲੀ (*dll_lua_pushlstring) (lua_State *L, ਅਚਲ ਅੱਖਰ *s, size_t l);
ਖਾਲੀ (*dll_lua_pushstring) (lua_State *L, ਅਚਲ ਅੱਖਰ *s);
ਅਚਲ ਅੱਖਰ *(*dll_lua_pushfstring) (lua_State *L, ਅਚਲ ਅੱਖਰ *fmt, ...);
ਖਾਲੀ (*dll_lua_pushcclosure) (lua_State *L, lua_CFunction fn, ਪੂਰਨ n);
ਖਾਲੀ (*dll_lua_pushboolean) (lua_State *L, ਪੂਰਨ b);
ਖਾਲੀ (*dll_lua_pushlightuserdata) (lua_State *L, ਖਾਲੀ *p);
ਖਾਲੀ (*dll_lua_getfield) (lua_State *L, ਪੂਰਨ idx, ਅਚਲ ਅੱਖਰ *k);
#  ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ <= 502
ਖਾਲੀ (*dll_lua_rawget) (lua_State *L, ਪੂਰਨ idx);
ਖਾਲੀ (*dll_lua_rawgeti) (lua_State *L, ਪੂਰਨ idx, ਪੂਰਨ n);
#  ਨਹੀਂਤਾਂ
ਪੂਰਨ (*dll_lua_rawget) (lua_State *L, ਪੂਰਨ idx);
ਪੂਰਨ (*dll_lua_rawgeti) (lua_State *L, ਪੂਰਨ idx, lua_Integer n);
#  endif
ਖਾਲੀ (*dll_lua_createtable) (lua_State *L, ਪੂਰਨ narr, ਪੂਰਨ nrec);
ਖਾਲੀ (*dll_lua_settable) (lua_State *L, ਪੂਰਨ idx);
#  ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ >= 504
ਖਾਲੀ *(*dll_lua_newuserdatauv) (lua_State *L, size_t sz, ਪੂਰਨ nuvalue);
#  ਨਹੀਂਤਾਂ
ਖਾਲੀ *(*dll_lua_newuserdata) (lua_State *L, size_t sz);
#  endif
ਪੂਰਨ (*dll_lua_getmetatable) (lua_State *L, ਪੂਰਨ objindex);
ਖਾਲੀ (*dll_lua_setfield) (lua_State *L, ਪੂਰਨ idx, ਅਚਲ ਅੱਖਰ *k);
ਖਾਲੀ (*dll_lua_rawset) (lua_State *L, ਪੂਰਨ idx);
#  ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ <= 502
ਖਾਲੀ (*dll_lua_rawseti) (lua_State *L, ਪੂਰਨ idx, ਪੂਰਨ n);
#  ਨਹੀਂਤਾਂ
ਖਾਲੀ (*dll_lua_rawseti) (lua_State *L, ਪੂਰਨ idx, lua_Integer n);
#  endif
ਪੂਰਨ (*dll_lua_setmetatable) (lua_State *L, ਪੂਰਨ objindex);
ਪੂਰਨ (*dll_lua_ਅਗਲਾ) (lua_State *L, ਪੂਰਨ idx);
// libs
ਪੂਰਨ (*dll_luaopen_ਅਧਾਰ) (lua_State *L);
ਪੂਰਨ (*dll_luaopen_ਸਾਰਣੀ) (lua_State *L);
ਪੂਰਨ (*dll_luaopen_string) (lua_State *L);
ਪੂਰਨ (*dll_luaopen_math) (lua_State *L);
ਪੂਰਨ (*dll_luaopen_io) (lua_State *L);
ਪੂਰਨ (*dll_luaopen_os) (lua_State *L);
ਪੂਰਨ (*dll_luaopen_package) (lua_State *L);
ਪੂਰਨ (*dll_luaopen_debug) (lua_State *L);
#  ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ >= 505
ਖਾਲੀ (*dll_luaL_openselectedlibs) (lua_State *L, ਪੂਰਨ ਲੱਦੋ, ਪੂਰਨ preload);
#  ਨਹੀਂਤਾਂ
ਖਾਲੀ (*dll_luaL_openlibs) (lua_State *L);
#  endif

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ **luaV_function;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
    ਅਚਲ ਅੱਖਰ *name;
    luaV_function func;
} luaV_Reg;

ਸਥਿਰ ਅਚਲ luaV_Reg luaV_dll[] = {
    // lauxlib
#  ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ <= 501
    {"luaL_register", (luaV_function) &dll_lua_L_ਰਜਿਸਟਰ},
    {"luaL_prepbuffer", (luaV_function) &dll_luaL_prepbuffer},
    {"luaL_openlib", (luaV_function) &dll_luaL_openlib},
    {"luaL_typerror", (luaV_function) &dll_luaL_typerror},
    {"luaL_loadfile", (luaV_function) &dll_luaL_loadfile},
    {"luaL_loadbuffer", (luaV_function) &dll_luaL_loadbuffer},
#  ਨਹੀਂਤਾਂ
    {"luaL_prepbuffsize", (luaV_function) &dll_luaL_prepbuffsize},
    {"luaL_setfuncs", (luaV_function) &dll_luaL_setfuncs},
    {"luaL_loadfilex", (luaV_function) &dll_luaL_loadfilex},
    {"luaL_loadbufferx", (luaV_function) &dll_luaL_loadbufferx},
    {"luaL_argerror", (luaV_function) &dll_luaL_argerror},
#  endif
#  ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ >= 504
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
#  ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ <= 501
    {"lua_tonumber", (luaV_function) &dll_lua_tonumber},
    {"lua_tointeger", (luaV_function) &dll_lua_tointeger},
    {"lua_call", (luaV_function) &dll_lua_call},
    {"lua_pcall", (luaV_function) &dll_lua_pcall},
#  ਨਹੀਂਤਾਂ
    {"lua_tonumberx", (luaV_function) &dll_lua_tonumberx},
    {"lua_tointegerx", (luaV_function) &dll_lua_tointegerx},
    {"lua_callk", (luaV_function) &dll_lua_callk},
    {"lua_pcallk", (luaV_function) &dll_lua_pcallk},
    {"lua_getglobal", (luaV_function) &dll_lua_getglobal},
    {"lua_setglobal", (luaV_function) &dll_lua_setglobal},
#  endif
#  ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ <= 502
    {"lua_replace", (luaV_function) &dll_lua_replace},
    {"lua_remove", (luaV_function) &dll_lua_remove},
#  endif
#  ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ >= 503
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
#  ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ >= 504
    {"lua_newuserdatauv", (luaV_function) &dll_lua_newuserdatauv},
#  ਨਹੀਂਤਾਂ
    {"lua_newuserdata", (luaV_function) &dll_lua_newuserdata},
#  endif
    {"lua_getmetatable", (luaV_function) &dll_lua_getmetatable},
    {"lua_setfield", (luaV_function) &dll_lua_setfield},
    {"lua_rawset", (luaV_function) &dll_lua_rawset},
    {"lua_rawseti", (luaV_function) &dll_lua_rawseti},
    {"lua_setmetatable", (luaV_function) &dll_lua_setmetatable},
    {"lua_next", (luaV_function) &dll_lua_ਅਗਲਾ},
    // libs
    {"luaopen_base", (luaV_function) &dll_luaopen_ਅਧਾਰ},
    {"luaopen_table", (luaV_function) &dll_luaopen_ਸਾਰਣੀ},
    {"luaopen_string", (luaV_function) &dll_luaopen_string},
    {"luaopen_math", (luaV_function) &dll_luaopen_math},
    {"luaopen_io", (luaV_function) &dll_luaopen_io},
    {"luaopen_os", (luaV_function) &dll_luaopen_os},
    {"luaopen_package", (luaV_function) &dll_luaopen_package},
    {"luaopen_debug", (luaV_function) &dll_luaopen_debug},
#  ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ >= 505
    {"luaL_openselectedlibs", (luaV_function) &dll_luaL_openselectedlibs},
#  ਨਹੀਂਤਾਂ
    {"luaL_openlibs", (luaV_function) &dll_luaL_openlibs},
#  endif
    {ਨਿਰਾ, ਨਿਰਾ}
};

ਸਥਿਰ HANDLE hinstLua = ਨਿਰਾ;

    ਸਥਿਰ ਪੂਰਨ
lua_link_init(ਅੱਖਰ *libname, ਪੂਰਨ verbose)
{
    ਅਚਲ luaV_Reg *reg;
    ਜੇ (hinstLua) ਵਾਪਸ OK;
    hinstLua = ਲੱਦੋ_dll(libname);
    ਜੇ (!hinstLua)
    {
	ਜੇ (verbose)
	    semsg(_(e_could_not_ਲੱਦੋ_library_str_str), libname, ਲੱਦੋ_dll_error());
	ਵਾਪਸ FAIL;
    }
    ਲਈ (reg = luaV_dll; reg->func; reg++)
    {
	ਜੇ ((*reg->func = ਚਿੰਨ੍ਹ_from_dll(hinstLua, reg->name)) == ਨਿਰਾ)
	{
	    close_dll(hinstLua);
	    hinstLua = 0;
	    ਜੇ (verbose)
		semsg(_(e_could_not_ਲੱਦੋ_library_function_str), reg->name);
	    ਵਾਪਸ FAIL;
	}
    }
    ਵਾਪਸ OK;
}
# endif // DYNAMIC_LUA

# ਜੇ defined(DYNAMIC_LUA)
    ਪੂਰਨ
lua_enabled(ਪੂਰਨ verbose)
{
    ਵਾਪਸ lua_link_init((ਅੱਖਰ *)p_luadll, verbose) == OK;
}
# endif

# ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ > 501 && LUA_ਸੰਸਕਰਣ_ਅੰਕ < 504
    ਸਥਿਰ ਪੂਰਨ
luaL_typeerror(lua_State *L, ਪੂਰਨ narg, ਅਚਲ ਅੱਖਰ *tname)
{
    ਅਚਲ ਅੱਖਰ *ਸੁਨੇਹਾ = lua_pushfstring(L, "%s expected, got %s",
	    tname, luaL_typename(L, narg));
    ਵਾਪਸ luaL_argerror(L, narg, ਸੁਨੇਹਾ);
}
# endif

    ਸਥਿਰ LUAV_INLINE ਖਾਲੀ
luaV_getudata(lua_State *L, ਖਾਲੀ *v)
{
    lua_pushlightuserdata(L, (ਖਾਲੀ *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // now the cache table is at the top of the stack
    lua_pushlightuserdata(L, v);
    lua_rawget(L, -2);
    lua_remove(L, -2);  // remove the cache table from the stack
}

    ਸਥਿਰ LUAV_INLINE ਖਾਲੀ
luaV_setudata(lua_State *L, ਖਾਲੀ *v)
{
    lua_pushlightuserdata(L, (ਖਾਲੀ *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // cache table is at -1
    lua_pushlightuserdata(L, v);       // ...now at -2
    lua_pushvalue(L, -3);	       // copy the userdata (cache at -3)
    lua_rawset(L, -3);		       // consumes two stack items
    lua_ਕੱਢੋ(L, 1);		       // and remove the cache table
}

// =======   Internal   =======

    ਸਥਿਰ ਖਾਲੀ
luaV_newmetatable(lua_State *L, ਅਚਲ ਅੱਖਰ *tname)
{
    lua_newtable(L);
    lua_pushlightuserdata(L, (ਖਾਲੀ *) tname);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
}

    ਸਥਿਰ ਖਾਲੀ *
luaV_toudata(lua_State *L, ਪੂਰਨ ud, ਅਚਲ ਅੱਖਰ *tname)
{
    ਖਾਲੀ *p = lua_touserdata(L, ud);

    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // value is userdata
    ਜੇ (lua_getmetatable(L, ud)) // does it have a metatable?
    {
	luaV_getfield(L, tname); // get metatable
	ਜੇ (lua_rawequal(L, -1, -2)) // MTs match?
	{
	    lua_ਕੱਢੋ(L, 2); // MTs
	    ਵਾਪਸ p;
	}
    }

    ਵਾਪਸ ਨਿਰਾ;
}

    ਸਥਿਰ ਖਾਲੀ *
luaV_checkcache(lua_State *L, ਖਾਲੀ *p)
{
    luaV_getudata(L, p);
    ਜੇ (lua_isnil(L, -1)) luaL_error(L, "invalid object");
    lua_ਕੱਢੋ(L, 1);
    ਵਾਪਸ p;
}

# define luaV_unbox(L,luatyp,ud) (*((luatyp *) lua_touserdata((L),(ud))))

# define luaV_checkvalid(L,luatyp,ud) \
    luaV_checkcache((L), (ਖਾਲੀ *) luaV_unbox((L),luatyp,(ud)))

    ਸਥਿਰ ਖਾਲੀ *
luaV_checkudata(lua_State *L, ਪੂਰਨ ud, ਅਚਲ ਅੱਖਰ *tname)
{
    ਖਾਲੀ *p = luaV_toudata(L, ud, tname);
    ਜੇ (p == ਨਿਰਾ) luaL_typeerror(L, ud, tname);
    ਵਾਪਸ p;
}

    ਸਥਿਰ ਖਾਲੀ
luaV_pushtypval(lua_State *L, typval_T *tv)
{
    ਜੇ (tv == ਨਿਰਾ)
    {
	lua_pushnil(L);
	ਵਾਪਸ;
    }
    ਚੋਣ (tv->v_type)
    {
	ਹਾਲਤ VAR_STRING:
	    lua_pushstring(L, tv->vval.v_string == ਨਿਰਾ
					    ? "" : (ਅੱਖਰ *)tv->vval.v_string);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	    lua_pushinteger(L, (ਪੂਰਨ) tv->vval.v_ਸੰਖਿਆ);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_FLOAT:
	    lua_pushnumber(L, (lua_ਸੰਖਿਆ) tv->vval.v_float);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_LIST:
	    luaV_pushlist(L, tv->vval.v_list);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_DICT:
	    luaV_pushdict(L, tv->vval.v_dict);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_BOOL:
	ਹਾਲਤ VAR_SPECIAL:
	    ਜੇ (tv->vval.v_ਸੰਖਿਆ <= VVAL_TRUE)
		lua_pushboolean(L, (ਪੂਰਨ) tv->vval.v_ਸੰਖਿਆ);
	    ਨਹੀਂਤਾਂ
		lua_pushnil(L);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_FUNC:
	    luaV_pushfuncref(L, tv->vval.v_string);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_PARTIAL:
	    // TODO: handle partial arguments
	    luaV_pushfuncref(L, partial_name(tv->vval.v_partial));
	    ਰੋਕੋ;

	ਹਾਲਤ VAR_BLOB:
	    luaV_pushblob(L, tv->vval.v_blob);
	    ਰੋਕੋ;
	ਮੂਲ:
	    lua_pushnil(L);
    }
}

/*
 * Converts lua value at 'pos' to typval 'tv'.
 * Returns OK or FAIL.
 */
    ਸਥਿਰ ਪੂਰਨ
luaV_totypval(lua_State *L, ਪੂਰਨ pos, typval_T *tv)
{
    ਪੂਰਨ ਸਥਿਤੀ = OK;

    tv->v_ਤਾਲਾ = 0;

    ਚੋਣ (lua_type(L, pos))
    {
	ਹਾਲਤ LUA_TBOOLEAN:
	    tv->v_type = VAR_BOOL;
	    tv->vval.v_ਸੰਖਿਆ = (varnumber_T) lua_toboolean(L, pos);
	    ਰੋਕੋ;
	ਹਾਲਤ LUA_TNIL:
	    tv->v_type = VAR_SPECIAL;
	    tv->vval.v_ਸੰਖਿਆ = VVAL_NULL;
	    ਰੋਕੋ;
	ਹਾਲਤ LUA_TSTRING:
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strsave((char_u *) lua_tostring(L, pos));
	    ਰੋਕੋ;
	ਹਾਲਤ LUA_TNUMBER:
	{
	    ਅਚਲ lua_ਸੰਖਿਆ n = lua_tonumber(L, pos);

	    ਜੇ (n > (lua_ਸੰਖਿਆ)INT64_MAX || n < (lua_ਸੰਖਿਆ)INT64_MIN
		    || ((lua_ਸੰਖਿਆ)((varnumber_T)n)) != n)
	    {
		tv->v_type = VAR_FLOAT;
		tv->vval.v_float = (float_T)n;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		tv->v_type = VAR_ਸੰਖਿਆ;
		tv->vval.v_ਸੰਖਿਆ = (varnumber_T)n;
	    }
	}
	    ਰੋਕੋ;
	ਹਾਲਤ LUA_TFUNCTION:
	{
	    char_u *name;
	    luaV_CFuncState *state;

	    lua_pushvalue(L, pos);
	    state = ALLOC_CLEAR_ONE(luaV_CFuncState);
	    state->lua_funcref = luaL_ref(L, LUA_REGISTRYINDEX);
	    state->L = L;
	    state->lua_tableref = LUA_NOREF;
	    name = ਰਜਿਸਟਰ_cfunc(&luaV_call_lua_func,
					      &luaV_call_lua_func_free, state);
	    tv->v_type = VAR_FUNC;
	    tv->vval.v_string = vim_strsave(name);
	    ਰੋਕੋ;
	}
	ਹਾਲਤ LUA_TTABLE:
	{
	    ਪੂਰਨ lua_tableref;

	    lua_pushvalue(L, pos);
	    lua_tableref = luaL_ref(L, LUA_REGISTRYINDEX);
	    ਜੇ (lua_getmetatable(L, pos))
	    {
		lua_getfield(L, -1, LUA___CALL);
		ਜੇ (lua_isfunction(L, -1))
		{
		    char_u *name;
		    ਪੂਰਨ lua_funcref = luaL_ref(L, LUA_REGISTRYINDEX);
		    luaV_CFuncState *state = ALLOC_CLEAR_ONE(luaV_CFuncState);

		    state->lua_funcref = lua_funcref;
		    state->L = L;
		    state->lua_tableref = lua_tableref;
		    name = ਰਜਿਸਟਰ_cfunc(&luaV_call_lua_func,
					      &luaV_call_lua_func_free, state);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(name);
		    ਰੋਕੋ;
		}
	    }
	    tv->v_type = VAR_ਸੰਖਿਆ;
	    tv->vval.v_ਸੰਖਿਆ = 0;
	    ਸਥਿਤੀ = FAIL;
	    ਰੋਕੋ;
	}
	ਹਾਲਤ LUA_TUSERDATA:
	{
	    ਖਾਲੀ *p = lua_touserdata(L, pos);

	    ਜੇ (lua_getmetatable(L, pos)) // has metatable?
	    {
		// check list
		luaV_getfield(L, LUAVIM_LIST);
		ਜੇ (lua_rawequal(L, -1, -2))
		{
		    tv->v_type = VAR_LIST;
		    tv->vval.v_list = *((luaV_List *) p);
		    ++tv->vval.v_list->lv_refcount;
		    lua_ਕੱਢੋ(L, 2); // MTs
		    ਰੋਕੋ;
		}
		// check dict
		luaV_getfield(L, LUAVIM_DICT);
		ਜੇ (lua_rawequal(L, -1, -3))
		{
		    tv->v_type = VAR_DICT;
		    tv->vval.v_dict = *((luaV_Dict *) p);
		    ++tv->vval.v_dict->dv_refcount;
		    lua_ਕੱਢੋ(L, 3); // MTs
		    ਰੋਕੋ;
		}
		// check blob
		luaV_getfield(L, LUAVIM_BLOB);
		ਜੇ (lua_rawequal(L, -1, -4))
		{
		    tv->v_type = VAR_BLOB;
		    tv->vval.v_blob = *((luaV_Blob *) p);
		    ++tv->vval.v_blob->bv_refcount;
		    lua_ਕੱਢੋ(L, 4); // MTs
		    ਰੋਕੋ;
		}
		// check funcref
		luaV_getfield(L, LUAVIM_FUNCREF);
		ਜੇ (lua_rawequal(L, -1, -5))
		{
		    luaV_Funcref *f = (luaV_Funcref *) p;

		    func_ref(f->name);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(f->name);
		    lua_ਕੱਢੋ(L, 5); // MTs
		    ਰੋਕੋ;
		}
		lua_ਕੱਢੋ(L, 4); // MTs
	    }
	}
	// FALLTHROUGH
	ਮੂਲ:
	    tv->v_type = VAR_ਸੰਖਿਆ;
	    tv->vval.v_ਸੰਖਿਆ = 0;
	    ਸਥਿਤੀ = FAIL;
    }
    ਵਾਪਸ ਸਥਿਤੀ;
}

/*
 * similar to luaL_addlstring, but replaces \0 with \n if toline and
 * \n with \0 otherwise
 */
    ਸਥਿਰ ਖਾਲੀ
luaV_addlstring(lua_L_ਬਫਰ *b, ਅਚਲ ਅੱਖਰ *s, size_t l, ਪੂਰਨ toline)
{
    ਜਦੋਂ (l--)
    {
	ਜੇ (*s == '\0' && toline)
	    luaL_addchar(b, '\n');
	ਨਹੀਂਤਾਂ ਜੇ (*s == '\n' && !toline)
	    luaL_addchar(b, '\0');
	ਨਹੀਂਤਾਂ
	    luaL_addchar(b, *s);
	s++;
    }
}

    ਸਥਿਰ ਖਾਲੀ
luaV_pushline(lua_State *L, buf_T *buf, linenr_T n)
{
    ਅਚਲ ਅੱਖਰ *s = (ਅਚਲ ਅੱਖਰ *) ml_get_buf(buf, n, FALSE);
    lua_L_ਬਫਰ b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, ਲੜੀ_ਲੰਬਾਈ(s), 0);
    luaL_pushresult(&b);
}

    ਸਥਿਰ char_u *
luaV_toline(lua_State *L, ਪੂਰਨ pos)
{
    size_t l;
    ਅਚਲ ਅੱਖਰ *s = lua_tolstring(L, pos, &l);

    lua_L_ਬਫਰ b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 1);
    luaL_pushresult(&b);
    ਵਾਪਸ (char_u *) lua_tostring(L, -1);
}

/*
 * pops a string s from the top of the stack and calls mf(t) for pieces t of
 * s separated by newlines
 */
    ਸਥਿਰ ਖਾਲੀ
luaV_msgfunc(lua_State *L, msgfunc_T mf)
{
    lua_L_ਬਫਰ b;
    size_t l;
    ਅਚਲ ਅੱਖਰ *p, *s = lua_tolstring(L, -1, &l);
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 0);
    luaL_pushresult(&b);
    // break string
    p = s = lua_tolstring(L, -1, &l);
    ਜਦੋਂ (l--)
    {
	ਜੇ (*p++ == '\0') // break?
	{
	    mf((ਅੱਖਰ *)s);
	    s = p;
	}
    }
    mf((ਅੱਖਰ *)s);
    lua_ਕੱਢੋ(L, 2); // original and modified strings
}

# define luaV_newtype(typ,tname,luatyp,luatname) \
	ਸਥਿਰ luatyp * \
    luaV_new##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = (luatyp *) lua_newuserdata(L, ਆਕਾਰ(luatyp)); \
	*o = obj; \
	luaV_setudata(L, obj); /* cache[obj] = udata */ \
	luaV_getfield(L, luatname); \
	lua_setmetatable(L, -2); \
	ਵਾਪਸ o; \
    }

# define luaV_pushtype(typ,tname,luatyp) \
	ਸਥਿਰ luatyp * \
    lua_V_ਧੱਕੋ##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = ਨਿਰਾ; \
	ਜੇ (obj == ਨਿਰਾ) \
	    lua_pushnil(L); \
	ਨਹੀਂਤਾਂ \
	{ \
	    luaV_getudata(L, obj); \
	    ਜੇ (lua_isnil(L, -1)) /* not interned? */ \
	    { \
		lua_ਕੱਢੋ(L, 1); \
		o = luaV_new##tname(L, obj); \
	    } \
	    ਨਹੀਂਤਾਂ \
		o = (luatyp *) lua_touserdata(L, -1); \
	} \
	ਵਾਪਸ o; \
    }

# define luaV_type_tostring(tname,luatname) \
	ਸਥਿਰ ਪੂਰਨ \
    luaV_##tname##_tostring(lua_State *L) \
    { \
	lua_pushfstring(L, "%s: %p", luatname, lua_touserdata(L, 1)); \
	ਵਾਪਸ 1; \
    }

// =======   List type   =======

    ਸਥਿਰ luaV_List *
luaV_newlist(lua_State *L, list_T *lis)
{
    luaV_List *l = (luaV_List *) lua_newuserdata(L, ਆਕਾਰ(luaV_List));
    *l = lis;
    lis->lv_refcount++; // reference in Lua
    luaV_setudata(L, lis); // cache[lis] = udata
    luaV_getfield(L, LUAVIM_LIST);
    lua_setmetatable(L, -2);
    ਵਾਪਸ l;
}

luaV_pushtype(list_T, list, luaV_List)
luaV_type_tostring(list, LUAVIM_LIST)

    ਸਥਿਰ ਪੂਰਨ
luaV_list_len(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushinteger(L, (ਪੂਰਨ) list_len(l));
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
luaV_list_iter(lua_State *L)
{
    listitem_T *li = (listitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    ਜੇ (li == ਨਿਰਾ) ਵਾਪਸ 0;
    luaV_pushtypval(L, &li->li_tv);
    lua_pushlightuserdata(L, (ਖਾਲੀ *) li->li_ਅਗਲਾ);
    lua_replace(L, lua_upvalueindex(1));
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
luaV_list_call(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushlightuserdata(L, (ਖਾਲੀ *) l->lv_ਪਹਿਲਾ);
    lua_pushcclosure(L, luaV_list_iter, 1);
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
lua_V_list_ਸੂਚਕਾਂਕ(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    ਜੇ (lua_isnumber(L, 2)) // list item?
    {
	ਲੰਮਾ n = (ਲੰਮਾ) luaL_checkinteger(L, 2);
	listitem_T *li;

	// Lua array index starts with 1 while Vim uses 0, subtract 1 to
	// normalize.
	n -= 1;
	li = list_ਲੱਭੋ(l, n);
	ਜੇ (li == ਨਿਰਾ)
	    lua_pushnil(L);
	ਨਹੀਂਤਾਂ
	    luaV_pushtypval(L, &li->li_tv);
    }
    ਨਹੀਂਤਾਂ ਜੇ (lua_isstring(L, 2)) // method?
    {
	ਅਚਲ ਅੱਖਰ *s = lua_tostring(L, 2);
	ਜੇ (ਲੜੀ_ਤੁਲਨਾ(s, "add", 3) == 0
		|| ਲੜੀ_ਤੁਲਨਾ(s, "insert", 6) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	ਨਹੀਂਤਾਂ
	    lua_pushnil(L);
    }
    ਨਹੀਂਤਾਂ
	lua_pushnil(L);
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
luaV_list_newindex(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    ਲੰਮਾ n = (ਲੰਮਾ) luaL_checkinteger(L, 2);
    listitem_T *li;

    // Lua array index starts with 1 while Vim uses 0, subtract 1 to normalize.
    n -= 1;

    ਜੇ (l->lv_ਤਾਲਾ)
	luaL_error(L, "list is locked");
    li = list_ਲੱਭੋ(l, n);
    ਜੇ (li == ਨਿਰਾ)
    {
	ਜੇ (!lua_isnil(L, 3))
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "inserting list item");
	    ਜੇ (list_ਪਾਓ_tv(l, &v, li) == FAIL)
		luaL_error(L, "failed to add item to list");
	    clear_tv(&v);
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (lua_isnil(L, 3)) // remove?
	{
	    vimlist_remove(l, li, li);
	    listitem_free(l, li);
	}
	ਨਹੀਂਤਾਂ
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "setting list item");
	    clear_tv(&li->li_tv);
	    li->li_tv = v;
	}
    }
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
lua_V_list_ਜੋੜੋ(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (ਖਾਲੀ *) *lis);
    typval_T v;
    ਜੇ (l->lv_ਤਾਲਾ)
	luaL_error(L, "list is locked");
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "adding list item");
    ਜੇ (list_append_tv(l, &v) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
lua_V_list_ਪਾਓ(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (ਖਾਲੀ *) *lis);
    ਲੰਮਾ pos = (ਲੰਮਾ) luaL_optinteger(L, 3, 0);
    listitem_T *li = ਨਿਰਾ;
    typval_T v;
    ਜੇ (l->lv_ਤਾਲਾ)
	luaL_error(L, "list is locked");
    ਜੇ (pos < l->lv_len)
    {
	li = list_ਲੱਭੋ(l, pos);
	ਜੇ (li == ਨਿਰਾ)
	    luaL_error(L, "invalid position");
    }
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "inserting list item");
    ਜੇ (list_ਪਾਓ_tv(l, &v, li) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    ਵਾਪਸ 1;
}

ਸਥਿਰ ਅਚਲ luaL_Reg luaV_List_mt[] = {
    {"__tostring", luaV_list_tostring},
    {"__len", luaV_list_len},
    {"__call", luaV_list_call},
    {"__index", lua_V_list_ਸੂਚਕਾਂਕ},
    {"__newindex", luaV_list_newindex},
    {"add", lua_V_list_ਜੋੜੋ},
    {"insert", lua_V_list_ਪਾਓ},
    {ਨਿਰਾ, ਨਿਰਾ}
};


// =======   Dict type   =======

    ਸਥਿਰ luaV_Dict *
luaV_newdict(lua_State *L, dict_T *dic)
{
    luaV_Dict *d = (luaV_Dict *) lua_newuserdata(L, ਆਕਾਰ(luaV_Dict));
    *d = dic;
    dic->dv_refcount++; // reference in Lua
    luaV_setudata(L, dic); // cache[dic] = udata
    luaV_getfield(L, LUAVIM_DICT);
    lua_setmetatable(L, -2);
    ਵਾਪਸ d;
}

luaV_pushtype(dict_T, dict, luaV_Dict)
luaV_type_tostring(dict, LUAVIM_DICT)

    ਸਥਿਰ ਪੂਰਨ
luaV_dict_len(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    lua_pushinteger(L, (ਪੂਰਨ) dict_len(d));
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
luaV_dict_iter(lua_State *L ਅਣਵਰਤਿਆ)
{
# ifdef FEAT_EVAL
    hashitem_T *hi = (hashitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    ਪੂਰਨ n = lua_tointeger(L, lua_upvalueindex(2));
    dictitem_T *di;
    ਜੇ (n <= 0) ਵਾਪਸ 0;
    ਜਦੋਂ (HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi)) hi++;
    di = dict_lookup(hi);
    lua_pushstring(L, (ਅੱਖਰ *) hi->hi_ਕੁੰਜੀ);
    luaV_pushtypval(L, &di->di_tv);
    lua_pushlightuserdata(L, (ਖਾਲੀ *) (hi + 1));
    lua_replace(L, lua_upvalueindex(1));
    lua_pushinteger(L, n - 1);
    lua_replace(L, lua_upvalueindex(2));
    ਵਾਪਸ 2;
# ਨਹੀਂਤਾਂ
    ਵਾਪਸ 0;
# endif
}

    ਸਥਿਰ ਪੂਰਨ
luaV_dict_call(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    hashtab_T *ht = &d->dv_hashtab;
    lua_pushlightuserdata(L, (ਖਾਲੀ *) ht->ht_array);
    lua_pushinteger(L, ht->ht_ਵਰਤਿਆ); // # remaining items
    lua_pushcclosure(L, luaV_dict_iter, 2);
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
lua_V_dict_ਸੂਚਕਾਂਕ(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *ਕੁੰਜੀ = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di = dict_ਲੱਭੋ(d, ਕੁੰਜੀ, -1);

    ਜੇ (di == ਨਿਰਾ)
    {
	lua_pushnil(L);
	ਵਾਪਸ 1;
    }

    luaV_pushtypval(L, &di->di_tv);
    ਜੇ (di->di_tv.v_type == VAR_FUNC) // funcref?
    {
	luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, -1);
	f->self = d; // keep "self" reference
	d->dv_refcount++;
    }

    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
luaV_dict_newindex(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *ਕੁੰਜੀ = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di;
    typval_T tv;

    ਜੇ (d->dv_ਤਾਲਾ)
	luaL_error(L, "dict is locked");
    ਜੇ (ਕੁੰਜੀ == ਨਿਰਾ)
	ਵਾਪਸ 0;
    ਜੇ (*ਕੁੰਜੀ == NUL)
	luaL_error(L, "empty key");
    ਜੇ (!lua_isnil(L, 3)) // read value?
    {
	luaV_checktypval(L, 3, &tv, "setting dict item");
	ਜੇ (d->dv_scope == VAR_DEF_SCOPE && tv.v_type == VAR_FUNC)
	{
	    clear_tv(&tv);
	    luaL_error(L, "cannot assign funcref to builtin scope");
	}
    }
    di = dict_ਲੱਭੋ(d, ਕੁੰਜੀ, -1);
    ਜੇ (di == ਨਿਰਾ) // non-existing key?
    {
	ਜੇ (lua_isnil(L, 3))
	    ਵਾਪਸ 0;
	di = dictitem_alloc(ਕੁੰਜੀ);
	ਜੇ (di == ਨਿਰਾ)
	{
	    clear_tv(&tv);
	    ਵਾਪਸ 0;
	}
	ਜੇ (dict_ਜੋੜੋ(d, di) == FAIL)
	{
	    vim_free(di);
	    clear_tv(&tv);
	    ਵਾਪਸ 0;
	}
    }
    ਨਹੀਂਤਾਂ
	clear_tv(&di->di_tv);
    ਜੇ (lua_isnil(L, 3)) // remove?
    {
	hashitem_T *hi = hash_ਲੱਭੋ(&d->dv_hashtab, di->di_ਕੁੰਜੀ);
	hash_remove(&d->dv_hashtab, hi, "Lua new index");
	dictitem_free(di);
    }
    ਨਹੀਂਤਾਂ
	di->di_tv = tv;
    ਵਾਪਸ 0;
}

ਸਥਿਰ ਅਚਲ luaL_Reg luaV_Dict_mt[] = {
    {"__tostring", luaV_dict_tostring},
    {"__len", luaV_dict_len},
    {"__call", luaV_dict_call},
    {"__index", lua_V_dict_ਸੂਚਕਾਂਕ},
    {"__newindex", luaV_dict_newindex},
    {ਨਿਰਾ, ਨਿਰਾ}
};


// =======   Blob type   =======

    ਸਥਿਰ luaV_Blob *
luaV_newblob(lua_State *L, blob_T *blo)
{
    luaV_Blob *b = (luaV_Blob *) lua_newuserdata(L, ਆਕਾਰ(luaV_Blob));
    *b = blo;
    blo->bv_refcount++; // reference in Lua
    luaV_setudata(L, blo); // cache[blo] = udata
    luaV_getfield(L, LUAVIM_BLOB);
    lua_setmetatable(L, -2);
    ਵਾਪਸ b;
}

luaV_pushtype(blob_T, blob, luaV_Blob)
luaV_type_tostring(blob, LUAVIM_BLOB)

    ਸਥਿਰ ਪੂਰਨ
luaV_blob_gc(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    blob_unref(b);
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
luaV_blob_len(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    lua_pushinteger(L, (ਪੂਰਨ) blob_len(b));
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
lua_V_blob_ਸੂਚਕਾਂਕ(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    ਜੇ (lua_isnumber(L, 2))
    {
	ਪੂਰਨ idx = luaL_checkinteger(L, 2);
	ਜੇ (idx < 0)
	    // Negative index is relative to the end.
	    idx += (ਪੂਰਨ) blob_len(b);
	ਜੇ (idx >= 0 && idx < blob_len(b))
	    lua_pushnumber(L, (lua_ਸੰਖਿਆ) blob_get(b, idx));
	ਨਹੀਂਤਾਂ
	    lua_pushnil(L);
    }
    ਨਹੀਂਤਾਂ ਜੇ (lua_isstring(L, 2))
    {
	ਅਚਲ ਅੱਖਰ *s = lua_tostring(L, 2);
	ਜੇ (ਲੜੀ_ਤੁਲਨਾ(s, "add", 3) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	ਨਹੀਂਤਾਂ
	    lua_pushnil(L);
    }
    ਨਹੀਂਤਾਂ
	lua_pushnil(L);
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
luaV_blob_newindex(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    ਜੇ (b->bv_ਤਾਲਾ)
	luaL_error(L, "blob is locked");

    ਜੇ (!lua_isnumber(L, 2))
	ਵਾਪਸ 0;

    ਲੰਮਾ len = blob_len(b);
    ਪੂਰਨ idx = luaL_checkinteger(L, 2);
    ਪੂਰਨ val = luaL_checkinteger(L, 3);
    ਜੇ (idx < 0)
	// Negative index is relative to the end.
	idx += (ਪੂਰਨ) len;
    ਜੇ ((idx >= 0 && idx < len)
	    || (idx == len && ga_grow(&b->bv_ga, 1) == OK))
    {
	blob_ਸੈੱਟ(b, idx, (char_u) val);
	ਜੇ (idx == len)
	    ++b->bv_ga.ga_len;
    }
    ਨਹੀਂਤਾਂ
	luaL_error(L, "index out of range");

    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
lua_V_blob_ਜੋੜੋ(lua_State *L)
{
    luaV_Blob *blo = luaV_checkudata(L, 1, LUAVIM_BLOB);
    blob_T *b = (blob_T *) luaV_checkcache(L, (ਖਾਲੀ *) *blo);
    ਜੇ (b->bv_ਤਾਲਾ)
	luaL_error(L, "blob is locked");
    lua_settop(L, 2);
    ਜੇ (!lua_isstring(L, 2))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 2));
    ਨਹੀਂਤਾਂ
    {
	size_t i, l = 0;
	ਅਚਲ ਅੱਖਰ *s = lua_tolstring(L, 2, &l);

	ਜੇ (ga_grow(&b->bv_ga, (ਪੂਰਨ)l) == OK)
	    ਲਈ (i = 0; i < l; ++i)
		ga_append(&b->bv_ga, s[i]);
    }
    lua_settop(L, 1);
    ਵਾਪਸ 1;
}

ਸਥਿਰ ਅਚਲ luaL_Reg luaV_Blob_mt[] = {
    {"__tostring", luaV_blob_tostring},
    {"__gc", luaV_blob_gc},
    {"__len", luaV_blob_len},
    {"__index", lua_V_blob_ਸੂਚਕਾਂਕ},
    {"__newindex", luaV_blob_newindex},
    {"add", lua_V_blob_ਜੋੜੋ},
    {ਨਿਰਾ, ਨਿਰਾ}
};


// =======   Funcref type   =======

    ਸਥਿਰ luaV_Funcref *
luaV_newfuncref(lua_State *L, char_u *name)
{
    luaV_Funcref *f = (luaV_Funcref *)lua_newuserdata(L, ਆਕਾਰ(luaV_Funcref));

    ਜੇ (name != ਨਿਰਾ)
    {
	func_ref(name);
	f->name = vim_strsave(name);
    }
    f->self = ਨਿਰਾ;
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_setmetatable(L, -2);
    ਵਾਪਸ f;
}

    ਸਥਿਰ luaV_Funcref *
luaV_pushfuncref(lua_State *L, char_u *name)
{
    ਵਾਪਸ luaV_newfuncref(L, name);
}


luaV_type_tostring(funcref, LUAVIM_FUNCREF)

    ਸਥਿਰ ਪੂਰਨ
luaV_funcref_gc(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    func_unref(f->name);
    vim_free(f->name);
    // NOTE: Don't call "dict_unref(f->self)", because the dict of "f->self"
    // will be (or has been already) freed by Vim's garbage collection.
    ਵਾਪਸ 0;
}

// equivalent to string(funcref)
    ਸਥਿਰ ਪੂਰਨ
luaV_funcref_len(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    lua_pushstring(L, (ਅਚਲ ਅੱਖਰ *) f->name);
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
luaV_funcref_call(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);
    ਪੂਰਨ i, n = lua_gettop(L) - 1; // #args
    ਪੂਰਨ ਸਥਿਤੀ = FAIL;
    typval_T ਦਲੀਲਾਂ;
    typval_T rettv;

    ਦਲੀਲਾਂ.v_type = VAR_LIST;
    ਦਲੀਲਾਂ.vval.v_list = list_alloc();
    rettv.v_type = VAR_UNKNOWN; // as in clear_tv
    ਜੇ (ਦਲੀਲਾਂ.vval.v_list != ਨਿਰਾ)
    {
	typval_T v;

	ਲਈ (i = 0; i < n; i++)
	{
	    luaV_checktypval(L, i + 2, &v, "calling funcref");
	    list_append_tv(ਦਲੀਲਾਂ.vval.v_list, &v);
	    clear_tv(&v);
	}
	ਸਥਿਤੀ = func_call(f->name, &ਦਲੀਲਾਂ, ਨਿਰਾ, f->self, &rettv);
	ਜੇ (ਸਥਿਤੀ == OK)
	    luaV_pushtypval(L, &rettv);
	clear_tv(&ਦਲੀਲਾਂ);
	clear_tv(&rettv);
    }
    ਜੇ (ਸਥਿਤੀ != OK)
	luaL_error(L, "cannot call funcref");
    ਵਾਪਸ 1;
}

ਸਥਿਰ ਅਚਲ luaL_Reg luaV_Funcref_mt[] = {
    {"__tostring", luaV_funcref_tostring},
    {"__gc", luaV_funcref_gc},
    {"__len", luaV_funcref_len},
    {"__call", luaV_funcref_call},
    {ਨਿਰਾ, ਨਿਰਾ}
};


// =======   Buffer type   =======

luaV_newtype(buf_T, ਬਫਰ, lua_V_ਬਫਰ, LUAVIM_ਬਫਰ)
luaV_pushtype(buf_T, ਬਫਰ, lua_V_ਬਫਰ)
luaV_type_tostring(ਬਫਰ, LUAVIM_ਬਫਰ)

    ਸਥਿਰ ਪੂਰਨ
lua_V_ਬਫਰ_len(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, lua_V_ਬਫਰ, 1);
    lua_pushinteger(L, b->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ);
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
lua_V_ਬਫਰ_call(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, lua_V_ਬਫਰ, 1);
    lua_settop(L, 1);
    ਸੈੱਟ_curbuf(b, DOBUF_ਵੰਡੋ);
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
lua_V_ਬਫਰ_ਸੂਚਕਾਂਕ(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, lua_V_ਬਫਰ, 1);
    linenr_T n = (linenr_T) lua_tointeger(L, 2);
    ਜੇ (n > 0 && n <= b->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	luaV_pushline(L, b, n);
    ਨਹੀਂਤਾਂ ਜੇ (lua_isstring(L, 2))
    {
	ਅਚਲ ਅੱਖਰ *s = lua_tostring(L, 2);
	ਜੇ (ਲੜੀ_ਤੁਲਨਾ(s, "name", 4) == 0)
	    lua_pushstring(L, (b->b_sfname == ਨਿਰਾ)
					? "" : (ਅੱਖਰ *) b->b_sfname);
	ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਤੁਲਨਾ(s, "fname", 5) == 0)
	    lua_pushstring(L, (b->b_ffname == ਨਿਰਾ)
					? "" : (ਅੱਖਰ *) b->b_ffname);
	ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਤੁਲਨਾ(s, "number", 6) == 0)
	    lua_pushinteger(L, b->b_fnum);
	// methods
	ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਤੁਲਨਾ(s,   "insert", 6) == 0
		|| ਲੜੀ_ਤੁਲਨਾ(s, "next", 4) == 0
		|| ਲੜੀ_ਤੁਲਨਾ(s, "previous", 8) == 0
		|| ਲੜੀ_ਤੁਲਨਾ(s, "isvalid", 7) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	ਨਹੀਂਤਾਂ
	    lua_pushnil(L);
    }
    ਨਹੀਂਤਾਂ
	lua_pushnil(L);
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
lua_V_ਬਫਰ_newindex(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, lua_V_ਬਫਰ, 1);
    linenr_T n = (linenr_T) luaL_checkinteger(L, 2);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    ਜੇ (n < 1 || n > b->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	luaL_error(L, "invalid line number");
    ਜੇ (lua_isnil(L, 3)) // delete line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	ਜੇ (u_savedel(n, 1L) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	ਨਹੀਂਤਾਂ ਜੇ (ml_delete(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot delete line");
	}
	ਨਹੀਂਤਾਂ
	{
	    deleted_ਸਤਰਾਂ_mark(n, 1L);
	    ਜੇ (b == curwin->w_ਬਫਰ) // fix cursor in current window?
	    {
		ਜੇ (curwin->w_cursor.lnum >= n)
		{
		    ਜੇ (curwin->w_cursor.lnum > n)
		    {
			curwin->w_cursor.lnum -= 1;
			check_cursor_ਖਾਨਾ();
		    }
		    ਨਹੀਂਤਾਂ
			check_cursor();
		    ਬਦਲਿਆ_cline_bef_curs();
		}
		invalidate_botline();
	    }
	}
	curbuf = buf;
    }
    ਨਹੀਂਤਾਂ ਜੇ (lua_isstring(L, 3)) // update line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	ਜੇ (u_savesub(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	ਨਹੀਂਤਾਂ ਜੇ (ml_replace(n, luaV_toline(L, 3), TRUE) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot replace line");
	}
	ਨਹੀਂਤਾਂ
	    ਬਦਲਿਆ_ਬਾਈਟਾਂ(n, 0);
	curbuf = buf;
	ਜੇ (b == curwin->w_ਬਫਰ)
	    check_cursor_ਖਾਨਾ();
    }
    ਨਹੀਂਤਾਂ
	luaL_error(L, "wrong argument to change line");
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
lua_V_ਬਫਰ_ਪਾਓ(lua_State *L)
{
    lua_V_ਬਫਰ *lb = luaV_checkudata(L, 1, LUAVIM_ਬਫਰ);
    buf_T *b = (buf_T *) luaV_checkcache(L, (ਖਾਲੀ *) *lb);
    linenr_T ਆਖਰੀ = b->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ;
    linenr_T n = (linenr_T) luaL_optinteger(L, 3, ਆਖਰੀ);
    buf_T *buf;
    luaL_checktype(L, 2, LUA_TSTRING);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    // fix insertion line
    ਜੇ (n < 0) n = 0;
    ਜੇ (n > ਆਖਰੀ) n = ਆਖਰੀ;
    // insert
    buf = curbuf;
    curbuf = b;
    ਜੇ (u_save(n, n + 1) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot save undo information");
    }
    ਨਹੀਂਤਾਂ ਜੇ (ml_append(n, luaV_toline(L, 2), 0, FALSE) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot insert line");
    }
    ਨਹੀਂਤਾਂ
	appended_ਸਤਰਾਂ_mark(n, 1L);
    curbuf = buf;
    update_screen(UPD_VALID);
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
lua_V_ਬਫਰ_ਅਗਲਾ(lua_State *L)
{
    lua_V_ਬਫਰ *b = luaV_checkudata(L, 1, LUAVIM_ਬਫਰ);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (ਖਾਲੀ *) *b);
    luaV_pushbuffer(L, buf->b_ਅਗਲਾ);
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
lua_V_ਬਫਰ_ਪਿਛਲਾ(lua_State *L)
{
    lua_V_ਬਫਰ *b = luaV_checkudata(L, 1, LUAVIM_ਬਫਰ);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (ਖਾਲੀ *) *b);
    luaV_pushbuffer(L, buf->b_prev);
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
lua_V_ਬਫਰ_isvalid(lua_State *L)
{
    lua_V_ਬਫਰ *b = luaV_checkudata(L, 1, LUAVIM_ਬਫਰ);
    luaV_getudata(L, *b);
    lua_pushboolean(L, !lua_isnil(L, -1));
    ਵਾਪਸ 1;
}

ਸਥਿਰ ਅਚਲ luaL_Reg lua_V_ਬਫਰ_mt[] = {
    {"__tostring", lua_V_ਬਫਰ_tostring},
    {"__len", lua_V_ਬਫਰ_len},
    {"__call", lua_V_ਬਫਰ_call},
    {"__index", lua_V_ਬਫਰ_ਸੂਚਕਾਂਕ},
    {"__newindex", lua_V_ਬਫਰ_newindex},
    {"insert", lua_V_ਬਫਰ_ਪਾਓ},
    {"next", lua_V_ਬਫਰ_ਅਗਲਾ},
    {"previous", lua_V_ਬਫਰ_ਪਿਛਲਾ},
    {"isvalid", lua_V_ਬਫਰ_isvalid},
    {ਨਿਰਾ, ਨਿਰਾ}
};


// =======   Window type   =======

luaV_newtype(win_T, ਖਿੜਕੀ_2, lua_V_ਖਿੜਕੀ, LUAVIM_ਖਿੜਕੀ)
luaV_pushtype(win_T, ਖਿੜਕੀ_2, lua_V_ਖਿੜਕੀ)
luaV_type_tostring(ਖਿੜਕੀ_2, LUAVIM_ਖਿੜਕੀ)

    ਸਥਿਰ ਪੂਰਨ
lua_V_ਖਿੜਕੀ_call(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, lua_V_ਖਿੜਕੀ, 1);
    lua_settop(L, 1);
    win_goto(w);
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
lua_V_ਖਿੜਕੀ_ਸੂਚਕਾਂਕ(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, lua_V_ਖਿੜਕੀ, 1);
    ਅਚਲ ਅੱਖਰ *s = luaL_checkstring(L, 2);
    ਜੇ (ਲੜੀ_ਤੁਲਨਾ(s, "buffer", 6) == 0)
	luaV_pushbuffer(L, w->w_ਬਫਰ);
    ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਤੁਲਨਾ(s, "line", 4) == 0)
	lua_pushinteger(L, w->w_cursor.lnum);
    ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਤੁਲਨਾ(s, "col", 3) == 0)
	lua_pushinteger(L, w->w_cursor.ਖਾਨਾ + 1);
    ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਤੁਲਨਾ(s, "width", 5) == 0)
	lua_pushinteger(L, w->w_ਚੌੜਾਈ);
    ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਤੁਲਨਾ(s, "height", 6) == 0)
	lua_pushinteger(L, w->w_ਉਚਾਈ);
    // methods
    ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਤੁਲਨਾ(s,   "next", 4) == 0
	    || ਲੜੀ_ਤੁਲਨਾ(s, "previous", 8) == 0
	    || ਲੜੀ_ਤੁਲਨਾ(s, "isvalid", 7) == 0)
    {
	lua_getmetatable(L, 1);
	lua_getfield(L, -1, s);
    }
    ਨਹੀਂਤਾਂ
	lua_pushnil(L);
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
lua_V_ਖਿੜਕੀ_newindex(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, lua_V_ਖਿੜਕੀ, 1);
    ਅਚਲ ਅੱਖਰ *s = luaL_checkstring(L, 2);
    ਪੂਰਨ v = luaL_checkinteger(L, 3);
    ਜੇ (ਲੜੀ_ਤੁਲਨਾ(s, "line", 4) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	ਜੇ (v < 1 || v > w->w_ਬਫਰ->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    luaL_error(L, "line out of range");
	w->w_cursor.lnum = v;
	update_screen(UPD_VALID);
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਤੁਲਨਾ(s, "col", 3) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	w->w_cursor.ਖਾਨਾ = v - 1;
	w->w_ਸੈੱਟ_curswant = true;
	update_screen(UPD_VALID);
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਤੁਲਨਾ(s, "width", 5) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setwidth(v);
	curwin = win;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਤੁਲਨਾ(s, "height", 6) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setheight(v);
	curwin = win;
    }
    ਨਹੀਂਤਾਂ
	luaL_error(L, "invalid window property: `%s'", s);
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
lua_V_ਖਿੜਕੀ_ਅਗਲਾ(lua_State *L)
{
    lua_V_ਖਿੜਕੀ *w = luaV_checkudata(L, 1, LUAVIM_ਖਿੜਕੀ);
    win_T *win = (win_T *) luaV_checkcache(L, (ਖਾਲੀ *) *w);
    luaV_pushwindow(L, win->w_ਅਗਲਾ);
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
lua_V_ਖਿੜਕੀ_ਪਿਛਲਾ(lua_State *L)
{
    lua_V_ਖਿੜਕੀ *w = luaV_checkudata(L, 1, LUAVIM_ਖਿੜਕੀ);
    win_T *win = (win_T *) luaV_checkcache(L, (ਖਾਲੀ *) *w);
    luaV_pushwindow(L, win->w_prev);
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
lua_V_ਖਿੜਕੀ_isvalid(lua_State *L)
{
    lua_V_ਖਿੜਕੀ *w = luaV_checkudata(L, 1, LUAVIM_ਖਿੜਕੀ);
    luaV_getudata(L, *w);
    lua_pushboolean(L, !lua_isnil(L, -1));
    ਵਾਪਸ 1;
}

ਸਥਿਰ ਅਚਲ luaL_Reg lua_V_ਖਿੜਕੀ_mt[] = {
    {"__tostring", lua_V_ਖਿੜਕੀ_tostring},
    {"__call", lua_V_ਖਿੜਕੀ_call},
    {"__index", lua_V_ਖਿੜਕੀ_ਸੂਚਕਾਂਕ},
    {"__newindex", lua_V_ਖਿੜਕੀ_newindex},
    {"next", lua_V_ਖਿੜਕੀ_ਅਗਲਾ},
    {"previous", lua_V_ਖਿੜਕੀ_ਪਿਛਲਾ},
    {"isvalid", lua_V_ਖਿੜਕੀ_isvalid},
    {ਨਿਰਾ, ਨਿਰਾ}
};


// =======   Vim module   =======

    ਸਥਿਰ ਪੂਰਨ
luaV_print(lua_State *L)
{
    ਪੂਰਨ		i, n = lua_gettop(L); // nargs
    ਅਚਲ ਅੱਖਰ	*s;
    size_t	l;
    garray_T	ਸੁਨੇਹਾ_ga;

    ga_init2(&ਸੁਨੇਹਾ_ga, 1, 128);
    lua_getglobal(L, "tostring");
    ਲਈ (i = 1; i <= n; i++)
    {
	lua_pushvalue(L, -1); // tostring
	lua_pushvalue(L, i); // arg
	lua_call(L, 1, 1);
	s = lua_tolstring(L, -1, &l);
	ਜੇ (s == ਨਿਰਾ)
	    ਵਾਪਸ luaL_error(L, "cannot convert to string");
	ਜੇ (i > 1)
	    ga_append(&ਸੁਨੇਹਾ_ga, ' '); // use space instead of tab
	ga_concat_len(&ਸੁਨੇਹਾ_ga, (char_u *)s, l);
	lua_ਕੱਢੋ(L, 1);
    }
    // Replace any "\n" with "\0"
    ਲਈ (i = 0; i < ਸੁਨੇਹਾ_ga.ga_len; i++)
	ਜੇ (((ਅੱਖਰ *)ਸੁਨੇਹਾ_ga.ga_ਅੰਕੜੇ)[i] == '\n')
	    ((ਅੱਖਰ *)ਸੁਨੇਹਾ_ga.ga_ਅੰਕੜੇ)[i] = '\0';
    lua_pushlstring(L, ਸੁਨੇਹਾ_ga.ga_ਅੰਕੜੇ, ਸੁਨੇਹਾ_ga.ga_len);
    ਜੇ (!got_int)
	lua_V_ਸੁਨੇਹਾ(L);

    ga_clear(&ਸੁਨੇਹਾ_ga);
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
luaV_debug(lua_State *L)
{
    lua_settop(L, 0);
    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "eval");
    lua_remove(L, -2); // vim.eval at position 1
    ਲਈ (;;)
    {
	ਅਚਲ ਅੱਖਰ *ਨਿਵੇਸ਼;
	size_t l;
	lua_pushvalue(L, 1); // vim.eval
	lua_pushliteral(L, "input('lua_debug> ')");
	lua_call(L, 1, 1); // return string
	ਨਿਵੇਸ਼ = lua_tolstring(L, -1, &l);
	ਜੇ (l == 0 || strcmp(ਨਿਵੇਸ਼, "cont") == 0)
	    ਵਾਪਸ 0;
	ਸੁਨੇਹਾ_putchar('\n'); // avoid outputting on input line
	ਜੇ (luaL_loadbuffer(L, ਨਿਵੇਸ਼, l, "=(debug command)")
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
	lua_settop(L, 1); // remove eventual returns, but keep vim.eval
    }
}

    ਸਥਿਰ dict_T *
luaV_get_var_scope(lua_State *L)
{
    ਅਚਲ ਅੱਖਰ	*scope = luaL_checkstring(L, 1);
    dict_T	*dict = ਨਿਰਾ;

    ਜੇ (STRICMP((ਅੱਖਰ *)scope, "g") == 0)
	dict = get_globvar_dict();
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP((ਅੱਖਰ *)scope, "v") == 0)
	dict = get_vimvar_dict();
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP((ਅੱਖਰ *)scope, "b") == 0)
	dict = curbuf->b_vars;
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP((ਅੱਖਰ *)scope, "w") == 0)
	dict = curwin->w_vars;
    ਨਹੀਂਤਾਂ ਜੇ (STRICMP((ਅੱਖਰ *)scope, "t") == 0)
	dict = curtab->tp_vars;
    ਨਹੀਂਤਾਂ
    {
	luaL_error(L, "invalid scope %s", scope);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਾਪਸ dict;
}

    ਸਥਿਰ ਪੂਰਨ
luaV_setvar(lua_State *L)
{
    dict_T	*dict;
    dictitem_T	*di;
    size_t	len;
    ਅੱਖਰ	*name;
    ਪੂਰਨ		del;
    ਅੱਖਰ	*error = ਨਿਰਾ;

    name = (ਅੱਖਰ *)luaL_checklstring(L, 3, &len);
    del = (lua_gettop(L) < 4) || lua_isnil(L, 4);

    dict = luaV_get_var_scope(L);
    ਜੇ (dict == ਨਿਰਾ)
	ਵਾਪਸ 0;

    di = dict_ਲੱਭੋ(dict, (char_u *)name, (ਪੂਰਨ)len);
    ਜੇ (di != ਨਿਰਾ)
    {
	ਜੇ (di->di_flags & DI_FLAGS_RO)
	    error = "variable is read-only";
	ਨਹੀਂਤਾਂ ਜੇ (di->di_flags & DI_FLAGS_ਤਾਲਾ)
	    error = "variable is locked";
	ਨਹੀਂਤਾਂ ਜੇ (del && di->di_flags & DI_FLAGS_FIX)
	    error = "variable is fixed";
	ਜੇ (error != ਨਿਰਾ)
	    ਵਾਪਸ luaL_error(L, error);
    }
    ਨਹੀਂਤਾਂ ਜੇ (dict->dv_ਤਾਲਾ)
	ਵਾਪਸ luaL_error(L, "Dictionary is locked");

    ਜੇ (del)
    {
	// Delete the key
	ਜੇ (di == ਨਿਰਾ)
	    // Doesn't exist, nothing to do
	    ਵਾਪਸ 0;
	// Delete the entry
	dictitem_remove(dict, di, "Lua delete variable");
    }
    ਨਹੀਂਤਾਂ
    {
	// Update the key
	typval_T	tv;

	// Convert the lua value to a Vim script type in the temporary variable
	lua_pushvalue(L, 4);
	ਜੇ (luaV_totypval(L, -1, &tv) == FAIL)
	    ਵਾਪਸ luaL_error(L, "Couldn't convert lua value");

	ਜੇ (di == ਨਿਰਾ)
	{
	    // Need to create an entry
	    di = dictitem_alloc((char_u *)name);
	    ਜੇ (di == ਨਿਰਾ)
	    {
		clear_tv(&tv);
		ਵਾਪਸ 0;
	    }
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	    ਜੇ (dict_ਜੋੜੋ(dict, di) == FAIL)
	    {
		dictitem_free(di);
		clear_tv(&tv);
		ਵਾਪਸ luaL_error(L, "Couldn't add to dictionary");
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ type_error = FALSE;
	    ਜੇ (dict == get_vimvar_dict()
	       && !before_ਸੈੱਟ_vvar((char_u *)name, di, &tv, TRUE, &type_error))
	    {
		clear_tv(&tv);
		ਜੇ (type_error)
		    ਵਾਪਸ luaL_error(L,
				"Setting v:%s to value with wrong type", name);
		ਵਾਪਸ 0;
	    }
	    // Clear the old value
	    clear_tv(&di->di_tv);
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	}

	// Clear the temporary variable
	clear_tv(&tv);
    }

    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
luaV_getvar(lua_State *L)
{
    dict_T	*dict = luaV_get_var_scope(L);
    size_t	len;
    ਅਚਲ ਅੱਖਰ	*name = luaL_checklstring(L, 3, &len);
    dictitem_T	*di = dict_ਲੱਭੋ(dict, (char_u *)name, (ਪੂਰਨ)len);

    ਜੇ (di == ਨਿਰਾ)
	ਵਾਪਸ 0;  // nil

    luaV_pushtypval(L, &di->di_tv);
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
luaV_command(lua_State *L)
{
    char_u  *s = vim_strsave((char_u *)luaL_checkstring(L, 1));

    execute_cmds_from_string(s);
    vim_free(s);
    update_screen(UPD_VALID);
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
luaV_eval(lua_State *L)
{
    typval_T *tv = eval_ਵਿਅੰਜਕ((char_u *) luaL_checkstring(L, 1), ਨਿਰਾ);

    ਜੇ (tv == ਨਿਰਾ) luaL_error(L, "invalid expression");
    luaV_pushtypval(L, tv);
    free_tv(tv);
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
luaV_beep(lua_State *L ਅਣਵਰਤਿਆ)
{
    vim_beep(BO_LANG);
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
lua_V_ਸਤਰ(lua_State *L)
{
    luaV_pushline(L, curbuf, curwin->w_cursor.lnum);
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
luaV_list(lua_State *L)
{
    list_T *l;
    ਪੂਰਨ initarg = !lua_isnoneornil(L, 1);

    ਜੇ (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    l = list_alloc();
    ਜੇ (l == ਨਿਰਾ)
    {
	lua_pushnil(L);
	ਵਾਪਸ 1;
    }

    luaV_newlist(L, l);
    ਜੇ (!initarg)
	ਵਾਪਸ 1;

    // traverse table to init list
    ਪੂਰਨ notnil, i = 0;
    typval_T v;
    ਕਰੋ
    {
	lua_rawgeti(L, 1, ++i);
	notnil = !lua_isnil(L, -1);
	ਜੇ (notnil)
	{
	    luaV_checktypval(L, -1, &v, "vim.list");
	    list_append_tv(l, &v);
	    clear_tv(&v);
	}
	lua_ਕੱਢੋ(L, 1); //ਮੁੱਲ
    } ਜਦੋਂ (notnil);

    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
luaV_dict(lua_State *L)
{
    dict_T *d;
    ਪੂਰਨ initarg = !lua_isnoneornil(L, 1);

    ਜੇ (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    d = dict_alloc();
    ਜੇ (d == ਨਿਰਾ)
    {
	lua_pushnil(L);
	ਵਾਪਸ 1;
    }

    luaV_newdict(L, d);
    ਜੇ (!initarg)
	ਵਾਪਸ 1;

    // traverse table to init dict
    lua_pushnil(L);
    ਜਦੋਂ (lua_ਅਗਲਾ(L, 1))
    {
	char_u *ਕੁੰਜੀ;
	dictitem_T *di;
	typval_T v;

	lua_pushvalue(L, -2); // dup key in case it's a number
	ਕੁੰਜੀ = (char_u *) lua_tostring(L, -1);
	ਜੇ (ਕੁੰਜੀ == ਨਿਰਾ)
	{
	    lua_pushnil(L);
	    ਵਾਪਸ 1;
	}
	ਜੇ (*ਕੁੰਜੀ == NUL)
	    luaL_error(L, "table has empty key");
	luaV_checktypval(L, -2, &v, "vim.dict"); //ਮੁੱਲ
	di = dictitem_alloc(ਕੁੰਜੀ);
	ਜੇ (di == ਨਿਰਾ || dict_ਜੋੜੋ(d, di) == FAIL)
	{
	    vim_free(di);
	    lua_pushnil(L);
	    ਵਾਪਸ 1;
	}
	di->di_tv = v;
	lua_ਕੱਢੋ(L, 2); // key copy and value
    }

    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
luaV_blob(lua_State *L)
{
    blob_T *b;
    ਪੂਰਨ initarg = !lua_isnoneornil(L, 1);

    ਜੇ (initarg && !lua_isstring(L, 1))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 1));

    b = blob_alloc();
    ਜੇ (b == ਨਿਰਾ)
    {
	lua_pushnil(L);
	ਵਾਪਸ 1;
    }

    luaV_newblob(L, b);
    ਜੇ (!initarg)
	ਵਾਪਸ 1;

    // traverse table to init blob
    size_t i, l = 0;
    ਅਚਲ ਅੱਖਰ *s = lua_tolstring(L, 1, &l);

    ਜੇ (ga_grow(&b->bv_ga, (ਪੂਰਨ)l) == OK)
	ਲਈ (i = 0; i < l; ++i)
	    ga_append(&b->bv_ga, s[i]);

    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
luaV_funcref(lua_State *L)
{
    ਅਚਲ ਅੱਖਰ *name = luaL_checkstring(L, 1);
    // note: not checking if function exists (needs function_exists)
    ਜੇ (name == ਨਿਰਾ || *name == NUL || VIM_ISDIGIT(*name))
	luaL_error(L, "invalid function name: %s", name);
    luaV_newfuncref(L, (char_u *) name);
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
lua_V_ਬਫਰ_2(lua_State *L)
{
    buf_T *buf;
    ਜੇ (lua_isstring(L, 1)) // get by number or name?
    {
	ਜੇ (lua_isnumber(L, 1)) // by number?
	{
	    ਪੂਰਨ n = lua_tointeger(L, 1);
	    FOR_ALL_BUFFERS(buf)
		ਜੇ (buf->b_fnum == n) ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ // by name
	{
	    size_t l;
	    ਅਚਲ ਅੱਖਰ *s = lua_tolstring(L, 1, &l);
	    FOR_ALL_BUFFERS(buf)
	    {
		ਜੇ (buf->b_ffname == ਨਿਰਾ || buf->b_sfname == ਨਿਰਾ)
		{
		    ਜੇ (l == 0) ਰੋਕੋ;
		}
		ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਤੁਲਨਾ(s, (ਅੱਖਰ *)buf->b_ffname, l) == 0
			|| ਲੜੀ_ਤੁਲਨਾ(s, (ਅੱਖਰ *)buf->b_sfname, l) == 0)
		    ਰੋਕੋ;
	    }
	}
    }
    ਨਹੀਂਤਾਂ
	buf = (lua_toboolean(L, 1)) ? firstbuf : curbuf; // first buffer?
    luaV_pushbuffer(L, buf);
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
lua_V_ਖਿੜਕੀ_2(lua_State *L)
{
    win_T *win;
    ਜੇ (lua_isnumber(L, 1)) // get by number?
    {
	ਪੂਰਨ n = lua_tointeger(L, 1);
	ਲਈ (win = firstwin; win != ਨਿਰਾ; win = win->w_ਅਗਲਾ, n--)
	    ਜੇ (n == 1) ਰੋਕੋ;
    }
    ਨਹੀਂਤਾਂ
	win = (lua_toboolean(L, 1)) ? firstwin : curwin; // first window?
    luaV_pushwindow(L, win);
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
luaV_open(lua_State *L)
{
    char_u *s = ਨਿਰਾ;
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    ਜੇ (lua_isstring(L, 1)) s = (char_u *) lua_tostring(L, 1);
    luaV_pushbuffer(L, buflist_new(s, ਨਿਰਾ, 1L, BLN_LISTED));
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
luaV_type(lua_State *L)
{
    luaL_checkany(L, 1);
    ਜੇ (lua_type(L, 1) == LUA_TUSERDATA) // check vim udata?
    {
	lua_settop(L, 1);
	ਜੇ (lua_getmetatable(L, 1))
	{
	    luaV_getfield(L, LUAVIM_LIST);
	    ਜੇ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "list");
		ਵਾਪਸ 1;
	    }
	    luaV_getfield(L, LUAVIM_DICT);
	    ਜੇ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "dict");
		ਵਾਪਸ 1;
	    }
	    luaV_getfield(L, LUAVIM_BLOB);
	    ਜੇ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "blob");
		ਵਾਪਸ 1;
	    }
	    luaV_getfield(L, LUAVIM_FUNCREF);
	    ਜੇ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "funcref");
		ਵਾਪਸ 1;
	    }
	    luaV_getfield(L, LUAVIM_ਬਫਰ);
	    ਜੇ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "buffer");
		ਵਾਪਸ 1;
	    }
	    luaV_getfield(L, LUAVIM_ਖਿੜਕੀ);
	    ਜੇ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "window");
		ਵਾਪਸ 1;
	    }
	}
    }
    lua_pushstring(L, luaL_typename(L, 1)); // fallback
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
luaV_call(lua_State *L)
{
    ਪੂਰਨ		argc = lua_gettop(L) - 1;
    size_t	funcname_len;
    char_u	*funcname;
    ਅੱਖਰ	*error = ਨਿਰਾ;
    typval_T	rettv;
    typval_T	argv[MAX_FUNC_ਦਲੀਲਾਂ + 1];
    ਪੂਰਨ		i = 0;

    ਜੇ (argc > MAX_FUNC_ਦਲੀਲਾਂ)
	ਵਾਪਸ luaL_error(L, "Function called with too many arguments");

    funcname = (char_u *)luaL_checklstring(L, 1, &funcname_len);

    ਲਈ (; i < argc; i++)
    {
	ਜੇ (luaV_totypval(L, i + 2, &argv[i]) == FAIL)
	{
	    error = "lua: cannot convert value";
	    ਜਾਓ free_vim_ਦਲੀਲਾਂ;
	}
    }

    argv[argc].v_type = VAR_UNKNOWN;

    ਜੇ (call_vim_function(funcname, argc, argv, &rettv) == FAIL)
    {
	error = "lua: call_vim_function failed";
	ਜਾਓ free_vim_ਦਲੀਲਾਂ;
    }

    luaV_pushtypval(L, &rettv);
    clear_tv(&rettv);

free_vim_ਦਲੀਲਾਂ:
    ਜਦੋਂ (i > 0)
	clear_tv(&argv[--i]);

    ਜੇ (error == ਨਿਰਾ)
	ਵਾਪਸ 1;
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ luaL_error(L, error);
}

/*
 * Return the Vim version as a Lua table
 */
    ਸਥਿਰ ਪੂਰਨ
lua_V_ਸੰਸਕਰਣ(lua_State *L)
{
    lua_newtable(L);
    lua_pushstring(L, "major");
    lua_pushinteger(L, VIM_ਸੰਸਕਰਣ_MAJOR);
    lua_settable(L, -3);
    lua_pushstring(L, "minor");
    lua_pushinteger(L, VIM_ਸੰਸਕਰਣ_MINOR);
    lua_settable(L, -3);
    lua_pushstring(L, "patch");
    lua_pushinteger(L, highest_patch());
    lua_settable(L, -3);
    ਵਾਪਸ 1;
}

ਸਥਿਰ ਅਚਲ luaL_Reg luaV_module[] = {
    {"command", luaV_command},
    {"eval", luaV_eval},
    {"beep", luaV_beep},
    {"line", lua_V_ਸਤਰ},
    {"list", luaV_list},
    {"dict", luaV_dict},
    {"blob", luaV_blob},
    {"funcref", luaV_funcref},
    {"buffer", lua_V_ਬਫਰ_2},
    {"window", lua_V_ਖਿੜਕੀ_2},
    {"open", luaV_open},
    {"type", luaV_type},
    {"call", luaV_call},
    {"_getvar", luaV_getvar},
    {"_setvar", luaV_setvar},
    {"version", lua_V_ਸੰਸਕਰਣ},
    {"lua_version", ਨਿਰਾ},
    {ਨਿਰਾ, ਨਿਰਾ}
};

/*
 * for freeing list, dict, buffer and window objects; lightuserdata as arg
 */
    ਸਥਿਰ ਪੂਰਨ
luaV_free(lua_State *L)
{
    lua_pushnil(L);
    luaV_setudata(L, lua_touserdata(L, 1));
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
luaV_luaeval(lua_State *L)
{
    lua_L_ਬਫਰ b;
    size_t l;
    ਅਚਲ ਅੱਖਰ *str = lua_tolstring(L, 1, &l);
    typval_T *arg = (typval_T *) lua_touserdata(L, 2);
    typval_T *rettv = (typval_T *) lua_touserdata(L, 3);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, LUAVIM_EVALHEADER, ਆਕਾਰ(LUAVIM_EVALHEADER) - 1);
    luaL_addlstring(&b, str, l);
    luaL_pushresult(&b);
    str = lua_tolstring(L, -1, &l);
    ਜੇ (luaL_loadbuffer(L, str, l, LUAVIM_EVALNAME)) // compile error?
    {
	luaV_emsg(L);
	ਵਾਪਸ 0;
    }
    luaV_pushtypval(L, arg);
    ਜੇ (lua_pcall(L, 1, 1, 0)) // running error?
    {
	luaV_emsg(L);
	ਵਾਪਸ 0;
    }
    ਜੇ (luaV_totypval(L, -1, rettv) == FAIL)
	emsg("luaeval: cannot convert value");
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
luaV_setref(lua_State *L)
{
    ਪੂਰਨ copyID = lua_tointeger(L, 1);
    ਪੂਰਨ abort = FALSE;

    lua_pushlightuserdata(L, (ਖਾਲੀ *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // the cache table

    luaV_getfield(L, LUAVIM_LIST);
    luaV_getfield(L, LUAVIM_DICT);
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_pushnil(L);
    // traverse cache table
    ਜਦੋਂ (!abort && lua_ਅਗਲਾ(L, 2) != 0)
    {
	lua_getmetatable(L, -1);
	ਜੇ (lua_rawequal(L, -1, 3)) // list?
	{
	    list_T *l = (list_T *)lua_touserdata(L, 6); //ਕੁੰਜੀ

	    abort = ਸੈੱਟ_ref_ਵਿੱਚ_list(l, copyID);
	}
	ਨਹੀਂਤਾਂ ਜੇ (lua_rawequal(L, -1, 4)) // dict?
	{
	    dict_T *d = (dict_T *)lua_touserdata(L, 6); //ਕੁੰਜੀ

	    abort = ਸੈੱਟ_ref_ਵਿੱਚ_dict(d, copyID);
	}
	ਨਹੀਂਤਾਂ ਜੇ (lua_rawequal(L, -1, 5)) // funcref?
	{
	    luaV_Funcref *f = (luaV_Funcref *)lua_touserdata(L, 6); //ਕੁੰਜੀ

	    abort = ਸੈੱਟ_ref_ਵਿੱਚ_dict(f->self, copyID);
	}
	lua_ਕੱਢੋ(L, 2); // metatable and value
    }
    lua_pushinteger(L, abort);
    ਵਾਪਸ 1;
}

    ਸਥਿਰ ਪੂਰਨ
luaV_pushversion(lua_State *L)
{
    ਪੂਰਨ major = 0;
    ਪੂਰਨ minor = 0;
    ਪੂਰਨ patch = 0;
    ਅੱਖਰ s[16];

    ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(LUAVIM_ਸੰਸਕਰਣ, "Lua %d.%d.%d", &major, &minor, &patch);
    vim_snprintf(s, ਆਕਾਰ(s), "%d.%d.%d", major, minor, patch);
    lua_pushstring(L, s);
    ਵਾਪਸ 0;
}

# define LUA_VIM_FN_ਕੋਡ \
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

# define LUA_VIM_ਤਿਆਰੀ_VARIABLE_DICTS \
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

    ਸਥਿਰ ਪੂਰਨ
luaopen_vim(lua_State *L)
{
    lua_newtable(L);  // cache table
    lua_newtable(L);  // cache table's metatable
    lua_pushstring(L, "v");
    lua_setfield(L, -2, "__mode");
    lua_setmetatable(L, -2); // cache is weak-valued
    // put the cache table in the registry for luaV_get/setudata()
    lua_pushlightuserdata(L, (ਖਾਲੀ *) LUAVIM_UDATA_CACHE);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
    lua_ਕੱਢੋ(L, 1);  // we don't need the cache table here anymore
    // print
    lua_pushcfunction(L, luaV_print);
    lua_setglobal(L, "print");
    // debug.debug
    lua_getglobal(L, "debug");
    lua_pushcfunction(L, luaV_debug);
    lua_setfield(L, -2, "debug");
    lua_ਕੱਢੋ(L, 1);
    // free
    lua_pushlightuserdata(L, (ਖਾਲੀ *) LUAVIM_FREE);
    lua_pushcfunction(L, luaV_free);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // luaeval
    lua_pushlightuserdata(L, (ਖਾਲੀ *) LUAVIM_LUAEVAL);
    lua_pushcfunction(L, luaV_luaeval);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // setref
    lua_pushlightuserdata(L, (ਖਾਲੀ *) LUAVIM_SETREF);
    lua_pushcfunction(L, luaV_setref);
    lua_rawset(L, LUA_REGISTRYINDEX);
    //ਰਜਿਸਟਰ
    luaV_newmetatable(L, LUAVIM_LIST);
    lua_V_ਰਜਿਸਟਰ(L, luaV_List_mt);
    lua_ਕੱਢੋ(L, 1);
    luaV_newmetatable(L, LUAVIM_DICT);
    lua_V_ਰਜਿਸਟਰ(L, luaV_Dict_mt);
    lua_ਕੱਢੋ(L, 1);
    luaV_newmetatable(L, LUAVIM_BLOB);
    lua_V_ਰਜਿਸਟਰ(L, luaV_Blob_mt);
    lua_ਕੱਢੋ(L, 1);
    luaV_newmetatable(L, LUAVIM_FUNCREF);
    lua_V_ਰਜਿਸਟਰ(L, luaV_Funcref_mt);
    lua_ਕੱਢੋ(L, 1);
    luaV_newmetatable(L, LUAVIM_ਬਫਰ);
    lua_V_ਰਜਿਸਟਰ(L, lua_V_ਬਫਰ_mt);
    lua_ਕੱਢੋ(L, 1);
    luaV_newmetatable(L, LUAVIM_ਖਿੜਕੀ);
    lua_V_ਰਜਿਸਟਰ(L, lua_V_ਖਿੜਕੀ_mt);
    lua_ਕੱਢੋ(L, 1);
    lua_newtable(L); // vim table
    lua_V_ਰਜਿਸਟਰ(L, luaV_module);
    luaV_pushversion(L);
    lua_setfield(L, -2, "lua_version");
    lua_setglobal(L, LUAVIM_NAME);
    // custom code
    (ਖਾਲੀ)luaL_dostring(L, LUA_VIM_FN_ਕੋਡ);
    (ਖਾਲੀ)luaL_dostring(L, LUA_VIM_UPDATE_PACKAGE_PATHS);
    (ਖਾਲੀ)luaL_dostring(L, LUA_VIM_ਤਿਆਰੀ_VARIABLE_DICTS);

    lua_getglobal(L, LUAVIM_NAME);
    lua_getfield(L, -1, "_update_package_paths");

    ਜੇ (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);

    ਵਾਪਸ 0;
}

    ਸਥਿਰ lua_State *
luaV_newstate(ਖਾਲੀ)
{
    lua_State *L = luaL_newstate();
# ਜੇ LUA_ਸੰਸਕਰਣ_ਅੰਕ >= 505
    luaL_openselectedlibs(L, ~0, 0); // core libs
# ਨਹੀਂਤਾਂ
    luaL_openlibs(L); // core libs
# endif
    lua_pushcfunction(L, luaopen_vim); // vim
    lua_call(L, 0, 0);
    ਵਾਪਸ L;
}

    ਸਥਿਰ ਖਾਲੀ
luaV_setrange(lua_State *L, ਪੂਰਨ line1, ਪੂਰਨ line2)
{
    lua_getglobal(L, LUAVIM_NAME);
    lua_pushinteger(L, line1);
    lua_setfield(L, -2, "firstline");
    lua_pushinteger(L, line2);
    lua_setfield(L, -2, "lastline");
    lua_ਕੱਢੋ(L, 1); // vim table
}


// =======   Interface   =======

ਸਥਿਰ lua_State *L = ਨਿਰਾ;

    ਸਥਿਰ ਪੂਰਨ
lua_isopen(ਖਾਲੀ)
{
    ਵਾਪਸ L != ਨਿਰਾ;
}

    ਸਥਿਰ ਪੂਰਨ
lua_init(ਖਾਲੀ)
{
    ਜੇ (lua_isopen())
	ਵਾਪਸ OK;

# ifdef DYNAMIC_LUA
    ਜੇ (!lua_enabled(TRUE))
    {
	emsg(_("Lua library cannot be loaded."));
	ਵਾਪਸ FAIL;
    }
# endif
    L = luaV_newstate();

    ਵਾਪਸ OK;
}

    ਖਾਲੀ
lua_ਸਿਰਾ(ਖਾਲੀ)
{
    ਜੇ (!lua_isopen())
	ਵਾਪਸ;

    lua_close(L);
    L = ਨਿਰਾ;
}

/*
 * ex commands
 */
    ਖਾਲੀ
ex_lua(exarg_T *eap)
{
    ਅੱਖਰ *script = (ਅੱਖਰ *)script_get(eap, eap->arg);

    ਜੇ (!eap->skip && lua_init() == OK)
    {
	ਅੱਖਰ *s = script != ਨਿਰਾ ? script : (ਅੱਖਰ *)eap->arg;

	luaV_setrange(L, eap->line1, eap->line2);
	ਜੇ (luaL_loadbuffer(L, s, ਲੜੀ_ਲੰਬਾਈ(s), LUAVIM_CHUNKNAME)
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
    ਜੇ (script != ਨਿਰਾ)
	vim_free(script);
}

    ਖਾਲੀ
ex_luado(exarg_T *eap)
{
    linenr_T l;
    ਅਚਲ ਅੱਖਰ *s = (ਅਚਲ ਅੱਖਰ *) eap->arg;
    lua_L_ਬਫਰ b;
    size_t len;
    buf_T *was_curbuf = curbuf;

    ਜੇ (lua_init() == FAIL) ਵਾਪਸ;
    ਜੇ (u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
    {
	emsg(_("cannot save undo information"));
	ਵਾਪਸ;
    }
    luaV_setrange(L, eap->line1, eap->line2);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, "return function(line, linenr) ", 30); //ਸਿਰਲੇਖ
    luaL_addlstring(&b, s, ਲੜੀ_ਲੰਬਾਈ(s));
    luaL_addlstring(&b, " end", 4); // footer
    luaL_pushresult(&b);
    s = lua_tolstring(L, -1, &len);
    ਜੇ (luaL_loadbuffer(L, s, len, LUAVIM_CHUNKNAME))
    {
	luaV_emsg(L);
	lua_ਕੱਢੋ(L, 1); // function body
	ਵਾਪਸ;
    }
    lua_call(L, 0, 1);
    lua_replace(L, -2); // function -> body
    ਲਈ (l = eap->line1; l <= eap->line2; l++)
    {
	// Check the line number, the command may have deleted lines.
	ਜੇ (l > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    ਰੋਕੋ;

	lua_pushvalue(L, -1); // function
	luaV_pushline(L, curbuf, l); // current line as arg
	lua_pushinteger(L, l); // current line number as arg
	ਜੇ (lua_pcall(L, 2, 1, 0))
	{
	    luaV_emsg(L);
	    ਰੋਕੋ;
	}

	// Catch the command switching to another buffer.
	// Check the line number, the command may have deleted lines.
	ਜੇ (curbuf != was_curbuf || l > curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ)
	    ਰੋਕੋ;

	ਜੇ (lua_isstring(L, -1)) // update line?
	{
# ifdef HAVE_SANDBOX
	    luaV_checksandbox(L);
# endif
	    ml_replace(l, luaV_toline(L, -1), TRUE);
	    ਬਦਲਿਆ_ਬਾਈਟਾਂ(l, 0);
	    lua_ਕੱਢੋ(L, 1); // result from luaV_toline
	}
	lua_ਕੱਢੋ(L, 1); //ਸਤਰ
    }
    lua_ਕੱਢੋ(L, 1); // function
    check_cursor();
    update_screen(UPD_NOT_VALID);
}

    ਖਾਲੀ
ex_luafile(exarg_T *eap)
{
    ਜੇ (lua_init() == FAIL)
	ਵਾਪਸ;
    ਜੇ (!eap->skip)
    {
	luaV_setrange(L, eap->line1, eap->line2);
	ਜੇ (luaL_loadfile(L, (ਅੱਖਰ *) eap->arg) || lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
}

# define luaV_freetype(typ,tname) \
	ਖਾਲੀ \
    lua_##tname##_free(typ *o) \
    { \
	ਜੇ (!lua_isopen()) ਵਾਪਸ; \
	luaV_getfield(L, LUAVIM_FREE); \
	lua_pushlightuserdata(L, (ਖਾਲੀ *) o); \
	lua_call(L, 1, 0); \
    }

luaV_freetype(buf_T, ਬਫਰ)
luaV_freetype(win_T, ਖਿੜਕੀ_2)

    ਖਾਲੀ
do_luaeval(char_u *str, typval_T *arg, typval_T *rettv)
{
    lua_init();
    luaV_getfield(L, LUAVIM_LUAEVAL);
    lua_pushstring(L, (ਅੱਖਰ *) str);
    lua_pushlightuserdata(L, (ਖਾਲੀ *) arg);
    lua_pushlightuserdata(L, (ਖਾਲੀ *) rettv);
    lua_call(L, 3, 0);
}

    ਪੂਰਨ
ਸੈੱਟ_ref_ਵਿੱਚ_lua(ਪੂਰਨ copyID)
{
    ਪੂਰਨ aborted = 0;

    ਜੇ (!lua_isopen())
	ਵਾਪਸ 0;

    luaV_getfield(L, LUAVIM_SETREF);
    // call the function with 1 arg, getting 1 result back
    lua_pushinteger(L, copyID);
    lua_call(L, 1, 1);
    // get the result
    aborted = lua_tointeger(L, -1);
    // pop result off the stack
    lua_ਕੱਢੋ(L, 1);

    ਵਾਪਸ aborted;
}

    ਖਾਲੀ
update_package_paths_ਵਿੱਚ_lua(ਖਾਲੀ)
{
    ਜੇ (!lua_isopen())
	ਵਾਪਸ;

    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "_update_package_paths");

    ਜੇ (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);
}

/*
 * Native C function callback
 */
    ਸਥਿਰ ਪੂਰਨ
luaV_call_lua_func(
	ਪੂਰਨ	 argcount,
	typval_T *argvars,
	typval_T *rettv,
	ਖਾਲੀ	 *state)
{
    ਪੂਰਨ i;
    ਪੂਰਨ luaargcount = argcount;
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_funcref);

    ਜੇ (funcstate->lua_tableref != LUA_NOREF)
    {
	// First arg for metatable __call method is a table
	luaargcount += 1;
	lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    }

    ਲਈ (i = 0; i < argcount; ++i)
	luaV_pushtypval(funcstate->L, &argvars[i]);

    ਜੇ (lua_pcall(funcstate->L, luaargcount, 1, 0))
    {
	luaV_emsg(funcstate->L);
	ਵਾਪਸ (ਪੂਰਨ)FCERR_OTHER;
    }

    luaV_checktypval(funcstate->L, -1, rettv, "get return value");
    ਵਾਪਸ (ਪੂਰਨ)FCERR_NONE;
}

/*
 * Free up any lua references held by the func state.
 */
    ਸਥਿਰ ਖਾਲੀ
luaV_call_lua_func_free(ਖਾਲੀ *state)
{
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_funcref);
    funcstate->L = ਨਿਰਾ;
    ਜੇ (funcstate->lua_tableref != LUA_NOREF)
	luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    VIM_CLEAR(funcstate);
}

#ਜੇ_ਅੰਤ
