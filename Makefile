all: minic

SRC := src
OBJ := obj

CFLAGS = -g3

SOURCES := $(wildcard $(SRC)/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

minic: $(OBJECTS)
	$(CC) $^ -o $@ $(LIBS) $(LDFLAGS)

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) -I$(SRC) -c $< -o $@

clean:
	rm -f $(OBJECTS)
