# d'Arc 0.2.0: samples makefile

default: $(PLAT)
 
clean:
	rm -f *. *.o *.a *.so

# convenience targets for popular platforms

none:
	@echo "Please choose a platform:"
	@echo "   $(PLATS)"

aix:
	@echo "Not tested, please contribute"

ansi:
	@echo "Not tested, please contribute"

bsd:
	@echo "Not tested, please contribute"

freebsd:
	@echo "Not tested, please contribute"

generic:
	@echo "Not tested, please contribute"

linux:
	gcc -O2 -fpic -fomit-frame-pointer -Wall -shared -DLUA_5_1 -I../../include/lua-5.1 -I../../src -o filter.so filter.c ../../src/darc.c
	gcc -O2 -fomit-frame-pointer -Wall -shared -DJIT_2 -I../../include/luajit-2.0 -I../../src -o filterjit.so filter.c ../../src/darc.c
	gcc -O2 -fpic -fomit-frame-pointer -Wall -shared -DLUA_5_1 -DLESS_SKEW_FOR_BENCHMARK -I../../include/lua-5.1 -I../../src -o filter4bench.so filter4bench.c ../../src/darc.c
	gcc -O2 -fomit-frame-pointer -Wall -shared -DJIT_2 -DLESS_SKEW_FOR_BENCHMARK -I../../include/luajit-2.0 -I../../src -o filter4benchjit.so filter4bench.c ../../src/darc.c
	@echo "run a test: lua[jit] samples/filter/filter.lua"
	@echo "benchmark:  lua[jit] samples/filter/filterbench.lua"

linux-64:
	gcc -O2 -fpic -Wall -shared -DLUA_5_1 -I../../include/lua-5.1 -I../../src -o filter.so filter.c ../../src/darc.c
	gcc -O2 -fpic -Wall -shared -DJIT_2 -I../../include/luajit-2.0 -I../../src -o filterjit.so filter.c ../../src/darc.c
	gcc -O2 -fpic -Wall -shared -DLUA_5_1 -DLESS_SKEW_FOR_BENCHMARK -I../../include/lua-5.1 -I../../src -o filter4bench.so filter4bench.c ../../src/darc.c
	gcc -O2 -fpic -Wall -shared -DJIT_2 -DLESS_SKEW_FOR_BENCHMARK -I../../include/luajit-2.0 -I../../src -o filter4benchjit.so filter4bench.c ../../src/darc.c
	@echo "run a test: lua[jit] samples/filter/filter.lua"
	@echo "benchmark:  lua[jit] samples/filter/filterbench.lua"

macosx:
	export MACOSX_DEPLOYMENT_TARGET=10.4
	gcc -Wall -bundle -undefined dynamic_lookup -DLUA_5_1 -I../../src -I../../include/lua-5.1 -o filter.so ../../src/darc.c filter.c
	gcc -Wall -bundle -undefined dynamic_lookup -DJIT_2 -L. -I../../src -I../../include/luajit-2.0 -o filterjit.so ../../src/darc.c filter.c
	gcc -Wall -bundle -undefined dynamic_lookup -DLUA_5_1 -DLESS_SKEW_FOR_BENCHMARK -I../../src -I../../include/lua-5.1 -o filter4bench.so ../../src/darc.c filter4bench.c
	gcc -Wall -bundle -undefined dynamic_lookup -DJIT_2 -DLESS_SKEW_FOR_BENCHMARK -L. -I../../src -I../../include/luajit-2.0 -o filter4benchjit.so ../../src/darc.c filter4bench.c
	@echo "run a test: lua[jit] samples/filter/filter.lua" 
	@echo "benchmark:  lua[jit] samples/filter/filterbench.lua" 

macosx-ppc: 
	export MACOSX_DEPLOYMENT_TARGET="10.3" 
	gcc -bundle -undefined dynamic_lookup -DLUA_5_1 -I../../src -I../../include/lua-5.1 -o filter.so filter.c ../../src/darc.c 
	gcc -bundle -undefined dynamic_lookup -DLUA_5_1 -I../../src -I../../include/lua-5.1 -o filter4bench.so filter4bench.c ../../src/darc.c 

mingw:
	@echo "Not tested, please contribute"

posix:
	@echo "Not tested, please contribute"

solaris:
	@echo "Not tested, please contribute"

# list targets that do not create files (but not all makes understand .PHONY)
.PHONY: all $(PLATS) default o a clean depend echo none

filter.o: filter.c

# --------------------------------------------------------------------------
# Makefile darc/src Apr 2011 - powered by Eonblast http://www.eonblast.com
