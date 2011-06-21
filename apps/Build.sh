mkdir -p build/Jivai
echo "output=build/$1.exe
manifest=yes
main=$1.c
include=.
include=../src
map=.:build
map=../src:build/Jivai
optimlevel=0
dbgsym=yes
debug=yes
inclhdr=../config.h" | ../../Depend/Depend.exe build /dev/stdin
