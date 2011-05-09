
<h1>d'Arc 0.2.0</h1>

This is a bare bones sample. The interesting stuff is right at the top.

**sample/filter/filter.c**
    /*       (                             (                                  
     *       )\ )      (     )        d'Arc)\ )                      (        
     *      (()/(  (   )\ ( /(   (   (    (()/(    )     )           )\   (   
     *       /(_)) )\ ((_))\()) ))\  )(    /(_))( /(    (     `  )  ((_) ))\  
     *      (_))_|((_) _ (_))/ /((_)(()\  (_))  )(_))   )\  ' /(/(   _  /(( ) 
     *      | |_   (_)| || |_ (_))   ((_) / __|((_)_  _((_)) ((_)_\ | |(_)) /   
     *      | __|  | || ||  _|/ -_) | '_| \__ \/ _` || '  \()| '_ \)| |/ -_)  
     *      |_|    |_||_| \__|\___| |_|   |___/\__,_||_|_|_| | .__/ |_|\___|  
     *                                                       |_|
     *
     *      This is a sample for d'Arc 0.2.0, a Lua sub API for Lua and LuaJIT
     *
     *      Swearword filter: returns true when a swear is found in a value.
     *
     *      > =filter.check("dork")
     *      false
     *
     *      > =filter.check({"nice", "tender", "bitch")
     *      true
     *
     *      This is not even entirely fictitious: word filters are historically
     *      annoyingly resource consuming. Still, this only for illustration. 
     */
    
    #include <string.h>
    #include "darc.h"
    
    int inspect(const char *string);
    
    /*****************************************************************************\
     *             core function: traverse a table looking at each element        *
    \*****************************************************************************/ 
    
    /* Relevant things: 1) the MACROS and 2) the DARC_TRAVERSE() function.       */
    
    int word_filter(TValue *variable, void *state)
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
    
    /* Check out filter4bench.c for how this is done using the official API.     */
    
    /*****************************************************************************\ 
    *                            detect a foul word                               *                                   
    \*****************************************************************************/ 
    
    /* nothing relevant in this section                                          */
    
    int inspect(const char *string)
    {
        if(strstr(string, "bitch") == 0)
            return 1;
    
        /* yeah, genius. But you get the point. */
        return 0;
    }
    
    /****************************************************************************)\
    ***                               LUA HOOKS                                *(*)
    \*****************************************************************************/
    
    /* nothing relevant in this section                                          */
    
    static int filter_check (lua_State *L);
    
    static const struct luaL_Reg filter [] = {
    
    	{"check", filter_check }, /* main function: check strings for swearwords  */
    	{ NULL, NULL }
    };
    
    /*---------------------------------------------------------------------------*\
    *  Main function: Test a Lua value for whether it contains swear words        *
    \*---------------------------------------------------------------------------*/
    
    /* nothing relevant in this section                                          */
    
    static int filter_check(lua_State *L) {
    
        lua_settop(L, 1);   // arguments, max: 1
    	TValue *subject = index2addr(L, -1);
    	lua_pop(L, 1);
    
        int ok = word_filter(subject, (void *)0);
        
    	lua_pushboolean(L, !ok);
    
    	return 1;	
    }
    
    /*--------------------------------------------------------------------- ( ----*\
     *  Registration function for opening the filter module                (,\    *
    \*----------------------------------------------------------------------------*/
    /*
     * We register a function of a different name for Lua and LuaJIT respectively.
     *
     * Because as a convention, we always compile two libs, in this case:
     * 'filter' for Lua, 'filterjit' for LuaJIT. That's one for each target,
     * Lua and LuaJIT. And we want to have the name of the resulting lib
     * be identical to the function registered here, which opens the module. 
     *
     * Using the official API, one and the same lib works for both Lua and LuaJIT.
     * Not so for d'Arc, by definition, because it bypasses the official API.
     */
    
    #ifdef LUA_5_1
    
    LUALIB_API int luaopen_filter (lua_State *L) {
    
    	luaL_register(L, "filter", filter); /* registering this module (PUC Lua) */
    	return 1;
    }
    
    #endif
    
    #ifdef JIT_2
    
    LUALIB_API int luaopen_filterjit /* <<-- 'jit' */ (lua_State *L) {
    
    	luaL_register(L, "filter", filter); /* registering this module (LuaJIT) */
    	return 1;
    }
    
    #endif
    

Run it by doing: lua[jit] samples/filter/filter.lua.
