
<h1>d'Arc 0.2.0</h1>

This is the complete d'Arc 0.2 source

**darc.h**
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
**darc.c**
    /**-------------------------------------------------------------------------***
    *** Package     : d'Arc - fast Lua sub API                                  ***
    *** File        : darc.c                                                    ***
    *** Description : macros for VM value and table node access, Lua & LuaJIT   ***
    *** Version     : 0.2.0 / alpha                                             ***
    *** Requirement : Lua 5.1.4 or LuaJIT 2 beta 7                              ***
    *** Copyright   : April 1st 2011 Henning Diedrich                           ***
    *** Author      : H. Diedrich <hd2010@eonblast.com>                         ***
    *** License     : see file LICENSE                                          ***
    *** Created     : 01 Apr 2011                                               ***
    *** Changed     : 08 May 2011                                               ***
    ***-------------------------------------------------------------------------***
    ***                                                                         ***
    ***  d'Arc is a faster way to access Lua values and traverse tables in C.   ***
    ***  It supports Lua 5.1.4 and LuaJIT 2 beta 7 and is loathable for not     ***
    ***  using the Lua API. Please use it only if you are 18+ years old.        ***
    ***                                                                         ***
    ***-------------------------------------------------------------------------***
    ***                                                                         ***
    *** This file contains d'Arc table traversal functions for Lua and LuaJIT:  ***
    ***                                                                         ***
    ***                           darc_traverse()                               ***
    ***                                                                         ***
    ***-------------------------------------------------------------------------***
    ***                                                                         ***
    ***            ASCII art: http://patorjk.com/software/taag/                 ***
    ***                                                                         ***
    ***-------------------------------------------------------------------------***
    
                               (  (    (               
                               )\ ))\   )\    (         
                              (()/((_)(((_)(  )(    (   
                               ((_) ) )\ _ )\(()\   )\  
                               _| |   (_)_\(_)((_)((_) 
                             / _` |    / _ \ | '_|/ _|  
                             \__,_|   /_/ \_\|_|  \__|                          
    
    
     ***************************************************************************** 
    ***                                                                         ***
    ***                            HANDLING TABLES                              ***
    ***                                                                         ***
     *****************************************************************************/
    
    #include "darc.h"
    
    /*---------------------------------------------------------------------------*\
     * A table, internally, has a hash & an array part                           *
    \*---------------------------------------------------------------------------*/
    
    int darc_traverse(const Table *t, foldfunc fold, void *cargo) {
    
        int goon = darc_hash_part(t, fold, (void *)cargo);
    
        if(!goon) return goon;
        
        goon = darc_array_part(t, fold, cargo);
    
        return goon;
    }
    
    /*---------------------------------------------------------------------------*\
     * Traversing the array part                                                 *
    \*---------------------------------------------------------------------------*/
    /* This is Lua 5.1.4 specific.                                               */
    
    #ifdef LUA_5_1
    
    int darc_array_part (const Table *t, foldfunc fold, void *cargo) 
    {
    	int lg;
    	int ttlg;  /* 2^lg */
    	int i = 1;  /* count to traverse all array keys */
    	for (lg=0, ttlg=1; lg<=MAXBITS; lg++, ttlg*=2) {  /* for each slice */
    		int lim = ttlg;
    		if (lim > t->sizearray) {
    			lim = t->sizearray;  /* adjust upper limit */
    			if (i > lim)
    				break;  /* no more elements */
    		}
    		/* elements in range (2^(lg-1), 2^lg] */
    		for (; i <= lim; i++) {
    			TValue * v = &t->array[i-1];
    			if(!ttisnil(v)) {
    
                    if(!(*fold)(v, cargo)) return 0; // =: break
    
    			}
        	}
    	}
    	
    	return 1; // =: continue 
    }
    
    /*---------------------------------------------------------------------------*\
     * Traversing the hash part                                                  *
    \*---------------------------------------------------------------------------*/
    /* This is Lua 5.1.4 specific.                                               */
    
    int darc_hash_part (const Table *t, foldfunc fold, void *cargo) 
    {
    	int i = sizenode(t);
    
    	while (i--) {
    		Node *node = &t->node[i];
    		if(!ttisnil(key2tval(node)) && !ttisnil(gval(node))) {
    
                    if(!(*fold)(key2tval(node), cargo)) return 0;  // =: break
    
    		}
    	}
    	
    	return 1;  // =: continue
    }
    #endif
    
    #ifdef JIT_2
    
    /*---------------------------------------------------------------------------*\
     * Traversing the array part                                                 *
    \*---------------------------------------------------------------------------*/
    /* This is LuaJIT 2 specific.                                                */
    
    int darc_array_part (const Table *t, foldfunc fold, void *cargo) 
    {
    	uint32_t i, b;
    	if (t->asize == 0) return 1; // =: continue
    	for (i = b = 0; b < LJ_MAX_ABITS; b++) {
    		uint32_t n, top = 2u << b;
    		TValue *array;
    		if (top >= t->asize) {
    			top = t->asize-1;
    			if (i > top)
    			break;
    		}
    		array = tvref(t->array);
    		for (n = 0; i <= top; i++) {
    
    			TValue *v = &array[i]; /* i, not i-1, as in the Lua 5.1 part  */
    			if (!tvisnil(v)) {
    
                    if(!(*fold)(v, cargo)) return 0; // =: break
    
    			}
    		}
    	}
    	return 1;  // =: continue
    }
    
    /*---------------------------------------------------------------------------*\
     * Traversing the hash part                                                  *
    \*---------------------------------------------------------------------------*/
    /* This is LuaJIT 2 specific.                                                */
    
    int darc_hash_part (const Table *t, foldfunc fold, void *cargo) 
    {
        uint32_t i, hmask = t->hmask;
        Node *node = noderef(t->node);
        for (i = 0; i <= hmask; i++) {
    
            Node *n = &node[i];
            if (!tvisnil(&n->val) && !tvisnil(&n->key)) {
    
                if(!(*fold)((&n->val), cargo)) return 0; // =: break
            }
        }
    	return 1;  // =: continue
    }
    
    #endif // < JIT
    
    /*****************************************************************************\
    ***                                                                         ***
    ***                              ORIGINAL LUA                               ***
    ***                                                                         ***
    *******************************************************************************
    **      copy of needed original Lua source, which Lua does not export        **
    \*****************************************************************************/ 
    
    /*---------------------------------------------------------------------------*\
    **  Get the C pointer for the value on the stack                             **
    \*---------------------------------------------------------------------------*/
    
    #ifdef LUA_5_1 // > > > > > > > > > > > > > > > > > > > > > > > > > > > > > >
    
    /* from Lua 5.1.4 source */
    TValue *index2adr (lua_State *L, int idx) {
    
      if (idx > 0) {   
        TValue *o = L->base + (idx - 1);
        api_check(L, idx <= L->ci->top - L->base);
        if (o >= L->top) return cast(TValue *, luaO_nilobject);
        else return o;  
      }
      else if (idx > LUA_REGISTRYINDEX) {
        api_check(L, idx != 0 && -idx <= L->top - L->base);
        return L->top + idx;
      }
      else switch (idx) {  /* pseudo-indices */
        case LUA_REGISTRYINDEX: return registry(L);
        case LUA_ENVIRONINDEX: {
          Closure *func = curr_func(L);
          sethvalue(L, &L->env, func->c.env);
          return &L->env;
        }
        case LUA_GLOBALSINDEX: return gt(L);
        default: {
          Closure *func = curr_func(L);
          idx = LUA_GLOBALSINDEX - idx;
          return (idx <= func->c.nupvalues)
                    ? &func->c.upvalue[idx-1]
                    : cast(TValue *, luaO_nilobject);
        }
      }
    }
    #endif
    
    /*****************************************************************************\
    *   copy of needed original LuaJIT source, which LuaJIT does not export       *
    \*****************************************************************************/ 
    
    /*---------------------------------------------------------------------------*\
    **  Get the C pointer for the value on the stack                             **
    \*---------------------------------------------------------------------------*/
    
    
    #ifdef JIT_2 // | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | | |
    
    /* from jit/src/lj_api.c */
    TValue *index2adr(lua_State *L, int idx)
    {
      if (idx > 0) {
        TValue *o = L->base + (idx - 1);
        return o < L->top ? o : niltv(L);
      } else if (idx > LUA_REGISTRYINDEX) {
        api_check(L, idx != 0 && -idx <= L->top - L->base);
        return L->top + idx;
      } else if (idx == LUA_GLOBALSINDEX) {
        TValue *o = &G(L)->tmptv;
        settabV(L, o, tabref(L->env));
        return o;
      } else if (idx == LUA_REGISTRYINDEX) {
        return registry(L);
      } else {
        GCfunc *fn = curr_func(L);
        api_check(L, fn->c.gct == ~LJ_TFUNC && !isluafunc(fn));
        if (idx == LUA_ENVIRONINDEX) {
          TValue *o = &G(L)->tmptv;
          settabV(L, o, tabref(fn->c.env));
          return o;
        } else {
          idx = LUA_GLOBALSINDEX - idx;
          return idx <= fn->c.nupvalues ? &fn->c.upvalue[idx-1] : niltv(L);
        }
      }
    }
    #endif // Lua/JIT < < < < < < < < < < < < < < < < < < < < < < < < < < < < < < <
    
    /* For above original Lua and LuaJIT source, see special notes in LICENSE. */
    

And here is a <a href=sample.html>sample</a> on how to use it.
