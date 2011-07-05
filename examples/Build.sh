mkdir -p build/Jivai

if [ -z "$1" ]
then
	echo "No parameter given."
	exit 1
fi

echo "manifest=yes
main=$1.c
include=.
include=../src
map=.:build
map=../src:build/Jivai
optimlevel=0
dbgsym=yes
debug=yes
inclhdr=../config.h" | ../../Depend/Depend.exe build /dev/stdin
