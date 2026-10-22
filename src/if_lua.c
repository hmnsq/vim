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

#അന്തര്ഭാവ "vim.h"
#അന്തര്ഭാവ "version.h"

#അന്തര്ഭാവ <lua.h>
#അന്തര്ഭാവ <lualib.h>
#അന്തര്ഭാവ <lauxlib.h>

#എങ്കിൽ __STDC_VERSION__ >= 199901L
# define LUAV_INLINE വരിയിൽ
#അല്ലെങ്കിൽ
# define LUAV_INLINE
#endif

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#എങ്കിൽ defined(FEAT_LUA)

# define LUAVIM_CHUNKNAME "vim chunk"
# define LUAVIM_NAME "vim"
# define LUAVIM_EVALNAME "luaeval"
# define LUAVIM_EVALHEADER "local _A=select(1,...) return "

# ifdef LUA_RELEASE
#  define LUAVIM_VERSION LUA_RELEASE
# അല്ലെങ്കിൽ
#  define LUAVIM_VERSION LUA_VERSION
# endif

തരനിർവചനം buf_T *luaV_Buffer;
തരനിർവചനം win_T *luaV_Window;
തരനിർവചനം dict_T *luaV_Dict;
തരനിർവചനം list_T *luaV_List;
തരനിർവചനം blob_T *luaV_Blob;
തരനിർവചനം ഘടന {
    char_u	*name;	// funcref
    dict_T	*self;	// selfdict
} luaV_Funcref;
തരനിർവചനം പൂർണ്ണസംഖ്യ (*msgfunc_T)(അക്ഷരം *);

തരനിർവചനം ഘടന {
    പൂർണ്ണസംഖ്യ lua_funcref;    // ref to a lua func
    പൂർണ്ണസംഖ്യ lua_tableref;   // ref to a lua table if metatable else LUA_NOREF. used
			// for __call
    lua_State *L;
} luaV_CFuncState;

സ്ഥിതം സ്ഥിരം അക്ഷരം LUAVIM_DICT[] = "dict";
സ്ഥിതം സ്ഥിരം അക്ഷരം LUAVIM_LIST[] = "list";
സ്ഥിതം സ്ഥിരം അക്ഷരം LUAVIM_BLOB[] = "blob";
സ്ഥിതം സ്ഥിരം അക്ഷരം LUAVIM_FUNCREF[] = "funcref";
സ്ഥിതം സ്ഥിരം അക്ഷരം LUAVIM_BUFFER[] = "buffer";
സ്ഥിതം സ്ഥിരം അക്ഷരം LUAVIM_WINDOW[] = "window";
സ്ഥിതം സ്ഥിരം അക്ഷരം LUAVIM_FREE[] = "luaV_free";
സ്ഥിതം സ്ഥിരം അക്ഷരം LUAVIM_LUAEVAL[] = "luaV_luaeval";
സ്ഥിതം സ്ഥിരം അക്ഷരം LUAVIM_SETREF[] = "luaV_setref";

സ്ഥിതം സ്ഥിരം അക്ഷരം LUA___CALL[] = "__call";

// get/setudata manage references to vim userdata in a cache table through
// object pointers (light userdata). The cache table itself is retrieved
// from the registry.

സ്ഥിതം സ്ഥിരം അക്ഷരം LUAVIM_UDATA_CACHE[] = "luaV_udata_cache";

# define luaV_getfield(L, s) \
    lua_pushlightuserdata((L), (ശൂന്യം *)(s)); \
    lua_rawget((L), LUA_REGISTRYINDEX)
# define luaV_checksandbox(L) \
    എങ്കിൽ (sandbox) luaL_error((L), "not allowed in sandbox")
# define lua_V_സന്ദേശം(L) luaV_msgfunc((L), (msgfunc_T) സന്ദേശം)
# define luaV_emsg(L) luaV_msgfunc((L), (msgfunc_T) emsg)
# define luaV_checktypval(L, a, v, സന്ദേശം) \
    ചെയ്യുക { \
	എങ്കിൽ (luaV_totypval(L, a, v) == FAIL) \
	    luaL_error(L, സന്ദേശം ": cannot convert value"); \
    } ആകുമ്പോൾ (0)

സ്ഥിതം luaV_List *luaV_pushlist(lua_State *L, list_T *lis);
സ്ഥിതം luaV_Dict *luaV_pushdict(lua_State *L, dict_T *dic);
സ്ഥിതം luaV_Blob *luaV_pushblob(lua_State *L, blob_T *blo);
സ്ഥിതം luaV_Funcref *luaV_pushfuncref(lua_State *L, char_u *name);
സ്ഥിതം പൂർണ്ണസംഖ്യ luaV_call_lua_func(പൂർണ്ണസംഖ്യ argcount, typval_T *argvars, typval_T *rettv, ശൂന്യം *state);
സ്ഥിതം ശൂന്യം luaV_call_lua_func_free(ശൂന്യം *state);

# എങ്കിൽ LUA_VERSION_അക്കം <= 501
#  define luaV_register(L, l) luaL_register(L, ശൂന്യ, l)
#  define luaL_typeerror luaL_typerror
# അല്ലെങ്കിൽ
#  define luaV_register(L, l) luaL_setfuncs(L, l, 0)
# endif

# ifdef DYNAMIC_LUA

#  ifdef MSWIN
#   define load_dll vimLoadLib
#   define ചിഹ്നം_from_dll GetProcAddress
#   define close_dll FreeLibrary
#   define load_dll_error GetWin32Error
#  അല്ലെങ്കിൽ
#   include <dlfcn.h>
#   define HANDLE ശൂന്യം*
#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#   define ചിഹ്നം_from_dll dlsym
#   define close_dll dlclose
#   define load_dll_error dlerror
#  endif

// lauxlib
#  എങ്കിൽ LUA_VERSION_അക്കം <= 501
#   define luaL_register dll_luaL_register
#   define luaL_prepbuffer dll_luaL_prepbuffer
#   define luaL_openlib dll_luaL_openlib
#   define luaL_typerror dll_luaL_typerror
#   define luaL_loadfile dll_luaL_loadfile
#   define luaL_loadbuffer dll_luaL_loadbuffer
#  അല്ലെങ്കിൽ
#   define luaL_prepbuffsize dll_luaL_prepbuffsize
#   define luaL_setfuncs dll_luaL_setfuncs
#   define luaL_loadfilex dll_luaL_loadfilex
#   define luaL_loadbufferx dll_luaL_loadbufferx
#   define luaL_argerror dll_luaL_argerror
#  endif
#  എങ്കിൽ LUA_VERSION_അക്കം >= 504
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
#  എങ്കിൽ LUA_VERSION_അക്കം <= 501
#   define lua_tonumber dll_lua_tonumber
#   define lua_tointeger dll_lua_tointeger
#   define lua_call dll_lua_call
#   define lua_pcall dll_lua_pcall
#  അല്ലെങ്കിൽ
#   define lua_tonumberx dll_lua_tonumberx
#   define lua_tointegerx dll_lua_tointegerx
#   define lua_callk dll_lua_callk
#   define lua_pcallk dll_lua_pcallk
#   define lua_getglobal dll_lua_getglobal
#   define lua_setglobal dll_lua_setglobal
#  endif
#  എങ്കിൽ LUA_VERSION_അക്കം <= 502
#   define lua_replace dll_lua_replace
#   define lua_remove dll_lua_remove
#  endif
#  എങ്കിൽ LUA_VERSION_അക്കം >= 503
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
#  എങ്കിൽ LUA_VERSION_അക്കം >= 504
#   define lua_newuserdatauv dll_lua_newuserdatauv
#  അല്ലെങ്കിൽ
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
#  എങ്കിൽ LUA_VERSION_അക്കം >= 505
#   define luaL_openselectedlibs dll_luaL_openselectedlibs
#  അല്ലെങ്കിൽ
#   define luaL_openlibs dll_luaL_openlibs
#  endif

// lauxlib
#  എങ്കിൽ LUA_VERSION_അക്കം <= 501
ശൂന്യം (*dll_luaL_register) (lua_State *L, സ്ഥിരം അക്ഷരം *libname, സ്ഥിരം luaL_Reg *l);
അക്ഷരം *(*dll_luaL_prepbuffer) (luaL_Buffer *B);
ശൂന്യം (*dll_luaL_openlib) (lua_State *L, സ്ഥിരം അക്ഷരം *libname, സ്ഥിരം luaL_Reg *l, പൂർണ്ണസംഖ്യ nup);
പൂർണ്ണസംഖ്യ (*dll_luaL_typerror) (lua_State *L, പൂർണ്ണസംഖ്യ narg, സ്ഥിരം അക്ഷരം *tname);
പൂർണ്ണസംഖ്യ (*dll_luaL_loadfile) (lua_State *L, സ്ഥിരം അക്ഷരം *filename);
പൂർണ്ണസംഖ്യ (*dll_luaL_loadbuffer) (lua_State *L, സ്ഥിരം അക്ഷരം *buff, size_t sz, സ്ഥിരം അക്ഷരം *name);
#  അല്ലെങ്കിൽ
അക്ഷരം *(*dll_luaL_prepbuffsize) (luaL_Buffer *B, size_t sz);
ശൂന്യം (*dll_luaL_setfuncs) (lua_State *L, സ്ഥിരം luaL_Reg *l, പൂർണ്ണസംഖ്യ nup);
പൂർണ്ണസംഖ്യ (*dll_luaL_loadfilex) (lua_State *L, സ്ഥിരം അക്ഷരം *filename, സ്ഥിരം അക്ഷരം *mode);
പൂർണ്ണസംഖ്യ (*dll_luaL_loadbufferx) (lua_State *L, സ്ഥിരം അക്ഷരം *buff, size_t sz, സ്ഥിരം അക്ഷരം *name, സ്ഥിരം അക്ഷരം *mode);
പൂർണ്ണസംഖ്യ (*dll_luaL_argerror) (lua_State *L, പൂർണ്ണസംഖ്യ numarg, സ്ഥിരം അക്ഷരം *extramsg);
#  endif
#  എങ്കിൽ LUA_VERSION_അക്കം >= 504
പൂർണ്ണസംഖ്യ (*dll_luaL_typeerror) (lua_State *L, പൂർണ്ണസംഖ്യ narg, സ്ഥിരം അക്ഷരം *tname);
#  endif
ശൂന്യം (*dll_luaL_checkany) (lua_State *L, പൂർണ്ണസംഖ്യ narg);
സ്ഥിരം അക്ഷരം *(*dll_luaL_checklstring) (lua_State *L, പൂർണ്ണസംഖ്യ അക്കം_Arg, size_t *l);
lua_Integer (*dll_luaL_checkinteger) (lua_State *L, പൂർണ്ണസംഖ്യ അക്കം_Arg);
lua_Integer (*dll_luaL_optinteger) (lua_State *L, പൂർണ്ണസംഖ്യ nArg, lua_Integer def);
ശൂന്യം (*dll_luaL_checktype) (lua_State *L, പൂർണ്ണസംഖ്യ narg, പൂർണ്ണസംഖ്യ t);
പൂർണ്ണസംഖ്യ (*dll_luaL_error) (lua_State *L, സ്ഥിരം അക്ഷരം *fmt, ...);
lua_State *(*dll_luaL_newstate) (ശൂന്യം);
ശൂന്യം (*dll_luaL_buffinit) (lua_State *L, luaL_Buffer *B);
ശൂന്യം (*dll_luaL_addlstring) (luaL_Buffer *B, സ്ഥിരം അക്ഷരം *s, size_t l);
ശൂന്യം (*dll_luaL_pushresult) (luaL_Buffer *B);
പൂർണ്ണസംഖ്യ (*dll_luaL_loadstring) (lua_State *L, സ്ഥിരം അക്ഷരം *s);
പൂർണ്ണസംഖ്യ (*dll_luaL_ref) (lua_State *L, പൂർണ്ണസംഖ്യ idx);
#  എങ്കിൽ LUA_VERSION_അക്കം <= 502
ശൂന്യം (*dll_luaL_unref) (lua_State *L, പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ n);
#  അല്ലെങ്കിൽ
ശൂന്യം (*dll_luaL_unref) (lua_State *L, പൂർണ്ണസംഖ്യ idx, lua_Integer n);
#  endif
// lua
#  എങ്കിൽ LUA_VERSION_അക്കം <= 501
lua_സംഖ്യ (*dll_lua_tonumber) (lua_State *L, പൂർണ്ണസംഖ്യ idx);
lua_Integer (*dll_lua_tointeger) (lua_State *L, പൂർണ്ണസംഖ്യ idx);
ശൂന്യം (*dll_lua_call) (lua_State *L, പൂർണ്ണസംഖ്യ nargs, പൂർണ്ണസംഖ്യ nresults);
പൂർണ്ണസംഖ്യ (*dll_lua_pcall) (lua_State *L, പൂർണ്ണസംഖ്യ nargs, പൂർണ്ണസംഖ്യ nresults, പൂർണ്ണസംഖ്യ errfunc);
#  അല്ലെങ്കിൽ
lua_സംഖ്യ (*dll_lua_tonumberx) (lua_State *L, പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ *isnum);
lua_Integer (*dll_lua_tointegerx) (lua_State *L, പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ *isnum);
ശൂന്യം (*dll_lua_callk) (lua_State *L, പൂർണ്ണസംഖ്യ nargs, പൂർണ്ണസംഖ്യ nresults, പൂർണ്ണസംഖ്യ ctx,
	lua_CFunction k);
പൂർണ്ണസംഖ്യ (*dll_lua_pcallk) (lua_State *L, പൂർണ്ണസംഖ്യ nargs, പൂർണ്ണസംഖ്യ nresults, പൂർണ്ണസംഖ്യ errfunc,
	പൂർണ്ണസംഖ്യ ctx, lua_CFunction k);
ശൂന്യം (*dll_lua_getglobal) (lua_State *L, സ്ഥിരം അക്ഷരം *var);
ശൂന്യം (*dll_lua_setglobal) (lua_State *L, സ്ഥിരം അക്ഷരം *var);
#  endif
#  എങ്കിൽ LUA_VERSION_അക്കം <= 502
ശൂന്യം (*dll_lua_replace) (lua_State *L, പൂർണ്ണസംഖ്യ idx);
ശൂന്യം (*dll_lua_remove) (lua_State *L, പൂർണ്ണസംഖ്യ idx);
#  endif
#  എങ്കിൽ LUA_VERSION_അക്കം >= 503
ശൂന്യം  (*dll_lua_rotate) (lua_State *L, പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ n);
ശൂന്യം (*dll_lua_copy) (lua_State *L, പൂർണ്ണസംഖ്യ fromidx, പൂർണ്ണസംഖ്യ toidx);
#  endif
സ്ഥിരം അക്ഷരം *(*dll_lua_typename) (lua_State *L, പൂർണ്ണസംഖ്യ tp);
ശൂന്യം       (*dll_lua_close) (lua_State *L);
പൂർണ്ണസംഖ്യ (*dll_lua_gettop) (lua_State *L);
ശൂന്യം (*dll_lua_settop) (lua_State *L, പൂർണ്ണസംഖ്യ idx);
ശൂന്യം (*dll_lua_pushvalue) (lua_State *L, പൂർണ്ണസംഖ്യ idx);
പൂർണ്ണസംഖ്യ (*dll_lua_isnumber) (lua_State *L, പൂർണ്ണസംഖ്യ idx);
പൂർണ്ണസംഖ്യ (*dll_lua_isstring) (lua_State *L, പൂർണ്ണസംഖ്യ idx);
പൂർണ്ണസംഖ്യ (*dll_lua_type) (lua_State *L, പൂർണ്ണസംഖ്യ idx);
പൂർണ്ണസംഖ്യ (*dll_lua_rawequal) (lua_State *L, പൂർണ്ണസംഖ്യ idx1, പൂർണ്ണസംഖ്യ idx2);
പൂർണ്ണസംഖ്യ (*dll_lua_toboolean) (lua_State *L, പൂർണ്ണസംഖ്യ idx);
സ്ഥിരം അക്ഷരം *(*dll_lua_tolstring) (lua_State *L, പൂർണ്ണസംഖ്യ idx, size_t *len);
ശൂന്യം *(*dll_lua_touserdata) (lua_State *L, പൂർണ്ണസംഖ്യ idx);
ശൂന്യം (*dll_lua_pushnil) (lua_State *L);
ശൂന്യം (*dll_lua_pushnumber) (lua_State *L, lua_സംഖ്യ n);
ശൂന്യം (*dll_lua_pushinteger) (lua_State *L, lua_Integer n);
ശൂന്യം (*dll_lua_pushlstring) (lua_State *L, സ്ഥിരം അക്ഷരം *s, size_t l);
ശൂന്യം (*dll_lua_pushstring) (lua_State *L, സ്ഥിരം അക്ഷരം *s);
സ്ഥിരം അക്ഷരം *(*dll_lua_pushfstring) (lua_State *L, സ്ഥിരം അക്ഷരം *fmt, ...);
ശൂന്യം (*dll_lua_pushcclosure) (lua_State *L, lua_CFunction fn, പൂർണ്ണസംഖ്യ n);
ശൂന്യം (*dll_lua_pushboolean) (lua_State *L, പൂർണ്ണസംഖ്യ b);
ശൂന്യം (*dll_lua_pushlightuserdata) (lua_State *L, ശൂന്യം *p);
ശൂന്യം (*dll_lua_getfield) (lua_State *L, പൂർണ്ണസംഖ്യ idx, സ്ഥിരം അക്ഷരം *k);
#  എങ്കിൽ LUA_VERSION_അക്കം <= 502
ശൂന്യം (*dll_lua_rawget) (lua_State *L, പൂർണ്ണസംഖ്യ idx);
ശൂന്യം (*dll_lua_rawgeti) (lua_State *L, പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ n);
#  അല്ലെങ്കിൽ
പൂർണ്ണസംഖ്യ (*dll_lua_rawget) (lua_State *L, പൂർണ്ണസംഖ്യ idx);
പൂർണ്ണസംഖ്യ (*dll_lua_rawgeti) (lua_State *L, പൂർണ്ണസംഖ്യ idx, lua_Integer n);
#  endif
ശൂന്യം (*dll_lua_createtable) (lua_State *L, പൂർണ്ണസംഖ്യ narr, പൂർണ്ണസംഖ്യ nrec);
ശൂന്യം (*dll_lua_settable) (lua_State *L, പൂർണ്ണസംഖ്യ idx);
#  എങ്കിൽ LUA_VERSION_അക്കം >= 504
ശൂന്യം *(*dll_lua_newuserdatauv) (lua_State *L, size_t sz, പൂർണ്ണസംഖ്യ nuvalue);
#  അല്ലെങ്കിൽ
ശൂന്യം *(*dll_lua_newuserdata) (lua_State *L, size_t sz);
#  endif
പൂർണ്ണസംഖ്യ (*dll_lua_getmetatable) (lua_State *L, പൂർണ്ണസംഖ്യ objindex);
ശൂന്യം (*dll_lua_setfield) (lua_State *L, പൂർണ്ണസംഖ്യ idx, സ്ഥിരം അക്ഷരം *k);
ശൂന്യം (*dll_lua_rawset) (lua_State *L, പൂർണ്ണസംഖ്യ idx);
#  എങ്കിൽ LUA_VERSION_അക്കം <= 502
ശൂന്യം (*dll_lua_rawseti) (lua_State *L, പൂർണ്ണസംഖ്യ idx, പൂർണ്ണസംഖ്യ n);
#  അല്ലെങ്കിൽ
ശൂന്യം (*dll_lua_rawseti) (lua_State *L, പൂർണ്ണസംഖ്യ idx, lua_Integer n);
#  endif
പൂർണ്ണസംഖ്യ (*dll_lua_setmetatable) (lua_State *L, പൂർണ്ണസംഖ്യ objindex);
പൂർണ്ണസംഖ്യ (*dll_lua_next) (lua_State *L, പൂർണ്ണസംഖ്യ idx);
// libs
പൂർണ്ണസംഖ്യ (*dll_luaopen_base) (lua_State *L);
പൂർണ്ണസംഖ്യ (*dll_luaopen_table) (lua_State *L);
പൂർണ്ണസംഖ്യ (*dll_luaopen_string) (lua_State *L);
പൂർണ്ണസംഖ്യ (*dll_luaopen_math) (lua_State *L);
പൂർണ്ണസംഖ്യ (*dll_luaopen_io) (lua_State *L);
പൂർണ്ണസംഖ്യ (*dll_luaopen_os) (lua_State *L);
പൂർണ്ണസംഖ്യ (*dll_luaopen_package) (lua_State *L);
പൂർണ്ണസംഖ്യ (*dll_luaopen_debug) (lua_State *L);
#  എങ്കിൽ LUA_VERSION_അക്കം >= 505
ശൂന്യം (*dll_luaL_openselectedlibs) (lua_State *L, പൂർണ്ണസംഖ്യ load, പൂർണ്ണസംഖ്യ preload);
#  അല്ലെങ്കിൽ
ശൂന്യം (*dll_luaL_openlibs) (lua_State *L);
#  endif

തരനിർവചനം ശൂന്യം **luaV_function;
തരനിർവചനം ഘടന {
    സ്ഥിരം അക്ഷരം *name;
    luaV_function func;
} luaV_Reg;

സ്ഥിതം സ്ഥിരം luaV_Reg luaV_dll[] = {
    // lauxlib
#  എങ്കിൽ LUA_VERSION_അക്കം <= 501
    {"luaL_register", (luaV_function) &dll_luaL_register},
    {"luaL_prepbuffer", (luaV_function) &dll_luaL_prepbuffer},
    {"luaL_openlib", (luaV_function) &dll_luaL_openlib},
    {"luaL_typerror", (luaV_function) &dll_luaL_typerror},
    {"luaL_loadfile", (luaV_function) &dll_luaL_loadfile},
    {"luaL_loadbuffer", (luaV_function) &dll_luaL_loadbuffer},
#  അല്ലെങ്കിൽ
    {"luaL_prepbuffsize", (luaV_function) &dll_luaL_prepbuffsize},
    {"luaL_setfuncs", (luaV_function) &dll_luaL_setfuncs},
    {"luaL_loadfilex", (luaV_function) &dll_luaL_loadfilex},
    {"luaL_loadbufferx", (luaV_function) &dll_luaL_loadbufferx},
    {"luaL_argerror", (luaV_function) &dll_luaL_argerror},
#  endif
#  എങ്കിൽ LUA_VERSION_അക്കം >= 504
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
#  എങ്കിൽ LUA_VERSION_അക്കം <= 501
    {"lua_tonumber", (luaV_function) &dll_lua_tonumber},
    {"lua_tointeger", (luaV_function) &dll_lua_tointeger},
    {"lua_call", (luaV_function) &dll_lua_call},
    {"lua_pcall", (luaV_function) &dll_lua_pcall},
#  അല്ലെങ്കിൽ
    {"lua_tonumberx", (luaV_function) &dll_lua_tonumberx},
    {"lua_tointegerx", (luaV_function) &dll_lua_tointegerx},
    {"lua_callk", (luaV_function) &dll_lua_callk},
    {"lua_pcallk", (luaV_function) &dll_lua_pcallk},
    {"lua_getglobal", (luaV_function) &dll_lua_getglobal},
    {"lua_setglobal", (luaV_function) &dll_lua_setglobal},
#  endif
#  എങ്കിൽ LUA_VERSION_അക്കം <= 502
    {"lua_replace", (luaV_function) &dll_lua_replace},
    {"lua_remove", (luaV_function) &dll_lua_remove},
#  endif
#  എങ്കിൽ LUA_VERSION_അക്കം >= 503
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
#  എങ്കിൽ LUA_VERSION_അക്കം >= 504
    {"lua_newuserdatauv", (luaV_function) &dll_lua_newuserdatauv},
#  അല്ലെങ്കിൽ
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
#  എങ്കിൽ LUA_VERSION_അക്കം >= 505
    {"luaL_openselectedlibs", (luaV_function) &dll_luaL_openselectedlibs},
#  അല്ലെങ്കിൽ
    {"luaL_openlibs", (luaV_function) &dll_luaL_openlibs},
#  endif
    {ശൂന്യ, ശൂന്യ}
};

സ്ഥിതം HANDLE hinstLua = ശൂന്യ;

    സ്ഥിതം പൂർണ്ണസംഖ്യ
lua_link_init(അക്ഷരം *libname, പൂർണ്ണസംഖ്യ verbose)
{
    സ്ഥിരം luaV_Reg *reg;
    എങ്കിൽ (hinstLua) തിരികെ OK;
    hinstLua = load_dll(libname);
    എങ്കിൽ (!hinstLua)
    {
	എങ്കിൽ (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	തിരികെ FAIL;
    }
    വേണ്ടി (reg = luaV_dll; reg->func; reg++)
    {
	എങ്കിൽ ((*reg->func = ചിഹ്നം_from_dll(hinstLua, reg->name)) == ശൂന്യ)
	{
	    close_dll(hinstLua);
	    hinstLua = 0;
	    എങ്കിൽ (verbose)
		semsg(_(e_could_not_load_library_function_str), reg->name);
	    തിരികെ FAIL;
	}
    }
    തിരികെ OK;
}
# endif // DYNAMIC_LUA

# എങ്കിൽ defined(DYNAMIC_LUA)
    പൂർണ്ണസംഖ്യ
lua_enabled(പൂർണ്ണസംഖ്യ verbose)
{
    തിരികെ lua_link_init((അക്ഷരം *)p_luadll, verbose) == OK;
}
# endif

# എങ്കിൽ LUA_VERSION_അക്കം > 501 && LUA_VERSION_അക്കം < 504
    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaL_typeerror(lua_State *L, പൂർണ്ണസംഖ്യ narg, സ്ഥിരം അക്ഷരം *tname)
{
    സ്ഥിരം അക്ഷരം *സന്ദേശം = lua_pushfstring(L, "%s expected, got %s",
	    tname, luaL_typename(L, narg));
    തിരികെ luaL_argerror(L, narg, സന്ദേശം);
}
# endif

    സ്ഥിതം LUAV_INLINE ശൂന്യം
luaV_getudata(lua_State *L, ശൂന്യം *v)
{
    lua_pushlightuserdata(L, (ശൂന്യം *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // now the cache table is at the top of the stack
    lua_pushlightuserdata(L, v);
    lua_rawget(L, -2);
    lua_remove(L, -2);  // remove the cache table from the stack
}

    സ്ഥിതം LUAV_INLINE ശൂന്യം
luaV_setudata(lua_State *L, ശൂന്യം *v)
{
    lua_pushlightuserdata(L, (ശൂന്യം *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // cache table is at -1
    lua_pushlightuserdata(L, v);       // ...now at -2
    lua_pushvalue(L, -3);	       // copy the userdata (cache at -3)
    lua_rawset(L, -3);		       // consumes two stack items
    lua_എടുക്കുക(L, 1);		       // and remove the cache table
}

// =======   Internal   =======

    സ്ഥിതം ശൂന്യം
luaV_newmetatable(lua_State *L, സ്ഥിരം അക്ഷരം *tname)
{
    lua_newtable(L);
    lua_pushlightuserdata(L, (ശൂന്യം *) tname);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
}

    സ്ഥിതം ശൂന്യം *
luaV_toudata(lua_State *L, പൂർണ്ണസംഖ്യ ud, സ്ഥിരം അക്ഷരം *tname)
{
    ശൂന്യം *p = lua_touserdata(L, ud);

    എങ്കിൽ (p == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // value is userdata
    എങ്കിൽ (lua_getmetatable(L, ud)) // does it have a metatable?
    {
	luaV_getfield(L, tname); // get metatable
	എങ്കിൽ (lua_rawequal(L, -1, -2)) // MTs match?
	{
	    lua_എടുക്കുക(L, 2); // MTs
	    തിരികെ p;
	}
    }

    തിരികെ ശൂന്യ;
}

    സ്ഥിതം ശൂന്യം *
luaV_checkcache(lua_State *L, ശൂന്യം *p)
{
    luaV_getudata(L, p);
    എങ്കിൽ (lua_isnil(L, -1)) luaL_error(L, "invalid object");
    lua_എടുക്കുക(L, 1);
    തിരികെ p;
}

# define luaV_unbox(L,luatyp,ud) (*((luatyp *) lua_touserdata((L),(ud))))

# define luaV_checkvalid(L,luatyp,ud) \
    luaV_checkcache((L), (ശൂന്യം *) luaV_unbox((L),luatyp,(ud)))

    സ്ഥിതം ശൂന്യം *
luaV_checkudata(lua_State *L, പൂർണ്ണസംഖ്യ ud, സ്ഥിരം അക്ഷരം *tname)
{
    ശൂന്യം *p = luaV_toudata(L, ud, tname);
    എങ്കിൽ (p == ശൂന്യ) luaL_typeerror(L, ud, tname);
    തിരികെ p;
}

    സ്ഥിതം ശൂന്യം
luaV_pushtypval(lua_State *L, typval_T *tv)
{
    എങ്കിൽ (tv == ശൂന്യ)
    {
	lua_pushnil(L);
	തിരികെ;
    }
    തിരഞ്ഞെടുപ്പ് (tv->v_type)
    {
	അവസ്ഥ VAR_STRING:
	    lua_pushstring(L, tv->vval.v_string == ശൂന്യ
					    ? "" : (അക്ഷരം *)tv->vval.v_string);
	    നിർത്തുക;
	അവസ്ഥ VAR_സംഖ്യ:
	    lua_pushinteger(L, (പൂർണ്ണസംഖ്യ) tv->vval.v_സംഖ്യ);
	    നിർത്തുക;
	അവസ്ഥ VAR_FLOAT:
	    lua_pushnumber(L, (lua_സംഖ്യ) tv->vval.v_float);
	    നിർത്തുക;
	അവസ്ഥ VAR_LIST:
	    luaV_pushlist(L, tv->vval.v_list);
	    നിർത്തുക;
	അവസ്ഥ VAR_DICT:
	    luaV_pushdict(L, tv->vval.v_dict);
	    നിർത്തുക;
	അവസ്ഥ VAR_BOOL:
	അവസ്ഥ VAR_SPECIAL:
	    എങ്കിൽ (tv->vval.v_സംഖ്യ <= VVAL_TRUE)
		lua_pushboolean(L, (പൂർണ്ണസംഖ്യ) tv->vval.v_സംഖ്യ);
	    അല്ലെങ്കിൽ
		lua_pushnil(L);
	    നിർത്തുക;
	അവസ്ഥ VAR_FUNC:
	    luaV_pushfuncref(L, tv->vval.v_string);
	    നിർത്തുക;
	അവസ്ഥ VAR_PARTIAL:
	    // TODO: handle partial arguments
	    luaV_pushfuncref(L, partial_name(tv->vval.v_partial));
	    നിർത്തുക;

	അവസ്ഥ VAR_BLOB:
	    luaV_pushblob(L, tv->vval.v_blob);
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    lua_pushnil(L);
    }
}

/*
 * Converts lua value at 'pos' to typval 'tv'.
 * Returns OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_totypval(lua_State *L, പൂർണ്ണസംഖ്യ pos, typval_T *tv)
{
    പൂർണ്ണസംഖ്യ status = OK;

    tv->v_lock = 0;

    തിരഞ്ഞെടുപ്പ് (lua_type(L, pos))
    {
	അവസ്ഥ LUA_TBOOLEAN:
	    tv->v_type = VAR_BOOL;
	    tv->vval.v_സംഖ്യ = (varnumber_T) lua_toboolean(L, pos);
	    നിർത്തുക;
	അവസ്ഥ LUA_TNIL:
	    tv->v_type = VAR_SPECIAL;
	    tv->vval.v_സംഖ്യ = VVAL_NULL;
	    നിർത്തുക;
	അവസ്ഥ LUA_TSTRING:
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strsave((char_u *) lua_tostring(L, pos));
	    നിർത്തുക;
	അവസ്ഥ LUA_TNUMBER:
	{
	    സ്ഥിരം lua_സംഖ്യ n = lua_tonumber(L, pos);

	    എങ്കിൽ (n > (lua_സംഖ്യ)INT64_MAX || n < (lua_സംഖ്യ)INT64_MIN
		    || ((lua_സംഖ്യ)((varnumber_T)n)) != n)
	    {
		tv->v_type = VAR_FLOAT;
		tv->vval.v_float = (float_T)n;
	    }
	    അല്ലെങ്കിൽ
	    {
		tv->v_type = VAR_സംഖ്യ;
		tv->vval.v_സംഖ്യ = (varnumber_T)n;
	    }
	}
	    നിർത്തുക;
	അവസ്ഥ LUA_TFUNCTION:
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
	    നിർത്തുക;
	}
	അവസ്ഥ LUA_TTABLE:
	{
	    പൂർണ്ണസംഖ്യ lua_tableref;

	    lua_pushvalue(L, pos);
	    lua_tableref = luaL_ref(L, LUA_REGISTRYINDEX);
	    എങ്കിൽ (lua_getmetatable(L, pos))
	    {
		lua_getfield(L, -1, LUA___CALL);
		എങ്കിൽ (lua_isfunction(L, -1))
		{
		    char_u *name;
		    പൂർണ്ണസംഖ്യ lua_funcref = luaL_ref(L, LUA_REGISTRYINDEX);
		    luaV_CFuncState *state = ALLOC_CLEAR_ONE(luaV_CFuncState);

		    state->lua_funcref = lua_funcref;
		    state->L = L;
		    state->lua_tableref = lua_tableref;
		    name = register_cfunc(&luaV_call_lua_func,
					      &luaV_call_lua_func_free, state);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(name);
		    നിർത്തുക;
		}
	    }
	    tv->v_type = VAR_സംഖ്യ;
	    tv->vval.v_സംഖ്യ = 0;
	    status = FAIL;
	    നിർത്തുക;
	}
	അവസ്ഥ LUA_TUSERDATA:
	{
	    ശൂന്യം *p = lua_touserdata(L, pos);

	    എങ്കിൽ (lua_getmetatable(L, pos)) // has metatable?
	    {
		// check list
		luaV_getfield(L, LUAVIM_LIST);
		എങ്കിൽ (lua_rawequal(L, -1, -2))
		{
		    tv->v_type = VAR_LIST;
		    tv->vval.v_list = *((luaV_List *) p);
		    ++tv->vval.v_list->lv_refcount;
		    lua_എടുക്കുക(L, 2); // MTs
		    നിർത്തുക;
		}
		// check dict
		luaV_getfield(L, LUAVIM_DICT);
		എങ്കിൽ (lua_rawequal(L, -1, -3))
		{
		    tv->v_type = VAR_DICT;
		    tv->vval.v_dict = *((luaV_Dict *) p);
		    ++tv->vval.v_dict->dv_refcount;
		    lua_എടുക്കുക(L, 3); // MTs
		    നിർത്തുക;
		}
		// check blob
		luaV_getfield(L, LUAVIM_BLOB);
		എങ്കിൽ (lua_rawequal(L, -1, -4))
		{
		    tv->v_type = VAR_BLOB;
		    tv->vval.v_blob = *((luaV_Blob *) p);
		    ++tv->vval.v_blob->bv_refcount;
		    lua_എടുക്കുക(L, 4); // MTs
		    നിർത്തുക;
		}
		// check funcref
		luaV_getfield(L, LUAVIM_FUNCREF);
		എങ്കിൽ (lua_rawequal(L, -1, -5))
		{
		    luaV_Funcref *f = (luaV_Funcref *) p;

		    func_ref(f->name);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(f->name);
		    lua_എടുക്കുക(L, 5); // MTs
		    നിർത്തുക;
		}
		lua_എടുക്കുക(L, 4); // MTs
	    }
	}
	// FALLTHROUGH
	സ്ഥിരസ്ഥിതി:
	    tv->v_type = VAR_സംഖ്യ;
	    tv->vval.v_സംഖ്യ = 0;
	    status = FAIL;
    }
    തിരികെ status;
}

/*
 * similar to luaL_addlstring, but replaces \0 with \n if toline and
 * \n with \0 otherwise
 */
    സ്ഥിതം ശൂന്യം
luaV_addlstring(luaL_Buffer *b, സ്ഥിരം അക്ഷരം *s, size_t l, പൂർണ്ണസംഖ്യ toline)
{
    ആകുമ്പോൾ (l--)
    {
	എങ്കിൽ (*s == '\0' && toline)
	    luaL_addchar(b, '\n');
	അല്ലെങ്കിൽ എങ്കിൽ (*s == '\n' && !toline)
	    luaL_addchar(b, '\0');
	അല്ലെങ്കിൽ
	    luaL_addchar(b, *s);
	s++;
    }
}

    സ്ഥിതം ശൂന്യം
luaV_pushline(lua_State *L, buf_T *buf, linenr_T n)
{
    സ്ഥിരം അക്ഷരം *s = (സ്ഥിരം അക്ഷരം *) ml_get_buf(buf, n, FALSE);
    luaL_Buffer b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, strlen(s), 0);
    luaL_pushresult(&b);
}

    സ്ഥിതം char_u *
luaV_toline(lua_State *L, പൂർണ്ണസംഖ്യ pos)
{
    size_t l;
    സ്ഥിരം അക്ഷരം *s = lua_tolstring(L, pos, &l);

    luaL_Buffer b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 1);
    luaL_pushresult(&b);
    തിരികെ (char_u *) lua_tostring(L, -1);
}

/*
 * pops a string s from the top of the stack and calls mf(t) for pieces t of
 * s separated by newlines
 */
    സ്ഥിതം ശൂന്യം
luaV_msgfunc(lua_State *L, msgfunc_T mf)
{
    luaL_Buffer b;
    size_t l;
    സ്ഥിരം അക്ഷരം *p, *s = lua_tolstring(L, -1, &l);
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 0);
    luaL_pushresult(&b);
    // break string
    p = s = lua_tolstring(L, -1, &l);
    ആകുമ്പോൾ (l--)
    {
	എങ്കിൽ (*p++ == '\0') // break?
	{
	    mf((അക്ഷരം *)s);
	    s = p;
	}
    }
    mf((അക്ഷരം *)s);
    lua_എടുക്കുക(L, 2); // original and modified strings
}

# define luaV_newtype(typ,tname,luatyp,luatname) \
	സ്ഥിതം luatyp * \
    luaV_new##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = (luatyp *) lua_newuserdata(L, വലുപ്പം(luatyp)); \
	*o = obj; \
	luaV_setudata(L, obj); /* cache[obj] = udata */ \
	luaV_getfield(L, luatname); \
	lua_setmetatable(L, -2); \
	തിരികെ o; \
    }

# define luaV_pushtype(typ,tname,luatyp) \
	സ്ഥിതം luatyp * \
    lua_V_തള്ളുക##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = ശൂന്യ; \
	എങ്കിൽ (obj == ശൂന്യ) \
	    lua_pushnil(L); \
	അല്ലെങ്കിൽ \
	{ \
	    luaV_getudata(L, obj); \
	    എങ്കിൽ (lua_isnil(L, -1)) /* not interned? */ \
	    { \
		lua_എടുക്കുക(L, 1); \
		o = luaV_new##tname(L, obj); \
	    } \
	    അല്ലെങ്കിൽ \
		o = (luatyp *) lua_touserdata(L, -1); \
	} \
	തിരികെ o; \
    }

# define luaV_type_tostring(tname,luatname) \
	സ്ഥിതം പൂർണ്ണസംഖ്യ \
    luaV_##tname##_tostring(lua_State *L) \
    { \
	lua_pushfstring(L, "%s: %p", luatname, lua_touserdata(L, 1)); \
	തിരികെ 1; \
    }

// =======   List type   =======

    സ്ഥിതം luaV_List *
luaV_newlist(lua_State *L, list_T *lis)
{
    luaV_List *l = (luaV_List *) lua_newuserdata(L, വലുപ്പം(luaV_List));
    *l = lis;
    lis->lv_refcount++; // reference in Lua
    luaV_setudata(L, lis); // cache[lis] = udata
    luaV_getfield(L, LUAVIM_LIST);
    lua_setmetatable(L, -2);
    തിരികെ l;
}

luaV_pushtype(list_T, list, luaV_List)
luaV_type_tostring(list, LUAVIM_LIST)

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_list_len(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushinteger(L, (പൂർണ്ണസംഖ്യ) list_len(l));
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_list_iter(lua_State *L)
{
    listitem_T *li = (listitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    എങ്കിൽ (li == ശൂന്യ) തിരികെ 0;
    luaV_pushtypval(L, &li->li_tv);
    lua_pushlightuserdata(L, (ശൂന്യം *) li->li_next);
    lua_replace(L, lua_upvalueindex(1));
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_list_call(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushlightuserdata(L, (ശൂന്യം *) l->lv_first);
    lua_pushcclosure(L, luaV_list_iter, 1);
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_list_index(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    എങ്കിൽ (lua_isnumber(L, 2)) // list item?
    {
	long n = (long) luaL_checkinteger(L, 2);
	listitem_T *li;

	// Lua array index starts with 1 while Vim uses 0, subtract 1 to
	// normalize.
	n -= 1;
	li = list_find(l, n);
	എങ്കിൽ (li == ശൂന്യ)
	    lua_pushnil(L);
	അല്ലെങ്കിൽ
	    luaV_pushtypval(L, &li->li_tv);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (lua_isstring(L, 2)) // method?
    {
	സ്ഥിരം അക്ഷരം *s = lua_tostring(L, 2);
	എങ്കിൽ (strncmp(s, "add", 3) == 0
		|| strncmp(s, "insert", 6) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	അല്ലെങ്കിൽ
	    lua_pushnil(L);
    }
    അല്ലെങ്കിൽ
	lua_pushnil(L);
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_list_newindex(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    long n = (long) luaL_checkinteger(L, 2);
    listitem_T *li;

    // Lua array index starts with 1 while Vim uses 0, subtract 1 to normalize.
    n -= 1;

    എങ്കിൽ (l->lv_lock)
	luaL_error(L, "list is locked");
    li = list_find(l, n);
    എങ്കിൽ (li == ശൂന്യ)
    {
	എങ്കിൽ (!lua_isnil(L, 3))
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "inserting list item");
	    എങ്കിൽ (list_തിരുകുക_tv(l, &v, li) == FAIL)
		luaL_error(L, "failed to add item to list");
	    clear_tv(&v);
	}
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (lua_isnil(L, 3)) // remove?
	{
	    vimlist_remove(l, li, li);
	    listitem_free(l, li);
	}
	അല്ലെങ്കിൽ
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "setting list item");
	    clear_tv(&li->li_tv);
	    li->li_tv = v;
	}
    }
    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_list_add(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (ശൂന്യം *) *lis);
    typval_T v;
    എങ്കിൽ (l->lv_lock)
	luaL_error(L, "list is locked");
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "adding list item");
    എങ്കിൽ (list_append_tv(l, &v) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
lua_V_list_തിരുകുക(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (ശൂന്യം *) *lis);
    long pos = (long) luaL_optinteger(L, 3, 0);
    listitem_T *li = ശൂന്യ;
    typval_T v;
    എങ്കിൽ (l->lv_lock)
	luaL_error(L, "list is locked");
    എങ്കിൽ (pos < l->lv_len)
    {
	li = list_find(l, pos);
	എങ്കിൽ (li == ശൂന്യ)
	    luaL_error(L, "invalid position");
    }
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "inserting list item");
    എങ്കിൽ (list_തിരുകുക_tv(l, &v, li) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    തിരികെ 1;
}

സ്ഥിതം സ്ഥിരം luaL_Reg luaV_List_mt[] = {
    {"__tostring", luaV_list_tostring},
    {"__len", luaV_list_len},
    {"__call", luaV_list_call},
    {"__index", luaV_list_index},
    {"__newindex", luaV_list_newindex},
    {"add", luaV_list_add},
    {"insert", lua_V_list_തിരുകുക},
    {ശൂന്യ, ശൂന്യ}
};


// =======   Dict type   =======

    സ്ഥിതം luaV_Dict *
luaV_newdict(lua_State *L, dict_T *dic)
{
    luaV_Dict *d = (luaV_Dict *) lua_newuserdata(L, വലുപ്പം(luaV_Dict));
    *d = dic;
    dic->dv_refcount++; // reference in Lua
    luaV_setudata(L, dic); // cache[dic] = udata
    luaV_getfield(L, LUAVIM_DICT);
    lua_setmetatable(L, -2);
    തിരികെ d;
}

luaV_pushtype(dict_T, dict, luaV_Dict)
luaV_type_tostring(dict, LUAVIM_DICT)

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_dict_len(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    lua_pushinteger(L, (പൂർണ്ണസംഖ്യ) dict_len(d));
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_dict_iter(lua_State *L UNUSED)
{
# ifdef FEAT_EVAL
    hashitem_T *hi = (hashitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    പൂർണ്ണസംഖ്യ n = lua_tointeger(L, lua_upvalueindex(2));
    dictitem_T *di;
    എങ്കിൽ (n <= 0) തിരികെ 0;
    ആകുമ്പോൾ (HASHITEM_EMPTY(hi)) hi++;
    di = dict_lookup(hi);
    lua_pushstring(L, (അക്ഷരം *) hi->hi_താക്കോൽ);
    luaV_pushtypval(L, &di->di_tv);
    lua_pushlightuserdata(L, (ശൂന്യം *) (hi + 1));
    lua_replace(L, lua_upvalueindex(1));
    lua_pushinteger(L, n - 1);
    lua_replace(L, lua_upvalueindex(2));
    തിരികെ 2;
# അല്ലെങ്കിൽ
    തിരികെ 0;
# endif
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_dict_call(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    hashtab_T *ht = &d->dv_hashtab;
    lua_pushlightuserdata(L, (ശൂന്യം *) ht->ht_array);
    lua_pushinteger(L, ht->ht_used); // # remaining items
    lua_pushcclosure(L, luaV_dict_iter, 2);
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_dict_index(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *താക്കോൽ = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di = dict_find(d, താക്കോൽ, -1);

    എങ്കിൽ (di == ശൂന്യ)
    {
	lua_pushnil(L);
	തിരികെ 1;
    }

    luaV_pushtypval(L, &di->di_tv);
    എങ്കിൽ (di->di_tv.v_type == VAR_FUNC) // funcref?
    {
	luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, -1);
	f->self = d; // keep "self" reference
	d->dv_refcount++;
    }

    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_dict_newindex(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *താക്കോൽ = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di;
    typval_T tv;

    എങ്കിൽ (d->dv_lock)
	luaL_error(L, "dict is locked");
    എങ്കിൽ (താക്കോൽ == ശൂന്യ)
	തിരികെ 0;
    എങ്കിൽ (*താക്കോൽ == NUL)
	luaL_error(L, "empty key");
    എങ്കിൽ (!lua_isnil(L, 3)) // read value?
    {
	luaV_checktypval(L, 3, &tv, "setting dict item");
	എങ്കിൽ (d->dv_scope == VAR_DEF_SCOPE && tv.v_type == VAR_FUNC)
	{
	    clear_tv(&tv);
	    luaL_error(L, "cannot assign funcref to builtin scope");
	}
    }
    di = dict_find(d, താക്കോൽ, -1);
    എങ്കിൽ (di == ശൂന്യ) // non-existing key?
    {
	എങ്കിൽ (lua_isnil(L, 3))
	    തിരികെ 0;
	di = dictitem_alloc(താക്കോൽ);
	എങ്കിൽ (di == ശൂന്യ)
	{
	    clear_tv(&tv);
	    തിരികെ 0;
	}
	എങ്കിൽ (dict_add(d, di) == FAIL)
	{
	    vim_free(di);
	    clear_tv(&tv);
	    തിരികെ 0;
	}
    }
    അല്ലെങ്കിൽ
	clear_tv(&di->di_tv);
    എങ്കിൽ (lua_isnil(L, 3)) // remove?
    {
	hashitem_T *hi = hash_find(&d->dv_hashtab, di->di_താക്കോൽ);
	hash_remove(&d->dv_hashtab, hi, "Lua new index");
	dictitem_free(di);
    }
    അല്ലെങ്കിൽ
	di->di_tv = tv;
    തിരികെ 0;
}

സ്ഥിതം സ്ഥിരം luaL_Reg luaV_Dict_mt[] = {
    {"__tostring", luaV_dict_tostring},
    {"__len", luaV_dict_len},
    {"__call", luaV_dict_call},
    {"__index", luaV_dict_index},
    {"__newindex", luaV_dict_newindex},
    {ശൂന്യ, ശൂന്യ}
};


// =======   Blob type   =======

    സ്ഥിതം luaV_Blob *
luaV_newblob(lua_State *L, blob_T *blo)
{
    luaV_Blob *b = (luaV_Blob *) lua_newuserdata(L, വലുപ്പം(luaV_Blob));
    *b = blo;
    blo->bv_refcount++; // reference in Lua
    luaV_setudata(L, blo); // cache[blo] = udata
    luaV_getfield(L, LUAVIM_BLOB);
    lua_setmetatable(L, -2);
    തിരികെ b;
}

luaV_pushtype(blob_T, blob, luaV_Blob)
luaV_type_tostring(blob, LUAVIM_BLOB)

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_blob_gc(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    blob_unref(b);
    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_blob_len(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    lua_pushinteger(L, (പൂർണ്ണസംഖ്യ) blob_len(b));
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_blob_index(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    എങ്കിൽ (lua_isnumber(L, 2))
    {
	പൂർണ്ണസംഖ്യ idx = luaL_checkinteger(L, 2);
	എങ്കിൽ (idx < 0)
	    // Negative index is relative to the end.
	    idx += (പൂർണ്ണസംഖ്യ) blob_len(b);
	എങ്കിൽ (idx >= 0 && idx < blob_len(b))
	    lua_pushnumber(L, (lua_സംഖ്യ) blob_get(b, idx));
	അല്ലെങ്കിൽ
	    lua_pushnil(L);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (lua_isstring(L, 2))
    {
	സ്ഥിരം അക്ഷരം *s = lua_tostring(L, 2);
	എങ്കിൽ (strncmp(s, "add", 3) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	അല്ലെങ്കിൽ
	    lua_pushnil(L);
    }
    അല്ലെങ്കിൽ
	lua_pushnil(L);
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_blob_newindex(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    എങ്കിൽ (b->bv_lock)
	luaL_error(L, "blob is locked");

    എങ്കിൽ (!lua_isnumber(L, 2))
	തിരികെ 0;

    long len = blob_len(b);
    പൂർണ്ണസംഖ്യ idx = luaL_checkinteger(L, 2);
    പൂർണ്ണസംഖ്യ val = luaL_checkinteger(L, 3);
    എങ്കിൽ (idx < 0)
	// Negative index is relative to the end.
	idx += (പൂർണ്ണസംഖ്യ) len;
    എങ്കിൽ ((idx >= 0 && idx < len)
	    || (idx == len && ga_grow(&b->bv_ga, 1) == OK))
    {
	blob_set(b, idx, (char_u) val);
	എങ്കിൽ (idx == len)
	    ++b->bv_ga.ga_len;
    }
    അല്ലെങ്കിൽ
	luaL_error(L, "index out of range");

    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_blob_add(lua_State *L)
{
    luaV_Blob *blo = luaV_checkudata(L, 1, LUAVIM_BLOB);
    blob_T *b = (blob_T *) luaV_checkcache(L, (ശൂന്യം *) *blo);
    എങ്കിൽ (b->bv_lock)
	luaL_error(L, "blob is locked");
    lua_settop(L, 2);
    എങ്കിൽ (!lua_isstring(L, 2))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 2));
    അല്ലെങ്കിൽ
    {
	size_t i, l = 0;
	സ്ഥിരം അക്ഷരം *s = lua_tolstring(L, 2, &l);

	എങ്കിൽ (ga_grow(&b->bv_ga, (പൂർണ്ണസംഖ്യ)l) == OK)
	    വേണ്ടി (i = 0; i < l; ++i)
		ga_append(&b->bv_ga, s[i]);
    }
    lua_settop(L, 1);
    തിരികെ 1;
}

സ്ഥിതം സ്ഥിരം luaL_Reg luaV_Blob_mt[] = {
    {"__tostring", luaV_blob_tostring},
    {"__gc", luaV_blob_gc},
    {"__len", luaV_blob_len},
    {"__index", luaV_blob_index},
    {"__newindex", luaV_blob_newindex},
    {"add", luaV_blob_add},
    {ശൂന്യ, ശൂന്യ}
};


// =======   Funcref type   =======

    സ്ഥിതം luaV_Funcref *
luaV_newfuncref(lua_State *L, char_u *name)
{
    luaV_Funcref *f = (luaV_Funcref *)lua_newuserdata(L, വലുപ്പം(luaV_Funcref));

    എങ്കിൽ (name != ശൂന്യ)
    {
	func_ref(name);
	f->name = vim_strsave(name);
    }
    f->self = ശൂന്യ;
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_setmetatable(L, -2);
    തിരികെ f;
}

    സ്ഥിതം luaV_Funcref *
luaV_pushfuncref(lua_State *L, char_u *name)
{
    തിരികെ luaV_newfuncref(L, name);
}


luaV_type_tostring(funcref, LUAVIM_FUNCREF)

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_funcref_gc(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    func_unref(f->name);
    vim_free(f->name);
    // NOTE: Don't call "dict_unref(f->self)", because the dict of "f->self"
    // will be (or has been already) freed by Vim's garbage collection.
    തിരികെ 0;
}

// equivalent to string(funcref)
    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_funcref_len(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    lua_pushstring(L, (സ്ഥിരം അക്ഷരം *) f->name);
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_funcref_call(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);
    പൂർണ്ണസംഖ്യ i, n = lua_gettop(L) - 1; // #args
    പൂർണ്ണസംഖ്യ status = FAIL;
    typval_T args;
    typval_T rettv;

    args.v_type = VAR_LIST;
    args.vval.v_list = list_alloc();
    rettv.v_type = VAR_UNKNOWN; // as in clear_tv
    എങ്കിൽ (args.vval.v_list != ശൂന്യ)
    {
	typval_T v;

	വേണ്ടി (i = 0; i < n; i++)
	{
	    luaV_checktypval(L, i + 2, &v, "calling funcref");
	    list_append_tv(args.vval.v_list, &v);
	    clear_tv(&v);
	}
	status = func_call(f->name, &args, ശൂന്യ, f->self, &rettv);
	എങ്കിൽ (status == OK)
	    luaV_pushtypval(L, &rettv);
	clear_tv(&args);
	clear_tv(&rettv);
    }
    എങ്കിൽ (status != OK)
	luaL_error(L, "cannot call funcref");
    തിരികെ 1;
}

സ്ഥിതം സ്ഥിരം luaL_Reg luaV_Funcref_mt[] = {
    {"__tostring", luaV_funcref_tostring},
    {"__gc", luaV_funcref_gc},
    {"__len", luaV_funcref_len},
    {"__call", luaV_funcref_call},
    {ശൂന്യ, ശൂന്യ}
};


// =======   Buffer type   =======

luaV_newtype(buf_T, buffer, luaV_Buffer, LUAVIM_BUFFER)
luaV_pushtype(buf_T, buffer, luaV_Buffer)
luaV_type_tostring(buffer, LUAVIM_BUFFER)

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_buffer_len(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    lua_pushinteger(L, b->b_ml.ml_വരി_എണ്ണം);
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_buffer_call(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    lua_settop(L, 1);
    set_curbuf(b, DOBUF_SPLIT);
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_buffer_index(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    linenr_T n = (linenr_T) lua_tointeger(L, 2);
    എങ്കിൽ (n > 0 && n <= b->b_ml.ml_വരി_എണ്ണം)
	luaV_pushline(L, b, n);
    അല്ലെങ്കിൽ എങ്കിൽ (lua_isstring(L, 2))
    {
	സ്ഥിരം അക്ഷരം *s = lua_tostring(L, 2);
	എങ്കിൽ (strncmp(s, "name", 4) == 0)
	    lua_pushstring(L, (b->b_sfname == ശൂന്യ)
					? "" : (അക്ഷരം *) b->b_sfname);
	അല്ലെങ്കിൽ എങ്കിൽ (strncmp(s, "fname", 5) == 0)
	    lua_pushstring(L, (b->b_ffname == ശൂന്യ)
					? "" : (അക്ഷരം *) b->b_ffname);
	അല്ലെങ്കിൽ എങ്കിൽ (strncmp(s, "number", 6) == 0)
	    lua_pushinteger(L, b->b_fnum);
	// methods
	അല്ലെങ്കിൽ എങ്കിൽ (strncmp(s,   "insert", 6) == 0
		|| strncmp(s, "next", 4) == 0
		|| strncmp(s, "previous", 8) == 0
		|| strncmp(s, "isvalid", 7) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	അല്ലെങ്കിൽ
	    lua_pushnil(L);
    }
    അല്ലെങ്കിൽ
	lua_pushnil(L);
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_buffer_newindex(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    linenr_T n = (linenr_T) luaL_checkinteger(L, 2);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    എങ്കിൽ (n < 1 || n > b->b_ml.ml_വരി_എണ്ണം)
	luaL_error(L, "invalid line number");
    എങ്കിൽ (lua_isnil(L, 3)) // delete line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	എങ്കിൽ (u_savedel(n, 1L) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	അല്ലെങ്കിൽ എങ്കിൽ (ml_delete(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot delete line");
	}
	അല്ലെങ്കിൽ
	{
	    deleted_വരികൾ_mark(n, 1L);
	    എങ്കിൽ (b == curwin->w_buffer) // fix cursor in current window?
	    {
		എങ്കിൽ (curwin->w_cursor.lnum >= n)
		{
		    എങ്കിൽ (curwin->w_cursor.lnum > n)
		    {
			curwin->w_cursor.lnum -= 1;
			check_cursor_col();
		    }
		    അല്ലെങ്കിൽ
			check_cursor();
		    changed_cline_bef_curs();
		}
		invalidate_botline();
	    }
	}
	curbuf = buf;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (lua_isstring(L, 3)) // update line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	എങ്കിൽ (u_savesub(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	അല്ലെങ്കിൽ എങ്കിൽ (ml_replace(n, luaV_toline(L, 3), TRUE) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot replace line");
	}
	അല്ലെങ്കിൽ
	    changed_bytes(n, 0);
	curbuf = buf;
	എങ്കിൽ (b == curwin->w_buffer)
	    check_cursor_col();
    }
    അല്ലെങ്കിൽ
	luaL_error(L, "wrong argument to change line");
    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
lua_V_buffer_തിരുകുക(lua_State *L)
{
    luaV_Buffer *lb = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *b = (buf_T *) luaV_checkcache(L, (ശൂന്യം *) *lb);
    linenr_T last = b->b_ml.ml_വരി_എണ്ണം;
    linenr_T n = (linenr_T) luaL_optinteger(L, 3, last);
    buf_T *buf;
    luaL_checktype(L, 2, LUA_TSTRING);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    // fix insertion line
    എങ്കിൽ (n < 0) n = 0;
    എങ്കിൽ (n > last) n = last;
    // insert
    buf = curbuf;
    curbuf = b;
    എങ്കിൽ (u_save(n, n + 1) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot save undo information");
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ml_append(n, luaV_toline(L, 2), 0, FALSE) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot insert line");
    }
    അല്ലെങ്കിൽ
	appended_വരികൾ_mark(n, 1L);
    curbuf = buf;
    update_screen(UPD_VALID);
    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_buffer_next(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (ശൂന്യം *) *b);
    luaV_pushbuffer(L, buf->b_next);
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_buffer_previous(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (ശൂന്യം *) *b);
    luaV_pushbuffer(L, buf->b_prev);
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_buffer_isvalid(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    luaV_getudata(L, *b);
    lua_pushboolean(L, !lua_isnil(L, -1));
    തിരികെ 1;
}

സ്ഥിതം സ്ഥിരം luaL_Reg luaV_Buffer_mt[] = {
    {"__tostring", luaV_buffer_tostring},
    {"__len", luaV_buffer_len},
    {"__call", luaV_buffer_call},
    {"__index", luaV_buffer_index},
    {"__newindex", luaV_buffer_newindex},
    {"insert", lua_V_buffer_തിരുകുക},
    {"next", luaV_buffer_next},
    {"previous", luaV_buffer_previous},
    {"isvalid", luaV_buffer_isvalid},
    {ശൂന്യ, ശൂന്യ}
};


// =======   Window type   =======

luaV_newtype(win_T, window, luaV_Window, LUAVIM_WINDOW)
luaV_pushtype(win_T, window, luaV_Window)
luaV_type_tostring(window, LUAVIM_WINDOW)

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_window_call(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    lua_settop(L, 1);
    win_goto(w);
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_window_index(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    സ്ഥിരം അക്ഷരം *s = luaL_checkstring(L, 2);
    എങ്കിൽ (strncmp(s, "buffer", 6) == 0)
	luaV_pushbuffer(L, w->w_buffer);
    അല്ലെങ്കിൽ എങ്കിൽ (strncmp(s, "line", 4) == 0)
	lua_pushinteger(L, w->w_cursor.lnum);
    അല്ലെങ്കിൽ എങ്കിൽ (strncmp(s, "col", 3) == 0)
	lua_pushinteger(L, w->w_cursor.col + 1);
    അല്ലെങ്കിൽ എങ്കിൽ (strncmp(s, "width", 5) == 0)
	lua_pushinteger(L, w->w_width);
    അല്ലെങ്കിൽ എങ്കിൽ (strncmp(s, "height", 6) == 0)
	lua_pushinteger(L, w->w_height);
    // methods
    അല്ലെങ്കിൽ എങ്കിൽ (strncmp(s,   "next", 4) == 0
	    || strncmp(s, "previous", 8) == 0
	    || strncmp(s, "isvalid", 7) == 0)
    {
	lua_getmetatable(L, 1);
	lua_getfield(L, -1, s);
    }
    അല്ലെങ്കിൽ
	lua_pushnil(L);
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_window_newindex(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    സ്ഥിരം അക്ഷരം *s = luaL_checkstring(L, 2);
    പൂർണ്ണസംഖ്യ v = luaL_checkinteger(L, 3);
    എങ്കിൽ (strncmp(s, "line", 4) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	എങ്കിൽ (v < 1 || v > w->w_buffer->b_ml.ml_വരി_എണ്ണം)
	    luaL_error(L, "line out of range");
	w->w_cursor.lnum = v;
	update_screen(UPD_VALID);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (strncmp(s, "col", 3) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	w->w_cursor.col = v - 1;
	w->w_set_curswant = true;
	update_screen(UPD_VALID);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (strncmp(s, "width", 5) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setwidth(v);
	curwin = win;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (strncmp(s, "height", 6) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setheight(v);
	curwin = win;
    }
    അല്ലെങ്കിൽ
	luaL_error(L, "invalid window property: `%s'", s);
    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_window_next(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    win_T *win = (win_T *) luaV_checkcache(L, (ശൂന്യം *) *w);
    luaV_pushwindow(L, win->w_next);
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_window_previous(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    win_T *win = (win_T *) luaV_checkcache(L, (ശൂന്യം *) *w);
    luaV_pushwindow(L, win->w_prev);
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_window_isvalid(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    luaV_getudata(L, *w);
    lua_pushboolean(L, !lua_isnil(L, -1));
    തിരികെ 1;
}

സ്ഥിതം സ്ഥിരം luaL_Reg luaV_Window_mt[] = {
    {"__tostring", luaV_window_tostring},
    {"__call", luaV_window_call},
    {"__index", luaV_window_index},
    {"__newindex", luaV_window_newindex},
    {"next", luaV_window_next},
    {"previous", luaV_window_previous},
    {"isvalid", luaV_window_isvalid},
    {ശൂന്യ, ശൂന്യ}
};


// =======   Vim module   =======

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_print(lua_State *L)
{
    പൂർണ്ണസംഖ്യ		i, n = lua_gettop(L); // nargs
    സ്ഥിരം അക്ഷരം	*s;
    size_t	l;
    garray_T	സന്ദേശം_ga;

    ga_init2(&സന്ദേശം_ga, 1, 128);
    lua_getglobal(L, "tostring");
    വേണ്ടി (i = 1; i <= n; i++)
    {
	lua_pushvalue(L, -1); // tostring
	lua_pushvalue(L, i); // arg
	lua_call(L, 1, 1);
	s = lua_tolstring(L, -1, &l);
	എങ്കിൽ (s == ശൂന്യ)
	    തിരികെ luaL_error(L, "cannot convert to string");
	എങ്കിൽ (i > 1)
	    ga_append(&സന്ദേശം_ga, ' '); // use space instead of tab
	ga_concat_len(&സന്ദേശം_ga, (char_u *)s, l);
	lua_എടുക്കുക(L, 1);
    }
    // Replace any "\n" with "\0"
    വേണ്ടി (i = 0; i < സന്ദേശം_ga.ga_len; i++)
	എങ്കിൽ (((അക്ഷരം *)സന്ദേശം_ga.ga_data)[i] == '\n')
	    ((അക്ഷരം *)സന്ദേശം_ga.ga_data)[i] = '\0';
    lua_pushlstring(L, സന്ദേശം_ga.ga_data, സന്ദേശം_ga.ga_len);
    എങ്കിൽ (!got_int)
	lua_V_സന്ദേശം(L);

    ga_clear(&സന്ദേശം_ga);
    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_debug(lua_State *L)
{
    lua_settop(L, 0);
    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "eval");
    lua_remove(L, -2); // vim.eval at position 1
    വേണ്ടി (;;)
    {
	സ്ഥിരം അക്ഷരം *നിവേശം;
	size_t l;
	lua_pushvalue(L, 1); // vim.eval
	lua_pushliteral(L, "input('lua_debug> ')");
	lua_call(L, 1, 1); // return string
	നിവേശം = lua_tolstring(L, -1, &l);
	എങ്കിൽ (l == 0 || strcmp(നിവേശം, "cont") == 0)
	    തിരികെ 0;
	സന്ദേശം_putchar('\n'); // avoid outputting on input line
	എങ്കിൽ (luaL_loadbuffer(L, നിവേശം, l, "=(debug command)")
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
	lua_settop(L, 1); // remove eventual returns, but keep vim.eval
    }
}

    സ്ഥിതം dict_T *
luaV_get_var_scope(lua_State *L)
{
    സ്ഥിരം അക്ഷരം	*scope = luaL_checkstring(L, 1);
    dict_T	*dict = ശൂന്യ;

    എങ്കിൽ (STRICMP((അക്ഷരം *)scope, "g") == 0)
	dict = get_globvar_dict();
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP((അക്ഷരം *)scope, "v") == 0)
	dict = get_vimvar_dict();
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP((അക്ഷരം *)scope, "b") == 0)
	dict = curbuf->b_vars;
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP((അക്ഷരം *)scope, "w") == 0)
	dict = curwin->w_vars;
    അല്ലെങ്കിൽ എങ്കിൽ (STRICMP((അക്ഷരം *)scope, "t") == 0)
	dict = curtab->tp_vars;
    അല്ലെങ്കിൽ
    {
	luaL_error(L, "invalid scope %s", scope);
	തിരികെ ശൂന്യ;
    }

    തിരികെ dict;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_setvar(lua_State *L)
{
    dict_T	*dict;
    dictitem_T	*di;
    size_t	len;
    അക്ഷരം	*name;
    പൂർണ്ണസംഖ്യ		del;
    അക്ഷരം	*error = ശൂന്യ;

    name = (അക്ഷരം *)luaL_checklstring(L, 3, &len);
    del = (lua_gettop(L) < 4) || lua_isnil(L, 4);

    dict = luaV_get_var_scope(L);
    എങ്കിൽ (dict == ശൂന്യ)
	തിരികെ 0;

    di = dict_find(dict, (char_u *)name, (പൂർണ്ണസംഖ്യ)len);
    എങ്കിൽ (di != ശൂന്യ)
    {
	എങ്കിൽ (di->di_flags & DI_FLAGS_RO)
	    error = "variable is read-only";
	അല്ലെങ്കിൽ എങ്കിൽ (di->di_flags & DI_FLAGS_LOCK)
	    error = "variable is locked";
	അല്ലെങ്കിൽ എങ്കിൽ (del && di->di_flags & DI_FLAGS_FIX)
	    error = "variable is fixed";
	എങ്കിൽ (error != ശൂന്യ)
	    തിരികെ luaL_error(L, error);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (dict->dv_lock)
	തിരികെ luaL_error(L, "Dictionary is locked");

    എങ്കിൽ (del)
    {
	// Delete the key
	എങ്കിൽ (di == ശൂന്യ)
	    // Doesn't exist, nothing to do
	    തിരികെ 0;
	// Delete the entry
	dictitem_remove(dict, di, "Lua delete variable");
    }
    അല്ലെങ്കിൽ
    {
	// Update the key
	typval_T	tv;

	// Convert the lua value to a Vim script type in the temporary variable
	lua_pushvalue(L, 4);
	എങ്കിൽ (luaV_totypval(L, -1, &tv) == FAIL)
	    തിരികെ luaL_error(L, "Couldn't convert lua value");

	എങ്കിൽ (di == ശൂന്യ)
	{
	    // Need to create an entry
	    di = dictitem_alloc((char_u *)name);
	    എങ്കിൽ (di == ശൂന്യ)
	    {
		clear_tv(&tv);
		തിരികെ 0;
	    }
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	    എങ്കിൽ (dict_add(dict, di) == FAIL)
	    {
		dictitem_free(di);
		clear_tv(&tv);
		തിരികെ luaL_error(L, "Couldn't add to dictionary");
	    }
	}
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ type_error = FALSE;
	    എങ്കിൽ (dict == get_vimvar_dict()
	       && !before_set_vvar((char_u *)name, di, &tv, TRUE, &type_error))
	    {
		clear_tv(&tv);
		എങ്കിൽ (type_error)
		    തിരികെ luaL_error(L,
				"Setting v:%s to value with wrong type", name);
		തിരികെ 0;
	    }
	    // Clear the old value
	    clear_tv(&di->di_tv);
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	}

	// Clear the temporary variable
	clear_tv(&tv);
    }

    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_getvar(lua_State *L)
{
    dict_T	*dict = luaV_get_var_scope(L);
    size_t	len;
    സ്ഥിരം അക്ഷരം	*name = luaL_checklstring(L, 3, &len);
    dictitem_T	*di = dict_find(dict, (char_u *)name, (പൂർണ്ണസംഖ്യ)len);

    എങ്കിൽ (di == ശൂന്യ)
	തിരികെ 0;  // nil

    luaV_pushtypval(L, &di->di_tv);
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_command(lua_State *L)
{
    char_u  *s = vim_strsave((char_u *)luaL_checkstring(L, 1));

    execute_cmds_from_string(s);
    vim_free(s);
    update_screen(UPD_VALID);
    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_eval(lua_State *L)
{
    typval_T *tv = eval_വ്യഞ്ജകം((char_u *) luaL_checkstring(L, 1), ശൂന്യ);

    എങ്കിൽ (tv == ശൂന്യ) luaL_error(L, "invalid expression");
    luaV_pushtypval(L, tv);
    free_tv(tv);
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_beep(lua_State *L UNUSED)
{
    vim_beep(BO_LANG);
    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
lua_V_വരി(lua_State *L)
{
    luaV_pushline(L, curbuf, curwin->w_cursor.lnum);
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_list(lua_State *L)
{
    list_T *l;
    പൂർണ്ണസംഖ്യ initarg = !lua_isnoneornil(L, 1);

    എങ്കിൽ (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    l = list_alloc();
    എങ്കിൽ (l == ശൂന്യ)
    {
	lua_pushnil(L);
	തിരികെ 1;
    }

    luaV_newlist(L, l);
    എങ്കിൽ (!initarg)
	തിരികെ 1;

    // traverse table to init list
    പൂർണ്ണസംഖ്യ notnil, i = 0;
    typval_T v;
    ചെയ്യുക
    {
	lua_rawgeti(L, 1, ++i);
	notnil = !lua_isnil(L, -1);
	എങ്കിൽ (notnil)
	{
	    luaV_checktypval(L, -1, &v, "vim.list");
	    list_append_tv(l, &v);
	    clear_tv(&v);
	}
	lua_എടുക്കുക(L, 1); //മൂല്യം
    } ആകുമ്പോൾ (notnil);

    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_dict(lua_State *L)
{
    dict_T *d;
    പൂർണ്ണസംഖ്യ initarg = !lua_isnoneornil(L, 1);

    എങ്കിൽ (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    d = dict_alloc();
    എങ്കിൽ (d == ശൂന്യ)
    {
	lua_pushnil(L);
	തിരികെ 1;
    }

    luaV_newdict(L, d);
    എങ്കിൽ (!initarg)
	തിരികെ 1;

    // traverse table to init dict
    lua_pushnil(L);
    ആകുമ്പോൾ (lua_next(L, 1))
    {
	char_u *താക്കോൽ;
	dictitem_T *di;
	typval_T v;

	lua_pushvalue(L, -2); // dup key in case it's a number
	താക്കോൽ = (char_u *) lua_tostring(L, -1);
	എങ്കിൽ (താക്കോൽ == ശൂന്യ)
	{
	    lua_pushnil(L);
	    തിരികെ 1;
	}
	എങ്കിൽ (*താക്കോൽ == NUL)
	    luaL_error(L, "table has empty key");
	luaV_checktypval(L, -2, &v, "vim.dict"); //മൂല്യം
	di = dictitem_alloc(താക്കോൽ);
	എങ്കിൽ (di == ശൂന്യ || dict_add(d, di) == FAIL)
	{
	    vim_free(di);
	    lua_pushnil(L);
	    തിരികെ 1;
	}
	di->di_tv = v;
	lua_എടുക്കുക(L, 2); // key copy and value
    }

    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_blob(lua_State *L)
{
    blob_T *b;
    പൂർണ്ണസംഖ്യ initarg = !lua_isnoneornil(L, 1);

    എങ്കിൽ (initarg && !lua_isstring(L, 1))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 1));

    b = blob_alloc();
    എങ്കിൽ (b == ശൂന്യ)
    {
	lua_pushnil(L);
	തിരികെ 1;
    }

    luaV_newblob(L, b);
    എങ്കിൽ (!initarg)
	തിരികെ 1;

    // traverse table to init blob
    size_t i, l = 0;
    സ്ഥിരം അക്ഷരം *s = lua_tolstring(L, 1, &l);

    എങ്കിൽ (ga_grow(&b->bv_ga, (പൂർണ്ണസംഖ്യ)l) == OK)
	വേണ്ടി (i = 0; i < l; ++i)
	    ga_append(&b->bv_ga, s[i]);

    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_funcref(lua_State *L)
{
    സ്ഥിരം അക്ഷരം *name = luaL_checkstring(L, 1);
    // note: not checking if function exists (needs function_exists)
    എങ്കിൽ (name == ശൂന്യ || *name == NUL || VIM_ISDIGIT(*name))
	luaL_error(L, "invalid function name: %s", name);
    luaV_newfuncref(L, (char_u *) name);
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_buffer(lua_State *L)
{
    buf_T *buf;
    എങ്കിൽ (lua_isstring(L, 1)) // get by number or name?
    {
	എങ്കിൽ (lua_isnumber(L, 1)) // by number?
	{
	    പൂർണ്ണസംഖ്യ n = lua_tointeger(L, 1);
	    FOR_ALL_BUFFERS(buf)
		എങ്കിൽ (buf->b_fnum == n) നിർത്തുക;
	}
	അല്ലെങ്കിൽ // by name
	{
	    size_t l;
	    സ്ഥിരം അക്ഷരം *s = lua_tolstring(L, 1, &l);
	    FOR_ALL_BUFFERS(buf)
	    {
		എങ്കിൽ (buf->b_ffname == ശൂന്യ || buf->b_sfname == ശൂന്യ)
		{
		    എങ്കിൽ (l == 0) നിർത്തുക;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (strncmp(s, (അക്ഷരം *)buf->b_ffname, l) == 0
			|| strncmp(s, (അക്ഷരം *)buf->b_sfname, l) == 0)
		    നിർത്തുക;
	    }
	}
    }
    അല്ലെങ്കിൽ
	buf = (lua_toboolean(L, 1)) ? firstbuf : curbuf; // first buffer?
    luaV_pushbuffer(L, buf);
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_window(lua_State *L)
{
    win_T *win;
    എങ്കിൽ (lua_isnumber(L, 1)) // get by number?
    {
	പൂർണ്ണസംഖ്യ n = lua_tointeger(L, 1);
	വേണ്ടി (win = firstwin; win != ശൂന്യ; win = win->w_next, n--)
	    എങ്കിൽ (n == 1) നിർത്തുക;
    }
    അല്ലെങ്കിൽ
	win = (lua_toboolean(L, 1)) ? firstwin : curwin; // first window?
    luaV_pushwindow(L, win);
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_open(lua_State *L)
{
    char_u *s = ശൂന്യ;
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    എങ്കിൽ (lua_isstring(L, 1)) s = (char_u *) lua_tostring(L, 1);
    luaV_pushbuffer(L, buflist_new(s, ശൂന്യ, 1L, BLN_LISTED));
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_type(lua_State *L)
{
    luaL_checkany(L, 1);
    എങ്കിൽ (lua_type(L, 1) == LUA_TUSERDATA) // check vim udata?
    {
	lua_settop(L, 1);
	എങ്കിൽ (lua_getmetatable(L, 1))
	{
	    luaV_getfield(L, LUAVIM_LIST);
	    എങ്കിൽ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "list");
		തിരികെ 1;
	    }
	    luaV_getfield(L, LUAVIM_DICT);
	    എങ്കിൽ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "dict");
		തിരികെ 1;
	    }
	    luaV_getfield(L, LUAVIM_BLOB);
	    എങ്കിൽ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "blob");
		തിരികെ 1;
	    }
	    luaV_getfield(L, LUAVIM_FUNCREF);
	    എങ്കിൽ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "funcref");
		തിരികെ 1;
	    }
	    luaV_getfield(L, LUAVIM_BUFFER);
	    എങ്കിൽ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "buffer");
		തിരികെ 1;
	    }
	    luaV_getfield(L, LUAVIM_WINDOW);
	    എങ്കിൽ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "window");
		തിരികെ 1;
	    }
	}
    }
    lua_pushstring(L, luaL_typename(L, 1)); // fallback
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_call(lua_State *L)
{
    പൂർണ്ണസംഖ്യ		argc = lua_gettop(L) - 1;
    size_t	funcname_len;
    char_u	*funcname;
    അക്ഷരം	*error = ശൂന്യ;
    typval_T	rettv;
    typval_T	argv[MAX_FUNC_ARGS + 1];
    പൂർണ്ണസംഖ്യ		i = 0;

    എങ്കിൽ (argc > MAX_FUNC_ARGS)
	തിരികെ luaL_error(L, "Function called with too many arguments");

    funcname = (char_u *)luaL_checklstring(L, 1, &funcname_len);

    വേണ്ടി (; i < argc; i++)
    {
	എങ്കിൽ (luaV_totypval(L, i + 2, &argv[i]) == FAIL)
	{
	    error = "lua: cannot convert value";
	    പോകുക free_vim_args;
	}
    }

    argv[argc].v_type = VAR_UNKNOWN;

    എങ്കിൽ (call_vim_function(funcname, argc, argv, &rettv) == FAIL)
    {
	error = "lua: call_vim_function failed";
	പോകുക free_vim_args;
    }

    luaV_pushtypval(L, &rettv);
    clear_tv(&rettv);

free_vim_args:
    ആകുമ്പോൾ (i > 0)
	clear_tv(&argv[--i]);

    എങ്കിൽ (error == ശൂന്യ)
	തിരികെ 1;
    അല്ലെങ്കിൽ
	തിരികെ luaL_error(L, error);
}

/*
 * Return the Vim version as a Lua table
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
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
    തിരികെ 1;
}

സ്ഥിതം സ്ഥിരം luaL_Reg luaV_module[] = {
    {"command", luaV_command},
    {"eval", luaV_eval},
    {"beep", luaV_beep},
    {"line", lua_V_വരി},
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
    {"lua_version", ശൂന്യ},
    {ശൂന്യ, ശൂന്യ}
};

/*
 * for freeing list, dict, buffer and window objects; lightuserdata as arg
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_free(lua_State *L)
{
    lua_pushnil(L);
    luaV_setudata(L, lua_touserdata(L, 1));
    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_luaeval(lua_State *L)
{
    luaL_Buffer b;
    size_t l;
    സ്ഥിരം അക്ഷരം *str = lua_tolstring(L, 1, &l);
    typval_T *arg = (typval_T *) lua_touserdata(L, 2);
    typval_T *rettv = (typval_T *) lua_touserdata(L, 3);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, LUAVIM_EVALHEADER, വലുപ്പം(LUAVIM_EVALHEADER) - 1);
    luaL_addlstring(&b, str, l);
    luaL_pushresult(&b);
    str = lua_tolstring(L, -1, &l);
    എങ്കിൽ (luaL_loadbuffer(L, str, l, LUAVIM_EVALNAME)) // compile error?
    {
	luaV_emsg(L);
	തിരികെ 0;
    }
    luaV_pushtypval(L, arg);
    എങ്കിൽ (lua_pcall(L, 1, 1, 0)) // running error?
    {
	luaV_emsg(L);
	തിരികെ 0;
    }
    എങ്കിൽ (luaV_totypval(L, -1, rettv) == FAIL)
	emsg("luaeval: cannot convert value");
    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_setref(lua_State *L)
{
    പൂർണ്ണസംഖ്യ copyID = lua_tointeger(L, 1);
    പൂർണ്ണസംഖ്യ abort = FALSE;

    lua_pushlightuserdata(L, (ശൂന്യം *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // the cache table

    luaV_getfield(L, LUAVIM_LIST);
    luaV_getfield(L, LUAVIM_DICT);
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_pushnil(L);
    // traverse cache table
    ആകുമ്പോൾ (!abort && lua_next(L, 2) != 0)
    {
	lua_getmetatable(L, -1);
	എങ്കിൽ (lua_rawequal(L, -1, 3)) // list?
	{
	    list_T *l = (list_T *)lua_touserdata(L, 6); //താക്കോൽ

	    abort = set_ref_in_list(l, copyID);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (lua_rawequal(L, -1, 4)) // dict?
	{
	    dict_T *d = (dict_T *)lua_touserdata(L, 6); //താക്കോൽ

	    abort = set_ref_in_dict(d, copyID);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (lua_rawequal(L, -1, 5)) // funcref?
	{
	    luaV_Funcref *f = (luaV_Funcref *)lua_touserdata(L, 6); //താക്കോൽ

	    abort = set_ref_in_dict(f->self, copyID);
	}
	lua_എടുക്കുക(L, 2); // metatable and value
    }
    lua_pushinteger(L, abort);
    തിരികെ 1;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_pushversion(lua_State *L)
{
    പൂർണ്ണസംഖ്യ major = 0;
    പൂർണ്ണസംഖ്യ minor = 0;
    പൂർണ്ണസംഖ്യ patch = 0;
    അക്ഷരം s[16];

    sscanf(LUAVIM_VERSION, "Lua %d.%d.%d", &major, &minor, &patch);
    vim_snprintf(s, വലുപ്പം(s), "%d.%d.%d", major, minor, patch);
    lua_pushstring(L, s);
    തിരികെ 0;
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

    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaopen_vim(lua_State *L)
{
    lua_newtable(L);  // cache table
    lua_newtable(L);  // cache table's metatable
    lua_pushstring(L, "v");
    lua_setfield(L, -2, "__mode");
    lua_setmetatable(L, -2); // cache is weak-valued
    // put the cache table in the registry for luaV_get/setudata()
    lua_pushlightuserdata(L, (ശൂന്യം *) LUAVIM_UDATA_CACHE);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
    lua_എടുക്കുക(L, 1);  // we don't need the cache table here anymore
    // print
    lua_pushcfunction(L, luaV_print);
    lua_setglobal(L, "print");
    // debug.debug
    lua_getglobal(L, "debug");
    lua_pushcfunction(L, luaV_debug);
    lua_setfield(L, -2, "debug");
    lua_എടുക്കുക(L, 1);
    // free
    lua_pushlightuserdata(L, (ശൂന്യം *) LUAVIM_FREE);
    lua_pushcfunction(L, luaV_free);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // luaeval
    lua_pushlightuserdata(L, (ശൂന്യം *) LUAVIM_LUAEVAL);
    lua_pushcfunction(L, luaV_luaeval);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // setref
    lua_pushlightuserdata(L, (ശൂന്യം *) LUAVIM_SETREF);
    lua_pushcfunction(L, luaV_setref);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // register
    luaV_newmetatable(L, LUAVIM_LIST);
    luaV_register(L, luaV_List_mt);
    lua_എടുക്കുക(L, 1);
    luaV_newmetatable(L, LUAVIM_DICT);
    luaV_register(L, luaV_Dict_mt);
    lua_എടുക്കുക(L, 1);
    luaV_newmetatable(L, LUAVIM_BLOB);
    luaV_register(L, luaV_Blob_mt);
    lua_എടുക്കുക(L, 1);
    luaV_newmetatable(L, LUAVIM_FUNCREF);
    luaV_register(L, luaV_Funcref_mt);
    lua_എടുക്കുക(L, 1);
    luaV_newmetatable(L, LUAVIM_BUFFER);
    luaV_register(L, luaV_Buffer_mt);
    lua_എടുക്കുക(L, 1);
    luaV_newmetatable(L, LUAVIM_WINDOW);
    luaV_register(L, luaV_Window_mt);
    lua_എടുക്കുക(L, 1);
    lua_newtable(L); // vim table
    luaV_register(L, luaV_module);
    luaV_pushversion(L);
    lua_setfield(L, -2, "lua_version");
    lua_setglobal(L, LUAVIM_NAME);
    // custom code
    (ശൂന്യം)luaL_dostring(L, LUA_VIM_FN_CODE);
    (ശൂന്യം)luaL_dostring(L, LUA_VIM_UPDATE_PACKAGE_PATHS);
    (ശൂന്യം)luaL_dostring(L, LUA_VIM_SETUP_VARIABLE_DICTS);

    lua_getglobal(L, LUAVIM_NAME);
    lua_getfield(L, -1, "_update_package_paths");

    എങ്കിൽ (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);

    തിരികെ 0;
}

    സ്ഥിതം lua_State *
luaV_newstate(ശൂന്യം)
{
    lua_State *L = luaL_newstate();
# എങ്കിൽ LUA_VERSION_അക്കം >= 505
    luaL_openselectedlibs(L, ~0, 0); // core libs
# അല്ലെങ്കിൽ
    luaL_openlibs(L); // core libs
# endif
    lua_pushcfunction(L, luaopen_vim); // vim
    lua_call(L, 0, 0);
    തിരികെ L;
}

    സ്ഥിതം ശൂന്യം
luaV_setrange(lua_State *L, പൂർണ്ണസംഖ്യ line1, പൂർണ്ണസംഖ്യ line2)
{
    lua_getglobal(L, LUAVIM_NAME);
    lua_pushinteger(L, line1);
    lua_setfield(L, -2, "firstline");
    lua_pushinteger(L, line2);
    lua_setfield(L, -2, "lastline");
    lua_എടുക്കുക(L, 1); // vim table
}


// =======   Interface   =======

സ്ഥിതം lua_State *L = ശൂന്യ;

    സ്ഥിതം പൂർണ്ണസംഖ്യ
lua_isopen(ശൂന്യം)
{
    തിരികെ L != ശൂന്യ;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
lua_init(ശൂന്യം)
{
    എങ്കിൽ (lua_isopen())
	തിരികെ OK;

# ifdef DYNAMIC_LUA
    എങ്കിൽ (!lua_enabled(TRUE))
    {
	emsg(_("Lua library cannot be loaded."));
	തിരികെ FAIL;
    }
# endif
    L = luaV_newstate();

    തിരികെ OK;
}

    ശൂന്യം
lua_end(ശൂന്യം)
{
    എങ്കിൽ (!lua_isopen())
	തിരികെ;

    lua_close(L);
    L = ശൂന്യ;
}

/*
 * ex commands
 */
    ശൂന്യം
ex_lua(exarg_T *eap)
{
    അക്ഷരം *script = (അക്ഷരം *)script_get(eap, eap->arg);

    എങ്കിൽ (!eap->skip && lua_init() == OK)
    {
	അക്ഷരം *s = script != ശൂന്യ ? script : (അക്ഷരം *)eap->arg;

	luaV_setrange(L, eap->line1, eap->line2);
	എങ്കിൽ (luaL_loadbuffer(L, s, strlen(s), LUAVIM_CHUNKNAME)
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
    എങ്കിൽ (script != ശൂന്യ)
	vim_free(script);
}

    ശൂന്യം
ex_luado(exarg_T *eap)
{
    linenr_T l;
    സ്ഥിരം അക്ഷരം *s = (സ്ഥിരം അക്ഷരം *) eap->arg;
    luaL_Buffer b;
    size_t len;
    buf_T *was_curbuf = curbuf;

    എങ്കിൽ (lua_init() == FAIL) തിരികെ;
    എങ്കിൽ (u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
    {
	emsg(_("cannot save undo information"));
	തിരികെ;
    }
    luaV_setrange(L, eap->line1, eap->line2);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, "return function(line, linenr) ", 30); // header
    luaL_addlstring(&b, s, strlen(s));
    luaL_addlstring(&b, " end", 4); // footer
    luaL_pushresult(&b);
    s = lua_tolstring(L, -1, &len);
    എങ്കിൽ (luaL_loadbuffer(L, s, len, LUAVIM_CHUNKNAME))
    {
	luaV_emsg(L);
	lua_എടുക്കുക(L, 1); // function body
	തിരികെ;
    }
    lua_call(L, 0, 1);
    lua_replace(L, -2); // function -> body
    വേണ്ടി (l = eap->line1; l <= eap->line2; l++)
    {
	// Check the line number, the command may have deleted lines.
	എങ്കിൽ (l > curbuf->b_ml.ml_വരി_എണ്ണം)
	    നിർത്തുക;

	lua_pushvalue(L, -1); // function
	luaV_pushline(L, curbuf, l); // current line as arg
	lua_pushinteger(L, l); // current line number as arg
	എങ്കിൽ (lua_pcall(L, 2, 1, 0))
	{
	    luaV_emsg(L);
	    നിർത്തുക;
	}

	// Catch the command switching to another buffer.
	// Check the line number, the command may have deleted lines.
	എങ്കിൽ (curbuf != was_curbuf || l > curbuf->b_ml.ml_വരി_എണ്ണം)
	    നിർത്തുക;

	എങ്കിൽ (lua_isstring(L, -1)) // update line?
	{
# ifdef HAVE_SANDBOX
	    luaV_checksandbox(L);
# endif
	    ml_replace(l, luaV_toline(L, -1), TRUE);
	    changed_bytes(l, 0);
	    lua_എടുക്കുക(L, 1); // result from luaV_toline
	}
	lua_എടുക്കുക(L, 1); //വരി
    }
    lua_എടുക്കുക(L, 1); // function
    check_cursor();
    update_screen(UPD_NOT_VALID);
}

    ശൂന്യം
ex_luafile(exarg_T *eap)
{
    എങ്കിൽ (lua_init() == FAIL)
	തിരികെ;
    എങ്കിൽ (!eap->skip)
    {
	luaV_setrange(L, eap->line1, eap->line2);
	എങ്കിൽ (luaL_loadfile(L, (അക്ഷരം *) eap->arg) || lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
}

# define luaV_freetype(typ,tname) \
	ശൂന്യം \
    lua_##tname##_free(typ *o) \
    { \
	എങ്കിൽ (!lua_isopen()) തിരികെ; \
	luaV_getfield(L, LUAVIM_FREE); \
	lua_pushlightuserdata(L, (ശൂന്യം *) o); \
	lua_call(L, 1, 0); \
    }

luaV_freetype(buf_T, buffer)
luaV_freetype(win_T, window)

    ശൂന്യം
do_luaeval(char_u *str, typval_T *arg, typval_T *rettv)
{
    lua_init();
    luaV_getfield(L, LUAVIM_LUAEVAL);
    lua_pushstring(L, (അക്ഷരം *) str);
    lua_pushlightuserdata(L, (ശൂന്യം *) arg);
    lua_pushlightuserdata(L, (ശൂന്യം *) rettv);
    lua_call(L, 3, 0);
}

    പൂർണ്ണസംഖ്യ
set_ref_in_lua(പൂർണ്ണസംഖ്യ copyID)
{
    പൂർണ്ണസംഖ്യ aborted = 0;

    എങ്കിൽ (!lua_isopen())
	തിരികെ 0;

    luaV_getfield(L, LUAVIM_SETREF);
    // call the function with 1 arg, getting 1 result back
    lua_pushinteger(L, copyID);
    lua_call(L, 1, 1);
    // get the result
    aborted = lua_tointeger(L, -1);
    // pop result off the stack
    lua_എടുക്കുക(L, 1);

    തിരികെ aborted;
}

    ശൂന്യം
update_package_paths_in_lua(ശൂന്യം)
{
    എങ്കിൽ (!lua_isopen())
	തിരികെ;

    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "_update_package_paths");

    എങ്കിൽ (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);
}

/*
 * Native C function callback
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
luaV_call_lua_func(
	പൂർണ്ണസംഖ്യ	 argcount,
	typval_T *argvars,
	typval_T *rettv,
	ശൂന്യം	 *state)
{
    പൂർണ്ണസംഖ്യ i;
    പൂർണ്ണസംഖ്യ luaargcount = argcount;
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_funcref);

    എങ്കിൽ (funcstate->lua_tableref != LUA_NOREF)
    {
	// First arg for metatable __call method is a table
	luaargcount += 1;
	lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    }

    വേണ്ടി (i = 0; i < argcount; ++i)
	luaV_pushtypval(funcstate->L, &argvars[i]);

    എങ്കിൽ (lua_pcall(funcstate->L, luaargcount, 1, 0))
    {
	luaV_emsg(funcstate->L);
	തിരികെ (പൂർണ്ണസംഖ്യ)FCERR_OTHER;
    }

    luaV_checktypval(funcstate->L, -1, rettv, "get return value");
    തിരികെ (പൂർണ്ണസംഖ്യ)FCERR_NONE;
}

/*
 * Free up any lua references held by the func state.
 */
    സ്ഥിതം ശൂന്യം
luaV_call_lua_func_free(ശൂന്യം *state)
{
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_funcref);
    funcstate->L = ശൂന്യ;
    എങ്കിൽ (funcstate->lua_tableref != LUA_NOREF)
	luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    VIM_CLEAR(funcstate);
}

#endif
