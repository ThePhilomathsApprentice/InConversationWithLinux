gcc shmInit.c -o shmInit
gcc semaSHM.c -o semaSHM
gcc shmDInit.c -o shmDInit

taskset 0x00000001 ./shmInit

##Test_1: Uni Processor Scenario
#taskset 0x00000001 ./semaSHM

##Test_2a: Multi-Processor Scenario
taskset 0x00000003 ./semaSHM

##Test_2b: Multi-Processor Scenario
#taskset 0x00000007 ./semaSHM



#
taskset 0x00000001 ./shmDInit
