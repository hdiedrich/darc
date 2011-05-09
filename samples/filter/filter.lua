--       (                             (                                  
--       )\ )      (     )        d'Arc)\ )                      (        
--      (()/(  (   )\ ( /(   (   (    (()/(    )     )           )\   (   
--       /(_)) )\ ((_))\()) ))\  )(    /(_))( /(    (     `  )  ((_) ))\  
--      (_))_|((_) _ (_))/ /((_)(()\  (_))  )(_))   )\  ' /(/(   _  /(( ) 
--      | |_   (_)| || |_ (_))   ((_) / __|((_)_  _((_)) ((_)_\ | |(_)) /   
--      | __|  | || ||  _|/ -_) | '_| \__ \/ _` || '  \()| '_ \)| |/ -_)  
--      |_|    |_||_| \__|\___| |_|   |___/\__,_||_|_|_| | .__/ |_|\___|  
--          .    '            . .             .      ' ..|_|       '    
--      This is a sample for d'Arc 0.2.0, a Lua sub API for Lua and LuaJIT
--
--      Swearword filter: returns true when a swear is found in a table.
--
--      =filter.check("dork")
--      true
--
--      =filter.check({"nice", "tender", "bitch")
--      true
--
--      This is not even 100% fictitious, word filters are historically
--      annoyingly resource consuming. Still only for illustration. 
--
--
-------------------------------------------------------------------------------
--- Package     : d'Arc - fast Lua sub API                                  ---
--- File        : sampels/filter/filter.lua                                 ---
--- Description : executes the filter.c sample file                         ---
--- Version     : 0.2.0 / alpha                                             ---
--- Copyright   : (c) 2011 Henning Diedrich                                 ---
--- Author      : H. Diedrich <hd2010@eonblast.com>                         ---
--- License     : see file LICENSE                                          ---
--- Created     : 07 Apr 2011                                               ---
--- Changed     : 08 May 2011                                               ---
-------------------------------------------------------------------------------


-- you have built the sample filter lib using make in project root
package.cpath="./?.so;samples/filter/?.so"
if(jit == nil) then lib = "filter" else lib = "filterjit" end
filter = require(lib)

print(" (")
print("(_) Swearword filter - a d'Arc sample ("..lib..")")

-- three samples 
io.write("'Jeanne': "); 
print(filter.check("Jeanne"))

io.write("{'Jeanne','d\'Arc that bitch'}: "); 
print(filter.check({"Jeanne","d'Arc that bitch"}))

io.write("{{'help!',{22, {'Life is a beach.', 1}, 'foo'}, 'luck', 'struck'}, nil}: "); 
print(filter.check({{"help!",{22, {"Life is a beach.", 1}, "foo"}, "luck", "struck"}, nil}))

