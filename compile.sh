if [ ! -d ./build ]; then 
 mkdir build 
fi


cmake -G Ninja -S. -Bbuild
