PROJECT_NAME = scp
SRC_DIR = src
BUILD_DIR = build
RM = rm -r -f
MKDIR = mkdir -p
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)%.cpp=$(BUILD_DIR)%.o)
BIN = $(BUILD_DIR)/$(PROJECT_NAME)
SHELL = /bin/bash

.PHONY: clean run

all: $(PROJECT_NAME)

$(PROJECT_NAME): mkdir-build $(OBJS)
	$(CXX) -std=c++11 -o $(BIN) $(OBJS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -std=c++11 -c $< -o $@

mkdir-build:
	@$(MKDIR) $(BUILD_DIR)

clean:
	@$(RM) $(BUILD_DIR)

run: $(BUILD_DIR)/$(PROJECT_NAME)
	$(BUILD_DIR)/$(PROJECT_NAME)