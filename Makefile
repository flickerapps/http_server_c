CC = gcc
OBJ = \
	src/main.o \
	src/server.o
CFLAGS = -lpthread
OUT = bin
EXEC = $(OUT)/http_server
INCLUDES = -I include -I modules
$(EXEC): $(OBJ)
	mkdir -p $(OUT)
	$(CC) $(CFLAGS) $(OBJ) $(INCLUDES) -o  $@
all: $(EXEC)
run: $(EXEC)
	./$(EXEC) 80 200
restart:
	make clean
	make run
src/%.o: src/%.c
	$(CC) $(INCLUDES) $(CFLAGS) -o $@ -c $<
.PHONY: clean
clean:
	rm -r bin
	rm src/*.o

