REGISTRY_URL ?= finals/challenges
CHAL_NAME ?= finalpass
CHAL_PORT ?= 12345
CHAL_HOST ?= 172.17.0.1

.PHONY: static
static:
	sudo rm -rf data/*
	docker build static -t $(REGISTRY_URL)/$(CHAL_NAME):static
	docker run -v $(PWD)/data:/binaries $(REGISTRY_URL)/$(CHAL_NAME):static
	sudo rm -rf static/finalpass
	sudo cp data/finalpass static/
	sudo chown ${USER}:${USER} static/finalpass

.PHONY:build
build:

	docker build static -t $(REGISTRY_URL)/$(CHAL_NAME):static
	docker build challenge -t $(REGISTRY_URL)/$(CHAL_NAME):challenge
	docker build solver -t $(REGISTRY_URL)/$(CHAL_NAME):solver

.PHONY:challenge
challenge:
	socat -v tcp-listen:$(CHAL_PORT),reuseaddr exec:"docker run --rm -i -e CHAL_PORT=$(CHAL_PORT) $(REGISTRY_URL)/${CHAL_NAME}\:challenge" > log 2>&1 &
	echo "To connect: nc localhost $(CHAL_PORT)"

.PHONY:solver
solver:
	socat -v tcp-listen:$(CHAL_PORT),reuseaddr exec:"docker run --rm -i -p 1337\:1337 -e CHAL_PORT=$(CHAL_PORT) $(REGISTRY_URL)/${CHAL_NAME}\:challenge" > log 2>&1 &
	docker run --rm -i -e CHAL_HOST=${CHAL_HOST} -e CHAL_PORT=${CHAL_PORT} $(REGISTRY_URL)/${CHAL_NAME}:solver
	echo "To connect: nc localhost $(CHAL_PORT)"

# .PHONY:public
# public:
# 	./make_public.sh
