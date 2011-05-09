# d'Arc 0.2.0 makefile
#
# NOTE: THERE IS NOTHING TO MAKE.
# But you can use this to make samples and benchmarks.

# ---------------------------------------------------------------------

# Your platform. See PLATS for possible values.
PLAT= none

all: $(PLAT)

# use one of these, e.g. 'make linux'
PLATS= linux linux-64 macosx macosx-ppc
# wish list, please contribute: aix ansi bsd freebsd generic mingw posix solaris

# branch off to samples/filter/Makefile
$(PLATS):
	@echo "building a sample: word filter" 
	(cd samples/filter && $(MAKE) $@)

# ---------------------------------------------------------------------

# produce doc html from readmes and original source files
html:
	@lua etc/markdown.lua --style etc/css/style.css --inline-style README.md
	@sed -E -e 's/url\(\.\.\/\.\.\/etc/url(etc/g' -i '' README.html
	
	# name blurb
	@lua etc/markdown.lua --style etc/css/style.css --inline-style doc/NAME.md
	@sed -E -e 's/url\(\.\.\/\.\.\/etc/url(..\/etc/g' -i '' doc/NAME.html
	@sed -E -e 's/src=etc/src=..\/etc/g' -i '' doc/NAME.html
	
	# benchmark page
	@lua etc/markdown.lua --style etc/css/style.css --inline-style doc/BENCHMARKS.md
	@sed -E -e 's/url\(\.\.\/\.\.\/etc/url(..\/etc/g' -i '' doc/BENCHMARKS.html
	@sed -E -e 's/src=etc/src=..\/etc/g' -i '' doc/BENCHMARKS.html
	@lua etc/markdown.lua --style etc/css/style.css --inline-style doc/IMPLEMENTATION.md
	@sed -E -e 's/url\(\.\.\/\.\.\/etc/url(..\/etc/g' -i '' doc/IMPLEMENTATION.html
	@sed -E -e 's/src=etc/src=..\/etc/g' -i '' doc/IMPLEMENTATION.html
	@lua etc/markdown.lua --style etc/css/style.css --inline-style LICENSE
	@sed -E -e 's/url\(\.\.\/\.\.\/etc/url(etc/g' -i '' LICENSE.html
	
	# sample source page        
	@cp samples/filter/filter.c doc/sample.tmp
	@sed -E -e 's/^/    /g' -i '' doc/sample.tmp
	@echo "\n<h1>d'Arc 0.2.0</h1>\n" > doc/sample.md
	@echo "This is a bare bones sample. The interesting stuff is right at the top.\n" >> doc/sample.md
	@echo "**sample/filter/filter.c**" >> doc/sample.md
	@cat doc/sample.tmp >> doc/sample.md
	@echo "\nRun it by doing: lua[jit] samples/filter/filter.lua." >> doc/sample.md
	@lua etc/markdown.lua --style etc/css/style.css --inline-style doc/sample.md
	@sed -E -e 's/src=etc\/images/src=..\/etc\/images/g' -i '' doc/sample.html
	@sed -E -e 's/..\/..\/etc\/images/..\/etc\/images/g' -i '' doc/sample.html
	
	# bench source page        
	@cp samples/filter/filter4bench.c doc/bench.tmp
	@sed -E -e 's/^/    /g' -i '' doc/bench.tmp
	@echo "\n<p><b>d'Arc 0.2.0</b></p><h2>Benchmarks</h2>\n" > doc/bench.md
	@echo "This is the benchmark code, functionally the same as the briefer <a href=sample.html>filter sample</a>.\n" >> doc/bench.md
	@echo "**samples/filter/filter4bench.c**" >> doc/bench.md
	@cat doc/bench.tmp >> doc/bench.md
	@echo "\nRun it by doing: lua[jit] samples/filter/filterbench.lua." >> doc/bench.md
	@lua etc/markdown.lua --style etc/css/style.css --inline-style doc/bench.md
	@sed -E -e 's/src=etc\/images/src=..\/etc\/images/g' -i '' doc/bench.html
	@sed -E -e 's/..\/..\/etc\/images/..\/etc\/images/g' -i '' doc/bench.html
	
	# darc source pages: .c .h
	@cp src/darc.h doc/darc.h.md
	@cp src/darc.c doc/darc.c.md
	@sed -E -e 's/^/    /g' -i '' doc/darc.h.md
	@sed -E -e 's/^/    /g' -i '' doc/darc.c.md
	@echo "\n<h1>d'Arc 0.2.0</h1>\n" > doc/src.md
	@echo "This is the complete d'Arc 0.2 source\n" >> doc/src.md
	@echo "**darc.h**" >> doc/src.md
	@cat doc/darc.h.md >> doc/src.md
	@echo "**darc.c**" >> doc/src.md
	@cat doc/darc.c.md >> doc/src.md
	@echo "\nAnd here is a <a href=sample.html>sample</a> on how to use it." >> doc/src.md
	@lua etc/markdown.lua --style etc/css/style.css --inline-style doc/src.md
	@sed -E -e 's/src=etc\/images/src=..\/etc\/images/g' -i '' doc/src.html
	@sed -E -e 's/..\/..\/etc\/images/..\/etc\/images/g' -i '' doc/src.html

# ---------------------------------------------------------------------

# print help
none:
	@echo "d'Arc is used by compiling it into your C source."
	@echo "There is nothing to be made for that, but samples:"
	@echo 
	@echo "Building d'Arc samples"
	@echo "----------------------"
	@echo "To build the d'Arc 'word filter' sample, please do"
	@echo "    make <PLATFORM>"
	@echo 
	@echo "where <PLATFORM> is one of these:"
	@echo "    $(PLATS)"
	@echo 
	@echo "    e.g. 'make linux-64'"
	@echo 
