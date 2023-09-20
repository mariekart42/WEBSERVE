NAME = webshit
CXX = c++
CXXFLAGS = -g -std=c++98 -Wall -Werror -Wextra
HEADER_DIR = ./header
SRC_DIR = ./src/
OBJ_DIR = ./.build/
LIBS = -lm
RM = rm -rf

SRC = main.cpp utils.cpp setServer.cpp connectClients.cpp Request.cpp Response.cpp Error.cpp urlDecoded.cpp configParser.cpp logging.cpp

OBJ = $(SRC:.cpp=.o)
OBJ_PREF := $(addprefix $(OBJ_DIR), $(OBJ))

$(NAME): $(OBJ_PREF) $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(OBJ_PREF) -I $(HEADER_DIR) $(LIBS) -o $@

$(OBJ_PREF): .build/%.o: src/%.cpp
	@mkdir -p $(OBJ_DIR)
	@$(CXX) $(CXXFLAGS) -c $< -o $@ -I $(HEADER_DIR)

all: $(NAME)

clean:
	@$(RM) $(OBJ_DIR)

fclean: clean
	@$(RM) $(NAME)
	@echo "Success, directory cleaned"

re: fclean all

.PHONY: all clean fclean re