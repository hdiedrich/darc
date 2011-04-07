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

# produce doc html from readmes
html:
	@lua etc/markdown.lua --style etc/css/style.css --inline-style README.md
	@sed -E -e 's/url\(\.\.\/\.\.\/etc/url(etc/g' -i .bak README.html
	@lua etc/markdown.lua --style etc/css/style.css --inline-style etc/NAME.md
	@sed -E -e 's/url\(\.\.\/\.\.\/etc/url(..\/etc/g' -i .bak etc/NAME.html
	@sed -E -e 's/src=etc/src=..\/etc/g' -i .bak etc/NAME.html
	@lua etc/markdown.lua --style etc/css/style.css --inline-style LICENSE
	@sed -E -e 's/url\(\.\.\/\.\.\/etc/url(etc/g' -i .bak LICENSE.html
	@cp samples/filter/filter.c samples/filter/filter.md
	@sed -E -e 's/^/    /g' -i .bak samples/filter/filter.md
	@lua etc/markdown.lua --style etc/css/style.css --inline-style samples/filter/filter.md
	@sed -E -e 's/src=etc\/images/src=..\/..\/etc\/images/g' -i .bak samples/filter/filter.html

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
