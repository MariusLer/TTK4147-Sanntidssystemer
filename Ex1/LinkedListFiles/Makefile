CCFLAGS = -Wall -std=c99
LDFLAGS=
SOURCES=$(wildcard *c)
OBJECTS=$(SOURCES:.c=.o)
TARGET=taskB
all: $(TARGET)

$(TARGET): $(OBJECTS)
	gcc -o $@ $^ $(LDFLAGS)
	
%.o: %.c %.h
	gcc $(CCFLAGS) -c $<

%.o: %.c
	gcc $(CCFLAGS) -c $<

clean:
	rm -f *.o $(TARGET)
