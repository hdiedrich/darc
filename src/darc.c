/**-------------------------------------------------------------------------***
*** Package     : d'Arc - fast Lua sub API                                  ***
*** File        : darc.c                                                    ***
*** Description : macros for VM value and table node access, Lua & LuaJIT   ***
*** Version     : 0.1.0 / alpha                                             ***
*** Requirement : Lua 5.1.4 or LuaJIT 2 beta 6                              ***
*** Copyright   : April 1st 2011 Henning Diedrich                           ***
*** Author      : H. Diedrich <hd2010@eonblast.com>                         ***
*** License     : see file LICENSE                                          ***
*** Created     : 01 Apr 2011                                               ***
*** Changed     : 07 Apr 2011                                               ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  d'Arc is a faster way to access Lua values and traverse tables in C.   ***
***  It supports Lua 5.1.4 and LuaJIT 2 beta 6 and is loathable for not     ***
***  using the Lua API. Please use it only if you are 18+ years old.        ***
***                                                                         ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  This file contains d'Arc table traversal functions for Lua and LuaJIT. ***
***                                                                         ***
***-------------------------------------------------------------------------***
***                                                                         ***
***            ASCII art: http://patorjk.com/software/taag/                 ***
***                                                                         ***
***-------------------------------------------------------------------------***

                           (   (    (               
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
	if (t->asize == 0) return;
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

            if(!(*fold)(v, cargo)) return 0; // =: break
        }
    }
	return 1;  // =: continue
}

#endif // < JIT

/*****************************************************************************\
***                                                                         ***
***                              ORIGINAL LUA                               ***
***                                                                         ***
 ***************************************************************************** 
 *      copy of needed original Lua source, which Lua does not export        *
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



