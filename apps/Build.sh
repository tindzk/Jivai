mkdir -p build/Jivai
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
