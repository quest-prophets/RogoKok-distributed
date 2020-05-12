export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/home/dell/RogoKok-distributed/pa5/lib64";
### пустая строка
LD_PRELOAD=/home/dell/RogoKok-distributed/pa5/lib64/libruntime.so 
rm pa5
rm events.log
rm pipes.log
make
./pa5 --mutexl -p 9
