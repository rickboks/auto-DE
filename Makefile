EXE = experiment
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include
LDFLAGS += -lm -lstdc++fs 

SRC:= $(shell find src/ ! -name "experiment.cc" -name "*.cc")
OBJ = $(SRC:$(SRC_DIR)/%.cc=$(OBJ_DIR)/%.o)
INC = -I $(INC_DIR)

CC = g++
CC_COCO = gcc
CFLAGS  = -Wall -Wextra -std=c++17 -O2 -g
CFLAGS_COCO = -g -ggdb -std=c89 -pedantic -Wall -Wextra -Wstrict-prototypes -Wshadow -Wno-sign-compare -Wno-sign-conversion

all: $(OBJ_DIR) $(EXE)

clean:
	rm -rf $(OBJ_DIR)/*.o $(EXE) $(MPI_EXE) configurations

$(EXE): $(OBJ) $(OBJ_DIR)/coco.o $(OBJ_DIR)/experiment.o
	${CC} ${CFLAGS} -o $(EXE) $(OBJ) $(OBJ_DIR)/experiment.o $(OBJ_DIR)/coco.o ${LDFLAGS}

$(OBJ_DIR)/coco.o: $(SRC_DIR)/coco.c
	$(CC_COCO) -c $(CFLAGS_COCO) -o $(OBJ_DIR)/coco.o $(SRC_DIR)/coco.c -lm

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc $(INC_DIR)/*
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

.PHONY: all
.PHONY: clean
