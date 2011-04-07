d'Arc 0.1.0
## Implementation <a name=implementation></a>

As for the example mentioned before: accessing a string. It's illuminating to look at what happens behind the scenes. The official API goes through quite some hops. Let's follow the code for the d'Arc macro **XLUA_STRING()** for Lua 5.1, and then the implementation of the official API in Lua 5.1, which is **lua_tostring()**. 

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