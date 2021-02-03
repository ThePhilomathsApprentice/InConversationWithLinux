#!/bin/bash 

rm ./bin/echoServer

gcc echoServer.c socket.c -Werror -lpthread -o ./bin/echoServer
./bin/echoServer