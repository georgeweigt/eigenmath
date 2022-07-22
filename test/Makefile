.PHONY: default clean

default:
	make -s selftest1
	make -s selftest2

selftest1: selftest1-source-files/*
	cat selftest1-source-files/* > selftest1
	echo \"ok\" >> selftest1

selftest2: selftest2-source-files/*
	cat selftest2-source-files/* > selftest2
	echo \"ok\" >> selftest2

clean:
	rm -f selftest1 selftest2
