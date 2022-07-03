.PHONY: default clean

default:
	make selftest1
	make selftest2

selftest1: selftest1-source-files/*
	cat selftest1-source-files/* ok >selftest1

selftest2: selftest2-source-files/*
	cat selftest2-source-files/* ok >selftest2

clean:
	rm -f selftest1 selftest2
