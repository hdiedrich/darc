# d'Arc 0.0.1 (sketch)

d'Arc is a small, fast, straight forward, you better read-only, **sub-API** interface to Lua data for both Lua 5.1.4 and LuaJIT 2 beta 6. It's not for the faint of heart but really sweet. Using it may get you fired. Or promoted. Or both, just like Jeanne. 

As evidenced by the existence of LuaJIT, speed can play a role in Lua projects. d'Arc lets you read out variables and tables as fast as possible, for both Lua and LuaJIT main branch versions, skipping the part where it's copied and  put on the stack. Traversing a table, and reading out a variable wastes a lot of time when done right, i.e. using the API. Which - life can be so confusing - implies that under some circumstances doing it right maybe is not doing it right after all. It's complicated. Jeanne to the rescue. d'Arc is faster, use it for in-house read-only stuff that you can control. 

<div class=rightinset style='width:40%'>
With Jeanne, in the end, it didn't really matter were the voices came from that she heard. She was highly effective for them and kicked some butt, they so hated her for it, just killing her wouldn't do. Same for your projects: in the face of world history, maybe it matters less that inevitably some people will feel you are cheating when using something as dirty as d'Arc to be faster than a nice, clean API call. To wit, playing by the rules will safe them re-doing 50 lines of code in case Lua publishes a next version but will waste gazillions of cpu cycles that translate into millions of wasted Megawatts, wasted life time and dead trees! Right. So do the right thing, revel in their frantic screams and do something nice. And dirty. And even green, too!
</div>

And watch out when you get all enthusiastic and start feeling like writing to the variables. That could have all sorts of funky consequences that you will not have plumbed until you tested gc, longjumping, yielding and parallel processes. Stay with reading unless you know what you are doing, which is also the requirement to use this in the first place, in case that hadn't transpired yet.

Maybe memorize it like this: Jeanne listened to God and that worked great for a while. But it didn't necessarily work the other way around when she got in trouble. See, that's the quintessential read-only. God is listening. But you better take care of the action part down here.
