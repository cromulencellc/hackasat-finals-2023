FROM ubuntu:22.04 as builder
RUN DEBIAN_FRONTEND=noninteractive \
  apt -y update && apt -y upgrade && \
  apt -y install build-essential

RUN mkdir -p /src
RUN mkdir -p /build
COPY src/* /src/
COPY Makefile .
RUN make

FROM ubuntu:22.04

VOLUME /htmlvol/

WORKDIR /server
COPY --from=builder /build/has4-web .
COPY server.ini .
RUN mkdir -p www
COPY mime.types /etc/mime.types

# service will drop privs to uid/gid 1000 after binding to port 80
RUN useradd -u 1000  -ms /bin/bash has4
CMD ["./has4-web"]
