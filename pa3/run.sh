export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/home/dell/RogoKok-distributed/pa3/lib64";
### пустая строка
LD_PRELOAD=/home/dell/RogoKok-distributed/pa3/lib64/libruntime.so 
rm pa3
rm events.log
rm pipes.log
make
./pa3 -p 5 10 10 20 20 30 

