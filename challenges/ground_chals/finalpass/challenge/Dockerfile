FROM finals/challenges/finalpass/static:static as binary

FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt -y update && apt -y upgrade && apt -y install \
    tini \
    socat

WORKDIR /challenge/
COPY run.sh /challenge/
COPY --from=binary /finalpass/finalpass /challenge/
RUN mkdir -p /challenge/db/
COPY --from=binary /finalpass/admin.db /challenge/db

RUN chmod +x run.sh

# Add less privledged user and change to them
RUN groupadd -r --gid=1000 challenge && \
    useradd -m -r -s /bin/bash -g challenge --uid=1000 challenge && \
    chown -R 1000:1000 /challenge/

USER challenge

WORKDIR /challenge

ENTRYPOINT [ "tini", "--", "/challenge/run.sh" ]
