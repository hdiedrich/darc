# d'Arc 0.1.0

d'Arc is a small, fast, straight forward, you better read-only, **sub-API** interface to Lua data for both Lua 5.1.4 and LuaJIT 2 beta 6. It's not for the faint of heart but really sweet. Using it may get you fired. Or promoted. Or both, just like [Jeanne](etc/NAME.html). 

As evidenced by the existence of LuaJIT, speed can play a role in Lua projects. d'Arc lets you read out Lua variables and tables as fast as possible, for both Lua and LuaJIT main branch versions, skipping the slow parts where you'd search for keys and propagate values via the Lua stack. Traversing a table, and reading out a variable, wastes a lot of time when done right, i.e. using the official API. Which - life can be so confusing - implies that under some circumstances doing it right may not be doing it right after all. It's complicated. Jeanne to the rescue. d'Arc is faster, use it for in-house read-only stuff that you can control. 


The main point is, that you could traverse tables faster and access variables more directly. If you care to compare C level [implementation](#implementation), please take a look at the bottom of this page. On the face of it, d'Arc looks like this:

    const char *s = XLUA_STRING(tval); 
    
... instead of going via the Lua state and stack, like this:

    const char *s = lua_tostring (lua_State *L, int index);


<div class=rightinset style='width:40%'>
With Jeanne, in the end, it didn't really matter were the voices came from that she heard. She was highly effective for them and kicked some butt, they so hated her for it, just killing her wouldn't do. Same for your projects: in the face of world history, maybe it matters less that inevitably some people will feel you are cheating when using something as dirty as d'Arc to be faster than a nice, clean API call. To wit, playing by the rules will safe them re-doing 50 lines of code in case Lua publishes a next version but will waste gazillions of cpu cycles that translate into millions of wasted Megawatts, wasted life time and dead trees! Right. So do the right thing, revel in their frantic screams and do something nice. And dirty. And even green, too!
</div>

The calls are not only different in form but as the different parameters suggest, they are used in different places. Which is what provides for the performance gain.

Watch out when you get all enthusiastic and start feeling like writing to the variables. That could have all sorts of funky consequences that you will not have plumbed until you tested string interning, gc, longjumping, yielding and parallel processes. Stay with reading unless you know what you are doing. Which is also the requirement to use this in the first place, in case that hadn't transpired yet.

## Usage

See [sample/filter/filter.c](samples/filter/filter.html)

Basically you get a bunch of oyster shucker macros to test for type and for direct access of the actual contents of the VM value structures. 

Plus, one function to traverse a table in low-level flight.

The macros and that fold function do not alter the Lua stack and no values are copied. You could write to and change the variable contents, but that's not the intention at this point. There are side effects, e.g. strings are internalized so changeing one will change all of the same value, etc.

Maybe memorize it like this: Jeanne listened to God and that worked great for a while. But it didn't necessarily work the other way around when she got in trouble. The quintessential read-only: God is listening. But he likes you to take care of the action.

**Type Tests**

    XLUA_IS_NIL(o)		
    XLUA_IS_TRUE(o)		 
    XLUA_IS_FALSE(o)		 
    XLUA_IS_BOOLEAN(o) 
    XLUA_IS_NUMBER(o)	 
    XLUA_IS_STRING(o)	 
    XLUA_IS_TABLE(o)	 
    XLUA_IS_FUNCTION(o)	
    XLUA_IS_USERDATA(o)	
    XLUA_IS_THREAD(o)		
    XLUA_IS_LIGHTUSERDATA(o)

These take a TValue pointer as argument and return 0 (false) or 1 (true).


**Value Access**

    XLUA_BOOLEAN(o)   		
    XLUA_NUMBER(o)   	        
    XLUA_STRING(o)          	
    XLUA_STRING_LENGTH(o)   	
    XLUA_TABLE(o)				

Take a TValue pointer as argument and return int, int, char *, size_t, TValue respectively. Remember, Lua allows \\0 as part of strings, and strings are interned.

**Table Entry Access**

    XLUA_NODE_KEYVAL(node) 	
    XLUA_NODE_VAL(node)		

The results of these are TValue pointers that can be used as arguments to the above type test and value access macros. But you'd only use these if you start decomposing and reinterpreting darc.c to traverse tables yet more streamlined than by using darc_traverse(). Otherwise, I think you won't run into a 'node' in the first place. I might be wrong.

**Stack Access**

    XLUA_INDEX_TO_ADDRESS(L, index) 

This returns a TValue pointer of a value residing on the Lua stack. Usually, that's your starting point. As mentioned before, d'Arc does not alter the Lua stack.

If you're only starting out with Lua C programming: the Lua stack is not the C stack, it's a specific, Lua VM structure used for the values the Lua VM swishes around.

**Table Traversal**

    typedef int (*foldfunc)(TValue *o, void *cargo);

    int darc_traverse(const Table *t, foldfunc fold, void *cargo);

This function is used to traverse a table, see the sample, below. It is implemented in a fashion akin the internal rehash functions of the VM and applies the 'foldfunc' function, that you assign to it, to every table element. Traversing down into a nested table that you encounter, has to be initiated in that foldfunc. Which is completely straight forward: since where you start from is usually a TValue, which could be a table, or not. You would usually start with a call to your 'foldfunc', not darc\_traverse(). Your foldfunc then calls darc\_traverse() if appropriate. Or it does something else completely, e.g. in case the top value is not a table anyway. Now, because your foldfunc will usually call darc\_traverse() with itself as argument, that darc_traverse() call will be the one and only call you write, and will handle all nested tables recursively. 

Maybe just read on? The sample will make it quite clear.

## Sample

A swearword filter, which returns true when a bad word is found in a Lua value:

    > v={{'help!',{22, {'Oh damn.', 1}, 'foo'}, 'luck', 'struck'}, nil}
    > print(filter.check(v))
    true

It is implemented in [sample/filter/filter.c](samples/filter/filter.html). Most of that file is boilerplate to get it to play with Lua as a library. The core of it is the following function. It is a **foldfunc** function, which is defined as

    typedef int (*foldfunc)(TValue *o, void *cargo);

This function is the central vehicle of the table traversal in d'Arc:  
1. the function is passed a Lua VM value to process, TValue *o.  
2. if you need state for your work inside the table, and to return results, pass a data structure in by means of the cargo pointer  
3. if it returns 0, the traversal is broken off and at the top level, 0 is returned  

In the case of filter.c, you can see how it indirectly calls itself, by passing itself to darc_traverse as function pointer. This should be the usual way to traverse into nested tables.
 
    int lame_word_filter(TValue *subject, void *cargo)
    {
        if(XLUA_IS_STRING(subject))
        
            /* actually act on the string. */
            return inspect(XLUA_STRING(subject)); 
            
        else if(XLUA_IS_TABLE(subject))
    
            /* traverse down into a table. */
            return darc_traverse(XLUA_TABLE(subject), lame_word_filter, cargo); 
        
        /* ignore all other types. */
        return 1;
    }
        
To reiterate: this source compiles for Lua and LuaJIT. It does not use the official Lua API but accesses the values more directly with the macros XLUA\_IS\_STRING(), XLUA\_STRING() etc. The function is not optimized to the hilt so as to remain sufficiently instructive.

The above function is tied into Lua by this generic function:

    static int filter_check(lua_State *L) {
    
        lua_settop(L, 1);
        const TValue *subject = index2addr(L, -1);
        lua_pop(L, 1);
    
        int ok = lame_word_filter(subject, (void *)0);
        lua_pushboolean(L, !ok);
    
        return 1;	
    }

For this sample, the integer return value was quite usefull. It signals if the traversal was complete (1) or broken off (0). This can be used to signal if an offending word was found, or not. For more interesting stuff, you would use the cargo pointer.


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


## Implementation <a name=implementation></a>

As for the example mentioned above: accessing a string. It's illuminating to look at what happens behind the scene. The official API simply goes through quite some hops. Let's follow the code for the d'Arc macro for Lua 5.1, and then the implementation of the official API in Lua 5.1. 

The difference is no outrage, even though it may look like it at a casual glance. The functions that are called as part of the handling in the official Lua API don't do a lot in most cases. But they still cost a function call. While d'Arc goes directly to it, calls no function and needs no stack arithmetics, but operates on the *actual* VM value and table structures.

### d'Arc sub API

Tracing a string access:

    char *s = XLUA_STRING(tval)

darc.h:82: macro
    #define XLUA_STRING(o)     (svalue(o))

lobject.h:211: macro
    #define svalue(o)          getstr(rawtsvalue(o))

lobject.h:210: macro & pointer arithmetic
    #define getstr(ts)         cast(const char *, (ts) + 1)

lobject.h:94: macro
    #define rawtsvalue(o)	   check_exp(ttisstring(o), &(o)->value.gc->ts)

check\_exp(), and its first parameter, are usually transparent but can be switched on to be executed as an assertion.


### Lua API

Tracing a string access:

    lua_tostring(L,i)

lua.h:279: macro
    #define lua_tostring(L,i)     lua_tolstring(L, (i), NULL)

lapi.c:343: **lua_tolstring()** Lua 5.1.4

    LUA_API const char *lua_tolstring (lua_State *L, int idx, size_t *len) {
        StkId o = index2adr(L, idx);
        if (!ttisstring(o)) {
            lua_lock(L);  /* `luaV_tostring' may create a new string */
            if (!luaV_tostring(L, o)) {  /* conversion failed? */
                if (len != NULL) *len = 0;
                lua_unlock(L);
                return NULL;
            }
            luaC_checkGC(L);
            o = index2adr(L, idx);  /* previous call may reallocate the stack */
            lua_unlock(L);
        }
        if (len != NULL) *len = tsvalue(o)->len;
        return svalue(o);
    }

lapi.c:49: **index2adr()** Lua 5.1.4

    static TValue *index2adr (lua_State *L, int idx) {
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

lobject.h:211: macro
    #define svalue(o)       getstr(rawtsvalue(o))

lobject.h:210: macro & pointer arithmetic
    #define getstr(ts)         cast(const char *, (ts) + 1)

lobject.h:94: macro
    #define rawtsvalue(o)	check_exp(ttisstring(o), &(o)->value.gc->ts)


## Traversal

Let's take a look at the decisive game, table traversion. This time, Lua 5.1.4 first:

### Lua API

A typical traversal using the Lua C API could look like this (From the [Lua Manual](http://www.lua.org/manual/5.1/manual.html#2.8)):

    /* table is in the stack at index 't' */
    lua_pushnil(L);  /* first key */
    while (lua_next(L, t) != 0) {
        /* uses 'key' (at index -2) and 'value' (at index -1) */
        printf("%s - %s\n",
            lua_typename(L, lua_type(L, -2)),
            lua_typename(L, lua_type(L, -1)));
        /* removes 'value'; keeps 'key' for next iteration */
        lua_pop(L, 1);
    }

This example by Roberto uses lua\_next(), which in turn calls luaH\_next(), which uses findindex(). 

lapi.c:973: **lua_next()** Lua 5.1.4

    LUA_API int lua_next (lua_State *L, int idx) {
        StkId t;
        int more;
        lua_lock(L);
        t = index2adr(L, idx);
        api_check(L, ttistable(t));
        more = luaH_next(L, hvalue(t), L->top - 1);
        if (more) {
            api_incr_top(L);
        }
        else  /* no more elements */
            L->top -= 1;  /* remove key */
        lua_unlock(L);
        return more;
    }

And we are also seeing index2adr() again, as with the string example above: the overhead that results from using the Lua stack mechanism. We'll ignore it at this point, but the payload in this case is created using lua\_type(), which again calls index2adr. So it's called three times for each iteration. 

Usually, this function will return almost immediately. But it's still some pointer calculating and a function call involved, for every value you are retrieving.

lapi.c:49: **index2adr()** Lua 5.1.4

    static TValue *index2adr (lua_State *L, int idx) {
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


ltable.c:162: **luaH_next()** Lua 5.1.4

    int luaH_next (lua_State *L, Table *t, StkId key) {
        int i = findindex(L, t, key);  /* find original element */
        for (i++; i < t->sizearray; i++) {  /* try first array part */
            if (!ttisnil(&t->array[i])) {  /* a non-nil value? */
                setnvalue(key, cast_num(i+1));
                setobj2s(L, key+1, &t->array[i]);
                return 1;
            }
        }
        for (i -= t->sizearray; i < sizenode(t); i++) {  /* then hash part */
            if (!ttisnil(gval(gnode(t, i)))) {  /* a non-nil value? */
                setobj2s(L, key, key2tval(gnode(t, i)));
                setobj2s(L, key+1, gval(gnode(t, i)));
                return 1;
            }
        }
        return 0;  /* no more elements */
    }

ltable.c:137: **findindex()** Lua 5.1.4
    /*
    ** returns the index of a `key' for table traversals. First goes all
    ** elements in the array part, then elements in the hash part. The
    ** beginning of a traversal is signalled by -1.
    */
    static int findindex (lua_State *L, Table *t, StkId key) {
        int i;
        if (ttisnil(key)) return -1;  /* first iteration */
        i = arrayindex(key);
        if (0 < i && i <= t->sizearray)  /* is `key' inside array part? */
        return i-1;  /* yes; that's the index (corrected to C) */
        else {
            Node *n = mainposition(t, key);
            do {  /* check whether `key' is somewhere in the chain */
                /* key may be dead already, but it is ok to use it in `next' */
                if (luaO_rawequalObj(key2tval(n), key) ||
                   (ttype(gkey(n)) == LUA_TDEADKEY && iscollectable(key) &&
                   gcvalue(gkey(n)) == gcvalue(key))) {
                        i = cast_int(n - gnode(t, 0));  /* key index in hash table */
                        /* hash elements are numbered after array ones */
                        return i + t->sizearray;
                }
                else n = gnext(n);
            } while (n);
            luaG_runerror(L, "invalid key to " LUA_QL("next"));  /* key not found */
            return 0;  /* to avoid warnings */
        }
    }

Even for the fastest variant, a table that is a pure array, the above is a lot more effort than what d'Arc does. The worse situation are associative arrays, for example when iterating through a table that has string keys. It looks like every key is **searched for**, although the task is merely to cover them all. This case might make for a geometric loss of performance when iterating through tables using the official API. With d'Arc, the curve may be flat. Sorry for having no hard data for the specific case yet.

### d'Arc sub API

So how does the traversal of a table look on the ground when using d'Arc? The main differences to the official API implementation are:

1. values are accessed directly, skipping stack arithmetics.

2. the avoidance of any searches for keys, simply iterating through the hash.  

First, all table elements in the internal array part of the table are traversed, then, all hash elements:

darc.c:49: **darc\_traverse()**
    int darc_traverse(const Table *t, foldfunc fold, void *cargo) {
        if( darc_hash_part(t, fold, (void *)cargo) )
            return darc_array_part(t, fold, cargo);
        return 0;
    }


<div class=rightinset style='width:60%'>
It's not relevant here - but the gist of it is that Lua automatically balances the hash and the array part of the table and the next power of two plays a role in the decision were to put new elements: in the hash, or in the array part. It is not the case that all integer keys always reside in the array part.
</div>

Note: since it may be confusing to read, the power of two logic in the following is not intrinsic to d'Arc but courtesy of the rehash() function in the original Lua 5.1.4 source, ltable.c:333. 

darc.c:67: **darc\_array\_part()**
    int darc_array_part (const Table *t, foldfunc fold, void *cargo) {
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
                if(!ttisnil(v))
                    if(!(*fold)(v, cargo)) return 0;
            }
        }
        return 1;
    }

darc.c:98: **darc\_hash\_part()**
    int darc_hash_part (const Table *t, foldfunc fold, void *cargo)  {
        int i = sizenode(t);
        while (i--) {
            Node *node = &t->node[i];
            if(!ttisnil(key2tval(node)) && !ttisnil(gval(node))) 
                if(!(*fold)(key2tval(node), cargo)) return 0;
        }
        return 1;
    }

If you read ALL of this, I'd like to hear from you. Please send photo and feedback to hd2010@eonblast.com, or send me a message on github.com.  

Thanks,  
Henning

<center><a href=index.html>Home</a></center>