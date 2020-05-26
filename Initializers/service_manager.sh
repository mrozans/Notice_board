#!/bin/bash

# Autorzy: Tomasz Jóźwik, Marcin Różański

if [ "$MODE" == "server" ]
then
    /app/build/server
elif [ "$MODE" == "client" ]
then
    /app/build/client
else
    echo Mode is not chosen properly
fi
