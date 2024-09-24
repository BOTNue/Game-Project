#!/bin/sh
cc "${1:-main.c}" $(pkg-config --libs --cflags raylib) -o "${2:-YourGame}"