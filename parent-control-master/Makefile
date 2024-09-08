CC = gcc
CFLAGS = -Wall -Werror -Wextra
CUSTOMFLAGS = -lcurl -lpthread

BUILD = parental_control
BUILD_DIR = build
INSTALL = install
INSTALL_DIR = /usr/local/bin
HEADS_DIR = include
OBJ_DIR = obj
SRC_DIR = src

SRC = $(wildcard $(SRC_DIR)/*.c)
HEADS = $(wildcard $(HEADS_DIR)/*.h)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: clean

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) -c $(CFLAGS) -I$(HEADS_DIR) $< -o $@

$(BUILD): $(OBJ_DIR) $(BUILD_DIR) $(OBJ)
	$(CC) -I$(HEADS_DIR) $(OBJ) -o $(BUILD_DIR)/$@ $(CUSTOMFLAGS)

$(INSTALL): $(BUILD)
	sudo cp $(BUILD_DIR)/$(BUILD) $(INSTALL_DIR)

clean:
	rm -rf $(OBJ_DIR)
	rm -rf $(BUILD_DIR)
	rm -rf err_log log pidfile runtime

clean_install:
	sudo rm -rf $(INSTALL_DIR)/$(BUILD)

$(OBJ_DIR):
	mkdir $(OBJ_DIR)

$(BUILD_DIR):
	mkdir $(BUILD_DIR)