#!/bin/bash

if [ "$MODE" == "server" ]
then
    /app/build/server
elif [ "$MODE" == "client" ]
then
    echo client mode
else
    echo mode is not chosen properly
fi
