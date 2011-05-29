#!/bin/sh

mkdir -p build/Jivai

if [ -z "$1" ]
then
	echo "No parameter given."
	exit 1
fi

../../Depend/Depend.exe    \
	build                  \
	output=build/$1.exe    \
	manifest=Manifest.h    \
	main=$1.c              \
	include=.              \
	include=../src         \
	map=.:build            \
	map=../src:build/Jivai \
	optimlevel=0           \
	dbgsym=yes             \
	debug=yes              \
	inclhdr=../config.h
