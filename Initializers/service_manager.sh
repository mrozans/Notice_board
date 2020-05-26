#!/bin/bash

# Autorzy: Tomasz Jóźwik, Marcin Różański
# Data: 20.05.2020
# Tytuł projektu: Tablica ogłoszeń

if [ "$MODE" == "server" ]
then
    /app/build/server
elif [ "$MODE" == "client" ]
then
    /app/build/client
else
    echo Mode is not chosen properly
fi
