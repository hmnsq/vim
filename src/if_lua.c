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

#जइ __STDC_VERSION__ >= 199901L
# define LUAV_INLINE अन्तःस्थ
#अण्णहा
# define LUAV_INLINE
#endif

// Only do the following when the feature is enabled.  Needed for "make
// depend".
#जइ defined(FEAT_LUA)

# define LUAVIM_CHUNKNAME "vim chunk"
# define LUAVIM_NAME "vim"
# define LUAVIM_EVALNAME "luaeval"
# define LUAVIM_EVALHEADER "local _A=select(1,...) return "

# ifdef LUA_RELEASE
#  define LUAVIM_VERSION LUA_RELEASE
# अण्णहा
#  define LUAVIM_VERSION LUA_VERSION
# endif

प्रकारनिर्देश buf_T *luaV_Buffer;
प्रकारनिर्देश win_T *luaV_Window;
प्रकारनिर्देश dict_T *luaV_Dict;
प्रकारनिर्देश list_T *luaV_List;
प्रकारनिर्देश blob_T *luaV_Blob;
प्रकारनिर्देश संरचना {
    char_u	*name;	// funcref
    dict_T	*self;	// selfdict
} luaV_Funcref;
प्रकारनिर्देश पूर्णाङ्क (*msgfunc_T)(वर्ण *);

प्रकारनिर्देश संरचना {
    पूर्णाङ्क lua_funcref;    // ref to a lua func
    पूर्णाङ्क lua_tableref;   // ref to a lua table if metatable else LUA_NOREF. used
			// for __call
    lua_State *L;
} luaV_CFuncState;

स्थिर ध्रुव वर्ण LUAVIM_DICT[] = "dict";
स्थिर ध्रुव वर्ण LUAVIM_LIST[] = "list";
स्थिर ध्रुव वर्ण LUAVIM_BLOB[] = "blob";
स्थिर ध्रुव वर्ण LUAVIM_FUNCREF[] = "funcref";
स्थिर ध्रुव वर्ण LUAVIM_BUFFER[] = "buffer";
स्थिर ध्रुव वर्ण LUAVIM_WINDOW[] = "window";
स्थिर ध्रुव वर्ण LUAVIM_FREE[] = "luaV_free";
स्थिर ध्रुव वर्ण LUAVIM_LUAEVAL[] = "luaV_luaeval";
स्थिर ध्रुव वर्ण LUAVIM_SETREF[] = "luaV_setref";

स्थिर ध्रुव वर्ण LUA___CALL[] = "__call";

// get/setudata manage references to vim userdata in a cache table through
// object pointers (light userdata). The cache table itself is retrieved
// from the registry.

स्थिर ध्रुव वर्ण LUAVIM_UDATA_CACHE[] = "luaV_udata_cache";

# define luaV_getfield(L, s) \
    lua_pushlightuserdata((L), (शून्य *)(s)); \
    lua_rawget((L), LUA_REGISTRYINDEX)
# define luaV_checksandbox(L) \
    जइ (sandbox) luaL_error((L), "not allowed in sandbox")
# define lua_V_सन्देशः(L) luaV_msgfunc((L), (msgfunc_T) सन्देशः)
# define luaV_emsg(L) luaV_msgfunc((L), (msgfunc_T) emsg)
# define luaV_checktypval(L, a, v, सन्देशः) \
    करेहि { \
	जइ (luaV_totypval(L, a, v) == FAIL) \
	    luaL_error(L, सन्देशः ": cannot convert value"); \
    } जाव (0)

स्थिर luaV_List *luaV_pushlist(lua_State *L, list_T *lis);
स्थिर luaV_Dict *luaV_pushdict(lua_State *L, dict_T *dic);
स्थिर luaV_Blob *luaV_pushblob(lua_State *L, blob_T *blo);
स्थिर luaV_Funcref *luaV_pushfuncref(lua_State *L, char_u *name);
स्थिर पूर्णाङ्क luaV_call_lua_func(पूर्णाङ्क argcount, typval_T *argvars, typval_T *rettv, शून्य *state);
स्थिर शून्य luaV_call_lua_func_free(शून्य *state);

# जइ LUA_VERSION_अङ्कः <= 501
#  define luaV_register(L, l) luaL_register(L, रिक्त, l)
#  define luaL_typeerror luaL_typerror
# अण्णहा
#  define luaV_register(L, l) luaL_setfuncs(L, l, 0)
# endif

# ifdef DYNAMIC_LUA

#  ifdef MSWIN
#   define load_dll vimLoadLib
#   define चिह्नम्_from_dll GetProcAddress
#   define close_dll FreeLibrary
#   define load_dll_error GetWin32Error
#  अण्णहा
#   include <dlfcn.h>
#   define HANDLE शून्य*
#   define load_dll(n) dlopen((n), RTLD_LAZY|RTLD_GLOBAL)
#   define चिह्नम्_from_dll dlsym
#   define close_dll dlclose
#   define load_dll_error dlerror
#  endif

// lauxlib
#  जइ LUA_VERSION_अङ्कः <= 501
#   define luaL_register dll_luaL_register
#   define luaL_prepbuffer dll_luaL_prepbuffer
#   define luaL_openlib dll_luaL_openlib
#   define luaL_typerror dll_luaL_typerror
#   define luaL_loadfile dll_luaL_loadfile
#   define luaL_loadbuffer dll_luaL_loadbuffer
#  अण्णहा
#   define luaL_prepbuffsize dll_luaL_prepbuffsize
#   define luaL_setfuncs dll_luaL_setfuncs
#   define luaL_loadfilex dll_luaL_loadfilex
#   define luaL_loadbufferx dll_luaL_loadbufferx
#   define luaL_argerror dll_luaL_argerror
#  endif
#  जइ LUA_VERSION_अङ्कः >= 504
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
#  जइ LUA_VERSION_अङ्कः <= 501
#   define lua_tonumber dll_lua_tonumber
#   define lua_tointeger dll_lua_tointeger
#   define lua_call dll_lua_call
#   define lua_pcall dll_lua_pcall
#  अण्णहा
#   define lua_tonumberx dll_lua_tonumberx
#   define lua_tointegerx dll_lua_tointegerx
#   define lua_callk dll_lua_callk
#   define lua_pcallk dll_lua_pcallk
#   define lua_getglobal dll_lua_getglobal
#   define lua_setglobal dll_lua_setglobal
#  endif
#  जइ LUA_VERSION_अङ्कः <= 502
#   define lua_replace dll_lua_replace
#   define lua_remove dll_lua_remove
#  endif
#  जइ LUA_VERSION_अङ्कः >= 503
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
#  जइ LUA_VERSION_अङ्कः >= 504
#   define lua_newuserdatauv dll_lua_newuserdatauv
#  अण्णहा
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
#  जइ LUA_VERSION_अङ्कः >= 505
#   define luaL_openselectedlibs dll_luaL_openselectedlibs
#  अण्णहा
#   define luaL_openlibs dll_luaL_openlibs
#  endif

// lauxlib
#  जइ LUA_VERSION_अङ्कः <= 501
शून्य (*dll_luaL_register) (lua_State *L, ध्रुव वर्ण *libname, ध्रुव luaL_Reg *l);
वर्ण *(*dll_luaL_prepbuffer) (luaL_Buffer *B);
शून्य (*dll_luaL_openlib) (lua_State *L, ध्रुव वर्ण *libname, ध्रुव luaL_Reg *l, पूर्णाङ्क nup);
पूर्णाङ्क (*dll_luaL_typerror) (lua_State *L, पूर्णाङ्क narg, ध्रुव वर्ण *tname);
पूर्णाङ्क (*dll_luaL_loadfile) (lua_State *L, ध्रुव वर्ण *filename);
पूर्णाङ्क (*dll_luaL_loadbuffer) (lua_State *L, ध्रुव वर्ण *buff, size_t sz, ध्रुव वर्ण *name);
#  अण्णहा
वर्ण *(*dll_luaL_prepbuffsize) (luaL_Buffer *B, size_t sz);
शून्य (*dll_luaL_setfuncs) (lua_State *L, ध्रुव luaL_Reg *l, पूर्णाङ्क nup);
पूर्णाङ्क (*dll_luaL_loadfilex) (lua_State *L, ध्रुव वर्ण *filename, ध्रुव वर्ण *mode);
पूर्णाङ्क (*dll_luaL_loadbufferx) (lua_State *L, ध्रुव वर्ण *buff, size_t sz, ध्रुव वर्ण *name, ध्रुव वर्ण *mode);
पूर्णाङ्क (*dll_luaL_argerror) (lua_State *L, पूर्णाङ्क numarg, ध्रुव वर्ण *extramsg);
#  endif
#  जइ LUA_VERSION_अङ्कः >= 504
पूर्णाङ्क (*dll_luaL_typeerror) (lua_State *L, पूर्णाङ्क narg, ध्रुव वर्ण *tname);
#  endif
शून्य (*dll_luaL_checkany) (lua_State *L, पूर्णाङ्क narg);
ध्रुव वर्ण *(*dll_luaL_checklstring) (lua_State *L, पूर्णाङ्क अङ्कः_Arg, size_t *l);
lua_Integer (*dll_luaL_checkinteger) (lua_State *L, पूर्णाङ्क अङ्कः_Arg);
lua_Integer (*dll_luaL_optinteger) (lua_State *L, पूर्णाङ्क nArg, lua_Integer def);
शून्य (*dll_luaL_checktype) (lua_State *L, पूर्णाङ्क narg, पूर्णाङ्क t);
पूर्णाङ्क (*dll_luaL_error) (lua_State *L, ध्रुव वर्ण *fmt, ...);
lua_State *(*dll_luaL_newstate) (शून्य);
शून्य (*dll_luaL_buffinit) (lua_State *L, luaL_Buffer *B);
शून्य (*dll_luaL_addlstring) (luaL_Buffer *B, ध्रुव वर्ण *s, size_t l);
शून्य (*dll_luaL_pushresult) (luaL_Buffer *B);
पूर्णाङ्क (*dll_luaL_loadstring) (lua_State *L, ध्रुव वर्ण *s);
पूर्णाङ्क (*dll_luaL_ref) (lua_State *L, पूर्णाङ्क idx);
#  जइ LUA_VERSION_अङ्कः <= 502
शून्य (*dll_luaL_unref) (lua_State *L, पूर्णाङ्क idx, पूर्णाङ्क n);
#  अण्णहा
शून्य (*dll_luaL_unref) (lua_State *L, पूर्णाङ्क idx, lua_Integer n);
#  endif
// lua
#  जइ LUA_VERSION_अङ्कः <= 501
lua_सङ्ख्या (*dll_lua_tonumber) (lua_State *L, पूर्णाङ्क idx);
lua_Integer (*dll_lua_tointeger) (lua_State *L, पूर्णाङ्क idx);
शून्य (*dll_lua_call) (lua_State *L, पूर्णाङ्क nargs, पूर्णाङ्क nresults);
पूर्णाङ्क (*dll_lua_pcall) (lua_State *L, पूर्णाङ्क nargs, पूर्णाङ्क nresults, पूर्णाङ्क errfunc);
#  अण्णहा
lua_सङ्ख्या (*dll_lua_tonumberx) (lua_State *L, पूर्णाङ्क idx, पूर्णाङ्क *isnum);
lua_Integer (*dll_lua_tointegerx) (lua_State *L, पूर्णाङ्क idx, पूर्णाङ्क *isnum);
शून्य (*dll_lua_callk) (lua_State *L, पूर्णाङ्क nargs, पूर्णाङ्क nresults, पूर्णाङ्क ctx,
	lua_CFunction k);
पूर्णाङ्क (*dll_lua_pcallk) (lua_State *L, पूर्णाङ्क nargs, पूर्णाङ्क nresults, पूर्णाङ्क errfunc,
	पूर्णाङ्क ctx, lua_CFunction k);
शून्य (*dll_lua_getglobal) (lua_State *L, ध्रुव वर्ण *var);
शून्य (*dll_lua_setglobal) (lua_State *L, ध्रुव वर्ण *var);
#  endif
#  जइ LUA_VERSION_अङ्कः <= 502
शून्य (*dll_lua_replace) (lua_State *L, पूर्णाङ्क idx);
शून्य (*dll_lua_remove) (lua_State *L, पूर्णाङ्क idx);
#  endif
#  जइ LUA_VERSION_अङ्कः >= 503
शून्य  (*dll_lua_rotate) (lua_State *L, पूर्णाङ्क idx, पूर्णाङ्क n);
शून्य (*dll_lua_copy) (lua_State *L, पूर्णाङ्क fromidx, पूर्णाङ्क toidx);
#  endif
ध्रुव वर्ण *(*dll_lua_typename) (lua_State *L, पूर्णाङ्क tp);
शून्य       (*dll_lua_close) (lua_State *L);
पूर्णाङ्क (*dll_lua_gettop) (lua_State *L);
शून्य (*dll_lua_settop) (lua_State *L, पूर्णाङ्क idx);
शून्य (*dll_lua_pushvalue) (lua_State *L, पूर्णाङ्क idx);
पूर्णाङ्क (*dll_lua_isnumber) (lua_State *L, पूर्णाङ्क idx);
पूर्णाङ्क (*dll_lua_isstring) (lua_State *L, पूर्णाङ्क idx);
पूर्णाङ्क (*dll_lua_type) (lua_State *L, पूर्णाङ्क idx);
पूर्णाङ्क (*dll_lua_rawequal) (lua_State *L, पूर्णाङ्क idx1, पूर्णाङ्क idx2);
पूर्णाङ्क (*dll_lua_toboolean) (lua_State *L, पूर्णाङ्क idx);
ध्रुव वर्ण *(*dll_lua_tolstring) (lua_State *L, पूर्णाङ्क idx, size_t *len);
शून्य *(*dll_lua_touserdata) (lua_State *L, पूर्णाङ्क idx);
शून्य (*dll_lua_pushnil) (lua_State *L);
शून्य (*dll_lua_pushnumber) (lua_State *L, lua_सङ्ख्या n);
शून्य (*dll_lua_pushinteger) (lua_State *L, lua_Integer n);
शून्य (*dll_lua_pushlstring) (lua_State *L, ध्रुव वर्ण *s, size_t l);
शून्य (*dll_lua_pushstring) (lua_State *L, ध्रुव वर्ण *s);
ध्रुव वर्ण *(*dll_lua_pushfstring) (lua_State *L, ध्रुव वर्ण *fmt, ...);
शून्य (*dll_lua_pushcclosure) (lua_State *L, lua_CFunction fn, पूर्णाङ्क n);
शून्य (*dll_lua_pushboolean) (lua_State *L, पूर्णाङ्क b);
शून्य (*dll_lua_pushlightuserdata) (lua_State *L, शून्य *p);
शून्य (*dll_lua_getfield) (lua_State *L, पूर्णाङ्क idx, ध्रुव वर्ण *k);
#  जइ LUA_VERSION_अङ्कः <= 502
शून्य (*dll_lua_rawget) (lua_State *L, पूर्णाङ्क idx);
शून्य (*dll_lua_rawgeti) (lua_State *L, पूर्णाङ्क idx, पूर्णाङ्क n);
#  अण्णहा
पूर्णाङ्क (*dll_lua_rawget) (lua_State *L, पूर्णाङ्क idx);
पूर्णाङ्क (*dll_lua_rawgeti) (lua_State *L, पूर्णाङ्क idx, lua_Integer n);
#  endif
शून्य (*dll_lua_createtable) (lua_State *L, पूर्णाङ्क narr, पूर्णाङ्क nrec);
शून्य (*dll_lua_settable) (lua_State *L, पूर्णाङ्क idx);
#  जइ LUA_VERSION_अङ्कः >= 504
शून्य *(*dll_lua_newuserdatauv) (lua_State *L, size_t sz, पूर्णाङ्क nuvalue);
#  अण्णहा
शून्य *(*dll_lua_newuserdata) (lua_State *L, size_t sz);
#  endif
पूर्णाङ्क (*dll_lua_getmetatable) (lua_State *L, पूर्णाङ्क objindex);
शून्य (*dll_lua_setfield) (lua_State *L, पूर्णाङ्क idx, ध्रुव वर्ण *k);
शून्य (*dll_lua_rawset) (lua_State *L, पूर्णाङ्क idx);
#  जइ LUA_VERSION_अङ्कः <= 502
शून्य (*dll_lua_rawseti) (lua_State *L, पूर्णाङ्क idx, पूर्णाङ्क n);
#  अण्णहा
शून्य (*dll_lua_rawseti) (lua_State *L, पूर्णाङ्क idx, lua_Integer n);
#  endif
पूर्णाङ्क (*dll_lua_setmetatable) (lua_State *L, पूर्णाङ्क objindex);
पूर्णाङ्क (*dll_lua_next) (lua_State *L, पूर्णाङ्क idx);
// libs
पूर्णाङ्क (*dll_luaopen_base) (lua_State *L);
पूर्णाङ्क (*dll_luaopen_table) (lua_State *L);
पूर्णाङ्क (*dll_luaopen_string) (lua_State *L);
पूर्णाङ्क (*dll_luaopen_math) (lua_State *L);
पूर्णाङ्क (*dll_luaopen_io) (lua_State *L);
पूर्णाङ्क (*dll_luaopen_os) (lua_State *L);
पूर्णाङ्क (*dll_luaopen_package) (lua_State *L);
पूर्णाङ्क (*dll_luaopen_debug) (lua_State *L);
#  जइ LUA_VERSION_अङ्कः >= 505
शून्य (*dll_luaL_openselectedlibs) (lua_State *L, पूर्णाङ्क load, पूर्णाङ्क preload);
#  अण्णहा
शून्य (*dll_luaL_openlibs) (lua_State *L);
#  endif

प्रकारनिर्देश शून्य **luaV_function;
प्रकारनिर्देश संरचना {
    ध्रुव वर्ण *name;
    luaV_function func;
} luaV_Reg;

स्थिर ध्रुव luaV_Reg luaV_dll[] = {
    // lauxlib
#  जइ LUA_VERSION_अङ्कः <= 501
    {"luaL_register", (luaV_function) &dll_luaL_register},
    {"luaL_prepbuffer", (luaV_function) &dll_luaL_prepbuffer},
    {"luaL_openlib", (luaV_function) &dll_luaL_openlib},
    {"luaL_typerror", (luaV_function) &dll_luaL_typerror},
    {"luaL_loadfile", (luaV_function) &dll_luaL_loadfile},
    {"luaL_loadbuffer", (luaV_function) &dll_luaL_loadbuffer},
#  अण्णहा
    {"luaL_prepbuffsize", (luaV_function) &dll_luaL_prepbuffsize},
    {"luaL_setfuncs", (luaV_function) &dll_luaL_setfuncs},
    {"luaL_loadfilex", (luaV_function) &dll_luaL_loadfilex},
    {"luaL_loadbufferx", (luaV_function) &dll_luaL_loadbufferx},
    {"luaL_argerror", (luaV_function) &dll_luaL_argerror},
#  endif
#  जइ LUA_VERSION_अङ्कः >= 504
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
#  जइ LUA_VERSION_अङ्कः <= 501
    {"lua_tonumber", (luaV_function) &dll_lua_tonumber},
    {"lua_tointeger", (luaV_function) &dll_lua_tointeger},
    {"lua_call", (luaV_function) &dll_lua_call},
    {"lua_pcall", (luaV_function) &dll_lua_pcall},
#  अण्णहा
    {"lua_tonumberx", (luaV_function) &dll_lua_tonumberx},
    {"lua_tointegerx", (luaV_function) &dll_lua_tointegerx},
    {"lua_callk", (luaV_function) &dll_lua_callk},
    {"lua_pcallk", (luaV_function) &dll_lua_pcallk},
    {"lua_getglobal", (luaV_function) &dll_lua_getglobal},
    {"lua_setglobal", (luaV_function) &dll_lua_setglobal},
#  endif
#  जइ LUA_VERSION_अङ्कः <= 502
    {"lua_replace", (luaV_function) &dll_lua_replace},
    {"lua_remove", (luaV_function) &dll_lua_remove},
#  endif
#  जइ LUA_VERSION_अङ्कः >= 503
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
#  जइ LUA_VERSION_अङ्कः >= 504
    {"lua_newuserdatauv", (luaV_function) &dll_lua_newuserdatauv},
#  अण्णहा
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
#  जइ LUA_VERSION_अङ्कः >= 505
    {"luaL_openselectedlibs", (luaV_function) &dll_luaL_openselectedlibs},
#  अण्णहा
    {"luaL_openlibs", (luaV_function) &dll_luaL_openlibs},
#  endif
    {रिक्त, रिक्त}
};

स्थिर HANDLE hinstLua = रिक्त;

    स्थिर पूर्णाङ्क
lua_link_init(वर्ण *libname, पूर्णाङ्क verbose)
{
    ध्रुव luaV_Reg *reg;
    जइ (hinstLua) प्रत्यागम OK;
    hinstLua = load_dll(libname);
    जइ (!hinstLua)
    {
	जइ (verbose)
	    semsg(_(e_could_not_load_library_str_str), libname, load_dll_error());
	प्रत्यागम FAIL;
    }
    प्रति (reg = luaV_dll; reg->func; reg++)
    {
	जइ ((*reg->func = चिह्नम्_from_dll(hinstLua, reg->name)) == रिक्त)
	{
	    close_dll(hinstLua);
	    hinstLua = 0;
	    जइ (verbose)
		semsg(_(e_could_not_load_library_function_str), reg->name);
	    प्रत्यागम FAIL;
	}
    }
    प्रत्यागम OK;
}
# endif // DYNAMIC_LUA

# जइ defined(DYNAMIC_LUA)
    पूर्णाङ्क
lua_enabled(पूर्णाङ्क verbose)
{
    प्रत्यागम lua_link_init((वर्ण *)p_luadll, verbose) == OK;
}
# endif

# जइ LUA_VERSION_अङ्कः > 501 && LUA_VERSION_अङ्कः < 504
    स्थिर पूर्णाङ्क
luaL_typeerror(lua_State *L, पूर्णाङ्क narg, ध्रुव वर्ण *tname)
{
    ध्रुव वर्ण *सन्देशः = lua_pushfstring(L, "%s expected, got %s",
	    tname, luaL_typename(L, narg));
    प्रत्यागम luaL_argerror(L, narg, सन्देशः);
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
    lua_निष्कासय(L, 1);		       // and remove the cache table
}

// =======   Internal   =======

    स्थिर शून्य
luaV_newmetatable(lua_State *L, ध्रुव वर्ण *tname)
{
    lua_newtable(L);
    lua_pushlightuserdata(L, (शून्य *) tname);
    lua_pushvalue(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
}

    स्थिर शून्य *
luaV_toudata(lua_State *L, पूर्णाङ्क ud, ध्रुव वर्ण *tname)
{
    शून्य *p = lua_touserdata(L, ud);

    जइ (p == रिक्त)
	प्रत्यागम रिक्त;

    // value is userdata
    जइ (lua_getmetatable(L, ud)) // does it have a metatable?
    {
	luaV_getfield(L, tname); // get metatable
	जइ (lua_rawequal(L, -1, -2)) // MTs match?
	{
	    lua_निष्कासय(L, 2); // MTs
	    प्रत्यागम p;
	}
    }

    प्रत्यागम रिक्त;
}

    स्थिर शून्य *
luaV_checkcache(lua_State *L, शून्य *p)
{
    luaV_getudata(L, p);
    जइ (lua_isnil(L, -1)) luaL_error(L, "invalid object");
    lua_निष्कासय(L, 1);
    प्रत्यागम p;
}

# define luaV_unbox(L,luatyp,ud) (*((luatyp *) lua_touserdata((L),(ud))))

# define luaV_checkvalid(L,luatyp,ud) \
    luaV_checkcache((L), (शून्य *) luaV_unbox((L),luatyp,(ud)))

    स्थिर शून्य *
luaV_checkudata(lua_State *L, पूर्णाङ्क ud, ध्रुव वर्ण *tname)
{
    शून्य *p = luaV_toudata(L, ud, tname);
    जइ (p == रिक्त) luaL_typeerror(L, ud, tname);
    प्रत्यागम p;
}

    स्थिर शून्य
luaV_pushtypval(lua_State *L, typval_T *tv)
{
    जइ (tv == रिक्त)
    {
	lua_pushnil(L);
	प्रत्यागम;
    }
    विकल्प (tv->v_type)
    {
	पक्ष VAR_STRING:
	    lua_pushstring(L, tv->vval.v_string == रिक्त
					    ? "" : (वर्ण *)tv->vval.v_string);
	    भङ्ग;
	पक्ष VAR_सङ्ख्या:
	    lua_pushinteger(L, (पूर्णाङ्क) tv->vval.v_सङ्ख्या);
	    भङ्ग;
	पक्ष VAR_FLOAT:
	    lua_pushnumber(L, (lua_सङ्ख्या) tv->vval.v_float);
	    भङ्ग;
	पक्ष VAR_LIST:
	    luaV_pushlist(L, tv->vval.v_list);
	    भङ्ग;
	पक्ष VAR_DICT:
	    luaV_pushdict(L, tv->vval.v_dict);
	    भङ्ग;
	पक्ष VAR_BOOL:
	पक्ष VAR_SPECIAL:
	    जइ (tv->vval.v_सङ्ख्या <= VVAL_TRUE)
		lua_pushboolean(L, (पूर्णाङ्क) tv->vval.v_सङ्ख्या);
	    अण्णहा
		lua_pushnil(L);
	    भङ्ग;
	पक्ष VAR_FUNC:
	    luaV_pushfuncref(L, tv->vval.v_string);
	    भङ्ग;
	पक्ष VAR_PARTIAL:
	    // TODO: handle partial arguments
	    luaV_pushfuncref(L, partial_name(tv->vval.v_partial));
	    भङ्ग;

	पक्ष VAR_BLOB:
	    luaV_pushblob(L, tv->vval.v_blob);
	    भङ्ग;
	स्वभाव:
	    lua_pushnil(L);
    }
}

/*
 * Converts lua value at 'pos' to typval 'tv'.
 * Returns OK or FAIL.
 */
    स्थिर पूर्णाङ्क
luaV_totypval(lua_State *L, पूर्णाङ्क pos, typval_T *tv)
{
    पूर्णाङ्क status = OK;

    tv->v_lock = 0;

    विकल्प (lua_type(L, pos))
    {
	पक्ष LUA_TBOOLEAN:
	    tv->v_type = VAR_BOOL;
	    tv->vval.v_सङ्ख्या = (varnumber_T) lua_toboolean(L, pos);
	    भङ्ग;
	पक्ष LUA_TNIL:
	    tv->v_type = VAR_SPECIAL;
	    tv->vval.v_सङ्ख्या = VVAL_NULL;
	    भङ्ग;
	पक्ष LUA_TSTRING:
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strsave((char_u *) lua_tostring(L, pos));
	    भङ्ग;
	पक्ष LUA_TNUMBER:
	{
	    ध्रुव lua_सङ्ख्या n = lua_tonumber(L, pos);

	    जइ (n > (lua_सङ्ख्या)INT64_MAX || n < (lua_सङ्ख्या)INT64_MIN
		    || ((lua_सङ्ख्या)((varnumber_T)n)) != n)
	    {
		tv->v_type = VAR_FLOAT;
		tv->vval.v_float = (float_T)n;
	    }
	    अण्णहा
	    {
		tv->v_type = VAR_सङ्ख्या;
		tv->vval.v_सङ्ख्या = (varnumber_T)n;
	    }
	}
	    भङ्ग;
	पक्ष LUA_TFUNCTION:
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
	    भङ्ग;
	}
	पक्ष LUA_TTABLE:
	{
	    पूर्णाङ्क lua_tableref;

	    lua_pushvalue(L, pos);
	    lua_tableref = luaL_ref(L, LUA_REGISTRYINDEX);
	    जइ (lua_getmetatable(L, pos))
	    {
		lua_getfield(L, -1, LUA___CALL);
		जइ (lua_isfunction(L, -1))
		{
		    char_u *name;
		    पूर्णाङ्क lua_funcref = luaL_ref(L, LUA_REGISTRYINDEX);
		    luaV_CFuncState *state = ALLOC_CLEAR_ONE(luaV_CFuncState);

		    state->lua_funcref = lua_funcref;
		    state->L = L;
		    state->lua_tableref = lua_tableref;
		    name = register_cfunc(&luaV_call_lua_func,
					      &luaV_call_lua_func_free, state);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(name);
		    भङ्ग;
		}
	    }
	    tv->v_type = VAR_सङ्ख्या;
	    tv->vval.v_सङ्ख्या = 0;
	    status = FAIL;
	    भङ्ग;
	}
	पक्ष LUA_TUSERDATA:
	{
	    शून्य *p = lua_touserdata(L, pos);

	    जइ (lua_getmetatable(L, pos)) // has metatable?
	    {
		// check list
		luaV_getfield(L, LUAVIM_LIST);
		जइ (lua_rawequal(L, -1, -2))
		{
		    tv->v_type = VAR_LIST;
		    tv->vval.v_list = *((luaV_List *) p);
		    ++tv->vval.v_list->lv_refcount;
		    lua_निष्कासय(L, 2); // MTs
		    भङ्ग;
		}
		// check dict
		luaV_getfield(L, LUAVIM_DICT);
		जइ (lua_rawequal(L, -1, -3))
		{
		    tv->v_type = VAR_DICT;
		    tv->vval.v_dict = *((luaV_Dict *) p);
		    ++tv->vval.v_dict->dv_refcount;
		    lua_निष्कासय(L, 3); // MTs
		    भङ्ग;
		}
		// check blob
		luaV_getfield(L, LUAVIM_BLOB);
		जइ (lua_rawequal(L, -1, -4))
		{
		    tv->v_type = VAR_BLOB;
		    tv->vval.v_blob = *((luaV_Blob *) p);
		    ++tv->vval.v_blob->bv_refcount;
		    lua_निष्कासय(L, 4); // MTs
		    भङ्ग;
		}
		// check funcref
		luaV_getfield(L, LUAVIM_FUNCREF);
		जइ (lua_rawequal(L, -1, -5))
		{
		    luaV_Funcref *f = (luaV_Funcref *) p;

		    func_ref(f->name);
		    tv->v_type = VAR_FUNC;
		    tv->vval.v_string = vim_strsave(f->name);
		    lua_निष्कासय(L, 5); // MTs
		    भङ्ग;
		}
		lua_निष्कासय(L, 4); // MTs
	    }
	}
	// FALLTHROUGH
	स्वभाव:
	    tv->v_type = VAR_सङ्ख्या;
	    tv->vval.v_सङ्ख्या = 0;
	    status = FAIL;
    }
    प्रत्यागम status;
}

/*
 * similar to luaL_addlstring, but replaces \0 with \n if toline and
 * \n with \0 otherwise
 */
    स्थिर शून्य
luaV_addlstring(luaL_Buffer *b, ध्रुव वर्ण *s, size_t l, पूर्णाङ्क toline)
{
    जाव (l--)
    {
	जइ (*s == '\0' && toline)
	    luaL_addchar(b, '\n');
	अण्णहा जइ (*s == '\n' && !toline)
	    luaL_addchar(b, '\0');
	अण्णहा
	    luaL_addchar(b, *s);
	s++;
    }
}

    स्थिर शून्य
luaV_pushline(lua_State *L, buf_T *buf, linenr_T n)
{
    ध्रुव वर्ण *s = (ध्रुव वर्ण *) ml_get_buf(buf, n, FALSE);
    luaL_Buffer b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, strlen(s), 0);
    luaL_pushresult(&b);
}

    स्थिर char_u *
luaV_toline(lua_State *L, पूर्णाङ्क pos)
{
    size_t l;
    ध्रुव वर्ण *s = lua_tolstring(L, pos, &l);

    luaL_Buffer b;
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 1);
    luaL_pushresult(&b);
    प्रत्यागम (char_u *) lua_tostring(L, -1);
}

/*
 * pops a string s from the top of the stack and calls mf(t) for pieces t of
 * s separated by newlines
 */
    स्थिर शून्य
luaV_msgfunc(lua_State *L, msgfunc_T mf)
{
    luaL_Buffer b;
    size_t l;
    ध्रुव वर्ण *p, *s = lua_tolstring(L, -1, &l);
    luaL_buffinit(L, &b);
    luaV_addlstring(&b, s, l, 0);
    luaL_pushresult(&b);
    // break string
    p = s = lua_tolstring(L, -1, &l);
    जाव (l--)
    {
	जइ (*p++ == '\0') // break?
	{
	    mf((वर्ण *)s);
	    s = p;
	}
    }
    mf((वर्ण *)s);
    lua_निष्कासय(L, 2); // original and modified strings
}

# define luaV_newtype(typ,tname,luatyp,luatname) \
	स्थिर luatyp * \
    luaV_new##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = (luatyp *) lua_newuserdata(L, परिमाण(luatyp)); \
	*o = obj; \
	luaV_setudata(L, obj); /* cache[obj] = udata */ \
	luaV_getfield(L, luatname); \
	lua_setmetatable(L, -2); \
	प्रत्यागम o; \
    }

# define luaV_pushtype(typ,tname,luatyp) \
	स्थिर luatyp * \
    lua_V_प्रक्षिप##tname(lua_State *L, typ *obj) \
    { \
	luatyp *o = रिक्त; \
	जइ (obj == रिक्त) \
	    lua_pushnil(L); \
	अण्णहा \
	{ \
	    luaV_getudata(L, obj); \
	    जइ (lua_isnil(L, -1)) /* not interned? */ \
	    { \
		lua_निष्कासय(L, 1); \
		o = luaV_new##tname(L, obj); \
	    } \
	    अण्णहा \
		o = (luatyp *) lua_touserdata(L, -1); \
	} \
	प्रत्यागम o; \
    }

# define luaV_type_tostring(tname,luatname) \
	स्थिर पूर्णाङ्क \
    luaV_##tname##_tostring(lua_State *L) \
    { \
	lua_pushfstring(L, "%s: %p", luatname, lua_touserdata(L, 1)); \
	प्रत्यागम 1; \
    }

// =======   List type   =======

    स्थिर luaV_List *
luaV_newlist(lua_State *L, list_T *lis)
{
    luaV_List *l = (luaV_List *) lua_newuserdata(L, परिमाण(luaV_List));
    *l = lis;
    lis->lv_refcount++; // reference in Lua
    luaV_setudata(L, lis); // cache[lis] = udata
    luaV_getfield(L, LUAVIM_LIST);
    lua_setmetatable(L, -2);
    प्रत्यागम l;
}

luaV_pushtype(list_T, list, luaV_List)
luaV_type_tostring(list, LUAVIM_LIST)

    स्थिर पूर्णाङ्क
luaV_list_len(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushinteger(L, (पूर्णाङ्क) list_len(l));
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_list_iter(lua_State *L)
{
    listitem_T *li = (listitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    जइ (li == रिक्त) प्रत्यागम 0;
    luaV_pushtypval(L, &li->li_tv);
    lua_pushlightuserdata(L, (शून्य *) li->li_next);
    lua_replace(L, lua_upvalueindex(1));
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_list_call(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    lua_pushlightuserdata(L, (शून्य *) l->lv_first);
    lua_pushcclosure(L, luaV_list_iter, 1);
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_list_index(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    जइ (lua_isnumber(L, 2)) // list item?
    {
	long n = (long) luaL_checkinteger(L, 2);
	listitem_T *li;

	// Lua array index starts with 1 while Vim uses 0, subtract 1 to
	// normalize.
	n -= 1;
	li = list_find(l, n);
	जइ (li == रिक्त)
	    lua_pushnil(L);
	अण्णहा
	    luaV_pushtypval(L, &li->li_tv);
    }
    अण्णहा जइ (lua_isstring(L, 2)) // method?
    {
	ध्रुव वर्ण *s = lua_tostring(L, 2);
	जइ (strncmp(s, "add", 3) == 0
		|| strncmp(s, "insert", 6) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	अण्णहा
	    lua_pushnil(L);
    }
    अण्णहा
	lua_pushnil(L);
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_list_newindex(lua_State *L)
{
    list_T *l = luaV_unbox(L, luaV_List, 1);
    long n = (long) luaL_checkinteger(L, 2);
    listitem_T *li;

    // Lua array index starts with 1 while Vim uses 0, subtract 1 to normalize.
    n -= 1;

    जइ (l->lv_lock)
	luaL_error(L, "list is locked");
    li = list_find(l, n);
    जइ (li == रिक्त)
    {
	जइ (!lua_isnil(L, 3))
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "inserting list item");
	    जइ (list_निवेशय_tv(l, &v, li) == FAIL)
		luaL_error(L, "failed to add item to list");
	    clear_tv(&v);
	}
    }
    अण्णहा
    {
	जइ (lua_isnil(L, 3)) // remove?
	{
	    vimlist_remove(l, li, li);
	    listitem_free(l, li);
	}
	अण्णहा
	{
	    typval_T v;
	    luaV_checktypval(L, 3, &v, "setting list item");
	    clear_tv(&li->li_tv);
	    li->li_tv = v;
	}
    }
    प्रत्यागम 0;
}

    स्थिर पूर्णाङ्क
luaV_list_add(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (शून्य *) *lis);
    typval_T v;
    जइ (l->lv_lock)
	luaL_error(L, "list is locked");
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "adding list item");
    जइ (list_append_tv(l, &v) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
lua_V_list_निवेशय(lua_State *L)
{
    luaV_List *lis = luaV_checkudata(L, 1, LUAVIM_LIST);
    list_T *l = (list_T *) luaV_checkcache(L, (शून्य *) *lis);
    long pos = (long) luaL_optinteger(L, 3, 0);
    listitem_T *li = रिक्त;
    typval_T v;
    जइ (l->lv_lock)
	luaL_error(L, "list is locked");
    जइ (pos < l->lv_len)
    {
	li = list_find(l, pos);
	जइ (li == रिक्त)
	    luaL_error(L, "invalid position");
    }
    lua_settop(L, 2);
    luaV_checktypval(L, 2, &v, "inserting list item");
    जइ (list_निवेशय_tv(l, &v, li) == FAIL)
	luaL_error(L, "failed to add item to list");
    clear_tv(&v);
    lua_settop(L, 1);
    प्रत्यागम 1;
}

स्थिर ध्रुव luaL_Reg luaV_List_mt[] = {
    {"__tostring", luaV_list_tostring},
    {"__len", luaV_list_len},
    {"__call", luaV_list_call},
    {"__index", luaV_list_index},
    {"__newindex", luaV_list_newindex},
    {"add", luaV_list_add},
    {"insert", lua_V_list_निवेशय},
    {रिक्त, रिक्त}
};


// =======   Dict type   =======

    स्थिर luaV_Dict *
luaV_newdict(lua_State *L, dict_T *dic)
{
    luaV_Dict *d = (luaV_Dict *) lua_newuserdata(L, परिमाण(luaV_Dict));
    *d = dic;
    dic->dv_refcount++; // reference in Lua
    luaV_setudata(L, dic); // cache[dic] = udata
    luaV_getfield(L, LUAVIM_DICT);
    lua_setmetatable(L, -2);
    प्रत्यागम d;
}

luaV_pushtype(dict_T, dict, luaV_Dict)
luaV_type_tostring(dict, LUAVIM_DICT)

    स्थिर पूर्णाङ्क
luaV_dict_len(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    lua_pushinteger(L, (पूर्णाङ्क) dict_len(d));
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_dict_iter(lua_State *L UNUSED)
{
# ifdef FEAT_EVAL
    hashitem_T *hi = (hashitem_T *) lua_touserdata(L, lua_upvalueindex(1));
    पूर्णाङ्क n = lua_tointeger(L, lua_upvalueindex(2));
    dictitem_T *di;
    जइ (n <= 0) प्रत्यागम 0;
    जाव (HASHITEM_EMPTY(hi)) hi++;
    di = dict_lookup(hi);
    lua_pushstring(L, (वर्ण *) hi->hi_कुञ्चिका);
    luaV_pushtypval(L, &di->di_tv);
    lua_pushlightuserdata(L, (शून्य *) (hi + 1));
    lua_replace(L, lua_upvalueindex(1));
    lua_pushinteger(L, n - 1);
    lua_replace(L, lua_upvalueindex(2));
    प्रत्यागम 2;
# अण्णहा
    प्रत्यागम 0;
# endif
}

    स्थिर पूर्णाङ्क
luaV_dict_call(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    hashtab_T *ht = &d->dv_hashtab;
    lua_pushlightuserdata(L, (शून्य *) ht->ht_array);
    lua_pushinteger(L, ht->ht_used); // # remaining items
    lua_pushcclosure(L, luaV_dict_iter, 2);
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_dict_index(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *कुञ्चिका = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di = dict_find(d, कुञ्चिका, -1);

    जइ (di == रिक्त)
    {
	lua_pushnil(L);
	प्रत्यागम 1;
    }

    luaV_pushtypval(L, &di->di_tv);
    जइ (di->di_tv.v_type == VAR_FUNC) // funcref?
    {
	luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, -1);
	f->self = d; // keep "self" reference
	d->dv_refcount++;
    }

    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_dict_newindex(lua_State *L)
{
    dict_T *d = luaV_unbox(L, luaV_Dict, 1);
    char_u *कुञ्चिका = (char_u *) luaL_checkstring(L, 2);
    dictitem_T *di;
    typval_T tv;

    जइ (d->dv_lock)
	luaL_error(L, "dict is locked");
    जइ (कुञ्चिका == रिक्त)
	प्रत्यागम 0;
    जइ (*कुञ्चिका == NUL)
	luaL_error(L, "empty key");
    जइ (!lua_isnil(L, 3)) // read value?
    {
	luaV_checktypval(L, 3, &tv, "setting dict item");
	जइ (d->dv_scope == VAR_DEF_SCOPE && tv.v_type == VAR_FUNC)
	{
	    clear_tv(&tv);
	    luaL_error(L, "cannot assign funcref to builtin scope");
	}
    }
    di = dict_find(d, कुञ्चिका, -1);
    जइ (di == रिक्त) // non-existing key?
    {
	जइ (lua_isnil(L, 3))
	    प्रत्यागम 0;
	di = dictitem_alloc(कुञ्चिका);
	जइ (di == रिक्त)
	{
	    clear_tv(&tv);
	    प्रत्यागम 0;
	}
	जइ (dict_add(d, di) == FAIL)
	{
	    vim_free(di);
	    clear_tv(&tv);
	    प्रत्यागम 0;
	}
    }
    अण्णहा
	clear_tv(&di->di_tv);
    जइ (lua_isnil(L, 3)) // remove?
    {
	hashitem_T *hi = hash_find(&d->dv_hashtab, di->di_कुञ्चिका);
	hash_remove(&d->dv_hashtab, hi, "Lua new index");
	dictitem_free(di);
    }
    अण्णहा
	di->di_tv = tv;
    प्रत्यागम 0;
}

स्थिर ध्रुव luaL_Reg luaV_Dict_mt[] = {
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
    luaV_Blob *b = (luaV_Blob *) lua_newuserdata(L, परिमाण(luaV_Blob));
    *b = blo;
    blo->bv_refcount++; // reference in Lua
    luaV_setudata(L, blo); // cache[blo] = udata
    luaV_getfield(L, LUAVIM_BLOB);
    lua_setmetatable(L, -2);
    प्रत्यागम b;
}

luaV_pushtype(blob_T, blob, luaV_Blob)
luaV_type_tostring(blob, LUAVIM_BLOB)

    स्थिर पूर्णाङ्क
luaV_blob_gc(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    blob_unref(b);
    प्रत्यागम 0;
}

    स्थिर पूर्णाङ्क
luaV_blob_len(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    lua_pushinteger(L, (पूर्णाङ्क) blob_len(b));
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_blob_index(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    जइ (lua_isnumber(L, 2))
    {
	पूर्णाङ्क idx = luaL_checkinteger(L, 2);
	जइ (idx < 0)
	    // Negative index is relative to the end.
	    idx += (पूर्णाङ्क) blob_len(b);
	जइ (idx >= 0 && idx < blob_len(b))
	    lua_pushnumber(L, (lua_सङ्ख्या) blob_get(b, idx));
	अण्णहा
	    lua_pushnil(L);
    }
    अण्णहा जइ (lua_isstring(L, 2))
    {
	ध्रुव वर्ण *s = lua_tostring(L, 2);
	जइ (strncmp(s, "add", 3) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	अण्णहा
	    lua_pushnil(L);
    }
    अण्णहा
	lua_pushnil(L);
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_blob_newindex(lua_State *L)
{
    blob_T *b = luaV_unbox(L, luaV_Blob, 1);
    जइ (b->bv_lock)
	luaL_error(L, "blob is locked");

    जइ (!lua_isnumber(L, 2))
	प्रत्यागम 0;

    long len = blob_len(b);
    पूर्णाङ्क idx = luaL_checkinteger(L, 2);
    पूर्णाङ्क val = luaL_checkinteger(L, 3);
    जइ (idx < 0)
	// Negative index is relative to the end.
	idx += (पूर्णाङ्क) len;
    जइ ((idx >= 0 && idx < len)
	    || (idx == len && ga_grow(&b->bv_ga, 1) == OK))
    {
	blob_set(b, idx, (char_u) val);
	जइ (idx == len)
	    ++b->bv_ga.ga_len;
    }
    अण्णहा
	luaL_error(L, "index out of range");

    प्रत्यागम 0;
}

    स्थिर पूर्णाङ्क
luaV_blob_add(lua_State *L)
{
    luaV_Blob *blo = luaV_checkudata(L, 1, LUAVIM_BLOB);
    blob_T *b = (blob_T *) luaV_checkcache(L, (शून्य *) *blo);
    जइ (b->bv_lock)
	luaL_error(L, "blob is locked");
    lua_settop(L, 2);
    जइ (!lua_isstring(L, 2))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 2));
    अण्णहा
    {
	size_t i, l = 0;
	ध्रुव वर्ण *s = lua_tolstring(L, 2, &l);

	जइ (ga_grow(&b->bv_ga, (पूर्णाङ्क)l) == OK)
	    प्रति (i = 0; i < l; ++i)
		ga_append(&b->bv_ga, s[i]);
    }
    lua_settop(L, 1);
    प्रत्यागम 1;
}

स्थिर ध्रुव luaL_Reg luaV_Blob_mt[] = {
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
    luaV_Funcref *f = (luaV_Funcref *)lua_newuserdata(L, परिमाण(luaV_Funcref));

    जइ (name != रिक्त)
    {
	func_ref(name);
	f->name = vim_strsave(name);
    }
    f->self = रिक्त;
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_setmetatable(L, -2);
    प्रत्यागम f;
}

    स्थिर luaV_Funcref *
luaV_pushfuncref(lua_State *L, char_u *name)
{
    प्रत्यागम luaV_newfuncref(L, name);
}


luaV_type_tostring(funcref, LUAVIM_FUNCREF)

    स्थिर पूर्णाङ्क
luaV_funcref_gc(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    func_unref(f->name);
    vim_free(f->name);
    // NOTE: Don't call "dict_unref(f->self)", because the dict of "f->self"
    // will be (or has been already) freed by Vim's garbage collection.
    प्रत्यागम 0;
}

// equivalent to string(funcref)
    स्थिर पूर्णाङ्क
luaV_funcref_len(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);

    lua_pushstring(L, (ध्रुव वर्ण *) f->name);
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_funcref_call(lua_State *L)
{
    luaV_Funcref *f = (luaV_Funcref *) lua_touserdata(L, 1);
    पूर्णाङ्क i, n = lua_gettop(L) - 1; // #args
    पूर्णाङ्क status = FAIL;
    typval_T args;
    typval_T rettv;

    args.v_type = VAR_LIST;
    args.vval.v_list = list_alloc();
    rettv.v_type = VAR_UNKNOWN; // as in clear_tv
    जइ (args.vval.v_list != रिक्त)
    {
	typval_T v;

	प्रति (i = 0; i < n; i++)
	{
	    luaV_checktypval(L, i + 2, &v, "calling funcref");
	    list_append_tv(args.vval.v_list, &v);
	    clear_tv(&v);
	}
	status = func_call(f->name, &args, रिक्त, f->self, &rettv);
	जइ (status == OK)
	    luaV_pushtypval(L, &rettv);
	clear_tv(&args);
	clear_tv(&rettv);
    }
    जइ (status != OK)
	luaL_error(L, "cannot call funcref");
    प्रत्यागम 1;
}

स्थिर ध्रुव luaL_Reg luaV_Funcref_mt[] = {
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

    स्थिर पूर्णाङ्क
luaV_buffer_len(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    lua_pushinteger(L, b->b_ml.ml_पङ्क्तिः_गणनम्);
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_buffer_call(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    lua_settop(L, 1);
    set_curbuf(b, DOBUF_SPLIT);
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_buffer_index(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    linenr_T n = (linenr_T) lua_tointeger(L, 2);
    जइ (n > 0 && n <= b->b_ml.ml_पङ्क्तिः_गणनम्)
	luaV_pushline(L, b, n);
    अण्णहा जइ (lua_isstring(L, 2))
    {
	ध्रुव वर्ण *s = lua_tostring(L, 2);
	जइ (strncmp(s, "name", 4) == 0)
	    lua_pushstring(L, (b->b_sfname == रिक्त)
					? "" : (वर्ण *) b->b_sfname);
	अण्णहा जइ (strncmp(s, "fname", 5) == 0)
	    lua_pushstring(L, (b->b_ffname == रिक्त)
					? "" : (वर्ण *) b->b_ffname);
	अण्णहा जइ (strncmp(s, "number", 6) == 0)
	    lua_pushinteger(L, b->b_fnum);
	// methods
	अण्णहा जइ (strncmp(s,   "insert", 6) == 0
		|| strncmp(s, "next", 4) == 0
		|| strncmp(s, "previous", 8) == 0
		|| strncmp(s, "isvalid", 7) == 0)
	{
	    lua_getmetatable(L, 1);
	    lua_getfield(L, -1, s);
	}
	अण्णहा
	    lua_pushnil(L);
    }
    अण्णहा
	lua_pushnil(L);
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_buffer_newindex(lua_State *L)
{
    buf_T *b = (buf_T *) luaV_checkvalid(L, luaV_Buffer, 1);
    linenr_T n = (linenr_T) luaL_checkinteger(L, 2);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    जइ (n < 1 || n > b->b_ml.ml_पङ्क्तिः_गणनम्)
	luaL_error(L, "invalid line number");
    जइ (lua_isnil(L, 3)) // delete line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	जइ (u_savedel(n, 1L) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	अण्णहा जइ (ml_delete(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot delete line");
	}
	अण्णहा
	{
	    deleted_पङ्क्तयः_mark(n, 1L);
	    जइ (b == curwin->w_buffer) // fix cursor in current window?
	    {
		जइ (curwin->w_cursor.lnum >= n)
		{
		    जइ (curwin->w_cursor.lnum > n)
		    {
			curwin->w_cursor.lnum -= 1;
			check_cursor_col();
		    }
		    अण्णहा
			check_cursor();
		    changed_cline_bef_curs();
		}
		invalidate_botline();
	    }
	}
	curbuf = buf;
    }
    अण्णहा जइ (lua_isstring(L, 3)) // update line
    {
	buf_T *buf = curbuf;
	curbuf = b;
	जइ (u_savesub(n) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot save undo information");
	}
	अण्णहा जइ (ml_replace(n, luaV_toline(L, 3), TRUE) == FAIL)
	{
	    curbuf = buf;
	    luaL_error(L, "cannot replace line");
	}
	अण्णहा
	    changed_bytes(n, 0);
	curbuf = buf;
	जइ (b == curwin->w_buffer)
	    check_cursor_col();
    }
    अण्णहा
	luaL_error(L, "wrong argument to change line");
    प्रत्यागम 0;
}

    स्थिर पूर्णाङ्क
lua_V_buffer_निवेशय(lua_State *L)
{
    luaV_Buffer *lb = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *b = (buf_T *) luaV_checkcache(L, (शून्य *) *lb);
    linenr_T last = b->b_ml.ml_पङ्क्तिः_गणनम्;
    linenr_T n = (linenr_T) luaL_optinteger(L, 3, last);
    buf_T *buf;
    luaL_checktype(L, 2, LUA_TSTRING);
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    // fix insertion line
    जइ (n < 0) n = 0;
    जइ (n > last) n = last;
    // insert
    buf = curbuf;
    curbuf = b;
    जइ (u_save(n, n + 1) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot save undo information");
    }
    अण्णहा जइ (ml_append(n, luaV_toline(L, 2), 0, FALSE) == FAIL)
    {
	curbuf = buf;
	luaL_error(L, "cannot insert line");
    }
    अण्णहा
	appended_पङ्क्तयः_mark(n, 1L);
    curbuf = buf;
    update_screen(UPD_VALID);
    प्रत्यागम 0;
}

    स्थिर पूर्णाङ्क
luaV_buffer_next(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (शून्य *) *b);
    luaV_pushbuffer(L, buf->b_next);
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_buffer_previous(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    buf_T *buf = (buf_T *) luaV_checkcache(L, (शून्य *) *b);
    luaV_pushbuffer(L, buf->b_prev);
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_buffer_isvalid(lua_State *L)
{
    luaV_Buffer *b = luaV_checkudata(L, 1, LUAVIM_BUFFER);
    luaV_getudata(L, *b);
    lua_pushboolean(L, !lua_isnil(L, -1));
    प्रत्यागम 1;
}

स्थिर ध्रुव luaL_Reg luaV_Buffer_mt[] = {
    {"__tostring", luaV_buffer_tostring},
    {"__len", luaV_buffer_len},
    {"__call", luaV_buffer_call},
    {"__index", luaV_buffer_index},
    {"__newindex", luaV_buffer_newindex},
    {"insert", lua_V_buffer_निवेशय},
    {"next", luaV_buffer_next},
    {"previous", luaV_buffer_previous},
    {"isvalid", luaV_buffer_isvalid},
    {रिक्त, रिक्त}
};


// =======   Window type   =======

luaV_newtype(win_T, window, luaV_Window, LUAVIM_WINDOW)
luaV_pushtype(win_T, window, luaV_Window)
luaV_type_tostring(window, LUAVIM_WINDOW)

    स्थिर पूर्णाङ्क
luaV_window_call(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    lua_settop(L, 1);
    win_goto(w);
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_window_index(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    ध्रुव वर्ण *s = luaL_checkstring(L, 2);
    जइ (strncmp(s, "buffer", 6) == 0)
	luaV_pushbuffer(L, w->w_buffer);
    अण्णहा जइ (strncmp(s, "line", 4) == 0)
	lua_pushinteger(L, w->w_cursor.lnum);
    अण्णहा जइ (strncmp(s, "col", 3) == 0)
	lua_pushinteger(L, w->w_cursor.col + 1);
    अण्णहा जइ (strncmp(s, "width", 5) == 0)
	lua_pushinteger(L, w->w_width);
    अण्णहा जइ (strncmp(s, "height", 6) == 0)
	lua_pushinteger(L, w->w_height);
    // methods
    अण्णहा जइ (strncmp(s,   "next", 4) == 0
	    || strncmp(s, "previous", 8) == 0
	    || strncmp(s, "isvalid", 7) == 0)
    {
	lua_getmetatable(L, 1);
	lua_getfield(L, -1, s);
    }
    अण्णहा
	lua_pushnil(L);
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_window_newindex(lua_State *L)
{
    win_T *w = (win_T *) luaV_checkvalid(L, luaV_Window, 1);
    ध्रुव वर्ण *s = luaL_checkstring(L, 2);
    पूर्णाङ्क v = luaL_checkinteger(L, 3);
    जइ (strncmp(s, "line", 4) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	जइ (v < 1 || v > w->w_buffer->b_ml.ml_पङ्क्तिः_गणनम्)
	    luaL_error(L, "line out of range");
	w->w_cursor.lnum = v;
	update_screen(UPD_VALID);
    }
    अण्णहा जइ (strncmp(s, "col", 3) == 0)
    {
# ifdef HAVE_SANDBOX
	luaV_checksandbox(L);
# endif
	w->w_cursor.col = v - 1;
	w->w_set_curswant = true;
	update_screen(UPD_VALID);
    }
    अण्णहा जइ (strncmp(s, "width", 5) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setwidth(v);
	curwin = win;
    }
    अण्णहा जइ (strncmp(s, "height", 6) == 0)
    {
	win_T *win = curwin;
# ifdef FEAT_GUI
	need_mouse_correct = TRUE;
# endif
	curwin = w;
	win_setheight(v);
	curwin = win;
    }
    अण्णहा
	luaL_error(L, "invalid window property: `%s'", s);
    प्रत्यागम 0;
}

    स्थिर पूर्णाङ्क
luaV_window_next(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    win_T *win = (win_T *) luaV_checkcache(L, (शून्य *) *w);
    luaV_pushwindow(L, win->w_next);
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_window_previous(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    win_T *win = (win_T *) luaV_checkcache(L, (शून्य *) *w);
    luaV_pushwindow(L, win->w_prev);
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_window_isvalid(lua_State *L)
{
    luaV_Window *w = luaV_checkudata(L, 1, LUAVIM_WINDOW);
    luaV_getudata(L, *w);
    lua_pushboolean(L, !lua_isnil(L, -1));
    प्रत्यागम 1;
}

स्थिर ध्रुव luaL_Reg luaV_Window_mt[] = {
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

    स्थिर पूर्णाङ्क
luaV_print(lua_State *L)
{
    पूर्णाङ्क		i, n = lua_gettop(L); // nargs
    ध्रुव वर्ण	*s;
    size_t	l;
    garray_T	सन्देशः_ga;

    ga_init2(&सन्देशः_ga, 1, 128);
    lua_getglobal(L, "tostring");
    प्रति (i = 1; i <= n; i++)
    {
	lua_pushvalue(L, -1); // tostring
	lua_pushvalue(L, i); // arg
	lua_call(L, 1, 1);
	s = lua_tolstring(L, -1, &l);
	जइ (s == रिक्त)
	    प्रत्यागम luaL_error(L, "cannot convert to string");
	जइ (i > 1)
	    ga_append(&सन्देशः_ga, ' '); // use space instead of tab
	ga_concat_len(&सन्देशः_ga, (char_u *)s, l);
	lua_निष्कासय(L, 1);
    }
    // Replace any "\n" with "\0"
    प्रति (i = 0; i < सन्देशः_ga.ga_len; i++)
	जइ (((वर्ण *)सन्देशः_ga.ga_data)[i] == '\n')
	    ((वर्ण *)सन्देशः_ga.ga_data)[i] = '\0';
    lua_pushlstring(L, सन्देशः_ga.ga_data, सन्देशः_ga.ga_len);
    जइ (!got_int)
	lua_V_सन्देशः(L);

    ga_clear(&सन्देशः_ga);
    प्रत्यागम 0;
}

    स्थिर पूर्णाङ्क
luaV_debug(lua_State *L)
{
    lua_settop(L, 0);
    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "eval");
    lua_remove(L, -2); // vim.eval at position 1
    प्रति (;;)
    {
	ध्रुव वर्ण *निवेशः;
	size_t l;
	lua_pushvalue(L, 1); // vim.eval
	lua_pushliteral(L, "input('lua_debug> ')");
	lua_call(L, 1, 1); // return string
	निवेशः = lua_tolstring(L, -1, &l);
	जइ (l == 0 || strcmp(निवेशः, "cont") == 0)
	    प्रत्यागम 0;
	सन्देशः_putchar('\n'); // avoid outputting on input line
	जइ (luaL_loadbuffer(L, निवेशः, l, "=(debug command)")
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
	lua_settop(L, 1); // remove eventual returns, but keep vim.eval
    }
}

    स्थिर dict_T *
luaV_get_var_scope(lua_State *L)
{
    ध्रुव वर्ण	*scope = luaL_checkstring(L, 1);
    dict_T	*dict = रिक्त;

    जइ (STRICMP((वर्ण *)scope, "g") == 0)
	dict = get_globvar_dict();
    अण्णहा जइ (STRICMP((वर्ण *)scope, "v") == 0)
	dict = get_vimvar_dict();
    अण्णहा जइ (STRICMP((वर्ण *)scope, "b") == 0)
	dict = curbuf->b_vars;
    अण्णहा जइ (STRICMP((वर्ण *)scope, "w") == 0)
	dict = curwin->w_vars;
    अण्णहा जइ (STRICMP((वर्ण *)scope, "t") == 0)
	dict = curtab->tp_vars;
    अण्णहा
    {
	luaL_error(L, "invalid scope %s", scope);
	प्रत्यागम रिक्त;
    }

    प्रत्यागम dict;
}

    स्थिर पूर्णाङ्क
luaV_setvar(lua_State *L)
{
    dict_T	*dict;
    dictitem_T	*di;
    size_t	len;
    वर्ण	*name;
    पूर्णाङ्क		del;
    वर्ण	*error = रिक्त;

    name = (वर्ण *)luaL_checklstring(L, 3, &len);
    del = (lua_gettop(L) < 4) || lua_isnil(L, 4);

    dict = luaV_get_var_scope(L);
    जइ (dict == रिक्त)
	प्रत्यागम 0;

    di = dict_find(dict, (char_u *)name, (पूर्णाङ्क)len);
    जइ (di != रिक्त)
    {
	जइ (di->di_flags & DI_FLAGS_RO)
	    error = "variable is read-only";
	अण्णहा जइ (di->di_flags & DI_FLAGS_LOCK)
	    error = "variable is locked";
	अण्णहा जइ (del && di->di_flags & DI_FLAGS_FIX)
	    error = "variable is fixed";
	जइ (error != रिक्त)
	    प्रत्यागम luaL_error(L, error);
    }
    अण्णहा जइ (dict->dv_lock)
	प्रत्यागम luaL_error(L, "Dictionary is locked");

    जइ (del)
    {
	// Delete the key
	जइ (di == रिक्त)
	    // Doesn't exist, nothing to do
	    प्रत्यागम 0;
	// Delete the entry
	dictitem_remove(dict, di, "Lua delete variable");
    }
    अण्णहा
    {
	// Update the key
	typval_T	tv;

	// Convert the lua value to a Vim script type in the temporary variable
	lua_pushvalue(L, 4);
	जइ (luaV_totypval(L, -1, &tv) == FAIL)
	    प्रत्यागम luaL_error(L, "Couldn't convert lua value");

	जइ (di == रिक्त)
	{
	    // Need to create an entry
	    di = dictitem_alloc((char_u *)name);
	    जइ (di == रिक्त)
	    {
		clear_tv(&tv);
		प्रत्यागम 0;
	    }
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	    जइ (dict_add(dict, di) == FAIL)
	    {
		dictitem_free(di);
		clear_tv(&tv);
		प्रत्यागम luaL_error(L, "Couldn't add to dictionary");
	    }
	}
	अण्णहा
	{
	    पूर्णाङ्क type_error = FALSE;
	    जइ (dict == get_vimvar_dict()
	       && !before_set_vvar((char_u *)name, di, &tv, TRUE, &type_error))
	    {
		clear_tv(&tv);
		जइ (type_error)
		    प्रत्यागम luaL_error(L,
				"Setting v:%s to value with wrong type", name);
		प्रत्यागम 0;
	    }
	    // Clear the old value
	    clear_tv(&di->di_tv);
	    // Update the value
	    copy_tv(&tv, &di->di_tv);
	}

	// Clear the temporary variable
	clear_tv(&tv);
    }

    प्रत्यागम 0;
}

    स्थिर पूर्णाङ्क
luaV_getvar(lua_State *L)
{
    dict_T	*dict = luaV_get_var_scope(L);
    size_t	len;
    ध्रुव वर्ण	*name = luaL_checklstring(L, 3, &len);
    dictitem_T	*di = dict_find(dict, (char_u *)name, (पूर्णाङ्क)len);

    जइ (di == रिक्त)
	प्रत्यागम 0;  // nil

    luaV_pushtypval(L, &di->di_tv);
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_command(lua_State *L)
{
    char_u  *s = vim_strsave((char_u *)luaL_checkstring(L, 1));

    execute_cmds_from_string(s);
    vim_free(s);
    update_screen(UPD_VALID);
    प्रत्यागम 0;
}

    स्थिर पूर्णाङ्क
luaV_eval(lua_State *L)
{
    typval_T *tv = eval_व्यञ्जकम्((char_u *) luaL_checkstring(L, 1), रिक्त);

    जइ (tv == रिक्त) luaL_error(L, "invalid expression");
    luaV_pushtypval(L, tv);
    free_tv(tv);
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_beep(lua_State *L UNUSED)
{
    vim_beep(BO_LANG);
    प्रत्यागम 0;
}

    स्थिर पूर्णाङ्क
lua_V_पङ्क्तिः(lua_State *L)
{
    luaV_pushline(L, curbuf, curwin->w_cursor.lnum);
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_list(lua_State *L)
{
    list_T *l;
    पूर्णाङ्क initarg = !lua_isnoneornil(L, 1);

    जइ (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    l = list_alloc();
    जइ (l == रिक्त)
    {
	lua_pushnil(L);
	प्रत्यागम 1;
    }

    luaV_newlist(L, l);
    जइ (!initarg)
	प्रत्यागम 1;

    // traverse table to init list
    पूर्णाङ्क notnil, i = 0;
    typval_T v;
    करेहि
    {
	lua_rawgeti(L, 1, ++i);
	notnil = !lua_isnil(L, -1);
	जइ (notnil)
	{
	    luaV_checktypval(L, -1, &v, "vim.list");
	    list_append_tv(l, &v);
	    clear_tv(&v);
	}
	lua_निष्कासय(L, 1); //मानम्
    } जाव (notnil);

    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_dict(lua_State *L)
{
    dict_T *d;
    पूर्णाङ्क initarg = !lua_isnoneornil(L, 1);

    जइ (initarg && lua_type(L, 1) != LUA_TTABLE)
	luaL_error(L, "table expected, got %s", luaL_typename(L, 1));

    d = dict_alloc();
    जइ (d == रिक्त)
    {
	lua_pushnil(L);
	प्रत्यागम 1;
    }

    luaV_newdict(L, d);
    जइ (!initarg)
	प्रत्यागम 1;

    // traverse table to init dict
    lua_pushnil(L);
    जाव (lua_next(L, 1))
    {
	char_u *कुञ्चिका;
	dictitem_T *di;
	typval_T v;

	lua_pushvalue(L, -2); // dup key in case it's a number
	कुञ्चिका = (char_u *) lua_tostring(L, -1);
	जइ (कुञ्चिका == रिक्त)
	{
	    lua_pushnil(L);
	    प्रत्यागम 1;
	}
	जइ (*कुञ्चिका == NUL)
	    luaL_error(L, "table has empty key");
	luaV_checktypval(L, -2, &v, "vim.dict"); //मानम्
	di = dictitem_alloc(कुञ्चिका);
	जइ (di == रिक्त || dict_add(d, di) == FAIL)
	{
	    vim_free(di);
	    lua_pushnil(L);
	    प्रत्यागम 1;
	}
	di->di_tv = v;
	lua_निष्कासय(L, 2); // key copy and value
    }

    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_blob(lua_State *L)
{
    blob_T *b;
    पूर्णाङ्क initarg = !lua_isnoneornil(L, 1);

    जइ (initarg && !lua_isstring(L, 1))
	luaL_error(L, "string expected, got %s", luaL_typename(L, 1));

    b = blob_alloc();
    जइ (b == रिक्त)
    {
	lua_pushnil(L);
	प्रत्यागम 1;
    }

    luaV_newblob(L, b);
    जइ (!initarg)
	प्रत्यागम 1;

    // traverse table to init blob
    size_t i, l = 0;
    ध्रुव वर्ण *s = lua_tolstring(L, 1, &l);

    जइ (ga_grow(&b->bv_ga, (पूर्णाङ्क)l) == OK)
	प्रति (i = 0; i < l; ++i)
	    ga_append(&b->bv_ga, s[i]);

    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_funcref(lua_State *L)
{
    ध्रुव वर्ण *name = luaL_checkstring(L, 1);
    // note: not checking if function exists (needs function_exists)
    जइ (name == रिक्त || *name == NUL || VIM_ISDIGIT(*name))
	luaL_error(L, "invalid function name: %s", name);
    luaV_newfuncref(L, (char_u *) name);
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_buffer(lua_State *L)
{
    buf_T *buf;
    जइ (lua_isstring(L, 1)) // get by number or name?
    {
	जइ (lua_isnumber(L, 1)) // by number?
	{
	    पूर्णाङ्क n = lua_tointeger(L, 1);
	    FOR_ALL_BUFFERS(buf)
		जइ (buf->b_fnum == n) भङ्ग;
	}
	अण्णहा // by name
	{
	    size_t l;
	    ध्रुव वर्ण *s = lua_tolstring(L, 1, &l);
	    FOR_ALL_BUFFERS(buf)
	    {
		जइ (buf->b_ffname == रिक्त || buf->b_sfname == रिक्त)
		{
		    जइ (l == 0) भङ्ग;
		}
		अण्णहा जइ (strncmp(s, (वर्ण *)buf->b_ffname, l) == 0
			|| strncmp(s, (वर्ण *)buf->b_sfname, l) == 0)
		    भङ्ग;
	    }
	}
    }
    अण्णहा
	buf = (lua_toboolean(L, 1)) ? firstbuf : curbuf; // first buffer?
    luaV_pushbuffer(L, buf);
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_window(lua_State *L)
{
    win_T *win;
    जइ (lua_isnumber(L, 1)) // get by number?
    {
	पूर्णाङ्क n = lua_tointeger(L, 1);
	प्रति (win = firstwin; win != रिक्त; win = win->w_next, n--)
	    जइ (n == 1) भङ्ग;
    }
    अण्णहा
	win = (lua_toboolean(L, 1)) ? firstwin : curwin; // first window?
    luaV_pushwindow(L, win);
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_open(lua_State *L)
{
    char_u *s = रिक्त;
# ifdef HAVE_SANDBOX
    luaV_checksandbox(L);
# endif
    जइ (lua_isstring(L, 1)) s = (char_u *) lua_tostring(L, 1);
    luaV_pushbuffer(L, buflist_new(s, रिक्त, 1L, BLN_LISTED));
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_type(lua_State *L)
{
    luaL_checkany(L, 1);
    जइ (lua_type(L, 1) == LUA_TUSERDATA) // check vim udata?
    {
	lua_settop(L, 1);
	जइ (lua_getmetatable(L, 1))
	{
	    luaV_getfield(L, LUAVIM_LIST);
	    जइ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "list");
		प्रत्यागम 1;
	    }
	    luaV_getfield(L, LUAVIM_DICT);
	    जइ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "dict");
		प्रत्यागम 1;
	    }
	    luaV_getfield(L, LUAVIM_BLOB);
	    जइ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "blob");
		प्रत्यागम 1;
	    }
	    luaV_getfield(L, LUAVIM_FUNCREF);
	    जइ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "funcref");
		प्रत्यागम 1;
	    }
	    luaV_getfield(L, LUAVIM_BUFFER);
	    जइ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "buffer");
		प्रत्यागम 1;
	    }
	    luaV_getfield(L, LUAVIM_WINDOW);
	    जइ (lua_rawequal(L, -1, 2))
	    {
		lua_pushstring(L, "window");
		प्रत्यागम 1;
	    }
	}
    }
    lua_pushstring(L, luaL_typename(L, 1)); // fallback
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_call(lua_State *L)
{
    पूर्णाङ्क		argc = lua_gettop(L) - 1;
    size_t	funcname_len;
    char_u	*funcname;
    वर्ण	*error = रिक्त;
    typval_T	rettv;
    typval_T	argv[MAX_FUNC_ARGS + 1];
    पूर्णाङ्क		i = 0;

    जइ (argc > MAX_FUNC_ARGS)
	प्रत्यागम luaL_error(L, "Function called with too many arguments");

    funcname = (char_u *)luaL_checklstring(L, 1, &funcname_len);

    प्रति (; i < argc; i++)
    {
	जइ (luaV_totypval(L, i + 2, &argv[i]) == FAIL)
	{
	    error = "lua: cannot convert value";
	    गच्छ free_vim_args;
	}
    }

    argv[argc].v_type = VAR_UNKNOWN;

    जइ (call_vim_function(funcname, argc, argv, &rettv) == FAIL)
    {
	error = "lua: call_vim_function failed";
	गच्छ free_vim_args;
    }

    luaV_pushtypval(L, &rettv);
    clear_tv(&rettv);

free_vim_args:
    जाव (i > 0)
	clear_tv(&argv[--i]);

    जइ (error == रिक्त)
	प्रत्यागम 1;
    अण्णहा
	प्रत्यागम luaL_error(L, error);
}

/*
 * Return the Vim version as a Lua table
 */
    स्थिर पूर्णाङ्क
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
    प्रत्यागम 1;
}

स्थिर ध्रुव luaL_Reg luaV_module[] = {
    {"command", luaV_command},
    {"eval", luaV_eval},
    {"beep", luaV_beep},
    {"line", lua_V_पङ्क्तिः},
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
    स्थिर पूर्णाङ्क
luaV_free(lua_State *L)
{
    lua_pushnil(L);
    luaV_setudata(L, lua_touserdata(L, 1));
    प्रत्यागम 0;
}

    स्थिर पूर्णाङ्क
luaV_luaeval(lua_State *L)
{
    luaL_Buffer b;
    size_t l;
    ध्रुव वर्ण *str = lua_tolstring(L, 1, &l);
    typval_T *arg = (typval_T *) lua_touserdata(L, 2);
    typval_T *rettv = (typval_T *) lua_touserdata(L, 3);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, LUAVIM_EVALHEADER, परिमाण(LUAVIM_EVALHEADER) - 1);
    luaL_addlstring(&b, str, l);
    luaL_pushresult(&b);
    str = lua_tolstring(L, -1, &l);
    जइ (luaL_loadbuffer(L, str, l, LUAVIM_EVALNAME)) // compile error?
    {
	luaV_emsg(L);
	प्रत्यागम 0;
    }
    luaV_pushtypval(L, arg);
    जइ (lua_pcall(L, 1, 1, 0)) // running error?
    {
	luaV_emsg(L);
	प्रत्यागम 0;
    }
    जइ (luaV_totypval(L, -1, rettv) == FAIL)
	emsg("luaeval: cannot convert value");
    प्रत्यागम 0;
}

    स्थिर पूर्णाङ्क
luaV_setref(lua_State *L)
{
    पूर्णाङ्क copyID = lua_tointeger(L, 1);
    पूर्णाङ्क abort = FALSE;

    lua_pushlightuserdata(L, (शून्य *) LUAVIM_UDATA_CACHE);
    lua_rawget(L, LUA_REGISTRYINDEX);  // the cache table

    luaV_getfield(L, LUAVIM_LIST);
    luaV_getfield(L, LUAVIM_DICT);
    luaV_getfield(L, LUAVIM_FUNCREF);
    lua_pushnil(L);
    // traverse cache table
    जाव (!abort && lua_next(L, 2) != 0)
    {
	lua_getmetatable(L, -1);
	जइ (lua_rawequal(L, -1, 3)) // list?
	{
	    list_T *l = (list_T *)lua_touserdata(L, 6); //कुञ्चिका

	    abort = set_ref_in_list(l, copyID);
	}
	अण्णहा जइ (lua_rawequal(L, -1, 4)) // dict?
	{
	    dict_T *d = (dict_T *)lua_touserdata(L, 6); //कुञ्चिका

	    abort = set_ref_in_dict(d, copyID);
	}
	अण्णहा जइ (lua_rawequal(L, -1, 5)) // funcref?
	{
	    luaV_Funcref *f = (luaV_Funcref *)lua_touserdata(L, 6); //कुञ्चिका

	    abort = set_ref_in_dict(f->self, copyID);
	}
	lua_निष्कासय(L, 2); // metatable and value
    }
    lua_pushinteger(L, abort);
    प्रत्यागम 1;
}

    स्थिर पूर्णाङ्क
luaV_pushversion(lua_State *L)
{
    पूर्णाङ्क major = 0;
    पूर्णाङ्क minor = 0;
    पूर्णाङ्क patch = 0;
    वर्ण s[16];

    sscanf(LUAVIM_VERSION, "Lua %d.%d.%d", &major, &minor, &patch);
    vim_snprintf(s, परिमाण(s), "%d.%d.%d", major, minor, patch);
    lua_pushstring(L, s);
    प्रत्यागम 0;
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

    स्थिर पूर्णाङ्क
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
    lua_निष्कासय(L, 1);  // we don't need the cache table here anymore
    // print
    lua_pushcfunction(L, luaV_print);
    lua_setglobal(L, "print");
    // debug.debug
    lua_getglobal(L, "debug");
    lua_pushcfunction(L, luaV_debug);
    lua_setfield(L, -2, "debug");
    lua_निष्कासय(L, 1);
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
    lua_निष्कासय(L, 1);
    luaV_newmetatable(L, LUAVIM_DICT);
    luaV_register(L, luaV_Dict_mt);
    lua_निष्कासय(L, 1);
    luaV_newmetatable(L, LUAVIM_BLOB);
    luaV_register(L, luaV_Blob_mt);
    lua_निष्कासय(L, 1);
    luaV_newmetatable(L, LUAVIM_FUNCREF);
    luaV_register(L, luaV_Funcref_mt);
    lua_निष्कासय(L, 1);
    luaV_newmetatable(L, LUAVIM_BUFFER);
    luaV_register(L, luaV_Buffer_mt);
    lua_निष्कासय(L, 1);
    luaV_newmetatable(L, LUAVIM_WINDOW);
    luaV_register(L, luaV_Window_mt);
    lua_निष्कासय(L, 1);
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

    जइ (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);

    प्रत्यागम 0;
}

    स्थिर lua_State *
luaV_newstate(शून्य)
{
    lua_State *L = luaL_newstate();
# जइ LUA_VERSION_अङ्कः >= 505
    luaL_openselectedlibs(L, ~0, 0); // core libs
# अण्णहा
    luaL_openlibs(L); // core libs
# endif
    lua_pushcfunction(L, luaopen_vim); // vim
    lua_call(L, 0, 0);
    प्रत्यागम L;
}

    स्थिर शून्य
luaV_setrange(lua_State *L, पूर्णाङ्क line1, पूर्णाङ्क line2)
{
    lua_getglobal(L, LUAVIM_NAME);
    lua_pushinteger(L, line1);
    lua_setfield(L, -2, "firstline");
    lua_pushinteger(L, line2);
    lua_setfield(L, -2, "lastline");
    lua_निष्कासय(L, 1); // vim table
}


// =======   Interface   =======

स्थिर lua_State *L = रिक्त;

    स्थिर पूर्णाङ्क
lua_isopen(शून्य)
{
    प्रत्यागम L != रिक्त;
}

    स्थिर पूर्णाङ्क
lua_init(शून्य)
{
    जइ (lua_isopen())
	प्रत्यागम OK;

# ifdef DYNAMIC_LUA
    जइ (!lua_enabled(TRUE))
    {
	emsg(_("Lua library cannot be loaded."));
	प्रत्यागम FAIL;
    }
# endif
    L = luaV_newstate();

    प्रत्यागम OK;
}

    शून्य
lua_end(शून्य)
{
    जइ (!lua_isopen())
	प्रत्यागम;

    lua_close(L);
    L = रिक्त;
}

/*
 * ex commands
 */
    शून्य
ex_lua(exarg_T *eap)
{
    वर्ण *script = (वर्ण *)script_get(eap, eap->arg);

    जइ (!eap->skip && lua_init() == OK)
    {
	वर्ण *s = script != रिक्त ? script : (वर्ण *)eap->arg;

	luaV_setrange(L, eap->line1, eap->line2);
	जइ (luaL_loadbuffer(L, s, strlen(s), LUAVIM_CHUNKNAME)
		|| lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
    जइ (script != रिक्त)
	vim_free(script);
}

    शून्य
ex_luado(exarg_T *eap)
{
    linenr_T l;
    ध्रुव वर्ण *s = (ध्रुव वर्ण *) eap->arg;
    luaL_Buffer b;
    size_t len;
    buf_T *was_curbuf = curbuf;

    जइ (lua_init() == FAIL) प्रत्यागम;
    जइ (u_save(eap->line1 - 1, eap->line2 + 1) == FAIL)
    {
	emsg(_("cannot save undo information"));
	प्रत्यागम;
    }
    luaV_setrange(L, eap->line1, eap->line2);
    luaL_buffinit(L, &b);
    luaL_addlstring(&b, "return function(line, linenr) ", 30); // header
    luaL_addlstring(&b, s, strlen(s));
    luaL_addlstring(&b, " end", 4); // footer
    luaL_pushresult(&b);
    s = lua_tolstring(L, -1, &len);
    जइ (luaL_loadbuffer(L, s, len, LUAVIM_CHUNKNAME))
    {
	luaV_emsg(L);
	lua_निष्कासय(L, 1); // function body
	प्रत्यागम;
    }
    lua_call(L, 0, 1);
    lua_replace(L, -2); // function -> body
    प्रति (l = eap->line1; l <= eap->line2; l++)
    {
	// Check the line number, the command may have deleted lines.
	जइ (l > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	    भङ्ग;

	lua_pushvalue(L, -1); // function
	luaV_pushline(L, curbuf, l); // current line as arg
	lua_pushinteger(L, l); // current line number as arg
	जइ (lua_pcall(L, 2, 1, 0))
	{
	    luaV_emsg(L);
	    भङ्ग;
	}

	// Catch the command switching to another buffer.
	// Check the line number, the command may have deleted lines.
	जइ (curbuf != was_curbuf || l > curbuf->b_ml.ml_पङ्क्तिः_गणनम्)
	    भङ्ग;

	जइ (lua_isstring(L, -1)) // update line?
	{
# ifdef HAVE_SANDBOX
	    luaV_checksandbox(L);
# endif
	    ml_replace(l, luaV_toline(L, -1), TRUE);
	    changed_bytes(l, 0);
	    lua_निष्कासय(L, 1); // result from luaV_toline
	}
	lua_निष्कासय(L, 1); //पङ्क्तिः
    }
    lua_निष्कासय(L, 1); // function
    check_cursor();
    update_screen(UPD_NOT_VALID);
}

    शून्य
ex_luafile(exarg_T *eap)
{
    जइ (lua_init() == FAIL)
	प्रत्यागम;
    जइ (!eap->skip)
    {
	luaV_setrange(L, eap->line1, eap->line2);
	जइ (luaL_loadfile(L, (वर्ण *) eap->arg) || lua_pcall(L, 0, 0, 0))
	    luaV_emsg(L);
    }
}

# define luaV_freetype(typ,tname) \
	शून्य \
    lua_##tname##_free(typ *o) \
    { \
	जइ (!lua_isopen()) प्रत्यागम; \
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
    lua_pushstring(L, (वर्ण *) str);
    lua_pushlightuserdata(L, (शून्य *) arg);
    lua_pushlightuserdata(L, (शून्य *) rettv);
    lua_call(L, 3, 0);
}

    पूर्णाङ्क
set_ref_in_lua(पूर्णाङ्क copyID)
{
    पूर्णाङ्क aborted = 0;

    जइ (!lua_isopen())
	प्रत्यागम 0;

    luaV_getfield(L, LUAVIM_SETREF);
    // call the function with 1 arg, getting 1 result back
    lua_pushinteger(L, copyID);
    lua_call(L, 1, 1);
    // get the result
    aborted = lua_tointeger(L, -1);
    // pop result off the stack
    lua_निष्कासय(L, 1);

    प्रत्यागम aborted;
}

    शून्य
update_package_paths_in_lua(शून्य)
{
    जइ (!lua_isopen())
	प्रत्यागम;

    lua_getglobal(L, "vim");
    lua_getfield(L, -1, "_update_package_paths");

    जइ (lua_pcall(L, 0, 0, 0))
	luaV_emsg(L);
}

/*
 * Native C function callback
 */
    स्थिर पूर्णाङ्क
luaV_call_lua_func(
	पूर्णाङ्क	 argcount,
	typval_T *argvars,
	typval_T *rettv,
	शून्य	 *state)
{
    पूर्णाङ्क i;
    पूर्णाङ्क luaargcount = argcount;
    luaV_CFuncState *funcstate = (luaV_CFuncState*)state;
    lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_funcref);

    जइ (funcstate->lua_tableref != LUA_NOREF)
    {
	// First arg for metatable __call method is a table
	luaargcount += 1;
	lua_rawgeti(funcstate->L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    }

    प्रति (i = 0; i < argcount; ++i)
	luaV_pushtypval(funcstate->L, &argvars[i]);

    जइ (lua_pcall(funcstate->L, luaargcount, 1, 0))
    {
	luaV_emsg(funcstate->L);
	प्रत्यागम (पूर्णाङ्क)FCERR_OTHER;
    }

    luaV_checktypval(funcstate->L, -1, rettv, "get return value");
    प्रत्यागम (पूर्णाङ्क)FCERR_NONE;
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
    जइ (funcstate->lua_tableref != LUA_NOREF)
	luaL_unref(L, LUA_REGISTRYINDEX, funcstate->lua_tableref);
    VIM_CLEAR(funcstate);
}

#endif
