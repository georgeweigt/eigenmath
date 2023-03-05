.PHONY: default clean

default:
	make selftest1
	make selftest2

selftest1: src1/* fin
	cat src1/* fin > selftest1

selftest2: src2/* fin
	cat src2/* fin > selftest2

clean:
	rm -f selftest1 selftest2
