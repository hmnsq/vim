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

#অন্তর্ভাব "vim.h"
#অন্তর্ভাব "version.h"

#অন্তর্ভাব <lua.h>
#অন্তর্ভাব <lualib.h>
#অন্তর্ভাব <lauxlib.h>

#যদি __STDC_VERSION__ >= 199901L
# define LUAV_INLINE অন্তঃরেখ
#অন্যথা
# define LUAV_INLINE
#endif

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#যদি defined(FEAT_LUA)

# define LUAVIM_CHUNKNAME "vim chunk"
# define LUAVIM_NAME "vim"
# define LUAVIM_EVALNAME "luaeval"
# define LUAVIM_EVALHEADER "local _A=select(1,...) return "

# ifdef LUA_RELEASE
#  define LUAVIM_VERSION LUA_RELEASE
# অন্যথা
#  define LUAVIM_VERSION LUA_VERSION
# endif

প্রকারলেখ buf_T *lua_V_বাফার;
প্রকারলেখ win_T *luaV_Window;
প্রকারলেখ dict_T *luaV_Dict;
প্রকারলেখ list_T *luaV_List;
প্রকারলেখ blob_T *luaV_Blob;
প্রকারলেখ কাঠামো {
    char_u	*name;	// funcref
    dict_T	*self;	// selfdict
} luaV_Funcref;
প্রকারলেখ পূর্ণসংখ্যা (*msgfunc_T)(অক্ষর *);

প্রকারলেখ কাঠামো {
    পূর্ণসংখ্যা lua_funcref;    // ref to a lua func
    পূর্ণসংখ্যা lua_tableref;   // ref to a lua table if metatable else LUA_NOREF. used
			// for __call
    lua_State *L;
} luaV_CFuncState;

স্থির ধ্রুবক অক্ষর LUAVIM_DICT[] = "dict";
স্থির ধ্রুবক অক্ষর LUAVIM_LIST[] = "list";
স্থির ধ্রুবক অক্ষর LUAVIM_BLOB[] = "blob";
স্থির ধ্রুবক অক্ষর LUAVIM_FUNCREF[] = "funcref";
স্থির ধ্রুবক অক্ষর LUAVIM_বাফার[] = "buffer";
স্থির ধ্রুবক অক্ষর LUAVIM_WINDOW[] = "window";
স্থির ধ্রুবক অক্ষর LUAVIM_FREE[] = "luaV_free";
স্থির ধ্রুবক অক্ষর LUAVIM_LUAEVAL[] = "luaV_luaeval";
স্থির ধ্রুবক অক্ষর LUAVIM_SETREF[] = "luaV_setref";

স্থির ধ্রুবক অক্ষর LUA___CALL[] = "__call";

// get/setudata manage references to vim userdata in a cache table through
// object pointers (light userdata). The cache table itself is retrieved
// from the registry.

স্থির ধ্রুবক অক্ষর LUAVIM_UDATA_CACHE[] = "luaV_udata_cache";

# define luaV_getfield(L, s) \
    lua_pushlightuserdata((L), (শূন্য *)(s)); \
    lua_rawget((L), LUA_REGISTRYINDEX)
# define luaV_checksandbox(L) \
    যদি (sandbox) luaL_error((L), "not allowed in sandbox")
# define lua_V_বার্তা(L) luaV_msgfunc((L), (msgfunc_T) বার্তা)
# define luaV_emsg(L) luaV_msgfunc((L), (msgfunc_T) emsg)
# define luaV_checktypval(L, a, v, বার্তা) \
    করো { \
	যদি (luaV_totypval(L, a, v) == FAIL) \
	    luaL_error(L, বার্তা ": cannot convert value"); \
    } যখন (0)

স্থির luaV_List *luaV_pushlist(lua_State *L, list_T *lis);
স্থির luaV_Dict *luaV_pushdict(lua_State *L, dict_T *dic);
স্থির luaV_Blob *luaV_pushblob(lua_State *L, blob_T *blo);
স্থির luaV_Funcref *luaV_pushfuncref(lua_State *L, char_u *name);
স্থির পূর্ণসংখ্যা luaV_call_lua_func(পূর্ণসংখ্যা argcount, typval_T *argvars, typval_T *rettv, শূন্য *state);
স্থির শূন্য luaV_call_lua_func_free(শূন্য *state);

# যদি LUA_VERSION_অঙ্ক <= 501
#  define luaV_register(L, l) luaL_register(L, ফাঁকা, l)
#  define luaL_typeerror luaL_typerror
# অন্যথা
#  define luaV_register(L, l) luaL_setfuncs(L, l, 0)
# endif

# ifdef DYNAMIC_LUA

#  ifdef MSWIN
#   define load_dll vimLoadLib
#   define প্রতীক_from_dll GetProcAddress
#   define close_dll FreeLibrary
#   define load_dll_error GetWin32Error
#  অন্যথা
#   include <dlfcn.h>
#   define HANDLE শূন্য*
#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#   define প্রতীক_from_dll dlsym
#   define close_dll dlclose
#   define load_dll_error dlerror
#  endif

// lauxlib
#  যদি LUA_VERSION_অঙ্ক <= 501
#   define luaL_register dll_luaL_register
#   define luaL_prepbuffer dll_luaL_prepbuffer
#   define luaL_openlib dll_luaL_openlib
#   define luaL_typerror dll_luaL_typerror
#   define luaL_loadfile dll_luaL_loadfile
#   define luaL_loadbuffer dll_luaL_loadbuffer
#  অন্যথা
#   define luaL_prepbuffsize dll_luaL_prepbuffsize
#   define luaL_setfuncs dll_luaL_setfuncs
#   define luaL_loadfilex dll_luaL_loadfilex
#   define luaL_loadbufferx dll_luaL_loadbufferx
#   define luaL_argerror dll_luaL_argerror
#  endif
#  যদি LUA_VERSION_অঙ্ক >= 504
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
#  যদি LUA_VERSION_অঙ্ক <= 501
#   define lua_tonumber dll_lua_tonumber
#   define lua_tointeger dll_lua_tointeger
#   define lua_call dll_lua_call
#   define lua_pcall dll_lua_pcall
#  অন্যথা
#   define lua_tonumberx dll_lua_tonumberx
#   define lua_tointegerx dll_lua_tointegerx
#   define lua_callk dll_lua_callk
#   define lua_pcallk dll_lua_pcallk
#   define lua_getglobal dll_lua_getglobal
#   define lua_setglobal dll_lua_setglobal
#  endif
#  যদি LUA_VERSION_অঙ্ক <= 502
#   define lua_replace dll_lua_replace
#   define lua_remove dll_lua_remove
#  endif
#  যদি LUA_VERSION_অঙ্ক >= 503
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
#  যদি LUA_VERSION_অঙ্ক >= 504
#   define lua_newuserdatauv dll_lua_newuserdatauv
#  অন্যথা
#   define lua_newuserdata dll_lua_newuserdata
#  endif
#  define lua_getmetatable dll_lua_getmetatable
#  define lua_setfield dll_lua_setfield
#  define lua_rawset dll_lua_rawset
#  define lua_rawseti dll_lua_rawseti
#  define lua_setmetatable dll_lua_setmetatable
#  define lua_পরবর্তী dll_lua_পরবর্তী
// libs
#  define luaopen_base dll_luaopen_base
#  define luaopen_table dll_luaopen_table
#  define luaopen_string dll_luaopen_string
#  define luaopen_math dll_luaopen_math
#  define luaopen_io dll_luaopen_io
#  define luaopen_os dll_luaopen_os
#  define luaopen_package dll_luaopen_package
#  define luaopen_debug dll_luaopen_debug
#  যদি LUA_VERSION_অঙ্ক >= 505
#   define luaL_openselectedlibs dll_luaL_openselectedlibs
#  অন্যথা
#   define luaL_openlibs dll_luaL_openlibs
#  endif

// lauxlib
#  যদি LUA_VERSION_অঙ্ক <= 501
শূন্য (*dll_luaL_register) (lua_State *L, ধ্রুবক অক্ষর *libname, ধ্রুবক luaL_Reg *l);
অক্ষর *(*dll_luaL_prepbuffer) (lua_L_বাফার *B);
শূন্য (*dll_luaL_openlib) (lua_State *L, ধ্রুবক অক্ষর *libname, ধ্রুবক luaL_Reg *l, পূর্ণসংখ্যা nup);
পূর্ণসংখ্যা (*dll_luaL_typerror) (lua_State *L, পূর্ণসংখ্যা narg, ধ্রুবক অক্ষর *tname);
পূর্ণসংখ্যা (*dll_luaL_loadfile) (lua_State *L, ধ্রুবক অক্ষর *filename);
পূর্ণসংখ্যা (*dll_luaL_loadbuffer) (lua_State *L, ধ্রুবক অক্ষর *buff, size_t sz, ধ্রুবক অক্ষর *name);
#  অন্যথা
অক্ষর *(*dll_luaL_prepbuffsize) (lua_L_বাফার *B, size_t sz);
শূন্য (*dll_luaL_setfuncs) (lua_State *L, ধ্রুবক luaL_Reg *l, পূর্ণসংখ্যা nup);
পূর্ণসংখ্যা (*dll_luaL_loadfilex) (lua_State *L, ধ্রুবক অক্ষর *filename, ধ্রুবক অক্ষর *mode);
পূর্ণসংখ্যা (*dll_luaL_loadbufferx) (lua_State *L, ধ্রুবক অক্ষর *buff, size_t sz, ধ্রুবক অক্ষর *name, ধ্রুবক অক্ষর *mode);
পূর্ণসংখ্যা (*dll_luaL_argerror) (lua_State *L, পূর্ণসংখ্যা numarg, ধ্রুবক অক্ষর *extramsg);
#  endif
#  যদি LUA_VERSION_অঙ্ক >= 504
পূর্ণসংখ্যা (*dll_luaL_typeerror) (lua_State *L, পূর্ণসংখ্যা narg, ধ্রুবক অক্ষর *tname);
#  endif
শূন্য (*dll_luaL_checkany) (lua_State *L, পূর্ণসংখ্যা narg);
ধ্রুবক অক্ষর *(*dll_luaL_checklstring) (lua_State *L, পূর্ণসংখ্যা অঙ্ক_Arg, size_t *l);
lua_Integer (*dll_luaL_checkinteger) (lua_State *L, পূর্ণসংখ্যা অঙ্ক_Arg);
lua_Integer (*dll_luaL_optinteger) (lua_State *L, পূর্ণসংখ্যা nArg, lua_Integer def);
শূন্য (*dll_luaL_checktype) (lua_State *L, পূর্ণসংখ্যা narg, পূর্ণসংখ্যা t);
পূর্ণসংখ্যা (*dll_luaL_error) (lua_State *L, ধ্রুবক অক্ষর *fmt, ...);
lua_State *(*dll_luaL_newstate) (শূন্য);
শূন্য (*dll_luaL_buffinit) (lua_State *L, lua_L_বাফার *B);
শূন্য (*dll_luaL_addlstring) (lua_L_বাফার *B, ধ্রুবক অক্ষর *s, size_t l);
শূন্য (*dll_luaL_pushresult) (lua_L_বাফার *B);
পূর্ণসংখ্যা (*dll_luaL_loadstring) (lua_State *L, ধ্রুবক অক্ষর *s);
পূর্ণসংখ্যা (*dll_luaL_ref) (lua_State *L, পূর্ণসংখ্যা idx);
#  যদি LUA_VERSION_অঙ্ক <= 502
শূন্য (*dll_luaL_unref) (lua_State *L, পূর্ণসংখ্যা idx, পূর্ণসংখ্যা n);
#  অন্যথা
শূন্য (*dll_luaL_unref) (lua_State *L, পূর্ণসংখ্যা idx, lua_Integer n);
#  endif
// lua
#  যদি LUA_VERSION_অঙ্ক <= 501
lua_সংখ্যা (*dll_lua_tonumber) (lua_State *L, পূর্ণসংখ্যা idx);
lua_Integer (*dll_lua_tointeger) (lua_State *L, পূর্ণসংখ্যা idx);
শূন্য (*dll_lua_call) (lua_State *L, পূর্ণসংখ্যা nargs, পূর্ণসংখ্যা nresults);
পূর্ণসংখ্যা (*dll_lua_pcall) (lua_State *L, পূর্ণসংখ্যা nargs, পূর্ণসংখ্যা nresults, পূর্ণসংখ্যা errfunc);
#  অন্যথা
lua_সংখ্যা (*dll_lua_tonumberx) (lua_State *L, পূর্ণসংখ্যা idx, পূর্ণসংখ্যা *isnum);
lua_Integer (*dll_lua_tointegerx) (lua_State *L, পূর্ণসংখ্যা idx, পূর্ণসংখ্যা *isnum);
শূন্য (*dll_lua_callk) (lua_State *L, পূর্ণসংখ্যা nargs, পূর্ণসংখ্যা nresults, পূর্ণসংখ্যা ctx,
	lua_CFunction k);
পূর্ণসংখ্যা (*dll_lua_pcallk) (lua_State *L, পূর্ণসংখ্যা nargs, পূর্ণসংখ্যা nresults, পূর্ণসংখ্যা errfunc,
	পূর্ণসংখ্যা ctx, lua_CFunction k);
শূন্য (*dll_lua_getglobal) (lua_State *L, ধ্রুবক অক্ষর *var);
শূন্য (*dll_lua_setglobal) (lua_State *L, ধ্রুবক অক্ষর *var);
#  endif
#  যদি LUA_VERSION_অঙ্ক <= 502
শূন্য (*dll_lua_replace) (lua_State *L, পূর্ণসংখ্যা idx);
শূন্য (*dll_lua_remove) (lua_State *L, পূর্ণসংখ্যা idx);
#  endif
#  যদি LUA_VERSION_অঙ্ক >= 503
শূন্য  (*dll_lua_rotate) (lua_State *L, পূর্ণসংখ্যা idx, পূর্ণসংখ্যা n);
শূন্য (*dll_lua_copy) (lua_State *L, পূর্ণসংখ্যা fromidx, পূর্ণসংখ্যা toidx);
#  endif
ধ্রুবক অক্ষর *(*dll_lua_typename) (lua_State *L, পূর্ণসংখ্যা tp);
শূন্য       (*dll_lua_close) (lua_State *L);
পূর্ণসংখ্যা (*dll_lua_gettop) (lua_State *L);
শূন্য (*dll_lua_settop) (lua_State *L, পূর্ণসংখ্যা idx);
শূন্য (*dll_lua_pushvalue) (lua_State *L, পূর্ণসংখ্যা idx);
পূর্ণসংখ্যা (*dll_lua_isnumber) (lua_State *L, পূর্ণসংখ্যা idx);
পূর্ণসংখ্যা (*dll_lua_isstring) (lua_State *L, পূর্ণসংখ্যা idx);
পূর্ণসংখ্যা (*dll_lua_type) (lua_State *L, পূর্ণসংখ্যা idx);
পূর্ণসংখ্যা (*dll_lua_rawequal) (lua_State *L, পূর্ণসংখ্যা idx1, পূর্ণসংখ্যা idx2);
পূর্ণসংখ্যা (*dll_lua_toboolean) (lua_State *L, পূর্ণসংখ্যা idx);
ধ্রুবক অক্ষর *(*dll_lua_tolstring) (lua_State *L, পূর্ণসংখ্যা idx, size_t *len);
শূন্য *(*dll_lua_touserdata) (lua_State *L, পূর্ণসংখ্যা idx);
শূন্য (*dll_lua_pushnil) (lua_State *L);
শূন্য (*dll_lua_pushnumber) (lua_State *L, lua_সংখ্যা n);
শূন্য (*dll_lua_pushinteger) (lua_State *L, lua_Integer n);
শূন্য (*dll_lua_pushlstring) (lua_State *L, ধ্রুবক অক্ষর *s, size_t l);
শূন্য (*dll_lua_pushstring) (lua_State *L, ধ্রুবক অক্ষর *s);
ধ্রুবক অক্ষর *(*dll_lua_pushfstring) (lua_State *L, ধ্রুবক অক্ষর *fmt, ...);
শূন্য (*dll_lua_pushcclosure) (lua_State *L, lua_CFunction fn, পূর্ণসংখ্যা n);
শূন্য (*dll_lua_pushboolean) (lua_State *L, পূর্ণসংখ্যা b);
শূন্য (*dll_lua_pushlightuserdata) (lua_State *L, শূন্য *p);
শূন্য (*dll_lua_getfield) (lua_State *L, পূর্ণসংখ্যা idx, ধ্রুবক অক্ষর *k);
#  যদি LUA_VERSION_অঙ্ক <= 502
শূন্য (*dll_lua_rawget) (lua_State *L, পূর্ণসংখ্যা idx);
শূন্য (*dll_lua_rawgeti) (lua_State *L, পূর্ণসংখ্যা idx, পূর্ণসংখ্যা n);
#  অন্যথা
পূর্ণসংখ্যা (*dll_lua_rawget) (lua_State *L, পূর্ণসংখ্যা idx);
পূর্ণসংখ্যা (*dll_lua_rawgeti) (lua_State *L, পূর্ণসংখ্যা idx, lua_Integer n);
#  endif
শূন্য (*dll_lua_createtable) (lua_State *L, পূর্ণসংখ্যা narr, পূর্ণসংখ্যা nrec);
শূন্য (*dll_lua_settable) (lua_State *L, পূর্ণসংখ্যা idx);
#  যদি LUA_VERSION_অঙ্ক >= 504
শূন্য *(*dll_lua_newuserdatauv) (lua_State *L, size_t sz, পূর্ণসংখ্যা nuvalue);
#  অন্যথা
শূন্য *(*dll_lua_newuserdata) (lua_State *L, size_t sz);
#  endif
পূর্ণসংখ্যা (*dll_lua_getmetatable) (lua_State *L, পূর্ণসংখ্যা objindex);
শূন্য (*dll_lua_setfield) (lua_State *L, পূর্ণসংখ্যা idx, ধ্রুবক অক্ষর *k);
শূন্য (*dll_lua_rawset) (lua_State *L, পূর্ণসংখ্যা idx);
#  যদি LUA_VERSION_অঙ্ক <= 502
শূন্য (*dll_lua_rawseti) (lua_State *L, পূর্ণসংখ্যা idx, পূর্ণসংখ্যা n);
#  অন্যথা
শূন্য (*dll_lua_rawseti) (lua_State *L, পূর্ণসংখ্যা idx, lua_Integer n);
#  endif
পূর্ণসংখ্যা (*dll_lua_setmetatable) (lua_State *L, পূর্ণসংখ্যা objindex);
পূর্ণসংখ্যা (*dll_lua_পরবর্তী) (lua_State *L, পূর্ণসংখ্যা idx);
// libs
পূর্ণসংখ্যা (*dll_luaopen_base) (lua_State *L);
পূর্ণসংখ্যা (*dll_luaopen_table) (lua_State *L);
পূর্ণসংখ্যা (*dll_luaopen_string) (lua_State *L);
পূর্ণসংখ্যা (*dll_luaopen_math) (lua_State *L);
পূর্ণসংখ্যা (*dll_luaopen_io) (lua_State *L);
পূর্ণসংখ্যা (*dll_luaopen_os) (lua_State *L);
পূর্ণসংখ্যা (*dll_luaopen_package) (lua_State *L);
পূর্ণসংখ্যা (*dll_luaopen_debug) (lua_State *L);
#  যদি LUA_VERSION_অঙ্ক >= 505
শূন্য (*dll_luaL_openselectedlibs) (lua_State *L, পূর্ণসংখ্যা load, পূর্ণসংখ্যা preload);
#  অন্যথা
শূন্য (*dll_luaL_openlibs) (lua_State *L);
#  endif

প্রকারলেখ শূন্য **luaV_function;
প্রকারলেখ কাঠামো {
    ধ্রুবক অক্ষর *name;
    luaV_function func;
} luaV_Reg;

স্থির ধ্রুবক luaV_Reg luaV_dll[] = {
    // lauxlib
#  যদি LUA_VERSION_অঙ্ক <= 501
    {"luaL_register", (luaV_function) &dll_luaL_register},
    {"luaL_prepbuffer", (luaV_function) &dll_luaL_prepbuffer},
    {"luaL_openlib", (luaV_function) &dll_luaL_openlib},
    {"luaL_typerror", (luaV_function) &dll_luaL_typerror},
    {"luaL_loadfile", (luaV_function) &dll_luaL_loadfile},
    {"luaL_loadbuffer", (luaV_function) &dll_luaL_loadbuffer},
#  অন্যথা
    {"luaL_prepbuffsize", (luaV_function) &dll_luaL_prepbuffsize},
    {"luaL_setfuncs", (luaV_function) &dll_luaL_setfuncs},
    {"luaL_loadfilex", (luaV_function) &dll_luaL_loadfilex},
    {"luaL_loadbufferx", (luaV_function) &dll_luaL_loadbufferx},
    {"luaL_argerror", (luaV_function) &dll_luaL_argerror},
#  endif
#  যদি LUA_VERSION_অঙ্ক >= 504
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
#  যদি LUA_VERSION_অঙ্ক <= 501
    {"lua_tonumber", (luaV_function) &dll_lua_tonumber},
    {"lua_tointeger", (luaV_function) &dll_lua_tointeger},
    {"lua_call", (luaV_function) &dll_lua_call},
    {"lua_pcall", (luaV_function) &dll_lua_pcall},
#  অন্যথা
    {"lua_tonumberx", (luaV_function) &dll_lua_tonumberx},
    {"lua_tointegerx", (luaV_function) &dll_lua_tointegerx},
    {"lua_callk", (luaV_function) &dll_lua_callk},
    {"lua_pcallk", (luaV_function) &dll_lua_pcallk},
    {"lua_getglobal", (luaV_function) &dll_lua_getglobal},
    {"lua_setglobal", (luaV_function) &dll_lua_setglobal},
#  endif
#  যদি LUA_VERSION_অঙ্ক <= 502
    {"lua_replace", (luaV_function) &dll_lua_replace},
    {"lua_remove", (luaV_function) &dll_lua_remove},
#  endif
#  যদি LUA_VERSION_অঙ্ক >= 503
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
#  যদি LUA_VERSION_অঙ্ক >= 504
    {"lua_newuserdatauv", (luaV_function) &dll_lua_newuserdatauv},
#  অন্যথা
    {"lua_newuserdata", (luaV_function) &dll_lua_newuserdata},
#  endif
    {"lua_getmetatable", (luaV_function) &dll_lua_getmetatable},
    {"lua_setfield", (luaV_function) &dll_lua_setfield},
    {"lua_rawset", (luaV_function) &dll_lua_rawset},
    {"lua_rawseti", (luaV_function) &dll_lua_rawseti},
    {"lua_setmetatable", (luaV_function) &dll_lua_setmetatable},
    {"lua_next", (luaV_function) &dll_lua_পরবর্তী},
    // libs
    {"luaopen_base", (luaV_function) &dll_luaopen_base},
    {"luaopen_table", (luaV_function) &dll_luaopen_table},
    {"luaopen_string", (luaV_function) &dll_luaopen_string},
    {"luaopen_math", (luaV_function) &dll_luaopen_math},
    {"luaopen_io", (luaV_function) &dll_luaopen_io},
    {"luaopen_os", (luaV_function) &dll_luaopen_os},
    {"luaopen_package", (luaV_function) &dll_luaopen_package},
    {"luaopen_debug", (luaV_function) &dll_luaopen_debug},
#  যদি LUA_VERSION_অঙ্ক >= 505
    {"luaL_openselectedlibs", (luaV_function) &dll_luaL_openselectedlibs},
#  অন্যথা
    {"luaL_openlibs", (luaV_function) &dll_luaL_openlibs},
#  endif
    {ফাঁকা, ফাঁকা}
};

স্থির HANDLE hinstLua = ফাঁকা;

    স্থির পূর্ণসংখ্যা
lua_link_init(অক্ষর *libname, পূর্ণসংখ্যা verbose)
{
    ধ্রুবক luaV_Reg *reg;
    যদি (hinstLua) ফেরত OK;
    hinstLua = load_dll(libname);
    যদি (!hinstLua)
    {
	যদি (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	ফেরত FAIL;
    }
    জন্য (reg = luaV_dll; reg->func; reg++)
    {
	যদি ((*reg->func = প্রতীক_from_dll(hinstLua, reg->name)) == ফাঁকা)
	{
	    close_dll(hinstLua);
	    hinstLua = 0;
	    যদি (verbose)
		semsg(_(e_could_not_load_library_function_str), reg->name);
	    ফেরত FAIL;
	}
    }
    ফেরত OK;
}
# endif // DYNAMIC_LUA

# যদি defined(DYNAMIC_LUA)
    পূর্ণসংখ্যা
lua_enabled(পূর্ণসংখ্যা verbose)
{
    ফেরত lua_link_init((অক্ষর *)p_luadll, verbose) == OK;
}
# endif

# যদি LUA_VERSION_অঙ্ক > 501 && LUA_VERSION_অঙ্ক < 504
    স্থির পূর্ণসংখ্যা
luaL_typeerror(lua_State *L, পূর্ণসংখ্যা narg, ধ্রুবক অক্ষর *tname)
{
    ধ্রুবক অক্ষর *বার্তা = lua_pushfstring(L, "%s expected, got %s",
	    tname, luaL_typename(L, narg));
    ফেরত luaL_argerror(L, narg, বার্তা);
}
# endif

    স্থির LUAV_INLINE শূন্য
luaV_getudata(lua_State *L, শূন্য *v)
{
    lua_pushlightuserdata(L, (শূন্য *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // now the cache table is at the top of the stack
    lua_pushlightuserdata(L, v);
    lua_rawget(L, -2);
    lua_remove(L, -2);  // remove the cache table from the stack
}

    স্থির LUAV_INLINE শূন্য
luaV_setudata(lua_State *L, শূন্য *v)
{
    lua_pushlightuserdata(L, (শূন্য *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // cache table is at -1
    lua_pushlightuserdata(L, v);       // ...now at -2
    lua_pushvalue(L, -3);	       // copy the userdata (cache at -3)
    lua_rawset(L, -3);		       // consumes two stack items
    lua_বের_করো(L, 1);		       // and remove the cache table
}

// =======   Internal   =======

    স্থির শূন্য
luaV_newmetatable(lua_State *L, ধ্রুবক অক্ষর *tname)
{
    lua_newtable(L);
    lua_pushlightuserdata(L, (শূন্য *) tname);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
}

    স্থির শূন্য *
luaV_toudata(lua_State *L, পূর্ণসংখ্যা ud, ধ্রুবক অক্ষর *tname)
{
    শূন্য *p = lua_touserdata(L, ud);

    যদি (p == ফাঁকা)
	ফেরত ফাঁকা;

    // value is userdata
    যদি (lua_getmetatable(L, ud)) // does it have a metatable?
    {
	luaV_getfield(L, tname); // get metatable
	যদি (lua_rawequal(L, -1, -2)) // MTs match?
	{
	    lua_বের_করো(L, 2); // MTs
	    ফেরত p;
	}
    }

    ফেরত ফাঁকা;
}

    স্থির শূন্য *
luaV_checkcache(lua_State *L, শূন্য *p)
{
    luaV_getudata(L, p);
    যদি (lua_isnil(L, -1)) luaL_error(L, "invalid object");
    lua_বের_করো(L, 1);
    ফেরত p;
}

# define luaV_unbox(L,luatyp,ud) (*((luatyp *) lua_touserdata((L),(ud))))

# define luaV_checkvalid(L,luatyp,ud) \
    luaV_checkcache((L), (শূন্য *) luaV_unbox((L),luatyp,(ud)))

    স্থির শূন্য *
luaV_checkudata(lua_State *L, পূর্ণসংখ্যা ud, ধ্রুবক অক্ষর *tname)
{
    শূন্য *p = luaV_toudata(L, ud, tname);
    যদি (p == ফাঁকা) luaL_typeerror(L, ud, tname);
    ফেরত p;
}

    স্থির শূন্য
luaV_pushtypval(lua_State *L, typval_T *tv)
{
    যদি (tv == ফাঁকা)
    {
	lua_pushnil(L);
	ফেরত;
    }
    নির্বাচন (tv->v_type)
    {
	ক্ষেত্র VAR_STRING:
	    lua_pushstring(L, tv->vval.v_string == ফাঁকা
					    ? "" : (অক্ষর *)tv->vval.v_string);
	    বিরতি;
	ক্ষেত্র VAR_সংখ্যা:
	    lua_pushinteger(L, (পূর্ণসংখ্যা) tv->vval.v_সংখ্যা);
	    বিরতি;
	ক্ষেত্র VAR_FLOAT:
	    lua_pushnumber(L, (lua_সংখ্যা) tv->vval.v_float);
	    বিরতি;
	ক্ষেত্র VAR_LIST:
	    luaV_pushlist(L, tv->vval.v_list);
	    বিরতি;
	ক্ষেত্র VAR_DICT:
	    luaV_pushdict(L, tv->vval.v_dict);
	    বিরতি;
	ক্ষেত্র VAR_BOOL:
	ক্ষেত্র VAR_SPECIAL:
	    যদি (tv->vval.v_সংখ্যা <= VVAL_TRUE)
		lua_pushboolean(L, (পূর্ণসংখ্যা) tv->vval.v_সংখ্যা);
	    অন্যথা
		lua_pushnil(L);
	    বিরতি;
	ক্ষেত্র VAR_FUNC:
	    luaV_pushfuncref(L, tv->vval.v_string);
	    বিরতি;
	ক্ষেত্র VAR_PARTIAL:
	    // TODO: handle partial arguments
	    luaV_pushfuncref(L, partial_name(tv->vval.v_partial));
	    বিরতি;

	ক্ষেত্র VAR_BLOB:
	    luaV_pushblob(L, tv->vval.v_blob);
	    বিরতি;
	পূর্বনির্ধারিত:
	    lua_pushnil(L);
    }
}

/*
 * Converts lua value at 'pos' to typval 'tv'.
 * Returns OK or FAIL.
 */
    স্থির পূর্ণসংখ্যা
luaV_totypval(lua_State *L, পূর্ণসংখ্যা pos, typval_T *tv)
{
    পূর্ণসংখ্যা status = OK;

    tv->v_lock = 0;

    নির্বাচন (lua_type(L, pos))
    {
	ক্ষেত্র LUA_TBOOLEAN:
	    tv->v_type = VAR_BOOL;
	    tv->vval.v_সংখ্যা = (varnumber_T) lua_toboolean(L, pos);
	    বিরতি;
	ক্ষেত্র LUA_TNIL:
	    tv->v_type = VAR_SPECIAL;
	    tv->vval.v_সংখ্যা = VVAL_NULL;
	    বিরতি;
	ক্ষেত্র LUA_TSTRING:
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strsave((char_u *) lua_tostring(L, pos));
	    বিরতি;
	ক্ষেত্র LUA_TNUMBER:
	{
	    ধ্রুবক lua_সংখ্যা n = lua_tonumber(L, pos);

	    যদি (n > (lua_সংখ্যা)INT64_MAX || n < (lua_সংখ্যা)INT64_MIN
		    || ((lua_সংখ্যা)((varnumber_T)n)) != n)
	    {
		tv->v_type = VAR_FLOAT;
		tv->vval.v_float = (float_T)n;
	    }
	    অন্যথা
	    {
		tv->v_type = VAR_সংখ্যা;
		tv->vval.v_সংখ্যা = (varnumber_T)n;
	    }
	}
	    বিরতি;
	ক্ষেত্র LUA_TFUNCTION:
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
	    বিরতি;
	}
	ক্ষেত্র LUA_TTABLE:
	{
	    পূর্ণসংখ্যা lua_tableref;

	    lua_pushvalue(L, pos);
	    lua_tableref = luaL_ref(L, LUA_REGISTRYINDEX);
	    যদি (lua_getmetatable(L, pos))
	    {
		lua_getfield(L, -1, LUA___CALL);
		যদি (lua_isfunction(L, -1))
		{
		    char_u *name;
		    পূর্ণসংখ্যা lua_funcref = luaL_ref(L, LUA_REGISTRYINDEX);
		    luaV_CFuncState *state = ALLOC_CLEAR_ONE(luaV_CFuncState);

		    state->lua_funcref = lua_funcref;
		    state->L = L;
		    state->lua_tableref = lua_tableref;
		    name = register_cfunc(&luaV_call_lua_func,
					      &luaV_call_lua_func_free, state);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(name);
		    বিরতি;
		}
	    }
	    tv->v_type = VAR_সংখ্যা;
	    tv->vval.v_সংখ্যা = 0;
	    status = FAIL;
	    বিরতি;
	}
	ক্ষেত্র LUA_TUSERDATA:
	{
	    শূন্য *p = lua_touserdata(L, pos);

	    যদি (lua_getmetatable(L, pos)) // has metatable?
	    {
		// check list
		luaV_getfield(L, LUAVIM_LIST);
		যদি (lua_rawequal(L, -1, -2))
		{
		    tv->v_type = VAR_LIST;
		    tv->vval.v_list = *((luaV_List *) p);
		    ++tv->vval.v_list->lv_refcount;
		    lua_বের_করো(L, 2); // MTs
		    বিরতি;
		}
		// check dict
		luaV_getfield(L, LUAVIM_DICT);
		যদি (lua_rawequal(L, -1, -3))
		{
		    tv->v_type = VAR_DICT;
		    tv->vval.v_dict = *((luaV_Dict *) p);
		    ++tv->vval.v_dict->dv_refcount;
		    lua_বের_করো(L, 3); // MTs
		    বিরতি;
		}
		// check blob
		luaV_getfield(L, LUAVIM_BLOB);
		যদি (lua_rawequal(L, -1, -4))
		{
		    tv->v_type = VAR_BLOB;
		    tv->vval.v_blob = *((luaV_Blob *) p);
		    ++tv->vval.v_blob->bv_refcount;
		    lua_বের_করো(L, 4); // MTs
		    বিরতি;
		}
		// check funcref
		luaV_getfield(L, LUAVIM_FUNCREF);
		যদি (lua_rawequal(L, -1, -5))
		{
		    luaV_Funcref *f = (luaV_Funcref *) p;

		    func_ref(f->name);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(f->name);
		    lua_বের_করো(L, 5); // MTs
		    বিরতি;
		}
		lua_বের_করো(L, 4); // MTs
	    }
	}
	// FALLTHROUGH
	পূর্বনির্ধারিত:
	    tv->v_type = VAR_সংখ্যা;
	    tv->vval.v_সংখ্যা = 0;
	    status = FAIL;
    }
    ফেরত status;
}

/*
 * similar to luaL_addlstring, but replaces \0 with \n if toline and
 * \n with \0 otherwise
 */
    স্থির শূন্য
luaV_addlstring(lua_L_বাফার *b, ধ্রুবক অক্ষর *s, size_t l, পূর্ণসংখ্যা toline)
{
    যখন (l--)
    {
	যদি (*s == '\0' && toline)
	    luaL_addchar(b, '\n');
	অন্যথা যদি (*s == '\n' && !toline)
	    luaL_addchar(b, '\0');
	অন্যথা
	    luaL_addchar(b, *s);
	s++;
    }
}

    স্থির শূন্য
luaV_pushline(lua_State *L, buf_T *buf, linenr_T n)
{
    ধ্রুবক অক্ষর *s = (ধ্রুবক অক্ষর *) ml_get_buf(buf, n, FALSE);
    lua_L_বাফার b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, strlen(s), 0);
    luaL_pushresult(&b);
}

    স্থির char_u *
luaV_toline(lua_State *L, পূর্ণসংখ্যা pos)
{
    size_t l;
    ধ্রুবক অক্ষর *s = lua_tolstring(L, pos, &l);

    lua_L_বাফার b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 1);
    luaL_pushresult(&b);
    ফেরত (char_u *) lua_tostring(L, -1);
}

/*
 * pops a string s from the top of the stack and calls mf(t) for pieces t of
 * s separated by newlines
 */
    স্থির শূন্য
luaV_msgfunc(lua_State *L, msgfunc_T mf)
{
    lua_L_বাফার b;
    size_t l;
    ধ্রুবক অক্ষর *p, *s = lua_tolstring(L, -1, &l);
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 0);
    luaL_pushresult(&b);
    // break string
    p = s = lua_tolstring(L, -1, &l);
    যখন (l--)
    {
	যদি (*p++ == '\0') // break?
	{
	    mf((অক্ষর *)s);
	    s = p;
	}
    }
    mf((অক্ষর *)s);
    lua_বের_করো(L, 2); // original and modified strings
}

# define luaV_newtype(typ,tname,luatyp,luatname) \
	স্থির luatyp * \
    luaV_new##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = (luatyp *) lua_newuserdata(L, আকার(luatyp)); \
	*o = obj; \
	luaV_setudata(L, obj); /* cache[obj] = udata */ \
	luaV_getfield(L, luatname); \
	lua_setmetatable(L, -2); \
	ফেরত o; \
    }

# define luaV_pushtype(typ,tname,luatyp) \
	স্থির luatyp * \
    lua_V_ঠেলো##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = ফাঁকা; \
	যদি (obj == ফাঁকা) \
	    lua_pushnil(L); \
	অন্যথা \
	{ \
	    luaV_getudata(L, obj); \
	    যদি (lua_isnil(L, -1)) /* not interned? */ \
	    { \
		lua_বের_করো(L, 1); \
		o = luaV_new##tname(L, obj); \
	    } \
	    অন্যথা \
		o = (luatyp *) lua_touserdata(L, -1); \
	} \
	ফেরত o; \
    }

# define luaV_type_tostring(tname,luatname) \
	স্থির পূর্ণসংখ্যা \
    luaV_##tname##_tostring(lua_State *L) \
    { \
	lua_pushfstring(L, "%s: %p", luatname, lua_touserdata(L, 1)); \
	ফেরত 1; \
    }

// =======   List type   =======

    স্থির luaV_List *
luaV_newlist(lua_State *L, list_T *lis)
{
    luaV_List *l = (luaV_List *) lua_newuserdata(L, আকার(luaV_List));
    *l = lis;
    lis->lv_refcount++; // reference in Lua
    luaV_setudata(L, lis); // cache[lis] = udata
    luaV_getfield(L, LUAVIM_LIST);
    lua_setmetatable(L, -2);
    ফেরত l;
}

luaV_pushtype(list_T, list, luaV_List)
luaV_type_tostring(list, LUAVIM_LIST)

    স্থির পূর্ণসংখ্যা
luaV_list_len(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushinteger(L, (পূর্ণসংখ্যা) list_len(l));
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
luaV_list_iter(lua_State *L)
{
    listitem_T *li = (listitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    যদি (li == ফাঁকা) ফেরত 0;
    luaV_pushtypval(L, &li->li_tv);
    lua_pushlightuserdata(L, (শূন্য *) li->li_পরবর্তী);
    lua_replace(L, lua_upvalueindex(1));
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
luaV_list_call(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushlightuserdata(L, (শূন্য *) l->lv_first);
    lua_pushcclosure(L, luaV_list_iter, 1);
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
lua_V_list_সূচক(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    যদি (lua_isnumber(L, 2)) // list item?
    {
	long n = (long) luaL_checkinteger(L, 2);
	listitem_T *li;

	// Lua array index starts with 1 while Vim uses 0, subtract 1 to
	// normalize.
	n -= 1;
	li = list_find(l, n);
	যদি (li == ফাঁকা)
	    lua_pushnil(L);
	অন্যথা
	    luaV_pushtypval(L, &li->li_tv);
    }
    অন্যথা যদি (lua_isstring(L, 2)) // method?
    {
	ধ্রুবক অক্ষর *s = lua_tostring(L, 2);
	যদি (strncmp(s, "add", 3) == 0
		|| strncmp(s, "insert", 6) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	অন্যথা
	    lua_pushnil(L);
    }
    অন্যথা
	lua_pushnil(L);
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
luaV_list_newindex(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    long n = (long) luaL_checkinteger(L, 2);
    listitem_T *li;

    // Lua array index starts with 1 while Vim uses 0, subtract 1 to normalize.
    n -= 1;

    যদি (l->lv_lock)
	luaL_error(L, "list is locked");
    li = list_find(l, n);
    যদি (li == ফাঁকা)
    {
	যদি (!lua_isnil(L, 3))
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "inserting list item");
	    যদি (list_ঢোকাও_tv(l, &v, li) == FAIL)
		luaL_error(L, "failed to add item to list");
	    clear_tv(&v);
	}
    }
    অন্যথা
    {
	যদি (lua_isnil(L, 3)) // remove?
	{
	    vimlist_remove(l, li, li);
	    listitem_free(l, li);
	}
	অন্যথা
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "setting list item");
	    clear_tv(&li->li_tv);
	    li->li_tv = v;
	}
    }
    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
luaV_list_add(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (শূন্য *) *lis);
    typval_T v;
    যদি (l->lv_lock)
	luaL_error(L, "list is locked");
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "adding list item");
    যদি (list_append_tv(l, &v) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
lua_V_list_ঢোকাও(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (শূন্য *) *lis);
    long pos = (long) luaL_optinteger(L, 3, 0);
    listitem_T *li = ফাঁকা;
    typval_T v;
    যদি (l->lv_lock)
	luaL_error(L, "list is locked");
    যদি (pos < l->lv_len)
    {
	li = list_find(l, pos);
	যদি (li == ফাঁকা)
	    luaL_error(L, "invalid position");
    }
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "inserting list item");
    যদি (list_ঢোকাও_tv(l, &v, li) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    ফেরত 1;
}

স্থির ধ্রুবক luaL_Reg luaV_List_mt[] = {
    {"__tostring", luaV_list_tostring},
    {"__len", luaV_list_len},
    {"__call", luaV_list_call},
    {"__index", lua_V_list_সূচক},
    {"__newindex", luaV_list_newindex},
    {"add", luaV_list_add},
    {"insert", lua_V_list_ঢোকাও},
    {ফাঁকা, ফাঁকা}
};


// =======   Dict type   =======

    স্থির luaV_Dict *
luaV_newdict(lua_State *L, dict_T *dic)
{
    luaV_Dict *d = (luaV_Dict *) lua_newuserdata(L, আকার(luaV_Dict));
    *d = dic;
    dic->dv_refcount++; // reference in Lua
    luaV_setudata(L, dic); // cache[dic] = udata
    luaV_getfield(L, LUAVIM_DICT);
    lua_setmetatable(L, -2);
    ফেরত d;
}

luaV_pushtype(dict_T, dict, luaV_Dict)
luaV_type_tostring(dict, LUAVIM_DICT)

    স্থির পূর্ণসংখ্যা
luaV_dict_len(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    lua_pushinteger(L, (পূর্ণসংখ্যা) dict_len(d));
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
luaV_dict_iter(lua_State *L UNUSED)
{
# ifdef FEAT_EVAL
    hashitem_T *hi = (hashitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    পূর্ণসংখ্যা n = lua_tointeger(L, lua_upvalueindex(2));
    dictitem_T *di;
    যদি (n <= 0) ফেরত 0;
    যখন (HASHITEM_EMPTY(hi)) hi++;
    di = dict_lookup(hi);
    lua_pushstring(L, (অক্ষর *) hi->hi_চাবি);
    luaV_pushtypval(L, &di->di_tv);
    lua_pushlightuserdata(L, (শূন্য *) (hi + 1));
    lua_replace(L, lua_upvalueindex(1));
    lua_pushinteger(L, n - 1);
    lua_replace(L, lua_upvalueindex(2));
    ফেরত 2;
# অন্যথা
    ফেরত 0;
# endif
}

    স্থির পূর্ণসংখ্যা
luaV_dict_call(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    hashtab_T *ht = &d->dv_hashtab;
    lua_pushlightuserdata(L, (শূন্য *) ht->ht_array);
    lua_pushinteger(L, ht->ht_used); // # remaining items
    lua_pushcclosure(L, luaV_dict_iter, 2);
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
lua_V_dict_সূচক(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *চাবি = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di = dict_find(d, চাবি, -1);

    যদি (di == ফাঁকা)
    {
	lua_pushnil(L);
	ফেরত 1;
    }

    luaV_pushtypval(L, &di->di_tv);
    যদি (di->di_tv.v_type == VAR_FUNC) // funcref?
    {
	luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, -1);
	f->self = d; // keep "self" reference
	d->dv_refcount++;
    }

    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
luaV_dict_newindex(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *চাবি = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di;
    typval_T tv;

    যদি (d->dv_lock)
	luaL_error(L, "dict is locked");
    যদি (চাবি == ফাঁকা)
	ফেরত 0;
    যদি (*চাবি == NUL)
	luaL_error(L, "empty key");
    যদি (!lua_isnil(L, 3)) // read value?
    {
	luaV_checktypval(L, 3, &tv, "setting dict item");
	যদি (d->dv_scope == VAR_DEF_SCOPE && tv.v_type == VAR_FUNC)
	{
	    clear_tv(&tv);
	    luaL_error(L, "cannot assign funcref to builtin scope");
	}
    }
    di = dict_find(d, চাবি, -1);
    যদি (di == ফাঁকা) // non-existing key?
    {
	যদি (lua_isnil(L, 3))
	    ফেরত 0;
	di = dictitem_alloc(চাবি);
	যদি (di == ফাঁকা)
	{
	    clear_tv(&tv);
	    ফেরত 0;
	}
	যদি (dict_add(d, di) == FAIL)
	{
	    vim_free(di);
	    clear_tv(&tv);
	    ফেরত 0;
	}
    }
    অন্যথা
	clear_tv(&di->di_tv);
    যদি (lua_isnil(L, 3)) // remove?
    {
	hashitem_T *hi = hash_find(&d->dv_hashtab, di->di_চাবি);
	hash_remove(&d->dv_hashtab, hi, "Lua new index");
	dictitem_free(di);
    }
    অন্যথা
	di->di_tv = tv;
    ফেরত 0;
}

স্থির ধ্রুবক luaL_Reg luaV_Dict_mt[] = {
    {"__tostring", luaV_dict_tostring},
    {"__len", luaV_dict_len},
    {"__call", luaV_dict_call},
    {"__index", lua_V_dict_সূচক},
    {"__newindex", luaV_dict_newindex},
    {ফাঁকা, ফাঁকা}
};


// =======   Blob type   =======

    স্থির luaV_Blob *
luaV_newblob(lua_State *L, blob_T *blo)
{
    luaV_Blob *b = (luaV_Blob *) lua_newuserdata(L, আকার(luaV_Blob));
    *b = blo;
    blo->bv_refcount++; // reference in Lua
    luaV_setudata(L, blo); // cache[blo] = udata
    luaV_getfield(L, LUAVIM_BLOB);
    lua_setmetatable(L, -2);
    ফেরত b;
}

luaV_pushtype(blob_T, blob, luaV_Blob)
luaV_type_tostring(blob, LUAVIM_BLOB)

    স্থির পূর্ণসংখ্যা
luaV_blob_gc(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    blob_unref(b);
    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
luaV_blob_len(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    lua_pushinteger(L, (পূর্ণসংখ্যা) blob_len(b));
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
lua_V_blob_সূচক(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    যদি (lua_isnumber(L, 2))
    {
	পূর্ণসংখ্যা idx = luaL_checkinteger(L, 2);
	যদি (idx < 0)
	    // Negative index is relative to the end.
	    idx += (পূর্ণসংখ্যা) blob_len(b);
	যদি (idx >= 0 && idx < blob_len(b))
	    lua_pushnumber(L, (lua_সংখ্যা) blob_get(b, idx));
	অন্যথা
	    lua_pushnil(L);
    }
    অন্যথা যদি (lua_isstring(L, 2))
    {
	ধ্রুবক অক্ষর *s = lua_tostring(L, 2);
	যদি (strncmp(s, "add", 3) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	অন্যথা
	    lua_pushnil(L);
    }
    অন্যথা
	lua_pushnil(L);
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
luaV_blob_newindex(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    যদি (b->bv_lock)
	luaL_error(L, "blob is locked");

    যদি (!lua_isnumber(L, 2))
	ফেরত 0;

    long len = blob_len(b);
    পূর্ণসংখ্যা idx = luaL_checkinteger(L, 2);
    পূর্ণসংখ্যা val = luaL_checkinteger(L, 3);
    যদি (idx < 0)
	// Negative index is relative to the end.
	idx += (পূর্ণসংখ্যা) len;
    যদি ((idx >= 0 && idx < len)
	    || (idx == len && ga_grow(&b->bv_ga, 1) == OK))
    {
	blob_set(b, idx, (char_u) val);
	যদি (idx == len)
	    ++b->bv_ga.ga_len;
    }
    অন্যথা
	luaL_error(L, "index out of range");

    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
luaV_blob_add(lua_State *L)
{
    luaV_Blob *blo = luaV_checkudata(L, 1, LUAVIM_BLOB);
    blob_T *b = (blob_T *) luaV_checkcache(L, (শূন্য *) *blo);
    যদি (b->bv_lock)
	luaL_error(L, "blob is locked");
    lua_settop(L, 2);
    যদি (!lua_isstring(L, 2))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 2));
    অন্যথা
    {
	size_t i, l = 0;
	ধ্রুবক অক্ষর *s = lua_tolstring(L, 2, &l);

	যদি (ga_grow(&b->bv_ga, (পূর্ণসংখ্যা)l) == OK)
	    জন্য (i = 0; i < l; ++i)
		ga_append(&b->bv_ga, s[i]);
    }
    lua_settop(L, 1);
    ফেরত 1;
}

স্থির ধ্রুবক luaL_Reg luaV_Blob_mt[] = {
    {"__tostring", luaV_blob_tostring},
    {"__gc", luaV_blob_gc},
    {"__len", luaV_blob_len},
    {"__index", lua_V_blob_সূচক},
    {"__newindex", luaV_blob_newindex},
    {"add", luaV_blob_add},
    {ফাঁকা, ফাঁকা}
};


// =======   Funcref type   =======

    স্থির luaV_Funcref *
luaV_newfuncref(lua_State *L, char_u *name)
{
    luaV_Funcref *f = (luaV_Funcref *)lua_newuserdata(L, আকার(luaV_Funcref));

    যদি (name != ফাঁকা)
    {
	func_ref(name);
	f->name = vim_strsave(name);
    }
    f->self = ফাঁকা;
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_setmetatable(L, -2);
    ফেরত f;
}

    স্থির luaV_Funcref *
luaV_pushfuncref(lua_State *L, char_u *name)
{
    ফেরত luaV_newfuncref(L, name);
}


luaV_type_tostring(funcref, LUAVIM_FUNCREF)

    স্থির পূর্ণসংখ্যা
luaV_funcref_gc(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    func_unref(f->name);
    vim_free(f->name);
    // NOTE: Don't call "dict_unref(f->self)", because the dict of "f->self"
    // will be (or has been already) freed by Vim's garbage collection.
    ফেরত 0;
}

// equivalent to string(funcref)
    স্থির পূর্ণসংখ্যা
luaV_funcref_len(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    lua_pushstring(L, (ধ্রুবক অক্ষর *) f->name);
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
luaV_funcref_call(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);
    পূর্ণসংখ্যা i, n = lua_gettop(L) - 1; // #args
    পূর্ণসংখ্যা status = FAIL;
    typval_T args;
    typval_T rettv;

    args.v_type = VAR_LIST;
    args.vval.v_list = list_alloc();
    rettv.v_type = VAR_UNKNOWN; // as in clear_tv
    যদি (args.vval.v_list != ফাঁকা)
    {
	typval_T v;

	জন্য (i = 0; i < n; i++)
	{
	    luaV_checktypval(L, i + 2, &v, "calling funcref");
	    list_append_tv(args.vval.v_list, &v);
	    clear_tv(&v);
	}
	status = func_call(f->name, &args, ফাঁকা, f->self, &rettv);
	যদি (status == OK)
	    luaV_pushtypval(L, &rettv);
	clear_tv(&args);
	clear_tv(&rettv);
    }
    যদি (status != OK)
	luaL_error(L, "cannot call funcref");
    ফেরত 1;
}

স্থির ধ্রুবক luaL_Reg luaV_Funcref_mt[] = {
    {"__tostring", luaV_funcref_tostring},
    {"__gc", luaV_funcref_gc},
    {"__len", luaV_funcref_len},
    {"__call", luaV_funcref_call},
    {ফাঁকা, ফাঁকা}
};


// =======   Buffer type   =======

luaV_newtype(buf_T, বাফার, lua_V_বাফার, LUAVIM_বাফার)
luaV_pushtype(buf_T, বাফার, lua_V_বাফার)
luaV_type_tostring(বাফার, LUAVIM_বাফার)

    স্থির পূর্ণসংখ্যা
lua_V_বাফার_len(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, lua_V_বাফার, 1);
    lua_pushinteger(L, b->b_ml.ml_সারি_গুনতি);
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
lua_V_বাফার_call(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, lua_V_বাফার, 1);
    lua_settop(L, 1);
    set_curbuf(b, DOBUF_SPLIT);
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
lua_V_বাফার_সূচক(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, lua_V_বাফার, 1);
    linenr_T n = (linenr_T) lua_tointeger(L, 2);
    যদি (n > 0 && n <= b->b_ml.ml_সারি_গুনতি)
	luaV_pushline(L, b, n);
    অন্যথা যদি (lua_isstring(L, 2))
    {
	ধ্রুবক অক্ষর *s = lua_tostring(L, 2);
	যদি (strncmp(s, "name", 4) == 0)
	    lua_pushstring(L, (b->b_sfname == ফাঁকা)
					? "" : (অক্ষর *) b->b_sfname);
	অন্যথা যদি (strncmp(s, "fname", 5) == 0)
	    lua_pushstring(L, (b->b_ffname == ফাঁকা)
					? "" : (অক্ষর *) b->b_ffname);
	অন্যথা যদি (strncmp(s, "number", 6) == 0)
	    lua_pushinteger(L, b->b_fnum);
	// methods
	অন্যথা যদি (strncmp(s,   "insert", 6) == 0
		|| strncmp(s, "next", 4) == 0
		|| strncmp(s, "previous", 8) == 0
		|| strncmp(s, "isvalid", 7) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	অন্যথা
	    lua_pushnil(L);
    }
    অন্যথা
	lua_pushnil(L);
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
lua_V_বাফার_newindex(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, lua_V_বাফার, 1);
    linenr_T n = (linenr_T) luaL_checkinteger(L, 2);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    যদি (n < 1 || n > b->b_ml.ml_সারি_গুনতি)
	luaL_error(L, "invalid line number");
    যদি (lua_isnil(L, 3)) // delete line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	যদি (u_savedel(n, 1L) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	অন্যথা যদি (ml_delete(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot delete line");
	}
	অন্যথা
	{
	    deleted_সারিগুলি_mark(n, 1L);
	    যদি (b == curwin->w_বাফার) // fix cursor in current window?
	    {
		যদি (curwin->w_cursor.lnum >= n)
		{
		    যদি (curwin->w_cursor.lnum > n)
		    {
			curwin->w_cursor.lnum -= 1;
			check_cursor_col();
		    }
		    অন্যথা
			check_cursor();
		    changed_cline_bef_curs();
		}
		invalidate_botline();
	    }
	}
	curbuf = buf;
    }
    অন্যথা যদি (lua_isstring(L, 3)) // update line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	যদি (u_savesub(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	অন্যথা যদি (ml_replace(n, luaV_toline(L, 3), TRUE) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot replace line");
	}
	অন্যথা
	    changed_bytes(n, 0);
	curbuf = buf;
	যদি (b == curwin->w_বাফার)
	    check_cursor_col();
    }
    অন্যথা
	luaL_error(L, "wrong argument to change line");
    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
lua_V_বাফার_ঢোকাও(lua_State *L)
{
    lua_V_বাফার *lb = luaV_checkudata(L, 1, LUAVIM_বাফার);
    buf_T *b = (buf_T *) luaV_checkcache(L, (শূন্য *) *lb);
    linenr_T last = b->b_ml.ml_সারি_গুনতি;
    linenr_T n = (linenr_T) luaL_optinteger(L, 3, last);
    buf_T *buf;
    luaL_checktype(L, 2, LUA_TSTRING);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    // fix insertion line
    যদি (n < 0) n = 0;
    যদি (n > last) n = last;
    // insert
    buf = curbuf;
    curbuf = b;
    যদি (u_save(n, n + 1) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot save undo information");
    }
    অন্যথা যদি (ml_append(n, luaV_toline(L, 2), 0, FALSE) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot insert line");
    }
    অন্যথা
	appended_সারিগুলি_mark(n, 1L);
    curbuf = buf;
    update_screen(UPD_VALID);
    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
lua_V_বাফার_পরবর্তী(lua_State *L)
{
    lua_V_বাফার *b = luaV_checkudata(L, 1, LUAVIM_বাফার);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (শূন্য *) *b);
    luaV_pushbuffer(L, buf->b_পরবর্তী);
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
lua_V_বাফার_previous(lua_State *L)
{
    lua_V_বাফার *b = luaV_checkudata(L, 1, LUAVIM_বাফার);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (শূন্য *) *b);
    luaV_pushbuffer(L, buf->b_পূর্ব);
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
lua_V_বাফার_isvalid(lua_State *L)
{
    lua_V_বাফার *b = luaV_checkudata(L, 1, LUAVIM_বাফার);
    luaV_getudata(L, *b);
    lua_pushboolean(L, !lua_isnil(L, -1));
    ফেরত 1;
}

স্থির ধ্রুবক luaL_Reg lua_V_বাফার_mt[] = {
    {"__tostring", lua_V_বাফার_tostring},
    {"__len", lua_V_বাফার_len},
    {"__call", lua_V_বাফার_call},
    {"__index", lua_V_বাফার_সূচক},
    {"__newindex", lua_V_বাফার_newindex},
    {"insert", lua_V_বাফার_ঢোকাও},
    {"next", lua_V_বাফার_পরবর্তী},
    {"previous", lua_V_বাফার_previous},
    {"isvalid", lua_V_বাফার_isvalid},
    {ফাঁকা, ফাঁকা}
};


// =======   Window type   =======

luaV_newtype(win_T, window, luaV_Window, LUAVIM_WINDOW)
luaV_pushtype(win_T, window, luaV_Window)
luaV_type_tostring(window, LUAVIM_WINDOW)

    স্থির পূর্ণসংখ্যা
luaV_window_call(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    lua_settop(L, 1);
    win_goto(w);
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
lua_V_window_সূচক(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    ধ্রুবক অক্ষর *s = luaL_checkstring(L, 2);
    যদি (strncmp(s, "buffer", 6) == 0)
	luaV_pushbuffer(L, w->w_বাফার);
    অন্যথা যদি (strncmp(s, "line", 4) == 0)
	lua_pushinteger(L, w->w_cursor.lnum);
    অন্যথা যদি (strncmp(s, "col", 3) == 0)
	lua_pushinteger(L, w->w_cursor.col + 1);
    অন্যথা যদি (strncmp(s, "width", 5) == 0)
	lua_pushinteger(L, w->w_width);
    অন্যথা যদি (strncmp(s, "height", 6) == 0)
	lua_pushinteger(L, w->w_height);
    // methods
    অন্যথা যদি (strncmp(s,   "next", 4) == 0
	    || strncmp(s, "previous", 8) == 0
	    || strncmp(s, "isvalid", 7) == 0)
    {
	lua_getmetatable(L, 1);
	lua_getfield(L, -1, s);
    }
    অন্যথা
	lua_pushnil(L);
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
luaV_window_newindex(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    ধ্রুবক অক্ষর *s = luaL_checkstring(L, 2);
    পূর্ণসংখ্যা v = luaL_checkinteger(L, 3);
    যদি (strncmp(s, "line", 4) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	যদি (v < 1 || v > w->w_বাফার->b_ml.ml_সারি_গুনতি)
	    luaL_error(L, "line out of range");
	w->w_cursor.lnum = v;
	update_screen(UPD_VALID);
    }
    অন্যথা যদি (strncmp(s, "col", 3) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	w->w_cursor.col = v - 1;
	w->w_set_curswant = true;
	update_screen(UPD_VALID);
    }
    অন্যথা যদি (strncmp(s, "width", 5) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setwidth(v);
	curwin = win;
    }
    অন্যথা যদি (strncmp(s, "height", 6) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setheight(v);
	curwin = win;
    }
    অন্যথা
	luaL_error(L, "invalid window property: `%s'", s);
    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
lua_V_window_পরবর্তী(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    win_T *win = (win_T *) luaV_checkcache(L, (শূন্য *) *w);
    luaV_pushwindow(L, win->w_পরবর্তী);
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
luaV_window_previous(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    win_T *win = (win_T *) luaV_checkcache(L, (শূন্য *) *w);
    luaV_pushwindow(L, win->w_পূর্ব);
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
luaV_window_isvalid(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    luaV_getudata(L, *w);
    lua_pushboolean(L, !lua_isnil(L, -1));
    ফেরত 1;
}

স্থির ধ্রুবক luaL_Reg luaV_Window_mt[] = {
    {"__tostring", luaV_window_tostring},
    {"__call", luaV_window_call},
    {"__index", lua_V_window_সূচক},
    {"__newindex", luaV_window_newindex},
    {"next", lua_V_window_পরবর্তী},
    {"previous", luaV_window_previous},
    {"isvalid", luaV_window_isvalid},
    {ফাঁকা, ফাঁকা}
};


// =======   Vim module   =======

    স্থির পূর্ণসংখ্যা
luaV_print(lua_State *L)
{
    পূর্ণসংখ্যা		i, n = lua_gettop(L); // nargs
    ধ্রুবক অক্ষর	*s;
    size_t	l;
    garray_T	বার্তা_ga;

    ga_init2(&বার্তা_ga, 1, 128);
    lua_getglobal(L, "tostring");
    জন্য (i = 1; i <= n; i++)
    {
	lua_pushvalue(L, -1); // tostring
	lua_pushvalue(L, i); // arg
	lua_call(L, 1, 1);
	s = lua_tolstring(L, -1, &l);
	যদি (s == ফাঁকা)
	    ফেরত luaL_error(L, "cannot convert to string");
	যদি (i > 1)
	    ga_append(&বার্তা_ga, ' '); // use space instead of tab
	ga_concat_len(&বার্তা_ga, (char_u *)s, l);
	lua_বের_করো(L, 1);
    }
    // Replace any "\n" with "\0"
    জন্য (i = 0; i < বার্তা_ga.ga_len; i++)
	যদি (((অক্ষর *)বার্তা_ga.ga_তথ্য)[i] == '\n')
	    ((অক্ষর *)বার্তা_ga.ga_তথ্য)[i] = '\0';
    lua_pushlstring(L, বার্তা_ga.ga_তথ্য, বার্তা_ga.ga_len);
    যদি (!got_int)
	lua_V_বার্তা(L);

    ga_clear(&বার্তা_ga);
    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
luaV_debug(lua_State *L)
{
    lua_settop(L, 0);
    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "eval");
    lua_remove(L, -2); // vim.eval at position 1
    জন্য (;;)
    {
	ধ্রুবক অক্ষর *প্রবেশ;
	size_t l;
	lua_pushvalue(L, 1); // vim.eval
	lua_pushliteral(L, "input('lua_debug> ')");
	lua_call(L, 1, 1); // return string
	প্রবেশ = lua_tolstring(L, -1, &l);
	যদি (l == 0 || strcmp(প্রবেশ, "cont") == 0)
	    ফেরত 0;
	বার্তা_putchar('\n'); // avoid outputting on input line
	যদি (luaL_loadbuffer(L, প্রবেশ, l, "=(debug command)")
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
	lua_settop(L, 1); // remove eventual returns, but keep vim.eval
    }
}

    স্থির dict_T *
luaV_get_var_scope(lua_State *L)
{
    ধ্রুবক অক্ষর	*scope = luaL_checkstring(L, 1);
    dict_T	*dict = ফাঁকা;

    যদি (STRICMP((অক্ষর *)scope, "g") == 0)
	dict = get_globvar_dict();
    অন্যথা যদি (STRICMP((অক্ষর *)scope, "v") == 0)
	dict = get_vimvar_dict();
    অন্যথা যদি (STRICMP((অক্ষর *)scope, "b") == 0)
	dict = curbuf->b_vars;
    অন্যথা যদি (STRICMP((অক্ষর *)scope, "w") == 0)
	dict = curwin->w_vars;
    অন্যথা যদি (STRICMP((অক্ষর *)scope, "t") == 0)
	dict = curtab->tp_vars;
    অন্যথা
    {
	luaL_error(L, "invalid scope %s", scope);
	ফেরত ফাঁকা;
    }

    ফেরত dict;
}

    স্থির পূর্ণসংখ্যা
luaV_setvar(lua_State *L)
{
    dict_T	*dict;
    dictitem_T	*di;
    size_t	len;
    অক্ষর	*name;
    পূর্ণসংখ্যা		del;
    অক্ষর	*error = ফাঁকা;

    name = (অক্ষর *)luaL_checklstring(L, 3, &len);
    del = (lua_gettop(L) < 4) || lua_isnil(L, 4);

    dict = luaV_get_var_scope(L);
    যদি (dict == ফাঁকা)
	ফেরত 0;

    di = dict_find(dict, (char_u *)name, (পূর্ণসংখ্যা)len);
    যদি (di != ফাঁকা)
    {
	যদি (di->di_flags & DI_FLAGS_RO)
	    error = "variable is read-only";
	অন্যথা যদি (di->di_flags & DI_FLAGS_LOCK)
	    error = "variable is locked";
	অন্যথা যদি (del && di->di_flags & DI_FLAGS_FIX)
	    error = "variable is fixed";
	যদি (error != ফাঁকা)
	    ফেরত luaL_error(L, error);
    }
    অন্যথা যদি (dict->dv_lock)
	ফেরত luaL_error(L, "Dictionary is locked");

    যদি (del)
    {
	// Delete the key
	যদি (di == ফাঁকা)
	    // Doesn't exist, nothing to do
	    ফেরত 0;
	// Delete the entry
	dictitem_remove(dict, di, "Lua delete variable");
    }
    অন্যথা
    {
	// Update the key
	typval_T	tv;

	// Convert the lua value to a Vim script type in the temporary variable
	lua_pushvalue(L, 4);
	যদি (luaV_totypval(L, -1, &tv) == FAIL)
	    ফেরত luaL_error(L, "Couldn't convert lua value");

	যদি (di == ফাঁকা)
	{
	    // Need to create an entry
	    di = dictitem_alloc((char_u *)name);
	    যদি (di == ফাঁকা)
	    {
		clear_tv(&tv);
		ফেরত 0;
	    }
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	    যদি (dict_add(dict, di) == FAIL)
	    {
		dictitem_free(di);
		clear_tv(&tv);
		ফেরত luaL_error(L, "Couldn't add to dictionary");
	    }
	}
	অন্যথা
	{
	    পূর্ণসংখ্যা type_error = FALSE;
	    যদি (dict == get_vimvar_dict()
	       && !before_set_vvar((char_u *)name, di, &tv, TRUE, &type_error))
	    {
		clear_tv(&tv);
		যদি (type_error)
		    ফেরত luaL_error(L,
				"Setting v:%s to value with wrong type", name);
		ফেরত 0;
	    }
	    // Clear the old value
	    clear_tv(&di->di_tv);
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	}

	// Clear the temporary variable
	clear_tv(&tv);
    }

    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
luaV_getvar(lua_State *L)
{
    dict_T	*dict = luaV_get_var_scope(L);
    size_t	len;
    ধ্রুবক অক্ষর	*name = luaL_checklstring(L, 3, &len);
    dictitem_T	*di = dict_find(dict, (char_u *)name, (পূর্ণসংখ্যা)len);

    যদি (di == ফাঁকা)
	ফেরত 0;  // nil

    luaV_pushtypval(L, &di->di_tv);
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
luaV_command(lua_State *L)
{
    char_u  *s = vim_strsave((char_u *)luaL_checkstring(L, 1));

    execute_cmds_from_string(s);
    vim_free(s);
    update_screen(UPD_VALID);
    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
luaV_eval(lua_State *L)
{
    typval_T *tv = eval_রাশিমালা((char_u *) luaL_checkstring(L, 1), ফাঁকা);

    যদি (tv == ফাঁকা) luaL_error(L, "invalid expression");
    luaV_pushtypval(L, tv);
    free_tv(tv);
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
luaV_beep(lua_State *L UNUSED)
{
    vim_beep(BO_LANG);
    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
lua_V_সারি(lua_State *L)
{
    luaV_pushline(L, curbuf, curwin->w_cursor.lnum);
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
luaV_list(lua_State *L)
{
    list_T *l;
    পূর্ণসংখ্যা initarg = !lua_isnoneornil(L, 1);

    যদি (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    l = list_alloc();
    যদি (l == ফাঁকা)
    {
	lua_pushnil(L);
	ফেরত 1;
    }

    luaV_newlist(L, l);
    যদি (!initarg)
	ফেরত 1;

    // traverse table to init list
    পূর্ণসংখ্যা notnil, i = 0;
    typval_T v;
    করো
    {
	lua_rawgeti(L, 1, ++i);
	notnil = !lua_isnil(L, -1);
	যদি (notnil)
	{
	    luaV_checktypval(L, -1, &v, "vim.list");
	    list_append_tv(l, &v);
	    clear_tv(&v);
	}
	lua_বের_করো(L, 1); //মান
    } যখন (notnil);

    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
luaV_dict(lua_State *L)
{
    dict_T *d;
    পূর্ণসংখ্যা initarg = !lua_isnoneornil(L, 1);

    যদি (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    d = dict_alloc();
    যদি (d == ফাঁকা)
    {
	lua_pushnil(L);
	ফেরত 1;
    }

    luaV_newdict(L, d);
    যদি (!initarg)
	ফেরত 1;

    // traverse table to init dict
    lua_pushnil(L);
    যখন (lua_পরবর্তী(L, 1))
    {
	char_u *চাবি;
	dictitem_T *di;
	typval_T v;

	lua_pushvalue(L, -2); // dup key in case it's a number
	চাবি = (char_u *) lua_tostring(L, -1);
	যদি (চাবি == ফাঁকা)
	{
	    lua_pushnil(L);
	    ফেরত 1;
	}
	যদি (*চাবি == NUL)
	    luaL_error(L, "table has empty key");
	luaV_checktypval(L, -2, &v, "vim.dict"); //মান
	di = dictitem_alloc(চাবি);
	যদি (di == ফাঁকা || dict_add(d, di) == FAIL)
	{
	    vim_free(di);
	    lua_pushnil(L);
	    ফেরত 1;
	}
	di->di_tv = v;
	lua_বের_করো(L, 2); // key copy and value
    }

    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
luaV_blob(lua_State *L)
{
    blob_T *b;
    পূর্ণসংখ্যা initarg = !lua_isnoneornil(L, 1);

    যদি (initarg && !lua_isstring(L, 1))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 1));

    b = blob_alloc();
    যদি (b == ফাঁকা)
    {
	lua_pushnil(L);
	ফেরত 1;
    }

    luaV_newblob(L, b);
    যদি (!initarg)
	ফেরত 1;

    // traverse table to init blob
    size_t i, l = 0;
    ধ্রুবক অক্ষর *s = lua_tolstring(L, 1, &l);

    যদি (ga_grow(&b->bv_ga, (পূর্ণসংখ্যা)l) == OK)
	জন্য (i = 0; i < l; ++i)
	    ga_append(&b->bv_ga, s[i]);

    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
luaV_funcref(lua_State *L)
{
    ধ্রুবক অক্ষর *name = luaL_checkstring(L, 1);
    // note: not checking if function exists (needs function_exists)
    যদি (name == ফাঁকা || *name == NUL || VIM_ISDIGIT(*name))
	luaL_error(L, "invalid function name: %s", name);
    luaV_newfuncref(L, (char_u *) name);
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
lua_V_বাফার_2(lua_State *L)
{
    buf_T *buf;
    যদি (lua_isstring(L, 1)) // get by number or name?
    {
	যদি (lua_isnumber(L, 1)) // by number?
	{
	    পূর্ণসংখ্যা n = lua_tointeger(L, 1);
	    FOR_ALL_BUFFERS(buf)
		যদি (buf->b_fnum == n) বিরতি;
	}
	অন্যথা // by name
	{
	    size_t l;
	    ধ্রুবক অক্ষর *s = lua_tolstring(L, 1, &l);
	    FOR_ALL_BUFFERS(buf)
	    {
		যদি (buf->b_ffname == ফাঁকা || buf->b_sfname == ফাঁকা)
		{
		    যদি (l == 0) বিরতি;
		}
		অন্যথা যদি (strncmp(s, (অক্ষর *)buf->b_ffname, l) == 0
			|| strncmp(s, (অক্ষর *)buf->b_sfname, l) == 0)
		    বিরতি;
	    }
	}
    }
    অন্যথা
	buf = (lua_toboolean(L, 1)) ? firstbuf : curbuf; // first buffer?
    luaV_pushbuffer(L, buf);
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
luaV_window(lua_State *L)
{
    win_T *win;
    যদি (lua_isnumber(L, 1)) // get by number?
    {
	পূর্ণসংখ্যা n = lua_tointeger(L, 1);
	জন্য (win = firstwin; win != ফাঁকা; win = win->w_পরবর্তী, n--)
	    যদি (n == 1) বিরতি;
    }
    অন্যথা
	win = (lua_toboolean(L, 1)) ? firstwin : curwin; // first window?
    luaV_pushwindow(L, win);
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
luaV_open(lua_State *L)
{
    char_u *s = ফাঁকা;
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    যদি (lua_isstring(L, 1)) s = (char_u *) lua_tostring(L, 1);
    luaV_pushbuffer(L, buflist_new(s, ফাঁকা, 1L, BLN_LISTED));
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
luaV_type(lua_State *L)
{
    luaL_checkany(L, 1);
    যদি (lua_type(L, 1) == LUA_TUSERDATA) // check vim udata?
    {
	lua_settop(L, 1);
	যদি (lua_getmetatable(L, 1))
	{
	    luaV_getfield(L, LUAVIM_LIST);
	    যদি (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "list");
		ফেরত 1;
	    }
	    luaV_getfield(L, LUAVIM_DICT);
	    যদি (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "dict");
		ফেরত 1;
	    }
	    luaV_getfield(L, LUAVIM_BLOB);
	    যদি (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "blob");
		ফেরত 1;
	    }
	    luaV_getfield(L, LUAVIM_FUNCREF);
	    যদি (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "funcref");
		ফেরত 1;
	    }
	    luaV_getfield(L, LUAVIM_বাফার);
	    যদি (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "buffer");
		ফেরত 1;
	    }
	    luaV_getfield(L, LUAVIM_WINDOW);
	    যদি (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "window");
		ফেরত 1;
	    }
	}
    }
    lua_pushstring(L, luaL_typename(L, 1)); // fallback
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
luaV_call(lua_State *L)
{
    পূর্ণসংখ্যা		argc = lua_gettop(L) - 1;
    size_t	funcname_len;
    char_u	*funcname;
    অক্ষর	*error = ফাঁকা;
    typval_T	rettv;
    typval_T	argv[MAX_FUNC_ARGS + 1];
    পূর্ণসংখ্যা		i = 0;

    যদি (argc > MAX_FUNC_ARGS)
	ফেরত luaL_error(L, "Function called with too many arguments");

    funcname = (char_u *)luaL_checklstring(L, 1, &funcname_len);

    জন্য (; i < argc; i++)
    {
	যদি (luaV_totypval(L, i + 2, &argv[i]) == FAIL)
	{
	    error = "lua: cannot convert value";
	    যাও free_vim_args;
	}
    }

    argv[argc].v_type = VAR_UNKNOWN;

    যদি (call_vim_function(funcname, argc, argv, &rettv) == FAIL)
    {
	error = "lua: call_vim_function failed";
	যাও free_vim_args;
    }

    luaV_pushtypval(L, &rettv);
    clear_tv(&rettv);

free_vim_args:
    যখন (i > 0)
	clear_tv(&argv[--i]);

    যদি (error == ফাঁকা)
	ফেরত 1;
    অন্যথা
	ফেরত luaL_error(L, error);
}

/*
 * Return the Vim version as a Lua table
 */
    স্থির পূর্ণসংখ্যা
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
    ফেরত 1;
}

স্থির ধ্রুবক luaL_Reg luaV_module[] = {
    {"command", luaV_command},
    {"eval", luaV_eval},
    {"beep", luaV_beep},
    {"line", lua_V_সারি},
    {"list", luaV_list},
    {"dict", luaV_dict},
    {"blob", luaV_blob},
    {"funcref", luaV_funcref},
    {"buffer", lua_V_বাফার_2},
    {"window", luaV_window},
    {"open", luaV_open},
    {"type", luaV_type},
    {"call", luaV_call},
    {"_getvar", luaV_getvar},
    {"_setvar", luaV_setvar},
    {"version", luaV_version},
    {"lua_version", ফাঁকা},
    {ফাঁকা, ফাঁকা}
};

/*
 * for freeing list, dict, buffer and window objects; lightuserdata as arg
 */
    স্থির পূর্ণসংখ্যা
luaV_free(lua_State *L)
{
    lua_pushnil(L);
    luaV_setudata(L, lua_touserdata(L, 1));
    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
luaV_luaeval(lua_State *L)
{
    lua_L_বাফার b;
    size_t l;
    ধ্রুবক অক্ষর *str = lua_tolstring(L, 1, &l);
    typval_T *arg = (typval_T *) lua_touserdata(L, 2);
    typval_T *rettv = (typval_T *) lua_touserdata(L, 3);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, LUAVIM_EVALHEADER, আকার(LUAVIM_EVALHEADER) - 1);
    luaL_addlstring(&b, str, l);
    luaL_pushresult(&b);
    str = lua_tolstring(L, -1, &l);
    যদি (luaL_loadbuffer(L, str, l, LUAVIM_EVALNAME)) // compile error?
    {
	luaV_emsg(L);
	ফেরত 0;
    }
    luaV_pushtypval(L, arg);
    যদি (lua_pcall(L, 1, 1, 0)) // running error?
    {
	luaV_emsg(L);
	ফেরত 0;
    }
    যদি (luaV_totypval(L, -1, rettv) == FAIL)
	emsg("luaeval: cannot convert value");
    ফেরত 0;
}

    স্থির পূর্ণসংখ্যা
luaV_setref(lua_State *L)
{
    পূর্ণসংখ্যা copyID = lua_tointeger(L, 1);
    পূর্ণসংখ্যা abort = FALSE;

    lua_pushlightuserdata(L, (শূন্য *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // the cache table

    luaV_getfield(L, LUAVIM_LIST);
    luaV_getfield(L, LUAVIM_DICT);
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_pushnil(L);
    // traverse cache table
    যখন (!abort && lua_পরবর্তী(L, 2) != 0)
    {
	lua_getmetatable(L, -1);
	যদি (lua_rawequal(L, -1, 3)) // list?
	{
	    list_T *l = (list_T *)lua_touserdata(L, 6); //চাবি

	    abort = set_ref_in_list(l, copyID);
	}
	অন্যথা যদি (lua_rawequal(L, -1, 4)) // dict?
	{
	    dict_T *d = (dict_T *)lua_touserdata(L, 6); //চাবি

	    abort = set_ref_in_dict(d, copyID);
	}
	অন্যথা যদি (lua_rawequal(L, -1, 5)) // funcref?
	{
	    luaV_Funcref *f = (luaV_Funcref *)lua_touserdata(L, 6); //চাবি

	    abort = set_ref_in_dict(f->self, copyID);
	}
	lua_বের_করো(L, 2); // metatable and value
    }
    lua_pushinteger(L, abort);
    ফেরত 1;
}

    স্থির পূর্ণসংখ্যা
luaV_pushversion(lua_State *L)
{
    পূর্ণসংখ্যা major = 0;
    পূর্ণসংখ্যা minor = 0;
    পূর্ণসংখ্যা patch = 0;
    অক্ষর s[16];

    sscanf(LUAVIM_VERSION, "Lua %d.%d.%d", &major, &minor, &patch);
    vim_snprintf(s, আকার(s), "%d.%d.%d", major, minor, patch);
    lua_pushstring(L, s);
    ফেরত 0;
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

    স্থির পূর্ণসংখ্যা
luaopen_vim(lua_State *L)
{
    lua_newtable(L);  // cache table
    lua_newtable(L);  // cache table's metatable
    lua_pushstring(L, "v");
    lua_setfield(L, -2, "__mode");
    lua_setmetatable(L, -2); // cache is weak-valued
    // put the cache table in the registry for luaV_get/setudata()
    lua_pushlightuserdata(L, (শূন্য *) LUAVIM_UDATA_CACHE);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
    lua_বের_করো(L, 1);  // we don't need the cache table here anymore
    // print
    lua_pushcfunction(L, luaV_print);
    lua_setglobal(L, "print");
    // debug.debug
    lua_getglobal(L, "debug");
    lua_pushcfunction(L, luaV_debug);
    lua_setfield(L, -2, "debug");
    lua_বের_করো(L, 1);
    // free
    lua_pushlightuserdata(L, (শূন্য *) LUAVIM_FREE);
    lua_pushcfunction(L, luaV_free);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // luaeval
    lua_pushlightuserdata(L, (শূন্য *) LUAVIM_LUAEVAL);
    lua_pushcfunction(L, luaV_luaeval);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // setref
    lua_pushlightuserdata(L, (শূন্য *) LUAVIM_SETREF);
    lua_pushcfunction(L, luaV_setref);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // register
    luaV_newmetatable(L, LUAVIM_LIST);
    luaV_register(L, luaV_List_mt);
    lua_বের_করো(L, 1);
    luaV_newmetatable(L, LUAVIM_DICT);
    luaV_register(L, luaV_Dict_mt);
    lua_বের_করো(L, 1);
    luaV_newmetatable(L, LUAVIM_BLOB);
    luaV_register(L, luaV_Blob_mt);
    lua_বের_করো(L, 1);
    luaV_newmetatable(L, LUAVIM_FUNCREF);
    luaV_register(L, luaV_Funcref_mt);
    lua_বের_করো(L, 1);
    luaV_newmetatable(L, LUAVIM_বাফার);
    luaV_register(L, lua_V_বাফার_mt);
    lua_বের_করো(L, 1);
    luaV_newmetatable(L, LUAVIM_WINDOW);
    luaV_register(L, luaV_Window_mt);
    lua_বের_করো(L, 1);
    lua_newtable(L); // vim table
    luaV_register(L, luaV_module);
    luaV_pushversion(L);
    lua_setfield(L, -2, "lua_version");
    lua_setglobal(L, LUAVIM_NAME);
    // custom code
    (শূন্য)luaL_dostring(L, LUA_VIM_FN_CODE);
    (শূন্য)luaL_dostring(L, LUA_VIM_UPDATE_PACKAGE_PATHS);
    (শূন্য)luaL_dostring(L, LUA_VIM_SETUP_VARIABLE_DICTS);

    lua_getglobal(L, LUAVIM_NAME);
    lua_getfield(L, -1, "_update_package_paths");

    যদি (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);

    ফেরত 0;
}

    স্থির lua_State *
luaV_newstate(শূন্য)
{
    lua_State *L = luaL_newstate();
# যদি LUA_VERSION_অঙ্ক >= 505
    luaL_openselectedlibs(L, ~0, 0); // core libs
# অন্যথা
    luaL_openlibs(L); // core libs
# endif
    lua_pushcfunction(L, luaopen_vim); // vim
    lua_call(L, 0, 0);
    ফেরত L;
}

    স্থির শূন্য
luaV_setrange(lua_State *L, পূর্ণসংখ্যা line1, পূর্ণসংখ্যা line2)
{
    lua_getglobal(L, LUAVIM_NAME);
    lua_pushinteger(L, line1);
    lua_setfield(L, -2, "firstline");
    lua_pushinteger(L, line2);
    lua_setfield(L, -2, "lastline");
    lua_বের_করো(L, 1); // vim table
}


// =======   Interface   =======

স্থির lua_State *L = ফাঁকা;

    স্থির পূর্ণসংখ্যা
lua_isopen(শূন্য)
{
    ফেরত L != ফাঁকা;
}

    স্থির পূর্ণসংখ্যা
lua_init(শূন্য)
{
    যদি (lua_isopen())
	ফেরত OK;

# ifdef DYNAMIC_LUA
    যদি (!lua_enabled(TRUE))
    {
	emsg(_("Lua library cannot be loaded."));
	ফেরত FAIL;
    }
# endif
    L = luaV_newstate();

    ফেরত OK;
}

    শূন্য
lua_end(শূন্য)
{
    যদি (!lua_isopen())
	ফেরত;

    lua_close(L);
    L = ফাঁকা;
}

/*
 * ex commands
 */
    শূন্য
ex_lua(exarg_T *eap)
{
    অক্ষর *script = (অক্ষর *)script_get(eap, eap->arg);

    যদি (!eap->skip && lua_init() == OK)
    {
	অক্ষর *s = script != ফাঁকা ? script : (অক্ষর *)eap->arg;

	luaV_setrange(L, eap->line1, eap->line2);
	যদি (luaL_loadbuffer(L, s, strlen(s), LUAVIM_CHUNKNAME)
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
    যদি (script != ফাঁকা)
	vim_free(script);
}

    শূন্য
ex_luado(exarg_T *eap)
{
    linenr_T l;
    ধ্রুবক অক্ষর *s = (ধ্রুবক অক্ষর *) eap->arg;
    lua_L_বাফার b;
    size_t len;
    buf_T *was_curbuf = curbuf;

    যদি (lua_init() == FAIL) ফেরত;
    যদি (u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
    {
	emsg(_("cannot save undo information"));
	ফেরত;
    }
    luaV_setrange(L, eap->line1, eap->line2);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, "return function(line, linenr) ", 30); // header
    luaL_addlstring(&b, s, strlen(s));
    luaL_addlstring(&b, " end", 4); // footer
    luaL_pushresult(&b);
    s = lua_tolstring(L, -1, &len);
    যদি (luaL_loadbuffer(L, s, len, LUAVIM_CHUNKNAME))
    {
	luaV_emsg(L);
	lua_বের_করো(L, 1); // function body
	ফেরত;
    }
    lua_call(L, 0, 1);
    lua_replace(L, -2); // function -> body
    জন্য (l = eap->line1; l <= eap->line2; l++)
    {
	// Check the line number, the command may have deleted lines.
	যদি (l > curbuf->b_ml.ml_সারি_গুনতি)
	    বিরতি;

	lua_pushvalue(L, -1); // function
	luaV_pushline(L, curbuf, l); // current line as arg
	lua_pushinteger(L, l); // current line number as arg
	যদি (lua_pcall(L, 2, 1, 0))
	{
	    luaV_emsg(L);
	    বিরতি;
	}

	// Catch the command switching to another buffer.
	// Check the line number, the command may have deleted lines.
	যদি (curbuf != was_curbuf || l > curbuf->b_ml.ml_সারি_গুনতি)
	    বিরতি;

	যদি (lua_isstring(L, -1)) // update line?
	{
# ifdef HAVE_SANDBOX
	    luaV_checksandbox(L);
# endif
	    ml_replace(l, luaV_toline(L, -1), TRUE);
	    changed_bytes(l, 0);
	    lua_বের_করো(L, 1); // result from luaV_toline
	}
	lua_বের_করো(L, 1); //সারি
    }
    lua_বের_করো(L, 1); // function
    check_cursor();
    update_screen(UPD_NOT_VALID);
}

    শূন্য
ex_luafile(exarg_T *eap)
{
    যদি (lua_init() == FAIL)
	ফেরত;
    যদি (!eap->skip)
    {
	luaV_setrange(L, eap->line1, eap->line2);
	যদি (luaL_loadfile(L, (অক্ষর *) eap->arg) || lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
}

# define luaV_freetype(typ,tname) \
	শূন্য \
    lua_##tname##_free(typ *o) \
    { \
	যদি (!lua_isopen()) ফেরত; \
	luaV_getfield(L, LUAVIM_FREE); \
	lua_pushlightuserdata(L, (শূন্য *) o); \
	lua_call(L, 1, 0); \
    }

luaV_freetype(buf_T, বাফার)
luaV_freetype(win_T, window)

    শূন্য
do_luaeval(char_u *str, typval_T *arg, typval_T *rettv)
{
    lua_init();
    luaV_getfield(L, LUAVIM_LUAEVAL);
    lua_pushstring(L, (অক্ষর *) str);
    lua_pushlightuserdata(L, (শূন্য *) arg);
    lua_pushlightuserdata(L, (শূন্য *) rettv);
    lua_call(L, 3, 0);
}

    পূর্ণসংখ্যা
set_ref_in_lua(পূর্ণসংখ্যা copyID)
{
    পূর্ণসংখ্যা aborted = 0;

    যদি (!lua_isopen())
	ফেরত 0;

    luaV_getfield(L, LUAVIM_SETREF);
    // call the function with 1 arg, getting 1 result back
    lua_pushinteger(L, copyID);
    lua_call(L, 1, 1);
    // get the result
    aborted = lua_tointeger(L, -1);
    // pop result off the stack
    lua_বের_করো(L, 1);

    ফেরত aborted;
}

    শূন্য
update_package_paths_in_lua(শূন্য)
{
    যদি (!lua_isopen())
	ফেরত;

    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "_update_package_paths");

    যদি (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);
}

/*
 * Native C function callback
 */
    স্থির পূর্ণসংখ্যা
luaV_call_lua_func(
	পূর্ণসংখ্যা	 argcount,
	typval_T *argvars,
	typval_T *rettv,
	শূন্য	 *state)
{
    পূর্ণসংখ্যা i;
    পূর্ণসংখ্যা luaargcount = argcount;
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_funcref);

    যদি (funcstate->lua_tableref != LUA_NOREF)
    {
	// First arg for metatable __call method is a table
	luaargcount += 1;
	lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    }

    জন্য (i = 0; i < argcount; ++i)
	luaV_pushtypval(funcstate->L, &argvars[i]);

    যদি (lua_pcall(funcstate->L, luaargcount, 1, 0))
    {
	luaV_emsg(funcstate->L);
	ফেরত (পূর্ণসংখ্যা)FCERR_OTHER;
    }

    luaV_checktypval(funcstate->L, -1, rettv, "get return value");
    ফেরত (পূর্ণসংখ্যা)FCERR_NONE;
}

/*
 * Free up any lua references held by the func state.
 */
    স্থির শূন্য
luaV_call_lua_func_free(শূন্য *state)
{
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_funcref);
    funcstate->L = ফাঁকা;
    যদি (funcstate->lua_tableref != LUA_NOREF)
	luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    VIM_CLEAR(funcstate);
}

#endif
