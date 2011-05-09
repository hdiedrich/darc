-------------------------------------------------------------------------------
--- Package     : Fleece - fast Lua to JSON module                          ---
--- File        : bench9y.lua                                               ---
--- Description : Fleece graphical benchmarks 'vs' literal Lua              ---
--- Version     : 0.4.0 / alpha                                             ---
--- Copyright   : 2011 Henning Diedrich, Eonblast Corporation               ---
--- Author      : H. Diedrich <hd2010@eonblast.com>                         ---
--- License     : see file LICENSE                                          ---
--- Created     : 25 Mar 2011                                               ---
--- Changed     : 25 Mar 2011                                               ---
-------------------------------------------------------------------------------
---                                                                         ---
---  ASCII plotted graph showing performance relative to element count.     ---
---  This runs Fleece vs literal Lua code: a loop and concat                ---
---                                                                         ---
---  Use: lua test/bench9y.lua                                              ---
---                                                                         ---
-------------------------------------------------------------------------------

lib = "darc"
STEPS    = 30
ELEMENTS = STEPS * 40
CYCLES   = 50000
BESTOF = 3
GRAPH_HEIGHT = 12
VERBOSITY = 1

print("Fleece Benchmark Graph, vs LuaJSON C library")
print("==========================================================")
print("A couple of random tables are created and speed plotted.")
print("You should have built fleece first with 'make <PLATFORM>' ")
print("and built the bundled LuaJSON by 'make <PLATFORM>-test',")
print("and should now be in the fleece root directory.")

package.cpath="src/?.so"
fleece = require(lib)
-- package.path="etc/json4/?.lua"
-- json4 = require("json")
package.cpath="etc/luajson/?.so"
luajson = require("luajson")

-- luajson stuff
local base = _G
local json = luajson
json.null = {_mt = {__tostring = function () return "null" end, __call = function () return "null" end}}
base.setmetatable(json.null, json.null._mt)

sep = "------------------------------------------------------------------------------"
subsep= ".............................................................................."

printcol = 0
function printf(...)
	s = string.format(...)
	if(s:find("\n")) then printcol = 0 else printcol = printcol + s:len() end
    io.write(s)
end

-- verbosity controlled conditional printing
function printfv(vl,...)
    if vl <= VERBOSITY then printf(...) end
end

function tab(x)
	while(printcol < x) do io.write(" "); printcol = printcol + 1 end
end

function tabv(vl,x)
    if vl <= VERBOSITY then tab(x) end
end

function nanosecs_per(time, per)
	return time * 1000000000 / per
end

function microsecs_per(time, per)
	return time * 1000000 / per
end

local abc = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'}
function randstr(i)
      local r = {}
      local length = math.random(1,20)
      local i
      for i = 1, length do
         r[i] = abc[math.random(1, 26)]
      end

      return table.concat(r)
end

function prcstr(part, base)
    if secnd == 0 then return 0 end
    x = math.floor(part / base * 100)
    if(x <= 2) then
        x = math.floor(part / base * 1000) / 10
    end
    return x
end

function stringify(t) table.val_to_str( t ) end


-----------------------------------------------------------------------
-- decimal point
-----------------------------------------------------------------------
-- RichardWarburton http://lua-users.org/wiki/FormattingNumbers

function comma_value(n)
	local left,num,right = string.match(n,'^([^%d]*%d)(%d*)(.-)$')
	return left..(num:reverse():gsub('(%d%d%d)','%1,'):reverse())..right
end

-----------------------------------------------------------------------
-- plotter
-----------------------------------------------------------------------
function tlimits(t)
    if #t == 0 then return nil,nil end
    min = t[1] 
    max = t[1]
    for _,v in next, t do 
        min = math.min(min,v)
        max = math.max(max,v)
    end
    return min,max
end

function plot(p1, p2, name1, name2)

    imax = math.max(#p1,#p2)
    vmin1,vmax1 = tlimits(p1)
    vmin2,vmax2 = tlimits(p2)
    
    if vmax1 == nil and vmax2 == nil then
        print("Can't plot ", vmin1, vmax2, vmin2, vmin2)
        return
    end

    if vmin1 == nil then vmin1 = 0 end
    if vmax1 == nil then vmax1 = 0 end
    if vmin2 == nil then vmin2 = 0 end
    if vmax2 == nil then vmax2 = 0 end

    vmin = math.max(vmin1,vmin2)
    vmax = math.max(vmax1,vmax2)
    bar  = 10 ^ (math.ceil(math.log10(vmax / 100))) 
    ymax = math.max(10,math.ceil( vmax / bar ) * bar)
    steps = GRAPH_HEIGHT
    step = math.ceil(ymax / steps / 10) * 10
    
    
    print("nsec/elem    -- Nanoseconds per table element --  ")
    
    -- print(#p1, #p2, vmin, vmax, steps, step)
    
    ylast = ymax * 10

    for y = ymax,0,-step do
    
        p1[1] = 0
        p2[1] = 0
        plotline(y, ylast, p1, p2)

        ylast = y

    end

    if ylast ~= 0 then
        plotline(0, ylast, p1, p2)
    end

    -- x axis legend
    ----------------
    xl = {}
    for items  = 0,ELEMENTS,math.ceil(ELEMENTS / STEPS) do
        if items == 0 then items = 1 end
    -- clone of this loop head at (*) !!
        xl[#xl+1] = items
    end

    io.write(string.format("%12.12s   ", "elements:"))
    for x = 1,imax,10 do
        leg = "^" .. xl[x]
        io.write(leg)
        for li = 1,10-leg:len()-1 do io.write(' ') end
    end
    print()
    -- print("Fleece 0.4.0")

end

function plotline(y, ylast, p1, p2)

        io.write(string.format("%10d  |  ", y))
        local s = {}
        local u1, u2
        for x = 1,imax,1 do
            local y1 = p1[x]
            local y2 = p2[x]
            -- hit?
            if y1 == nil then y1 = 0 end
            if y2 == nil then y2 = 0 end
            if y1 ~= nil and y1 >= y and y1 < ylast then 
                u1 = true
            else
                u1 = false
            end
            if y2 ~= nil and y2 >= y and y2 < ylast then 
                u2 = true
            else
                u2 = false
            end
            -- plot
            if u1 == true and u2 == true then io.write('*')
            elseif u1 then io.write('+')
            elseif u2 then io.write('x')
            elseif y == 0 then
                if y1==nil or y2==nil then io.write(':')
                else io.write('-')
                end
            else io.write(' ')
            end
        end
        
        if u1 then 
            min,max = tlimits(p1)
            if u2 then io.write("+: ") end
            io.write(string.format(" [%d..%d]   ", min, max))
        end

        if u2 then 
            if u1 then io.write("*: ") end
            min,max = tlimits(p2)
            io.write(string.format(" [%d..%d]", min, max))
        end

        print()

end


-----------------------------------------------------------------------
-- mini json stringify for output
-----------------------------------------------------------------------
-- from http://lua-users.org/wiki/TableUtils
function table.val_to_str ( v )
  if "string" == type( v ) then
    v = string.gsub( v, "\n", "\\n" )
    if string.match( string.gsub(v,"[^'\"]",""), '^"+$' ) then
      return "'" .. v .. "'"
    end
    return '"' .. string.gsub(v,'"', '\\"' ) .. '"'
  else
    return "table" == type( v ) and table.tostring( v ) or
      tostring( v )
  end
end

-- from http://lua-users.org/wiki/TableUtils
function table.key_to_str ( k )
  if "string" == type( k ) and string.match( k, "^[_%a][_%a%d]*$" ) then
    return k
  else
    return "[" .. table.val_to_str( k ) .. "]"
  end
end

-- from http://lua-users.org/wiki/TableUtils
function table.tostring( tbl )
  local result, done = {}, {}
  for k, v in ipairs( tbl ) do
    table.insert( result, table.val_to_str( v ) )
    done[ k ] = true
  end
  for k, v in pairs( tbl ) do
    if not done[ k ] then
      table.insert( result,
        table.key_to_str( k ) .. "=" .. table.val_to_str( v ) )
    end
  end
  return "{" .. table.concat( result, "," ) .. "}"
end

local t = {}
local function measure(items, prepP, prepare, actionP, action, printPrepP)

  local cycles = math.max(1,math.ceil(CYCLES / items))
  local clock  = os.clock

  printfv(2, "%dx %-12s ", cycles, actionP)

  if prepare ~= nil then
    t = {}
    local size = 0
    for i = 1, items, 1 do prepare(i) end
    print("prep done")
  end

  local last = nil
  local best = nil

  for k = 1,BESTOF do
      local tm = clock()
      for i = 1, cycles do last = action(i) end
      if best == nil then
          best = clock() - tm
      else
          best = math.min(best, clock() - tm)
      end
  end  
  tm = best
  
  --if DYN and tm == 0 then CYCLES = math.ceil(CYCLES * 4) end 
  --if DYN and tm > 1 then CYCLES = math.ceil(CYCLES / 2) end 

  if tm ~= 0 then
  	 mspc= nanosecs_per(tm, cycles * items)
  	 tabv(2,27)
  	 printfv(2, "%10.0fns/element ", mspc)
  else
	  mspc = nil
	  printfv(2, "%dx %-12s ** sample too small, could not measure, increase CYCLES ** ", cycles, actionP)
  end
  
  return mspc, last 

end

local function measure2(prepP, prepare, prompt1, action1, prompt2, action2)

    if(_PATCH) then io.write(_PATCH) else io.write(_VERSION .. ' official') end
    print(" - Fleece 0.4.0")
    print(sep)
    printf("%d - %d elements in %-25s\n", 1, ELEMENTS, prepP)
    print("+: " .. prompt1)
    print("x: " .. prompt2)
    print(sep)

    t2 = {}; t3 = {}
    for items  = 1,ELEMENTS,math.ceil(ELEMENTS / STEPS) do
        if items == 0 then items = 1 end
    -- clone of this loop head at (*) !
        
        if(VERBOSITY < 2) then io.write('.'); io.flush() end
        
        t = prepare(items)

        if anyprev then printfv(2, subsep) end
        printfv(2, "%d elements in %-25s\n", items, prepP)
        printfv(2, subsep)

        secnd, r2 = measure(items, prepP, nil, prompt1, action1, true)
        if secnd == nil then secnd = 0 end
        t2[#t2+1] = secnd        
        printfv(2, "     %.20s.. \n", r2)

        third, r3 = measure(items, prepP, nil, prompt2, action2)
        if third == nil then third = 0 end
        t3[#t3+1] = third
        
        if(secnd and third) then prc = prcstr(third, secnd)
            printfv(2, "%3g%% %.20s.. \n", prc, r3)
        else 
            prc = "-" 
            printfv(2, "     %.20s.. \n", r3)
        end
       
        anyprev = true
    end

    if(VERBOSITY < 2) then print() end
    
    plot(t2, t3, name2, name3)
    
end

name2 = "official API"
name3 = "darc 0.2"

measure2("t = {{'help!',{22, {'Oh damn.', 1}, 'foo'}, 'luck', 'struck'}, nil}:",
        "",
        function(size) local t; t={}; for i=1,size do t[i]=i end; return t; end,
		"function(i) s='[';i=1;while(i<=#t)do s=s..t[i]..',';i=i+1;end;return s;end",
		function(i) s='['; i=1; while(i<=#t) do s=s..t[i]..','; i = i + 1; end; return s; end,
		"fleece.json(t)",
		function(i) return fleece.json(t) end
		)

