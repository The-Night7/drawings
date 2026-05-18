# ─── Compilateur & flags ──────────────────────────────────────
CC      = gcc
CFLAGS  = -Wall -Wextra -O2 $(shell pkg-config --cflags gtk+-3.0 cairo)
LIBS    = $(shell pkg-config --libs gtk+-3.0 cairo) -lm

# ─── Fichiers ─────────────────────────────────────────────────
TARGET  = dessin
SRC     = dessin.c
OBJ     = $(SRC:.c=.o)

# ─── Règles ───────────────────────────────────────────────────
.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: all
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET) dessin.png