SRC_DIR := src
INC_DIR := include
ELF_DIR := bin
SRC_OBJS := $(wildcard $(SRC_DIR)/*.c)
INC_OBJS := $(wildcard $(INC_DIR)/*.h)
ELF := $(ELF_DIR)/myshell
CXXFLAGS := -g -I $(INC_DIR)

$(ELF) : $(SRC_OBJS) $(INC_OBJS)
	gcc -o $@ $(SRC_OBJS) $(INC_OBJS) $(CXXFLAGS)


clear:
	rm -rf $(ELF)
run:
	./bin/myshell
