#!/bin/bash

gcc -fPIC -shared caculate.c -o libcaculate.so
gcc -rdynamic -o main main.c -ldl