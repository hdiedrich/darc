/*       (                             (                                  
 *       )\ )      (     )        d'Arc)\ )                      (        
 *      (()/(  (   )\ ( /(   (   (    (()/(    )     )           )\   (   
 *       /(_)) )\ ((_))\()) ))\  )(    /(_))( /(    (     `  )  ((_) ))\  
 *      (_))_|((_) _ (_))/ /((_)(()\  (_))  )(_))   )\  ' /(/(   _  /(( ) 
 *      | |_   (_)| || |_ (_))   ((_) / __|((_)_  _((_)) ((_)_\ | |(_)) /   
 *      | __|  | || ||  _|/ -_) | '_| \__ \/ _` || '  \()| '_ \)| |/ -_)  
 *      |_|    |_||_| \__|\___| |_|   |___/\__,_||_|_|_| | .__/ |_|\___|  
 *
 *
 *      This is a sample for d'Arc 0.2.0, a Lua sub API for Lua and LuaJIT
 *
 *      Swearword filter: returns true when a swear is found in a value.
 *
 *      > =filter.check("dork")
 *      true
 *
 *      > =filter.check({"nice", "tender", "bitch")
 *      true
 *
 *      This is not even entirely fictitious: word filters are historically
 *      annoyingly resource consuming. Still, this only for illustration. 
 */

#include <string.h>
#include "darc.h"

/*****************************************************************************\
***                                                                         ***
***                               DECLARATIONS                        )     ***
***                                                                  ( )    ***
\*******************************************************************( )(******/

/* nothing special in this section                                           */

LUALIB_API int luaopen_filter (lua_State *L);
LUALIB_API int luaopen_filterjit (lua_State *L);

static int filter_check (lua_State *L);
static int filter_hello (lua_State *L);
static int filter_check_official(lua_State *L);

static int word_filter(TValue *variable, void *state);
static int word_filter_official(lua_State *L, int t);

int inspect(const char *string);

/****************************************************************************.\
***                                                                         .**
***                               LUA HOOKS                                 *)*
***                                                                         (*)
\*****************************************************************************/

/*---------------------------------------------------------------------------*\
 *  List of functions to register with Lua                                   *
\*---------------------------------------------------------------------------*/

/* nothing special in this section                                           */

static const struct luaL_Reg filter [] = {

	{"hello", filter_hello }, /* bare bone test function to test the build    */
	{"check", filter_check }, /* main function: check strings for swearwords  */
	{"check_official", filter_check_official }, /* to benchmark, same via API */
	{ NULL, NULL }
};

/*--------------------------------------------------------------------  ( ----*\
 *  Registration Function for Opening the filter Module                ( \    *
\*----------------------------------------------------------------------------*/
/*
 * We register a function of a different name for Lua and LuaJIT respectively.
 *
 * Because as a convention, we always compile two libs, once for each target,
 * ie Lua and LuaJIT, at once. And want to have the name of the resulting lib
 * be identical to the function registered here that opens the module. In this
 * case: 'filter' for Lua, 'filterjit' for LuaJIT.
 *
 * Using the official API, one lib and the same works for both Lua and LuaJIT.
 * Not so, by definition, when using d'Arc, which reaches below that API.
 */

#ifdef LUA_5_1
LUALIB_API int luaopen_filter4bench (lua_State *L) {

	luaL_register(L, "filter4bench", filter); /* registering this module (PUC Lua) */
	return 1;
}
#endif

#ifdef JIT_2
LUALIB_API int luaopen_filter4benchjit /* <<-- 'jit' */ (lua_State *L) {

	luaL_register(L, "filter4bench", filter); /* registering this module (LuaJIT) */
	return 1;
}
#endif



/*****************************************************************************\
***                                                                         ***
***                              LUA EXPORTS                                ***
***                                                                         ***
 ***************************************************************************** 
 *         entry point functions for calls coming from Lua scripts           *
\*****************************************************************************/ 

/*---------------------------------------------------------------------------*\
*  Main function: Test a Lua value for whether it contains swear words        *
\*---------------------------------------------------------------------------*/

/* nothing special in this section                                           */

static int filter_check(lua_State *L) {


    lua_settop(L, 1);   // arguments, max: 1
	TValue *subject = index2addr(L, -1);
	lua_pop(L, 1);

    int ok = word_filter(subject, (void *)0);
    
    /* note: for this simple sample, the boolean return value is usefull. 
       For more elaborate stuff, you'd use the state pointer for returns. */

	lua_pushboolean(L, !ok);

	return 1;	
}

/*---------------------------------------------------------------------------*\
*  For benchmarking: same functionality using the official API                *
\*---------------------------------------------------------------------------*/

/* nothing special in this section                                           */

static int filter_check_official(lua_State *L) {


    lua_settop(L, 1);   // arguments, max: 1

    int ok = word_filter_official(L, 1);

	lua_pop(L, 1);
    
    /* note: for this simple sample, the boolean return value is usefull. 
       For more elaborate stuff, you'd use the state pointer for returns. */

	lua_pushboolean(L, !ok);

	return 1;	
}

/*---------------------------------------------------------------------------*\
*   Hello test call implementation (simple function to test the lib build)    *
\*---------------------------------------------------------------------------*/

/* nothing special in this section                                           */

static int filter_hello(lua_State *L) {

	printf("Hello Jeanne!\n");
	return 1;
	
	/* that's it for this function, it is used to get plain signs of life. */	
}

/*****************************************************************************\
***                                                                         ***
***                          ACTUAL FUNCTIONALITY                           ***
***                                                                         ***
 ***************************************************************************** 
 *                      now to the things we are here for                    *
\*****************************************************************************/ 

/*  To reiterate: this source compiles for Lua and LuaJIT. It does NOT use the
 *  official Lua API, but accesses Lua values directly. Thus, you get two
 *  different libraries as a result, one for Lua, one for LuaJIT.
 *
 *  The relevant things here: 1) the macros and 2) the darc_traverse() function.
 *
 *  This sample is NOT optimized to the hilt, in order to be more instructive.
 *
 *  Anyway, in other words, this is the faster, d'Arc way of doing things:
 */

static int word_filter(TValue *variable, void *state)
{
     /* if a string: screen it for bad words */
    if(XLUA_IS_STRING(variable))
    
        /* screen it */
        return inspect(XLUA_STRING(variable));
        
    /* if a table: traverse it */
    else if(XLUA_IS_TABLE(variable))

        /* traverse table, using this function as callback, for each element */
        return darc_traverse(XLUA_TABLE(variable), word_filter, state); 
    
    return 1;
}
/* Maybe think about it for a moment. darc_traverse() is pretty powerful. */

/***************************************************************************** 
 *  ... and for benchmarking against: the same, but using the official API   *
\*****************************************************************************/ 
/* 
 * Now the plain vanilla, standard procedure as provided in the Lua manual:
 *
 * the conventional, slower way of doing things, for the same result as above. 
 *
 * Objectively, this is in a different way 'hard to follow', state-heavy due to
 * the use of the stack to propagate values. The callback used above may not
 * be immediately transparent either. But it's arguably more straight forward.
 *
 */

static int word_filter_official(lua_State *L, int t) {

    int notfound = 1;
    
    /* inspect top of stack for type */
    int type = lua_type(L, -1);

     /* if a string: screen it for bad words */
     if(type == LUA_TSTRING) {

            /* get the string via the stack */
            const char *s = lua_tostring(L, -1);
            
            /* screen it */
            notfound = inspect(s);
    }


    /* if a table: traverse it */
    else if(type == LUA_TTABLE) {

        /* table is in the stack at index 't'. Push an empty 'first key': */
        lua_pushnil(L);
        
        /* lua_next provides two over the stack: the key and the value */
        while (notfound && lua_next(L, t) != 0) {

            /* recurse, with 'key' (at index -2) and 'value' (at index -1) */
             notfound = word_filter_official(L, t+2);

            /* remove 'value'; keep 'key' for next iteration */
            lua_pop(L, 1);
        }
     }        

    return notfound;    
}

/***************************************************************************** 
 *  the actual work: find out foul mouthing                                  *
\*****************************************************************************/ 

/* Both above functions use this, just to give some color to this sample. 
 *
 * Then for benchmarking, the work bit is skipped entirely, to not water down
 * the difference in performance between the official API and d'Arc.
 */

int inspect(const char *string)
{
#ifdef LESS_SKEW_FOR_BENCHMARK
    return 1;
#else

    if(    strstr(string, "dork") == NULL
        && strstr(string, "damn") == NULL
        && strstr(string, "fuck") == NULL
        && strstr(string, "dick") == NULL
        && strstr(string, "f*ck") == NULL
        && strstr(string, "shit") == NULL
        && strstr(string, "sh*t") == NULL
        && strstr(string, "bitch") == NULL
        && strstr(string, "b*tch") == NULL
        && strstr(string, "biatch") == NULL
        && strstr(string, "idiot") == NULL
        && strstr(string, "sucker") == NULL
        && strstr(string, "s*cker") == NULL)

        /* yeah, genius. But you get the point. */
        
        return 1;

    return 0;
#endif
}
