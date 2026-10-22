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

#அந்தர்பாவ "vim.h"
#அந்தர்பாவ "version.h"

#அந்தர்பாவ <lua.h>
#அந்தர்பாவ <lualib.h>
#அந்தர்பாவ <lauxlib.h>

#எனில் __STDC_VERSION__ >= 199901L
# define LUAV_INLINE உள்வரி
#இல்லையெனில்
# define LUAV_INLINE
#endif

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#எனில் defined(FEAT_LUA)

# define LUAVIM_CHUNKNAME "vim chunk"
# define LUAVIM_NAME "vim"
# define LUAVIM_EVALNAME "luaeval"
# define LUAVIM_EVALHEADER "local _A=select(1,...) return "

# ifdef LUA_RELEASE
#  define LUAVIM_VERSION LUA_RELEASE
# இல்லையெனில்
#  define LUAVIM_VERSION LUA_VERSION
# endif

வகைவரை buf_T *luaV_Buffer;
வகைவரை win_T *luaV_Window;
வகைவரை dict_T *luaV_Dict;
வகைவரை list_T *luaV_List;
வகைவரை blob_T *luaV_Blob;
வகைவரை கட்டமைப்பு {
    char_u	*name;	// funcref
    dict_T	*self;	// selfdict
} luaV_Funcref;
வகைவரை முழு (*msgfunc_T)(எழுத்து *);

வகைவரை கட்டமைப்பு {
    முழு lua_funcref;    // ref to a lua func
    முழு lua_tableref;   // ref to a lua table if metatable else LUA_NOREF. used
			// for __call
    lua_State *L;
} luaV_CFuncState;

நிலையான மாறிலி எழுத்து LUAVIM_DICT[] = "dict";
நிலையான மாறிலி எழுத்து LUAVIM_LIST[] = "list";
நிலையான மாறிலி எழுத்து LUAVIM_BLOB[] = "blob";
நிலையான மாறிலி எழுத்து LUAVIM_FUNCREF[] = "funcref";
நிலையான மாறிலி எழுத்து LUAVIM_BUFFER[] = "buffer";
நிலையான மாறிலி எழுத்து LUAVIM_WINDOW[] = "window";
நிலையான மாறிலி எழுத்து LUAVIM_FREE[] = "luaV_free";
நிலையான மாறிலி எழுத்து LUAVIM_LUAEVAL[] = "luaV_luaeval";
நிலையான மாறிலி எழுத்து LUAVIM_SETREF[] = "luaV_setref";

நிலையான மாறிலி எழுத்து LUA___CALL[] = "__call";

// get/setudata manage references to vim userdata in a cache table through
// object pointers (light userdata). The cache table itself is retrieved
// from the registry.

நிலையான மாறிலி எழுத்து LUAVIM_UDATA_CACHE[] = "luaV_udata_cache";

# define luaV_getfield(L, s) \
    lua_pushlightuserdata((L), (வெறுமை *)(s)); \
    lua_rawget((L), LUA_REGISTRYINDEX)
# define luaV_checksandbox(L) \
    எனில் (sandbox) luaL_error((L), "not allowed in sandbox")
# define lua_V_செய்தி(L) luaV_msgfunc((L), (msgfunc_T) செய்தி)
# define luaV_emsg(L) luaV_msgfunc((L), (msgfunc_T) emsg)
# define luaV_checktypval(L, a, v, செய்தி) \
    செய் { \
	எனில் (luaV_totypval(L, a, v) == FAIL) \
	    luaL_error(L, செய்தி ": cannot convert value"); \
    } போது (0)

நிலையான luaV_List *luaV_pushlist(lua_State *L, list_T *lis);
நிலையான luaV_Dict *luaV_pushdict(lua_State *L, dict_T *dic);
நிலையான luaV_Blob *luaV_pushblob(lua_State *L, blob_T *blo);
நிலையான luaV_Funcref *luaV_pushfuncref(lua_State *L, char_u *name);
நிலையான முழு luaV_call_lua_func(முழு argcount, typval_T *argvars, typval_T *rettv, வெறுமை *state);
நிலையான வெறுமை luaV_call_lua_func_free(வெறுமை *state);

# எனில் LUA_VERSION_இலக்கம் <= 501
#  define luaV_register(L, l) luaL_register(L, வெற்று, l)
#  define luaL_typeerror luaL_typerror
# இல்லையெனில்
#  define luaV_register(L, l) luaL_setfuncs(L, l, 0)
# endif

# ifdef DYNAMIC_LUA

#  ifdef MSWIN
#   define load_dll vimLoadLib
#   define குறியீடு_from_dll GetProcAddress
#   define close_dll FreeLibrary
#   define load_dll_error GetWin32Error
#  இல்லையெனில்
#   include <dlfcn.h>
#   define HANDLE வெறுமை*
#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#   define குறியீடு_from_dll dlsym
#   define close_dll dlclose
#   define load_dll_error dlerror
#  endif

// lauxlib
#  எனில் LUA_VERSION_இலக்கம் <= 501
#   define luaL_register dll_luaL_register
#   define luaL_prepbuffer dll_luaL_prepbuffer
#   define luaL_openlib dll_luaL_openlib
#   define luaL_typerror dll_luaL_typerror
#   define luaL_loadfile dll_luaL_loadfile
#   define luaL_loadbuffer dll_luaL_loadbuffer
#  இல்லையெனில்
#   define luaL_prepbuffsize dll_luaL_prepbuffsize
#   define luaL_setfuncs dll_luaL_setfuncs
#   define luaL_loadfilex dll_luaL_loadfilex
#   define luaL_loadbufferx dll_luaL_loadbufferx
#   define luaL_argerror dll_luaL_argerror
#  endif
#  எனில் LUA_VERSION_இலக்கம் >= 504
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
#  எனில் LUA_VERSION_இலக்கம் <= 501
#   define lua_tonumber dll_lua_tonumber
#   define lua_tointeger dll_lua_tointeger
#   define lua_call dll_lua_call
#   define lua_pcall dll_lua_pcall
#  இல்லையெனில்
#   define lua_tonumberx dll_lua_tonumberx
#   define lua_tointegerx dll_lua_tointegerx
#   define lua_callk dll_lua_callk
#   define lua_pcallk dll_lua_pcallk
#   define lua_getglobal dll_lua_getglobal
#   define lua_setglobal dll_lua_setglobal
#  endif
#  எனில் LUA_VERSION_இலக்கம் <= 502
#   define lua_replace dll_lua_replace
#   define lua_remove dll_lua_remove
#  endif
#  எனில் LUA_VERSION_இலக்கம் >= 503
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
#  எனில் LUA_VERSION_இலக்கம் >= 504
#   define lua_newuserdatauv dll_lua_newuserdatauv
#  இல்லையெனில்
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
#  எனில் LUA_VERSION_இலக்கம் >= 505
#   define luaL_openselectedlibs dll_luaL_openselectedlibs
#  இல்லையெனில்
#   define luaL_openlibs dll_luaL_openlibs
#  endif

// lauxlib
#  எனில் LUA_VERSION_இலக்கம் <= 501
வெறுமை (*dll_luaL_register) (lua_State *L, மாறிலி எழுத்து *libname, மாறிலி luaL_Reg *l);
எழுத்து *(*dll_luaL_prepbuffer) (luaL_Buffer *B);
வெறுமை (*dll_luaL_openlib) (lua_State *L, மாறிலி எழுத்து *libname, மாறிலி luaL_Reg *l, முழு nup);
முழு (*dll_luaL_typerror) (lua_State *L, முழு narg, மாறிலி எழுத்து *tname);
முழு (*dll_luaL_loadfile) (lua_State *L, மாறிலி எழுத்து *filename);
முழு (*dll_luaL_loadbuffer) (lua_State *L, மாறிலி எழுத்து *buff, size_t sz, மாறிலி எழுத்து *name);
#  இல்லையெனில்
எழுத்து *(*dll_luaL_prepbuffsize) (luaL_Buffer *B, size_t sz);
வெறுமை (*dll_luaL_setfuncs) (lua_State *L, மாறிலி luaL_Reg *l, முழு nup);
முழு (*dll_luaL_loadfilex) (lua_State *L, மாறிலி எழுத்து *filename, மாறிலி எழுத்து *mode);
முழு (*dll_luaL_loadbufferx) (lua_State *L, மாறிலி எழுத்து *buff, size_t sz, மாறிலி எழுத்து *name, மாறிலி எழுத்து *mode);
முழு (*dll_luaL_argerror) (lua_State *L, முழு numarg, மாறிலி எழுத்து *extramsg);
#  endif
#  எனில் LUA_VERSION_இலக்கம் >= 504
முழு (*dll_luaL_typeerror) (lua_State *L, முழு narg, மாறிலி எழுத்து *tname);
#  endif
வெறுமை (*dll_luaL_checkany) (lua_State *L, முழு narg);
மாறிலி எழுத்து *(*dll_luaL_checklstring) (lua_State *L, முழு இலக்கம்_Arg, size_t *l);
lua_Integer (*dll_luaL_checkinteger) (lua_State *L, முழு இலக்கம்_Arg);
lua_Integer (*dll_luaL_optinteger) (lua_State *L, முழு nArg, lua_Integer def);
வெறுமை (*dll_luaL_checktype) (lua_State *L, முழு narg, முழு t);
முழு (*dll_luaL_error) (lua_State *L, மாறிலி எழுத்து *fmt, ...);
lua_State *(*dll_luaL_newstate) (வெறுமை);
வெறுமை (*dll_luaL_buffinit) (lua_State *L, luaL_Buffer *B);
வெறுமை (*dll_luaL_addlstring) (luaL_Buffer *B, மாறிலி எழுத்து *s, size_t l);
வெறுமை (*dll_luaL_pushresult) (luaL_Buffer *B);
முழு (*dll_luaL_loadstring) (lua_State *L, மாறிலி எழுத்து *s);
முழு (*dll_luaL_ref) (lua_State *L, முழு idx);
#  எனில் LUA_VERSION_இலக்கம் <= 502
வெறுமை (*dll_luaL_unref) (lua_State *L, முழு idx, முழு n);
#  இல்லையெனில்
வெறுமை (*dll_luaL_unref) (lua_State *L, முழு idx, lua_Integer n);
#  endif
// lua
#  எனில் LUA_VERSION_இலக்கம் <= 501
lua_எண் (*dll_lua_tonumber) (lua_State *L, முழு idx);
lua_Integer (*dll_lua_tointeger) (lua_State *L, முழு idx);
வெறுமை (*dll_lua_call) (lua_State *L, முழு nargs, முழு nresults);
முழு (*dll_lua_pcall) (lua_State *L, முழு nargs, முழு nresults, முழு errfunc);
#  இல்லையெனில்
lua_எண் (*dll_lua_tonumberx) (lua_State *L, முழு idx, முழு *isnum);
lua_Integer (*dll_lua_tointegerx) (lua_State *L, முழு idx, முழு *isnum);
வெறுமை (*dll_lua_callk) (lua_State *L, முழு nargs, முழு nresults, முழு ctx,
	lua_CFunction k);
முழு (*dll_lua_pcallk) (lua_State *L, முழு nargs, முழு nresults, முழு errfunc,
	முழு ctx, lua_CFunction k);
வெறுமை (*dll_lua_getglobal) (lua_State *L, மாறிலி எழுத்து *var);
வெறுமை (*dll_lua_setglobal) (lua_State *L, மாறிலி எழுத்து *var);
#  endif
#  எனில் LUA_VERSION_இலக்கம் <= 502
வெறுமை (*dll_lua_replace) (lua_State *L, முழு idx);
வெறுமை (*dll_lua_remove) (lua_State *L, முழு idx);
#  endif
#  எனில் LUA_VERSION_இலக்கம் >= 503
வெறுமை  (*dll_lua_rotate) (lua_State *L, முழு idx, முழு n);
வெறுமை (*dll_lua_copy) (lua_State *L, முழு fromidx, முழு toidx);
#  endif
மாறிலி எழுத்து *(*dll_lua_typename) (lua_State *L, முழு tp);
வெறுமை       (*dll_lua_close) (lua_State *L);
முழு (*dll_lua_gettop) (lua_State *L);
வெறுமை (*dll_lua_settop) (lua_State *L, முழு idx);
வெறுமை (*dll_lua_pushvalue) (lua_State *L, முழு idx);
முழு (*dll_lua_isnumber) (lua_State *L, முழு idx);
முழு (*dll_lua_isstring) (lua_State *L, முழு idx);
முழு (*dll_lua_type) (lua_State *L, முழு idx);
முழு (*dll_lua_rawequal) (lua_State *L, முழு idx1, முழு idx2);
முழு (*dll_lua_toboolean) (lua_State *L, முழு idx);
மாறிலி எழுத்து *(*dll_lua_tolstring) (lua_State *L, முழு idx, size_t *len);
வெறுமை *(*dll_lua_touserdata) (lua_State *L, முழு idx);
வெறுமை (*dll_lua_pushnil) (lua_State *L);
வெறுமை (*dll_lua_pushnumber) (lua_State *L, lua_எண் n);
வெறுமை (*dll_lua_pushinteger) (lua_State *L, lua_Integer n);
வெறுமை (*dll_lua_pushlstring) (lua_State *L, மாறிலி எழுத்து *s, size_t l);
வெறுமை (*dll_lua_pushstring) (lua_State *L, மாறிலி எழுத்து *s);
மாறிலி எழுத்து *(*dll_lua_pushfstring) (lua_State *L, மாறிலி எழுத்து *fmt, ...);
வெறுமை (*dll_lua_pushcclosure) (lua_State *L, lua_CFunction fn, முழு n);
வெறுமை (*dll_lua_pushboolean) (lua_State *L, முழு b);
வெறுமை (*dll_lua_pushlightuserdata) (lua_State *L, வெறுமை *p);
வெறுமை (*dll_lua_getfield) (lua_State *L, முழு idx, மாறிலி எழுத்து *k);
#  எனில் LUA_VERSION_இலக்கம் <= 502
வெறுமை (*dll_lua_rawget) (lua_State *L, முழு idx);
வெறுமை (*dll_lua_rawgeti) (lua_State *L, முழு idx, முழு n);
#  இல்லையெனில்
முழு (*dll_lua_rawget) (lua_State *L, முழு idx);
முழு (*dll_lua_rawgeti) (lua_State *L, முழு idx, lua_Integer n);
#  endif
வெறுமை (*dll_lua_createtable) (lua_State *L, முழு narr, முழு nrec);
வெறுமை (*dll_lua_settable) (lua_State *L, முழு idx);
#  எனில் LUA_VERSION_இலக்கம் >= 504
வெறுமை *(*dll_lua_newuserdatauv) (lua_State *L, size_t sz, முழு nuvalue);
#  இல்லையெனில்
வெறுமை *(*dll_lua_newuserdata) (lua_State *L, size_t sz);
#  endif
முழு (*dll_lua_getmetatable) (lua_State *L, முழு objindex);
வெறுமை (*dll_lua_setfield) (lua_State *L, முழு idx, மாறிலி எழுத்து *k);
வெறுமை (*dll_lua_rawset) (lua_State *L, முழு idx);
#  எனில் LUA_VERSION_இலக்கம் <= 502
வெறுமை (*dll_lua_rawseti) (lua_State *L, முழு idx, முழு n);
#  இல்லையெனில்
வெறுமை (*dll_lua_rawseti) (lua_State *L, முழு idx, lua_Integer n);
#  endif
முழு (*dll_lua_setmetatable) (lua_State *L, முழு objindex);
முழு (*dll_lua_next) (lua_State *L, முழு idx);
// libs
முழு (*dll_luaopen_base) (lua_State *L);
முழு (*dll_luaopen_table) (lua_State *L);
முழு (*dll_luaopen_string) (lua_State *L);
முழு (*dll_luaopen_math) (lua_State *L);
முழு (*dll_luaopen_io) (lua_State *L);
முழு (*dll_luaopen_os) (lua_State *L);
முழு (*dll_luaopen_package) (lua_State *L);
முழு (*dll_luaopen_debug) (lua_State *L);
#  எனில் LUA_VERSION_இலக்கம் >= 505
வெறுமை (*dll_luaL_openselectedlibs) (lua_State *L, முழு load, முழு preload);
#  இல்லையெனில்
வெறுமை (*dll_luaL_openlibs) (lua_State *L);
#  endif

வகைவரை வெறுமை **luaV_function;
வகைவரை கட்டமைப்பு {
    மாறிலி எழுத்து *name;
    luaV_function func;
} luaV_Reg;

நிலையான மாறிலி luaV_Reg luaV_dll[] = {
    // lauxlib
#  எனில் LUA_VERSION_இலக்கம் <= 501
    {"luaL_register", (luaV_function) &dll_luaL_register},
    {"luaL_prepbuffer", (luaV_function) &dll_luaL_prepbuffer},
    {"luaL_openlib", (luaV_function) &dll_luaL_openlib},
    {"luaL_typerror", (luaV_function) &dll_luaL_typerror},
    {"luaL_loadfile", (luaV_function) &dll_luaL_loadfile},
    {"luaL_loadbuffer", (luaV_function) &dll_luaL_loadbuffer},
#  இல்லையெனில்
    {"luaL_prepbuffsize", (luaV_function) &dll_luaL_prepbuffsize},
    {"luaL_setfuncs", (luaV_function) &dll_luaL_setfuncs},
    {"luaL_loadfilex", (luaV_function) &dll_luaL_loadfilex},
    {"luaL_loadbufferx", (luaV_function) &dll_luaL_loadbufferx},
    {"luaL_argerror", (luaV_function) &dll_luaL_argerror},
#  endif
#  எனில் LUA_VERSION_இலக்கம் >= 504
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
#  எனில் LUA_VERSION_இலக்கம் <= 501
    {"lua_tonumber", (luaV_function) &dll_lua_tonumber},
    {"lua_tointeger", (luaV_function) &dll_lua_tointeger},
    {"lua_call", (luaV_function) &dll_lua_call},
    {"lua_pcall", (luaV_function) &dll_lua_pcall},
#  இல்லையெனில்
    {"lua_tonumberx", (luaV_function) &dll_lua_tonumberx},
    {"lua_tointegerx", (luaV_function) &dll_lua_tointegerx},
    {"lua_callk", (luaV_function) &dll_lua_callk},
    {"lua_pcallk", (luaV_function) &dll_lua_pcallk},
    {"lua_getglobal", (luaV_function) &dll_lua_getglobal},
    {"lua_setglobal", (luaV_function) &dll_lua_setglobal},
#  endif
#  எனில் LUA_VERSION_இலக்கம் <= 502
    {"lua_replace", (luaV_function) &dll_lua_replace},
    {"lua_remove", (luaV_function) &dll_lua_remove},
#  endif
#  எனில் LUA_VERSION_இலக்கம் >= 503
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
#  எனில் LUA_VERSION_இலக்கம் >= 504
    {"lua_newuserdatauv", (luaV_function) &dll_lua_newuserdatauv},
#  இல்லையெனில்
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
#  எனில் LUA_VERSION_இலக்கம் >= 505
    {"luaL_openselectedlibs", (luaV_function) &dll_luaL_openselectedlibs},
#  இல்லையெனில்
    {"luaL_openlibs", (luaV_function) &dll_luaL_openlibs},
#  endif
    {வெற்று, வெற்று}
};

நிலையான HANDLE hinstLua = வெற்று;

    நிலையான முழு
lua_link_init(எழுத்து *libname, முழு verbose)
{
    மாறிலி luaV_Reg *reg;
    எனில் (hinstLua) திரும்பு OK;
    hinstLua = load_dll(libname);
    எனில் (!hinstLua)
    {
	எனில் (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	திரும்பு FAIL;
    }
    க்கு (reg = luaV_dll; reg->func; reg++)
    {
	எனில் ((*reg->func = குறியீடு_from_dll(hinstLua, reg->name)) == வெற்று)
	{
	    close_dll(hinstLua);
	    hinstLua = 0;
	    எனில் (verbose)
		semsg(_(e_could_not_load_library_function_str), reg->name);
	    திரும்பு FAIL;
	}
    }
    திரும்பு OK;
}
# endif // DYNAMIC_LUA

# எனில் defined(DYNAMIC_LUA)
    முழு
lua_enabled(முழு verbose)
{
    திரும்பு lua_link_init((எழுத்து *)p_luadll, verbose) == OK;
}
# endif

# எனில் LUA_VERSION_இலக்கம் > 501 && LUA_VERSION_இலக்கம் < 504
    நிலையான முழு
luaL_typeerror(lua_State *L, முழு narg, மாறிலி எழுத்து *tname)
{
    மாறிலி எழுத்து *செய்தி = lua_pushfstring(L, "%s expected, got %s",
	    tname, luaL_typename(L, narg));
    திரும்பு luaL_argerror(L, narg, செய்தி);
}
# endif

    நிலையான LUAV_INLINE வெறுமை
luaV_getudata(lua_State *L, வெறுமை *v)
{
    lua_pushlightuserdata(L, (வெறுமை *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // now the cache table is at the top of the stack
    lua_pushlightuserdata(L, v);
    lua_rawget(L, -2);
    lua_remove(L, -2);  // remove the cache table from the stack
}

    நிலையான LUAV_INLINE வெறுமை
luaV_setudata(lua_State *L, வெறுமை *v)
{
    lua_pushlightuserdata(L, (வெறுமை *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // cache table is at -1
    lua_pushlightuserdata(L, v);       // ...now at -2
    lua_pushvalue(L, -3);	       // copy the userdata (cache at -3)
    lua_rawset(L, -3);		       // consumes two stack items
    lua_எடு(L, 1);		       // and remove the cache table
}

// =======   Internal   =======

    நிலையான வெறுமை
luaV_newmetatable(lua_State *L, மாறிலி எழுத்து *tname)
{
    lua_newtable(L);
    lua_pushlightuserdata(L, (வெறுமை *) tname);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
}

    நிலையான வெறுமை *
luaV_toudata(lua_State *L, முழு ud, மாறிலி எழுத்து *tname)
{
    வெறுமை *p = lua_touserdata(L, ud);

    எனில் (p == வெற்று)
	திரும்பு வெற்று;

    // value is userdata
    எனில் (lua_getmetatable(L, ud)) // does it have a metatable?
    {
	luaV_getfield(L, tname); // get metatable
	எனில் (lua_rawequal(L, -1, -2)) // MTs match?
	{
	    lua_எடு(L, 2); // MTs
	    திரும்பு p;
	}
    }

    திரும்பு வெற்று;
}

    நிலையான வெறுமை *
luaV_checkcache(lua_State *L, வெறுமை *p)
{
    luaV_getudata(L, p);
    எனில் (lua_isnil(L, -1)) luaL_error(L, "invalid object");
    lua_எடு(L, 1);
    திரும்பு p;
}

# define luaV_unbox(L,luatyp,ud) (*((luatyp *) lua_touserdata((L),(ud))))

# define luaV_checkvalid(L,luatyp,ud) \
    luaV_checkcache((L), (வெறுமை *) luaV_unbox((L),luatyp,(ud)))

    நிலையான வெறுமை *
luaV_checkudata(lua_State *L, முழு ud, மாறிலி எழுத்து *tname)
{
    வெறுமை *p = luaV_toudata(L, ud, tname);
    எனில் (p == வெற்று) luaL_typeerror(L, ud, tname);
    திரும்பு p;
}

    நிலையான வெறுமை
luaV_pushtypval(lua_State *L, typval_T *tv)
{
    எனில் (tv == வெற்று)
    {
	lua_pushnil(L);
	திரும்பு;
    }
    தேர்வு (tv->v_type)
    {
	நிலை VAR_STRING:
	    lua_pushstring(L, tv->vval.v_string == வெற்று
					    ? "" : (எழுத்து *)tv->vval.v_string);
	    நிறுத்து;
	நிலை VAR_எண்:
	    lua_pushinteger(L, (முழு) tv->vval.v_எண்);
	    நிறுத்து;
	நிலை VAR_FLOAT:
	    lua_pushnumber(L, (lua_எண்) tv->vval.v_float);
	    நிறுத்து;
	நிலை VAR_LIST:
	    luaV_pushlist(L, tv->vval.v_list);
	    நிறுத்து;
	நிலை VAR_DICT:
	    luaV_pushdict(L, tv->vval.v_dict);
	    நிறுத்து;
	நிலை VAR_BOOL:
	நிலை VAR_SPECIAL:
	    எனில் (tv->vval.v_எண் <= VVAL_TRUE)
		lua_pushboolean(L, (முழு) tv->vval.v_எண்);
	    இல்லையெனில்
		lua_pushnil(L);
	    நிறுத்து;
	நிலை VAR_FUNC:
	    luaV_pushfuncref(L, tv->vval.v_string);
	    நிறுத்து;
	நிலை VAR_PARTIAL:
	    // TODO: handle partial arguments
	    luaV_pushfuncref(L, partial_name(tv->vval.v_partial));
	    நிறுத்து;

	நிலை VAR_BLOB:
	    luaV_pushblob(L, tv->vval.v_blob);
	    நிறுத்து;
	இயல்பு:
	    lua_pushnil(L);
    }
}

/*
 * Converts lua value at 'pos' to typval 'tv'.
 * Returns OK or FAIL.
 */
    நிலையான முழு
luaV_totypval(lua_State *L, முழு pos, typval_T *tv)
{
    முழு status = OK;

    tv->v_lock = 0;

    தேர்வு (lua_type(L, pos))
    {
	நிலை LUA_TBOOLEAN:
	    tv->v_type = VAR_BOOL;
	    tv->vval.v_எண் = (varnumber_T) lua_toboolean(L, pos);
	    நிறுத்து;
	நிலை LUA_TNIL:
	    tv->v_type = VAR_SPECIAL;
	    tv->vval.v_எண் = VVAL_NULL;
	    நிறுத்து;
	நிலை LUA_TSTRING:
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strsave((char_u *) lua_tostring(L, pos));
	    நிறுத்து;
	நிலை LUA_TNUMBER:
	{
	    மாறிலி lua_எண் n = lua_tonumber(L, pos);

	    எனில் (n > (lua_எண்)INT64_MAX || n < (lua_எண்)INT64_MIN
		    || ((lua_எண்)((varnumber_T)n)) != n)
	    {
		tv->v_type = VAR_FLOAT;
		tv->vval.v_float = (float_T)n;
	    }
	    இல்லையெனில்
	    {
		tv->v_type = VAR_எண்;
		tv->vval.v_எண் = (varnumber_T)n;
	    }
	}
	    நிறுத்து;
	நிலை LUA_TFUNCTION:
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
	    நிறுத்து;
	}
	நிலை LUA_TTABLE:
	{
	    முழு lua_tableref;

	    lua_pushvalue(L, pos);
	    lua_tableref = luaL_ref(L, LUA_REGISTRYINDEX);
	    எனில் (lua_getmetatable(L, pos))
	    {
		lua_getfield(L, -1, LUA___CALL);
		எனில் (lua_isfunction(L, -1))
		{
		    char_u *name;
		    முழு lua_funcref = luaL_ref(L, LUA_REGISTRYINDEX);
		    luaV_CFuncState *state = ALLOC_CLEAR_ONE(luaV_CFuncState);

		    state->lua_funcref = lua_funcref;
		    state->L = L;
		    state->lua_tableref = lua_tableref;
		    name = register_cfunc(&luaV_call_lua_func,
					      &luaV_call_lua_func_free, state);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(name);
		    நிறுத்து;
		}
	    }
	    tv->v_type = VAR_எண்;
	    tv->vval.v_எண் = 0;
	    status = FAIL;
	    நிறுத்து;
	}
	நிலை LUA_TUSERDATA:
	{
	    வெறுமை *p = lua_touserdata(L, pos);

	    எனில் (lua_getmetatable(L, pos)) // has metatable?
	    {
		// check list
		luaV_getfield(L, LUAVIM_LIST);
		எனில் (lua_rawequal(L, -1, -2))
		{
		    tv->v_type = VAR_LIST;
		    tv->vval.v_list = *((luaV_List *) p);
		    ++tv->vval.v_list->lv_refcount;
		    lua_எடு(L, 2); // MTs
		    நிறுத்து;
		}
		// check dict
		luaV_getfield(L, LUAVIM_DICT);
		எனில் (lua_rawequal(L, -1, -3))
		{
		    tv->v_type = VAR_DICT;
		    tv->vval.v_dict = *((luaV_Dict *) p);
		    ++tv->vval.v_dict->dv_refcount;
		    lua_எடு(L, 3); // MTs
		    நிறுத்து;
		}
		// check blob
		luaV_getfield(L, LUAVIM_BLOB);
		எனில் (lua_rawequal(L, -1, -4))
		{
		    tv->v_type = VAR_BLOB;
		    tv->vval.v_blob = *((luaV_Blob *) p);
		    ++tv->vval.v_blob->bv_refcount;
		    lua_எடு(L, 4); // MTs
		    நிறுத்து;
		}
		// check funcref
		luaV_getfield(L, LUAVIM_FUNCREF);
		எனில் (lua_rawequal(L, -1, -5))
		{
		    luaV_Funcref *f = (luaV_Funcref *) p;

		    func_ref(f->name);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(f->name);
		    lua_எடு(L, 5); // MTs
		    நிறுத்து;
		}
		lua_எடு(L, 4); // MTs
	    }
	}
	// FALLTHROUGH
	இயல்பு:
	    tv->v_type = VAR_எண்;
	    tv->vval.v_எண் = 0;
	    status = FAIL;
    }
    திரும்பு status;
}

/*
 * similar to luaL_addlstring, but replaces \0 with \n if toline and
 * \n with \0 otherwise
 */
    நிலையான வெறுமை
luaV_addlstring(luaL_Buffer *b, மாறிலி எழுத்து *s, size_t l, முழு toline)
{
    போது (l--)
    {
	எனில் (*s == '\0' && toline)
	    luaL_addchar(b, '\n');
	இல்லையெனில் எனில் (*s == '\n' && !toline)
	    luaL_addchar(b, '\0');
	இல்லையெனில்
	    luaL_addchar(b, *s);
	s++;
    }
}

    நிலையான வெறுமை
luaV_pushline(lua_State *L, buf_T *buf, linenr_T n)
{
    மாறிலி எழுத்து *s = (மாறிலி எழுத்து *) ml_get_buf(buf, n, FALSE);
    luaL_Buffer b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, strlen(s), 0);
    luaL_pushresult(&b);
}

    நிலையான char_u *
luaV_toline(lua_State *L, முழு pos)
{
    size_t l;
    மாறிலி எழுத்து *s = lua_tolstring(L, pos, &l);

    luaL_Buffer b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 1);
    luaL_pushresult(&b);
    திரும்பு (char_u *) lua_tostring(L, -1);
}

/*
 * pops a string s from the top of the stack and calls mf(t) for pieces t of
 * s separated by newlines
 */
    நிலையான வெறுமை
luaV_msgfunc(lua_State *L, msgfunc_T mf)
{
    luaL_Buffer b;
    size_t l;
    மாறிலி எழுத்து *p, *s = lua_tolstring(L, -1, &l);
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 0);
    luaL_pushresult(&b);
    // break string
    p = s = lua_tolstring(L, -1, &l);
    போது (l--)
    {
	எனில் (*p++ == '\0') // break?
	{
	    mf((எழுத்து *)s);
	    s = p;
	}
    }
    mf((எழுத்து *)s);
    lua_எடு(L, 2); // original and modified strings
}

# define luaV_newtype(typ,tname,luatyp,luatname) \
	நிலையான luatyp * \
    luaV_new##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = (luatyp *) lua_newuserdata(L, அளவு(luatyp)); \
	*o = obj; \
	luaV_setudata(L, obj); /* cache[obj] = udata */ \
	luaV_getfield(L, luatname); \
	lua_setmetatable(L, -2); \
	திரும்பு o; \
    }

# define luaV_pushtype(typ,tname,luatyp) \
	நிலையான luatyp * \
    lua_V_தள்ளு##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = வெற்று; \
	எனில் (obj == வெற்று) \
	    lua_pushnil(L); \
	இல்லையெனில் \
	{ \
	    luaV_getudata(L, obj); \
	    எனில் (lua_isnil(L, -1)) /* not interned? */ \
	    { \
		lua_எடு(L, 1); \
		o = luaV_new##tname(L, obj); \
	    } \
	    இல்லையெனில் \
		o = (luatyp *) lua_touserdata(L, -1); \
	} \
	திரும்பு o; \
    }

# define luaV_type_tostring(tname,luatname) \
	நிலையான முழு \
    luaV_##tname##_tostring(lua_State *L) \
    { \
	lua_pushfstring(L, "%s: %p", luatname, lua_touserdata(L, 1)); \
	திரும்பு 1; \
    }

// =======   List type   =======

    நிலையான luaV_List *
luaV_newlist(lua_State *L, list_T *lis)
{
    luaV_List *l = (luaV_List *) lua_newuserdata(L, அளவு(luaV_List));
    *l = lis;
    lis->lv_refcount++; // reference in Lua
    luaV_setudata(L, lis); // cache[lis] = udata
    luaV_getfield(L, LUAVIM_LIST);
    lua_setmetatable(L, -2);
    திரும்பு l;
}

luaV_pushtype(list_T, list, luaV_List)
luaV_type_tostring(list, LUAVIM_LIST)

    நிலையான முழு
luaV_list_len(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushinteger(L, (முழு) list_len(l));
    திரும்பு 1;
}

    நிலையான முழு
luaV_list_iter(lua_State *L)
{
    listitem_T *li = (listitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    எனில் (li == வெற்று) திரும்பு 0;
    luaV_pushtypval(L, &li->li_tv);
    lua_pushlightuserdata(L, (வெறுமை *) li->li_next);
    lua_replace(L, lua_upvalueindex(1));
    திரும்பு 1;
}

    நிலையான முழு
luaV_list_call(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushlightuserdata(L, (வெறுமை *) l->lv_first);
    lua_pushcclosure(L, luaV_list_iter, 1);
    திரும்பு 1;
}

    நிலையான முழு
luaV_list_index(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    எனில் (lua_isnumber(L, 2)) // list item?
    {
	long n = (long) luaL_checkinteger(L, 2);
	listitem_T *li;

	// Lua array index starts with 1 while Vim uses 0, subtract 1 to
	// normalize.
	n -= 1;
	li = list_find(l, n);
	எனில் (li == வெற்று)
	    lua_pushnil(L);
	இல்லையெனில்
	    luaV_pushtypval(L, &li->li_tv);
    }
    இல்லையெனில் எனில் (lua_isstring(L, 2)) // method?
    {
	மாறிலி எழுத்து *s = lua_tostring(L, 2);
	எனில் (strncmp(s, "add", 3) == 0
		|| strncmp(s, "insert", 6) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	இல்லையெனில்
	    lua_pushnil(L);
    }
    இல்லையெனில்
	lua_pushnil(L);
    திரும்பு 1;
}

    நிலையான முழு
luaV_list_newindex(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    long n = (long) luaL_checkinteger(L, 2);
    listitem_T *li;

    // Lua array index starts with 1 while Vim uses 0, subtract 1 to normalize.
    n -= 1;

    எனில் (l->lv_lock)
	luaL_error(L, "list is locked");
    li = list_find(l, n);
    எனில் (li == வெற்று)
    {
	எனில் (!lua_isnil(L, 3))
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "inserting list item");
	    எனில் (list_செருகு_tv(l, &v, li) == FAIL)
		luaL_error(L, "failed to add item to list");
	    clear_tv(&v);
	}
    }
    இல்லையெனில்
    {
	எனில் (lua_isnil(L, 3)) // remove?
	{
	    vimlist_remove(l, li, li);
	    listitem_free(l, li);
	}
	இல்லையெனில்
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "setting list item");
	    clear_tv(&li->li_tv);
	    li->li_tv = v;
	}
    }
    திரும்பு 0;
}

    நிலையான முழு
luaV_list_add(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (வெறுமை *) *lis);
    typval_T v;
    எனில் (l->lv_lock)
	luaL_error(L, "list is locked");
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "adding list item");
    எனில் (list_append_tv(l, &v) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    திரும்பு 1;
}

    நிலையான முழு
lua_V_list_செருகு(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (வெறுமை *) *lis);
    long pos = (long) luaL_optinteger(L, 3, 0);
    listitem_T *li = வெற்று;
    typval_T v;
    எனில் (l->lv_lock)
	luaL_error(L, "list is locked");
    எனில் (pos < l->lv_len)
    {
	li = list_find(l, pos);
	எனில் (li == வெற்று)
	    luaL_error(L, "invalid position");
    }
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "inserting list item");
    எனில் (list_செருகு_tv(l, &v, li) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    திரும்பு 1;
}

நிலையான மாறிலி luaL_Reg luaV_List_mt[] = {
    {"__tostring", luaV_list_tostring},
    {"__len", luaV_list_len},
    {"__call", luaV_list_call},
    {"__index", luaV_list_index},
    {"__newindex", luaV_list_newindex},
    {"add", luaV_list_add},
    {"insert", lua_V_list_செருகு},
    {வெற்று, வெற்று}
};


// =======   Dict type   =======

    நிலையான luaV_Dict *
luaV_newdict(lua_State *L, dict_T *dic)
{
    luaV_Dict *d = (luaV_Dict *) lua_newuserdata(L, அளவு(luaV_Dict));
    *d = dic;
    dic->dv_refcount++; // reference in Lua
    luaV_setudata(L, dic); // cache[dic] = udata
    luaV_getfield(L, LUAVIM_DICT);
    lua_setmetatable(L, -2);
    திரும்பு d;
}

luaV_pushtype(dict_T, dict, luaV_Dict)
luaV_type_tostring(dict, LUAVIM_DICT)

    நிலையான முழு
luaV_dict_len(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    lua_pushinteger(L, (முழு) dict_len(d));
    திரும்பு 1;
}

    நிலையான முழு
luaV_dict_iter(lua_State *L UNUSED)
{
# ifdef FEAT_EVAL
    hashitem_T *hi = (hashitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    முழு n = lua_tointeger(L, lua_upvalueindex(2));
    dictitem_T *di;
    எனில் (n <= 0) திரும்பு 0;
    போது (HASHITEM_EMPTY(hi)) hi++;
    di = dict_lookup(hi);
    lua_pushstring(L, (எழுத்து *) hi->hi_சாவி);
    luaV_pushtypval(L, &di->di_tv);
    lua_pushlightuserdata(L, (வெறுமை *) (hi + 1));
    lua_replace(L, lua_upvalueindex(1));
    lua_pushinteger(L, n - 1);
    lua_replace(L, lua_upvalueindex(2));
    திரும்பு 2;
# இல்லையெனில்
    திரும்பு 0;
# endif
}

    நிலையான முழு
luaV_dict_call(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    hashtab_T *ht = &d->dv_hashtab;
    lua_pushlightuserdata(L, (வெறுமை *) ht->ht_array);
    lua_pushinteger(L, ht->ht_used); // # remaining items
    lua_pushcclosure(L, luaV_dict_iter, 2);
    திரும்பு 1;
}

    நிலையான முழு
luaV_dict_index(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *சாவி = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di = dict_find(d, சாவி, -1);

    எனில் (di == வெற்று)
    {
	lua_pushnil(L);
	திரும்பு 1;
    }

    luaV_pushtypval(L, &di->di_tv);
    எனில் (di->di_tv.v_type == VAR_FUNC) // funcref?
    {
	luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, -1);
	f->self = d; // keep "self" reference
	d->dv_refcount++;
    }

    திரும்பு 1;
}

    நிலையான முழு
luaV_dict_newindex(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *சாவி = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di;
    typval_T tv;

    எனில் (d->dv_lock)
	luaL_error(L, "dict is locked");
    எனில் (சாவி == வெற்று)
	திரும்பு 0;
    எனில் (*சாவி == NUL)
	luaL_error(L, "empty key");
    எனில் (!lua_isnil(L, 3)) // read value?
    {
	luaV_checktypval(L, 3, &tv, "setting dict item");
	எனில் (d->dv_scope == VAR_DEF_SCOPE && tv.v_type == VAR_FUNC)
	{
	    clear_tv(&tv);
	    luaL_error(L, "cannot assign funcref to builtin scope");
	}
    }
    di = dict_find(d, சாவி, -1);
    எனில் (di == வெற்று) // non-existing key?
    {
	எனில் (lua_isnil(L, 3))
	    திரும்பு 0;
	di = dictitem_alloc(சாவி);
	எனில் (di == வெற்று)
	{
	    clear_tv(&tv);
	    திரும்பு 0;
	}
	எனில் (dict_add(d, di) == FAIL)
	{
	    vim_free(di);
	    clear_tv(&tv);
	    திரும்பு 0;
	}
    }
    இல்லையெனில்
	clear_tv(&di->di_tv);
    எனில் (lua_isnil(L, 3)) // remove?
    {
	hashitem_T *hi = hash_find(&d->dv_hashtab, di->di_சாவி);
	hash_remove(&d->dv_hashtab, hi, "Lua new index");
	dictitem_free(di);
    }
    இல்லையெனில்
	di->di_tv = tv;
    திரும்பு 0;
}

நிலையான மாறிலி luaL_Reg luaV_Dict_mt[] = {
    {"__tostring", luaV_dict_tostring},
    {"__len", luaV_dict_len},
    {"__call", luaV_dict_call},
    {"__index", luaV_dict_index},
    {"__newindex", luaV_dict_newindex},
    {வெற்று, வெற்று}
};


// =======   Blob type   =======

    நிலையான luaV_Blob *
luaV_newblob(lua_State *L, blob_T *blo)
{
    luaV_Blob *b = (luaV_Blob *) lua_newuserdata(L, அளவு(luaV_Blob));
    *b = blo;
    blo->bv_refcount++; // reference in Lua
    luaV_setudata(L, blo); // cache[blo] = udata
    luaV_getfield(L, LUAVIM_BLOB);
    lua_setmetatable(L, -2);
    திரும்பு b;
}

luaV_pushtype(blob_T, blob, luaV_Blob)
luaV_type_tostring(blob, LUAVIM_BLOB)

    நிலையான முழு
luaV_blob_gc(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    blob_unref(b);
    திரும்பு 0;
}

    நிலையான முழு
luaV_blob_len(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    lua_pushinteger(L, (முழு) blob_len(b));
    திரும்பு 1;
}

    நிலையான முழு
luaV_blob_index(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    எனில் (lua_isnumber(L, 2))
    {
	முழு idx = luaL_checkinteger(L, 2);
	எனில் (idx < 0)
	    // Negative index is relative to the end.
	    idx += (முழு) blob_len(b);
	எனில் (idx >= 0 && idx < blob_len(b))
	    lua_pushnumber(L, (lua_எண்) blob_get(b, idx));
	இல்லையெனில்
	    lua_pushnil(L);
    }
    இல்லையெனில் எனில் (lua_isstring(L, 2))
    {
	மாறிலி எழுத்து *s = lua_tostring(L, 2);
	எனில் (strncmp(s, "add", 3) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	இல்லையெனில்
	    lua_pushnil(L);
    }
    இல்லையெனில்
	lua_pushnil(L);
    திரும்பு 1;
}

    நிலையான முழு
luaV_blob_newindex(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    எனில் (b->bv_lock)
	luaL_error(L, "blob is locked");

    எனில் (!lua_isnumber(L, 2))
	திரும்பு 0;

    long len = blob_len(b);
    முழு idx = luaL_checkinteger(L, 2);
    முழு val = luaL_checkinteger(L, 3);
    எனில் (idx < 0)
	// Negative index is relative to the end.
	idx += (முழு) len;
    எனில் ((idx >= 0 && idx < len)
	    || (idx == len && ga_grow(&b->bv_ga, 1) == OK))
    {
	blob_set(b, idx, (char_u) val);
	எனில் (idx == len)
	    ++b->bv_ga.ga_len;
    }
    இல்லையெனில்
	luaL_error(L, "index out of range");

    திரும்பு 0;
}

    நிலையான முழு
luaV_blob_add(lua_State *L)
{
    luaV_Blob *blo = luaV_checkudata(L, 1, LUAVIM_BLOB);
    blob_T *b = (blob_T *) luaV_checkcache(L, (வெறுமை *) *blo);
    எனில் (b->bv_lock)
	luaL_error(L, "blob is locked");
    lua_settop(L, 2);
    எனில் (!lua_isstring(L, 2))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 2));
    இல்லையெனில்
    {
	size_t i, l = 0;
	மாறிலி எழுத்து *s = lua_tolstring(L, 2, &l);

	எனில் (ga_grow(&b->bv_ga, (முழு)l) == OK)
	    க்கு (i = 0; i < l; ++i)
		ga_append(&b->bv_ga, s[i]);
    }
    lua_settop(L, 1);
    திரும்பு 1;
}

நிலையான மாறிலி luaL_Reg luaV_Blob_mt[] = {
    {"__tostring", luaV_blob_tostring},
    {"__gc", luaV_blob_gc},
    {"__len", luaV_blob_len},
    {"__index", luaV_blob_index},
    {"__newindex", luaV_blob_newindex},
    {"add", luaV_blob_add},
    {வெற்று, வெற்று}
};


// =======   Funcref type   =======

    நிலையான luaV_Funcref *
luaV_newfuncref(lua_State *L, char_u *name)
{
    luaV_Funcref *f = (luaV_Funcref *)lua_newuserdata(L, அளவு(luaV_Funcref));

    எனில் (name != வெற்று)
    {
	func_ref(name);
	f->name = vim_strsave(name);
    }
    f->self = வெற்று;
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_setmetatable(L, -2);
    திரும்பு f;
}

    நிலையான luaV_Funcref *
luaV_pushfuncref(lua_State *L, char_u *name)
{
    திரும்பு luaV_newfuncref(L, name);
}


luaV_type_tostring(funcref, LUAVIM_FUNCREF)

    நிலையான முழு
luaV_funcref_gc(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    func_unref(f->name);
    vim_free(f->name);
    // NOTE: Don't call "dict_unref(f->self)", because the dict of "f->self"
    // will be (or has been already) freed by Vim's garbage collection.
    திரும்பு 0;
}

// equivalent to string(funcref)
    நிலையான முழு
luaV_funcref_len(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    lua_pushstring(L, (மாறிலி எழுத்து *) f->name);
    திரும்பு 1;
}

    நிலையான முழு
luaV_funcref_call(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);
    முழு i, n = lua_gettop(L) - 1; // #args
    முழு status = FAIL;
    typval_T args;
    typval_T rettv;

    args.v_type = VAR_LIST;
    args.vval.v_list = list_alloc();
    rettv.v_type = VAR_UNKNOWN; // as in clear_tv
    எனில் (args.vval.v_list != வெற்று)
    {
	typval_T v;

	க்கு (i = 0; i < n; i++)
	{
	    luaV_checktypval(L, i + 2, &v, "calling funcref");
	    list_append_tv(args.vval.v_list, &v);
	    clear_tv(&v);
	}
	status = func_call(f->name, &args, வெற்று, f->self, &rettv);
	எனில் (status == OK)
	    luaV_pushtypval(L, &rettv);
	clear_tv(&args);
	clear_tv(&rettv);
    }
    எனில் (status != OK)
	luaL_error(L, "cannot call funcref");
    திரும்பு 1;
}

நிலையான மாறிலி luaL_Reg luaV_Funcref_mt[] = {
    {"__tostring", luaV_funcref_tostring},
    {"__gc", luaV_funcref_gc},
    {"__len", luaV_funcref_len},
    {"__call", luaV_funcref_call},
    {வெற்று, வெற்று}
};


// =======   Buffer type   =======

luaV_newtype(buf_T, buffer, luaV_Buffer, LUAVIM_BUFFER)
luaV_pushtype(buf_T, buffer, luaV_Buffer)
luaV_type_tostring(buffer, LUAVIM_BUFFER)

    நிலையான முழு
luaV_buffer_len(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    lua_pushinteger(L, b->b_ml.ml_வரி_மொத்தம்);
    திரும்பு 1;
}

    நிலையான முழு
luaV_buffer_call(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    lua_settop(L, 1);
    set_curbuf(b, DOBUF_SPLIT);
    திரும்பு 1;
}

    நிலையான முழு
luaV_buffer_index(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    linenr_T n = (linenr_T) lua_tointeger(L, 2);
    எனில் (n > 0 && n <= b->b_ml.ml_வரி_மொத்தம்)
	luaV_pushline(L, b, n);
    இல்லையெனில் எனில் (lua_isstring(L, 2))
    {
	மாறிலி எழுத்து *s = lua_tostring(L, 2);
	எனில் (strncmp(s, "name", 4) == 0)
	    lua_pushstring(L, (b->b_sfname == வெற்று)
					? "" : (எழுத்து *) b->b_sfname);
	இல்லையெனில் எனில் (strncmp(s, "fname", 5) == 0)
	    lua_pushstring(L, (b->b_ffname == வெற்று)
					? "" : (எழுத்து *) b->b_ffname);
	இல்லையெனில் எனில் (strncmp(s, "number", 6) == 0)
	    lua_pushinteger(L, b->b_fnum);
	// methods
	இல்லையெனில் எனில் (strncmp(s,   "insert", 6) == 0
		|| strncmp(s, "next", 4) == 0
		|| strncmp(s, "previous", 8) == 0
		|| strncmp(s, "isvalid", 7) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	இல்லையெனில்
	    lua_pushnil(L);
    }
    இல்லையெனில்
	lua_pushnil(L);
    திரும்பு 1;
}

    நிலையான முழு
luaV_buffer_newindex(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    linenr_T n = (linenr_T) luaL_checkinteger(L, 2);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    எனில் (n < 1 || n > b->b_ml.ml_வரி_மொத்தம்)
	luaL_error(L, "invalid line number");
    எனில் (lua_isnil(L, 3)) // delete line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	எனில் (u_savedel(n, 1L) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	இல்லையெனில் எனில் (ml_delete(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot delete line");
	}
	இல்லையெனில்
	{
	    deleted_வரிகள்_mark(n, 1L);
	    எனில் (b == curwin->w_buffer) // fix cursor in current window?
	    {
		எனில் (curwin->w_cursor.lnum >= n)
		{
		    எனில் (curwin->w_cursor.lnum > n)
		    {
			curwin->w_cursor.lnum -= 1;
			check_cursor_col();
		    }
		    இல்லையெனில்
			check_cursor();
		    changed_cline_bef_curs();
		}
		invalidate_botline();
	    }
	}
	curbuf = buf;
    }
    இல்லையெனில் எனில் (lua_isstring(L, 3)) // update line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	எனில் (u_savesub(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	இல்லையெனில் எனில் (ml_replace(n, luaV_toline(L, 3), TRUE) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot replace line");
	}
	இல்லையெனில்
	    changed_bytes(n, 0);
	curbuf = buf;
	எனில் (b == curwin->w_buffer)
	    check_cursor_col();
    }
    இல்லையெனில்
	luaL_error(L, "wrong argument to change line");
    திரும்பு 0;
}

    நிலையான முழு
lua_V_buffer_செருகு(lua_State *L)
{
    luaV_Buffer *lb = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *b = (buf_T *) luaV_checkcache(L, (வெறுமை *) *lb);
    linenr_T last = b->b_ml.ml_வரி_மொத்தம்;
    linenr_T n = (linenr_T) luaL_optinteger(L, 3, last);
    buf_T *buf;
    luaL_checktype(L, 2, LUA_TSTRING);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    // fix insertion line
    எனில் (n < 0) n = 0;
    எனில் (n > last) n = last;
    // insert
    buf = curbuf;
    curbuf = b;
    எனில் (u_save(n, n + 1) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot save undo information");
    }
    இல்லையெனில் எனில் (ml_append(n, luaV_toline(L, 2), 0, FALSE) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot insert line");
    }
    இல்லையெனில்
	appended_வரிகள்_mark(n, 1L);
    curbuf = buf;
    update_screen(UPD_VALID);
    திரும்பு 0;
}

    நிலையான முழு
luaV_buffer_next(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (வெறுமை *) *b);
    luaV_pushbuffer(L, buf->b_next);
    திரும்பு 1;
}

    நிலையான முழு
luaV_buffer_previous(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (வெறுமை *) *b);
    luaV_pushbuffer(L, buf->b_prev);
    திரும்பு 1;
}

    நிலையான முழு
luaV_buffer_isvalid(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    luaV_getudata(L, *b);
    lua_pushboolean(L, !lua_isnil(L, -1));
    திரும்பு 1;
}

நிலையான மாறிலி luaL_Reg luaV_Buffer_mt[] = {
    {"__tostring", luaV_buffer_tostring},
    {"__len", luaV_buffer_len},
    {"__call", luaV_buffer_call},
    {"__index", luaV_buffer_index},
    {"__newindex", luaV_buffer_newindex},
    {"insert", lua_V_buffer_செருகு},
    {"next", luaV_buffer_next},
    {"previous", luaV_buffer_previous},
    {"isvalid", luaV_buffer_isvalid},
    {வெற்று, வெற்று}
};


// =======   Window type   =======

luaV_newtype(win_T, window, luaV_Window, LUAVIM_WINDOW)
luaV_pushtype(win_T, window, luaV_Window)
luaV_type_tostring(window, LUAVIM_WINDOW)

    நிலையான முழு
luaV_window_call(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    lua_settop(L, 1);
    win_goto(w);
    திரும்பு 1;
}

    நிலையான முழு
luaV_window_index(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    மாறிலி எழுத்து *s = luaL_checkstring(L, 2);
    எனில் (strncmp(s, "buffer", 6) == 0)
	luaV_pushbuffer(L, w->w_buffer);
    இல்லையெனில் எனில் (strncmp(s, "line", 4) == 0)
	lua_pushinteger(L, w->w_cursor.lnum);
    இல்லையெனில் எனில் (strncmp(s, "col", 3) == 0)
	lua_pushinteger(L, w->w_cursor.col + 1);
    இல்லையெனில் எனில் (strncmp(s, "width", 5) == 0)
	lua_pushinteger(L, w->w_width);
    இல்லையெனில் எனில் (strncmp(s, "height", 6) == 0)
	lua_pushinteger(L, w->w_height);
    // methods
    இல்லையெனில் எனில் (strncmp(s,   "next", 4) == 0
	    || strncmp(s, "previous", 8) == 0
	    || strncmp(s, "isvalid", 7) == 0)
    {
	lua_getmetatable(L, 1);
	lua_getfield(L, -1, s);
    }
    இல்லையெனில்
	lua_pushnil(L);
    திரும்பு 1;
}

    நிலையான முழு
luaV_window_newindex(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    மாறிலி எழுத்து *s = luaL_checkstring(L, 2);
    முழு v = luaL_checkinteger(L, 3);
    எனில் (strncmp(s, "line", 4) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	எனில் (v < 1 || v > w->w_buffer->b_ml.ml_வரி_மொத்தம்)
	    luaL_error(L, "line out of range");
	w->w_cursor.lnum = v;
	update_screen(UPD_VALID);
    }
    இல்லையெனில் எனில் (strncmp(s, "col", 3) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	w->w_cursor.col = v - 1;
	w->w_set_curswant = true;
	update_screen(UPD_VALID);
    }
    இல்லையெனில் எனில் (strncmp(s, "width", 5) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setwidth(v);
	curwin = win;
    }
    இல்லையெனில் எனில் (strncmp(s, "height", 6) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setheight(v);
	curwin = win;
    }
    இல்லையெனில்
	luaL_error(L, "invalid window property: `%s'", s);
    திரும்பு 0;
}

    நிலையான முழு
luaV_window_next(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    win_T *win = (win_T *) luaV_checkcache(L, (வெறுமை *) *w);
    luaV_pushwindow(L, win->w_next);
    திரும்பு 1;
}

    நிலையான முழு
luaV_window_previous(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    win_T *win = (win_T *) luaV_checkcache(L, (வெறுமை *) *w);
    luaV_pushwindow(L, win->w_prev);
    திரும்பு 1;
}

    நிலையான முழு
luaV_window_isvalid(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    luaV_getudata(L, *w);
    lua_pushboolean(L, !lua_isnil(L, -1));
    திரும்பு 1;
}

நிலையான மாறிலி luaL_Reg luaV_Window_mt[] = {
    {"__tostring", luaV_window_tostring},
    {"__call", luaV_window_call},
    {"__index", luaV_window_index},
    {"__newindex", luaV_window_newindex},
    {"next", luaV_window_next},
    {"previous", luaV_window_previous},
    {"isvalid", luaV_window_isvalid},
    {வெற்று, வெற்று}
};


// =======   Vim module   =======

    நிலையான முழு
luaV_print(lua_State *L)
{
    முழு		i, n = lua_gettop(L); // nargs
    மாறிலி எழுத்து	*s;
    size_t	l;
    garray_T	செய்தி_ga;

    ga_init2(&செய்தி_ga, 1, 128);
    lua_getglobal(L, "tostring");
    க்கு (i = 1; i <= n; i++)
    {
	lua_pushvalue(L, -1); // tostring
	lua_pushvalue(L, i); // arg
	lua_call(L, 1, 1);
	s = lua_tolstring(L, -1, &l);
	எனில் (s == வெற்று)
	    திரும்பு luaL_error(L, "cannot convert to string");
	எனில் (i > 1)
	    ga_append(&செய்தி_ga, ' '); // use space instead of tab
	ga_concat_len(&செய்தி_ga, (char_u *)s, l);
	lua_எடு(L, 1);
    }
    // Replace any "\n" with "\0"
    க்கு (i = 0; i < செய்தி_ga.ga_len; i++)
	எனில் (((எழுத்து *)செய்தி_ga.ga_data)[i] == '\n')
	    ((எழுத்து *)செய்தி_ga.ga_data)[i] = '\0';
    lua_pushlstring(L, செய்தி_ga.ga_data, செய்தி_ga.ga_len);
    எனில் (!got_int)
	lua_V_செய்தி(L);

    ga_clear(&செய்தி_ga);
    திரும்பு 0;
}

    நிலையான முழு
luaV_debug(lua_State *L)
{
    lua_settop(L, 0);
    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "eval");
    lua_remove(L, -2); // vim.eval at position 1
    க்கு (;;)
    {
	மாறிலி எழுத்து *உள்ளீடு;
	size_t l;
	lua_pushvalue(L, 1); // vim.eval
	lua_pushliteral(L, "input('lua_debug> ')");
	lua_call(L, 1, 1); // return string
	உள்ளீடு = lua_tolstring(L, -1, &l);
	எனில் (l == 0 || strcmp(உள்ளீடு, "cont") == 0)
	    திரும்பு 0;
	செய்தி_putchar('\n'); // avoid outputting on input line
	எனில் (luaL_loadbuffer(L, உள்ளீடு, l, "=(debug command)")
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
	lua_settop(L, 1); // remove eventual returns, but keep vim.eval
    }
}

    நிலையான dict_T *
luaV_get_var_scope(lua_State *L)
{
    மாறிலி எழுத்து	*scope = luaL_checkstring(L, 1);
    dict_T	*dict = வெற்று;

    எனில் (STRICMP((எழுத்து *)scope, "g") == 0)
	dict = get_globvar_dict();
    இல்லையெனில் எனில் (STRICMP((எழுத்து *)scope, "v") == 0)
	dict = get_vimvar_dict();
    இல்லையெனில் எனில் (STRICMP((எழுத்து *)scope, "b") == 0)
	dict = curbuf->b_vars;
    இல்லையெனில் எனில் (STRICMP((எழுத்து *)scope, "w") == 0)
	dict = curwin->w_vars;
    இல்லையெனில் எனில் (STRICMP((எழுத்து *)scope, "t") == 0)
	dict = curtab->tp_vars;
    இல்லையெனில்
    {
	luaL_error(L, "invalid scope %s", scope);
	திரும்பு வெற்று;
    }

    திரும்பு dict;
}

    நிலையான முழு
luaV_setvar(lua_State *L)
{
    dict_T	*dict;
    dictitem_T	*di;
    size_t	len;
    எழுத்து	*name;
    முழு		del;
    எழுத்து	*error = வெற்று;

    name = (எழுத்து *)luaL_checklstring(L, 3, &len);
    del = (lua_gettop(L) < 4) || lua_isnil(L, 4);

    dict = luaV_get_var_scope(L);
    எனில் (dict == வெற்று)
	திரும்பு 0;

    di = dict_find(dict, (char_u *)name, (முழு)len);
    எனில் (di != வெற்று)
    {
	எனில் (di->di_flags & DI_FLAGS_RO)
	    error = "variable is read-only";
	இல்லையெனில் எனில் (di->di_flags & DI_FLAGS_LOCK)
	    error = "variable is locked";
	இல்லையெனில் எனில் (del && di->di_flags & DI_FLAGS_FIX)
	    error = "variable is fixed";
	எனில் (error != வெற்று)
	    திரும்பு luaL_error(L, error);
    }
    இல்லையெனில் எனில் (dict->dv_lock)
	திரும்பு luaL_error(L, "Dictionary is locked");

    எனில் (del)
    {
	// Delete the key
	எனில் (di == வெற்று)
	    // Doesn't exist, nothing to do
	    திரும்பு 0;
	// Delete the entry
	dictitem_remove(dict, di, "Lua delete variable");
    }
    இல்லையெனில்
    {
	// Update the key
	typval_T	tv;

	// Convert the lua value to a Vim script type in the temporary variable
	lua_pushvalue(L, 4);
	எனில் (luaV_totypval(L, -1, &tv) == FAIL)
	    திரும்பு luaL_error(L, "Couldn't convert lua value");

	எனில் (di == வெற்று)
	{
	    // Need to create an entry
	    di = dictitem_alloc((char_u *)name);
	    எனில் (di == வெற்று)
	    {
		clear_tv(&tv);
		திரும்பு 0;
	    }
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	    எனில் (dict_add(dict, di) == FAIL)
	    {
		dictitem_free(di);
		clear_tv(&tv);
		திரும்பு luaL_error(L, "Couldn't add to dictionary");
	    }
	}
	இல்லையெனில்
	{
	    முழு type_error = FALSE;
	    எனில் (dict == get_vimvar_dict()
	       && !before_set_vvar((char_u *)name, di, &tv, TRUE, &type_error))
	    {
		clear_tv(&tv);
		எனில் (type_error)
		    திரும்பு luaL_error(L,
				"Setting v:%s to value with wrong type", name);
		திரும்பு 0;
	    }
	    // Clear the old value
	    clear_tv(&di->di_tv);
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	}

	// Clear the temporary variable
	clear_tv(&tv);
    }

    திரும்பு 0;
}

    நிலையான முழு
luaV_getvar(lua_State *L)
{
    dict_T	*dict = luaV_get_var_scope(L);
    size_t	len;
    மாறிலி எழுத்து	*name = luaL_checklstring(L, 3, &len);
    dictitem_T	*di = dict_find(dict, (char_u *)name, (முழு)len);

    எனில் (di == வெற்று)
	திரும்பு 0;  // nil

    luaV_pushtypval(L, &di->di_tv);
    திரும்பு 1;
}

    நிலையான முழு
luaV_command(lua_State *L)
{
    char_u  *s = vim_strsave((char_u *)luaL_checkstring(L, 1));

    execute_cmds_from_string(s);
    vim_free(s);
    update_screen(UPD_VALID);
    திரும்பு 0;
}

    நிலையான முழு
luaV_eval(lua_State *L)
{
    typval_T *tv = eval_கோவை((char_u *) luaL_checkstring(L, 1), வெற்று);

    எனில் (tv == வெற்று) luaL_error(L, "invalid expression");
    luaV_pushtypval(L, tv);
    free_tv(tv);
    திரும்பு 1;
}

    நிலையான முழு
luaV_beep(lua_State *L UNUSED)
{
    vim_beep(BO_LANG);
    திரும்பு 0;
}

    நிலையான முழு
lua_V_வரி(lua_State *L)
{
    luaV_pushline(L, curbuf, curwin->w_cursor.lnum);
    திரும்பு 1;
}

    நிலையான முழு
luaV_list(lua_State *L)
{
    list_T *l;
    முழு initarg = !lua_isnoneornil(L, 1);

    எனில் (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    l = list_alloc();
    எனில் (l == வெற்று)
    {
	lua_pushnil(L);
	திரும்பு 1;
    }

    luaV_newlist(L, l);
    எனில் (!initarg)
	திரும்பு 1;

    // traverse table to init list
    முழு notnil, i = 0;
    typval_T v;
    செய்
    {
	lua_rawgeti(L, 1, ++i);
	notnil = !lua_isnil(L, -1);
	எனில் (notnil)
	{
	    luaV_checktypval(L, -1, &v, "vim.list");
	    list_append_tv(l, &v);
	    clear_tv(&v);
	}
	lua_எடு(L, 1); //மதிப்பு
    } போது (notnil);

    திரும்பு 1;
}

    நிலையான முழு
luaV_dict(lua_State *L)
{
    dict_T *d;
    முழு initarg = !lua_isnoneornil(L, 1);

    எனில் (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    d = dict_alloc();
    எனில் (d == வெற்று)
    {
	lua_pushnil(L);
	திரும்பு 1;
    }

    luaV_newdict(L, d);
    எனில் (!initarg)
	திரும்பு 1;

    // traverse table to init dict
    lua_pushnil(L);
    போது (lua_next(L, 1))
    {
	char_u *சாவி;
	dictitem_T *di;
	typval_T v;

	lua_pushvalue(L, -2); // dup key in case it's a number
	சாவி = (char_u *) lua_tostring(L, -1);
	எனில் (சாவி == வெற்று)
	{
	    lua_pushnil(L);
	    திரும்பு 1;
	}
	எனில் (*சாவி == NUL)
	    luaL_error(L, "table has empty key");
	luaV_checktypval(L, -2, &v, "vim.dict"); //மதிப்பு
	di = dictitem_alloc(சாவி);
	எனில் (di == வெற்று || dict_add(d, di) == FAIL)
	{
	    vim_free(di);
	    lua_pushnil(L);
	    திரும்பு 1;
	}
	di->di_tv = v;
	lua_எடு(L, 2); // key copy and value
    }

    திரும்பு 1;
}

    நிலையான முழு
luaV_blob(lua_State *L)
{
    blob_T *b;
    முழு initarg = !lua_isnoneornil(L, 1);

    எனில் (initarg && !lua_isstring(L, 1))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 1));

    b = blob_alloc();
    எனில் (b == வெற்று)
    {
	lua_pushnil(L);
	திரும்பு 1;
    }

    luaV_newblob(L, b);
    எனில் (!initarg)
	திரும்பு 1;

    // traverse table to init blob
    size_t i, l = 0;
    மாறிலி எழுத்து *s = lua_tolstring(L, 1, &l);

    எனில் (ga_grow(&b->bv_ga, (முழு)l) == OK)
	க்கு (i = 0; i < l; ++i)
	    ga_append(&b->bv_ga, s[i]);

    திரும்பு 1;
}

    நிலையான முழு
luaV_funcref(lua_State *L)
{
    மாறிலி எழுத்து *name = luaL_checkstring(L, 1);
    // note: not checking if function exists (needs function_exists)
    எனில் (name == வெற்று || *name == NUL || VIM_ISDIGIT(*name))
	luaL_error(L, "invalid function name: %s", name);
    luaV_newfuncref(L, (char_u *) name);
    திரும்பு 1;
}

    நிலையான முழு
luaV_buffer(lua_State *L)
{
    buf_T *buf;
    எனில் (lua_isstring(L, 1)) // get by number or name?
    {
	எனில் (lua_isnumber(L, 1)) // by number?
	{
	    முழு n = lua_tointeger(L, 1);
	    FOR_ALL_BUFFERS(buf)
		எனில் (buf->b_fnum == n) நிறுத்து;
	}
	இல்லையெனில் // by name
	{
	    size_t l;
	    மாறிலி எழுத்து *s = lua_tolstring(L, 1, &l);
	    FOR_ALL_BUFFERS(buf)
	    {
		எனில் (buf->b_ffname == வெற்று || buf->b_sfname == வெற்று)
		{
		    எனில் (l == 0) நிறுத்து;
		}
		இல்லையெனில் எனில் (strncmp(s, (எழுத்து *)buf->b_ffname, l) == 0
			|| strncmp(s, (எழுத்து *)buf->b_sfname, l) == 0)
		    நிறுத்து;
	    }
	}
    }
    இல்லையெனில்
	buf = (lua_toboolean(L, 1)) ? firstbuf : curbuf; // first buffer?
    luaV_pushbuffer(L, buf);
    திரும்பு 1;
}

    நிலையான முழு
luaV_window(lua_State *L)
{
    win_T *win;
    எனில் (lua_isnumber(L, 1)) // get by number?
    {
	முழு n = lua_tointeger(L, 1);
	க்கு (win = firstwin; win != வெற்று; win = win->w_next, n--)
	    எனில் (n == 1) நிறுத்து;
    }
    இல்லையெனில்
	win = (lua_toboolean(L, 1)) ? firstwin : curwin; // first window?
    luaV_pushwindow(L, win);
    திரும்பு 1;
}

    நிலையான முழு
luaV_open(lua_State *L)
{
    char_u *s = வெற்று;
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    எனில் (lua_isstring(L, 1)) s = (char_u *) lua_tostring(L, 1);
    luaV_pushbuffer(L, buflist_new(s, வெற்று, 1L, BLN_LISTED));
    திரும்பு 1;
}

    நிலையான முழு
luaV_type(lua_State *L)
{
    luaL_checkany(L, 1);
    எனில் (lua_type(L, 1) == LUA_TUSERDATA) // check vim udata?
    {
	lua_settop(L, 1);
	எனில் (lua_getmetatable(L, 1))
	{
	    luaV_getfield(L, LUAVIM_LIST);
	    எனில் (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "list");
		திரும்பு 1;
	    }
	    luaV_getfield(L, LUAVIM_DICT);
	    எனில் (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "dict");
		திரும்பு 1;
	    }
	    luaV_getfield(L, LUAVIM_BLOB);
	    எனில் (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "blob");
		திரும்பு 1;
	    }
	    luaV_getfield(L, LUAVIM_FUNCREF);
	    எனில் (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "funcref");
		திரும்பு 1;
	    }
	    luaV_getfield(L, LUAVIM_BUFFER);
	    எனில் (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "buffer");
		திரும்பு 1;
	    }
	    luaV_getfield(L, LUAVIM_WINDOW);
	    எனில் (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "window");
		திரும்பு 1;
	    }
	}
    }
    lua_pushstring(L, luaL_typename(L, 1)); // fallback
    திரும்பு 1;
}

    நிலையான முழு
luaV_call(lua_State *L)
{
    முழு		argc = lua_gettop(L) - 1;
    size_t	funcname_len;
    char_u	*funcname;
    எழுத்து	*error = வெற்று;
    typval_T	rettv;
    typval_T	argv[MAX_FUNC_ARGS + 1];
    முழு		i = 0;

    எனில் (argc > MAX_FUNC_ARGS)
	திரும்பு luaL_error(L, "Function called with too many arguments");

    funcname = (char_u *)luaL_checklstring(L, 1, &funcname_len);

    க்கு (; i < argc; i++)
    {
	எனில் (luaV_totypval(L, i + 2, &argv[i]) == FAIL)
	{
	    error = "lua: cannot convert value";
	    செல் free_vim_args;
	}
    }

    argv[argc].v_type = VAR_UNKNOWN;

    எனில் (call_vim_function(funcname, argc, argv, &rettv) == FAIL)
    {
	error = "lua: call_vim_function failed";
	செல் free_vim_args;
    }

    luaV_pushtypval(L, &rettv);
    clear_tv(&rettv);

free_vim_args:
    போது (i > 0)
	clear_tv(&argv[--i]);

    எனில் (error == வெற்று)
	திரும்பு 1;
    இல்லையெனில்
	திரும்பு luaL_error(L, error);
}

/*
 * Return the Vim version as a Lua table
 */
    நிலையான முழு
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
    திரும்பு 1;
}

நிலையான மாறிலி luaL_Reg luaV_module[] = {
    {"command", luaV_command},
    {"eval", luaV_eval},
    {"beep", luaV_beep},
    {"line", lua_V_வரி},
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
    {"lua_version", வெற்று},
    {வெற்று, வெற்று}
};

/*
 * for freeing list, dict, buffer and window objects; lightuserdata as arg
 */
    நிலையான முழு
luaV_free(lua_State *L)
{
    lua_pushnil(L);
    luaV_setudata(L, lua_touserdata(L, 1));
    திரும்பு 0;
}

    நிலையான முழு
luaV_luaeval(lua_State *L)
{
    luaL_Buffer b;
    size_t l;
    மாறிலி எழுத்து *str = lua_tolstring(L, 1, &l);
    typval_T *arg = (typval_T *) lua_touserdata(L, 2);
    typval_T *rettv = (typval_T *) lua_touserdata(L, 3);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, LUAVIM_EVALHEADER, அளவு(LUAVIM_EVALHEADER) - 1);
    luaL_addlstring(&b, str, l);
    luaL_pushresult(&b);
    str = lua_tolstring(L, -1, &l);
    எனில் (luaL_loadbuffer(L, str, l, LUAVIM_EVALNAME)) // compile error?
    {
	luaV_emsg(L);
	திரும்பு 0;
    }
    luaV_pushtypval(L, arg);
    எனில் (lua_pcall(L, 1, 1, 0)) // running error?
    {
	luaV_emsg(L);
	திரும்பு 0;
    }
    எனில் (luaV_totypval(L, -1, rettv) == FAIL)
	emsg("luaeval: cannot convert value");
    திரும்பு 0;
}

    நிலையான முழு
luaV_setref(lua_State *L)
{
    முழு copyID = lua_tointeger(L, 1);
    முழு abort = FALSE;

    lua_pushlightuserdata(L, (வெறுமை *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // the cache table

    luaV_getfield(L, LUAVIM_LIST);
    luaV_getfield(L, LUAVIM_DICT);
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_pushnil(L);
    // traverse cache table
    போது (!abort && lua_next(L, 2) != 0)
    {
	lua_getmetatable(L, -1);
	எனில் (lua_rawequal(L, -1, 3)) // list?
	{
	    list_T *l = (list_T *)lua_touserdata(L, 6); //சாவி

	    abort = set_ref_in_list(l, copyID);
	}
	இல்லையெனில் எனில் (lua_rawequal(L, -1, 4)) // dict?
	{
	    dict_T *d = (dict_T *)lua_touserdata(L, 6); //சாவி

	    abort = set_ref_in_dict(d, copyID);
	}
	இல்லையெனில் எனில் (lua_rawequal(L, -1, 5)) // funcref?
	{
	    luaV_Funcref *f = (luaV_Funcref *)lua_touserdata(L, 6); //சாவி

	    abort = set_ref_in_dict(f->self, copyID);
	}
	lua_எடு(L, 2); // metatable and value
    }
    lua_pushinteger(L, abort);
    திரும்பு 1;
}

    நிலையான முழு
luaV_pushversion(lua_State *L)
{
    முழு major = 0;
    முழு minor = 0;
    முழு patch = 0;
    எழுத்து s[16];

    sscanf(LUAVIM_VERSION, "Lua %d.%d.%d", &major, &minor, &patch);
    vim_snprintf(s, அளவு(s), "%d.%d.%d", major, minor, patch);
    lua_pushstring(L, s);
    திரும்பு 0;
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

    நிலையான முழு
luaopen_vim(lua_State *L)
{
    lua_newtable(L);  // cache table
    lua_newtable(L);  // cache table's metatable
    lua_pushstring(L, "v");
    lua_setfield(L, -2, "__mode");
    lua_setmetatable(L, -2); // cache is weak-valued
    // put the cache table in the registry for luaV_get/setudata()
    lua_pushlightuserdata(L, (வெறுமை *) LUAVIM_UDATA_CACHE);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
    lua_எடு(L, 1);  // we don't need the cache table here anymore
    // print
    lua_pushcfunction(L, luaV_print);
    lua_setglobal(L, "print");
    // debug.debug
    lua_getglobal(L, "debug");
    lua_pushcfunction(L, luaV_debug);
    lua_setfield(L, -2, "debug");
    lua_எடு(L, 1);
    // free
    lua_pushlightuserdata(L, (வெறுமை *) LUAVIM_FREE);
    lua_pushcfunction(L, luaV_free);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // luaeval
    lua_pushlightuserdata(L, (வெறுமை *) LUAVIM_LUAEVAL);
    lua_pushcfunction(L, luaV_luaeval);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // setref
    lua_pushlightuserdata(L, (வெறுமை *) LUAVIM_SETREF);
    lua_pushcfunction(L, luaV_setref);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // register
    luaV_newmetatable(L, LUAVIM_LIST);
    luaV_register(L, luaV_List_mt);
    lua_எடு(L, 1);
    luaV_newmetatable(L, LUAVIM_DICT);
    luaV_register(L, luaV_Dict_mt);
    lua_எடு(L, 1);
    luaV_newmetatable(L, LUAVIM_BLOB);
    luaV_register(L, luaV_Blob_mt);
    lua_எடு(L, 1);
    luaV_newmetatable(L, LUAVIM_FUNCREF);
    luaV_register(L, luaV_Funcref_mt);
    lua_எடு(L, 1);
    luaV_newmetatable(L, LUAVIM_BUFFER);
    luaV_register(L, luaV_Buffer_mt);
    lua_எடு(L, 1);
    luaV_newmetatable(L, LUAVIM_WINDOW);
    luaV_register(L, luaV_Window_mt);
    lua_எடு(L, 1);
    lua_newtable(L); // vim table
    luaV_register(L, luaV_module);
    luaV_pushversion(L);
    lua_setfield(L, -2, "lua_version");
    lua_setglobal(L, LUAVIM_NAME);
    // custom code
    (வெறுமை)luaL_dostring(L, LUA_VIM_FN_CODE);
    (வெறுமை)luaL_dostring(L, LUA_VIM_UPDATE_PACKAGE_PATHS);
    (வெறுமை)luaL_dostring(L, LUA_VIM_SETUP_VARIABLE_DICTS);

    lua_getglobal(L, LUAVIM_NAME);
    lua_getfield(L, -1, "_update_package_paths");

    எனில் (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);

    திரும்பு 0;
}

    நிலையான lua_State *
luaV_newstate(வெறுமை)
{
    lua_State *L = luaL_newstate();
# எனில் LUA_VERSION_இலக்கம் >= 505
    luaL_openselectedlibs(L, ~0, 0); // core libs
# இல்லையெனில்
    luaL_openlibs(L); // core libs
# endif
    lua_pushcfunction(L, luaopen_vim); // vim
    lua_call(L, 0, 0);
    திரும்பு L;
}

    நிலையான வெறுமை
luaV_setrange(lua_State *L, முழு line1, முழு line2)
{
    lua_getglobal(L, LUAVIM_NAME);
    lua_pushinteger(L, line1);
    lua_setfield(L, -2, "firstline");
    lua_pushinteger(L, line2);
    lua_setfield(L, -2, "lastline");
    lua_எடு(L, 1); // vim table
}


// =======   Interface   =======

நிலையான lua_State *L = வெற்று;

    நிலையான முழு
lua_isopen(வெறுமை)
{
    திரும்பு L != வெற்று;
}

    நிலையான முழு
lua_init(வெறுமை)
{
    எனில் (lua_isopen())
	திரும்பு OK;

# ifdef DYNAMIC_LUA
    எனில் (!lua_enabled(TRUE))
    {
	emsg(_("Lua library cannot be loaded."));
	திரும்பு FAIL;
    }
# endif
    L = luaV_newstate();

    திரும்பு OK;
}

    வெறுமை
lua_end(வெறுமை)
{
    எனில் (!lua_isopen())
	திரும்பு;

    lua_close(L);
    L = வெற்று;
}

/*
 * ex commands
 */
    வெறுமை
ex_lua(exarg_T *eap)
{
    எழுத்து *script = (எழுத்து *)script_get(eap, eap->arg);

    எனில் (!eap->skip && lua_init() == OK)
    {
	எழுத்து *s = script != வெற்று ? script : (எழுத்து *)eap->arg;

	luaV_setrange(L, eap->line1, eap->line2);
	எனில் (luaL_loadbuffer(L, s, strlen(s), LUAVIM_CHUNKNAME)
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
    எனில் (script != வெற்று)
	vim_free(script);
}

    வெறுமை
ex_luado(exarg_T *eap)
{
    linenr_T l;
    மாறிலி எழுத்து *s = (மாறிலி எழுத்து *) eap->arg;
    luaL_Buffer b;
    size_t len;
    buf_T *was_curbuf = curbuf;

    எனில் (lua_init() == FAIL) திரும்பு;
    எனில் (u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
    {
	emsg(_("cannot save undo information"));
	திரும்பு;
    }
    luaV_setrange(L, eap->line1, eap->line2);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, "return function(line, linenr) ", 30); // header
    luaL_addlstring(&b, s, strlen(s));
    luaL_addlstring(&b, " end", 4); // footer
    luaL_pushresult(&b);
    s = lua_tolstring(L, -1, &len);
    எனில் (luaL_loadbuffer(L, s, len, LUAVIM_CHUNKNAME))
    {
	luaV_emsg(L);
	lua_எடு(L, 1); // function body
	திரும்பு;
    }
    lua_call(L, 0, 1);
    lua_replace(L, -2); // function -> body
    க்கு (l = eap->line1; l <= eap->line2; l++)
    {
	// Check the line number, the command may have deleted lines.
	எனில் (l > curbuf->b_ml.ml_வரி_மொத்தம்)
	    நிறுத்து;

	lua_pushvalue(L, -1); // function
	luaV_pushline(L, curbuf, l); // current line as arg
	lua_pushinteger(L, l); // current line number as arg
	எனில் (lua_pcall(L, 2, 1, 0))
	{
	    luaV_emsg(L);
	    நிறுத்து;
	}

	// Catch the command switching to another buffer.
	// Check the line number, the command may have deleted lines.
	எனில் (curbuf != was_curbuf || l > curbuf->b_ml.ml_வரி_மொத்தம்)
	    நிறுத்து;

	எனில் (lua_isstring(L, -1)) // update line?
	{
# ifdef HAVE_SANDBOX
	    luaV_checksandbox(L);
# endif
	    ml_replace(l, luaV_toline(L, -1), TRUE);
	    changed_bytes(l, 0);
	    lua_எடு(L, 1); // result from luaV_toline
	}
	lua_எடு(L, 1); //வரி
    }
    lua_எடு(L, 1); // function
    check_cursor();
    update_screen(UPD_NOT_VALID);
}

    வெறுமை
ex_luafile(exarg_T *eap)
{
    எனில் (lua_init() == FAIL)
	திரும்பு;
    எனில் (!eap->skip)
    {
	luaV_setrange(L, eap->line1, eap->line2);
	எனில் (luaL_loadfile(L, (எழுத்து *) eap->arg) || lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
}

# define luaV_freetype(typ,tname) \
	வெறுமை \
    lua_##tname##_free(typ *o) \
    { \
	எனில் (!lua_isopen()) திரும்பு; \
	luaV_getfield(L, LUAVIM_FREE); \
	lua_pushlightuserdata(L, (வெறுமை *) o); \
	lua_call(L, 1, 0); \
    }

luaV_freetype(buf_T, buffer)
luaV_freetype(win_T, window)

    வெறுமை
do_luaeval(char_u *str, typval_T *arg, typval_T *rettv)
{
    lua_init();
    luaV_getfield(L, LUAVIM_LUAEVAL);
    lua_pushstring(L, (எழுத்து *) str);
    lua_pushlightuserdata(L, (வெறுமை *) arg);
    lua_pushlightuserdata(L, (வெறுமை *) rettv);
    lua_call(L, 3, 0);
}

    முழு
set_ref_in_lua(முழு copyID)
{
    முழு aborted = 0;

    எனில் (!lua_isopen())
	திரும்பு 0;

    luaV_getfield(L, LUAVIM_SETREF);
    // call the function with 1 arg, getting 1 result back
    lua_pushinteger(L, copyID);
    lua_call(L, 1, 1);
    // get the result
    aborted = lua_tointeger(L, -1);
    // pop result off the stack
    lua_எடு(L, 1);

    திரும்பு aborted;
}

    வெறுமை
update_package_paths_in_lua(வெறுமை)
{
    எனில் (!lua_isopen())
	திரும்பு;

    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "_update_package_paths");

    எனில் (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);
}

/*
 * Native C function callback
 */
    நிலையான முழு
luaV_call_lua_func(
	முழு	 argcount,
	typval_T *argvars,
	typval_T *rettv,
	வெறுமை	 *state)
{
    முழு i;
    முழு luaargcount = argcount;
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_funcref);

    எனில் (funcstate->lua_tableref != LUA_NOREF)
    {
	// First arg for metatable __call method is a table
	luaargcount += 1;
	lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    }

    க்கு (i = 0; i < argcount; ++i)
	luaV_pushtypval(funcstate->L, &argvars[i]);

    எனில் (lua_pcall(funcstate->L, luaargcount, 1, 0))
    {
	luaV_emsg(funcstate->L);
	திரும்பு (முழு)FCERR_OTHER;
    }

    luaV_checktypval(funcstate->L, -1, rettv, "get return value");
    திரும்பு (முழு)FCERR_NONE;
}

/*
 * Free up any lua references held by the func state.
 */
    நிலையான வெறுமை
luaV_call_lua_func_free(வெறுமை *state)
{
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_funcref);
    funcstate->L = வெற்று;
    எனில் (funcstate->lua_tableref != LUA_NOREF)
	luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    VIM_CLEAR(funcstate);
}

#endif
