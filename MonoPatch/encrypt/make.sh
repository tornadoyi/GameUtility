rm -rf build
mkdir build

# build encrypt tool
gcc -m32 -o build/encrypt tool/encrypt.c include/tea.c -I ./include

# build so
cd demo
gcc -m32 testso.c ../include/tea.c -I . -I ../include -fPIC -shared -o libtest.so -std=c99
#gcc -m32 testso.c ../include/tea.c -I . -I ../include -fPIC -shared -ffunction-sections -fdata-sections --gc-sections -o libtest.so 

# build demo
gcc -m32 demo.c -L./ -Wl,-rpath,./ -ltest -lpthread -o demo -std=c99

#copy
cd ..
mv demo/libtest.so build
mv demo/demo build