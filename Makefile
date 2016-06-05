CC = gcc
OBJ = \
	src/main.o \
	src/server.o
CFLAGS = -lpthread
OUT = bin
EXEC = $(OUT)/http_server
$(EXEC): $(OBJ)
	mkdir -p $(OUT)
	$(CC) $(CFLAGS) $(OBJ) -I include -o  $@
all: $(EXEC)
run: $(EXEC)
	./$(EXEC) 80 200
restart:
	make clean
	make run
src/%.o: src/%.c
	$(CC) -I include $(CFLAGS) -o $@ -c $<
.PHONY: clean
clean:
	rm -r bin
	rm src/*.o

