# Authors: Tomasz Jóżwik & Marcin Różański

FROM ubuntu:18.04

LABEL maintainer="tjozwik@protonmail.com"

# Copy code

RUN mkdir /app
COPY . /app

# Install required dependencies and build

RUN apt update \
    && apt install software-properties-common -y \
    && add-apt-repository ppa:costamagnagianfranco/telegram \
    && apt update \
    && apt install -y g++ make cmake git libspdlog-dev libboost-all-dev libpqxx-dev \
    && mkdir /app/build \
    && cd /app/build \
    && cmake -DCMAKE_CXX_FLAGS="-Werror" .. \
    && cmake --build . \
    && chmod +x ../service_manager.sh

# Run service manager

CMD /app/service_manager.sh
