mangle.tab.c mangle.tab.h: mangle.y
	bison -d mangle.y

lex.yy.c: mangle.l mangle.tab.h
	flex mangle.l

mangle: lex.yy.c mangle.tab.c mangle.tab.h
	g++-6 mangle.tab.c lex.yy.c -lfl -o mangle
