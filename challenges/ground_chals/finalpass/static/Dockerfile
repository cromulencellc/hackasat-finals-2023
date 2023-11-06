FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
ENV TZ=Etc/UTC

RUN apt-get update \ 
    && apt-get upgrade -y \
    && apt-get install -y \
    build-essential \
    wget \
    ca-certificates \ 
    cmake \
    libssl-dev \
    python3

RUN apt-get clean autoclean \ 
    && rm -rf /var/lib/apt/lists/*

WORKDIR /home/builder

RUN mkdir finalpass
COPY manager ./manager/
COPY main.cpp ./
COPY CMakeLists.txt ./
COPY build.sh ./

VOLUME /binaries

RUN mkdir /finalpass/
RUN ./build.sh /finalpass/

# Scripts needed to create the admin DB
COPY DB.py ./
COPY admin.json ./
COPY create_admin.py ./
RUN python3 create_admin.py
RUN cp admin.db /finalpass/

ENTRYPOINT [ "./build.sh", "/binaries/" ]

