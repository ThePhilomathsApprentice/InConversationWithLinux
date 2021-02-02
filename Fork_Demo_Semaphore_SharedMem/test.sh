rm ./bin/semaSHM


gcc semaSHM.c shmInit.c shmDInit.c -Werror -o ./bin/semaSHM

##Test_1: Uni Processor Scenario
taskset 0x00000001 ./bin/semaSHM

##Test_2a: Multi-Processor Scenario
#taskset 0x00000003 ./bin/semaSHM

##Test_2b: Multi-Processor Scenario
#taskset 0x00000007 ./bin/semaSHM

