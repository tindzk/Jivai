#!/bin/sh

mkdir -p build/Jivai

../../Depend/Depend.exe    \
	build                  \
	main=TestSuite.c       \
	output=TestSuite.exe   \
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
