cd src/
rm MurmurHash3.{c,.orig.c,h} &> /dev/null
wget http://smhasher.googlecode.com/svn/trunk/MurmurHash3.cpp -O MurmurHash3.c
wget http://smhasher.googlecode.com/svn/trunk/MurmurHash3.h
cp MurmurHash3{,.orig}.c
patch MurmurHash3.c MurmurHash3.patch
