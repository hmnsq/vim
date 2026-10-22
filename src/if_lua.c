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

#अन्तर्भाव "vim.h"
#अन्तर्भाव "version.h"

#अन्तर्भाव <lua.h>
#अन्तर्भाव <lualib.h>
#अन्तर्भाव <lauxlib.h>

#जर __STDC_VERSION__ >= 199901L
# define LUAV_INLINE ओळीत
#अन्यथा
# define LUAV_INLINE
#endif

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#जर defined(FEAT_LUA)

# define LUAVIM_CHUNKNAME "vim chunk"
# define LUAVIM_NAME "vim"
# define LUAVIM_EVALNAME "luaeval"
# define LUAVIM_EVALHEADER "local _A=select(1,...) return "

# ifdef LUA_RELEASE
#  define LUAVIM_VERSION LUA_RELEASE
# अन्यथा
#  define LUAVIM_VERSION LUA_VERSION
# endif

प्रकारव्याख्या buf_T *luaV_Buffer;
प्रकारव्याख्या win_T *luaV_Window;
प्रकारव्याख्या dict_T *luaV_Dict;
प्रकारव्याख्या list_T *luaV_List;
प्रकारव्याख्या blob_T *luaV_Blob;
प्रकारव्याख्या संरचना {
    char_u	*name;	// funcref
    dict_T	*self;	// selfdict
} luaV_Funcref;
प्रकारव्याख्या पूर्णांक (*msgfunc_T)(अक्षर *);

प्रकारव्याख्या संरचना {
    पूर्णांक lua_funcref;    // ref to a lua func
    पूर्णांक lua_tableref;   // ref to a lua table if metatable else LUA_NOREF. used
			// for __call
    lua_State *L;
} luaV_CFuncState;

स्थिर स्थिरांक अक्षर LUAVIM_DICT[] = "dict";
स्थिर स्थिरांक अक्षर LUAVIM_LIST[] = "list";
स्थिर स्थिरांक अक्षर LUAVIM_BLOB[] = "blob";
स्थिर स्थिरांक अक्षर LUAVIM_FUNCREF[] = "funcref";
स्थिर स्थिरांक अक्षर LUAVIM_BUFFER[] = "buffer";
स्थिर स्थिरांक अक्षर LUAVIM_WINDOW[] = "window";
स्थिर स्थिरांक अक्षर LUAVIM_FREE[] = "luaV_free";
स्थिर स्थिरांक अक्षर LUAVIM_LUAEVAL[] = "luaV_luaeval";
स्थिर स्थिरांक अक्षर LUAVIM_SETREF[] = "luaV_setref";

स्थिर स्थिरांक अक्षर LUA___CALL[] = "__call";

// get/setudata manage references to vim userdata in a cache table through
// object pointers (light userdata). The cache table itself is retrieved
// from the registry.

स्थिर स्थिरांक अक्षर LUAVIM_UDATA_CACHE[] = "luaV_udata_cache";

# define luaV_getfield(L, s) \
    lua_pushlightuserdata((L), (शून्य *)(s)); \
    lua_rawget((L), LUA_REGISTRYINDEX)
# define luaV_checksandbox(L) \
    जर (sandbox) luaL_error((L), "not allowed in sandbox")
# define lua_V_संदेश(L) luaV_msgfunc((L), (msgfunc_T) संदेश)
# define luaV_emsg(L) luaV_msgfunc((L), (msgfunc_T) emsg)
# define luaV_checktypval(L, a, v, संदेश) \
    करा { \
	जर (luaV_totypval(L, a, v) == FAIL) \
	    luaL_error(L, संदेश ": cannot convert value"); \
    } पर्यंत (0)

स्थिर luaV_List *luaV_pushlist(lua_State *L, list_T *lis);
स्थिर luaV_Dict *luaV_pushdict(lua_State *L, dict_T *dic);
स्थिर luaV_Blob *luaV_pushblob(lua_State *L, blob_T *blo);
स्थिर luaV_Funcref *luaV_pushfuncref(lua_State *L, char_u *name);
स्थिर पूर्णांक luaV_call_lua_func(पूर्णांक argcount, typval_T *argvars, typval_T *rettv, शून्य *state);
स्थिर शून्य luaV_call_lua_func_free(शून्य *state);

# जर LUA_VERSION_अंक <= 501
#  define luaV_register(L, l) luaL_register(L, रिक्त, l)
#  define luaL_typeerror luaL_typerror
# अन्यथा
#  define luaV_register(L, l) luaL_setfuncs(L, l, 0)
# endif

# ifdef DYNAMIC_LUA

#  ifdef MSWIN
#   define load_dll vimLoadLib
#   define चिन्ह_from_dll GetProcAddress
#   define close_dll FreeLibrary
#   define load_dll_error GetWin32Error
#  अन्यथा
#   include <dlfcn.h>
#   define HANDLE शून्य*
#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#   define चिन्ह_from_dll dlsym
#   define close_dll dlclose
#   define load_dll_error dlerror
#  endif

// lauxlib
#  जर LUA_VERSION_अंक <= 501
#   define luaL_register dll_luaL_register
#   define luaL_prepbuffer dll_luaL_prepbuffer
#   define luaL_openlib dll_luaL_openlib
#   define luaL_typerror dll_luaL_typerror
#   define luaL_loadfile dll_luaL_loadfile
#   define luaL_loadbuffer dll_luaL_loadbuffer
#  अन्यथा
#   define luaL_prepbuffsize dll_luaL_prepbuffsize
#   define luaL_setfuncs dll_luaL_setfuncs
#   define luaL_loadfilex dll_luaL_loadfilex
#   define luaL_loadbufferx dll_luaL_loadbufferx
#   define luaL_argerror dll_luaL_argerror
#  endif
#  जर LUA_VERSION_अंक >= 504
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
#  जर LUA_VERSION_अंक <= 501
#   define lua_tonumber dll_lua_tonumber
#   define lua_tointeger dll_lua_tointeger
#   define lua_call dll_lua_call
#   define lua_pcall dll_lua_pcall
#  अन्यथा
#   define lua_tonumberx dll_lua_tonumberx
#   define lua_tointegerx dll_lua_tointegerx
#   define lua_callk dll_lua_callk
#   define lua_pcallk dll_lua_pcallk
#   define lua_getglobal dll_lua_getglobal
#   define lua_setglobal dll_lua_setglobal
#  endif
#  जर LUA_VERSION_अंक <= 502
#   define lua_replace dll_lua_replace
#   define lua_remove dll_lua_remove
#  endif
#  जर LUA_VERSION_अंक >= 503
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
#  जर LUA_VERSION_अंक >= 504
#   define lua_newuserdatauv dll_lua_newuserdatauv
#  अन्यथा
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
#  जर LUA_VERSION_अंक >= 505
#   define luaL_openselectedlibs dll_luaL_openselectedlibs
#  अन्यथा
#   define luaL_openlibs dll_luaL_openlibs
#  endif

// lauxlib
#  जर LUA_VERSION_अंक <= 501
शून्य (*dll_luaL_register) (lua_State *L, स्थिरांक अक्षर *libname, स्थिरांक luaL_Reg *l);
अक्षर *(*dll_luaL_prepbuffer) (luaL_Buffer *B);
शून्य (*dll_luaL_openlib) (lua_State *L, स्थिरांक अक्षर *libname, स्थिरांक luaL_Reg *l, पूर्णांक nup);
पूर्णांक (*dll_luaL_typerror) (lua_State *L, पूर्णांक narg, स्थिरांक अक्षर *tname);
पूर्णांक (*dll_luaL_loadfile) (lua_State *L, स्थिरांक अक्षर *filename);
पूर्णांक (*dll_luaL_loadbuffer) (lua_State *L, स्थिरांक अक्षर *buff, माप_प्रकार sz, स्थिरांक अक्षर *name);
#  अन्यथा
अक्षर *(*dll_luaL_prepbuffsize) (luaL_Buffer *B, माप_प्रकार sz);
शून्य (*dll_luaL_setfuncs) (lua_State *L, स्थिरांक luaL_Reg *l, पूर्णांक nup);
पूर्णांक (*dll_luaL_loadfilex) (lua_State *L, स्थिरांक अक्षर *filename, स्थिरांक अक्षर *mode);
पूर्णांक (*dll_luaL_loadbufferx) (lua_State *L, स्थिरांक अक्षर *buff, माप_प्रकार sz, स्थिरांक अक्षर *name, स्थिरांक अक्षर *mode);
पूर्णांक (*dll_luaL_argerror) (lua_State *L, पूर्णांक numarg, स्थिरांक अक्षर *extramsg);
#  endif
#  जर LUA_VERSION_अंक >= 504
पूर्णांक (*dll_luaL_typeerror) (lua_State *L, पूर्णांक narg, स्थिरांक अक्षर *tname);
#  endif
शून्य (*dll_luaL_checkany) (lua_State *L, पूर्णांक narg);
स्थिरांक अक्षर *(*dll_luaL_checklstring) (lua_State *L, पूर्णांक अंक_Arg, माप_प्रकार *l);
lua_Integer (*dll_luaL_checkinteger) (lua_State *L, पूर्णांक अंक_Arg);
lua_Integer (*dll_luaL_optinteger) (lua_State *L, पूर्णांक nArg, lua_Integer def);
शून्य (*dll_luaL_checktype) (lua_State *L, पूर्णांक narg, पूर्णांक t);
पूर्णांक (*dll_luaL_error) (lua_State *L, स्थिरांक अक्षर *fmt, ...);
lua_State *(*dll_luaL_newstate) (शून्य);
शून्य (*dll_luaL_buffinit) (lua_State *L, luaL_Buffer *B);
शून्य (*dll_luaL_addlstring) (luaL_Buffer *B, स्थिरांक अक्षर *s, माप_प्रकार l);
शून्य (*dll_luaL_pushresult) (luaL_Buffer *B);
पूर्णांक (*dll_luaL_loadstring) (lua_State *L, स्थिरांक अक्षर *s);
पूर्णांक (*dll_luaL_ref) (lua_State *L, पूर्णांक idx);
#  जर LUA_VERSION_अंक <= 502
शून्य (*dll_luaL_unref) (lua_State *L, पूर्णांक idx, पूर्णांक n);
#  अन्यथा
शून्य (*dll_luaL_unref) (lua_State *L, पूर्णांक idx, lua_Integer n);
#  endif
// lua
#  जर LUA_VERSION_अंक <= 501
lua_संख्या (*dll_lua_tonumber) (lua_State *L, पूर्णांक idx);
lua_Integer (*dll_lua_tointeger) (lua_State *L, पूर्णांक idx);
शून्य (*dll_lua_call) (lua_State *L, पूर्णांक nargs, पूर्णांक nresults);
पूर्णांक (*dll_lua_pcall) (lua_State *L, पूर्णांक nargs, पूर्णांक nresults, पूर्णांक errfunc);
#  अन्यथा
lua_संख्या (*dll_lua_tonumberx) (lua_State *L, पूर्णांक idx, पूर्णांक *isnum);
lua_Integer (*dll_lua_tointegerx) (lua_State *L, पूर्णांक idx, पूर्णांक *isnum);
शून्य (*dll_lua_callk) (lua_State *L, पूर्णांक nargs, पूर्णांक nresults, पूर्णांक ctx,
	lua_CFunction k);
पूर्णांक (*dll_lua_pcallk) (lua_State *L, पूर्णांक nargs, पूर्णांक nresults, पूर्णांक errfunc,
	पूर्णांक ctx, lua_CFunction k);
शून्य (*dll_lua_getglobal) (lua_State *L, स्थिरांक अक्षर *var);
शून्य (*dll_lua_setglobal) (lua_State *L, स्थिरांक अक्षर *var);
#  endif
#  जर LUA_VERSION_अंक <= 502
शून्य (*dll_lua_replace) (lua_State *L, पूर्णांक idx);
शून्य (*dll_lua_remove) (lua_State *L, पूर्णांक idx);
#  endif
#  जर LUA_VERSION_अंक >= 503
शून्य  (*dll_lua_rotate) (lua_State *L, पूर्णांक idx, पूर्णांक n);
शून्य (*dll_lua_copy) (lua_State *L, पूर्णांक fromidx, पूर्णांक toidx);
#  endif
स्थिरांक अक्षर *(*dll_lua_typename) (lua_State *L, पूर्णांक tp);
शून्य       (*dll_lua_close) (lua_State *L);
पूर्णांक (*dll_lua_gettop) (lua_State *L);
शून्य (*dll_lua_settop) (lua_State *L, पूर्णांक idx);
शून्य (*dll_lua_pushvalue) (lua_State *L, पूर्णांक idx);
पूर्णांक (*dll_lua_isnumber) (lua_State *L, पूर्णांक idx);
पूर्णांक (*dll_lua_isstring) (lua_State *L, पूर्णांक idx);
पूर्णांक (*dll_lua_type) (lua_State *L, पूर्णांक idx);
पूर्णांक (*dll_lua_rawequal) (lua_State *L, पूर्णांक idx1, पूर्णांक idx2);
पूर्णांक (*dll_lua_toboolean) (lua_State *L, पूर्णांक idx);
स्थिरांक अक्षर *(*dll_lua_tolstring) (lua_State *L, पूर्णांक idx, माप_प्रकार *len);
शून्य *(*dll_lua_touserdata) (lua_State *L, पूर्णांक idx);
शून्य (*dll_lua_pushnil) (lua_State *L);
शून्य (*dll_lua_pushnumber) (lua_State *L, lua_संख्या n);
शून्य (*dll_lua_pushinteger) (lua_State *L, lua_Integer n);
शून्य (*dll_lua_pushlstring) (lua_State *L, स्थिरांक अक्षर *s, माप_प्रकार l);
शून्य (*dll_lua_pushstring) (lua_State *L, स्थिरांक अक्षर *s);
स्थिरांक अक्षर *(*dll_lua_pushfstring) (lua_State *L, स्थिरांक अक्षर *fmt, ...);
शून्य (*dll_lua_pushcclosure) (lua_State *L, lua_CFunction fn, पूर्णांक n);
शून्य (*dll_lua_pushboolean) (lua_State *L, पूर्णांक b);
शून्य (*dll_lua_pushlightuserdata) (lua_State *L, शून्य *p);
शून्य (*dll_lua_getfield) (lua_State *L, पूर्णांक idx, स्थिरांक अक्षर *k);
#  जर LUA_VERSION_अंक <= 502
शून्य (*dll_lua_rawget) (lua_State *L, पूर्णांक idx);
शून्य (*dll_lua_rawgeti) (lua_State *L, पूर्णांक idx, पूर्णांक n);
#  अन्यथा
पूर्णांक (*dll_lua_rawget) (lua_State *L, पूर्णांक idx);
पूर्णांक (*dll_lua_rawgeti) (lua_State *L, पूर्णांक idx, lua_Integer n);
#  endif
शून्य (*dll_lua_createtable) (lua_State *L, पूर्णांक narr, पूर्णांक nrec);
शून्य (*dll_lua_settable) (lua_State *L, पूर्णांक idx);
#  जर LUA_VERSION_अंक >= 504
शून्य *(*dll_lua_newuserdatauv) (lua_State *L, माप_प्रकार sz, पूर्णांक nuvalue);
#  अन्यथा
शून्य *(*dll_lua_newuserdata) (lua_State *L, माप_प्रकार sz);
#  endif
पूर्णांक (*dll_lua_getmetatable) (lua_State *L, पूर्णांक objindex);
शून्य (*dll_lua_setfield) (lua_State *L, पूर्णांक idx, स्थिरांक अक्षर *k);
शून्य (*dll_lua_rawset) (lua_State *L, पूर्णांक idx);
#  जर LUA_VERSION_अंक <= 502
शून्य (*dll_lua_rawseti) (lua_State *L, पूर्णांक idx, पूर्णांक n);
#  अन्यथा
शून्य (*dll_lua_rawseti) (lua_State *L, पूर्णांक idx, lua_Integer n);
#  endif
पूर्णांक (*dll_lua_setmetatable) (lua_State *L, पूर्णांक objindex);
पूर्णांक (*dll_lua_next) (lua_State *L, पूर्णांक idx);
// libs
पूर्णांक (*dll_luaopen_base) (lua_State *L);
पूर्णांक (*dll_luaopen_table) (lua_State *L);
पूर्णांक (*dll_luaopen_string) (lua_State *L);
पूर्णांक (*dll_luaopen_math) (lua_State *L);
पूर्णांक (*dll_luaopen_io) (lua_State *L);
पूर्णांक (*dll_luaopen_os) (lua_State *L);
पूर्णांक (*dll_luaopen_package) (lua_State *L);
पूर्णांक (*dll_luaopen_debug) (lua_State *L);
#  जर LUA_VERSION_अंक >= 505
शून्य (*dll_luaL_openselectedlibs) (lua_State *L, पूर्णांक load, पूर्णांक preload);
#  अन्यथा
शून्य (*dll_luaL_openlibs) (lua_State *L);
#  endif

प्रकारव्याख्या शून्य **luaV_function;
प्रकारव्याख्या संरचना {
    स्थिरांक अक्षर *name;
    luaV_function func;
} luaV_Reg;

स्थिर स्थिरांक luaV_Reg luaV_dll[] = {
    // lauxlib
#  जर LUA_VERSION_अंक <= 501
    {"luaL_register", (luaV_function) &dll_luaL_register},
    {"luaL_prepbuffer", (luaV_function) &dll_luaL_prepbuffer},
    {"luaL_openlib", (luaV_function) &dll_luaL_openlib},
    {"luaL_typerror", (luaV_function) &dll_luaL_typerror},
    {"luaL_loadfile", (luaV_function) &dll_luaL_loadfile},
    {"luaL_loadbuffer", (luaV_function) &dll_luaL_loadbuffer},
#  अन्यथा
    {"luaL_prepbuffsize", (luaV_function) &dll_luaL_prepbuffsize},
    {"luaL_setfuncs", (luaV_function) &dll_luaL_setfuncs},
    {"luaL_loadfilex", (luaV_function) &dll_luaL_loadfilex},
    {"luaL_loadbufferx", (luaV_function) &dll_luaL_loadbufferx},
    {"luaL_argerror", (luaV_function) &dll_luaL_argerror},
#  endif
#  जर LUA_VERSION_अंक >= 504
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
#  जर LUA_VERSION_अंक <= 501
    {"lua_tonumber", (luaV_function) &dll_lua_tonumber},
    {"lua_tointeger", (luaV_function) &dll_lua_tointeger},
    {"lua_call", (luaV_function) &dll_lua_call},
    {"lua_pcall", (luaV_function) &dll_lua_pcall},
#  अन्यथा
    {"lua_tonumberx", (luaV_function) &dll_lua_tonumberx},
    {"lua_tointegerx", (luaV_function) &dll_lua_tointegerx},
    {"lua_callk", (luaV_function) &dll_lua_callk},
    {"lua_pcallk", (luaV_function) &dll_lua_pcallk},
    {"lua_getglobal", (luaV_function) &dll_lua_getglobal},
    {"lua_setglobal", (luaV_function) &dll_lua_setglobal},
#  endif
#  जर LUA_VERSION_अंक <= 502
    {"lua_replace", (luaV_function) &dll_lua_replace},
    {"lua_remove", (luaV_function) &dll_lua_remove},
#  endif
#  जर LUA_VERSION_अंक >= 503
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
#  जर LUA_VERSION_अंक >= 504
    {"lua_newuserdatauv", (luaV_function) &dll_lua_newuserdatauv},
#  अन्यथा
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
#  जर LUA_VERSION_अंक >= 505
    {"luaL_openselectedlibs", (luaV_function) &dll_luaL_openselectedlibs},
#  अन्यथा
    {"luaL_openlibs", (luaV_function) &dll_luaL_openlibs},
#  endif
    {रिक्त, रिक्त}
};

स्थिर HANDLE hinstLua = रिक्त;

    स्थिर पूर्णांक
lua_link_init(अक्षर *libname, पूर्णांक verbose)
{
    स्थिरांक luaV_Reg *reg;
    जर (hinstLua) परत OK;
    hinstLua = load_dll(libname);
    जर (!hinstLua)
    {
	जर (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	परत FAIL;
    }
    साठी (reg = luaV_dll; reg->func; reg++)
    {
	जर ((*reg->func = चिन्ह_from_dll(hinstLua, reg->name)) == रिक्त)
	{
	    close_dll(hinstLua);
	    hinstLua = 0;
	    जर (verbose)
		semsg(_(e_could_not_load_library_function_str), reg->name);
	    परत FAIL;
	}
    }
    परत OK;
}
# endif // DYNAMIC_LUA

# जर defined(DYNAMIC_LUA)
    पूर्णांक
lua_enabled(पूर्णांक verbose)
{
    परत lua_link_init((अक्षर *)p_luadll, verbose) == OK;
}
# endif

# जर LUA_VERSION_अंक > 501 && LUA_VERSION_अंक < 504
    स्थिर पूर्णांक
luaL_typeerror(lua_State *L, पूर्णांक narg, स्थिरांक अक्षर *tname)
{
    स्थिरांक अक्षर *संदेश = lua_pushfstring(L, "%s expected, got %s",
	    tname, luaL_typename(L, narg));
    परत luaL_argerror(L, narg, संदेश);
}
# endif

    स्थिर LUAV_INLINE शून्य
luaV_getudata(lua_State *L, शून्य *v)
{
    lua_pushlightuserdata(L, (शून्य *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // now the cache table is at the top of the stack
    lua_pushlightuserdata(L, v);
    lua_rawget(L, -2);
    lua_remove(L, -2);  // remove the cache table from the stack
}

    स्थिर LUAV_INLINE शून्य
luaV_setudata(lua_State *L, शून्य *v)
{
    lua_pushlightuserdata(L, (शून्य *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // cache table is at -1
    lua_pushlightuserdata(L, v);       // ...now at -2
    lua_pushvalue(L, -3);	       // copy the userdata (cache at -3)
    lua_rawset(L, -3);		       // consumes two stack items
    lua_काढा(L, 1);		       // and remove the cache table
}

// =======   Internal   =======

    स्थिर शून्य
luaV_newmetatable(lua_State *L, स्थिरांक अक्षर *tname)
{
    lua_newtable(L);
    lua_pushlightuserdata(L, (शून्य *) tname);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
}

    स्थिर शून्य *
luaV_toudata(lua_State *L, पूर्णांक ud, स्थिरांक अक्षर *tname)
{
    शून्य *p = lua_touserdata(L, ud);

    जर (p == रिक्त)
	परत रिक्त;

    // value is userdata
    जर (lua_getmetatable(L, ud)) // does it have a metatable?
    {
	luaV_getfield(L, tname); // get metatable
	जर (lua_rawequal(L, -1, -2)) // MTs match?
	{
	    lua_काढा(L, 2); // MTs
	    परत p;
	}
    }

    परत रिक्त;
}

    स्थिर शून्य *
luaV_checkcache(lua_State *L, शून्य *p)
{
    luaV_getudata(L, p);
    जर (lua_isnil(L, -1)) luaL_error(L, "invalid object");
    lua_काढा(L, 1);
    परत p;
}

# define luaV_unbox(L,luatyp,ud) (*((luatyp *) lua_touserdata((L),(ud))))

# define luaV_checkvalid(L,luatyp,ud) \
    luaV_checkcache((L), (शून्य *) luaV_unbox((L),luatyp,(ud)))

    स्थिर शून्य *
luaV_checkudata(lua_State *L, पूर्णांक ud, स्थिरांक अक्षर *tname)
{
    शून्य *p = luaV_toudata(L, ud, tname);
    जर (p == रिक्त) luaL_typeerror(L, ud, tname);
    परत p;
}

    स्थिर शून्य
luaV_pushtypval(lua_State *L, typval_T *tv)
{
    जर (tv == रिक्त)
    {
	lua_pushnil(L);
	परत;
    }
    निवड (tv->v_type)
    {
	स्थिती VAR_STRING:
	    lua_pushstring(L, tv->vval.v_string == रिक्त
					    ? "" : (अक्षर *)tv->vval.v_string);
	    थांबा;
	स्थिती VAR_संख्या:
	    lua_pushinteger(L, (पूर्णांक) tv->vval.v_संख्या);
	    थांबा;
	स्थिती VAR_FLOAT:
	    lua_pushnumber(L, (lua_संख्या) tv->vval.v_float);
	    थांबा;
	स्थिती VAR_LIST:
	    luaV_pushlist(L, tv->vval.v_list);
	    थांबा;
	स्थिती VAR_DICT:
	    luaV_pushdict(L, tv->vval.v_dict);
	    थांबा;
	स्थिती VAR_BOOL:
	स्थिती VAR_SPECIAL:
	    जर (tv->vval.v_संख्या <= VVAL_TRUE)
		lua_pushboolean(L, (पूर्णांक) tv->vval.v_संख्या);
	    अन्यथा
		lua_pushnil(L);
	    थांबा;
	स्थिती VAR_FUNC:
	    luaV_pushfuncref(L, tv->vval.v_string);
	    थांबा;
	स्थिती VAR_PARTIAL:
	    // TODO: handle partial arguments
	    luaV_pushfuncref(L, partial_name(tv->vval.v_partial));
	    थांबा;

	स्थिती VAR_BLOB:
	    luaV_pushblob(L, tv->vval.v_blob);
	    थांबा;
	पूर्वनिर्धारित:
	    lua_pushnil(L);
    }
}

/*
 * Converts lua value at 'pos' to typval 'tv'.
 * Returns OK or FAIL.
 */
    स्थिर पूर्णांक
luaV_totypval(lua_State *L, पूर्णांक pos, typval_T *tv)
{
    पूर्णांक status = OK;

    tv->v_lock = 0;

    निवड (lua_type(L, pos))
    {
	स्थिती LUA_TBOOLEAN:
	    tv->v_type = VAR_BOOL;
	    tv->vval.v_संख्या = (varnumber_T) lua_toboolean(L, pos);
	    थांबा;
	स्थिती LUA_TNIL:
	    tv->v_type = VAR_SPECIAL;
	    tv->vval.v_संख्या = VVAL_NULL;
	    थांबा;
	स्थिती LUA_TSTRING:
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strsave((char_u *) lua_tostring(L, pos));
	    थांबा;
	स्थिती LUA_TNUMBER:
	{
	    स्थिरांक lua_संख्या n = lua_tonumber(L, pos);

	    जर (n > (lua_संख्या)INT64_MAX || n < (lua_संख्या)INT64_MIN
		    || ((lua_संख्या)((varnumber_T)n)) != n)
	    {
		tv->v_type = VAR_FLOAT;
		tv->vval.v_float = (float_T)n;
	    }
	    अन्यथा
	    {
		tv->v_type = VAR_संख्या;
		tv->vval.v_संख्या = (varnumber_T)n;
	    }
	}
	    थांबा;
	स्थिती LUA_TFUNCTION:
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
	    थांबा;
	}
	स्थिती LUA_TTABLE:
	{
	    पूर्णांक lua_tableref;

	    lua_pushvalue(L, pos);
	    lua_tableref = luaL_ref(L, LUA_REGISTRYINDEX);
	    जर (lua_getmetatable(L, pos))
	    {
		lua_getfield(L, -1, LUA___CALL);
		जर (lua_isfunction(L, -1))
		{
		    char_u *name;
		    पूर्णांक lua_funcref = luaL_ref(L, LUA_REGISTRYINDEX);
		    luaV_CFuncState *state = ALLOC_CLEAR_ONE(luaV_CFuncState);

		    state->lua_funcref = lua_funcref;
		    state->L = L;
		    state->lua_tableref = lua_tableref;
		    name = register_cfunc(&luaV_call_lua_func,
					      &luaV_call_lua_func_free, state);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(name);
		    थांबा;
		}
	    }
	    tv->v_type = VAR_संख्या;
	    tv->vval.v_संख्या = 0;
	    status = FAIL;
	    थांबा;
	}
	स्थिती LUA_TUSERDATA:
	{
	    शून्य *p = lua_touserdata(L, pos);

	    जर (lua_getmetatable(L, pos)) // has metatable?
	    {
		// check list
		luaV_getfield(L, LUAVIM_LIST);
		जर (lua_rawequal(L, -1, -2))
		{
		    tv->v_type = VAR_LIST;
		    tv->vval.v_list = *((luaV_List *) p);
		    ++tv->vval.v_list->lv_refcount;
		    lua_काढा(L, 2); // MTs
		    थांबा;
		}
		// check dict
		luaV_getfield(L, LUAVIM_DICT);
		जर (lua_rawequal(L, -1, -3))
		{
		    tv->v_type = VAR_DICT;
		    tv->vval.v_dict = *((luaV_Dict *) p);
		    ++tv->vval.v_dict->dv_refcount;
		    lua_काढा(L, 3); // MTs
		    थांबा;
		}
		// check blob
		luaV_getfield(L, LUAVIM_BLOB);
		जर (lua_rawequal(L, -1, -4))
		{
		    tv->v_type = VAR_BLOB;
		    tv->vval.v_blob = *((luaV_Blob *) p);
		    ++tv->vval.v_blob->bv_refcount;
		    lua_काढा(L, 4); // MTs
		    थांबा;
		}
		// check funcref
		luaV_getfield(L, LUAVIM_FUNCREF);
		जर (lua_rawequal(L, -1, -5))
		{
		    luaV_Funcref *f = (luaV_Funcref *) p;

		    func_ref(f->name);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(f->name);
		    lua_काढा(L, 5); // MTs
		    थांबा;
		}
		lua_काढा(L, 4); // MTs
	    }
	}
	// FALLTHROUGH
	पूर्वनिर्धारित:
	    tv->v_type = VAR_संख्या;
	    tv->vval.v_संख्या = 0;
	    status = FAIL;
    }
    परत status;
}

/*
 * similar to luaL_addlstring, but replaces \0 with \n if toline and
 * \n with \0 otherwise
 */
    स्थिर शून्य
luaV_addlstring(luaL_Buffer *b, स्थिरांक अक्षर *s, माप_प्रकार l, पूर्णांक toline)
{
    पर्यंत (l--)
    {
	जर (*s == '\0' && toline)
	    luaL_addchar(b, '\n');
	अन्यथा जर (*s == '\n' && !toline)
	    luaL_addchar(b, '\0');
	अन्यथा
	    luaL_addchar(b, *s);
	s++;
    }
}

    स्थिर शून्य
luaV_pushline(lua_State *L, buf_T *buf, linenr_T n)
{
    स्थिरांक अक्षर *s = (स्थिरांक अक्षर *) ml_get_buf(buf, n, FALSE);
    luaL_Buffer b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, म_माप(s), 0);
    luaL_pushresult(&b);
}

    स्थिर char_u *
luaV_toline(lua_State *L, पूर्णांक pos)
{
    माप_प्रकार l;
    स्थिरांक अक्षर *s = lua_tolstring(L, pos, &l);

    luaL_Buffer b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 1);
    luaL_pushresult(&b);
    परत (char_u *) lua_tostring(L, -1);
}

/*
 * pops a string s from the top of the stack and calls mf(t) for pieces t of
 * s separated by newlines
 */
    स्थिर शून्य
luaV_msgfunc(lua_State *L, msgfunc_T mf)
{
    luaL_Buffer b;
    माप_प्रकार l;
    स्थिरांक अक्षर *p, *s = lua_tolstring(L, -1, &l);
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 0);
    luaL_pushresult(&b);
    // break string
    p = s = lua_tolstring(L, -1, &l);
    पर्यंत (l--)
    {
	जर (*p++ == '\0') // break?
	{
	    mf((अक्षर *)s);
	    s = p;
	}
    }
    mf((अक्षर *)s);
    lua_काढा(L, 2); // original and modified strings
}

# define luaV_newtype(typ,tname,luatyp,luatname) \
	स्थिर luatyp * \
    luaV_new##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = (luatyp *) lua_newuserdata(L, आकार(luatyp)); \
	*o = obj; \
	luaV_setudata(L, obj); /* cache[obj] = udata */ \
	luaV_getfield(L, luatname); \
	lua_setmetatable(L, -2); \
	परत o; \
    }

# define luaV_pushtype(typ,tname,luatyp) \
	स्थिर luatyp * \
    lua_V_ढकला##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = रिक्त; \
	जर (obj == रिक्त) \
	    lua_pushnil(L); \
	अन्यथा \
	{ \
	    luaV_getudata(L, obj); \
	    जर (lua_isnil(L, -1)) /* not interned? */ \
	    { \
		lua_काढा(L, 1); \
		o = luaV_new##tname(L, obj); \
	    } \
	    अन्यथा \
		o = (luatyp *) lua_touserdata(L, -1); \
	} \
	परत o; \
    }

# define luaV_type_tostring(tname,luatname) \
	स्थिर पूर्णांक \
    luaV_##tname##_tostring(lua_State *L) \
    { \
	lua_pushfstring(L, "%s: %p", luatname, lua_touserdata(L, 1)); \
	परत 1; \
    }

// =======   List type   =======

    स्थिर luaV_List *
luaV_newlist(lua_State *L, list_T *lis)
{
    luaV_List *l = (luaV_List *) lua_newuserdata(L, आकार(luaV_List));
    *l = lis;
    lis->lv_refcount++; // reference in Lua
    luaV_setudata(L, lis); // cache[lis] = udata
    luaV_getfield(L, LUAVIM_LIST);
    lua_setmetatable(L, -2);
    परत l;
}

luaV_pushtype(list_T, list, luaV_List)
luaV_type_tostring(list, LUAVIM_LIST)

    स्थिर पूर्णांक
luaV_list_len(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushinteger(L, (पूर्णांक) list_len(l));
    परत 1;
}

    स्थिर पूर्णांक
luaV_list_iter(lua_State *L)
{
    listitem_T *li = (listitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    जर (li == रिक्त) परत 0;
    luaV_pushtypval(L, &li->li_tv);
    lua_pushlightuserdata(L, (शून्य *) li->li_next);
    lua_replace(L, lua_upvalueindex(1));
    परत 1;
}

    स्थिर पूर्णांक
luaV_list_call(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushlightuserdata(L, (शून्य *) l->lv_first);
    lua_pushcclosure(L, luaV_list_iter, 1);
    परत 1;
}

    स्थिर पूर्णांक
luaV_list_index(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    जर (lua_isnumber(L, 2)) // list item?
    {
	दीर्घ n = (दीर्घ) luaL_checkinteger(L, 2);
	listitem_T *li;

	// Lua array index starts with 1 while Vim uses 0, subtract 1 to
	// normalize.
	n -= 1;
	li = list_find(l, n);
	जर (li == रिक्त)
	    lua_pushnil(L);
	अन्यथा
	    luaV_pushtypval(L, &li->li_tv);
    }
    अन्यथा जर (lua_isstring(L, 2)) // method?
    {
	स्थिरांक अक्षर *s = lua_tostring(L, 2);
	जर (म_भेदन(s, "add", 3) == 0
		|| म_भेदन(s, "insert", 6) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	अन्यथा
	    lua_pushnil(L);
    }
    अन्यथा
	lua_pushnil(L);
    परत 1;
}

    स्थिर पूर्णांक
luaV_list_newindex(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    दीर्घ n = (दीर्घ) luaL_checkinteger(L, 2);
    listitem_T *li;

    // Lua array index starts with 1 while Vim uses 0, subtract 1 to normalize.
    n -= 1;

    जर (l->lv_lock)
	luaL_error(L, "list is locked");
    li = list_find(l, n);
    जर (li == रिक्त)
    {
	जर (!lua_isnil(L, 3))
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "inserting list item");
	    जर (list_घाला_tv(l, &v, li) == FAIL)
		luaL_error(L, "failed to add item to list");
	    clear_tv(&v);
	}
    }
    अन्यथा
    {
	जर (lua_isnil(L, 3)) // remove?
	{
	    vimlist_remove(l, li, li);
	    listitem_free(l, li);
	}
	अन्यथा
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "setting list item");
	    clear_tv(&li->li_tv);
	    li->li_tv = v;
	}
    }
    परत 0;
}

    स्थिर पूर्णांक
luaV_list_add(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (शून्य *) *lis);
    typval_T v;
    जर (l->lv_lock)
	luaL_error(L, "list is locked");
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "adding list item");
    जर (list_append_tv(l, &v) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    परत 1;
}

    स्थिर पूर्णांक
lua_V_list_घाला(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (शून्य *) *lis);
    दीर्घ pos = (दीर्घ) luaL_optinteger(L, 3, 0);
    listitem_T *li = रिक्त;
    typval_T v;
    जर (l->lv_lock)
	luaL_error(L, "list is locked");
    जर (pos < l->lv_len)
    {
	li = list_find(l, pos);
	जर (li == रिक्त)
	    luaL_error(L, "invalid position");
    }
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "inserting list item");
    जर (list_घाला_tv(l, &v, li) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    परत 1;
}

स्थिर स्थिरांक luaL_Reg luaV_List_mt[] = {
    {"__tostring", luaV_list_tostring},
    {"__len", luaV_list_len},
    {"__call", luaV_list_call},
    {"__index", luaV_list_index},
    {"__newindex", luaV_list_newindex},
    {"add", luaV_list_add},
    {"insert", lua_V_list_घाला},
    {रिक्त, रिक्त}
};


// =======   Dict type   =======

    स्थिर luaV_Dict *
luaV_newdict(lua_State *L, dict_T *dic)
{
    luaV_Dict *d = (luaV_Dict *) lua_newuserdata(L, आकार(luaV_Dict));
    *d = dic;
    dic->dv_refcount++; // reference in Lua
    luaV_setudata(L, dic); // cache[dic] = udata
    luaV_getfield(L, LUAVIM_DICT);
    lua_setmetatable(L, -2);
    परत d;
}

luaV_pushtype(dict_T, dict, luaV_Dict)
luaV_type_tostring(dict, LUAVIM_DICT)

    स्थिर पूर्णांक
luaV_dict_len(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    lua_pushinteger(L, (पूर्णांक) dict_len(d));
    परत 1;
}

    स्थिर पूर्णांक
luaV_dict_iter(lua_State *L UNUSED)
{
# ifdef FEAT_EVAL
    hashitem_T *hi = (hashitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    पूर्णांक n = lua_tointeger(L, lua_upvalueindex(2));
    dictitem_T *di;
    जर (n <= 0) परत 0;
    पर्यंत (HASHITEM_EMPTY(hi)) hi++;
    di = dict_lookup(hi);
    lua_pushstring(L, (अक्षर *) hi->hi_किल्ली);
    luaV_pushtypval(L, &di->di_tv);
    lua_pushlightuserdata(L, (शून्य *) (hi + 1));
    lua_replace(L, lua_upvalueindex(1));
    lua_pushinteger(L, n - 1);
    lua_replace(L, lua_upvalueindex(2));
    परत 2;
# अन्यथा
    परत 0;
# endif
}

    स्थिर पूर्णांक
luaV_dict_call(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    hashtab_T *ht = &d->dv_hashtab;
    lua_pushlightuserdata(L, (शून्य *) ht->ht_array);
    lua_pushinteger(L, ht->ht_used); // # remaining items
    lua_pushcclosure(L, luaV_dict_iter, 2);
    परत 1;
}

    स्थिर पूर्णांक
luaV_dict_index(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *किल्ली = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di = dict_find(d, किल्ली, -1);

    जर (di == रिक्त)
    {
	lua_pushnil(L);
	परत 1;
    }

    luaV_pushtypval(L, &di->di_tv);
    जर (di->di_tv.v_type == VAR_FUNC) // funcref?
    {
	luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, -1);
	f->self = d; // keep "self" reference
	d->dv_refcount++;
    }

    परत 1;
}

    स्थिर पूर्णांक
luaV_dict_newindex(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *किल्ली = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di;
    typval_T tv;

    जर (d->dv_lock)
	luaL_error(L, "dict is locked");
    जर (किल्ली == रिक्त)
	परत 0;
    जर (*किल्ली == NUL)
	luaL_error(L, "empty key");
    जर (!lua_isnil(L, 3)) // read value?
    {
	luaV_checktypval(L, 3, &tv, "setting dict item");
	जर (d->dv_scope == VAR_DEF_SCOPE && tv.v_type == VAR_FUNC)
	{
	    clear_tv(&tv);
	    luaL_error(L, "cannot assign funcref to builtin scope");
	}
    }
    di = dict_find(d, किल्ली, -1);
    जर (di == रिक्त) // non-existing key?
    {
	जर (lua_isnil(L, 3))
	    परत 0;
	di = dictitem_alloc(किल्ली);
	जर (di == रिक्त)
	{
	    clear_tv(&tv);
	    परत 0;
	}
	जर (dict_add(d, di) == FAIL)
	{
	    vim_free(di);
	    clear_tv(&tv);
	    परत 0;
	}
    }
    अन्यथा
	clear_tv(&di->di_tv);
    जर (lua_isnil(L, 3)) // remove?
    {
	hashitem_T *hi = hash_find(&d->dv_hashtab, di->di_किल्ली);
	hash_remove(&d->dv_hashtab, hi, "Lua new index");
	dictitem_free(di);
    }
    अन्यथा
	di->di_tv = tv;
    परत 0;
}

स्थिर स्थिरांक luaL_Reg luaV_Dict_mt[] = {
    {"__tostring", luaV_dict_tostring},
    {"__len", luaV_dict_len},
    {"__call", luaV_dict_call},
    {"__index", luaV_dict_index},
    {"__newindex", luaV_dict_newindex},
    {रिक्त, रिक्त}
};


// =======   Blob type   =======

    स्थिर luaV_Blob *
luaV_newblob(lua_State *L, blob_T *blo)
{
    luaV_Blob *b = (luaV_Blob *) lua_newuserdata(L, आकार(luaV_Blob));
    *b = blo;
    blo->bv_refcount++; // reference in Lua
    luaV_setudata(L, blo); // cache[blo] = udata
    luaV_getfield(L, LUAVIM_BLOB);
    lua_setmetatable(L, -2);
    परत b;
}

luaV_pushtype(blob_T, blob, luaV_Blob)
luaV_type_tostring(blob, LUAVIM_BLOB)

    स्थिर पूर्णांक
luaV_blob_gc(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    blob_unref(b);
    परत 0;
}

    स्थिर पूर्णांक
luaV_blob_len(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    lua_pushinteger(L, (पूर्णांक) blob_len(b));
    परत 1;
}

    स्थिर पूर्णांक
luaV_blob_index(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    जर (lua_isnumber(L, 2))
    {
	पूर्णांक idx = luaL_checkinteger(L, 2);
	जर (idx < 0)
	    // Negative index is relative to the end.
	    idx += (पूर्णांक) blob_len(b);
	जर (idx >= 0 && idx < blob_len(b))
	    lua_pushnumber(L, (lua_संख्या) blob_get(b, idx));
	अन्यथा
	    lua_pushnil(L);
    }
    अन्यथा जर (lua_isstring(L, 2))
    {
	स्थिरांक अक्षर *s = lua_tostring(L, 2);
	जर (म_भेदन(s, "add", 3) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	अन्यथा
	    lua_pushnil(L);
    }
    अन्यथा
	lua_pushnil(L);
    परत 1;
}

    स्थिर पूर्णांक
luaV_blob_newindex(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    जर (b->bv_lock)
	luaL_error(L, "blob is locked");

    जर (!lua_isnumber(L, 2))
	परत 0;

    दीर्घ len = blob_len(b);
    पूर्णांक idx = luaL_checkinteger(L, 2);
    पूर्णांक val = luaL_checkinteger(L, 3);
    जर (idx < 0)
	// Negative index is relative to the end.
	idx += (पूर्णांक) len;
    जर ((idx >= 0 && idx < len)
	    || (idx == len && ga_grow(&b->bv_ga, 1) == OK))
    {
	blob_set(b, idx, (char_u) val);
	जर (idx == len)
	    ++b->bv_ga.ga_len;
    }
    अन्यथा
	luaL_error(L, "index out of range");

    परत 0;
}

    स्थिर पूर्णांक
luaV_blob_add(lua_State *L)
{
    luaV_Blob *blo = luaV_checkudata(L, 1, LUAVIM_BLOB);
    blob_T *b = (blob_T *) luaV_checkcache(L, (शून्य *) *blo);
    जर (b->bv_lock)
	luaL_error(L, "blob is locked");
    lua_settop(L, 2);
    जर (!lua_isstring(L, 2))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 2));
    अन्यथा
    {
	माप_प्रकार i, l = 0;
	स्थिरांक अक्षर *s = lua_tolstring(L, 2, &l);

	जर (ga_grow(&b->bv_ga, (पूर्णांक)l) == OK)
	    साठी (i = 0; i < l; ++i)
		ga_append(&b->bv_ga, s[i]);
    }
    lua_settop(L, 1);
    परत 1;
}

स्थिर स्थिरांक luaL_Reg luaV_Blob_mt[] = {
    {"__tostring", luaV_blob_tostring},
    {"__gc", luaV_blob_gc},
    {"__len", luaV_blob_len},
    {"__index", luaV_blob_index},
    {"__newindex", luaV_blob_newindex},
    {"add", luaV_blob_add},
    {रिक्त, रिक्त}
};


// =======   Funcref type   =======

    स्थिर luaV_Funcref *
luaV_newfuncref(lua_State *L, char_u *name)
{
    luaV_Funcref *f = (luaV_Funcref *)lua_newuserdata(L, आकार(luaV_Funcref));

    जर (name != रिक्त)
    {
	func_ref(name);
	f->name = vim_strsave(name);
    }
    f->self = रिक्त;
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_setmetatable(L, -2);
    परत f;
}

    स्थिर luaV_Funcref *
luaV_pushfuncref(lua_State *L, char_u *name)
{
    परत luaV_newfuncref(L, name);
}


luaV_type_tostring(funcref, LUAVIM_FUNCREF)

    स्थिर पूर्णांक
luaV_funcref_gc(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    func_unref(f->name);
    vim_free(f->name);
    // NOTE: Don't call "dict_unref(f->self)", because the dict of "f->self"
    // will be (or has been already) freed by Vim's garbage collection.
    परत 0;
}

// equivalent to string(funcref)
    स्थिर पूर्णांक
luaV_funcref_len(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    lua_pushstring(L, (स्थिरांक अक्षर *) f->name);
    परत 1;
}

    स्थिर पूर्णांक
luaV_funcref_call(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);
    पूर्णांक i, n = lua_gettop(L) - 1; // #args
    पूर्णांक status = FAIL;
    typval_T args;
    typval_T rettv;

    args.v_type = VAR_LIST;
    args.vval.v_list = list_alloc();
    rettv.v_type = VAR_UNKNOWN; // as in clear_tv
    जर (args.vval.v_list != रिक्त)
    {
	typval_T v;

	साठी (i = 0; i < n; i++)
	{
	    luaV_checktypval(L, i + 2, &v, "calling funcref");
	    list_append_tv(args.vval.v_list, &v);
	    clear_tv(&v);
	}
	status = func_call(f->name, &args, रिक्त, f->self, &rettv);
	जर (status == OK)
	    luaV_pushtypval(L, &rettv);
	clear_tv(&args);
	clear_tv(&rettv);
    }
    जर (status != OK)
	luaL_error(L, "cannot call funcref");
    परत 1;
}

स्थिर स्थिरांक luaL_Reg luaV_Funcref_mt[] = {
    {"__tostring", luaV_funcref_tostring},
    {"__gc", luaV_funcref_gc},
    {"__len", luaV_funcref_len},
    {"__call", luaV_funcref_call},
    {रिक्त, रिक्त}
};


// =======   Buffer type   =======

luaV_newtype(buf_T, buffer, luaV_Buffer, LUAVIM_BUFFER)
luaV_pushtype(buf_T, buffer, luaV_Buffer)
luaV_type_tostring(buffer, LUAVIM_BUFFER)

    स्थिर पूर्णांक
luaV_buffer_len(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    lua_pushinteger(L, b->b_ml.ml_ओळ_मोजणी);
    परत 1;
}

    स्थिर पूर्णांक
luaV_buffer_call(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    lua_settop(L, 1);
    set_curbuf(b, DOBUF_SPLIT);
    परत 1;
}

    स्थिर पूर्णांक
luaV_buffer_index(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    linenr_T n = (linenr_T) lua_tointeger(L, 2);
    जर (n > 0 && n <= b->b_ml.ml_ओळ_मोजणी)
	luaV_pushline(L, b, n);
    अन्यथा जर (lua_isstring(L, 2))
    {
	स्थिरांक अक्षर *s = lua_tostring(L, 2);
	जर (म_भेदन(s, "name", 4) == 0)
	    lua_pushstring(L, (b->b_sfname == रिक्त)
					? "" : (अक्षर *) b->b_sfname);
	अन्यथा जर (म_भेदन(s, "fname", 5) == 0)
	    lua_pushstring(L, (b->b_ffname == रिक्त)
					? "" : (अक्षर *) b->b_ffname);
	अन्यथा जर (म_भेदन(s, "number", 6) == 0)
	    lua_pushinteger(L, b->b_fnum);
	// methods
	अन्यथा जर (म_भेदन(s,   "insert", 6) == 0
		|| म_भेदन(s, "next", 4) == 0
		|| म_भेदन(s, "previous", 8) == 0
		|| म_भेदन(s, "isvalid", 7) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	अन्यथा
	    lua_pushnil(L);
    }
    अन्यथा
	lua_pushnil(L);
    परत 1;
}

    स्थिर पूर्णांक
luaV_buffer_newindex(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    linenr_T n = (linenr_T) luaL_checkinteger(L, 2);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    जर (n < 1 || n > b->b_ml.ml_ओळ_मोजणी)
	luaL_error(L, "invalid line number");
    जर (lua_isnil(L, 3)) // delete line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	जर (u_savedel(n, 1L) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	अन्यथा जर (ml_delete(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot delete line");
	}
	अन्यथा
	{
	    deleted_ओळी_mark(n, 1L);
	    जर (b == curwin->w_buffer) // fix cursor in current window?
	    {
		जर (curwin->w_cursor.lnum >= n)
		{
		    जर (curwin->w_cursor.lnum > n)
		    {
			curwin->w_cursor.lnum -= 1;
			check_cursor_col();
		    }
		    अन्यथा
			check_cursor();
		    changed_cline_bef_curs();
		}
		invalidate_botline();
	    }
	}
	curbuf = buf;
    }
    अन्यथा जर (lua_isstring(L, 3)) // update line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	जर (u_savesub(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	अन्यथा जर (ml_replace(n, luaV_toline(L, 3), TRUE) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot replace line");
	}
	अन्यथा
	    changed_bytes(n, 0);
	curbuf = buf;
	जर (b == curwin->w_buffer)
	    check_cursor_col();
    }
    अन्यथा
	luaL_error(L, "wrong argument to change line");
    परत 0;
}

    स्थिर पूर्णांक
lua_V_buffer_घाला(lua_State *L)
{
    luaV_Buffer *lb = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *b = (buf_T *) luaV_checkcache(L, (शून्य *) *lb);
    linenr_T last = b->b_ml.ml_ओळ_मोजणी;
    linenr_T n = (linenr_T) luaL_optinteger(L, 3, last);
    buf_T *buf;
    luaL_checktype(L, 2, LUA_TSTRING);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    // fix insertion line
    जर (n < 0) n = 0;
    जर (n > last) n = last;
    // insert
    buf = curbuf;
    curbuf = b;
    जर (u_save(n, n + 1) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot save undo information");
    }
    अन्यथा जर (ml_append(n, luaV_toline(L, 2), 0, FALSE) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot insert line");
    }
    अन्यथा
	appended_ओळी_mark(n, 1L);
    curbuf = buf;
    update_screen(UPD_VALID);
    परत 0;
}

    स्थिर पूर्णांक
luaV_buffer_next(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (शून्य *) *b);
    luaV_pushbuffer(L, buf->b_next);
    परत 1;
}

    स्थिर पूर्णांक
luaV_buffer_previous(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (शून्य *) *b);
    luaV_pushbuffer(L, buf->b_prev);
    परत 1;
}

    स्थिर पूर्णांक
luaV_buffer_isvalid(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    luaV_getudata(L, *b);
    lua_pushboolean(L, !lua_isnil(L, -1));
    परत 1;
}

स्थिर स्थिरांक luaL_Reg luaV_Buffer_mt[] = {
    {"__tostring", luaV_buffer_tostring},
    {"__len", luaV_buffer_len},
    {"__call", luaV_buffer_call},
    {"__index", luaV_buffer_index},
    {"__newindex", luaV_buffer_newindex},
    {"insert", lua_V_buffer_घाला},
    {"next", luaV_buffer_next},
    {"previous", luaV_buffer_previous},
    {"isvalid", luaV_buffer_isvalid},
    {रिक्त, रिक्त}
};


// =======   Window type   =======

luaV_newtype(win_T, window, luaV_Window, LUAVIM_WINDOW)
luaV_pushtype(win_T, window, luaV_Window)
luaV_type_tostring(window, LUAVIM_WINDOW)

    स्थिर पूर्णांक
luaV_window_call(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    lua_settop(L, 1);
    win_goto(w);
    परत 1;
}

    स्थिर पूर्णांक
luaV_window_index(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    स्थिरांक अक्षर *s = luaL_checkstring(L, 2);
    जर (म_भेदन(s, "buffer", 6) == 0)
	luaV_pushbuffer(L, w->w_buffer);
    अन्यथा जर (म_भेदन(s, "line", 4) == 0)
	lua_pushinteger(L, w->w_cursor.lnum);
    अन्यथा जर (म_भेदन(s, "col", 3) == 0)
	lua_pushinteger(L, w->w_cursor.col + 1);
    अन्यथा जर (म_भेदन(s, "width", 5) == 0)
	lua_pushinteger(L, w->w_width);
    अन्यथा जर (म_भेदन(s, "height", 6) == 0)
	lua_pushinteger(L, w->w_height);
    // methods
    अन्यथा जर (म_भेदन(s,   "next", 4) == 0
	    || म_भेदन(s, "previous", 8) == 0
	    || म_भेदन(s, "isvalid", 7) == 0)
    {
	lua_getmetatable(L, 1);
	lua_getfield(L, -1, s);
    }
    अन्यथा
	lua_pushnil(L);
    परत 1;
}

    स्थिर पूर्णांक
luaV_window_newindex(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    स्थिरांक अक्षर *s = luaL_checkstring(L, 2);
    पूर्णांक v = luaL_checkinteger(L, 3);
    जर (म_भेदन(s, "line", 4) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	जर (v < 1 || v > w->w_buffer->b_ml.ml_ओळ_मोजणी)
	    luaL_error(L, "line out of range");
	w->w_cursor.lnum = v;
	update_screen(UPD_VALID);
    }
    अन्यथा जर (म_भेदन(s, "col", 3) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	w->w_cursor.col = v - 1;
	w->w_set_curswant = true;
	update_screen(UPD_VALID);
    }
    अन्यथा जर (म_भेदन(s, "width", 5) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setwidth(v);
	curwin = win;
    }
    अन्यथा जर (म_भेदन(s, "height", 6) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setheight(v);
	curwin = win;
    }
    अन्यथा
	luaL_error(L, "invalid window property: `%s'", s);
    परत 0;
}

    स्थिर पूर्णांक
luaV_window_next(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    win_T *win = (win_T *) luaV_checkcache(L, (शून्य *) *w);
    luaV_pushwindow(L, win->w_next);
    परत 1;
}

    स्थिर पूर्णांक
luaV_window_previous(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    win_T *win = (win_T *) luaV_checkcache(L, (शून्य *) *w);
    luaV_pushwindow(L, win->w_prev);
    परत 1;
}

    स्थिर पूर्णांक
luaV_window_isvalid(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    luaV_getudata(L, *w);
    lua_pushboolean(L, !lua_isnil(L, -1));
    परत 1;
}

स्थिर स्थिरांक luaL_Reg luaV_Window_mt[] = {
    {"__tostring", luaV_window_tostring},
    {"__call", luaV_window_call},
    {"__index", luaV_window_index},
    {"__newindex", luaV_window_newindex},
    {"next", luaV_window_next},
    {"previous", luaV_window_previous},
    {"isvalid", luaV_window_isvalid},
    {रिक्त, रिक्त}
};


// =======   Vim module   =======

    स्थिर पूर्णांक
luaV_print(lua_State *L)
{
    पूर्णांक		i, n = lua_gettop(L); // nargs
    स्थिरांक अक्षर	*s;
    माप_प्रकार	l;
    garray_T	संदेश_ga;

    ga_init2(&संदेश_ga, 1, 128);
    lua_getglobal(L, "tostring");
    साठी (i = 1; i <= n; i++)
    {
	lua_pushvalue(L, -1); // tostring
	lua_pushvalue(L, i); // arg
	lua_call(L, 1, 1);
	s = lua_tolstring(L, -1, &l);
	जर (s == रिक्त)
	    परत luaL_error(L, "cannot convert to string");
	जर (i > 1)
	    ga_append(&संदेश_ga, ' '); // use space instead of tab
	ga_concat_len(&संदेश_ga, (char_u *)s, l);
	lua_काढा(L, 1);
    }
    // Replace any "\n" with "\0"
    साठी (i = 0; i < संदेश_ga.ga_len; i++)
	जर (((अक्षर *)संदेश_ga.ga_data)[i] == '\n')
	    ((अक्षर *)संदेश_ga.ga_data)[i] = '\0';
    lua_pushlstring(L, संदेश_ga.ga_data, संदेश_ga.ga_len);
    जर (!got_int)
	lua_V_संदेश(L);

    ga_clear(&संदेश_ga);
    परत 0;
}

    स्थिर पूर्णांक
luaV_debug(lua_State *L)
{
    lua_settop(L, 0);
    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "eval");
    lua_remove(L, -2); // vim.eval at position 1
    साठी (;;)
    {
	स्थिरांक अक्षर *आदान;
	माप_प्रकार l;
	lua_pushvalue(L, 1); // vim.eval
	lua_pushliteral(L, "input('lua_debug> ')");
	lua_call(L, 1, 1); // return string
	आदान = lua_tolstring(L, -1, &l);
	जर (l == 0 || म_भेद(आदान, "cont") == 0)
	    परत 0;
	संदेश_putchar('\n'); // avoid outputting on input line
	जर (luaL_loadbuffer(L, आदान, l, "=(debug command)")
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
	lua_settop(L, 1); // remove eventual returns, but keep vim.eval
    }
}

    स्थिर dict_T *
luaV_get_var_scope(lua_State *L)
{
    स्थिरांक अक्षर	*scope = luaL_checkstring(L, 1);
    dict_T	*dict = रिक्त;

    जर (STRICMP((अक्षर *)scope, "g") == 0)
	dict = get_globvar_dict();
    अन्यथा जर (STRICMP((अक्षर *)scope, "v") == 0)
	dict = get_vimvar_dict();
    अन्यथा जर (STRICMP((अक्षर *)scope, "b") == 0)
	dict = curbuf->b_vars;
    अन्यथा जर (STRICMP((अक्षर *)scope, "w") == 0)
	dict = curwin->w_vars;
    अन्यथा जर (STRICMP((अक्षर *)scope, "t") == 0)
	dict = curtab->tp_vars;
    अन्यथा
    {
	luaL_error(L, "invalid scope %s", scope);
	परत रिक्त;
    }

    परत dict;
}

    स्थिर पूर्णांक
luaV_setvar(lua_State *L)
{
    dict_T	*dict;
    dictitem_T	*di;
    माप_प्रकार	len;
    अक्षर	*name;
    पूर्णांक		del;
    अक्षर	*error = रिक्त;

    name = (अक्षर *)luaL_checklstring(L, 3, &len);
    del = (lua_gettop(L) < 4) || lua_isnil(L, 4);

    dict = luaV_get_var_scope(L);
    जर (dict == रिक्त)
	परत 0;

    di = dict_find(dict, (char_u *)name, (पूर्णांक)len);
    जर (di != रिक्त)
    {
	जर (di->di_flags & DI_FLAGS_RO)
	    error = "variable is read-only";
	अन्यथा जर (di->di_flags & DI_FLAGS_LOCK)
	    error = "variable is locked";
	अन्यथा जर (del && di->di_flags & DI_FLAGS_FIX)
	    error = "variable is fixed";
	जर (error != रिक्त)
	    परत luaL_error(L, error);
    }
    अन्यथा जर (dict->dv_lock)
	परत luaL_error(L, "Dictionary is locked");

    जर (del)
    {
	// Delete the key
	जर (di == रिक्त)
	    // Doesn't exist, nothing to do
	    परत 0;
	// Delete the entry
	dictitem_remove(dict, di, "Lua delete variable");
    }
    अन्यथा
    {
	// Update the key
	typval_T	tv;

	// Convert the lua value to a Vim script type in the temporary variable
	lua_pushvalue(L, 4);
	जर (luaV_totypval(L, -1, &tv) == FAIL)
	    परत luaL_error(L, "Couldn't convert lua value");

	जर (di == रिक्त)
	{
	    // Need to create an entry
	    di = dictitem_alloc((char_u *)name);
	    जर (di == रिक्त)
	    {
		clear_tv(&tv);
		परत 0;
	    }
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	    जर (dict_add(dict, di) == FAIL)
	    {
		dictitem_free(di);
		clear_tv(&tv);
		परत luaL_error(L, "Couldn't add to dictionary");
	    }
	}
	अन्यथा
	{
	    पूर्णांक type_error = FALSE;
	    जर (dict == get_vimvar_dict()
	       && !before_set_vvar((char_u *)name, di, &tv, TRUE, &type_error))
	    {
		clear_tv(&tv);
		जर (type_error)
		    परत luaL_error(L,
				"Setting v:%s to value with wrong type", name);
		परत 0;
	    }
	    // Clear the old value
	    clear_tv(&di->di_tv);
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	}

	// Clear the temporary variable
	clear_tv(&tv);
    }

    परत 0;
}

    स्थिर पूर्णांक
luaV_getvar(lua_State *L)
{
    dict_T	*dict = luaV_get_var_scope(L);
    माप_प्रकार	len;
    स्थिरांक अक्षर	*name = luaL_checklstring(L, 3, &len);
    dictitem_T	*di = dict_find(dict, (char_u *)name, (पूर्णांक)len);

    जर (di == रिक्त)
	परत 0;  // nil

    luaV_pushtypval(L, &di->di_tv);
    परत 1;
}

    स्थिर पूर्णांक
luaV_command(lua_State *L)
{
    char_u  *s = vim_strsave((char_u *)luaL_checkstring(L, 1));

    execute_cmds_from_string(s);
    vim_free(s);
    update_screen(UPD_VALID);
    परत 0;
}

    स्थिर पूर्णांक
luaV_eval(lua_State *L)
{
    typval_T *tv = eval_अभिव्यक्ती((char_u *) luaL_checkstring(L, 1), रिक्त);

    जर (tv == रिक्त) luaL_error(L, "invalid expression");
    luaV_pushtypval(L, tv);
    free_tv(tv);
    परत 1;
}

    स्थिर पूर्णांक
luaV_beep(lua_State *L UNUSED)
{
    vim_beep(BO_LANG);
    परत 0;
}

    स्थिर पूर्णांक
lua_V_ओळ(lua_State *L)
{
    luaV_pushline(L, curbuf, curwin->w_cursor.lnum);
    परत 1;
}

    स्थिर पूर्णांक
luaV_list(lua_State *L)
{
    list_T *l;
    पूर्णांक initarg = !lua_isnoneornil(L, 1);

    जर (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    l = list_alloc();
    जर (l == रिक्त)
    {
	lua_pushnil(L);
	परत 1;
    }

    luaV_newlist(L, l);
    जर (!initarg)
	परत 1;

    // traverse table to init list
    पूर्णांक notnil, i = 0;
    typval_T v;
    करा
    {
	lua_rawgeti(L, 1, ++i);
	notnil = !lua_isnil(L, -1);
	जर (notnil)
	{
	    luaV_checktypval(L, -1, &v, "vim.list");
	    list_append_tv(l, &v);
	    clear_tv(&v);
	}
	lua_काढा(L, 1); //मूल्य
    } पर्यंत (notnil);

    परत 1;
}

    स्थिर पूर्णांक
luaV_dict(lua_State *L)
{
    dict_T *d;
    पूर्णांक initarg = !lua_isnoneornil(L, 1);

    जर (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    d = dict_alloc();
    जर (d == रिक्त)
    {
	lua_pushnil(L);
	परत 1;
    }

    luaV_newdict(L, d);
    जर (!initarg)
	परत 1;

    // traverse table to init dict
    lua_pushnil(L);
    पर्यंत (lua_next(L, 1))
    {
	char_u *किल्ली;
	dictitem_T *di;
	typval_T v;

	lua_pushvalue(L, -2); // dup key in case it's a number
	किल्ली = (char_u *) lua_tostring(L, -1);
	जर (किल्ली == रिक्त)
	{
	    lua_pushnil(L);
	    परत 1;
	}
	जर (*किल्ली == NUL)
	    luaL_error(L, "table has empty key");
	luaV_checktypval(L, -2, &v, "vim.dict"); //मूल्य
	di = dictitem_alloc(किल्ली);
	जर (di == रिक्त || dict_add(d, di) == FAIL)
	{
	    vim_free(di);
	    lua_pushnil(L);
	    परत 1;
	}
	di->di_tv = v;
	lua_काढा(L, 2); // key copy and value
    }

    परत 1;
}

    स्थिर पूर्णांक
luaV_blob(lua_State *L)
{
    blob_T *b;
    पूर्णांक initarg = !lua_isnoneornil(L, 1);

    जर (initarg && !lua_isstring(L, 1))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 1));

    b = blob_alloc();
    जर (b == रिक्त)
    {
	lua_pushnil(L);
	परत 1;
    }

    luaV_newblob(L, b);
    जर (!initarg)
	परत 1;

    // traverse table to init blob
    माप_प्रकार i, l = 0;
    स्थिरांक अक्षर *s = lua_tolstring(L, 1, &l);

    जर (ga_grow(&b->bv_ga, (पूर्णांक)l) == OK)
	साठी (i = 0; i < l; ++i)
	    ga_append(&b->bv_ga, s[i]);

    परत 1;
}

    स्थिर पूर्णांक
luaV_funcref(lua_State *L)
{
    स्थिरांक अक्षर *name = luaL_checkstring(L, 1);
    // note: not checking if function exists (needs function_exists)
    जर (name == रिक्त || *name == NUL || VIM_ISDIGIT(*name))
	luaL_error(L, "invalid function name: %s", name);
    luaV_newfuncref(L, (char_u *) name);
    परत 1;
}

    स्थिर पूर्णांक
luaV_buffer(lua_State *L)
{
    buf_T *buf;
    जर (lua_isstring(L, 1)) // get by number or name?
    {
	जर (lua_isnumber(L, 1)) // by number?
	{
	    पूर्णांक n = lua_tointeger(L, 1);
	    FOR_ALL_BUFFERS(buf)
		जर (buf->b_fnum == n) थांबा;
	}
	अन्यथा // by name
	{
	    माप_प्रकार l;
	    स्थिरांक अक्षर *s = lua_tolstring(L, 1, &l);
	    FOR_ALL_BUFFERS(buf)
	    {
		जर (buf->b_ffname == रिक्त || buf->b_sfname == रिक्त)
		{
		    जर (l == 0) थांबा;
		}
		अन्यथा जर (म_भेदन(s, (अक्षर *)buf->b_ffname, l) == 0
			|| म_भेदन(s, (अक्षर *)buf->b_sfname, l) == 0)
		    थांबा;
	    }
	}
    }
    अन्यथा
	buf = (lua_toboolean(L, 1)) ? firstbuf : curbuf; // first buffer?
    luaV_pushbuffer(L, buf);
    परत 1;
}

    स्थिर पूर्णांक
luaV_window(lua_State *L)
{
    win_T *win;
    जर (lua_isnumber(L, 1)) // get by number?
    {
	पूर्णांक n = lua_tointeger(L, 1);
	साठी (win = firstwin; win != रिक्त; win = win->w_next, n--)
	    जर (n == 1) थांबा;
    }
    अन्यथा
	win = (lua_toboolean(L, 1)) ? firstwin : curwin; // first window?
    luaV_pushwindow(L, win);
    परत 1;
}

    स्थिर पूर्णांक
luaV_open(lua_State *L)
{
    char_u *s = रिक्त;
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    जर (lua_isstring(L, 1)) s = (char_u *) lua_tostring(L, 1);
    luaV_pushbuffer(L, buflist_new(s, रिक्त, 1L, BLN_LISTED));
    परत 1;
}

    स्थिर पूर्णांक
luaV_type(lua_State *L)
{
    luaL_checkany(L, 1);
    जर (lua_type(L, 1) == LUA_TUSERDATA) // check vim udata?
    {
	lua_settop(L, 1);
	जर (lua_getmetatable(L, 1))
	{
	    luaV_getfield(L, LUAVIM_LIST);
	    जर (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "list");
		परत 1;
	    }
	    luaV_getfield(L, LUAVIM_DICT);
	    जर (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "dict");
		परत 1;
	    }
	    luaV_getfield(L, LUAVIM_BLOB);
	    जर (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "blob");
		परत 1;
	    }
	    luaV_getfield(L, LUAVIM_FUNCREF);
	    जर (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "funcref");
		परत 1;
	    }
	    luaV_getfield(L, LUAVIM_BUFFER);
	    जर (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "buffer");
		परत 1;
	    }
	    luaV_getfield(L, LUAVIM_WINDOW);
	    जर (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "window");
		परत 1;
	    }
	}
    }
    lua_pushstring(L, luaL_typename(L, 1)); // fallback
    परत 1;
}

    स्थिर पूर्णांक
luaV_call(lua_State *L)
{
    पूर्णांक		argc = lua_gettop(L) - 1;
    माप_प्रकार	funcname_len;
    char_u	*funcname;
    अक्षर	*error = रिक्त;
    typval_T	rettv;
    typval_T	argv[MAX_FUNC_ARGS + 1];
    पूर्णांक		i = 0;

    जर (argc > MAX_FUNC_ARGS)
	परत luaL_error(L, "Function called with too many arguments");

    funcname = (char_u *)luaL_checklstring(L, 1, &funcname_len);

    साठी (; i < argc; i++)
    {
	जर (luaV_totypval(L, i + 2, &argv[i]) == FAIL)
	{
	    error = "lua: cannot convert value";
	    जा free_vim_args;
	}
    }

    argv[argc].v_type = VAR_UNKNOWN;

    जर (call_vim_function(funcname, argc, argv, &rettv) == FAIL)
    {
	error = "lua: call_vim_function failed";
	जा free_vim_args;
    }

    luaV_pushtypval(L, &rettv);
    clear_tv(&rettv);

free_vim_args:
    पर्यंत (i > 0)
	clear_tv(&argv[--i]);

    जर (error == रिक्त)
	परत 1;
    अन्यथा
	परत luaL_error(L, error);
}

/*
 * Return the Vim version as a Lua table
 */
    स्थिर पूर्णांक
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
    परत 1;
}

स्थिर स्थिरांक luaL_Reg luaV_module[] = {
    {"command", luaV_command},
    {"eval", luaV_eval},
    {"beep", luaV_beep},
    {"line", lua_V_ओळ},
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
    {"lua_version", रिक्त},
    {रिक्त, रिक्त}
};

/*
 * for freeing list, dict, buffer and window objects; lightuserdata as arg
 */
    स्थिर पूर्णांक
luaV_free(lua_State *L)
{
    lua_pushnil(L);
    luaV_setudata(L, lua_touserdata(L, 1));
    परत 0;
}

    स्थिर पूर्णांक
luaV_luaeval(lua_State *L)
{
    luaL_Buffer b;
    माप_प्रकार l;
    स्थिरांक अक्षर *str = lua_tolstring(L, 1, &l);
    typval_T *arg = (typval_T *) lua_touserdata(L, 2);
    typval_T *rettv = (typval_T *) lua_touserdata(L, 3);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, LUAVIM_EVALHEADER, आकार(LUAVIM_EVALHEADER) - 1);
    luaL_addlstring(&b, str, l);
    luaL_pushresult(&b);
    str = lua_tolstring(L, -1, &l);
    जर (luaL_loadbuffer(L, str, l, LUAVIM_EVALNAME)) // compile error?
    {
	luaV_emsg(L);
	परत 0;
    }
    luaV_pushtypval(L, arg);
    जर (lua_pcall(L, 1, 1, 0)) // running error?
    {
	luaV_emsg(L);
	परत 0;
    }
    जर (luaV_totypval(L, -1, rettv) == FAIL)
	emsg("luaeval: cannot convert value");
    परत 0;
}

    स्थिर पूर्णांक
luaV_setref(lua_State *L)
{
    पूर्णांक copyID = lua_tointeger(L, 1);
    पूर्णांक पात = FALSE;

    lua_pushlightuserdata(L, (शून्य *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // the cache table

    luaV_getfield(L, LUAVIM_LIST);
    luaV_getfield(L, LUAVIM_DICT);
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_pushnil(L);
    // traverse cache table
    पर्यंत (!पात && lua_next(L, 2) != 0)
    {
	lua_getmetatable(L, -1);
	जर (lua_rawequal(L, -1, 3)) // list?
	{
	    list_T *l = (list_T *)lua_touserdata(L, 6); //किल्ली

	    पात = set_ref_in_list(l, copyID);
	}
	अन्यथा जर (lua_rawequal(L, -1, 4)) // dict?
	{
	    dict_T *d = (dict_T *)lua_touserdata(L, 6); //किल्ली

	    पात = set_ref_in_dict(d, copyID);
	}
	अन्यथा जर (lua_rawequal(L, -1, 5)) // funcref?
	{
	    luaV_Funcref *f = (luaV_Funcref *)lua_touserdata(L, 6); //किल्ली

	    पात = set_ref_in_dict(f->self, copyID);
	}
	lua_काढा(L, 2); // metatable and value
    }
    lua_pushinteger(L, पात);
    परत 1;
}

    स्थिर पूर्णांक
luaV_pushversion(lua_State *L)
{
    पूर्णांक major = 0;
    पूर्णांक minor = 0;
    पूर्णांक patch = 0;
    अक्षर s[16];

    माला_पूछो(LUAVIM_VERSION, "Lua %d.%d.%d", &major, &minor, &patch);
    vim_snprintf(s, आकार(s), "%d.%d.%d", major, minor, patch);
    lua_pushstring(L, s);
    परत 0;
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

    स्थिर पूर्णांक
luaopen_vim(lua_State *L)
{
    lua_newtable(L);  // cache table
    lua_newtable(L);  // cache table's metatable
    lua_pushstring(L, "v");
    lua_setfield(L, -2, "__mode");
    lua_setmetatable(L, -2); // cache is weak-valued
    // put the cache table in the registry for luaV_get/setudata()
    lua_pushlightuserdata(L, (शून्य *) LUAVIM_UDATA_CACHE);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
    lua_काढा(L, 1);  // we don't need the cache table here anymore
    // print
    lua_pushcfunction(L, luaV_print);
    lua_setglobal(L, "print");
    // debug.debug
    lua_getglobal(L, "debug");
    lua_pushcfunction(L, luaV_debug);
    lua_setfield(L, -2, "debug");
    lua_काढा(L, 1);
    // free
    lua_pushlightuserdata(L, (शून्य *) LUAVIM_FREE);
    lua_pushcfunction(L, luaV_free);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // luaeval
    lua_pushlightuserdata(L, (शून्य *) LUAVIM_LUAEVAL);
    lua_pushcfunction(L, luaV_luaeval);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // setref
    lua_pushlightuserdata(L, (शून्य *) LUAVIM_SETREF);
    lua_pushcfunction(L, luaV_setref);
    lua_rawset(L, LUA_REGISTRYINDEX);
    // register
    luaV_newmetatable(L, LUAVIM_LIST);
    luaV_register(L, luaV_List_mt);
    lua_काढा(L, 1);
    luaV_newmetatable(L, LUAVIM_DICT);
    luaV_register(L, luaV_Dict_mt);
    lua_काढा(L, 1);
    luaV_newmetatable(L, LUAVIM_BLOB);
    luaV_register(L, luaV_Blob_mt);
    lua_काढा(L, 1);
    luaV_newmetatable(L, LUAVIM_FUNCREF);
    luaV_register(L, luaV_Funcref_mt);
    lua_काढा(L, 1);
    luaV_newmetatable(L, LUAVIM_BUFFER);
    luaV_register(L, luaV_Buffer_mt);
    lua_काढा(L, 1);
    luaV_newmetatable(L, LUAVIM_WINDOW);
    luaV_register(L, luaV_Window_mt);
    lua_काढा(L, 1);
    lua_newtable(L); // vim table
    luaV_register(L, luaV_module);
    luaV_pushversion(L);
    lua_setfield(L, -2, "lua_version");
    lua_setglobal(L, LUAVIM_NAME);
    // custom code
    (शून्य)luaL_dostring(L, LUA_VIM_FN_CODE);
    (शून्य)luaL_dostring(L, LUA_VIM_UPDATE_PACKAGE_PATHS);
    (शून्य)luaL_dostring(L, LUA_VIM_SETUP_VARIABLE_DICTS);

    lua_getglobal(L, LUAVIM_NAME);
    lua_getfield(L, -1, "_update_package_paths");

    जर (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);

    परत 0;
}

    स्थिर lua_State *
luaV_newstate(शून्य)
{
    lua_State *L = luaL_newstate();
# जर LUA_VERSION_अंक >= 505
    luaL_openselectedlibs(L, ~0, 0); // core libs
# अन्यथा
    luaL_openlibs(L); // core libs
# endif
    lua_pushcfunction(L, luaopen_vim); // vim
    lua_call(L, 0, 0);
    परत L;
}

    स्थिर शून्य
luaV_setrange(lua_State *L, पूर्णांक line1, पूर्णांक line2)
{
    lua_getglobal(L, LUAVIM_NAME);
    lua_pushinteger(L, line1);
    lua_setfield(L, -2, "firstline");
    lua_pushinteger(L, line2);
    lua_setfield(L, -2, "lastline");
    lua_काढा(L, 1); // vim table
}


// =======   Interface   =======

स्थिर lua_State *L = रिक्त;

    स्थिर पूर्णांक
lua_isopen(शून्य)
{
    परत L != रिक्त;
}

    स्थिर पूर्णांक
lua_init(शून्य)
{
    जर (lua_isopen())
	परत OK;

# ifdef DYNAMIC_LUA
    जर (!lua_enabled(TRUE))
    {
	emsg(_("Lua library cannot be loaded."));
	परत FAIL;
    }
# endif
    L = luaV_newstate();

    परत OK;
}

    शून्य
lua_end(शून्य)
{
    जर (!lua_isopen())
	परत;

    lua_close(L);
    L = रिक्त;
}

/*
 * ex commands
 */
    शून्य
ex_lua(exarg_T *eap)
{
    अक्षर *script = (अक्षर *)script_get(eap, eap->arg);

    जर (!eap->skip && lua_init() == OK)
    {
	अक्षर *s = script != रिक्त ? script : (अक्षर *)eap->arg;

	luaV_setrange(L, eap->line1, eap->line2);
	जर (luaL_loadbuffer(L, s, म_माप(s), LUAVIM_CHUNKNAME)
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
    जर (script != रिक्त)
	vim_free(script);
}

    शून्य
ex_luado(exarg_T *eap)
{
    linenr_T l;
    स्थिरांक अक्षर *s = (स्थिरांक अक्षर *) eap->arg;
    luaL_Buffer b;
    माप_प्रकार len;
    buf_T *was_curbuf = curbuf;

    जर (lua_init() == FAIL) परत;
    जर (u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
    {
	emsg(_("cannot save undo information"));
	परत;
    }
    luaV_setrange(L, eap->line1, eap->line2);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, "return function(line, linenr) ", 30); // header
    luaL_addlstring(&b, s, म_माप(s));
    luaL_addlstring(&b, " end", 4); // footer
    luaL_pushresult(&b);
    s = lua_tolstring(L, -1, &len);
    जर (luaL_loadbuffer(L, s, len, LUAVIM_CHUNKNAME))
    {
	luaV_emsg(L);
	lua_काढा(L, 1); // function body
	परत;
    }
    lua_call(L, 0, 1);
    lua_replace(L, -2); // function -> body
    साठी (l = eap->line1; l <= eap->line2; l++)
    {
	// Check the line number, the command may have deleted lines.
	जर (l > curbuf->b_ml.ml_ओळ_मोजणी)
	    थांबा;

	lua_pushvalue(L, -1); // function
	luaV_pushline(L, curbuf, l); // current line as arg
	lua_pushinteger(L, l); // current line number as arg
	जर (lua_pcall(L, 2, 1, 0))
	{
	    luaV_emsg(L);
	    थांबा;
	}

	// Catch the command switching to another buffer.
	// Check the line number, the command may have deleted lines.
	जर (curbuf != was_curbuf || l > curbuf->b_ml.ml_ओळ_मोजणी)
	    थांबा;

	जर (lua_isstring(L, -1)) // update line?
	{
# ifdef HAVE_SANDBOX
	    luaV_checksandbox(L);
# endif
	    ml_replace(l, luaV_toline(L, -1), TRUE);
	    changed_bytes(l, 0);
	    lua_काढा(L, 1); // result from luaV_toline
	}
	lua_काढा(L, 1); //ओळ
    }
    lua_काढा(L, 1); // function
    check_cursor();
    update_screen(UPD_NOT_VALID);
}

    शून्य
ex_luafile(exarg_T *eap)
{
    जर (lua_init() == FAIL)
	परत;
    जर (!eap->skip)
    {
	luaV_setrange(L, eap->line1, eap->line2);
	जर (luaL_loadfile(L, (अक्षर *) eap->arg) || lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
}

# define luaV_freetype(typ,tname) \
	शून्य \
    lua_##tname##_free(typ *o) \
    { \
	जर (!lua_isopen()) परत; \
	luaV_getfield(L, LUAVIM_FREE); \
	lua_pushlightuserdata(L, (शून्य *) o); \
	lua_call(L, 1, 0); \
    }

luaV_freetype(buf_T, buffer)
luaV_freetype(win_T, window)

    शून्य
do_luaeval(char_u *str, typval_T *arg, typval_T *rettv)
{
    lua_init();
    luaV_getfield(L, LUAVIM_LUAEVAL);
    lua_pushstring(L, (अक्षर *) str);
    lua_pushlightuserdata(L, (शून्य *) arg);
    lua_pushlightuserdata(L, (शून्य *) rettv);
    lua_call(L, 3, 0);
}

    पूर्णांक
set_ref_in_lua(पूर्णांक copyID)
{
    पूर्णांक aborted = 0;

    जर (!lua_isopen())
	परत 0;

    luaV_getfield(L, LUAVIM_SETREF);
    // call the function with 1 arg, getting 1 result back
    lua_pushinteger(L, copyID);
    lua_call(L, 1, 1);
    // get the result
    aborted = lua_tointeger(L, -1);
    // pop result off the stack
    lua_काढा(L, 1);

    परत aborted;
}

    शून्य
update_package_paths_in_lua(शून्य)
{
    जर (!lua_isopen())
	परत;

    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "_update_package_paths");

    जर (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);
}

/*
 * Native C function callback
 */
    स्थिर पूर्णांक
luaV_call_lua_func(
	पूर्णांक	 argcount,
	typval_T *argvars,
	typval_T *rettv,
	शून्य	 *state)
{
    पूर्णांक i;
    पूर्णांक luaargcount = argcount;
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_funcref);

    जर (funcstate->lua_tableref != LUA_NOREF)
    {
	// First arg for metatable __call method is a table
	luaargcount += 1;
	lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    }

    साठी (i = 0; i < argcount; ++i)
	luaV_pushtypval(funcstate->L, &argvars[i]);

    जर (lua_pcall(funcstate->L, luaargcount, 1, 0))
    {
	luaV_emsg(funcstate->L);
	परत (पूर्णांक)FCERR_OTHER;
    }

    luaV_checktypval(funcstate->L, -1, rettv, "get return value");
    परत (पूर्णांक)FCERR_NONE;
}

/*
 * Free up any lua references held by the func state.
 */
    स्थिर शून्य
luaV_call_lua_func_free(शून्य *state)
{
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_funcref);
    funcstate->L = रिक्त;
    जर (funcstate->lua_tableref != LUA_NOREF)
	luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    VIM_CLEAR(funcstate);
}

#endif
