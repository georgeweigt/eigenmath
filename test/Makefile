.PHONY: default

default:
	make selftest1
	make selftest2

selftest1: src1/*
	cat src1/* > selftest1
	echo \"ok\" >> selftest1

selftest2: src2/*
	cat src2/* > selftest2
	echo \"ok\" >> selftest2
