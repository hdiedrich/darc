d'Arc 0.2.0
## Implementation

**Since you asked.**

As for the example mentioned before: accessing a string. It's illuminating to look at what happens behind the scenes. This page provides some sort of a moderated step-into.  

The official API goes through quite some hops. Let's follow the code for the d'Arc macro **XLUA_STRING()** for Lua 5.1, and compare it to the implementation of string access in the official API in Lua 5.1, which is **lua_tostring()**. 

The difference between the two implementations, i.e. between the official API and d'Arc, is **not** an outrage. And even though the official API workload looks crowded at a casual glance: in most cases, the functions that are called  behind the scenes by the official Lua API do **not** do a lot. But they still cost a function call each. Which in a loop can amount to a significant waste of time. d'Arc on the other hand goes directly to it, calls no functions and needs no stack arithmetics either, which the official API must perform to calculate the position of any *actual* VM value on the stack.

Traversing a table is a different story, d'Arc saves a lot more time there. More on that, below. But let's stick with accessing a string for now:

### d'Arc - String Access

Trace of a string access:

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


### Official API - String Access

Trace of a string access:

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


The above also demonstrates what it is, that d'Arc cuts out. 
 
Again: if you inspect the code closely, you will find that the official Lua API's functions do not do anything too expensive. They calculate a bit, test a bit, but return fast. Most of the body is almost always skipped. Still, it makes a difference to cut this out. Not the least, the mere overhead of function calling.

However, somewhere at the beginning of your own code you have to get a Lua value into your fangs and that will require you to use the usual Lua API calls. The saving starts after the first access.

But for example for traversing a table, you need to go through the Lua API only once, to get the table itself. All nested elements are retrieved directly, using d'Arc.

## Traversal

So let's take a look at a more interesting example, table traversion. This time, Lua 5.1.4 first:

### Lua API

A typical traversal using the Lua C API may look like this (From the [Lua Manual](http://www.lua.org/manual/5.1/manual.html#2.8)):

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

This loop calls lua\_next() for every iteration, which calls luaH\_next(), which uses findindex(). See below.

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

And we are also seeing index2adr() again, as with the string example above: the overhead that results from using the Lua stack mechanism. We'll ignore it at this point, but the payload in this case, getting the type name, is created using lua\_type(), which again calls index2adr(). So as it turns out, in this example specificly, index2adr() is implicitely called three times each iteration.

Usually, this function will return almost immediately. But it's still a function call and some pointer calculating involved, for every value retrieved.

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

Even for the fastest variant, a table that is a pure array, the above is a lot more effort than what d'Arc does. A worse situation are associative arrays, i.e. tables that use string keys. Every key is hashed and potentially searched for, although the task is merely to cover all elements, with no regard to the key. This case can make for geometric loss of performance when iterating through tables using the official API. With d'Arc, the curve can be almost flat. This is the same for both Lua and LuaJIT and caused by the design of the official Lua API.

### d'Arc sub API

So how does the traversal of a table look on the ground when using d'Arc? Basically, you simply use a fold function, with a callback that is applied to each element met.

The main differences to the official API implementation are:

1. values are accessed directly, skipping stack arithmetics.

2. the avoidance of any hashing or searches for keys, simply iterating through the hashes.

The implementation of darc_traverse() looks like this. First, all table elements in the internal array part of the table are traversed, then, all hash elements:

darc.c:49: **darc\_traverse()**
    int darc_traverse(const Table *t, foldfunc fold, void *cargo) {
        if( darc_hash_part(t, fold, (void *)cargo) )
            return darc_array_part(t, fold, cargo);
        return 0;
    }


<div class=rightinset style='width:40%'>
It's not relevant here - but the gist of this is that Lua automatically balances the hash and the array part of the table and the respective next power of two of the currently reserved size plays a role in the decisions of the Lua table implementation as to were to put new elements: in the hash, or in the array part. <b>It is NOT the case that all integer keys always reside in the array part.</b>
</div>

Since it may be confusing to read, please note that the power of two logic in the following source is not intrinsic to d'Arc but courtesy of the original rehash() function in the original Lua 5.1.4 source, ltable.c:333. darc_traverse() is modeled after the table rehash functions, as they represent a highly optimized implementation of a table traversion that the Lua and LuaJIT VMs employ internally, for their own immediate needs. The rehash functions should be considered transparent for the Lua user, they re-size the pre-allocated internal table space when the need arises.

So in the function below, that array part is traversed, one element after the other. There is no actual pointer arithmetic involved. The loop is quite tight.

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

<center><a href=/index.html>Home</a></center>