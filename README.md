# d'Arc 0.1.0

d'Arc is a small, fast, straight forward, you better read-only, **sub-API** interface to Lua data for both Lua 5.1.4 and LuaJIT 2 beta 6. It's not for the faint of heart but really sweet. Using it may get you fired. Or promoted. Or both, just like [Jeanne](etc/NAME.html). 

As evidenced by the existence of LuaJIT, speed can play a role in Lua projects. d'Arc lets you read out Lua variables and tables [as fast as possible](doc/BENCHMARKS.html), for both Lua and LuaJIT main branch versions, skipping the slow parts where you'd search for keys and propagate values via the Lua stack. Traversing a table, and reading out a variable, wastes a lot of time when done right, i.e. using the official API. Which - life can be so confusing - implies that under some circumstances doing it right may not be doing it right after all. It's complicated. Jeanne to the rescue. d'Arc is faster, use it for in-house read-only stuff that you can control. 


The main point is, that you could traverse tables faster and access variables more directly. It may be helpful to look at the  [implementation](doc/IMPLEMENTATION.html) of d'Arc vs the official API at the C level. On the face of it, d'Arc looks like this:

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



If you read ALL of this, I'd like to hear from you. Please send photo and feedback to hd2010@eonblast.com, or send me a message on github.com.  

Thanks,  
Henning

<center><a href=index.html>Home</a></center>