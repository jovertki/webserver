NAME	= webserv
SRC		= $(wildcard *.cpp)
SERV 	= $(wildcard servers/*.cpp)
SOCK 	= $(wildcard sockets/*.cpp)
UTIL	= $(wildcard utils/*.cpp)

HDR		= $(wildcard *.hpp)

OBJ		= $(patsubst %.cpp, obj/%.o,$(SRC)) \
		  $(patsubst %.cpp, obj/%.o,$(SERV)) \
		  $(patsubst %.cpp, obj/%.o,$(SOCK)) \
		  $(patsubst %.cpp, obj/%.o,$(UTIL))
DEP		= $(patsubst %.o, %.d,$(OBJ))
CFLAGS	= -g
#CFLAGS	= -Wall -Werror -Wextra -Wshadow -Wno-shadow -std=c++98
CC		= clang++

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

obj/%.o: %.cpp $(HDR) | obj
	$(CC) $(CFLAGS) -MMD -c $< -o $@

obj/servers/%.o: server/%.cpp $(HDR) | obj
	$(CC) $(CFLAGS) -MMD -c $< -o $@

obj/sockets/%.o: sockets/%.cpp $(HDR) | obj
	$(CC) $(CFLAGS) -MMD -c $< -o $@

obj/utils/%.o: utils/%.cpp $(HDR) | obj
	$(CC) $(CFLAGS) -MMD -c $< -o $@

obj:
	mkdir -p obj
	mkdir -p obj/servers
	mkdir -p obj/sockets
	mkdir -p obj/utils

clean:
	$(RM) $(DEP)
	$(RM) $(OBJ)
	rmdir obj/servers
	rmdir obj/sockets
	rmdir obj/utils
	rmdir obj

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re