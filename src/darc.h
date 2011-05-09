/**-------------------------------------------------------------------------***
*** Package     : d'Arc - fast Lua sub API                                  ***
*** File        : darc.h                                                    ***
*** Description : macros for VM value and table node access, Lua & LuaJIT   ***
*** Version     : 0.2.0 / alpha                                             ***
*** Requirement : Lua 5.1.4 or LuaJIT 2 beta 7                              ***
*** Copyright   : April 1st 2011 Henning Diedrich                           ***
*** Author      : H. Diedrich <hd2010@eonblast.com>                         ***
*** License     : see file LICENSE                                          ***
*** Created     : 23 Mar 2011                                               ***
*** Changed     : 08 May 2011                                               ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  d'Arc is a faster way to access Lua values and traverse tables in C.   ***
***  It supports Lua 5.1.4 and LuaJIT 2 beta 7 and is loathable for not     ***
***  using the Lua API. Please use it only if you are 18+ years old.        ***
***                                                                         ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  This file contains macros that allow you to write source that will     ***
***  work with both the classic PUC Lua and Mike Pall's LuaJIT.             ***
***                                                                         ***
***-------------------------------------------------------------------------***

                            (  (    (               
                            )\ ))\   )\    (         
                           (()/((_)(((_)(  )(    (   
                            ((_) ) )\ _ )\(()\   )\  
                            _| |   (_)_\(_)((_)((_) 
                          / _` |    / _ \ | '_|/ _|  
                          \__,_|   /_/ \_\|_|  \__|                          


* devine version ----------------------------------------------------------- */

#ifdef LUA_5_1
  #undef JIT_2
#elif JIT_2
#else
  #error Please define either LUA_5_1 or JIT_2
#endif
  
/**-------------------------------------------------------------------------**\
***                                                                         ***
***                                LUA 5.1.4                                ***
***                                                                         ***
\**-------------------------------------------------------------------------**/

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

  TValue *index2adr(lua_State *L, int idx);

  /* Lua ............ see obj.h */

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

  #define XLUA_BOOLEAN(o)          ((o)->value.b)
  #define XLUA_NUMBER(o)   	       (nvalue(o))
  #define XLUA_STRING(o)           (svalue(o))
  #define XLUA_STRING_LENGTH(o)    (tsvalue(o)->len)
  #define XLUA_TABLE(o)			   (hvalue(o))

  #define XLUA_NODE_KEYVAL(node)   (key2tval(node))
  #define XLUA_NODE_VAL(node)      (gval(node))

  #define XLUA_INDEX_TO_ADDRESS(L, index) index2adr(L, index)

  #define index2addr(L, index) index2adr(L, index) // eliminate pitfall

  /* necessary for Lua code interfacing, taken from Lua source               */ 

  /* Lua syncrasies -------------------------------------------------------- */

  /* From Lua llimits.h */
  // #define MAX_INT (INT_MAX-2)  /* maximum value of an int (-2 for safety) */
  
  /* from luaconf.h TODO: clear up, should be superfluous */
  #if INT_MAX-20 < 32760 
	#define LUAI_BITSINT	16 
  #elif INT_MAX > 2147483640L 
	#define LUAI_BITSINT	32 
  #else 
	#error "you must define LUA_BITSINT with number of bits in an integer" 
  #endif 

  /* max size of array part is 2^MAXBITS (excerpt from Lua ltable.c) */
  #if LUAI_BITSINT > 26
  #define MAXBITS		26
  #else
  #define MAXBITS		(LUAI_BITSINT-2)
  #endif
  
  static const char ESC=27;
  static const char OPN=28;
  static const char CLS=29;
  
  #define true 1
  #define false 0

#endif

/**-------------------------------------------------------------------------**\
***                                                                         ***
***                                LUAJIT 2                                 ***
***                                                                         ***
\**-------------------------------------------------------------------------**/

#ifdef JIT_2 /* ............ see lj_obj.h */

  /* LuaJIT headers (in include/jit-2.0.0) --------------------------------- */

  #include "lua.h" 
  #include "lauxlib.h" 
  #include "lj_obj.h" 

  TValue *index2adr(lua_State *L, int idx); 

  typedef GCtab Table; 

  /* LuaJIT ............ see lj_obj.h */

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

  #define XLUA_INDEX_TO_ADDRESS(L, index) index2adr(L, index) 

  #define index2addr(L, index) index2adr(L, index) // eliminate pitfall

#endif


/**-------------------------------------------------------------------------**\
***                                                                         ***
***                                 d'Arc                                   ***
***                                                                         ***
\**-------------------------------------------------------------------------**/


typedef int (*foldfunc)(TValue *o, void *cargo);
int darc_traverse(const Table *t, foldfunc fold, void *cargo);
int darc_array_part (const Table *t, foldfunc func, void *cargo); 
int darc_hash_part (const Table *t, foldfunc func, void *cargo);
