/**-------------------------------------------------------------------------***
*** Package     : d'Arc - fast Lua sub API                                  ***
*** File        : darc.c                                                    ***
*** Description : macros for VM value and table node access, Lua & LuaJIT   ***
*** Version     : 0.0.1 / sketch                                            ***
*** Requirement : Lua 5.1.4 or LuaJIT 2 bet 6                               ***
*** Copyright   : April 1st 2011 Henning Diedrich                           ***
*** Author      : H. Diedrich <hd2010@eonblast.com>                         ***
*** License     : see file LICENSE                                          ***
*** Created     : 01 Apr 2011                                               ***
*** Changed     : 01 Apr 2011                                               ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  d'Arc is a faster way to access Lua values and traverse tables in C.   ***
***  It supports Lua 5.1.4 and LuaJIT 2 beta 6 and is loathable for not     ***
***  using the Lua API. Please use it only if you are 18+ years old.        ***
***                                                                         ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  This file contains a template that allows for source that will         ***
***  work with both the classic PUC Lua and Mike Pall's LuaJIT.             ***
***                                                                         ***
***-------------------------------------------------------------------------**/


/*****************************************************************************\
***                                                                         ***
***                            HANDLING TABLES                              ***
***                                                                         ***
\*****************************************************************************/

void darc_array_part (const Table *t); 
void darc_hash_part (const Table *t);

/*---------------------------------------------------------------------------*\
 * A table, internally, has a hash & an array part                           *
\*---------------------------------------------------------------------------*/

void darc_table(const Table *t) {
    darc_hash_part(t);
    darc_array_part(t);
}

/*---------------------------------------------------------------------------*\
 * Traversing the array part                                                 *
\*---------------------------------------------------------------------------*/
/* This is Lua 5.1.4 specific.                                               */

#ifdef LUA_5_1

void darc_array_part (const Table *t) 
{
	int lg;
	int ttlg;  /* 2^lg */
	int i = 1;  /* count to traverse all array keys */
	int try = 0; /* 1 := started out well to stay w/o a key = pure */
	int pu = 1;
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

                /** YOUR STUFF HERE:  DO FOR EVERY ARRAY PART ELEMENT */

			}
    	}
	}
}

/*---------------------------------------------------------------------------*\
 * Traversing the hash part                                                  *
\*---------------------------------------------------------------------------*/
/* This is Lua 5.1.4 specific.                                               */

void darc_hash_part (const Table *t) 
{
	int i = sizenode(t);
	int pure = *ppure;

	while (i--) {
		Node *node = &t->node[i];
		if(!ttisnil(key2tval(node)) && !ttisnil(gval(node))) {

            /** YOUR STUFF HERE:  DO FOR EVERY HASH PART ELEMENT */

		}
	}
}
#endif

#ifdef JIT_2

/*---------------------------------------------------------------------------*\
 * Traversing the array part                                                 *
\*---------------------------------------------------------------------------*/
/* This is LuaJIT 2 specific.                                                */

void darc_array_part (const Table *t) 
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

                /** YOUR STUFF HERE:  DO FOR EVERY ARRAY PART ELEMENT */

			}
		}
	}
}

/*---------------------------------------------------------------------------*\
 * Traversing the hash part                                                  *
\*---------------------------------------------------------------------------*/
/* This is LuaJIT 2 specific.                                                */

void darc_hash_part (const Table *t) 
{
  uint32_t i, hmask = t->hmask;
  Node *node = noderef(t->node);
  for (i = 0; i <= hmask; i++) {

    Node *n = &node[i];
    if (!tvisnil(&n->val) && !tvisnil(&n->key)) {

            /** YOUR STUFF HERE:  DO FOR EVERY HASH PART ELEMENT */

    }
  }
}

#endif // < JIT
