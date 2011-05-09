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
--- File        : samples/filter/filterbench.lua                            ---
--- Description : benchmarks the filter sample, using filter4bench.c        ---
--- Version     : 0.2.0 / alpha                                             ---
--- Copyright   : (c) 2011 Henning Diedrich                                 ---
--- Author      : H. Diedrich <hd2010@eonblast.com>                         ---
--- License     : see file LICENSE                                          ---
--- Created     : 07 Apr 2011                                               ---
--- Changed     : 08 May 2011                                               ---
-------------------------------------------------------------------------------

-- you built the sample filter lib using make in project root
package.cpath="./?.so;samples/filter/?.so"
if(jit == nil) then lib = "filter4bench" else lib = "filter4benchjit" end
filter = require(lib)

print(" (")
print("(_) Swearword filter - a d'Arc benchmark ("..lib..")")

ELEMENTS = 1000
CYCLES   = 1000

math.randomseed( os.time() )
math.random()

sep = "---------------------------------------------------------------------------------"

printcol = 0
function printf(...)
	s = string.format(...)
	if(s:find("\n") or s:len() == 0) then printcol = 0 else printcol = printcol + s:len() end
    io.write(s)
end

function tab(x)
	while(printcol < x) do io.write(" "); printcol = printcol + 1 end
end

function nanosecs_per(time, per)
	return time * 1000000000 / per
end

function microsecs_per(time, per)
	return time * 1000000 / per
end

local abc = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
		     'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'}

local esc = { '\"', '\\', '\b', '\f', '\r', '\n', '\t',
		     string.char(1), string.char(60), string.char(127) }

local chance = 20
		     
local abc_num = #abc
local esc_num = #esc

function randstr(i)
      local r = {}
      local length = math.random(1,10)
      local i
      for i = 1, length do
      	if(math.random(1,chance) == 1) then
	        r[i] = esc[math.random(1, esc_num)] -- esc!
    	else
			r[i] = abc[math.random(1, abc_num)] -- normal
      	end
      end
      return table.concat(r)
 end

function prcstr(part, base)
    if base == 0 or base == nil or part == nil then return 0 end
    x = math.floor(part / base * 100)
    if(x <= 2) then
        x = math.floor(part / base * 1000) / 10
    end
    return x
end

function escstr(x, max)
    s = ""
    len = math.min(x:len(), max)
    i = 1
    while(i <= len) do
        c = string.sub(x,i,i) 
        if(c:byte() < 32 or c:byte() == 127) then c = "#" end 
        s = s .. c
        i = i + 1
    end
    return s
end

function str(b)
	if (b) then return "true" else return "false" end 
end


t = nil
local function measure(prepP, prepare, actionP, action, printPrepP)

  local items  = ELEMENTS
  local cycles = CYCLES
  local clock  = os.clock

  if(printPrepP) then
    printf("%d elements in %-25s\n", items, prepP)
  end
  printf("%6dx %-30s ", cycles, actionP)

  if (t == nil) then
      t = {}
	  local i = 1
	  local size = 0
	  -- while(i <= items) do prepare(i); i = i + 1 end
	  prepare()
  end

  collectgarbage()

  i = 1
  local last = nil
  local tm = clock()
  while(i <= cycles) do last = action(i); i = i + 1 end
  tm = clock() - tm
  if tm ~= 0 then
  	 mspc= nanosecs_per(tm, cycles * items)
  	 tab(27)
  	 printf("%10.0fns/element ", mspc)
  else
	  mspc = nil
	  printf("%dx %-12s ** sample too small, could not measure. increase CYCLES. ** ", cycles, actionP)
  end
  
  return mspc, last 
end

t = nil
local function measure_i(prepP, prepare, actionP, action, printPrepP)

  local items  = ELEMENTS
  local cycles = CYCLES
  local clock  = os.clock

  if(printPrepP) then
    printf("%d elements in %-25s\n", items, prepP)
  end
  printf("%6dx %-30s ", cycles, actionP)

  if (t == nil) then
      t = {}
	  local i = 1
	  local size = 0
	  while(i <= items) do prepare(i); i = i + 1 end
  end

  collectgarbage()

  i = 1
  local last = nil
  local tm = clock()
  while(i <= cycles) do last = action(i); i = i + 1 end
  tm = clock() - tm
  if tm ~= 0 then
  	 mspc= nanosecs_per(tm, cycles * items)
  	 tab(27)
  	 printf("%10.0fns/element ", mspc)
  else
	  mspc = nil
	  printf("%dx %-12s ** sample too small, could not measure. increase CYCLES. ** ", cycles, actionP)
  end
  
  return mspc, last 
end

if(jit == nil) then version = _VERSION else version = jit.version end
if(_PATCH) then io.write(_PATCH) else io.write(version .. ' official') end
print(" - d'Arc 0.2.0")


local function measureX(prepP, prepare, prompt1, action1, prompt2, action2)

  print(sep)
  t = nil

	first, r1 = measure(prepP, prepare, prompt1, action1, true)
	printf("        %s \n", str(r1))

	submeasure(prepP, prepare, prompt2, action2, first)


end

local function measureXi(prepP, prepare, prompt1, action1, prompt2, action2)

  print(sep)
  t = nil

	first, r1 = measure_i(prepP, prepare, prompt1, action1, true)
	printf("        %s \n", str(r1))

	submeasure(prepP, prepare, prompt2, action2, first)


end

function submeasure(prepP, prepare, prompt, action, first)

	third, r = measure(prepP, prepare, prompt, action)
	prc = prcstr(third, first)
	printf("%3g%%, ", prc)
	printf("  %s\n", str(r))

end


ELEMENTS = 12
CYCLES   = 1000000

print(sep)

measureX("t={{'help!',{22, {'Oh damn.', 1}, 'foo'}, 'dork', 'struck'}, nil} ",
		function() t={{'help!',{22, {'Oh damn.', 1}, 'foo'}, 'luck', 'struck'}, nil} end,
		"filter.check_official(t)",
		function() return filter.check_official(t) end,
		"filter.check(t)",
		function() return filter.check(t) end
		)		

ELEMENTS = 10
CYCLES   = 1000000

measureXi("t[randstr(i)]=randstr(i)",
		function(i) t[randstr(i)] = randstr(i) end,
		"filter.check_official(t)",
		function() return filter.check_official(t) end,
		"filter.check(t)",
		function() return filter.check(t) end
		)		

ELEMENTS = 1000
CYCLES   = 10000

measureXi("t[randstr(i)]=randstr(i)",
		function(i) t[randstr(i)] = randstr(i) end,
		"filter.check_official(t)",
		function() return filter.check_official(t) end,
		"filter.check(t)",
		function() return filter.check(t) end
		)		

ELEMENTS = 10000
CYCLES   = 1000

measureXi("t[randstr(i)]=randstr(i)",
		function(i) t[randstr(i)] = randstr(i) end,
		"filter.check_official(t)",
		function() return filter.check_official(t) end,
		"filter.check(t)",
		function() return filter.check(t) end
		)		

ELEMENTS = 100000
CYCLES   = 100

measureXi("t[randstr(i)]=randstr(i)",
		function(i) t[randstr(i)] = randstr(i) end,
		"filter.check_official(t)",
		function() return filter.check_official(t) end,
		"filter.check(t)",
		function() return filter.check(t) end
		)		

ELEMENTS = 10000
CYCLES   = 10000

measureXi("t[i*100]=randstr(i)",
		function(i) t[i*100] = randstr(i) end,
		"filter.check_official(t)",
		function() return filter.check_official(t) end,
		"filter.check(t)",
		function() return filter.check(t) end
		)		
