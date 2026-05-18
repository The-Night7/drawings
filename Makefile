# ─── Compilateur & flags ──────────────────────────────────────
CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -std=c11 \
          $(shell pkg-config --cflags cairo) \
          -Wno-unused-parameter
LDFLAGS = $(shell pkg-config --libs cairo) -lm

# ─── Sources ──────────────────────────────────────────────────
TARGET  = cel_shading
SRCS    = main.c canvas.c palette.c brush.c shapes.c generator.c export.c
OBJS    = $(SRCS:.c=.o)

# ─── Règles ───────────────────────────────────────────────────
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "✅ Compilation OK → ./$(TARGET)"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ─── Nettoyage ────────────────────────────────────────────────
clean:
	rm -f $(OBJS) $(TARGET)

# ─── Raccourcis de test ───────────────────────────────────────
run:
	./$(TARGET) -s 0 -f 0 -p 4 -r 42 -o output.png
	@echo "📁 output.png généré"

gallery:
	./$(TARGET) --all -o ./gallery
	@echo "📁 Galerie dans ./gallery/"

batch:
	./$(TARGET) -b 5 -s 1 -f 0 -p 0 -o ./batch_output
	@echo "📁 Batch dans ./batch_output/"

debug:
	./$(TARGET) -s 2 -f 2 -p 1 -r 999 -o debug.png --layers
	@echo "📁 Calques exportés"

.PHONY: all clean run gallery batch debug