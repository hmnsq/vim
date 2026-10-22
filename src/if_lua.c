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

#અન્તર્ભાવ "vim.h"
#અન્તર્ભાવ "version.h"

#અન્તર્ભાવ <lua.h>
#અન્તર્ભાવ <lualib.h>
#અન્તર્ભાવ <lauxlib.h>

#જો __STDC_VERSION__ >= 199901L
# define LUAV_INLINE અંતઃરેખ
#નહીંતર
# define LUAV_INLINE
#endif

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#જો defined(FEAT_LUA)

# define LUAVIM_CHUNKNAME "vim chunk"
# define LUAVIM_NAME "vim"
# define LUAVIM_EVALNAME "luaeval"
# define LUAVIM_EVALHEADER "local _A=select(1,...) return "

# ifdef LUA_RELEASE
#  define LUAVIM_VERSION LUA_RELEASE
# નહીંતર
#  define LUAVIM_VERSION LUA_VERSION
# endif

પ્રકારવ્યાખ્યા buf_T *luaV_Buffer;
પ્રકારવ્યાખ્યા win_T *luaV_Window;
પ્રકારવ્યાખ્યા dict_T *luaV_Dict;
પ્રકારવ્યાખ્યા list_T *luaV_List;
પ્રકારવ્યાખ્યા blob_T *luaV_Blob;
પ્રકારવ્યાખ્યા સંરચના {
    char_u	*name;	// funcref
    dict_T	*self;	// selfdict
} luaV_Funcref;
પ્રકારવ્યાખ્યા પૂર્ણાંક (*msgfunc_T)(અક્ષર *);

પ્રકારવ્યાખ્યા સંરચના {
    પૂર્ણાંક lua_funcref;    // ref to a lua func
    પૂર્ણાંક lua_tableref;   // ref to a lua table if metatable else LUA_NOREF. used
			// for __call
    lua_State *L;
} luaV_CFuncState;

સ્થિર અચલ અક્ષર LUAVIM_DICT[] = "dict";
સ્થિર અચલ અક્ષર LUAVIM_LIST[] = "list";
સ્થિર અચલ અક્ષર LUAVIM_BLOB[] = "blob";
સ્થિર અચલ અક્ષર LUAVIM_FUNCREF[] = "funcref";
સ્થિર અચલ અક્ષર LUAVIM_BUFFER[] = "buffer";
સ્થિર અચલ અક્ષર LUAVIM_WINDOW[] = "window";
સ્થિર અચલ અક્ષર LUAVIM_FREE[] = "luaV_free";
સ્થિર અચલ અક્ષર LUAVIM_LUAEVAL[] = "luaV_luaeval";
સ્થિર અચલ અક્ષર LUAVIM_SETREF[] = "luaV_setref";

સ્થિર અચલ અક્ષર LUA___CALL[] = "__call";

// get/setudata manage references to vim userdata in a cache table through
// object pointers (light userdata). The cache table itself is retrieved
// from the registry.

સ્થિર અચલ અક્ષર LUAVIM_UDATA_CACHE[] = "luaV_udata_cache";

# define luaV_getfield(L, s) \
    lua_pushlightuserdata((L), (શૂન્ય *)(s)); \
    lua_rawget((L), LUA_REGISTRYINDEX)
# define luaV_checksandbox(L) \
    જો (sandbox) luaL_error((L), "not allowed in sandbox")
# define lua_V_સંદેશ(L) luaV_msgfunc((L), (msgfunc_T) સંદેશ)
# define luaV_emsg(L) luaV_msgfunc((L), (msgfunc_T) emsg)
# define luaV_checktypval(L, a, v, સંદેશ) \
    કરો { \
	જો (luaV_totypval(L, a, v) == FAIL) \
	    luaL_error(L, સંદેશ ": cannot convert value"); \
    } જ્યારે (0)

સ્થિર luaV_List *luaV_pushlist(lua_State *L, list_T *lis);
સ્થિર luaV_Dict *luaV_pushdict(lua_State *L, dict_T *dic);
સ્થિર luaV_Blob *luaV_pushblob(lua_State *L, blob_T *blo);
સ્થિર luaV_Funcref *luaV_pushfuncref(lua_State *L, char_u *name);
સ્થિર પૂર્ણાંક luaV_call_lua_func(પૂર્ણાંક argcount, typval_T *argvars, typval_T *rettv, શૂન્ય *state);
સ્થિર શૂન્ય luaV_call_lua_func_free(શૂન્ય *state);

# જો LUA_VERSION_અંક <= 501
#  define luaV_register(L, l) luaL_register(L, ખાલી, l)
#  define luaL_typeerror luaL_typerror
# નહીંતર
#  define luaV_register(L, l) luaL_setfuncs(L, l, 0)
# endif

# ifdef DYNAMIC_LUA

#  ifdef MSWIN
#   define load_dll vimLoadLib
#   define પ્રતીક_from_dll GetProcAddress
#   define close_dll FreeLibrary
#   define load_dll_error GetWin32Error
#  નહીંતર
#   include <dlfcn.h>
#   define HANDLE શૂન્ય*
#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#   define પ્રતીક_from_dll dlsym
#   define close_dll dlclose
#   define load_dll_error dlerror
#  endif

// lauxlib
#  જો LUA_VERSION_અંક <= 501
#   define luaL_register dll_luaL_register
#   define luaL_prepbuffer dll_luaL_prepbuffer
#   define luaL_openlib dll_luaL_openlib
#   define luaL_typerror dll_luaL_typerror
#   define luaL_loadfile dll_luaL_loadfile
#   define luaL_loadbuffer dll_luaL_loadbuffer
#  નહીંતર
#   define luaL_prepbuffsize dll_luaL_prepbuffsize
#   define luaL_setfuncs dll_luaL_setfuncs
#   define luaL_loadfilex dll_luaL_loadfilex
#   define luaL_loadbufferx dll_luaL_loadbufferx
#   define luaL_argerror dll_luaL_argerror
#  endif
#  જો LUA_VERSION_અંક >= 504
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
#  જો LUA_VERSION_અંક <= 501
#   define lua_tonumber dll_lua_tonumber
#   define lua_tointeger dll_lua_tointeger
#   define lua_call dll_lua_call
#   define lua_pcall dll_lua_pcall
#  નહીંતર
#   define lua_tonumberx dll_lua_tonumberx
#   define lua_tointegerx dll_lua_tointegerx
#   define lua_callk dll_lua_callk
#   define lua_pcallk dll_lua_pcallk
#   define lua_getglobal dll_lua_getglobal
#   define lua_setglobal dll_lua_setglobal
#  endif
#  જો LUA_VERSION_અંક <= 502
#   define lua_replace dll_lua_replace
#   define lua_remove dll_lua_remove
#  endif
#  જો LUA_VERSION_અંક >= 503
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
#  જો LUA_VERSION_અંક >= 504
#   define lua_newuserdatauv dll_lua_newuserdatauv
#  નહીંતર
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
#  જો LUA_VERSION_અંક >= 505
#   define luaL_openselectedlibs dll_luaL_openselectedlibs
#  નહીંતર
#   define luaL_openlibs dll_luaL_openlibs
#  endif

// lauxlib
#  જો LUA_VERSION_અંક <= 501
શૂન્ય (*dll_luaL_register) (lua_State *L, અચલ અક્ષર *libname, અચલ luaL_Reg *l);
અક્ષર *(*dll_luaL_prepbuffer) (luaL_Buffer *B);
શૂન્ય (*dll_luaL_openlib) (lua_State *L, અચલ અક્ષર *libname, અચલ luaL_Reg *l, પૂર્ણાંક nup);
પૂર્ણાંક (*dll_luaL_typerror) (lua_State *L, પૂર્ણાંક narg, અચલ અક્ષર *tname);
પૂર્ણાંક (*dll_luaL_loadfile) (lua_State *L, અચલ અક્ષર *filename);
પૂર્ણાંક (*dll_luaL_loadbuffer) (lua_State *L, અચલ અક્ષર *buff, size_t sz, અચલ અક્ષર *name);
#  નહીંતર
અક્ષર *(*dll_luaL_prepbuffsize) (luaL_Buffer *B, size_t sz);
શૂન્ય (*dll_luaL_setfuncs) (lua_State *L, અચલ luaL_Reg *l, પૂર્ણાંક nup);
પૂર્ણાંક (*dll_luaL_loadfilex) (lua_State *L, અચલ અક્ષર *filename, અચલ અક્ષર *mode);
પૂર્ણાંક (*dll_luaL_loadbufferx) (lua_State *L, અચલ અક્ષર *buff, size_t sz, અચલ અક્ષર *name, અચલ અક્ષર *mode);
પૂર્ણાંક (*dll_luaL_argerror) (lua_State *L, પૂર્ણાંક numarg, અચલ અક્ષર *extramsg);
#  endif
#  જો LUA_VERSION_અંક >= 504
પૂર્ણાંક (*dll_luaL_typeerror) (lua_State *L, પૂર્ણાંક narg, અચલ અક્ષર *tname);
#  endif
શૂન્ય (*dll_luaL_checkany) (lua_State *L, પૂર્ણાંક narg);
અચલ અક્ષર *(*dll_luaL_checklstring) (lua_State *L, પૂર્ણાંક અંક_Arg, size_t *l);
lua_Integer (*dll_luaL_checkinteger) (lua_State *L, પૂર્ણાંક અંક_Arg);
lua_Integer (*dll_luaL_optinteger) (lua_State *L, પૂર્ણાંક nArg, lua_Integer def);
શૂન્ય (*dll_luaL_checktype) (lua_State *L, પૂર્ણાંક narg, પૂર્ણાંક t);
પૂર્ણાંક (*dll_luaL_error) (lua_State *L, અચલ અક્ષર *fmt, ...);
lua_State *(*dll_luaL_newstate) (શૂન્ય);
શૂન્ય (*dll_luaL_buffinit) (lua_State *L, luaL_Buffer *B);
શૂન્ય (*dll_luaL_addlstring) (luaL_Buffer *B, અચલ અક્ષર *s, size_t l);
શૂન્ય (*dll_luaL_pushresult) (luaL_Buffer *B);
પૂર્ણાંક (*dll_luaL_loadstring) (lua_State *L, અચલ અક્ષર *s);
પૂર્ણાંક (*dll_luaL_ref) (lua_State *L, પૂર્ણાંક idx);
#  જો LUA_VERSION_અંક <= 502
શૂન્ય (*dll_luaL_unref) (lua_State *L, પૂર્ણાંક idx, પૂર્ણાંક n);
#  નહીંતર
શૂન્ય (*dll_luaL_unref) (lua_State *L, પૂર્ણાંક idx, lua_Integer n);
#  endif
// lua
#  જો LUA_VERSION_અંક <= 501
lua_સંખ્યા (*dll_lua_tonumber) (lua_State *L, પૂર્ણાંક idx);
lua_Integer (*dll_lua_tointeger) (lua_State *L, પૂર્ણાંક idx);
શૂન્ય (*dll_lua_call) (lua_State *L, પૂર્ણાંક nargs, પૂર્ણાંક nresults);
પૂર્ણાંક (*dll_lua_pcall) (lua_State *L, પૂર્ણાંક nargs, પૂર્ણાંક nresults, પૂર્ણાંક errfunc);
#  નહીંતર
lua_સંખ્યા (*dll_lua_tonumberx) (lua_State *L, પૂર્ણાંક idx, પૂર્ણાંક *isnum);
lua_Integer (*dll_lua_tointegerx) (lua_State *L, પૂર્ણાંક idx, પૂર્ણાંક *isnum);
શૂન્ય (*dll_lua_callk) (lua_State *L, પૂર્ણાંક nargs, પૂર્ણાંક nresults, પૂર્ણાંક ctx,
	lua_CFunction k);
પૂર્ણાંક (*dll_lua_pcallk) (lua_State *L, પૂર્ણાંક nargs, પૂર્ણાંક nresults, પૂર્ણાંક errfunc,
	પૂર્ણાંક ctx, lua_CFunction k);
શૂન્ય (*dll_lua_getglobal) (lua_State *L, અચલ અક્ષર *var);
શૂન્ય (*dll_lua_setglobal) (lua_State *L, અચલ અક્ષર *var);
#  endif
#  જો LUA_VERSION_અંક <= 502
શૂન્ય (*dll_lua_replace) (lua_State *L, પૂર્ણાંક idx);
શૂન્ય (*dll_lua_remove) (lua_State *L, પૂર્ણાંક idx);
#  endif
#  જો LUA_VERSION_અંક >= 503
શૂન્ય  (*dll_lua_rotate) (lua_State *L, પૂર્ણાંક idx, પૂર્ણાંક n);
શૂન્ય (*dll_lua_copy) (lua_State *L, પૂર્ણાંક fromidx, પૂર્ણાંક toidx);
#  endif
અચલ અક્ષર *(*dll_lua_typename) (lua_State *L, પૂર્ણાંક tp);
શૂન્ય       (*dll_lua_close) (lua_State *L);
પૂર્ણાંક (*dll_lua_gettop) (lua_State *L);
શૂન્ય (*dll_lua_settop) (lua_State *L, પૂર્ણાંક idx);
શૂન્ય (*dll_lua_pushvalue) (lua_State *L, પૂર્ણાંક idx);
પૂર્ણાંક (*dll_lua_isnumber) (lua_State *L, પૂર્ણાંક idx);
પૂર્ણાંક (*dll_lua_isstring) (lua_State *L, પૂર્ણાંક idx);
પૂર્ણાંક (*dll_lua_type) (lua_State *L, પૂર્ણાંક idx);
પૂર્ણાંક (*dll_lua_rawequal) (lua_State *L, પૂર્ણાંક idx1, પૂર્ણાંક idx2);
પૂર્ણાંક (*dll_lua_toboolean) (lua_State *L, પૂર્ણાંક idx);
અચલ અક્ષર *(*dll_lua_tolstring) (lua_State *L, પૂર્ણાંક idx, size_t *len);
શૂન્ય *(*dll_lua_touserdata) (lua_State *L, પૂર્ણાંક idx);
શૂન્ય (*dll_lua_pushnil) (lua_State *L);
શૂન્ય (*dll_lua_pushnumber) (lua_State *L, lua_સંખ્યા n);
શૂન્ય (*dll_lua_pushinteger) (lua_State *L, lua_Integer n);
શૂન્ય (*dll_lua_pushlstring) (lua_State *L, અચલ અક્ષર *s, size_t l);
શૂન્ય (*dll_lua_pushstring) (lua_State *L, અચલ અક્ષર *s);
અચલ અક્ષર *(*dll_lua_pushfstring) (lua_State *L, અચલ અક્ષર *fmt, ...);
શૂન્ય (*dll_lua_pushcclosure) (lua_State *L, lua_CFunction fn, પૂર્ણાંક n);
શૂન્ય (*dll_lua_pushboolean) (lua_State *L, પૂર્ણાંક b);
શૂન્ય (*dll_lua_pushlightuserdata) (lua_State *L, શૂન્ય *p);
શૂન્ય (*dll_lua_getfield) (lua_State *L, પૂર્ણાંક idx, અચલ અક્ષર *k);
#  જો LUA_VERSION_અંક <= 502
શૂન્ય (*dll_lua_rawget) (lua_State *L, પૂર્ણાંક idx);
શૂન્ય (*dll_lua_rawgeti) (lua_State *L, પૂર્ણાંક idx, પૂર્ણાંક n);
#  નહીંતર
પૂર્ણાંક (*dll_lua_rawget) (lua_State *L, પૂર્ણાંક idx);
પૂર્ણાંક (*dll_lua_rawgeti) (lua_State *L, પૂર્ણાંક idx, lua_Integer n);
#  endif
શૂન્ય (*dll_lua_createtable) (lua_State *L, પૂર્ણાંક narr, પૂર્ણાંક nrec);
શૂન્ય (*dll_lua_settable) (lua_State *L, પૂર્ણાંક idx);
#  જો LUA_VERSION_અંક >= 504
શૂન્ય *(*dll_lua_newuserdatauv) (lua_State *L, size_t sz, પૂર્ણાંક nuvalue);
#  નહીંતર
શૂન્ય *(*dll_lua_newuserdata) (lua_State *L, size_t sz);
#  endif
પૂર્ણાંક (*dll_lua_getmetatable) (lua_State *L, પૂર્ણાંક objindex);
શૂન્ય (*dll_lua_setfield) (lua_State *L, પૂર્ણાંક idx, અચલ અક્ષર *k);
શૂન્ય (*dll_lua_rawset) (lua_State *L, પૂર્ણાંક idx);
#  જો LUA_VERSION_અંક <= 502
શૂન્ય (*dll_lua_rawseti) (lua_State *L, પૂર્ણાંક idx, પૂર્ણાંક n);
#  નહીંતર
શૂન્ય (*dll_lua_rawseti) (lua_State *L, પૂર્ણાંક idx, lua_Integer n);
#  endif
પૂર્ણાંક (*dll_lua_setmetatable) (lua_State *L, પૂર્ણાંક objindex);
પૂર્ણાંક (*dll_lua_next) (lua_State *L, પૂર્ણાંક idx);
// libs
પૂર્ણાંક (*dll_luaopen_base) (lua_State *L);
પૂર્ણાંક (*dll_luaopen_table) (lua_State *L);
પૂર્ણાંક (*dll_luaopen_string) (lua_State *L);
પૂર્ણાંક (*dll_luaopen_math) (lua_State *L);
પૂર્ણાંક (*dll_luaopen_io) (lua_State *L);
પૂર્ણાંક (*dll_luaopen_os) (lua_State *L);
પૂર્ણાંક (*dll_luaopen_package) (lua_State *L);
પૂર્ણાંક (*dll_luaopen_debug) (lua_State *L);
#  જો LUA_VERSION_અંક >= 505
શૂન્ય (*dll_luaL_openselectedlibs) (lua_State *L, પૂર્ણાંક load, પૂર્ણાંક preload);
#  નહીંતર
શૂન્ય (*dll_luaL_openlibs) (lua_State *L);
#  endif

પ્રકારવ્યાખ્યા શૂન્ય **luaV_function;
પ્રકારવ્યાખ્યા સંરચના {
    અચલ અક્ષર *name;
    luaV_function func;
} luaV_Reg;

સ્થિર અચલ luaV_Reg luaV_dll[] = {
    // lauxlib
#  જો LUA_VERSION_અંક <= 501
    {"luaL_register", (luaV_function) &dll_luaL_register},
    {"luaL_prepbuffer", (luaV_function) &dll_luaL_prepbuffer},
    {"luaL_openlib", (luaV_function) &dll_luaL_openlib},
    {"luaL_typerror", (luaV_function) &dll_luaL_typerror},
    {"luaL_loadfile", (luaV_function) &dll_luaL_loadfile},
    {"luaL_loadbuffer", (luaV_function) &dll_luaL_loadbuffer},
#  નહીંતર
    {"luaL_prepbuffsize", (luaV_function) &dll_luaL_prepbuffsize},
    {"luaL_setfuncs", (luaV_function) &dll_luaL_setfuncs},
    {"luaL_loadfilex", (luaV_function) &dll_luaL_loadfilex},
    {"luaL_loadbufferx", (luaV_function) &dll_luaL_loadbufferx},
    {"luaL_argerror", (luaV_function) &dll_luaL_argerror},
#  endif
#  જો LUA_VERSION_અંક >= 504
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
#  જો LUA_VERSION_અંક <= 501
    {"lua_tonumber", (luaV_function) &dll_lua_tonumber},
    {"lua_tointeger", (luaV_function) &dll_lua_tointeger},
    {"lua_call", (luaV_function) &dll_lua_call},
    {"lua_pcall", (luaV_function) &dll_lua_pcall},
#  નહીંતર
    {"lua_tonumberx", (luaV_function) &dll_lua_tonumberx},
    {"lua_tointegerx", (luaV_function) &dll_lua_tointegerx},
    {"lua_callk", (luaV_function) &dll_lua_callk},
    {"lua_pcallk", (luaV_function) &dll_lua_pcallk},
    {"lua_getglobal", (luaV_function) &dll_lua_getglobal},
    {"lua_setglobal", (luaV_function) &dll_lua_setglobal},
#  endif
#  જો LUA_VERSION_અંક <= 502
    {"lua_replace", (luaV_function) &dll_lua_replace},
    {"lua_remove", (luaV_function) &dll_lua_remove},
#  endif
#  જો LUA_VERSION_અંક >= 503
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
#  જો LUA_VERSION_અંક >= 504
    {"lua_newuserdatauv", (luaV_function) &dll_lua_newuserdatauv},
#  નહીંતર
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
#  જો LUA_VERSION_અંક >= 505
    {"luaL_openselectedlibs", (luaV_function) &dll_luaL_openselectedlibs},
#  નહીંતર
    {"luaL_openlibs", (luaV_function) &dll_luaL_openlibs},
#  endif
    {ખાલી, ખાલી}
};

સ્થિર HANDLE hinstLua = ખાલી;

    સ્થિર પૂર્ણાંક
lua_link_init(અક્ષર *libname, પૂર્ણાંક verbose)
{
    અચલ luaV_Reg *reg;
    જો (hinstLua) પાછા OK;
    hinstLua = load_dll(libname);
    જો (!hinstLua)
    {
	જો (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	પાછા FAIL;
    }
    માટે (reg = luaV_dll; reg->func; reg++)
    {
	જો ((*reg->func = પ્રતીક_from_dll(hinstLua, reg->name)) == ખાલી)
	{
	    close_dll(hinstLua);
	    hinstLua = 0;
	    જો (verbose)
		semsg(_(e_could_not_load_library_function_str), reg->name);
	    પાછા FAIL;
	}
    }
    પાછા OK;
}
# endif // DYNAMIC_LUA

# જો defined(DYNAMIC_LUA)
    પૂર્ણાંક
lua_enabled(પૂર્ણાંક verbose)
{
    પાછા lua_link_init((અક્ષર *)p_luadll, verbose) == OK;
}
# endif

# જો LUA_VERSION_અંક > 501 && LUA_VERSION_અંક < 504
    સ્થિર પૂર્ણાંક
luaL_typeerror(lua_State *L, પૂર્ણાંક narg, અચલ અક્ષર *tname)
{
    અચલ અક્ષર *સંદેશ = lua_pushfstring(L, "%s expected, got %s",
	    tname, luaL_typename(L, narg));
    પાછા luaL_argerror(L, narg, સંદેશ);
}
# endif

    સ્થિર LUAV_INLINE શૂન્ય
luaV_getudata(lua_State *L, શૂન્ય *v)
{
    lua_pushlightuserdata(L, (શૂન્ય *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // now the cache table is at the top of the stack
    lua_pushlightuserdata(L, v);
    lua_rawget(L, -2);
    lua_remove(L, -2);  // remove the cache table from the stack
}

    સ્થિર LUAV_INLINE શૂન્ય
luaV_setudata(lua_State *L, શૂન્ય *v)
{
    lua_pushlightuserdata(L, (શૂન્ય *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // cache table is at -1
    lua_pushlightuserdata(L, v);       // ...now at -2
    lua_pushvalue(L, -3);	       // copy the userdata (cache at -3)
    lua_rawset(L, -3);		       // consumes two stack items
    lua_કાઢો(L, 1);		       // and remove the cache table
}

// =======   Internal   =======

    સ્થિર શૂન્ય
luaV_newmetatable(lua_State *L, અચલ અક્ષર *tname)
{
    lua_newtable(L);
    lua_pushlightuserdata(L, (શૂન્ય *) tname);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
}

    સ્થિર શૂન્ય *
luaV_toudata(lua_State *L, પૂર્ણાંક ud, અચલ અક્ષર *tname)
{
    શૂન્ય *p = lua_touserdata(L, ud);

    જો (p == ખાલી)
	પાછા ખાલી;

    // value is userdata
    જો (lua_getmetatable(L, ud)) // does it have a metatable?
    {
	luaV_getfield(L, tname); // get metatable
	જો (lua_rawequal(L, -1, -2)) // MTs match?
	{
	    lua_કાઢો(L, 2); // MTs
	    પાછા p;
	}
    }

    પાછા ખાલી;
}

    સ્થિર શૂન્ય *
luaV_checkcache(lua_State *L, શૂન્ય *p)
{
    luaV_getudata(L, p);
    જો (lua_isnil(L, -1)) luaL_error(L, "invalid object");
    lua_કાઢો(L, 1);
    પાછા p;
}

# define luaV_unbox(L,luatyp,ud) (*((luatyp *) lua_touserdata((L),(ud))))

# define luaV_checkvalid(L,luatyp,ud) \
    luaV_checkcache((L), (શૂન્ય *) luaV_unbox((L),luatyp,(ud)))

    સ્થિર શૂન્ય *
luaV_checkudata(lua_State *L, પૂર્ણાંક ud, અચલ અક્ષર *tname)
{
    શૂન્ય *p = luaV_toudata(L, ud, tname);
    જો (p == ખાલી) luaL_typeerror(L, ud, tname);
    પાછા p;
}

    સ્થિર શૂન્ય
luaV_pushtypval(lua_State *L, typval_T *tv)
{
    જો (tv == ખાલી)
    {
	lua_pushnil(L);
	પાછા;
    }
    પસંદગી (tv->v_type)
    {
	સ્થિતિ VAR_STRING:
	    lua_pushstring(L, tv->vval.v_string == ખાલી
					    ? "" : (અક્ષર *)tv->vval.v_string);
	    તોડો;
	સ્થિતિ VAR_સંખ્યા:
	    lua_pushinteger(L, (પૂર્ણાંક) tv->vval.v_સંખ્યા);
	    તોડો;
	સ્થિતિ VAR_FLOAT:
	    lua_pushnumber(L, (lua_સંખ્યા) tv->vval.v_float);
	    તોડો;
	સ્થિતિ VAR_LIST:
	    luaV_pushlist(L, tv->vval.v_list);
	    તોડો;
	સ્થિતિ VAR_DICT:
	    luaV_pushdict(L, tv->vval.v_dict);
	    તોડો;
	સ્થિતિ VAR_BOOL:
	સ્થિતિ VAR_SPECIAL:
	    જો (tv->vval.v_સંખ્યા <= VVAL_TRUE)
		lua_pushboolean(L, (પૂર્ણાંક) tv->vval.v_સંખ્યા);
	    નહીંતર
		lua_pushnil(L);
	    તોડો;
	સ્થિતિ VAR_FUNC:
	    luaV_pushfuncref(L, tv->vval.v_string);
	    તોડો;
	સ્થિતિ VAR_PARTIAL:
	    // TODO: handle partial arguments
	    luaV_pushfuncref(L, partial_name(tv->vval.v_partial));
	    તોડો;

	સ્થિતિ VAR_BLOB:
	    luaV_pushblob(L, tv->vval.v_blob);
	    તોડો;
	મૂળભૂત:
	    lua_pushnil(L);
    }
}

/*
 * Converts lua value at 'pos' to typval 'tv'.
 * Returns OK or FAIL.
 */
    સ્થિર પૂર્ણાંક
luaV_totypval(lua_State *L, પૂર્ણાંક pos, typval_T *tv)
{
    પૂર્ણાંક status = OK;

    tv->v_lock = 0;

    પસંદગી (lua_type(L, pos))
    {
	સ્થિતિ LUA_TBOOLEAN:
	    tv->v_type = VAR_BOOL;
	    tv->vval.v_સંખ્યા = (varnumber_T) lua_toboolean(L, pos);
	    તોડો;
	સ્થિતિ LUA_TNIL:
	    tv->v_type = VAR_SPECIAL;
	    tv->vval.v_સંખ્યા = VVAL_NULL;
	    તોડો;
	સ્થિતિ LUA_TSTRING:
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strsave((char_u *) lua_tostring(L, pos));
	    તોડો;
	સ્થિતિ LUA_TNUMBER:
	{
	    અચલ lua_સંખ્યા n = lua_tonumber(L, pos);

	    જો (n > (lua_સંખ્યા)INT64_MAX || n < (lua_સંખ્યા)INT64_MIN
		    || ((lua_સંખ્યા)((varnumber_T)n)) != n)
	    {
		tv->v_type = VAR_FLOAT;
		tv->vval.v_float = (float_T)n;
	    }
	    નહીંતર
	    {
		tv->v_type = VAR_સંખ્યા;
		tv->vval.v_સંખ્યા = (varnumber_T)n;
	    }
	}
	    તોડો;
	સ્થિતિ LUA_TFUNCTION:
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
	    તોડો;
	}
	સ્થિતિ LUA_TTABLE:
	{
	    પૂર્ણાંક lua_tableref;

	    lua_pushvalue(L, pos);
	    lua_tableref = luaL_ref(L, LUA_REGISTRYINDEX);
	    જો (lua_getmetatable(L, pos))
	    {
		lua_getfield(L, -1, LUA___CALL);
		જો (lua_isfunction(L, -1))
		{
		    char_u *name;
		    પૂર્ણાંક lua_funcref = luaL_ref(L, LUA_REGISTRYINDEX);
		    luaV_CFuncState *state = ALLOC_CLEAR_ONE(luaV_CFuncState);

		    state->lua_funcref = lua_funcref;
		    state->L = L;
		    state->lua_tableref = lua_tableref;
		    name = register_cfunc(&luaV_call_lua_func,
					      &luaV_call_lua_func_free, state);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(name);
		    તોડો;
		}
	    }
	    tv->v_type = VAR_સંખ્યા;
	    tv->vval.v_સંખ્યા = 0;
	    status = FAIL;
	    તોડો;
	}
	સ્થિતિ LUA_TUSERDATA:
	{
	    શૂન્ય *p = lua_touserdata(L, pos);

	    જો (lua_getmetatable(L, pos)) // has metatable?
	    {
		// check list
		luaV_getfield(L, LUAVIM_LIST);
		જો (lua_rawequal(L, -1, -2))
		{
		    tv->v_type = VAR_LIST;
		    tv->vval.v_list = *((luaV_List *) p);
		    ++tv->vval.v_list->lv_refcount;
		    lua_કાઢો(L, 2); // MTs
		    તોડો;
		}
		// check dict
		luaV_getfield(L, LUAVIM_DICT);
		જો (lua_rawequal(L, -1, -3))
		{
		    tv->v_type = VAR_DICT;
		    tv->vval.v_dict = *((luaV_Dict *) p);
		    ++tv->vval.v_dict->dv_refcount;
		    lua_કાઢો(L, 3); // MTs
		    તોડો;
		}
		// check blob
		luaV_getfield(L, LUAVIM_BLOB);
		જો (lua_rawequal(L, -1, -4))
		{
		    tv->v_type = VAR_BLOB;
		    tv->vval.v_blob = *((luaV_Blob *) p);
		    ++tv->vval.v_blob->bv_refcount;
		    lua_કાઢો(L, 4); // MTs
		    તોડો;
		}
		// check funcref
		luaV_getfield(L, LUAVIM_FUNCREF);
		જો (lua_rawequal(L, -1, -5))
		{
		    luaV_Funcref *f = (luaV_Funcref *) p;

		    func_ref(f->name);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(f->name);
		    lua_કાઢો(L, 5); // MTs
		    તોડો;
		}
		lua_કાઢો(L, 4); // MTs
	    }
	}
	// FALLTHROUGH
	મૂળભૂત:
	    tv->v_type = VAR_સંખ્યા;
	    tv->vval.v_સંખ્યા = 0;
	    status = FAIL;
    }
    પાછા status;
}

/*
 * similar to luaL_addlstring, but replaces \0 with \n if toline and
 * \n with \0 otherwise
 */
    સ્થિર શૂન્ય
luaV_addlstring(luaL_Buffer *b, અચલ અક્ષર *s, size_t l, પૂર્ણાંક toline)
{
    જ્યારે (l--)
    {
	જો (*s == '\0' && toline)
	    luaL_addchar(b, '\n');
	નહીંતર જો (*s == '\n' && !toline)
	    luaL_addchar(b, '\0');
	નહીંતર
	    luaL_addchar(b, *s);
	s++;
    }
}

    સ્થિર શૂન્ય
luaV_pushline(lua_State *L, buf_T *buf, linenr_T n)
{
    અચલ અક્ષર *s = (અચલ અક્ષર *) ml_get_buf(buf, n, FALSE);
    luaL_Buffer b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, strlen(s), 0);
    luaL_pushresult(&b);
}

    સ્થિર char_u *
luaV_toline(lua_State *L, પૂર્ણાંક pos)
{
    size_t l;
    અચલ અક્ષર *s = lua_tolstring(L, pos, &l);

    luaL_Buffer b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 1);
    luaL_pushresult(&b);
    પાછા (char_u *) lua_tostring(L, -1);
}

/*
 * pops a string s from the top of the stack and calls mf(t) for pieces t of
 * s separated by newlines
 */
    સ્થિર શૂન્ય
luaV_msgfunc(lua_State *L, msgfunc_T mf)
{
    luaL_Buffer b;
    size_t l;
    અચલ અક્ષર *p, *s = lua_tolstring(L, -1, &l);
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 0);
    luaL_pushresult(&b);
    // break string
    p = s = lua_tolstring(L, -1, &l);
    જ્યારે (l--)
    {
	જો (*p++ == '\0') // break?
	{
	    mf((અક્ષર *)s);
	    s = p;
	}
    }
    mf((અક્ષર *)s);
    lua_કાઢો(L, 2); // original and modified strings
}

# define luaV_newtype(typ,tname,luatyp,luatname) \
	સ્થિર luatyp * \
    luaV_new##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = (luatyp *) lua_newuserdata(L, કદ(luatyp)); \
	*o = obj; \
	luaV_setudata(L, obj); /* cache[obj] = udata */ \
	luaV_getfield(L, luatname); \
	lua_setmetatable(L, -2); \
	પાછા o; \
    }

# define luaV_pushtype(typ,tname,luatyp) \
	સ્થિર luatyp * \
    lua_V_ધકેલો##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = ખાલી; \
	જો (obj == ખાલી) \
	    lua_pushnil(L); \
	નહીંતર \
	{ \
	    luaV_getudata(L, obj); \
	    જો (lua_isnil(L, -1)) /* not interned? */ \
	    { \
		lua_કાઢો(L, 1); \
		o = luaV_new##tname(L, obj); \
	    } \
	    નહીંતર \
		o = (luatyp *) lua_touserdata(L, -1); \
	} \
	પાછા o; \
    }

# define luaV_type_tostring(tname,luatname) \
	સ્થિર પૂર્ણાંક \
    luaV_##tname##_tostring(lua_State *L) \
    { \
	lua_pushfstring(L, "%s: %p", luatname, lua_touserdata(L, 1)); \
	પાછા 1; \
    }

// =======   List type   =======

    સ્થિર luaV_List *
luaV_newlist(lua_State *L, list_T *lis)
{
    luaV_List *l = (luaV_List *) lua_newuserdata(L, કદ(luaV_List));
    *l = lis;
    lis->lv_refcount++; // reference in Lua
    luaV_setudata(L, lis); // cache[lis] = udata
    luaV_getfield(L, LUAVIM_LIST);
    lua_setmetatable(L, -2);
    પાછા l;
}

luaV_pushtype(list_T, list, luaV_List)
luaV_type_tostring(list, LUAVIM_LIST)

    સ્થિર પૂર્ણાંક
luaV_list_len(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushinteger(L, (પૂર્ણાંક) list_len(l));
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_list_iter(lua_State *L)
{
    listitem_T *li = (listitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    જો (li == ખાલી) પાછા 0;
    luaV_pushtypval(L, &li->li_tv);
    lua_pushlightuserdata(L, (શૂન્ય *) li->li_next);
    lua_replace(L, lua_upvalueindex(1));
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_list_call(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushlightuserdata(L, (શૂન્ય *) l->lv_first);
    lua_pushcclosure(L, luaV_list_iter, 1);
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_list_index(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    જો (lua_isnumber(L, 2)) // list item?
    {
	long n = (long) luaL_checkinteger(L, 2);
	listitem_T *li;

	// Lua array index starts with 1 while Vim uses 0, subtract 1 to
	// normalize.
	n -= 1;
	li = list_find(l, n);
	જો (li == ખાલી)
	    lua_pushnil(L);
	નહીંતર
	    luaV_pushtypval(L, &li->li_tv);
    }
    નહીંતર જો (lua_isstring(L, 2)) // method?
    {
	અચલ અક્ષર *s = lua_tostring(L, 2);
	જો (strncmp(s, "add", 3) == 0
		|| strncmp(s, "insert", 6) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	નહીંતર
	    lua_pushnil(L);
    }
    નહીંતર
	lua_pushnil(L);
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_list_newindex(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    long n = (long) luaL_checkinteger(L, 2);
    listitem_T *li;

    // Lua array index starts with 1 while Vim uses 0, subtract 1 to normalize.
    n -= 1;

    જો (l->lv_lock)
	luaL_error(L, "list is locked");
    li = list_find(l, n);
    જો (li == ખાલી)
    {
	જો (!lua_isnil(L, 3))
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "inserting list item");
	    જો (list_ઉમેરો_tv(l, &v, li) == FAIL)
		luaL_error(L, "failed to add item to list");
	    clear_tv(&v);
	}
    }
    નહીંતર
    {
	જો (lua_isnil(L, 3)) // remove?
	{
	    vimlist_remove(l, li, li);
	    listitem_free(l, li);
	}
	નહીંતર
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "setting list item");
	    clear_tv(&li->li_tv);
	    li->li_tv = v;
	}
    }
    પાછા 0;
}

    સ્થિર પૂર્ણાંક
luaV_list_add(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (શૂન્ય *) *lis);
    typval_T v;
    જો (l->lv_lock)
	luaL_error(L, "list is locked");
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "adding list item");
    જો (list_append_tv(l, &v) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
lua_V_list_ઉમેરો(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (શૂન્ય *) *lis);
    long pos = (long) luaL_optinteger(L, 3, 0);
    listitem_T *li = ખાલી;
    typval_T v;
    જો (l->lv_lock)
	luaL_error(L, "list is locked");
    જો (pos < l->lv_len)
    {
	li = list_find(l, pos);
	જો (li == ખાલી)
	    luaL_error(L, "invalid position");
    }
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "inserting list item");
    જો (list_ઉમેરો_tv(l, &v, li) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    પાછા 1;
}

સ્થિર અચલ luaL_Reg luaV_List_mt[] = {
    {"__tostring", luaV_list_tostring},
    {"__len", luaV_list_len},
    {"__call", luaV_list_call},
    {"__index", luaV_list_index},
    {"__newindex", luaV_list_newindex},
    {"add", luaV_list_add},
    {"insert", lua_V_list_ઉમેરો},
    {ખાલી, ખાલી}
};


// =======   Dict type   =======

    સ્થિર luaV_Dict *
luaV_newdict(lua_State *L, dict_T *dic)
{
    luaV_Dict *d = (luaV_Dict *) lua_newuserdata(L, કદ(luaV_Dict));
    *d = dic;
    dic->dv_refcount++; // reference in Lua
    luaV_setudata(L, dic); // cache[dic] = udata
    luaV_getfield(L, LUAVIM_DICT);
    lua_setmetatable(L, -2);
    પાછા d;
}

luaV_pushtype(dict_T, dict, luaV_Dict)
luaV_type_tostring(dict, LUAVIM_DICT)

    સ્થિર પૂર્ણાંક
luaV_dict_len(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    lua_pushinteger(L, (પૂર્ણાંક) dict_len(d));
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_dict_iter(lua_State *L UNUSED)
{
# ifdef FEAT_EVAL
    hashitem_T *hi = (hashitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    પૂર્ણાંક n = lua_tointeger(L, lua_upvalueindex(2));
    dictitem_T *di;
    જો (n <= 0) પાછા 0;
    જ્યારે (HASHITEM_EMPTY(hi)) hi++;
    di = dict_lookup(hi);
    lua_pushstring(L, (અક્ષર *) hi->hi_ચાવી);
    luaV_pushtypval(L, &di->di_tv);
    lua_pushlightuserdata(L, (શૂન્ય *) (hi + 1));
    lua_replace(L, lua_upvalueindex(1));
    lua_pushinteger(L, n - 1);
    lua_replace(L, lua_upvalueindex(2));
    પાછા 2;
# નહીંતર
    પાછા 0;
# endif
}

    સ્થિર પૂર્ણાંક
luaV_dict_call(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    hashtab_T *ht = &d->dv_hashtab;
    lua_pushlightuserdata(L, (શૂન્ય *) ht->ht_array);
    lua_pushinteger(L, ht->ht_used); // # remaining items
    lua_pushcclosure(L, luaV_dict_iter, 2);
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_dict_index(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *ચાવી = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di = dict_find(d, ચાવી, -1);

    જો (di == ખાલી)
    {
	lua_pushnil(L);
	પાછા 1;
    }

    luaV_pushtypval(L, &di->di_tv);
    જો (di->di_tv.v_type == VAR_FUNC) // funcref?
    {
	luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, -1);
	f->self = d; // keep "self" reference
	d->dv_refcount++;
    }

    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_dict_newindex(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *ચાવી = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di;
    typval_T tv;

    જો (d->dv_lock)
	luaL_error(L, "dict is locked");
    જો (ચાવી == ખાલી)
	પાછા 0;
    જો (*ચાવી == NUL)
	luaL_error(L, "empty key");
    જો (!lua_isnil(L, 3)) // read value?
    {
	luaV_checktypval(L, 3, &tv, "setting dict item");
	જો (d->dv_scope == VAR_DEF_SCOPE && tv.v_type == VAR_FUNC)
	{
	    clear_tv(&tv);
	    luaL_error(L, "cannot assign funcref to builtin scope");
	}
    }
    di = dict_find(d, ચાવી, -1);
    જો (di == ખાલી) // non-existing key?
    {
	જો (lua_isnil(L, 3))
	    પાછા 0;
	di = dictitem_alloc(ચાવી);
	જો (di == ખાલી)
	{
	    clear_tv(&tv);
	    પાછા 0;
	}
	જો (dict_add(d, di) == FAIL)
	{
	    vim_free(di);
	    clear_tv(&tv);
	    પાછા 0;
	}
    }
    નહીંતર
	clear_tv(&di->di_tv);
    જો (lua_isnil(L, 3)) // remove?
    {
	hashitem_T *hi = hash_find(&d->dv_hashtab, di->di_ચાવી);
	hash_remove(&d->dv_hashtab, hi, "Lua new index");
	dictitem_free(di);
    }
    નહીંતર
	di->di_tv = tv;
    પાછા 0;
}

સ્થિર અચલ luaL_Reg luaV_Dict_mt[] = {
    {"__tostring", luaV_dict_tostring},
    {"__len", luaV_dict_len},
    {"__call", luaV_dict_call},
    {"__index", luaV_dict_index},
    {"__newindex", luaV_dict_newindex},
    {ખાલી, ખાલી}
};


// =======   Blob type   =======

    સ્થિર luaV_Blob *
luaV_newblob(lua_State *L, blob_T *blo)
{
    luaV_Blob *b = (luaV_Blob *) lua_newuserdata(L, કદ(luaV_Blob));
    *b = blo;
    blo->bv_refcount++; // reference in Lua
    luaV_setudata(L, blo); // cache[blo] = udata
    luaV_getfield(L, LUAVIM_BLOB);
    lua_setmetatable(L, -2);
    પાછા b;
}

luaV_pushtype(blob_T, blob, luaV_Blob)
luaV_type_tostring(blob, LUAVIM_BLOB)

    સ્થિર પૂર્ણાંક
luaV_blob_gc(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    blob_unref(b);
    પાછા 0;
}

    સ્થિર પૂર્ણાંક
luaV_blob_len(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    lua_pushinteger(L, (પૂર્ણાંક) blob_len(b));
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_blob_index(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    જો (lua_isnumber(L, 2))
    {
	પૂર્ણાંક idx = luaL_checkinteger(L, 2);
	જો (idx < 0)
	    // Negative index is relative to the end.
	    idx += (પૂર્ણાંક) blob_len(b);
	જો (idx >= 0 && idx < blob_len(b))
	    lua_pushnumber(L, (lua_સંખ્યા) blob_get(b, idx));
	નહીંતર
	    lua_pushnil(L);
    }
    નહીંતર જો (lua_isstring(L, 2))
    {
	અચલ અક્ષર *s = lua_tostring(L, 2);
	જો (strncmp(s, "add", 3) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	નહીંતર
	    lua_pushnil(L);
    }
    નહીંતર
	lua_pushnil(L);
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_blob_newindex(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    જો (b->bv_lock)
	luaL_error(L, "blob is locked");

    જો (!lua_isnumber(L, 2))
	પાછા 0;

    long len = blob_len(b);
    પૂર્ણાંક idx = luaL_checkinteger(L, 2);
    પૂર્ણાંક val = luaL_checkinteger(L, 3);
    જો (idx < 0)
	// Negative index is relative to the end.
	idx += (પૂર્ણાંક) len;
    જો ((idx >= 0 && idx < len)
	    || (idx == len && ga_grow(&b->bv_ga, 1) == OK))
    {
	blob_set(b, idx, (char_u) val);
	જો (idx == len)
	    ++b->bv_ga.ga_len;
    }
    નહીંતર
	luaL_error(L, "index out of range");

    પાછા 0;
}

    સ્થિર પૂર્ણાંક
luaV_blob_add(lua_State *L)
{
    luaV_Blob *blo = luaV_checkudata(L, 1, LUAVIM_BLOB);
    blob_T *b = (blob_T *) luaV_checkcache(L, (શૂન્ય *) *blo);
    જો (b->bv_lock)
	luaL_error(L, "blob is locked");
    lua_settop(L, 2);
    જો (!lua_isstring(L, 2))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 2));
    નહીંતર
    {
	size_t i, l = 0;
	અચલ અક્ષર *s = lua_tolstring(L, 2, &l);

	જો (ga_grow(&b->bv_ga, (પૂર્ણાંક)l) == OK)
	    માટે (i = 0; i < l; ++i)
		ga_append(&b->bv_ga, s[i]);
    }
    lua_settop(L, 1);
    પાછા 1;
}

સ્થિર અચલ luaL_Reg luaV_Blob_mt[] = {
    {"__tostring", luaV_blob_tostring},
    {"__gc", luaV_blob_gc},
    {"__len", luaV_blob_len},
    {"__index", luaV_blob_index},
    {"__newindex", luaV_blob_newindex},
    {"add", luaV_blob_add},
    {ખાલી, ખાલી}
};


// =======   Funcref type   =======

    સ્થિર luaV_Funcref *
luaV_newfuncref(lua_State *L, char_u *name)
{
    luaV_Funcref *f = (luaV_Funcref *)lua_newuserdata(L, કદ(luaV_Funcref));

    જો (name != ખાલી)
    {
	func_ref(name);
	f->name = vim_strsave(name);
    }
    f->self = ખાલી;
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_setmetatable(L, -2);
    પાછા f;
}

    સ્થિર luaV_Funcref *
luaV_pushfuncref(lua_State *L, char_u *name)
{
    પાછા luaV_newfuncref(L, name);
}


luaV_type_tostring(funcref, LUAVIM_FUNCREF)

    સ્થિર પૂર્ણાંક
luaV_funcref_gc(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    func_unref(f->name);
    vim_free(f->name);
    // NOTE: Don't call "dict_unref(f->self)", because the dict of "f->self"
    // will be (or has been already) freed by Vim's garbage collection.
    પાછા 0;
}

// equivalent to string(funcref)
    સ્થિર પૂર્ણાંક
luaV_funcref_len(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    lua_pushstring(L, (અચલ અક્ષર *) f->name);
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_funcref_call(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);
    પૂર્ણાંક i, n = lua_gettop(L) - 1; // #args
    પૂર્ણાંક status = FAIL;
    typval_T args;
    typval_T rettv;

    args.v_type = VAR_LIST;
    args.vval.v_list = list_alloc();
    rettv.v_type = VAR_UNKNOWN; // as in clear_tv
    જો (args.vval.v_list != ખાલી)
    {
	typval_T v;

	માટે (i = 0; i < n; i++)
	{
	    luaV_checktypval(L, i + 2, &v, "calling funcref");
	    list_append_tv(args.vval.v_list, &v);
	    clear_tv(&v);
	}
	status = func_call(f->name, &args, ખાલી, f->self, &rettv);
	જો (status == OK)
	    luaV_pushtypval(L, &rettv);
	clear_tv(&args);
	clear_tv(&rettv);
    }
    જો (status != OK)
	luaL_error(L, "cannot call funcref");
    પાછા 1;
}

સ્થિર અચલ luaL_Reg luaV_Funcref_mt[] = {
    {"__tostring", luaV_funcref_tostring},
    {"__gc", luaV_funcref_gc},
    {"__len", luaV_funcref_len},
    {"__call", luaV_funcref_call},
    {ખાલી, ખાલી}
};


// =======   Buffer type   =======

luaV_newtype(buf_T, buffer, luaV_Buffer, LUAVIM_BUFFER)
luaV_pushtype(buf_T, buffer, luaV_Buffer)
luaV_type_tostring(buffer, LUAVIM_BUFFER)

    સ્થિર પૂર્ણાંક
luaV_buffer_len(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    lua_pushinteger(L, b->b_ml.ml_લીટી_ગણતરી);
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_buffer_call(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    lua_settop(L, 1);
    set_curbuf(b, DOBUF_SPLIT);
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_buffer_index(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    linenr_T n = (linenr_T) lua_tointeger(L, 2);
    જો (n > 0 && n <= b->b_ml.ml_લીટી_ગણતરી)
	luaV_pushline(L, b, n);
    નહીંતર જો (lua_isstring(L, 2))
    {
	અચલ અક્ષર *s = lua_tostring(L, 2);
	જો (strncmp(s, "name", 4) == 0)
	    lua_pushstring(L, (b->b_sfname == ખાલી)
					? "" : (અક્ષર *) b->b_sfname);
	નહીંતર જો (strncmp(s, "fname", 5) == 0)
	    lua_pushstring(L, (b->b_ffname == ખાલી)
					? "" : (અક્ષર *) b->b_ffname);
	નહીંતર જો (strncmp(s, "number", 6) == 0)
	    lua_pushinteger(L, b->b_fnum);
	// methods
	નહીંતર જો (strncmp(s,   "insert", 6) == 0
		|| strncmp(s, "next", 4) == 0
		|| strncmp(s, "previous", 8) == 0
		|| strncmp(s, "isvalid", 7) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	નહીંતર
	    lua_pushnil(L);
    }
    નહીંતર
	lua_pushnil(L);
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_buffer_newindex(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    linenr_T n = (linenr_T) luaL_checkinteger(L, 2);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    જો (n < 1 || n > b->b_ml.ml_લીટી_ગણતરી)
	luaL_error(L, "invalid line number");
    જો (lua_isnil(L, 3)) // delete line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	જો (u_savedel(n, 1L) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	નહીંતર જો (ml_delete(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot delete line");
	}
	નહીંતર
	{
	    deleted_લીટીઓ_mark(n, 1L);
	    જો (b == curwin->w_buffer) // fix cursor in current window?
	    {
		જો (curwin->w_cursor.lnum >= n)
		{
		    જો (curwin->w_cursor.lnum > n)
		    {
			curwin->w_cursor.lnum -= 1;
			check_cursor_col();
		    }
		    નહીંતર
			check_cursor();
		    changed_cline_bef_curs();
		}
		invalidate_botline();
	    }
	}
	curbuf = buf;
    }
    નહીંતર જો (lua_isstring(L, 3)) // update line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	જો (u_savesub(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	નહીંતર જો (ml_replace(n, luaV_toline(L, 3), TRUE) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot replace line");
	}
	નહીંતર
	    changed_bytes(n, 0);
	curbuf = buf;
	જો (b == curwin->w_buffer)
	    check_cursor_col();
    }
    નહીંતર
	luaL_error(L, "wrong argument to change line");
    પાછા 0;
}

    સ્થિર પૂર્ણાંક
lua_V_buffer_ઉમેરો(lua_State *L)
{
    luaV_Buffer *lb = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *b = (buf_T *) luaV_checkcache(L, (શૂન્ય *) *lb);
    linenr_T last = b->b_ml.ml_લીટી_ગણતરી;
    linenr_T n = (linenr_T) luaL_optinteger(L, 3, last);
    buf_T *buf;
    luaL_checktype(L, 2, LUA_TSTRING);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    // fix insertion line
    જો (n < 0) n = 0;
    જો (n > last) n = last;
    // insert
    buf = curbuf;
    curbuf = b;
    જો (u_save(n, n + 1) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot save undo information");
    }
    નહીંતર જો (ml_append(n, luaV_toline(L, 2), 0, FALSE) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot insert line");
    }
    નહીંતર
	appended_લીટીઓ_mark(n, 1L);
    curbuf = buf;
    update_screen(UPD_VALID);
    પાછા 0;
}

    સ્થિર પૂર્ણાંક
luaV_buffer_next(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (શૂન્ય *) *b);
    luaV_pushbuffer(L, buf->b_next);
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_buffer_previous(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (શૂન્ય *) *b);
    luaV_pushbuffer(L, buf->b_prev);
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_buffer_isvalid(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    luaV_getudata(L, *b);
    lua_pushboolean(L, !lua_isnil(L, -1));
    પાછા 1;
}

સ્થિર અચલ luaL_Reg luaV_Buffer_mt[] = {
    {"__tostring", luaV_buffer_tostring},
    {"__len", luaV_buffer_len},
    {"__call", luaV_buffer_call},
    {"__index", luaV_buffer_index},
    {"__newindex", luaV_buffer_newindex},
    {"insert", lua_V_buffer_ઉમેરો},
    {"next", luaV_buffer_next},
    {"previous", luaV_buffer_previous},
    {"isvalid", luaV_buffer_isvalid},
    {ખાલી, ખાલી}
};


// =======   Window type   =======

luaV_newtype(win_T, window, luaV_Window, LUAVIM_WINDOW)
luaV_pushtype(win_T, window, luaV_Window)
luaV_type_tostring(window, LUAVIM_WINDOW)

    સ્થિર પૂર્ણાંક
luaV_window_call(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    lua_settop(L, 1);
    win_goto(w);
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_window_index(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    અચલ અક્ષર *s = luaL_checkstring(L, 2);
    જો (strncmp(s, "buffer", 6) == 0)
	luaV_pushbuffer(L, w->w_buffer);
    નહીંતર જો (strncmp(s, "line", 4) == 0)
	lua_pushinteger(L, w->w_cursor.lnum);
    નહીંતર જો (strncmp(s, "col", 3) == 0)
	lua_pushinteger(L, w->w_cursor.col + 1);
    નહીંતર જો (strncmp(s, "width", 5) == 0)
	lua_pushinteger(L, w->w_width);
    નહીંતર જો (strncmp(s, "height", 6) == 0)
	lua_pushinteger(L, w->w_height);
    // methods
    નહીંતર જો (strncmp(s,   "next", 4) == 0
	    || strncmp(s, "previous", 8) == 0
	    || strncmp(s, "isvalid", 7) == 0)
    {
	lua_getmetatable(L, 1);
	lua_getfield(L, -1, s);
    }
    નહીંતર
	lua_pushnil(L);
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_window_newindex(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    અચલ અક્ષર *s = luaL_checkstring(L, 2);
    પૂર્ણાંક v = luaL_checkinteger(L, 3);
    જો (strncmp(s, "line", 4) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	જો (v < 1 || v > w->w_buffer->b_ml.ml_લીટી_ગણતરી)
	    luaL_error(L, "line out of range");
	w->w_cursor.lnum = v;
	update_screen(UPD_VALID);
    }
    નહીંતર જો (strncmp(s, "col", 3) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	w->w_cursor.col = v - 1;
	w->w_set_curswant = true;
	update_screen(UPD_VALID);
    }
    નહીંતર જો (strncmp(s, "width", 5) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setwidth(v);
	curwin = win;
    }
    નહીંતર જો (strncmp(s, "height", 6) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setheight(v);
	curwin = win;
    }
    નહીંતર
	luaL_error(L, "invalid window property: `%s'", s);
    પાછા 0;
}

    સ્થિર પૂર્ણાંક
luaV_window_next(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    win_T *win = (win_T *) luaV_checkcache(L, (શૂન્ય *) *w);
    luaV_pushwindow(L, win->w_next);
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_window_previous(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    win_T *win = (win_T *) luaV_checkcache(L, (શૂન્ય *) *w);
    luaV_pushwindow(L, win->w_prev);
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_window_isvalid(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    luaV_getudata(L, *w);
    lua_pushboolean(L, !lua_isnil(L, -1));
    પાછા 1;
}

સ્થિર અચલ luaL_Reg luaV_Window_mt[] = {
    {"__tostring", luaV_window_tostring},
    {"__call", luaV_window_call},
    {"__index", luaV_window_index},
    {"__newindex", luaV_window_newindex},
    {"next", luaV_window_next},
    {"previous", luaV_window_previous},
    {"isvalid", luaV_window_isvalid},
    {ખાલી, ખાલી}
};


// =======   Vim module   =======

    સ્થિર પૂર્ણાંક
luaV_print(lua_State *L)
{
    પૂર્ણાંક		i, n = lua_gettop(L); // nargs
    અચલ અક્ષર	*s;
    size_t	l;
    garray_T	સંદેશ_ga;

    ga_init2(&સંદેશ_ga, 1, 128);
    lua_getglobal(L, "tostring");
    માટે (i = 1; i <= n; i++)
    {
	lua_pushvalue(L, -1); // tostring
	lua_pushvalue(L, i); // arg
	lua_call(L, 1, 1);
	s = lua_tolstring(L, -1, &l);
	જો (s == ખાલી)
	    પાછા luaL_error(L, "cannot convert to string");
	જો (i > 1)
	    ga_append(&સંદેશ_ga, ' '); // use space instead of tab
	ga_concat_len(&સંદેશ_ga, (char_u *)s, l);
	lua_કાઢો(L, 1);
    }
    // Replace any "\n" with "\0"
    માટે (i = 0; i < સંદેશ_ga.ga_len; i++)
	જો (((અક્ષર *)સંદેશ_ga.ga_data)[i] == '\n')
	    ((અક્ષર *)સંદેશ_ga.ga_data)[i] = '\0';
    lua_pushlstring(L, સંદેશ_ga.ga_data, સંદેશ_ga.ga_len);
    જો (!got_int)
	lua_V_સંદેશ(L);

    ga_clear(&સંદેશ_ga);
    પાછા 0;
}

    સ્થિર પૂર્ણાંક
luaV_debug(lua_State *L)
{
    lua_settop(L, 0);
    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "eval");
    lua_remove(L, -2); // vim.eval at position 1
    માટે (;;)
    {
	અચલ અક્ષર *આગત;
	size_t l;
	lua_pushvalue(L, 1); // vim.eval
	lua_pushliteral(L, "input('lua_debug> ')");
	lua_call(L, 1, 1); // return string
	આગત = lua_tolstring(L, -1, &l);
	જો (l == 0 || strcmp(આગત, "cont") == 0)
	    પાછા 0;
	સંદેશ_putchar('\n'); // avoid outputting on input line
	જો (luaL_loadbuffer(L, આગત, l, "=(debug command)")
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
	lua_settop(L, 1); // remove eventual returns, but keep vim.eval
    }
}

    સ્થિર dict_T *
luaV_get_var_scope(lua_State *L)
{
    અચલ અક્ષર	*scope = luaL_checkstring(L, 1);
    dict_T	*dict = ખાલી;

    જો (STRICMP((અક્ષર *)scope, "g") == 0)
	dict = get_globvar_dict();
    નહીંતર જો (STRICMP((અક્ષર *)scope, "v") == 0)
	dict = get_vimvar_dict();
    નહીંતર જો (STRICMP((અક્ષર *)scope, "b") == 0)
	dict = curbuf->b_vars;
    નહીંતર જો (STRICMP((અક્ષર *)scope, "w") == 0)
	dict = curwin->w_vars;
    નહીંતર જો (STRICMP((અક્ષર *)scope, "t") == 0)
	dict = curtab->tp_vars;
    નહીંતર
    {
	luaL_error(L, "invalid scope %s", scope);
	પાછા ખાલી;
    }

    પાછા dict;
}

    સ્થિર પૂર્ણાંક
luaV_setvar(lua_State *L)
{
    dict_T	*dict;
    dictitem_T	*di;
    size_t	len;
    અક્ષર	*name;
    પૂર્ણાંક		del;
    અક્ષર	*error = ખાલી;

    name = (અક્ષર *)luaL_checklstring(L, 3, &len);
    del = (lua_gettop(L) < 4) || lua_isnil(L, 4);

    dict = luaV_get_var_scope(L);
    જો (dict == ખાલી)
	પાછા 0;

    di = dict_find(dict, (char_u *)name, (પૂર્ણાંક)len);
    જો (di != ખાલી)
    {
	જો (di->di_flags & DI_FLAGS_RO)
	    error = "variable is read-only";
	નહીંતર જો (di->di_flags & DI_FLAGS_LOCK)
	    error = "variable is locked";
	નહીંતર જો (del && di->di_flags & DI_FLAGS_FIX)
	    error = "variable is fixed";
	જો (error != ખાલી)
	    પાછા luaL_error(L, error);
    }
    નહીંતર જો (dict->dv_lock)
	પાછા luaL_error(L, "Dictionary is locked");

    જો (del)
    {
	// Delete the key
	જો (di == ખાલી)
	    // Doesn't exist, nothing to do
	    પાછા 0;
	// Delete the entry
	dictitem_remove(dict, di, "Lua delete variable");
    }
    નહીંતર
    {
	// Update the key
	typval_T	tv;

	// Convert the lua value to a Vim script type in the temporary variable
	lua_pushvalue(L, 4);
	જો (luaV_totypval(L, -1, &tv) == FAIL)
	    પાછા luaL_error(L, "Couldn't convert lua value");

	જો (di == ખાલી)
	{
	    // Need to create an entry
	    di = dictitem_alloc((char_u *)name);
	    જો (di == ખાલી)
	    {
		clear_tv(&tv);
		પાછા 0;
	    }
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	    જો (dict_add(dict, di) == FAIL)
	    {
		dictitem_free(di);
		clear_tv(&tv);
		પાછા luaL_error(L, "Couldn't add to dictionary");
	    }
	}
	નહીંતર
	{
	    પૂર્ણાંક type_error = FALSE;
	    જો (dict == get_vimvar_dict()
	       && !before_set_vvar((char_u *)name, di, &tv, TRUE, &type_error))
	    {
		clear_tv(&tv);
		જો (type_error)
		    પાછા luaL_error(L,
				"Setting v:%s to value with wrong type", name);
		પાછા 0;
	    }
	    // Clear the old value
	    clear_tv(&di->di_tv);
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	}

	// Clear the temporary variable
	clear_tv(&tv);
    }

    પાછા 0;
}

    સ્થિર પૂર્ણાંક
luaV_getvar(lua_State *L)
{
    dict_T	*dict = luaV_get_var_scope(L);
    size_t	len;
    અચલ અક્ષર	*name = luaL_checklstring(L, 3, &len);
    dictitem_T	*di = dict_find(dict, (char_u *)name, (પૂર્ણાંક)len);

    જો (di == ખાલી)
	પાછા 0;  // nil

    luaV_pushtypval(L, &di->di_tv);
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_command(lua_State *L)
{
    char_u  *s = vim_strsave((char_u *)luaL_checkstring(L, 1));

    execute_cmds_from_string(s);
    vim_free(s);
    update_screen(UPD_VALID);
    પાછા 0;
}

    સ્થિર પૂર્ણાંક
luaV_eval(lua_State *L)
{
    typval_T *tv = eval_પદાવલિ((char_u *) luaL_checkstring(L, 1), ખાલી);

    જો (tv == ખાલી) luaL_error(L, "invalid expression");
    luaV_pushtypval(L, tv);
    free_tv(tv);
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_beep(lua_State *L UNUSED)
{
    vim_beep(BO_LANG);
    પાછા 0;
}

    સ્થિર પૂર્ણાંક
lua_V_લીટી(lua_State *L)
{
    luaV_pushline(L, curbuf, curwin->w_cursor.lnum);
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_list(lua_State *L)
{
    list_T *l;
    પૂર્ણાંક initarg = !lua_isnoneornil(L, 1);

    જો (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    l = list_alloc();
    જો (l == ખાલી)
    {
	lua_pushnil(L);
	પાછા 1;
    }

    luaV_newlist(L, l);
    જો (!initarg)
	પાછા 1;

    // traverse table to init list
    પૂર્ણાંક notnil, i = 0;
    typval_T v;
    કરો
    {
	lua_rawgeti(L, 1, ++i);
	notnil = !lua_isnil(L, -1);
	જો (notnil)
	{
	    luaV_checktypval(L, -1, &v, "vim.list");
	    list_append_tv(l, &v);
	    clear_tv(&v);
	}
	lua_કાઢો(L, 1); //કિંમત
    } જ્યારે (notnil);

    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_dict(lua_State *L)
{
    dict_T *d;
    પૂર્ણાંક initarg = !lua_isnoneornil(L, 1);

    જો (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    d = dict_alloc();
    જો (d == ખાલી)
    {
	lua_pushnil(L);
	પાછા 1;
    }

    luaV_newdict(L, d);
    જો (!initarg)
	પાછા 1;

    // traverse table to init dict
    lua_pushnil(L);
    જ્યારે (lua_next(L, 1))
    {
	char_u *ચાવી;
	dictitem_T *di;
	typval_T v;

	lua_pushvalue(L, -2); // dup key in case it's a number
	ચાવી = (char_u *) lua_tostring(L, -1);
	જો (ચાવી == ખાલી)
	{
	    lua_pushnil(L);
	    પાછા 1;
	}
	જો (*ચાવી == NUL)
	    luaL_error(L, "table has empty key");
	luaV_checktypval(L, -2, &v, "vim.dict"); //કિંમત
	di = dictitem_alloc(ચાવી);
	જો (di == ખાલી || dict_add(d, di) == FAIL)
	{
	    vim_free(di);
	    lua_pushnil(L);
	    પાછા 1;
	}
	di->di_tv = v;
	lua_કાઢો(L, 2); // key copy and value
    }

    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_blob(lua_State *L)
{
    blob_T *b;
    પૂર્ણાંક initarg = !lua_isnoneornil(L, 1);

    જો (initarg && !lua_isstring(L, 1))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 1));

    b = blob_alloc();
    જો (b == ખાલી)
    {
	lua_pushnil(L);
	પાછા 1;
    }

    luaV_newblob(L, b);
    જો (!initarg)
	પાછા 1;

    // traverse table to init blob
    size_t i, l = 0;
    અચલ અક્ષર *s = lua_tolstring(L, 1, &l);

    જો (ga_grow(&b->bv_ga, (પૂર્ણાંક)l) == OK)
	માટે (i = 0; i < l; ++i)
	    ga_append(&b->bv_ga, s[i]);

    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_funcref(lua_State *L)
{
    અચલ અક્ષર *name = luaL_checkstring(L, 1);
    // note: not checking if function exists (needs function_exists)
    જો (name == ખાલી || *name == NUL || VIM_ISDIGIT(*name))
	luaL_error(L, "invalid function name: %s", name);
    luaV_newfuncref(L, (char_u *) name);
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_buffer(lua_State *L)
{
    buf_T *buf;
    જો (lua_isstring(L, 1)) // get by number or name?
    {
	જો (lua_isnumber(L, 1)) // by number?
	{
	    પૂર્ણાંક n = lua_tointeger(L, 1);
	    FOR_ALL_BUFFERS(buf)
		જો (buf->b_fnum == n) તોડો;
	}
	નહીંતર // by name
	{
	    size_t l;
	    અચલ અક્ષર *s = lua_tolstring(L, 1, &l);
	    FOR_ALL_BUFFERS(buf)
	    {
		જો (buf->b_ffname == ખાલી || buf->b_sfname == ખાલી)
		{
		    જો (l == 0) તોડો;
		}
		નહીંતર જો (strncmp(s, (અક્ષર *)buf->b_ffname, l) == 0
			|| strncmp(s, (અક્ષર *)buf->b_sfname, l) == 0)
		    તોડો;
	    }
	}
    }
    નહીંતર
	buf = (lua_toboolean(L, 1)) ? firstbuf : curbuf; // first buffer?
    luaV_pushbuffer(L, buf);
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_window(lua_State *L)
{
    win_T *win;
    જો (lua_isnumber(L, 1)) // get by number?
    {
	પૂર્ણાંક n = lua_tointeger(L, 1);
	માટે (win = firstwin; win != ખાલી; win = win->w_next, n--)
	    જો (n == 1) તોડો;
    }
    નહીંતર
	win = (lua_toboolean(L, 1)) ? firstwin : curwin; // first window?
    luaV_pushwindow(L, win);
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_open(lua_State *L)
{
    char_u *s = ખાલી;
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    જો (lua_isstring(L, 1)) s = (char_u *) lua_tostring(L, 1);
    luaV_pushbuffer(L, buflist_new(s, ખાલી, 1L, BLN_LISTED));
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_type(lua_State *L)
{
    luaL_checkany(L, 1);
    જો (lua_type(L, 1) == LUA_TUSERDATA) // check vim udata?
    {
	lua_settop(L, 1);
	જો (lua_getmetatable(L, 1))
	{
	    luaV_getfield(L, LUAVIM_LIST);
	    જો (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "list");
		પાછા 1;
	    }
	    luaV_getfield(L, LUAVIM_DICT);
	    જો (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "dict");
		પાછા 1;
	    }
	    luaV_getfield(L, LUAVIM_BLOB);
	    જો (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "blob");
		પાછા 1;
	    }
	    luaV_getfield(L, LUAVIM_FUNCREF);
	    જો (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "funcref");
		પાછા 1;
	    }
	    luaV_getfield(L, LUAVIM_BUFFER);
	    જો (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "buffer");
		પાછા 1;
	    }
	    luaV_getfield(L, LUAVIM_WINDOW);
	    જો (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "window");
		પાછા 1;
	    }
	}
    }
    lua_pushstring(L, luaL_typename(L, 1)); // fallback
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_call(lua_State *L)
{
    પૂર્ણાંક		argc = lua_gettop(L) - 1;
    size_t	funcname_len;
    char_u	*funcname;
    અક્ષર	*error = ખાલી;
    typval_T	rettv;
    typval_T	argv[MAX_FUNC_ARGS + 1];
    પૂર્ણાંક		i = 0;

    જો (argc > MAX_FUNC_ARGS)
	પાછા luaL_error(L, "Function called with too many arguments");

    funcname = (char_u *)luaL_checklstring(L, 1, &funcname_len);

    માટે (; i < argc; i++)
    {
	જો (luaV_totypval(L, i + 2, &argv[i]) == FAIL)
	{
	    error = "lua: cannot convert value";
	    જાઓ free_vim_args;
	}
    }

    argv[argc].v_type = VAR_UNKNOWN;

    જો (call_vim_function(funcname, argc, argv, &rettv) == FAIL)
    {
	error = "lua: call_vim_function failed";
	જાઓ free_vim_args;
    }

    luaV_pushtypval(L, &rettv);
    clear_tv(&rettv);

free_vim_args:
    જ્યારે (i > 0)
	clear_tv(&argv[--i]);

    જો (error == ખાલી)
	પાછા 1;
    નહીંતર
	પાછા luaL_error(L, error);
}

/*
 * Return the Vim version as a Lua table
 */
    સ્થિર પૂર્ણાંક
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
    પાછા 1;
}

સ્થિર અચલ luaL_Reg luaV_module[] = {
    {"command", luaV_command},
    {"eval", luaV_eval},
    {"beep", luaV_beep},
    {"line", lua_V_લીટી},
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
    {"lua_version", ખાલી},
    {ખાલી, ખાલી}
};

/*
 * for freeing list, dict, buffer and window objects; lightuserdata as arg
 */
    સ્થિર પૂર્ણાંક
luaV_free(lua_State *L)
{
    lua_pushnil(L);
    luaV_setudata(L, lua_touserdata(L, 1));
    પાછા 0;
}

    સ્થિર પૂર્ણાંક
luaV_luaeval(lua_State *L)
{
    luaL_Buffer b;
    size_t l;
    અચલ અક્ષર *str = lua_tolstring(L, 1, &l);
    typval_T *arg = (typval_T *) lua_touserdata(L, 2);
    typval_T *rettv = (typval_T *) lua_touserdata(L, 3);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, LUAVIM_EVALHEADER, કદ(LUAVIM_EVALHEADER) - 1);
    luaL_addlstring(&b, str, l);
    luaL_pushresult(&b);
    str = lua_tolstring(L, -1, &l);
    જો (luaL_loadbuffer(L, str, l, LUAVIM_EVALNAME)) // compile error?
    {
	luaV_emsg(L);
	પાછા 0;
    }
    luaV_pushtypval(L, arg);
    જો (lua_pcall(L, 1, 1, 0)) // running error?
    {
	luaV_emsg(L);
	પાછા 0;
    }
    જો (luaV_totypval(L, -1, rettv) == FAIL)
	emsg("luaeval: cannot convert value");
    પાછા 0;
}

    સ્થિર પૂર્ણાંક
luaV_setref(lua_State *L)
{
    પૂર્ણાંક copyID = lua_tointeger(L, 1);
    પૂર્ણાંક abort = FALSE;

    lua_pushlightuserdata(L, (શૂન્ય *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // the cache table

    luaV_getfield(L, LUAVIM_LIST);
    luaV_getfield(L, LUAVIM_DICT);
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_pushnil(L);
    // traverse cache table
    જ્યારે (!abort && lua_next(L, 2) != 0)
    {
	lua_getmetatable(L, -1);
	જો (lua_rawequal(L, -1, 3)) // list?
	{
	    list_T *l = (list_T *)lua_touserdata(L, 6); //ચાવી

	    abort = set_ref_in_list(l, copyID);
	}
	નહીંતર જો (lua_rawequal(L, -1, 4)) // dict?
	{
	    dict_T *d = (dict_T *)lua_touserdata(L, 6); //ચાવી

	    abort = set_ref_in_dict(d, copyID);
	}
	નહીંતર જો (lua_rawequal(L, -1, 5)) // funcref?
	{
	    luaV_Funcref *f = (luaV_Funcref *)lua_touserdata(L, 6); //ચાવી

	    abort = set_ref_in_dict(f->self, copyID);
	}
	lua_કાઢો(L, 2); // metatable and value
    }
    lua_pushinteger(L, abort);
    પાછા 1;
}

    સ્થિર પૂર્ણાંક
luaV_pushversion(lua_State *L)
{
    પૂર્ણાંક major = 0;
    પૂર્ણાંક minor = 0;
    પૂર્ણાંક patch = 0;
    અક્ષર s[16];

    sscanf(LUAVIM_VERSION, "Lua %d.%d.%d", &major, &minor, &patch);
    vim_snprintf(s, કદ(s), "%d.%d.%d", major, minor, patch);
    lua_pushstring(L, s);
    પાછા 0;
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

    સ્થિર પૂર્ણાંક
luaopen_vim(lua_State *L)
{
    lua_newtable(L);  // cache table
    lua_newtable(L);  // cache table's metatable
    lua_pushstring(L, "v");
    lua_setfield(L, -2, "__mode");
    lua_setmetatable(L, -2); // cache is weak-valued
    // put the cache table in the registry for luaV_get/setudata()
    lua_pushlightuserdata(L, (શૂન્ય *) LUAVIM_UDATA_CACHE);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
    lua_કાઢો(L, 1);  // we don't need the cache table here anymore
    // print
    lua_pushcfunction(L, luaV_print);
    lua_setglobal(L, "print");
    // debug.debug
    lua_getglobal(L, "debug");
    lua_pushcfunction(L, luaV_debug);
    lua_setfield(L, -2, "debug");
    lua_કાઢો(L, 1);
    // free
    lua_pushlightuserdata(L, (શૂન્ય *) LUAVIM_FREE);
    lua_pushcfunction(L, luaV_free);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // luaeval
    lua_pushlightuserdata(L, (શૂન્ય *) LUAVIM_LUAEVAL);
    lua_pushcfunction(L, luaV_luaeval);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // setref
    lua_pushlightuserdata(L, (શૂન્ય *) LUAVIM_SETREF);
    lua_pushcfunction(L, luaV_setref);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // register
    luaV_newmetatable(L, LUAVIM_LIST);
    luaV_register(L, luaV_List_mt);
    lua_કાઢો(L, 1);
    luaV_newmetatable(L, LUAVIM_DICT);
    luaV_register(L, luaV_Dict_mt);
    lua_કાઢો(L, 1);
    luaV_newmetatable(L, LUAVIM_BLOB);
    luaV_register(L, luaV_Blob_mt);
    lua_કાઢો(L, 1);
    luaV_newmetatable(L, LUAVIM_FUNCREF);
    luaV_register(L, luaV_Funcref_mt);
    lua_કાઢો(L, 1);
    luaV_newmetatable(L, LUAVIM_BUFFER);
    luaV_register(L, luaV_Buffer_mt);
    lua_કાઢો(L, 1);
    luaV_newmetatable(L, LUAVIM_WINDOW);
    luaV_register(L, luaV_Window_mt);
    lua_કાઢો(L, 1);
    lua_newtable(L); // vim table
    luaV_register(L, luaV_module);
    luaV_pushversion(L);
    lua_setfield(L, -2, "lua_version");
    lua_setglobal(L, LUAVIM_NAME);
    // custom code
    (શૂન્ય)luaL_dostring(L, LUA_VIM_FN_CODE);
    (શૂન્ય)luaL_dostring(L, LUA_VIM_UPDATE_PACKAGE_PATHS);
    (શૂન્ય)luaL_dostring(L, LUA_VIM_SETUP_VARIABLE_DICTS);

    lua_getglobal(L, LUAVIM_NAME);
    lua_getfield(L, -1, "_update_package_paths");

    જો (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);

    પાછા 0;
}

    સ્થિર lua_State *
luaV_newstate(શૂન્ય)
{
    lua_State *L = luaL_newstate();
# જો LUA_VERSION_અંક >= 505
    luaL_openselectedlibs(L, ~0, 0); // core libs
# નહીંતર
    luaL_openlibs(L); // core libs
# endif
    lua_pushcfunction(L, luaopen_vim); // vim
    lua_call(L, 0, 0);
    પાછા L;
}

    સ્થિર શૂન્ય
luaV_setrange(lua_State *L, પૂર્ણાંક line1, પૂર્ણાંક line2)
{
    lua_getglobal(L, LUAVIM_NAME);
    lua_pushinteger(L, line1);
    lua_setfield(L, -2, "firstline");
    lua_pushinteger(L, line2);
    lua_setfield(L, -2, "lastline");
    lua_કાઢો(L, 1); // vim table
}


// =======   Interface   =======

સ્થિર lua_State *L = ખાલી;

    સ્થિર પૂર્ણાંક
lua_isopen(શૂન્ય)
{
    પાછા L != ખાલી;
}

    સ્થિર પૂર્ણાંક
lua_init(શૂન્ય)
{
    જો (lua_isopen())
	પાછા OK;

# ifdef DYNAMIC_LUA
    જો (!lua_enabled(TRUE))
    {
	emsg(_("Lua library cannot be loaded."));
	પાછા FAIL;
    }
# endif
    L = luaV_newstate();

    પાછા OK;
}

    શૂન્ય
lua_end(શૂન્ય)
{
    જો (!lua_isopen())
	પાછા;

    lua_close(L);
    L = ખાલી;
}

/*
 * ex commands
 */
    શૂન્ય
ex_lua(exarg_T *eap)
{
    અક્ષર *script = (અક્ષર *)script_get(eap, eap->arg);

    જો (!eap->skip && lua_init() == OK)
    {
	અક્ષર *s = script != ખાલી ? script : (અક્ષર *)eap->arg;

	luaV_setrange(L, eap->line1, eap->line2);
	જો (luaL_loadbuffer(L, s, strlen(s), LUAVIM_CHUNKNAME)
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
    જો (script != ખાલી)
	vim_free(script);
}

    શૂન્ય
ex_luado(exarg_T *eap)
{
    linenr_T l;
    અચલ અક્ષર *s = (અચલ અક્ષર *) eap->arg;
    luaL_Buffer b;
    size_t len;
    buf_T *was_curbuf = curbuf;

    જો (lua_init() == FAIL) પાછા;
    જો (u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
    {
	emsg(_("cannot save undo information"));
	પાછા;
    }
    luaV_setrange(L, eap->line1, eap->line2);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, "return function(line, linenr) ", 30); // header
    luaL_addlstring(&b, s, strlen(s));
    luaL_addlstring(&b, " end", 4); // footer
    luaL_pushresult(&b);
    s = lua_tolstring(L, -1, &len);
    જો (luaL_loadbuffer(L, s, len, LUAVIM_CHUNKNAME))
    {
	luaV_emsg(L);
	lua_કાઢો(L, 1); // function body
	પાછા;
    }
    lua_call(L, 0, 1);
    lua_replace(L, -2); // function -> body
    માટે (l = eap->line1; l <= eap->line2; l++)
    {
	// Check the line number, the command may have deleted lines.
	જો (l > curbuf->b_ml.ml_લીટી_ગણતરી)
	    તોડો;

	lua_pushvalue(L, -1); // function
	luaV_pushline(L, curbuf, l); // current line as arg
	lua_pushinteger(L, l); // current line number as arg
	જો (lua_pcall(L, 2, 1, 0))
	{
	    luaV_emsg(L);
	    તોડો;
	}

	// Catch the command switching to another buffer.
	// Check the line number, the command may have deleted lines.
	જો (curbuf != was_curbuf || l > curbuf->b_ml.ml_લીટી_ગણતરી)
	    તોડો;

	જો (lua_isstring(L, -1)) // update line?
	{
# ifdef HAVE_SANDBOX
	    luaV_checksandbox(L);
# endif
	    ml_replace(l, luaV_toline(L, -1), TRUE);
	    changed_bytes(l, 0);
	    lua_કાઢો(L, 1); // result from luaV_toline
	}
	lua_કાઢો(L, 1); //લીટી
    }
    lua_કાઢો(L, 1); // function
    check_cursor();
    update_screen(UPD_NOT_VALID);
}

    શૂન્ય
ex_luafile(exarg_T *eap)
{
    જો (lua_init() == FAIL)
	પાછા;
    જો (!eap->skip)
    {
	luaV_setrange(L, eap->line1, eap->line2);
	જો (luaL_loadfile(L, (અક્ષર *) eap->arg) || lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
}

# define luaV_freetype(typ,tname) \
	શૂન્ય \
    lua_##tname##_free(typ *o) \
    { \
	જો (!lua_isopen()) પાછા; \
	luaV_getfield(L, LUAVIM_FREE); \
	lua_pushlightuserdata(L, (શૂન્ય *) o); \
	lua_call(L, 1, 0); \
    }

luaV_freetype(buf_T, buffer)
luaV_freetype(win_T, window)

    શૂન્ય
do_luaeval(char_u *str, typval_T *arg, typval_T *rettv)
{
    lua_init();
    luaV_getfield(L, LUAVIM_LUAEVAL);
    lua_pushstring(L, (અક્ષર *) str);
    lua_pushlightuserdata(L, (શૂન્ય *) arg);
    lua_pushlightuserdata(L, (શૂન્ય *) rettv);
    lua_call(L, 3, 0);
}

    પૂર્ણાંક
set_ref_in_lua(પૂર્ણાંક copyID)
{
    પૂર્ણાંક aborted = 0;

    જો (!lua_isopen())
	પાછા 0;

    luaV_getfield(L, LUAVIM_SETREF);
    // call the function with 1 arg, getting 1 result back
    lua_pushinteger(L, copyID);
    lua_call(L, 1, 1);
    // get the result
    aborted = lua_tointeger(L, -1);
    // pop result off the stack
    lua_કાઢો(L, 1);

    પાછા aborted;
}

    શૂન્ય
update_package_paths_in_lua(શૂન્ય)
{
    જો (!lua_isopen())
	પાછા;

    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "_update_package_paths");

    જો (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);
}

/*
 * Native C function callback
 */
    સ્થિર પૂર્ણાંક
luaV_call_lua_func(
	પૂર્ણાંક	 argcount,
	typval_T *argvars,
	typval_T *rettv,
	શૂન્ય	 *state)
{
    પૂર્ણાંક i;
    પૂર્ણાંક luaargcount = argcount;
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_funcref);

    જો (funcstate->lua_tableref != LUA_NOREF)
    {
	// First arg for metatable __call method is a table
	luaargcount += 1;
	lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    }

    માટે (i = 0; i < argcount; ++i)
	luaV_pushtypval(funcstate->L, &argvars[i]);

    જો (lua_pcall(funcstate->L, luaargcount, 1, 0))
    {
	luaV_emsg(funcstate->L);
	પાછા (પૂર્ણાંક)FCERR_OTHER;
    }

    luaV_checktypval(funcstate->L, -1, rettv, "get return value");
    પાછા (પૂર્ણાંક)FCERR_NONE;
}

/*
 * Free up any lua references held by the func state.
 */
    સ્થિર શૂન્ય
luaV_call_lua_func_free(શૂન્ય *state)
{
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_funcref);
    funcstate->L = ખાલી;
    જો (funcstate->lua_tableref != LUA_NOREF)
	luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    VIM_CLEAR(funcstate);
}

#endif
