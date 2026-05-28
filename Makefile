CC      = C:\msys64\mingw64\bin\gcc.exe
CFLAGS  = -Iinclude -IC:\msys64\mingw64\include\SDL2
LDFLAGS = -LC:\msys64\mingw64\lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -mwindows

OBJ_DIR = obj
TARGET  = sokoban.exe

OBJS = $(OBJ_DIR)\main.o \
       $(OBJ_DIR)\s_sdl.o \
       $(OBJ_DIR)\draw.o \
       $(OBJ_DIR)\sprite.o \
       $(OBJ_DIR)\motion.o \
       $(OBJ_DIR)\movement.o \
       $(OBJ_DIR)\physics.o \
       $(OBJ_DIR)\input.o

all: $(OBJ_DIR) $(TARGET)

$(OBJ_DIR):
	if not exist $(OBJ_DIR) mkdir $(OBJ_DIR)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)\main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)\s_sdl.o: src\s_sdl.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)\draw.o: src\draw.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)\sprite.o: src\sprite.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)\motion.o: src\motion.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)\movement.o: src\movement.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)\physics.o: src\physics.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)\input.o: src\input.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	if exist $(OBJ_DIR) rmdir /s /q $(OBJ_DIR)
	if exist $(TARGET) del $(TARGET)

.PHONY: all clean