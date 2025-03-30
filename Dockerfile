FROM ubuntu:latest

# Установка необходимых пакетов
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    mingw-w64 \
    nsis \
    qt6-base-dev \
    git \
    && rm -rf /var/lib/apt/lists/*

# Копирование исходного кода
COPY . /app
WORKDIR /app

# Сборка для Windows
RUN mkdir build-win && cd build-win && \
    cmake -DCMAKE_TOOLCHAIN_FILE=../windows-toolchain.cmake .. && \
    cmake --build . --config Release && \
    cpack -G NSIS 