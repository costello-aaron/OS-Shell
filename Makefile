# Makefile for Project 1: Shell

CC = gcc
CFLAGS = -Wall -g

# The main target
shell: shell.c
	$(CC) $(CFLAGS) shell.c -o shell

# Clean up build files
clean:
	rm -f shell *.o
