#!/bin/bash

cd /challenge
socat -v tcp-listen:${CHAL_PORT},reuseaddr exec:"./finalpass ${CHAL_PORT}"
