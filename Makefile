EXE = experiment
TUNE_EXE = tuning
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include
LDFLAGS += -lm -lstdc++fs 

SRC:= $(shell find src/ ! -name "experiment.cc" ! -name "tuning.cc" -name "*.cc")
OBJ = $(SRC:$(SRC_DIR)/%.cc=$(OBJ_DIR)/%.o) $(OBJ_DIR)/coco.o 
INC = -I $(INC_DIR)

CC = g++
CC_COCO = gcc
CFLAGS  = -Wall -Wextra -std=c++11 -O2 -g
CFLAGS_COCO = -g -ggdb -std=c89 -pedantic -Wall -Wextra -Wstrict-prototypes -Wshadow -Wno-sign-compare -Wno-sign-conversion

all: $(OBJ_DIR) $(EXE)

tune: $(OBJ_DIR) $(TUNE_EXE)

clean:
	rm -rf $(OBJ_DIR)/*.o $(EXE) configurations

$(EXE): $(OBJ) $(OBJ_DIR)/experiment.o
	$(CC) $(CFLAGS) -o $(EXE) $^ $(LDFLAGS)

$(TUNE_EXE): $(OBJ) $(OBJ_DIR)/tuning.o
	$(CC) $(CFLAGS) -o $(TUNE_EXE) $^ $(LDFLAGS)

$(OBJ_DIR)/coco.o: $(SRC_DIR)/coco.c
	$(CC_COCO) -c $(CFLAGS_COCO) -o $@ $< -lm

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc $(INC_DIR)/*
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

.PHONY: all clean tune
