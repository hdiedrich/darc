**d'Arc 0.2.0**  
## Benchmarks
    
These are benchmarks of a variant of the [filter sample](sample.html). For less skew, the actual filtering was skipped: inspect() was set to always return 1 right away. And for comparison, a function using the official Lua API was added, as you can see in the [source](bench.html) for this benchmark. The Lua script doing the time measurement is found in samples/filter/filterbench.lua. The benchmark lib is built when you do make &lt;PLATFORM&gt;.

For simply racing through a table, d'Arc is less than 2 times faster for small tables and more than 5 times faster for bigger ones, as compared to the official Lua API, for both Lua and LuaJIT. Because of the reason for that speed up, the factor will most likely continue to increase with table size, also beyond the number of elements measured here.

## Lua 5.1.4

**lua  samples/filter/filterbench.lua**

       (
      ( ) Swearword filter - a d'Arc benchmark (filter4bench)
          Lua 5.1 official vs. d'Arc 0.2.0
      
      ---------------------------------------------------------------------------------
      12 elements in t={{'help!',{22, {'Oh damn.', 1}, 'foo'}, 'dork', 'struck'}, nil} 
      1000000x filter.check_official(t)              143ns/element         false 
      1000000x filter.check(t)                        85ns/element  59%,   false
      ---------------------------------------------------------------------------------
      10 elements in t[randstr(i)]=randstr(i) 
      1000000x filter.check_official(t)              173ns/element         false 
      1000000x filter.check(t)                        92ns/element  53%,   false
      ---------------------------------------------------------------------------------
      1000 elements in t[randstr(i)]=randstr(i) 
       10000x filter.check_official(t)              100ns/element         false 
       10000x filter.check(t)                        20ns/element  19%,   false
      ---------------------------------------------------------------------------------
      10000 elements in t[randstr(i)]=randstr(i) 
        1000x filter.check_official(t)              100ns/element         false 
        1000x filter.check(t)                        29ns/element  28%,   false
      ---------------------------------------------------------------------------------
      100000 elements in t[randstr(i)]=randstr(i) 
         100x filter.check_official(t)              174ns/element         false 
         100x filter.check(t)                        25ns/element  14%,   false
      ---------------------------------------------------------------------------------
      10000 elements in t[i*100]=randstr(i)      
       10000x filter.check_official(t)              126ns/element         false 
       10000x filter.check(t)                        22ns/element  17%,   false



## LuaJIT 2

d'Arc benefits from JIT for small table sizes. That may be owed to the fact that the overhead of calling the function is visible with small tables. But the absolute speed you can reach using d'Arc is about the same for Lua and LuaJIT. Sparse arrays seem to perform better, for d'Arc, using classic PUC Lua (last row). If that is not a mistake in measurement, I can only speculate that it may point to differences in the implementation for hash calculation and hash look ups. Lua and LuaJIT are on a level playing field there. There is no JIT magic involved in such basics.

**luajit samples/filter/filterbench.lua**
      
       (
      ( ) Swearword filter - a d'Arc benchmark (filter4benchjit)
          LuaJIT 2.0.0-beta7 official vs. d'Arc 0.2.0
      
      ---------------------------------------------------------------------------------
      12 elements in t={{'help!',{22, {'Oh damn.', 1}, 'foo'}, 'dork', 'struck'}, nil} 
      1000000x filter.check_official(t)               89ns/element         false 
      1000000x filter.check(t)                        47ns/element  52%,   false
      ---------------------------------------------------------------------------------
      10 elements in t[randstr(i)]=randstr(i) 
      1000000x filter.check_official(t)              101ns/element         false 
      1000000x filter.check(t)                        45ns/element  44%,   false
      ---------------------------------------------------------------------------------
      1000 elements in t[randstr(i)]=randstr(i) 
       10000x filter.check_official(t)               86ns/element         false 
       10000x filter.check(t)                        19ns/element  22%,   false
      ---------------------------------------------------------------------------------
      10000 elements in t[randstr(i)]=randstr(i) 
        1000x filter.check_official(t)               85ns/element         false 
        1000x filter.check(t)                        29ns/element  34%,   false
      ---------------------------------------------------------------------------------
      100000 elements in t[randstr(i)]=randstr(i) 
         100x filter.check_official(t)              157ns/element         false 
         100x filter.check(t)                        24ns/element  15%,   false
      ---------------------------------------------------------------------------------
      10000 elements in t[i*100]=randstr(i)      
       10000x filter.check_official(t)               99ns/element         false 
       10000x filter.check(t)                        30ns/element  29%,   false

## Source

This is the relevant part of the benchmarked code:

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

And this is the comparison code that uses the official Lua API. Both snippets are compiled and run with no modification for both Lua and LuaJIT. And in case you can point me to a better implementation of the below, please let me know. It's plain vanilla, basically lifted from the Lua 5.1 manual.

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

For the benchmarking, filter4bench is compiled with LESS_SKEW_FOR_BENCHMARK switched on, which results into short cutting the 'filtering', so that it does not skew the measurement of the table traversion. This define basically makes the inspect() function return right away, and take no time:

    int inspect(const char *string)
    {
        return 1;
    }

## Hardware

    Processor      : 2 GHz Intel Core 2 Duo (1 cpu, 2 cores) 3 MB L2 Cache
    Memory         : 4 GB DDR3 1 GHz
    Bus            : 1 GHz
    System         : Mac OS X 10.5 - Darwin 9.8.0