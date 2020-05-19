export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/home/dell/RogoKok-distributed/pa6/lib64";
### пустая строка
LD_PRELOAD=/home/dell/RogoKok-distributed/pa6/lib64/libruntime.so 
rm pa6
rm events.log
rm pipes.log
make
./pa6 --mutexl -p 9
