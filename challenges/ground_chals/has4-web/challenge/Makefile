
TAG_NAME ?= has4-web:latest
REGISTRY ?= finals/challenges/has4-web
NETWORK ?= groundstation_default

BUILD_DIR ?= ./build
SRC_DIR ?= ./src
PRIV_DIR ?= ./priv

TARGET ?= has4-web

CC := gcc

CPPFLAGS ?= -MD
CFLAGS ?=  -g -Wall -fno-stack-protector  -no-pie -fno-pie
LDFLAGS ?= -g -lm -lcrypt -static -no-pie -fno-pie 

HARDCODED_SRCS = $(wildcard $(SRC_DIR)/*.c)
#PARSER_GENERATED_SRCS = $(SRC_DIR)/parse.tab.c $(SRC_DIR)/parse.yy.c

SRCS := $(HARDCODED_SRCS) $(PARSER_GENERATED_SRCS)
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	strip $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(SRC_DIR)/%.yy.c: $(PRIV_DIR)/%.lex $(SRC_DIR)/%.tab.c
	flex -o $@ $<

$(SRC_DIR)/%.tab.c: $(PRIV_DIR)/%.y
	bison -d -o $@ $<

.PHONY: clean docker

run: $(BUILD_DIR)/$(TARGET)
	$<

clean:
	rm -rf $(BUILD_DIR) $(PARSER_GENERATED_SRCS)

.PHONY: build
build:
	docker build -t ${REGISTRY}/$(TAG_NAME) .

docker_run:
	docker run -it -v /htmlvol:/htmlvol --network host $(REGISTRY)/$(TAG_NAME)
	# docker build --network host -t has4-web .
	# docker run --network host --rm -it -v `pwd`:/mnt has4-web

-include $(DEPS)
