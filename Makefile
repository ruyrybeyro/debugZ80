CC = clang
CFLAGS = -pipe -O2 -arch x86_64 -Wall -W -fPIC -I. -I../QtSpecem/h
LFLAGS = -lm

SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
TARGET = debugz80

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) $(LFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJECTS) $(TARGET) *~ core

