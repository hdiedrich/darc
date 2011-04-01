/**-------------------------------------------------------------------------***
*** Package     : d'Arc - fast Lua sub API                                  ***
*** File        : darc.h                                                    ***
*** Description : macros for VM value and table node access, Lua & LuaJIT   ***
*** Version     : 0.0.1 / sketch                                            ***
*** Requirement : Lua 5.1.4 or LuaJIT 2 bet 6                               ***
*** Copyright   : April 1st 2011 Henning Diedrich                           ***
*** Author      : H. Diedrich <hd2010@eonblast.com>                         ***
*** License     : see file LICENSE                                          ***
*** Created     : 23 Mar 2011                                               ***
*** Changed     : 01 Apr 2011                                               ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  d'Arc is a faster way to access Lua values and traverse tables in C.   ***
***  It supports Lua 5.1.4 and LuaJIT 2 beta 6 and is loathable for not     ***
***  using the Lua API. Please use it only if you are 18+ years old.        ***
***                                                                         ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  This file contains macros that allow you to write source that will     ***
***  work with both the classic PUC Lua and Mike Pall's LuaJIT.             ***
***                                                                         ***
***-------------------------------------------------------------------------**/

/* Divine version ----------------------------------------------------------- */

#ifdef LUA_5_1
  #undef JIT_2
#elif JIT_2
#else
  #error Please define either LUA_5_1 or JIT_2
#endif
  
/**-------------------------------------------------------------------------***
***                                                                         ***
***                                LUA 5.1.4                                ***
***                                                                         ***
***-------------------------------------------------------------------------**/

#ifdef LUA_5_1

  /* Lua headers (in include/lua-5.1.4) ------------------------------------ */
  #include "lobject.h"
  #include "ltable.h"
  #include "lua.h"
  #include "lauxlib.h"
  #include "lualib.h"
  #include "ldebug.h"
  #include "ldo.h"
  #include "lgc.h"
  #include "lmem.h"
  #include "lstate.h"

  /* Lua ............ see obj.h */

  #define XLUA_BOOLEAN(o)          ((o)->value.b)
  #define XLUA_NUMBER(o)   	       (nvalue(o))
  #define XLUA_STRING(o)           (svalue(o))
  #define XLUA_STRING_LENGTH(o)    (tsvalue(o)->len)
  #define XLUA_TABLE(o)			   (hvalue(o))

  #define XLUA_NODE_KEYVAL(node)   (key2tval(node))
  #define XLUA_NODE_VAL(node)      (gval(node))
  
  #define XLUA_IS_NIL(o)			ttisnil(o) 
  #define XLUA_IS_TRUE(o)			((o)->value.b == true) 
  #define XLUA_IS_FALSE(o)			((o)->value.b == false)
  #define XLUA_IS_BOOLEAN(o)		ttisboolean(o) 
  #define XLUA_IS_NUMBER(o)			ttisnumber(o) 
  #define XLUA_IS_STRING(o)			ttisstring(o) 
  #define XLUA_IS_TABLE(o)			ttistable(o) 
  #define XLUA_IS_FUNCTION(o)		ttisfunction(o) 
  #define XLUA_IS_USERDATA(o)		ttisuserdata(o) 
  #define XLUA_IS_THREAD(o)			ttisthread(o) 
  #define XLUA_IS_LIGHTUSERDATA(o)	ttislightuserdata(o) 

#endif

/**-------------------------------------------------------------------------***
***                                                                         ***
***                                LUAJIT 2                                 ***
***                                                                         ***
***-------------------------------------------------------------------------**/

#ifdef JIT_2 /* ............ see lj_obj.h */

  /* LuaJIT headers (in include/jit-2.0.0) --------------------------------- */
  
  #include "lua.h"
  #include "lauxlib.h"
  #include "lj_obj.h"

  typedef GCtab Table;

  #define XLUA_IS_NIL(o)			tvisnil(o) 
  #define XLUA_IS_TRUE(o)			tvistrue(o) 
  #define XLUA_IS_FALSE(o)			tvisfalse(o) 
  #define XLUA_IS_BOOLEAN(o)		tvisbool(o) 
  #define XLUA_IS_NUMBER(o)			tvisnumber(o) 
  #define XLUA_IS_STRING(o)			tvisstr(o) 
  #define XLUA_IS_TABLE(o)			tvistab(o) 
  #define XLUA_IS_FUNCTION(o)		tvisfunc(o) 
  #define XLUA_IS_USERDATA(o)		tvisudata(o) 
  #define XLUA_IS_THREAD(o)			tvisthread(o) 
  #define XLUA_IS_LIGHTUSERDATA(o)	tvislightud(o) 

  #define XLUA_BOOLEAN(o)   		(boolV(o)) 
  #define XLUA_NUMBER(o)   	        (numV(o))
  #define XLUA_STRING(o)          	(strVdata(o))
  #define XLUA_STRING_LENGTH(o)   	(strV(o)->len)
  #define XLUA_TABLE(o)				(tabV(o))
  
  #define XLUA_NODE_KEYVAL(node) 	((node)->key)
  #define XLUA_NODE_VAL(node)		((node)->val)

  /* Lua syncrasies -------------------------------------------------------- */

  /* from luaconf.h TODO: clear up, should be superfluous */
  #if INT_MAX-20 < 32760
	#define LUAI_BITSINT	16
  #elif INT_MAX > 2147483640L
	#define LUAI_BITSINT	32
  #else
	#error "you must define LUA_BITSINT with number of bits in an integer"
  #endif

  /* From Lua llimits.h */
  #define MAX_INT (INT_MAX-2)  /* maximum value of an int (-2 for safety) */

#endif
