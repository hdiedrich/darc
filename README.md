# d'Arc 0.2.0

**d'Arc is a small, fast, straight forward, you better read-only, sub-API interface to Lua data for both Lua 5.1.4 and LuaJIT 2 beta 7. It's not for the faint of heart but really sweet. Using it may get you fired. Or promoted. Or both, just like [Jeanne](http://hdiedrich.github.com/darc/etc/NAME.html).**

d'Arc lets you read out Lua variables and tables as [fast](http://hdiedrich.github.com/darc/doc/BENCHMARKS.html) as possible, for both Lua and LuaJIT. Done right, i.e. using the official Lua API, you will always be wasting quite a bit of time when traversing a table, or reading out a variable. Which - life can be so confusing - implies that under some circumstance doing it right may not be right after all. Like, if performance matters. It's complicated. Jeanne to the rescue. 

Using d'Arc allows you to avoid the repeat look ups, hash calculations and stack writes that bog down table traversion and variable access when using the official API for Lua or LuaJIT. And once you have accessed a variable, you keep a direct C pointer to its value, rather than going via a look up and the stack again. When traversing a table, d'Arc uses the existing, implicit pointers to the next elements rather than doing separate look ups for each and every individual key, as the official API does.

As an example, here is the [implementation](http://hdiedrich.github.com/darc/doc/IMPLEMENTATION.html) of d'Arc vs. the official API, at the C level. On the face of it, d'Arc looks like this:

    const char *s = XLUA_STRING(tval); 
    
... instead of going via the Lua state and stack, like this:

    const char *s = lua_tostring (lua_State *L, int index);


<div class=rightinset style='width:40%; color:#fff0f0;'>
With Jeanne, in the end, it didn't really matter were the voices came from that she heard. She was highly effective for them and kicked some butt. They so hated her for it, just killing her wouldn't do. Same for your projects: in the face of world history, maybe it matters less that inevitably some people will feel you are cheating when using something as dirty as d'Arc to be faster than a nice, clean API call. To wit, playing by the rules will safe them re-doing 50 lines of code in case Lua publishes a next version but will waste gazillions of cpu cycles that translate into millions of wasted Megawatts, wasted life time and dead trees! Right. So do the right thing, revel in their frantic screams and do something nice. And dirty. And even green, too!
</div>

The calls are not only different in form but as the different parameters suggest, they are used in different places. Which is what provides for the performance gain. With d'Arc you use pointers to variable structures (e.g. 'tval'), not the Lua state and stack index (L, and index).

Watch out when you get all enthusiastic and start feeling like writing to the variables. That can have all sorts of consequences that you will not have plumbed until you thought about string interning, garbage collection, longjumping, yielding and parallel processes. Stay with reading unless you know what you are doing. Which, anyway, happens to be the requirement for using d'Arc in the first place. In case that hadn't transpired yet. 

Maybe memorize it like this: Jeanne listened to God and that worked great for a while. But it didn't necessarily work the other way around when she got in trouble. The quintessential read-only: God is listening. But he likes you to take care of the action.

## Usage

See [sample/filter/filter.c](http://hdiedrich.github.com/darc/doc/sample.html)

Basically you get a bunch of oyster shucker macros for direct access of the actual contents of the VM value structures, and to test for the content types (string, number, table etc).

Plus, one function to traverse a table in low-level flight.

The macros and that fold function do not alter the Lua stack and no values are copied. You could write to and change the variable contents, but that's not the intention at this point. There are side effects, e.g. strings are [interned](http://en.wikipedia.org/wiki/String_interning) so changing a string will change all of the same value, etc.

**1) Stack Access**

    TValue *o = XLUA_INDEX_TO_ADDRESS(L, index);

This returns a TValue pointer of a value residing on the Lua stack. Usually, that's your starting point. As mentioned before, d'Arc does not alter the Lua stack.

If you're only starting out with Lua C programming: the Lua stack is not the C stack, it's a specific, Lua VM structure used for the values the Lua VM swishes around.

**2) Type Tests**

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

These take a TValue pointer as argument (o) and return 0 (false) or 1 (true).


**3) Value Access**

    XLUA_BOOLEAN(o)   		
    XLUA_NUMBER(o)   	        
    XLUA_STRING(o)          	
    XLUA_STRING_LENGTH(o)   	
    XLUA_TABLE(o)				

These also take a TValue pointer as argument (o) and return int, int, char *, size_t, TValue respectively. Remember, Lua allows \\0 as part of strings, and strings are interned.

And to say that again: the above compiles for both Lua and LuaJIT, that's the point.

**4) Table Traversal**

    int darc_traverse(const Table *t, foldfunc fold, void *state);

This function is used to traverse a table, see the sample below. It is implemented in a fashion akin to the internal rehash functions of the VMs, which of course are highly optimized. The function applies the 'foldfunc' callback function that you define, to every table element. 

    typedef int (*foldfunc)(TValue *o, void *state);

The sample below will make things quite clear.

**5) Table Entry Access**

    TValue *o = XLUA_NODE_KEYVAL(node); 	
    TValue *o = XLUA_NODE_VAL(node);		

Less frequently of interest, these functions are used on the 'nodes' that tables consist of: each node combines one key and its value. the results of these macros are TValue pointers that e.g. can be used as arguments to the type test and value access macros listed above. You may find use for these two macros, if you started decomposing and reinterpreting the darc.c source, to traverse tables yet *more* streamlined than by using darc_traverse(). Otherwise,  you probably won't run into nodes in the beginning. But maybe once you dig deeper, so here you are.


## Sample

A swearword filter, which returns true when a bad word is found in a Lua value:

    > v={{'help!',{22, {'Oh fuck', 1}}, nil}
    > print(filter.check(v))
    true

The source is in [sample/filter/filter.c](http://hdiedrich.github.com/darc/doc/sample.html). 

Most of that file is boilerplate to get it to play with Lua as a library. The core of it is the following function. This is such a **foldfunc** function, as discussed above, which is of the type

    typedef int (*foldfunc)(TValue *o, void *state);

This function is the central vehicle of the table traversal in d'Arc:  

1. the foldfunc is passed a Lua VM value to process, TValue *o. This is a table element, and it can also be a nested table.

2. if you need to collect state for your work inside the table, and to return results, you can pass a data structure in using the **state** pointer  

3. if foldfunc returns 0, the traversal is stopped, and at the top level, 0 is returned

In the case of the filter.c sample, you can follow how the foldfunc *indirectly calls itself*, by passing itself to darc_traverse as function pointer. This should be the usual way to traverse into nested tables.
 
    int lame_word_filter(TValue *subject, void *state)
    {
        if(XLUA_IS_STRING(subject))
        
            /* actually act on the string. */
            return inspect(XLUA_STRING(subject)); 
            
        else if(XLUA_IS_TABLE(subject))
    
            /* traverse down into a table. */
            return darc_traverse(XLUA_TABLE(subject), lame_word_filter, state); 
        
        /* ignore all other types. */
        return 1;
    }

Generally, traversing down into a *nested* table that you encounter, should be *initiated* in that foldfunc. Which is completely straight forward: since where you start from is usually a TValue, which could be a table, or not. Therefore, seen from the outer, Lua-registered function, you would usually start with a call to your 'foldfunc', not darc\_traverse(). It's your foldfunc that then calls darc\_traverse() first, if appropriate. (Or it does something else completely, e.g. in case the top value is not a table anyway.) Now, because your foldfunc will usually call darc\_traverse() with itself as argument, that darc_traverse() call inside your foldfunc will be the one and only call you write, and will handle all nested tables recursively. Are you with me? If you know a better way to put this, don't hesitate to let me know.
        
And, again: this source compiles for Lua and LuaJIT, even though it doesn't use the official API. It accesses the values more directly, eg. using the macros XLUA\_IS\_STRING(), XLUA\_STRING() etc. On the other hand, the source shown here is not optimized to the hilt so as to remain instructive.

The above function lame\_word\_filter() is tied into Lua by this generic function:

    static int filter_check(lua_State *L) {
    
        lua_settop(L, 1);
        const TValue *subject = index2addr(L, -1);
        lua_pop(L, 1);
    
        int ok = lame_word_filter(subject, (void *)0);
        lua_pushboolean(L, !ok);
    
        return 1;	
    }

For this sample, the **integer return value** is quite usefull. It suffices to report back 'yes' or 'no'. And that's what it always does, it signals if the traversal was complete (1) or broken off (0). In this case, that is used to report back whether an offensive word was found, or not. For more interesting stuff, you would use the state pointer.

Please send feedback, advise and corrections to hd2010@eonblast.com, or send me a [message](https://github.com/inbox/new/hdiedrich), or open an [issue](https://github.com/hdiedrich/darc/issues) on github.com.  

Thanks,  
Henning

<center>[Home](http://hdiedrich.github.com/darc/index.html)</center>