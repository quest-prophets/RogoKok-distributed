export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/home/dell/RogoKok-distributed/pa4/lib64";
### пустая строка
LD_PRELOAD=/home/dell/RogoKok-distributed/pa4/lib64/libruntime.so 
rm pa4
rm events.log
rm pipes.log
make
./pa4 -p 5 
