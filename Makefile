
# produce doc html from readmes
html:
	@lua etc/markdown.lua --style etc/css/style.css --inline-style README.md
	@sed -E -e 's/url\(\.\.\/\.\.\/etc/url(etc/g' -i .bak README.html
	@lua etc/markdown.lua --style etc/css/style.css --inline-style etc/NAME.md
	@sed -E -e 's/url\(\.\.\/\.\.\/etc/url(..\/etc/g' -i .bak etc/NAME.html
	@sed -E -e 's/src=etc/src=..\/etc/g' -i .bak etc/NAME.html
	@lua etc/markdown.lua --style etc/css/style.css --inline-style LICENSE
	@sed -E -e 's/url\(\.\.\/\.\.\/etc/url(etc/g' -i .bak LICENSE.html
