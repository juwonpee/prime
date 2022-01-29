CC = g++
CC_INCLUDE = -I
CCFLAGS = -O0 -ggdb -Wall
LDFLAGS = 

SRC_DIR = src

C_SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
C_OBJ = $(C_SOURCES:.cpp=.o)
OUTPUT = build/output


all: 
	$(CC) $(C_SOURCES) -o $(OUTPUT)

a:
	$(C_OBJ)

clear:
	find . -name "*.o" | xargs -r rm 