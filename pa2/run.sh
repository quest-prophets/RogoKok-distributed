export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:/home/dell/RogoKok-distributed/pa2/lib64";
### пустая строка
LD_PRELOAD=/home/dell/RogoKok-distributed/pa2/lib64/libruntime.so 
rm pa2
rm events.log
rm pipes.log
make
./pa2 -p 3 10 20 30

