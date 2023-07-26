NAME = webshit
CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98

HEADER_DIR = ./header
SRC_DIR = ./src/
OBJ_DIR = ./.build/
LIBS = -lm

SRC = main.cpp tcpServer.cpp utils.cpp

OBJ = $(SRC:.cpp=.o)
OBJ_PREF := $(addprefix $(OBJ_DIR), $(OBJ))

$(NAME): $(OBJ_PREF) $(OBJ_DIR)
	@$(CXX) $(CXXFLAGS) $(OBJ_PREF) -I$(HEADER_DIR) $(LIBS) -o $@
	@echo "Success, ./$(NAME) created"

$(OBJ_PREF): .build/%.o: src/%.cpp
	@mkdir -p $(OBJ_DIR)
	@$(CXX) $(CXXFLAGS) -c
