export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/mnt/d/Projects/github/RogoKok-distributed/pa4/lib64";
### пустая строка
LD_PRELOAD=/mnt/d/Projects/github/RogoKok-distributed/pa4/libruntime.so 
rm pa4
rm events.log
rm pipes.log
make
./pa4 --mutexl -p 5
