# Autorzy: Tomasz Jóżwik, Marcin Różański
# Data: 25.05.2020
# Tytuł projektu: Tablica ogłoszeń

FROM ubuntu:18.04
ARG DEBIAN_FRONTEND=noninteractive

LABEL maintainer="tjozwik@protonmail.com"

# Copy code

RUN mkdir /app
COPY . /app

# Install required dependencies

RUN apt update \
    && apt install software-properties-common -y \
    && add-apt-repository ppa:costamagnagianfranco/telegram \
    && apt update \
    && apt install -y g++ make cmake git libspdlog-dev libboost-all-dev libpqxx-dev

# Build app

RUN mkdir /app/build \
    && cd /app/build \
    && cmake -DCMAKE_CXX_FLAGS="-Werror" .. \
    && cmake --build .

# Configure service manager

RUN chmod +x /app/Initializers/service_manager.sh

# Run service manager

CMD /app/Initializers/service_manager.sh
