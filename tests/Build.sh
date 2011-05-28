#!/bin/sh

mkdir -p build/Jivai

../../Depend/Depend.bin    \
	build                  \
	main=TestSuite.c       \
	output=TestSuite.bin   \
	manifest=Manifest.h    \
	include=.              \
	include=../src         \
	map=.:build            \
	map=../src:build/Jivai \
	optimlevel=0           \
	dbgsym=yes             \
	add=*.c                \
	inclhdr=../config.h    \
	link=@m
