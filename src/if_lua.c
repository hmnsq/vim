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

#ଅନ୍ତର୍ଭାଵ "vim.h"
#ଅନ୍ତର୍ଭାଵ "version.h"

#ଅନ୍ତର୍ଭାଵ <lua.h>
#ଅନ୍ତର୍ଭାଵ <lualib.h>
#ଅନ୍ତର୍ଭାଵ <lauxlib.h>

#ଯଦି __STDC_VERSION__ >= 199901L
# define LUAV_INLINE ଧାଡ଼ିଭିତରେ
#ଅନ୍ୟଥା
# define LUAV_INLINE
#endif

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#ଯଦି defined(FEAT_LUA)

# define LUAVIM_CHUNKNAME "vim chunk"
# define LUAVIM_NAME "vim"
# define LUAVIM_EVALNAME "luaeval"
# define LUAVIM_EVALHEADER "local _A=select(1,...) return "

# ifdef LUA_RELEASE
#  define LUAVIM_VERSION LUA_RELEASE
# ଅନ୍ୟଥା
#  define LUAVIM_VERSION LUA_VERSION
# endif

ପ୍ରକାରବ୍ୟାଖ୍ୟା buf_T *luaV_Buffer;
ପ୍ରକାରବ୍ୟାଖ୍ୟା win_T *luaV_Window;
ପ୍ରକାରବ୍ୟାଖ୍ୟା dict_T *luaV_Dict;
ପ୍ରକାରବ୍ୟାଖ୍ୟା list_T *luaV_List;
ପ୍ରକାରବ୍ୟାଖ୍ୟା blob_T *luaV_Blob;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    char_u	*name;	// funcref
    dict_T	*self;	// selfdict
} luaV_Funcref;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*msgfunc_T)(ଅକ୍ଷର *);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lua_funcref;    // ref to a lua func
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lua_tableref;   // ref to a lua table if metatable else LUA_NOREF. used
			// for __call
    lua_State *L;
} luaV_CFuncState;

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର LUAVIM_DICT[] = "dict";
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର LUAVIM_LIST[] = "list";
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର LUAVIM_BLOB[] = "blob";
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର LUAVIM_FUNCREF[] = "funcref";
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର LUAVIM_BUFFER[] = "buffer";
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର LUAVIM_WINDOW[] = "window";
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର LUAVIM_FREE[] = "luaV_free";
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର LUAVIM_LUAEVAL[] = "luaV_luaeval";
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର LUAVIM_SETREF[] = "luaV_setref";

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର LUA___CALL[] = "__call";

// get/setudata manage references to vim userdata in a cache table through
// object pointers (light userdata). The cache table itself is retrieved
// from the registry.

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର LUAVIM_UDATA_CACHE[] = "luaV_udata_cache";

# define luaV_getfield(L, s) \
    lua_pushlightuserdata((L), (ଶୂନ୍ୟ *)(s)); \
    lua_rawget((L), LUA_REGISTRYINDEX)
# define luaV_checksandbox(L) \
    ଯଦି (sandbox) luaL_error((L), "not allowed in sandbox")
# define lua_V_ସନ୍ଦେଶ(L) luaV_msgfunc((L), (msgfunc_T) ସନ୍ଦେଶ)
# define luaV_emsg(L) luaV_msgfunc((L), (msgfunc_T) emsg)
# define luaV_checktypval(L, a, v, ସନ୍ଦେଶ) \
    କର { \
	ଯଦି (luaV_totypval(L, a, v) == FAIL) \
	    luaL_error(L, ସନ୍ଦେଶ ": cannot convert value"); \
    } ଯେତେବେଳେ (0)

ସ୍ଥିର luaV_List *luaV_pushlist(lua_State *L, list_T *lis);
ସ୍ଥିର luaV_Dict *luaV_pushdict(lua_State *L, dict_T *dic);
ସ୍ଥିର luaV_Blob *luaV_pushblob(lua_State *L, blob_T *blo);
ସ୍ଥିର luaV_Funcref *luaV_pushfuncref(lua_State *L, char_u *name);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା luaV_call_lua_func(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount, typval_T *argvars, typval_T *rettv, ଶୂନ୍ୟ *state);
ସ୍ଥିର ଶୂନ୍ୟ luaV_call_lua_func_free(ଶୂନ୍ୟ *state);

# ଯଦି LUA_VERSION_ଅଙ୍କ <= 501
#  define luaV_register(L, l) luaL_register(L, ଖାଲି, l)
#  define luaL_typeerror luaL_typerror
# ଅନ୍ୟଥା
#  define luaV_register(L, l) luaL_setfuncs(L, l, 0)
# endif

# ifdef DYNAMIC_LUA

#  ifdef MSWIN
#   define load_dll vimLoadLib
#   define ପ୍ରତୀକ_from_dll GetProcAddress
#   define close_dll FreeLibrary
#   define load_dll_error GetWin32Error
#  ଅନ୍ୟଥା
#   include <dlfcn.h>
#   define HANDLE ଶୂନ୍ୟ*
#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#   define ପ୍ରତୀକ_from_dll dlsym
#   define close_dll dlclose
#   define load_dll_error dlerror
#  endif

// lauxlib
#  ଯଦି LUA_VERSION_ଅଙ୍କ <= 501
#   define luaL_register dll_luaL_register
#   define luaL_prepbuffer dll_luaL_prepbuffer
#   define luaL_openlib dll_luaL_openlib
#   define luaL_typerror dll_luaL_typerror
#   define luaL_loadfile dll_luaL_loadfile
#   define luaL_loadbuffer dll_luaL_loadbuffer
#  ଅନ୍ୟଥା
#   define luaL_prepbuffsize dll_luaL_prepbuffsize
#   define luaL_setfuncs dll_luaL_setfuncs
#   define luaL_loadfilex dll_luaL_loadfilex
#   define luaL_loadbufferx dll_luaL_loadbufferx
#   define luaL_argerror dll_luaL_argerror
#  endif
#  ଯଦି LUA_VERSION_ଅଙ୍କ >= 504
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
#  ଯଦି LUA_VERSION_ଅଙ୍କ <= 501
#   define lua_tonumber dll_lua_tonumber
#   define lua_tointeger dll_lua_tointeger
#   define lua_call dll_lua_call
#   define lua_pcall dll_lua_pcall
#  ଅନ୍ୟଥା
#   define lua_tonumberx dll_lua_tonumberx
#   define lua_tointegerx dll_lua_tointegerx
#   define lua_callk dll_lua_callk
#   define lua_pcallk dll_lua_pcallk
#   define lua_getglobal dll_lua_getglobal
#   define lua_setglobal dll_lua_setglobal
#  endif
#  ଯଦି LUA_VERSION_ଅଙ୍କ <= 502
#   define lua_replace dll_lua_replace
#   define lua_remove dll_lua_remove
#  endif
#  ଯଦି LUA_VERSION_ଅଙ୍କ >= 503
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
#  ଯଦି LUA_VERSION_ଅଙ୍କ >= 504
#   define lua_newuserdatauv dll_lua_newuserdatauv
#  ଅନ୍ୟଥା
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
#  ଯଦି LUA_VERSION_ଅଙ୍କ >= 505
#   define luaL_openselectedlibs dll_luaL_openselectedlibs
#  ଅନ୍ୟଥା
#   define luaL_openlibs dll_luaL_openlibs
#  endif

// lauxlib
#  ଯଦି LUA_VERSION_ଅଙ୍କ <= 501
ଶୂନ୍ୟ (*dll_luaL_register) (lua_State *L, ଅଚଳ ଅକ୍ଷର *libname, ଅଚଳ luaL_Reg *l);
ଅକ୍ଷର *(*dll_luaL_prepbuffer) (luaL_Buffer *B);
ଶୂନ୍ୟ (*dll_luaL_openlib) (lua_State *L, ଅଚଳ ଅକ୍ଷର *libname, ଅଚଳ luaL_Reg *l, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nup);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_luaL_typerror) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା narg, ଅଚଳ ଅକ୍ଷର *tname);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_luaL_loadfile) (lua_State *L, ଅଚଳ ଅକ୍ଷର *filename);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_luaL_loadbuffer) (lua_State *L, ଅଚଳ ଅକ୍ଷର *buff, size_t sz, ଅଚଳ ଅକ୍ଷର *name);
#  ଅନ୍ୟଥା
ଅକ୍ଷର *(*dll_luaL_prepbuffsize) (luaL_Buffer *B, size_t sz);
ଶୂନ୍ୟ (*dll_luaL_setfuncs) (lua_State *L, ଅଚଳ luaL_Reg *l, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nup);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_luaL_loadfilex) (lua_State *L, ଅଚଳ ଅକ୍ଷର *filename, ଅଚଳ ଅକ୍ଷର *mode);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_luaL_loadbufferx) (lua_State *L, ଅଚଳ ଅକ୍ଷର *buff, size_t sz, ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଅକ୍ଷର *mode);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_luaL_argerror) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା numarg, ଅଚଳ ଅକ୍ଷର *extramsg);
#  endif
#  ଯଦି LUA_VERSION_ଅଙ୍କ >= 504
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_luaL_typeerror) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା narg, ଅଚଳ ଅକ୍ଷର *tname);
#  endif
ଶୂନ୍ୟ (*dll_luaL_checkany) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା narg);
ଅଚଳ ଅକ୍ଷର *(*dll_luaL_checklstring) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_Arg, size_t *l);
lua_Integer (*dll_luaL_checkinteger) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_Arg);
lua_Integer (*dll_luaL_optinteger) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nArg, lua_Integer def);
ଶୂନ୍ୟ (*dll_luaL_checktype) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା narg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା t);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_luaL_error) (lua_State *L, ଅଚଳ ଅକ୍ଷର *fmt, ...);
lua_State *(*dll_luaL_newstate) (ଶୂନ୍ୟ);
ଶୂନ୍ୟ (*dll_luaL_buffinit) (lua_State *L, luaL_Buffer *B);
ଶୂନ୍ୟ (*dll_luaL_addlstring) (luaL_Buffer *B, ଅଚଳ ଅକ୍ଷର *s, size_t l);
ଶୂନ୍ୟ (*dll_luaL_pushresult) (luaL_Buffer *B);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_luaL_loadstring) (lua_State *L, ଅଚଳ ଅକ୍ଷର *s);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_luaL_ref) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
#  ଯଦି LUA_VERSION_ଅଙ୍କ <= 502
ଶୂନ୍ୟ (*dll_luaL_unref) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n);
#  ଅନ୍ୟଥା
ଶୂନ୍ୟ (*dll_luaL_unref) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, lua_Integer n);
#  endif
// lua
#  ଯଦି LUA_VERSION_ଅଙ୍କ <= 501
lua_ସଂଖ୍ୟା (*dll_lua_tonumber) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
lua_Integer (*dll_lua_tointeger) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
ଶୂନ୍ୟ (*dll_lua_call) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nargs, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nresults);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_lua_pcall) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nargs, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nresults, ପୂର୍ଣ୍ଣସଂଖ୍ୟା errfunc);
#  ଅନ୍ୟଥା
lua_ସଂଖ୍ୟା (*dll_lua_tonumberx) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *isnum);
lua_Integer (*dll_lua_tointegerx) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *isnum);
ଶୂନ୍ୟ (*dll_lua_callk) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nargs, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nresults, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctx,
	lua_CFunction k);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_lua_pcallk) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nargs, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nresults, ପୂର୍ଣ୍ଣସଂଖ୍ୟା errfunc,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ctx, lua_CFunction k);
ଶୂନ୍ୟ (*dll_lua_getglobal) (lua_State *L, ଅଚଳ ଅକ୍ଷର *var);
ଶୂନ୍ୟ (*dll_lua_setglobal) (lua_State *L, ଅଚଳ ଅକ୍ଷର *var);
#  endif
#  ଯଦି LUA_VERSION_ଅଙ୍କ <= 502
ଶୂନ୍ୟ (*dll_lua_replace) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
ଶୂନ୍ୟ (*dll_lua_remove) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
#  endif
#  ଯଦି LUA_VERSION_ଅଙ୍କ >= 503
ଶୂନ୍ୟ  (*dll_lua_rotate) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n);
ଶୂନ୍ୟ (*dll_lua_copy) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fromidx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା toidx);
#  endif
ଅଚଳ ଅକ୍ଷର *(*dll_lua_typename) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା tp);
ଶୂନ୍ୟ       (*dll_lua_close) (lua_State *L);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_lua_gettop) (lua_State *L);
ଶୂନ୍ୟ (*dll_lua_settop) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
ଶୂନ୍ୟ (*dll_lua_pushvalue) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_lua_isnumber) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_lua_isstring) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_lua_type) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_lua_rawequal) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx2);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_lua_toboolean) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
ଅଚଳ ଅକ୍ଷର *(*dll_lua_tolstring) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, size_t *len);
ଶୂନ୍ୟ *(*dll_lua_touserdata) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
ଶୂନ୍ୟ (*dll_lua_pushnil) (lua_State *L);
ଶୂନ୍ୟ (*dll_lua_pushnumber) (lua_State *L, lua_ସଂଖ୍ୟା n);
ଶୂନ୍ୟ (*dll_lua_pushinteger) (lua_State *L, lua_Integer n);
ଶୂନ୍ୟ (*dll_lua_pushlstring) (lua_State *L, ଅଚଳ ଅକ୍ଷର *s, size_t l);
ଶୂନ୍ୟ (*dll_lua_pushstring) (lua_State *L, ଅଚଳ ଅକ୍ଷର *s);
ଅଚଳ ଅକ୍ଷର *(*dll_lua_pushfstring) (lua_State *L, ଅଚଳ ଅକ୍ଷର *fmt, ...);
ଶୂନ୍ୟ (*dll_lua_pushcclosure) (lua_State *L, lua_CFunction fn, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n);
ଶୂନ୍ୟ (*dll_lua_pushboolean) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା b);
ଶୂନ୍ୟ (*dll_lua_pushlightuserdata) (lua_State *L, ଶୂନ୍ୟ *p);
ଶୂନ୍ୟ (*dll_lua_getfield) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ଅଚଳ ଅକ୍ଷର *k);
#  ଯଦି LUA_VERSION_ଅଙ୍କ <= 502
ଶୂନ୍ୟ (*dll_lua_rawget) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
ଶୂନ୍ୟ (*dll_lua_rawgeti) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n);
#  ଅନ୍ୟଥା
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_lua_rawget) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_lua_rawgeti) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, lua_Integer n);
#  endif
ଶୂନ୍ୟ (*dll_lua_createtable) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା narr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nrec);
ଶୂନ୍ୟ (*dll_lua_settable) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
#  ଯଦି LUA_VERSION_ଅଙ୍କ >= 504
ଶୂନ୍ୟ *(*dll_lua_newuserdatauv) (lua_State *L, size_t sz, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nuvalue);
#  ଅନ୍ୟଥା
ଶୂନ୍ୟ *(*dll_lua_newuserdata) (lua_State *L, size_t sz);
#  endif
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_lua_getmetatable) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା objindex);
ଶୂନ୍ୟ (*dll_lua_setfield) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ଅଚଳ ଅକ୍ଷର *k);
ଶୂନ୍ୟ (*dll_lua_rawset) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
#  ଯଦି LUA_VERSION_ଅଙ୍କ <= 502
ଶୂନ୍ୟ (*dll_lua_rawseti) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n);
#  ଅନ୍ୟଥା
ଶୂନ୍ୟ (*dll_lua_rawseti) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, lua_Integer n);
#  endif
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_lua_setmetatable) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା objindex);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_lua_next) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx);
// libs
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_luaopen_base) (lua_State *L);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_luaopen_table) (lua_State *L);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_luaopen_string) (lua_State *L);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_luaopen_math) (lua_State *L);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_luaopen_io) (lua_State *L);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_luaopen_os) (lua_State *L);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_luaopen_package) (lua_State *L);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_luaopen_debug) (lua_State *L);
#  ଯଦି LUA_VERSION_ଅଙ୍କ >= 505
ଶୂନ୍ୟ (*dll_luaL_openselectedlibs) (lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା load, ପୂର୍ଣ୍ଣସଂଖ୍ୟା preload);
#  ଅନ୍ୟଥା
ଶୂନ୍ୟ (*dll_luaL_openlibs) (lua_State *L);
#  endif

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ **luaV_function;
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ଅଚଳ ଅକ୍ଷର *name;
    luaV_function func;
} luaV_Reg;

ସ୍ଥିର ଅଚଳ luaV_Reg luaV_dll[] = {
    // lauxlib
#  ଯଦି LUA_VERSION_ଅଙ୍କ <= 501
    {"luaL_register", (luaV_function) &dll_luaL_register},
    {"luaL_prepbuffer", (luaV_function) &dll_luaL_prepbuffer},
    {"luaL_openlib", (luaV_function) &dll_luaL_openlib},
    {"luaL_typerror", (luaV_function) &dll_luaL_typerror},
    {"luaL_loadfile", (luaV_function) &dll_luaL_loadfile},
    {"luaL_loadbuffer", (luaV_function) &dll_luaL_loadbuffer},
#  ଅନ୍ୟଥା
    {"luaL_prepbuffsize", (luaV_function) &dll_luaL_prepbuffsize},
    {"luaL_setfuncs", (luaV_function) &dll_luaL_setfuncs},
    {"luaL_loadfilex", (luaV_function) &dll_luaL_loadfilex},
    {"luaL_loadbufferx", (luaV_function) &dll_luaL_loadbufferx},
    {"luaL_argerror", (luaV_function) &dll_luaL_argerror},
#  endif
#  ଯଦି LUA_VERSION_ଅଙ୍କ >= 504
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
#  ଯଦି LUA_VERSION_ଅଙ୍କ <= 501
    {"lua_tonumber", (luaV_function) &dll_lua_tonumber},
    {"lua_tointeger", (luaV_function) &dll_lua_tointeger},
    {"lua_call", (luaV_function) &dll_lua_call},
    {"lua_pcall", (luaV_function) &dll_lua_pcall},
#  ଅନ୍ୟଥା
    {"lua_tonumberx", (luaV_function) &dll_lua_tonumberx},
    {"lua_tointegerx", (luaV_function) &dll_lua_tointegerx},
    {"lua_callk", (luaV_function) &dll_lua_callk},
    {"lua_pcallk", (luaV_function) &dll_lua_pcallk},
    {"lua_getglobal", (luaV_function) &dll_lua_getglobal},
    {"lua_setglobal", (luaV_function) &dll_lua_setglobal},
#  endif
#  ଯଦି LUA_VERSION_ଅଙ୍କ <= 502
    {"lua_replace", (luaV_function) &dll_lua_replace},
    {"lua_remove", (luaV_function) &dll_lua_remove},
#  endif
#  ଯଦି LUA_VERSION_ଅଙ୍କ >= 503
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
#  ଯଦି LUA_VERSION_ଅଙ୍କ >= 504
    {"lua_newuserdatauv", (luaV_function) &dll_lua_newuserdatauv},
#  ଅନ୍ୟଥା
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
#  ଯଦି LUA_VERSION_ଅଙ୍କ >= 505
    {"luaL_openselectedlibs", (luaV_function) &dll_luaL_openselectedlibs},
#  ଅନ୍ୟଥା
    {"luaL_openlibs", (luaV_function) &dll_luaL_openlibs},
#  endif
    {ଖାଲି, ଖାଲି}
};

ସ୍ଥିର HANDLE hinstLua = ଖାଲି;

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
lua_link_init(ଅକ୍ଷର *libname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା verbose)
{
    ଅଚଳ luaV_Reg *reg;
    ଯଦି (hinstLua) ଫେରନ୍ତୁ OK;
    hinstLua = load_dll(libname);
    ଯଦି (!hinstLua)
    {
	ଯଦି (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	ଫେରନ୍ତୁ FAIL;
    }
    ପାଇଁ (reg = luaV_dll; reg->func; reg++)
    {
	ଯଦି ((*reg->func = ପ୍ରତୀକ_from_dll(hinstLua, reg->name)) == ଖାଲି)
	{
	    close_dll(hinstLua);
	    hinstLua = 0;
	    ଯଦି (verbose)
		semsg(_(e_could_not_load_library_function_str), reg->name);
	    ଫେରନ୍ତୁ FAIL;
	}
    }
    ଫେରନ୍ତୁ OK;
}
# endif // DYNAMIC_LUA

# ଯଦି defined(DYNAMIC_LUA)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
lua_enabled(ପୂର୍ଣ୍ଣସଂଖ୍ୟା verbose)
{
    ଫେରନ୍ତୁ lua_link_init((ଅକ୍ଷର *)p_luadll, verbose) == OK;
}
# endif

# ଯଦି LUA_VERSION_ଅଙ୍କ > 501 && LUA_VERSION_ଅଙ୍କ < 504
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaL_typeerror(lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା narg, ଅଚଳ ଅକ୍ଷର *tname)
{
    ଅଚଳ ଅକ୍ଷର *ସନ୍ଦେଶ = lua_pushfstring(L, "%s expected, got %s",
	    tname, luaL_typename(L, narg));
    ଫେରନ୍ତୁ luaL_argerror(L, narg, ସନ୍ଦେଶ);
}
# endif

    ସ୍ଥିର LUAV_INLINE ଶୂନ୍ୟ
luaV_getudata(lua_State *L, ଶୂନ୍ୟ *v)
{
    lua_pushlightuserdata(L, (ଶୂନ୍ୟ *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // now the cache table is at the top of the stack
    lua_pushlightuserdata(L, v);
    lua_rawget(L, -2);
    lua_remove(L, -2);  // remove the cache table from the stack
}

    ସ୍ଥିର LUAV_INLINE ଶୂନ୍ୟ
luaV_setudata(lua_State *L, ଶୂନ୍ୟ *v)
{
    lua_pushlightuserdata(L, (ଶୂନ୍ୟ *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // cache table is at -1
    lua_pushlightuserdata(L, v);       // ...now at -2
    lua_pushvalue(L, -3);	       // copy the userdata (cache at -3)
    lua_rawset(L, -3);		       // consumes two stack items
    lua_କାଢ଼(L, 1);		       // and remove the cache table
}

// =======   Internal   =======

    ସ୍ଥିର ଶୂନ୍ୟ
luaV_newmetatable(lua_State *L, ଅଚଳ ଅକ୍ଷର *tname)
{
    lua_newtable(L);
    lua_pushlightuserdata(L, (ଶୂନ୍ୟ *) tname);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
}

    ସ୍ଥିର ଶୂନ୍ୟ *
luaV_toudata(lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ud, ଅଚଳ ଅକ୍ଷର *tname)
{
    ଶୂନ୍ୟ *p = lua_touserdata(L, ud);

    ଯଦି (p == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // value is userdata
    ଯଦି (lua_getmetatable(L, ud)) // does it have a metatable?
    {
	luaV_getfield(L, tname); // get metatable
	ଯଦି (lua_rawequal(L, -1, -2)) // MTs match?
	{
	    lua_କାଢ଼(L, 2); // MTs
	    ଫେରନ୍ତୁ p;
	}
    }

    ଫେରନ୍ତୁ ଖାଲି;
}

    ସ୍ଥିର ଶୂନ୍ୟ *
luaV_checkcache(lua_State *L, ଶୂନ୍ୟ *p)
{
    luaV_getudata(L, p);
    ଯଦି (lua_isnil(L, -1)) luaL_error(L, "invalid object");
    lua_କାଢ଼(L, 1);
    ଫେରନ୍ତୁ p;
}

# define luaV_unbox(L,luatyp,ud) (*((luatyp *) lua_touserdata((L),(ud))))

# define luaV_checkvalid(L,luatyp,ud) \
    luaV_checkcache((L), (ଶୂନ୍ୟ *) luaV_unbox((L),luatyp,(ud)))

    ସ୍ଥିର ଶୂନ୍ୟ *
luaV_checkudata(lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ud, ଅଚଳ ଅକ୍ଷର *tname)
{
    ଶୂନ୍ୟ *p = luaV_toudata(L, ud, tname);
    ଯଦି (p == ଖାଲି) luaL_typeerror(L, ud, tname);
    ଫେରନ୍ତୁ p;
}

    ସ୍ଥିର ଶୂନ୍ୟ
luaV_pushtypval(lua_State *L, typval_T *tv)
{
    ଯଦି (tv == ଖାଲି)
    {
	lua_pushnil(L);
	ଫେରନ୍ତୁ;
    }
    ବାଛ (tv->v_type)
    {
	ସ୍ଥିତି VAR_STRING:
	    lua_pushstring(L, tv->vval.v_string == ଖାଲି
					    ? "" : (ଅକ୍ଷର *)tv->vval.v_string);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	    lua_pushinteger(L, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) tv->vval.v_ସଂଖ୍ୟା);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_FLOAT:
	    lua_pushnumber(L, (lua_ସଂଖ୍ୟା) tv->vval.v_float);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_LIST:
	    luaV_pushlist(L, tv->vval.v_list);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_DICT:
	    luaV_pushdict(L, tv->vval.v_dict);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_BOOL:
	ସ୍ଥିତି VAR_SPECIAL:
	    ଯଦି (tv->vval.v_ସଂଖ୍ୟା <= VVAL_TRUE)
		lua_pushboolean(L, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) tv->vval.v_ସଂଖ୍ୟା);
	    ଅନ୍ୟଥା
		lua_pushnil(L);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_FUNC:
	    luaV_pushfuncref(L, tv->vval.v_string);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_PARTIAL:
	    // TODO: handle partial arguments
	    luaV_pushfuncref(L, partial_name(tv->vval.v_partial));
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_BLOB:
	    luaV_pushblob(L, tv->vval.v_blob);
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    lua_pushnil(L);
    }
}

/*
 * Converts lua value at 'pos' to typval 'tv'.
 * Returns OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_totypval(lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା pos, typval_T *tv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା status = OK;

    tv->v_lock = 0;

    ବାଛ (lua_type(L, pos))
    {
	ସ୍ଥିତି LUA_TBOOLEAN:
	    tv->v_type = VAR_BOOL;
	    tv->vval.v_ସଂଖ୍ୟା = (varnumber_T) lua_toboolean(L, pos);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି LUA_TNIL:
	    tv->v_type = VAR_SPECIAL;
	    tv->vval.v_ସଂଖ୍ୟା = VVAL_NULL;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି LUA_TSTRING:
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strsave((char_u *) lua_tostring(L, pos));
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି LUA_TNUMBER:
	{
	    ଅଚଳ lua_ସଂଖ୍ୟା n = lua_tonumber(L, pos);

	    ଯଦି (n > (lua_ସଂଖ୍ୟା)INT64_MAX || n < (lua_ସଂଖ୍ୟା)INT64_MIN
		    || ((lua_ସଂଖ୍ୟା)((varnumber_T)n)) != n)
	    {
		tv->v_type = VAR_FLOAT;
		tv->vval.v_float = (float_T)n;
	    }
	    ଅନ୍ୟଥା
	    {
		tv->v_type = VAR_ସଂଖ୍ୟା;
		tv->vval.v_ସଂଖ୍ୟା = (varnumber_T)n;
	    }
	}
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି LUA_TFUNCTION:
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
	    ଭାଙ୍ଗ;
	}
	ସ୍ଥିତି LUA_TTABLE:
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lua_tableref;

	    lua_pushvalue(L, pos);
	    lua_tableref = luaL_ref(L, LUA_REGISTRYINDEX);
	    ଯଦି (lua_getmetatable(L, pos))
	    {
		lua_getfield(L, -1, LUA___CALL);
		ଯଦି (lua_isfunction(L, -1))
		{
		    char_u *name;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା lua_funcref = luaL_ref(L, LUA_REGISTRYINDEX);
		    luaV_CFuncState *state = ALLOC_CLEAR_ONE(luaV_CFuncState);

		    state->lua_funcref = lua_funcref;
		    state->L = L;
		    state->lua_tableref = lua_tableref;
		    name = register_cfunc(&luaV_call_lua_func,
					      &luaV_call_lua_func_free, state);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(name);
		    ଭାଙ୍ଗ;
		}
	    }
	    tv->v_type = VAR_ସଂଖ୍ୟା;
	    tv->vval.v_ସଂଖ୍ୟା = 0;
	    status = FAIL;
	    ଭାଙ୍ଗ;
	}
	ସ୍ଥିତି LUA_TUSERDATA:
	{
	    ଶୂନ୍ୟ *p = lua_touserdata(L, pos);

	    ଯଦି (lua_getmetatable(L, pos)) // has metatable?
	    {
		// check list
		luaV_getfield(L, LUAVIM_LIST);
		ଯଦି (lua_rawequal(L, -1, -2))
		{
		    tv->v_type = VAR_LIST;
		    tv->vval.v_list = *((luaV_List *) p);
		    ++tv->vval.v_list->lv_refcount;
		    lua_କାଢ଼(L, 2); // MTs
		    ଭାଙ୍ଗ;
		}
		// check dict
		luaV_getfield(L, LUAVIM_DICT);
		ଯଦି (lua_rawequal(L, -1, -3))
		{
		    tv->v_type = VAR_DICT;
		    tv->vval.v_dict = *((luaV_Dict *) p);
		    ++tv->vval.v_dict->dv_refcount;
		    lua_କାଢ଼(L, 3); // MTs
		    ଭାଙ୍ଗ;
		}
		// check blob
		luaV_getfield(L, LUAVIM_BLOB);
		ଯଦି (lua_rawequal(L, -1, -4))
		{
		    tv->v_type = VAR_BLOB;
		    tv->vval.v_blob = *((luaV_Blob *) p);
		    ++tv->vval.v_blob->bv_refcount;
		    lua_କାଢ଼(L, 4); // MTs
		    ଭାଙ୍ଗ;
		}
		// check funcref
		luaV_getfield(L, LUAVIM_FUNCREF);
		ଯଦି (lua_rawequal(L, -1, -5))
		{
		    luaV_Funcref *f = (luaV_Funcref *) p;

		    func_ref(f->name);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(f->name);
		    lua_କାଢ଼(L, 5); // MTs
		    ଭାଙ୍ଗ;
		}
		lua_କାଢ଼(L, 4); // MTs
	    }
	}
	// FALLTHROUGH
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    tv->v_type = VAR_ସଂଖ୍ୟା;
	    tv->vval.v_ସଂଖ୍ୟା = 0;
	    status = FAIL;
    }
    ଫେରନ୍ତୁ status;
}

/*
 * similar to luaL_addlstring, but replaces \0 with \n if toline and
 * \n with \0 otherwise
 */
    ସ୍ଥିର ଶୂନ୍ୟ
luaV_addlstring(luaL_Buffer *b, ଅଚଳ ଅକ୍ଷର *s, size_t l, ପୂର୍ଣ୍ଣସଂଖ୍ୟା toline)
{
    ଯେତେବେଳେ (l--)
    {
	ଯଦି (*s == '\0' && toline)
	    luaL_addchar(b, '\n');
	ଅନ୍ୟଥା ଯଦି (*s == '\n' && !toline)
	    luaL_addchar(b, '\0');
	ଅନ୍ୟଥା
	    luaL_addchar(b, *s);
	s++;
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
luaV_pushline(lua_State *L, buf_T *buf, linenr_T n)
{
    ଅଚଳ ଅକ୍ଷର *s = (ଅଚଳ ଅକ୍ଷର *) ml_get_buf(buf, n, FALSE);
    luaL_Buffer b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, strlen(s), 0);
    luaL_pushresult(&b);
}

    ସ୍ଥିର char_u *
luaV_toline(lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା pos)
{
    size_t l;
    ଅଚଳ ଅକ୍ଷର *s = lua_tolstring(L, pos, &l);

    luaL_Buffer b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 1);
    luaL_pushresult(&b);
    ଫେରନ୍ତୁ (char_u *) lua_tostring(L, -1);
}

/*
 * pops a string s from the top of the stack and calls mf(t) for pieces t of
 * s separated by newlines
 */
    ସ୍ଥିର ଶୂନ୍ୟ
luaV_msgfunc(lua_State *L, msgfunc_T mf)
{
    luaL_Buffer b;
    size_t l;
    ଅଚଳ ଅକ୍ଷର *p, *s = lua_tolstring(L, -1, &l);
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 0);
    luaL_pushresult(&b);
    // break string
    p = s = lua_tolstring(L, -1, &l);
    ଯେତେବେଳେ (l--)
    {
	ଯଦି (*p++ == '\0') // break?
	{
	    mf((ଅକ୍ଷର *)s);
	    s = p;
	}
    }
    mf((ଅକ୍ଷର *)s);
    lua_କାଢ଼(L, 2); // original and modified strings
}

# define luaV_newtype(typ,tname,luatyp,luatname) \
	ସ୍ଥିର luatyp * \
    luaV_new##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = (luatyp *) lua_newuserdata(L, ଆକାର(luatyp)); \
	*o = obj; \
	luaV_setudata(L, obj); /* cache[obj] = udata */ \
	luaV_getfield(L, luatname); \
	lua_setmetatable(L, -2); \
	ଫେରନ୍ତୁ o; \
    }

# define luaV_pushtype(typ,tname,luatyp) \
	ସ୍ଥିର luatyp * \
    lua_V_ଠେଲ##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = ଖାଲି; \
	ଯଦି (obj == ଖାଲି) \
	    lua_pushnil(L); \
	ଅନ୍ୟଥା \
	{ \
	    luaV_getudata(L, obj); \
	    ଯଦି (lua_isnil(L, -1)) /* not interned? */ \
	    { \
		lua_କାଢ଼(L, 1); \
		o = luaV_new##tname(L, obj); \
	    } \
	    ଅନ୍ୟଥା \
		o = (luatyp *) lua_touserdata(L, -1); \
	} \
	ଫେରନ୍ତୁ o; \
    }

# define luaV_type_tostring(tname,luatname) \
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା \
    luaV_##tname##_tostring(lua_State *L) \
    { \
	lua_pushfstring(L, "%s: %p", luatname, lua_touserdata(L, 1)); \
	ଫେରନ୍ତୁ 1; \
    }

// =======   List type   =======

    ସ୍ଥିର luaV_List *
luaV_newlist(lua_State *L, list_T *lis)
{
    luaV_List *l = (luaV_List *) lua_newuserdata(L, ଆକାର(luaV_List));
    *l = lis;
    lis->lv_refcount++; // reference in Lua
    luaV_setudata(L, lis); // cache[lis] = udata
    luaV_getfield(L, LUAVIM_LIST);
    lua_setmetatable(L, -2);
    ଫେରନ୍ତୁ l;
}

luaV_pushtype(list_T, list, luaV_List)
luaV_type_tostring(list, LUAVIM_LIST)

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_list_len(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushinteger(L, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) list_len(l));
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_list_iter(lua_State *L)
{
    listitem_T *li = (listitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    ଯଦି (li == ଖାଲି) ଫେରନ୍ତୁ 0;
    luaV_pushtypval(L, &li->li_tv);
    lua_pushlightuserdata(L, (ଶୂନ୍ୟ *) li->li_next);
    lua_replace(L, lua_upvalueindex(1));
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_list_call(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushlightuserdata(L, (ଶୂନ୍ୟ *) l->lv_first);
    lua_pushcclosure(L, luaV_list_iter, 1);
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_list_index(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    ଯଦି (lua_isnumber(L, 2)) // list item?
    {
	long n = (long) luaL_checkinteger(L, 2);
	listitem_T *li;

	// Lua array index starts with 1 while Vim uses 0, subtract 1 to
	// normalize.
	n -= 1;
	li = list_find(l, n);
	ଯଦି (li == ଖାଲି)
	    lua_pushnil(L);
	ଅନ୍ୟଥା
	    luaV_pushtypval(L, &li->li_tv);
    }
    ଅନ୍ୟଥା ଯଦି (lua_isstring(L, 2)) // method?
    {
	ଅଚଳ ଅକ୍ଷର *s = lua_tostring(L, 2);
	ଯଦି (strncmp(s, "add", 3) == 0
		|| strncmp(s, "insert", 6) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	ଅନ୍ୟଥା
	    lua_pushnil(L);
    }
    ଅନ୍ୟଥା
	lua_pushnil(L);
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_list_newindex(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    long n = (long) luaL_checkinteger(L, 2);
    listitem_T *li;

    // Lua array index starts with 1 while Vim uses 0, subtract 1 to normalize.
    n -= 1;

    ଯଦି (l->lv_lock)
	luaL_error(L, "list is locked");
    li = list_find(l, n);
    ଯଦି (li == ଖାଲି)
    {
	ଯଦି (!lua_isnil(L, 3))
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "inserting list item");
	    ଯଦି (list_ଭର୍ତ୍ତି_tv(l, &v, li) == FAIL)
		luaL_error(L, "failed to add item to list");
	    clear_tv(&v);
	}
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (lua_isnil(L, 3)) // remove?
	{
	    vimlist_remove(l, li, li);
	    listitem_free(l, li);
	}
	ଅନ୍ୟଥା
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "setting list item");
	    clear_tv(&li->li_tv);
	    li->li_tv = v;
	}
    }
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_list_add(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (ଶୂନ୍ୟ *) *lis);
    typval_T v;
    ଯଦି (l->lv_lock)
	luaL_error(L, "list is locked");
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "adding list item");
    ଯଦି (list_append_tv(l, &v) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
lua_V_list_ଭର୍ତ୍ତି(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (ଶୂନ୍ୟ *) *lis);
    long pos = (long) luaL_optinteger(L, 3, 0);
    listitem_T *li = ଖାଲି;
    typval_T v;
    ଯଦି (l->lv_lock)
	luaL_error(L, "list is locked");
    ଯଦି (pos < l->lv_len)
    {
	li = list_find(l, pos);
	ଯଦି (li == ଖାଲି)
	    luaL_error(L, "invalid position");
    }
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "inserting list item");
    ଯଦି (list_ଭର୍ତ୍ତି_tv(l, &v, li) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ଅଚଳ luaL_Reg luaV_List_mt[] = {
    {"__tostring", luaV_list_tostring},
    {"__len", luaV_list_len},
    {"__call", luaV_list_call},
    {"__index", luaV_list_index},
    {"__newindex", luaV_list_newindex},
    {"add", luaV_list_add},
    {"insert", lua_V_list_ଭର୍ତ୍ତି},
    {ଖାଲି, ଖାଲି}
};


// =======   Dict type   =======

    ସ୍ଥିର luaV_Dict *
luaV_newdict(lua_State *L, dict_T *dic)
{
    luaV_Dict *d = (luaV_Dict *) lua_newuserdata(L, ଆକାର(luaV_Dict));
    *d = dic;
    dic->dv_refcount++; // reference in Lua
    luaV_setudata(L, dic); // cache[dic] = udata
    luaV_getfield(L, LUAVIM_DICT);
    lua_setmetatable(L, -2);
    ଫେରନ୍ତୁ d;
}

luaV_pushtype(dict_T, dict, luaV_Dict)
luaV_type_tostring(dict, LUAVIM_DICT)

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_dict_len(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    lua_pushinteger(L, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) dict_len(d));
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_dict_iter(lua_State *L UNUSED)
{
# ifdef FEAT_EVAL
    hashitem_T *hi = (hashitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = lua_tointeger(L, lua_upvalueindex(2));
    dictitem_T *di;
    ଯଦି (n <= 0) ଫେରନ୍ତୁ 0;
    ଯେତେବେଳେ (HASHITEM_EMPTY(hi)) hi++;
    di = dict_lookup(hi);
    lua_pushstring(L, (ଅକ୍ଷର *) hi->hi_ଚାବି);
    luaV_pushtypval(L, &di->di_tv);
    lua_pushlightuserdata(L, (ଶୂନ୍ୟ *) (hi + 1));
    lua_replace(L, lua_upvalueindex(1));
    lua_pushinteger(L, n - 1);
    lua_replace(L, lua_upvalueindex(2));
    ଫେରନ୍ତୁ 2;
# ଅନ୍ୟଥା
    ଫେରନ୍ତୁ 0;
# endif
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_dict_call(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    hashtab_T *ht = &d->dv_hashtab;
    lua_pushlightuserdata(L, (ଶୂନ୍ୟ *) ht->ht_array);
    lua_pushinteger(L, ht->ht_used); // # remaining items
    lua_pushcclosure(L, luaV_dict_iter, 2);
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_dict_index(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *ଚାବି = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di = dict_find(d, ଚାବି, -1);

    ଯଦି (di == ଖାଲି)
    {
	lua_pushnil(L);
	ଫେରନ୍ତୁ 1;
    }

    luaV_pushtypval(L, &di->di_tv);
    ଯଦି (di->di_tv.v_type == VAR_FUNC) // funcref?
    {
	luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, -1);
	f->self = d; // keep "self" reference
	d->dv_refcount++;
    }

    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_dict_newindex(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *ଚାବି = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di;
    typval_T tv;

    ଯଦି (d->dv_lock)
	luaL_error(L, "dict is locked");
    ଯଦି (ଚାବି == ଖାଲି)
	ଫେରନ୍ତୁ 0;
    ଯଦି (*ଚାବି == NUL)
	luaL_error(L, "empty key");
    ଯଦି (!lua_isnil(L, 3)) // read value?
    {
	luaV_checktypval(L, 3, &tv, "setting dict item");
	ଯଦି (d->dv_scope == VAR_DEF_SCOPE && tv.v_type == VAR_FUNC)
	{
	    clear_tv(&tv);
	    luaL_error(L, "cannot assign funcref to builtin scope");
	}
    }
    di = dict_find(d, ଚାବି, -1);
    ଯଦି (di == ଖାଲି) // non-existing key?
    {
	ଯଦି (lua_isnil(L, 3))
	    ଫେରନ୍ତୁ 0;
	di = dictitem_alloc(ଚାବି);
	ଯଦି (di == ଖାଲି)
	{
	    clear_tv(&tv);
	    ଫେରନ୍ତୁ 0;
	}
	ଯଦି (dict_add(d, di) == FAIL)
	{
	    vim_free(di);
	    clear_tv(&tv);
	    ଫେରନ୍ତୁ 0;
	}
    }
    ଅନ୍ୟଥା
	clear_tv(&di->di_tv);
    ଯଦି (lua_isnil(L, 3)) // remove?
    {
	hashitem_T *hi = hash_find(&d->dv_hashtab, di->di_ଚାବି);
	hash_remove(&d->dv_hashtab, hi, "Lua new index");
	dictitem_free(di);
    }
    ଅନ୍ୟଥା
	di->di_tv = tv;
    ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅଚଳ luaL_Reg luaV_Dict_mt[] = {
    {"__tostring", luaV_dict_tostring},
    {"__len", luaV_dict_len},
    {"__call", luaV_dict_call},
    {"__index", luaV_dict_index},
    {"__newindex", luaV_dict_newindex},
    {ଖାଲି, ଖାଲି}
};


// =======   Blob type   =======

    ସ୍ଥିର luaV_Blob *
luaV_newblob(lua_State *L, blob_T *blo)
{
    luaV_Blob *b = (luaV_Blob *) lua_newuserdata(L, ଆକାର(luaV_Blob));
    *b = blo;
    blo->bv_refcount++; // reference in Lua
    luaV_setudata(L, blo); // cache[blo] = udata
    luaV_getfield(L, LUAVIM_BLOB);
    lua_setmetatable(L, -2);
    ଫେରନ୍ତୁ b;
}

luaV_pushtype(blob_T, blob, luaV_Blob)
luaV_type_tostring(blob, LUAVIM_BLOB)

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_blob_gc(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    blob_unref(b);
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_blob_len(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    lua_pushinteger(L, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) blob_len(b));
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_blob_index(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    ଯଦି (lua_isnumber(L, 2))
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = luaL_checkinteger(L, 2);
	ଯଦି (idx < 0)
	    // Negative index is relative to the end.
	    idx += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) blob_len(b);
	ଯଦି (idx >= 0 && idx < blob_len(b))
	    lua_pushnumber(L, (lua_ସଂଖ୍ୟା) blob_get(b, idx));
	ଅନ୍ୟଥା
	    lua_pushnil(L);
    }
    ଅନ୍ୟଥା ଯଦି (lua_isstring(L, 2))
    {
	ଅଚଳ ଅକ୍ଷର *s = lua_tostring(L, 2);
	ଯଦି (strncmp(s, "add", 3) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	ଅନ୍ୟଥା
	    lua_pushnil(L);
    }
    ଅନ୍ୟଥା
	lua_pushnil(L);
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_blob_newindex(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    ଯଦି (b->bv_lock)
	luaL_error(L, "blob is locked");

    ଯଦି (!lua_isnumber(L, 2))
	ଫେରନ୍ତୁ 0;

    long len = blob_len(b);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = luaL_checkinteger(L, 2);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା val = luaL_checkinteger(L, 3);
    ଯଦି (idx < 0)
	// Negative index is relative to the end.
	idx += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) len;
    ଯଦି ((idx >= 0 && idx < len)
	    || (idx == len && ga_grow(&b->bv_ga, 1) == OK))
    {
	blob_set(b, idx, (char_u) val);
	ଯଦି (idx == len)
	    ++b->bv_ga.ga_len;
    }
    ଅନ୍ୟଥା
	luaL_error(L, "index out of range");

    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_blob_add(lua_State *L)
{
    luaV_Blob *blo = luaV_checkudata(L, 1, LUAVIM_BLOB);
    blob_T *b = (blob_T *) luaV_checkcache(L, (ଶୂନ୍ୟ *) *blo);
    ଯଦି (b->bv_lock)
	luaL_error(L, "blob is locked");
    lua_settop(L, 2);
    ଯଦି (!lua_isstring(L, 2))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 2));
    ଅନ୍ୟଥା
    {
	size_t i, l = 0;
	ଅଚଳ ଅକ୍ଷର *s = lua_tolstring(L, 2, &l);

	ଯଦି (ga_grow(&b->bv_ga, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)l) == OK)
	    ପାଇଁ (i = 0; i < l; ++i)
		ga_append(&b->bv_ga, s[i]);
    }
    lua_settop(L, 1);
    ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ଅଚଳ luaL_Reg luaV_Blob_mt[] = {
    {"__tostring", luaV_blob_tostring},
    {"__gc", luaV_blob_gc},
    {"__len", luaV_blob_len},
    {"__index", luaV_blob_index},
    {"__newindex", luaV_blob_newindex},
    {"add", luaV_blob_add},
    {ଖାଲି, ଖାଲି}
};


// =======   Funcref type   =======

    ସ୍ଥିର luaV_Funcref *
luaV_newfuncref(lua_State *L, char_u *name)
{
    luaV_Funcref *f = (luaV_Funcref *)lua_newuserdata(L, ଆକାର(luaV_Funcref));

    ଯଦି (name != ଖାଲି)
    {
	func_ref(name);
	f->name = vim_strsave(name);
    }
    f->self = ଖାଲି;
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_setmetatable(L, -2);
    ଫେରନ୍ତୁ f;
}

    ସ୍ଥିର luaV_Funcref *
luaV_pushfuncref(lua_State *L, char_u *name)
{
    ଫେରନ୍ତୁ luaV_newfuncref(L, name);
}


luaV_type_tostring(funcref, LUAVIM_FUNCREF)

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_funcref_gc(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    func_unref(f->name);
    vim_free(f->name);
    // NOTE: Don't call "dict_unref(f->self)", because the dict of "f->self"
    // will be (or has been already) freed by Vim's garbage collection.
    ଫେରନ୍ତୁ 0;
}

// equivalent to string(funcref)
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_funcref_len(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    lua_pushstring(L, (ଅଚଳ ଅକ୍ଷର *) f->name);
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_funcref_call(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i, n = lua_gettop(L) - 1; // #args
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା status = FAIL;
    typval_T args;
    typval_T rettv;

    args.v_type = VAR_LIST;
    args.vval.v_list = list_alloc();
    rettv.v_type = VAR_UNKNOWN; // as in clear_tv
    ଯଦି (args.vval.v_list != ଖାଲି)
    {
	typval_T v;

	ପାଇଁ (i = 0; i < n; i++)
	{
	    luaV_checktypval(L, i + 2, &v, "calling funcref");
	    list_append_tv(args.vval.v_list, &v);
	    clear_tv(&v);
	}
	status = func_call(f->name, &args, ଖାଲି, f->self, &rettv);
	ଯଦି (status == OK)
	    luaV_pushtypval(L, &rettv);
	clear_tv(&args);
	clear_tv(&rettv);
    }
    ଯଦି (status != OK)
	luaL_error(L, "cannot call funcref");
    ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ଅଚଳ luaL_Reg luaV_Funcref_mt[] = {
    {"__tostring", luaV_funcref_tostring},
    {"__gc", luaV_funcref_gc},
    {"__len", luaV_funcref_len},
    {"__call", luaV_funcref_call},
    {ଖାଲି, ଖାଲି}
};


// =======   Buffer type   =======

luaV_newtype(buf_T, buffer, luaV_Buffer, LUAVIM_BUFFER)
luaV_pushtype(buf_T, buffer, luaV_Buffer)
luaV_type_tostring(buffer, LUAVIM_BUFFER)

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_buffer_len(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    lua_pushinteger(L, b->b_ml.ml_ଧାଡ଼ି_ଗଣନା);
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_buffer_call(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    lua_settop(L, 1);
    set_curbuf(b, DOBUF_SPLIT);
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_buffer_index(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    linenr_T n = (linenr_T) lua_tointeger(L, 2);
    ଯଦି (n > 0 && n <= b->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	luaV_pushline(L, b, n);
    ଅନ୍ୟଥା ଯଦି (lua_isstring(L, 2))
    {
	ଅଚଳ ଅକ୍ଷର *s = lua_tostring(L, 2);
	ଯଦି (strncmp(s, "name", 4) == 0)
	    lua_pushstring(L, (b->b_sfname == ଖାଲି)
					? "" : (ଅକ୍ଷର *) b->b_sfname);
	ଅନ୍ୟଥା ଯଦି (strncmp(s, "fname", 5) == 0)
	    lua_pushstring(L, (b->b_ffname == ଖାଲି)
					? "" : (ଅକ୍ଷର *) b->b_ffname);
	ଅନ୍ୟଥା ଯଦି (strncmp(s, "number", 6) == 0)
	    lua_pushinteger(L, b->b_fnum);
	// methods
	ଅନ୍ୟଥା ଯଦି (strncmp(s,   "insert", 6) == 0
		|| strncmp(s, "next", 4) == 0
		|| strncmp(s, "previous", 8) == 0
		|| strncmp(s, "isvalid", 7) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	ଅନ୍ୟଥା
	    lua_pushnil(L);
    }
    ଅନ୍ୟଥା
	lua_pushnil(L);
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_buffer_newindex(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    linenr_T n = (linenr_T) luaL_checkinteger(L, 2);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    ଯଦି (n < 1 || n > b->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	luaL_error(L, "invalid line number");
    ଯଦି (lua_isnil(L, 3)) // delete line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	ଯଦି (u_savedel(n, 1L) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	ଅନ୍ୟଥା ଯଦି (ml_delete(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot delete line");
	}
	ଅନ୍ୟଥା
	{
	    deleted_ଧାଡ଼ିଗୁଡ଼ିକ_mark(n, 1L);
	    ଯଦି (b == curwin->w_buffer) // fix cursor in current window?
	    {
		ଯଦି (curwin->w_cursor.lnum >= n)
		{
		    ଯଦି (curwin->w_cursor.lnum > n)
		    {
			curwin->w_cursor.lnum -= 1;
			check_cursor_col();
		    }
		    ଅନ୍ୟଥା
			check_cursor();
		    changed_cline_bef_curs();
		}
		invalidate_botline();
	    }
	}
	curbuf = buf;
    }
    ଅନ୍ୟଥା ଯଦି (lua_isstring(L, 3)) // update line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	ଯଦି (u_savesub(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	ଅନ୍ୟଥା ଯଦି (ml_replace(n, luaV_toline(L, 3), TRUE) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot replace line");
	}
	ଅନ୍ୟଥା
	    changed_bytes(n, 0);
	curbuf = buf;
	ଯଦି (b == curwin->w_buffer)
	    check_cursor_col();
    }
    ଅନ୍ୟଥା
	luaL_error(L, "wrong argument to change line");
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
lua_V_buffer_ଭର୍ତ୍ତି(lua_State *L)
{
    luaV_Buffer *lb = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *b = (buf_T *) luaV_checkcache(L, (ଶୂନ୍ୟ *) *lb);
    linenr_T last = b->b_ml.ml_ଧାଡ଼ି_ଗଣନା;
    linenr_T n = (linenr_T) luaL_optinteger(L, 3, last);
    buf_T *buf;
    luaL_checktype(L, 2, LUA_TSTRING);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    // fix insertion line
    ଯଦି (n < 0) n = 0;
    ଯଦି (n > last) n = last;
    // insert
    buf = curbuf;
    curbuf = b;
    ଯଦି (u_save(n, n + 1) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot save undo information");
    }
    ଅନ୍ୟଥା ଯଦି (ml_append(n, luaV_toline(L, 2), 0, FALSE) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot insert line");
    }
    ଅନ୍ୟଥା
	appended_ଧାଡ଼ିଗୁଡ଼ିକ_mark(n, 1L);
    curbuf = buf;
    update_screen(UPD_VALID);
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_buffer_next(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (ଶୂନ୍ୟ *) *b);
    luaV_pushbuffer(L, buf->b_next);
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_buffer_previous(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (ଶୂନ୍ୟ *) *b);
    luaV_pushbuffer(L, buf->b_prev);
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_buffer_isvalid(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    luaV_getudata(L, *b);
    lua_pushboolean(L, !lua_isnil(L, -1));
    ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ଅଚଳ luaL_Reg luaV_Buffer_mt[] = {
    {"__tostring", luaV_buffer_tostring},
    {"__len", luaV_buffer_len},
    {"__call", luaV_buffer_call},
    {"__index", luaV_buffer_index},
    {"__newindex", luaV_buffer_newindex},
    {"insert", lua_V_buffer_ଭର୍ତ୍ତି},
    {"next", luaV_buffer_next},
    {"previous", luaV_buffer_previous},
    {"isvalid", luaV_buffer_isvalid},
    {ଖାଲି, ଖାଲି}
};


// =======   Window type   =======

luaV_newtype(win_T, window, luaV_Window, LUAVIM_WINDOW)
luaV_pushtype(win_T, window, luaV_Window)
luaV_type_tostring(window, LUAVIM_WINDOW)

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_window_call(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    lua_settop(L, 1);
    win_goto(w);
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_window_index(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    ଅଚଳ ଅକ୍ଷର *s = luaL_checkstring(L, 2);
    ଯଦି (strncmp(s, "buffer", 6) == 0)
	luaV_pushbuffer(L, w->w_buffer);
    ଅନ୍ୟଥା ଯଦି (strncmp(s, "line", 4) == 0)
	lua_pushinteger(L, w->w_cursor.lnum);
    ଅନ୍ୟଥା ଯଦି (strncmp(s, "col", 3) == 0)
	lua_pushinteger(L, w->w_cursor.col + 1);
    ଅନ୍ୟଥା ଯଦି (strncmp(s, "width", 5) == 0)
	lua_pushinteger(L, w->w_width);
    ଅନ୍ୟଥା ଯଦି (strncmp(s, "height", 6) == 0)
	lua_pushinteger(L, w->w_height);
    // methods
    ଅନ୍ୟଥା ଯଦି (strncmp(s,   "next", 4) == 0
	    || strncmp(s, "previous", 8) == 0
	    || strncmp(s, "isvalid", 7) == 0)
    {
	lua_getmetatable(L, 1);
	lua_getfield(L, -1, s);
    }
    ଅନ୍ୟଥା
	lua_pushnil(L);
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_window_newindex(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    ଅଚଳ ଅକ୍ଷର *s = luaL_checkstring(L, 2);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା v = luaL_checkinteger(L, 3);
    ଯଦି (strncmp(s, "line", 4) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	ଯଦି (v < 1 || v > w->w_buffer->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    luaL_error(L, "line out of range");
	w->w_cursor.lnum = v;
	update_screen(UPD_VALID);
    }
    ଅନ୍ୟଥା ଯଦି (strncmp(s, "col", 3) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	w->w_cursor.col = v - 1;
	w->w_set_curswant = true;
	update_screen(UPD_VALID);
    }
    ଅନ୍ୟଥା ଯଦି (strncmp(s, "width", 5) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setwidth(v);
	curwin = win;
    }
    ଅନ୍ୟଥା ଯଦି (strncmp(s, "height", 6) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setheight(v);
	curwin = win;
    }
    ଅନ୍ୟଥା
	luaL_error(L, "invalid window property: `%s'", s);
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_window_next(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    win_T *win = (win_T *) luaV_checkcache(L, (ଶୂନ୍ୟ *) *w);
    luaV_pushwindow(L, win->w_next);
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_window_previous(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    win_T *win = (win_T *) luaV_checkcache(L, (ଶୂନ୍ୟ *) *w);
    luaV_pushwindow(L, win->w_prev);
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_window_isvalid(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    luaV_getudata(L, *w);
    lua_pushboolean(L, !lua_isnil(L, -1));
    ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ଅଚଳ luaL_Reg luaV_Window_mt[] = {
    {"__tostring", luaV_window_tostring},
    {"__call", luaV_window_call},
    {"__index", luaV_window_index},
    {"__newindex", luaV_window_newindex},
    {"next", luaV_window_next},
    {"previous", luaV_window_previous},
    {"isvalid", luaV_window_isvalid},
    {ଖାଲି, ଖାଲି}
};


// =======   Vim module   =======

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_print(lua_State *L)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i, n = lua_gettop(L); // nargs
    ଅଚଳ ଅକ୍ଷର	*s;
    size_t	l;
    garray_T	ସନ୍ଦେଶ_ga;

    ga_init2(&ସନ୍ଦେଶ_ga, 1, 128);
    lua_getglobal(L, "tostring");
    ପାଇଁ (i = 1; i <= n; i++)
    {
	lua_pushvalue(L, -1); // tostring
	lua_pushvalue(L, i); // arg
	lua_call(L, 1, 1);
	s = lua_tolstring(L, -1, &l);
	ଯଦି (s == ଖାଲି)
	    ଫେରନ୍ତୁ luaL_error(L, "cannot convert to string");
	ଯଦି (i > 1)
	    ga_append(&ସନ୍ଦେଶ_ga, ' '); // use space instead of tab
	ga_concat_len(&ସନ୍ଦେଶ_ga, (char_u *)s, l);
	lua_କାଢ଼(L, 1);
    }
    // Replace any "\n" with "\0"
    ପାଇଁ (i = 0; i < ସନ୍ଦେଶ_ga.ga_len; i++)
	ଯଦି (((ଅକ୍ଷର *)ସନ୍ଦେଶ_ga.ga_data)[i] == '\n')
	    ((ଅକ୍ଷର *)ସନ୍ଦେଶ_ga.ga_data)[i] = '\0';
    lua_pushlstring(L, ସନ୍ଦେଶ_ga.ga_data, ସନ୍ଦେଶ_ga.ga_len);
    ଯଦି (!got_int)
	lua_V_ସନ୍ଦେଶ(L);

    ga_clear(&ସନ୍ଦେଶ_ga);
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_debug(lua_State *L)
{
    lua_settop(L, 0);
    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "eval");
    lua_remove(L, -2); // vim.eval at position 1
    ପାଇଁ (;;)
    {
	ଅଚଳ ଅକ୍ଷର *ନିବେଶ;
	size_t l;
	lua_pushvalue(L, 1); // vim.eval
	lua_pushliteral(L, "input('lua_debug> ')");
	lua_call(L, 1, 1); // return string
	ନିବେଶ = lua_tolstring(L, -1, &l);
	ଯଦି (l == 0 || strcmp(ନିବେଶ, "cont") == 0)
	    ଫେରନ୍ତୁ 0;
	ସନ୍ଦେଶ_putchar('\n'); // avoid outputting on input line
	ଯଦି (luaL_loadbuffer(L, ନିବେଶ, l, "=(debug command)")
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
	lua_settop(L, 1); // remove eventual returns, but keep vim.eval
    }
}

    ସ୍ଥିର dict_T *
luaV_get_var_scope(lua_State *L)
{
    ଅଚଳ ଅକ୍ଷର	*scope = luaL_checkstring(L, 1);
    dict_T	*dict = ଖାଲି;

    ଯଦି (STRICMP((ଅକ୍ଷର *)scope, "g") == 0)
	dict = get_globvar_dict();
    ଅନ୍ୟଥା ଯଦି (STRICMP((ଅକ୍ଷର *)scope, "v") == 0)
	dict = get_vimvar_dict();
    ଅନ୍ୟଥା ଯଦି (STRICMP((ଅକ୍ଷର *)scope, "b") == 0)
	dict = curbuf->b_vars;
    ଅନ୍ୟଥା ଯଦି (STRICMP((ଅକ୍ଷର *)scope, "w") == 0)
	dict = curwin->w_vars;
    ଅନ୍ୟଥା ଯଦି (STRICMP((ଅକ୍ଷର *)scope, "t") == 0)
	dict = curtab->tp_vars;
    ଅନ୍ୟଥା
    {
	luaL_error(L, "invalid scope %s", scope);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ dict;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_setvar(lua_State *L)
{
    dict_T	*dict;
    dictitem_T	*di;
    size_t	len;
    ଅକ୍ଷର	*name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		del;
    ଅକ୍ଷର	*error = ଖାଲି;

    name = (ଅକ୍ଷର *)luaL_checklstring(L, 3, &len);
    del = (lua_gettop(L) < 4) || lua_isnil(L, 4);

    dict = luaV_get_var_scope(L);
    ଯଦି (dict == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    di = dict_find(dict, (char_u *)name, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len);
    ଯଦି (di != ଖାଲି)
    {
	ଯଦି (di->di_flags & DI_FLAGS_RO)
	    error = "variable is read-only";
	ଅନ୍ୟଥା ଯଦି (di->di_flags & DI_FLAGS_LOCK)
	    error = "variable is locked";
	ଅନ୍ୟଥା ଯଦି (del && di->di_flags & DI_FLAGS_FIX)
	    error = "variable is fixed";
	ଯଦି (error != ଖାଲି)
	    ଫେରନ୍ତୁ luaL_error(L, error);
    }
    ଅନ୍ୟଥା ଯଦି (dict->dv_lock)
	ଫେରନ୍ତୁ luaL_error(L, "Dictionary is locked");

    ଯଦି (del)
    {
	// Delete the key
	ଯଦି (di == ଖାଲି)
	    // Doesn't exist, nothing to do
	    ଫେରନ୍ତୁ 0;
	// Delete the entry
	dictitem_remove(dict, di, "Lua delete variable");
    }
    ଅନ୍ୟଥା
    {
	// Update the key
	typval_T	tv;

	// Convert the lua value to a Vim script type in the temporary variable
	lua_pushvalue(L, 4);
	ଯଦି (luaV_totypval(L, -1, &tv) == FAIL)
	    ଫେରନ୍ତୁ luaL_error(L, "Couldn't convert lua value");

	ଯଦି (di == ଖାଲି)
	{
	    // Need to create an entry
	    di = dictitem_alloc((char_u *)name);
	    ଯଦି (di == ଖାଲି)
	    {
		clear_tv(&tv);
		ଫେରନ୍ତୁ 0;
	    }
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	    ଯଦି (dict_add(dict, di) == FAIL)
	    {
		dictitem_free(di);
		clear_tv(&tv);
		ଫେରନ୍ତୁ luaL_error(L, "Couldn't add to dictionary");
	    }
	}
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା type_error = FALSE;
	    ଯଦି (dict == get_vimvar_dict()
	       && !before_set_vvar((char_u *)name, di, &tv, TRUE, &type_error))
	    {
		clear_tv(&tv);
		ଯଦି (type_error)
		    ଫେରନ୍ତୁ luaL_error(L,
				"Setting v:%s to value with wrong type", name);
		ଫେରନ୍ତୁ 0;
	    }
	    // Clear the old value
	    clear_tv(&di->di_tv);
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	}

	// Clear the temporary variable
	clear_tv(&tv);
    }

    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_getvar(lua_State *L)
{
    dict_T	*dict = luaV_get_var_scope(L);
    size_t	len;
    ଅଚଳ ଅକ୍ଷର	*name = luaL_checklstring(L, 3, &len);
    dictitem_T	*di = dict_find(dict, (char_u *)name, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len);

    ଯଦି (di == ଖାଲି)
	ଫେରନ୍ତୁ 0;  // nil

    luaV_pushtypval(L, &di->di_tv);
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_command(lua_State *L)
{
    char_u  *s = vim_strsave((char_u *)luaL_checkstring(L, 1));

    execute_cmds_from_string(s);
    vim_free(s);
    update_screen(UPD_VALID);
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_eval(lua_State *L)
{
    typval_T *tv = eval_ପଦସମଷ୍ଟି((char_u *) luaL_checkstring(L, 1), ଖାଲି);

    ଯଦି (tv == ଖାଲି) luaL_error(L, "invalid expression");
    luaV_pushtypval(L, tv);
    free_tv(tv);
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_beep(lua_State *L UNUSED)
{
    vim_beep(BO_LANG);
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
lua_V_ଧାଡ଼ି(lua_State *L)
{
    luaV_pushline(L, curbuf, curwin->w_cursor.lnum);
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_list(lua_State *L)
{
    list_T *l;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା initarg = !lua_isnoneornil(L, 1);

    ଯଦି (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    l = list_alloc();
    ଯଦି (l == ଖାଲି)
    {
	lua_pushnil(L);
	ଫେରନ୍ତୁ 1;
    }

    luaV_newlist(L, l);
    ଯଦି (!initarg)
	ଫେରନ୍ତୁ 1;

    // traverse table to init list
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା notnil, i = 0;
    typval_T v;
    କର
    {
	lua_rawgeti(L, 1, ++i);
	notnil = !lua_isnil(L, -1);
	ଯଦି (notnil)
	{
	    luaV_checktypval(L, -1, &v, "vim.list");
	    list_append_tv(l, &v);
	    clear_tv(&v);
	}
	lua_କାଢ଼(L, 1); //ମୂଲ୍ୟ
    } ଯେତେବେଳେ (notnil);

    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_dict(lua_State *L)
{
    dict_T *d;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା initarg = !lua_isnoneornil(L, 1);

    ଯଦି (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    d = dict_alloc();
    ଯଦି (d == ଖାଲି)
    {
	lua_pushnil(L);
	ଫେରନ୍ତୁ 1;
    }

    luaV_newdict(L, d);
    ଯଦି (!initarg)
	ଫେରନ୍ତୁ 1;

    // traverse table to init dict
    lua_pushnil(L);
    ଯେତେବେଳେ (lua_next(L, 1))
    {
	char_u *ଚାବି;
	dictitem_T *di;
	typval_T v;

	lua_pushvalue(L, -2); // dup key in case it's a number
	ଚାବି = (char_u *) lua_tostring(L, -1);
	ଯଦି (ଚାବି == ଖାଲି)
	{
	    lua_pushnil(L);
	    ଫେରନ୍ତୁ 1;
	}
	ଯଦି (*ଚାବି == NUL)
	    luaL_error(L, "table has empty key");
	luaV_checktypval(L, -2, &v, "vim.dict"); //ମୂଲ୍ୟ
	di = dictitem_alloc(ଚାବି);
	ଯଦି (di == ଖାଲି || dict_add(d, di) == FAIL)
	{
	    vim_free(di);
	    lua_pushnil(L);
	    ଫେରନ୍ତୁ 1;
	}
	di->di_tv = v;
	lua_କାଢ଼(L, 2); // key copy and value
    }

    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_blob(lua_State *L)
{
    blob_T *b;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା initarg = !lua_isnoneornil(L, 1);

    ଯଦି (initarg && !lua_isstring(L, 1))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 1));

    b = blob_alloc();
    ଯଦି (b == ଖାଲି)
    {
	lua_pushnil(L);
	ଫେରନ୍ତୁ 1;
    }

    luaV_newblob(L, b);
    ଯଦି (!initarg)
	ଫେରନ୍ତୁ 1;

    // traverse table to init blob
    size_t i, l = 0;
    ଅଚଳ ଅକ୍ଷର *s = lua_tolstring(L, 1, &l);

    ଯଦି (ga_grow(&b->bv_ga, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)l) == OK)
	ପାଇଁ (i = 0; i < l; ++i)
	    ga_append(&b->bv_ga, s[i]);

    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_funcref(lua_State *L)
{
    ଅଚଳ ଅକ୍ଷର *name = luaL_checkstring(L, 1);
    // note: not checking if function exists (needs function_exists)
    ଯଦି (name == ଖାଲି || *name == NUL || VIM_ISDIGIT(*name))
	luaL_error(L, "invalid function name: %s", name);
    luaV_newfuncref(L, (char_u *) name);
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_buffer(lua_State *L)
{
    buf_T *buf;
    ଯଦି (lua_isstring(L, 1)) // get by number or name?
    {
	ଯଦି (lua_isnumber(L, 1)) // by number?
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = lua_tointeger(L, 1);
	    FOR_ALL_BUFFERS(buf)
		ଯଦି (buf->b_fnum == n) ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା // by name
	{
	    size_t l;
	    ଅଚଳ ଅକ୍ଷର *s = lua_tolstring(L, 1, &l);
	    FOR_ALL_BUFFERS(buf)
	    {
		ଯଦି (buf->b_ffname == ଖାଲି || buf->b_sfname == ଖାଲି)
		{
		    ଯଦି (l == 0) ଭାଙ୍ଗ;
		}
		ଅନ୍ୟଥା ଯଦି (strncmp(s, (ଅକ୍ଷର *)buf->b_ffname, l) == 0
			|| strncmp(s, (ଅକ୍ଷର *)buf->b_sfname, l) == 0)
		    ଭାଙ୍ଗ;
	    }
	}
    }
    ଅନ୍ୟଥା
	buf = (lua_toboolean(L, 1)) ? firstbuf : curbuf; // first buffer?
    luaV_pushbuffer(L, buf);
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_window(lua_State *L)
{
    win_T *win;
    ଯଦି (lua_isnumber(L, 1)) // get by number?
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = lua_tointeger(L, 1);
	ପାଇଁ (win = firstwin; win != ଖାଲି; win = win->w_next, n--)
	    ଯଦି (n == 1) ଭାଙ୍ଗ;
    }
    ଅନ୍ୟଥା
	win = (lua_toboolean(L, 1)) ? firstwin : curwin; // first window?
    luaV_pushwindow(L, win);
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_open(lua_State *L)
{
    char_u *s = ଖାଲି;
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    ଯଦି (lua_isstring(L, 1)) s = (char_u *) lua_tostring(L, 1);
    luaV_pushbuffer(L, buflist_new(s, ଖାଲି, 1L, BLN_LISTED));
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_type(lua_State *L)
{
    luaL_checkany(L, 1);
    ଯଦି (lua_type(L, 1) == LUA_TUSERDATA) // check vim udata?
    {
	lua_settop(L, 1);
	ଯଦି (lua_getmetatable(L, 1))
	{
	    luaV_getfield(L, LUAVIM_LIST);
	    ଯଦି (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "list");
		ଫେରନ୍ତୁ 1;
	    }
	    luaV_getfield(L, LUAVIM_DICT);
	    ଯଦି (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "dict");
		ଫେରନ୍ତୁ 1;
	    }
	    luaV_getfield(L, LUAVIM_BLOB);
	    ଯଦି (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "blob");
		ଫେରନ୍ତୁ 1;
	    }
	    luaV_getfield(L, LUAVIM_FUNCREF);
	    ଯଦି (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "funcref");
		ଫେରନ୍ତୁ 1;
	    }
	    luaV_getfield(L, LUAVIM_BUFFER);
	    ଯଦି (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "buffer");
		ଫେରନ୍ତୁ 1;
	    }
	    luaV_getfield(L, LUAVIM_WINDOW);
	    ଯଦି (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "window");
		ଫେରନ୍ତୁ 1;
	    }
	}
    }
    lua_pushstring(L, luaL_typename(L, 1)); // fallback
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_call(lua_State *L)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argc = lua_gettop(L) - 1;
    size_t	funcname_len;
    char_u	*funcname;
    ଅକ୍ଷର	*error = ଖାଲି;
    typval_T	rettv;
    typval_T	argv[MAX_FUNC_ARGS + 1];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = 0;

    ଯଦି (argc > MAX_FUNC_ARGS)
	ଫେରନ୍ତୁ luaL_error(L, "Function called with too many arguments");

    funcname = (char_u *)luaL_checklstring(L, 1, &funcname_len);

    ପାଇଁ (; i < argc; i++)
    {
	ଯଦି (luaV_totypval(L, i + 2, &argv[i]) == FAIL)
	{
	    error = "lua: cannot convert value";
	    ଯାଅ free_vim_args;
	}
    }

    argv[argc].v_type = VAR_UNKNOWN;

    ଯଦି (call_vim_function(funcname, argc, argv, &rettv) == FAIL)
    {
	error = "lua: call_vim_function failed";
	ଯାଅ free_vim_args;
    }

    luaV_pushtypval(L, &rettv);
    clear_tv(&rettv);

free_vim_args:
    ଯେତେବେଳେ (i > 0)
	clear_tv(&argv[--i]);

    ଯଦି (error == ଖାଲି)
	ଫେରନ୍ତୁ 1;
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ luaL_error(L, error);
}

/*
 * Return the Vim version as a Lua table
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
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
    ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର ଅଚଳ luaL_Reg luaV_module[] = {
    {"command", luaV_command},
    {"eval", luaV_eval},
    {"beep", luaV_beep},
    {"line", lua_V_ଧାଡ଼ି},
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
    {"lua_version", ଖାଲି},
    {ଖାଲି, ଖାଲି}
};

/*
 * for freeing list, dict, buffer and window objects; lightuserdata as arg
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_free(lua_State *L)
{
    lua_pushnil(L);
    luaV_setudata(L, lua_touserdata(L, 1));
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_luaeval(lua_State *L)
{
    luaL_Buffer b;
    size_t l;
    ଅଚଳ ଅକ୍ଷର *str = lua_tolstring(L, 1, &l);
    typval_T *arg = (typval_T *) lua_touserdata(L, 2);
    typval_T *rettv = (typval_T *) lua_touserdata(L, 3);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, LUAVIM_EVALHEADER, ଆକାର(LUAVIM_EVALHEADER) - 1);
    luaL_addlstring(&b, str, l);
    luaL_pushresult(&b);
    str = lua_tolstring(L, -1, &l);
    ଯଦି (luaL_loadbuffer(L, str, l, LUAVIM_EVALNAME)) // compile error?
    {
	luaV_emsg(L);
	ଫେରନ୍ତୁ 0;
    }
    luaV_pushtypval(L, arg);
    ଯଦି (lua_pcall(L, 1, 1, 0)) // running error?
    {
	luaV_emsg(L);
	ଫେରନ୍ତୁ 0;
    }
    ଯଦି (luaV_totypval(L, -1, rettv) == FAIL)
	emsg("luaeval: cannot convert value");
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_setref(lua_State *L)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID = lua_tointeger(L, 1);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା abort = FALSE;

    lua_pushlightuserdata(L, (ଶୂନ୍ୟ *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // the cache table

    luaV_getfield(L, LUAVIM_LIST);
    luaV_getfield(L, LUAVIM_DICT);
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_pushnil(L);
    // traverse cache table
    ଯେତେବେଳେ (!abort && lua_next(L, 2) != 0)
    {
	lua_getmetatable(L, -1);
	ଯଦି (lua_rawequal(L, -1, 3)) // list?
	{
	    list_T *l = (list_T *)lua_touserdata(L, 6); //ଚାବି

	    abort = set_ref_in_list(l, copyID);
	}
	ଅନ୍ୟଥା ଯଦି (lua_rawequal(L, -1, 4)) // dict?
	{
	    dict_T *d = (dict_T *)lua_touserdata(L, 6); //ଚାବି

	    abort = set_ref_in_dict(d, copyID);
	}
	ଅନ୍ୟଥା ଯଦି (lua_rawequal(L, -1, 5)) // funcref?
	{
	    luaV_Funcref *f = (luaV_Funcref *)lua_touserdata(L, 6); //ଚାବି

	    abort = set_ref_in_dict(f->self, copyID);
	}
	lua_କାଢ଼(L, 2); // metatable and value
    }
    lua_pushinteger(L, abort);
    ଫେରନ୍ତୁ 1;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_pushversion(lua_State *L)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା major = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା minor = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା patch = 0;
    ଅକ୍ଷର s[16];

    sscanf(LUAVIM_VERSION, "Lua %d.%d.%d", &major, &minor, &patch);
    vim_snprintf(s, ଆକାର(s), "%d.%d.%d", major, minor, patch);
    lua_pushstring(L, s);
    ଫେରନ୍ତୁ 0;
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

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaopen_vim(lua_State *L)
{
    lua_newtable(L);  // cache table
    lua_newtable(L);  // cache table's metatable
    lua_pushstring(L, "v");
    lua_setfield(L, -2, "__mode");
    lua_setmetatable(L, -2); // cache is weak-valued
    // put the cache table in the registry for luaV_get/setudata()
    lua_pushlightuserdata(L, (ଶୂନ୍ୟ *) LUAVIM_UDATA_CACHE);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
    lua_କାଢ଼(L, 1);  // we don't need the cache table here anymore
    // print
    lua_pushcfunction(L, luaV_print);
    lua_setglobal(L, "print");
    // debug.debug
    lua_getglobal(L, "debug");
    lua_pushcfunction(L, luaV_debug);
    lua_setfield(L, -2, "debug");
    lua_କାଢ଼(L, 1);
    // free
    lua_pushlightuserdata(L, (ଶୂନ୍ୟ *) LUAVIM_FREE);
    lua_pushcfunction(L, luaV_free);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // luaeval
    lua_pushlightuserdata(L, (ଶୂନ୍ୟ *) LUAVIM_LUAEVAL);
    lua_pushcfunction(L, luaV_luaeval);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // setref
    lua_pushlightuserdata(L, (ଶୂନ୍ୟ *) LUAVIM_SETREF);
    lua_pushcfunction(L, luaV_setref);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // register
    luaV_newmetatable(L, LUAVIM_LIST);
    luaV_register(L, luaV_List_mt);
    lua_କାଢ଼(L, 1);
    luaV_newmetatable(L, LUAVIM_DICT);
    luaV_register(L, luaV_Dict_mt);
    lua_କାଢ଼(L, 1);
    luaV_newmetatable(L, LUAVIM_BLOB);
    luaV_register(L, luaV_Blob_mt);
    lua_କାଢ଼(L, 1);
    luaV_newmetatable(L, LUAVIM_FUNCREF);
    luaV_register(L, luaV_Funcref_mt);
    lua_କାଢ଼(L, 1);
    luaV_newmetatable(L, LUAVIM_BUFFER);
    luaV_register(L, luaV_Buffer_mt);
    lua_କାଢ଼(L, 1);
    luaV_newmetatable(L, LUAVIM_WINDOW);
    luaV_register(L, luaV_Window_mt);
    lua_କାଢ଼(L, 1);
    lua_newtable(L); // vim table
    luaV_register(L, luaV_module);
    luaV_pushversion(L);
    lua_setfield(L, -2, "lua_version");
    lua_setglobal(L, LUAVIM_NAME);
    // custom code
    (ଶୂନ୍ୟ)luaL_dostring(L, LUA_VIM_FN_CODE);
    (ଶୂନ୍ୟ)luaL_dostring(L, LUA_VIM_UPDATE_PACKAGE_PATHS);
    (ଶୂନ୍ୟ)luaL_dostring(L, LUA_VIM_SETUP_VARIABLE_DICTS);

    lua_getglobal(L, LUAVIM_NAME);
    lua_getfield(L, -1, "_update_package_paths");

    ଯଦି (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);

    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର lua_State *
luaV_newstate(ଶୂନ୍ୟ)
{
    lua_State *L = luaL_newstate();
# ଯଦି LUA_VERSION_ଅଙ୍କ >= 505
    luaL_openselectedlibs(L, ~0, 0); // core libs
# ଅନ୍ୟଥା
    luaL_openlibs(L); // core libs
# endif
    lua_pushcfunction(L, luaopen_vim); // vim
    lua_call(L, 0, 0);
    ଫେରନ୍ତୁ L;
}

    ସ୍ଥିର ଶୂନ୍ୟ
luaV_setrange(lua_State *L, ପୂର୍ଣ୍ଣସଂଖ୍ୟା line1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା line2)
{
    lua_getglobal(L, LUAVIM_NAME);
    lua_pushinteger(L, line1);
    lua_setfield(L, -2, "firstline");
    lua_pushinteger(L, line2);
    lua_setfield(L, -2, "lastline");
    lua_କାଢ଼(L, 1); // vim table
}


// =======   Interface   =======

ସ୍ଥିର lua_State *L = ଖାଲି;

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
lua_isopen(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ L != ଖାଲି;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
lua_init(ଶୂନ୍ୟ)
{
    ଯଦି (lua_isopen())
	ଫେରନ୍ତୁ OK;

# ifdef DYNAMIC_LUA
    ଯଦି (!lua_enabled(TRUE))
    {
	emsg(_("Lua library cannot be loaded."));
	ଫେରନ୍ତୁ FAIL;
    }
# endif
    L = luaV_newstate();

    ଫେରନ୍ତୁ OK;
}

    ଶୂନ୍ୟ
lua_end(ଶୂନ୍ୟ)
{
    ଯଦି (!lua_isopen())
	ଫେରନ୍ତୁ;

    lua_close(L);
    L = ଖାଲି;
}

/*
 * ex commands
 */
    ଶୂନ୍ୟ
ex_lua(exarg_T *eap)
{
    ଅକ୍ଷର *script = (ଅକ୍ଷର *)script_get(eap, eap->arg);

    ଯଦି (!eap->skip && lua_init() == OK)
    {
	ଅକ୍ଷର *s = script != ଖାଲି ? script : (ଅକ୍ଷର *)eap->arg;

	luaV_setrange(L, eap->line1, eap->line2);
	ଯଦି (luaL_loadbuffer(L, s, strlen(s), LUAVIM_CHUNKNAME)
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
    ଯଦି (script != ଖାଲି)
	vim_free(script);
}

    ଶୂନ୍ୟ
ex_luado(exarg_T *eap)
{
    linenr_T l;
    ଅଚଳ ଅକ୍ଷର *s = (ଅଚଳ ଅକ୍ଷର *) eap->arg;
    luaL_Buffer b;
    size_t len;
    buf_T *was_curbuf = curbuf;

    ଯଦି (lua_init() == FAIL) ଫେରନ୍ତୁ;
    ଯଦି (u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
    {
	emsg(_("cannot save undo information"));
	ଫେରନ୍ତୁ;
    }
    luaV_setrange(L, eap->line1, eap->line2);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, "return function(line, linenr) ", 30); // header
    luaL_addlstring(&b, s, strlen(s));
    luaL_addlstring(&b, " end", 4); // footer
    luaL_pushresult(&b);
    s = lua_tolstring(L, -1, &len);
    ଯଦି (luaL_loadbuffer(L, s, len, LUAVIM_CHUNKNAME))
    {
	luaV_emsg(L);
	lua_କାଢ଼(L, 1); // function body
	ଫେରନ୍ତୁ;
    }
    lua_call(L, 0, 1);
    lua_replace(L, -2); // function -> body
    ପାଇଁ (l = eap->line1; l <= eap->line2; l++)
    {
	// Check the line number, the command may have deleted lines.
	ଯଦି (l > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    ଭାଙ୍ଗ;

	lua_pushvalue(L, -1); // function
	luaV_pushline(L, curbuf, l); // current line as arg
	lua_pushinteger(L, l); // current line number as arg
	ଯଦି (lua_pcall(L, 2, 1, 0))
	{
	    luaV_emsg(L);
	    ଭାଙ୍ଗ;
	}

	// Catch the command switching to another buffer.
	// Check the line number, the command may have deleted lines.
	ଯଦି (curbuf != was_curbuf || l > curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା)
	    ଭାଙ୍ଗ;

	ଯଦି (lua_isstring(L, -1)) // update line?
	{
# ifdef HAVE_SANDBOX
	    luaV_checksandbox(L);
# endif
	    ml_replace(l, luaV_toline(L, -1), TRUE);
	    changed_bytes(l, 0);
	    lua_କାଢ଼(L, 1); // result from luaV_toline
	}
	lua_କାଢ଼(L, 1); //ଧାଡ଼ି
    }
    lua_କାଢ଼(L, 1); // function
    check_cursor();
    update_screen(UPD_NOT_VALID);
}

    ଶୂନ୍ୟ
ex_luafile(exarg_T *eap)
{
    ଯଦି (lua_init() == FAIL)
	ଫେରନ୍ତୁ;
    ଯଦି (!eap->skip)
    {
	luaV_setrange(L, eap->line1, eap->line2);
	ଯଦି (luaL_loadfile(L, (ଅକ୍ଷର *) eap->arg) || lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
}

# define luaV_freetype(typ,tname) \
	ଶୂନ୍ୟ \
    lua_##tname##_free(typ *o) \
    { \
	ଯଦି (!lua_isopen()) ଫେରନ୍ତୁ; \
	luaV_getfield(L, LUAVIM_FREE); \
	lua_pushlightuserdata(L, (ଶୂନ୍ୟ *) o); \
	lua_call(L, 1, 0); \
    }

luaV_freetype(buf_T, buffer)
luaV_freetype(win_T, window)

    ଶୂନ୍ୟ
do_luaeval(char_u *str, typval_T *arg, typval_T *rettv)
{
    lua_init();
    luaV_getfield(L, LUAVIM_LUAEVAL);
    lua_pushstring(L, (ଅକ୍ଷର *) str);
    lua_pushlightuserdata(L, (ଶୂନ୍ୟ *) arg);
    lua_pushlightuserdata(L, (ଶୂନ୍ୟ *) rettv);
    lua_call(L, 3, 0);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_lua(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା aborted = 0;

    ଯଦି (!lua_isopen())
	ଫେରନ୍ତୁ 0;

    luaV_getfield(L, LUAVIM_SETREF);
    // call the function with 1 arg, getting 1 result back
    lua_pushinteger(L, copyID);
    lua_call(L, 1, 1);
    // get the result
    aborted = lua_tointeger(L, -1);
    // pop result off the stack
    lua_କାଢ଼(L, 1);

    ଫେରନ୍ତୁ aborted;
}

    ଶୂନ୍ୟ
update_package_paths_in_lua(ଶୂନ୍ୟ)
{
    ଯଦି (!lua_isopen())
	ଫେରନ୍ତୁ;

    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "_update_package_paths");

    ଯଦି (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);
}

/*
 * Native C function callback
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
luaV_call_lua_func(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	 argcount,
	typval_T *argvars,
	typval_T *rettv,
	ଶୂନ୍ୟ	 *state)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା luaargcount = argcount;
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_funcref);

    ଯଦି (funcstate->lua_tableref != LUA_NOREF)
    {
	// First arg for metatable __call method is a table
	luaargcount += 1;
	lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    }

    ପାଇଁ (i = 0; i < argcount; ++i)
	luaV_pushtypval(funcstate->L, &argvars[i]);

    ଯଦି (lua_pcall(funcstate->L, luaargcount, 1, 0))
    {
	luaV_emsg(funcstate->L);
	ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)FCERR_OTHER;
    }

    luaV_checktypval(funcstate->L, -1, rettv, "get return value");
    ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)FCERR_NONE;
}

/*
 * Free up any lua references held by the func state.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
luaV_call_lua_func_free(ଶୂନ୍ୟ *state)
{
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_funcref);
    funcstate->L = ଖାଲି;
    ଯଦି (funcstate->lua_tableref != LUA_NOREF)
	luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    VIM_CLEAR(funcstate);
}

#endif
