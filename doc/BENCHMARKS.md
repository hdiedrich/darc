d'Arc 0.1.0  
## Benchmarks
    
These are benchmarks with the filter sample, albeit with the actual filtering excluded from the math: inspect() was set to always return 1 right away.

For more or less empty racing through a table, d'Arc is up to 2 times faster for small tables and more than 3 times faster for bigger ones, as compared to the official Lua API.
    ---------------------------------------------------------------------------------
    12 elements in t={{'help!',{22, {'Oh damn.', 1}, 'foo'}, 'luck', 'struck'}, nil} 
    100000x filter.check_official(t)        392ns/element         false 
    100000x filter.check(t)                 250ns/element  63%,   false
    ---------------------------------------------------------------------------------
    10 elements in t[randstr(i)]=randstr(i) 
    100000x filter.check_official(t)        430ns/element         false 
    100000x filter.check(t)                 230ns/element  53%,   false
    ---------------------------------------------------------------------------------
    1000 elements in t[randstr(i)]=randstr(i) 
    1000x filter.check_official(t)          280ns/element         false 
    1000x filter.check(t)                    80ns/element  28%,   false
    ---------------------------------------------------------------------------------
    10000 elements in t[randstr(i)]=randstr(i) 
    100x filter.check_official(t)           440ns/element         false 
    100x filter.check(t)                    130ns/element  29%,   false
    ---------------------------------------------------------------------------------
    100000 elements in t[randstr(i)]=randstr(i) 
    10x filter.check_official(t)            520ns/element         false 
    10x filter.check(t)                     160ns/element  30%,   false
    ---------------------------------------------------------------------------------
    10000 elements in t[i*100]=randstr(i)      
    100x filter.check_official(t)           420ns/element         false 
    100x filter.check(t)                    120ns/element  28%,   false
    
This is the benchmarked code:

### d'Arc

    int lame_word_filter(TValue *subject, void *cargo)
    {
        if(XLUA_IS_STRING(subject))
        
            return inspect(XLUA_STRING(subject));
            
        else if(XLUA_IS_TABLE(subject))
    
            return darc_traverse(XLUA_TABLE(subject), lame_word_filter, cargo); 
        
        return 1;
    }
    
### Lua API

If somebody can point me to a better implementation, please let me know.

    static int word_filter_official(lua_State *L, int t) {
    
        int notfound = 1;
        int type = lua_type(L, -1);
    
        if(type == LUA_TTABLE) {
    
            /* table is in the stack at index 't' */
            lua_pushnil(L);  /* first key */
            while (notfound && lua_next(L, t) != 0) {
    
                /* 'key' (at index -2) and 'value' (at index -1) */
                 notfound = word_filter_official(L, t+2);
    
                /* removes 'value'; keeps 'key' for next iteration */
                lua_pop(L, 1);
            }
         }        
         else if(type == LUA_TSTRING) {
    
                const char *s = lua_tostring(L, -1);
                notfound = inspect(s);
        }
        return notfound;    
    }

For both, inspect was set to take no time:

    int inspect(const char *string)
    {
        return 1;
    }


