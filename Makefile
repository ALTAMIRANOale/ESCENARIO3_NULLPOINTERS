# =====================================================
# PROYECTO TLI - SIMULACION TIERRA-LUNA
# Compilacion con MinGW-w64 (w64devkit)
# Uso:  mingw32-make         -> compila tli.exe
#       mingw32-make run     -> compila y ejecuta
#       mingw32-make clean   -> borra objetos y ejecutable
# =====================================================

CC       = gcc
CFLAGS   = -std=c11 -Wall -Wextra
LDFLAGS  =
TARGET   = tli
SRCS     = main.c punto2.c trabajo.c arbol.c validaciones.c tli_calculos.c tli_explicaciones.c
OBJS     = $(SRCS:.c=.o)
HEADERS  = tli.h trabajo.h arbol.h validaciones.h

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET) $(TARGET).exe

.PHONY: all run clean
