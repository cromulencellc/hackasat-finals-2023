FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt -y update && apt -y upgrade && apt -y install \
    tini python3 python3-pip

RUN pip3 install pwntools

WORKDIR /solver
COPY *.py /solver/

CMD python3 solver.py --hostname $CHAL_HOST --port $CHAL_PORT
