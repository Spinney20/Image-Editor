# Dobre Andrei-Teodor 314CA

# compiler setup
CC = gcc
CFLAGS = -Wall -Wextra -std=c99

# define targets
TARGETS = image_editor

build: $(TARGETS)

image_editor: image_editor.c
	$(CC) $(CFLAGS) image_editor.c memory.c commands.c -lm -o image_editor

clean:
	rm -f $(TARGETS)
