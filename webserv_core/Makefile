NAME	= webserv
SRC		= $(wildcard *.cpp)
SERV 	= $(wildcard servers/*.cpp)
SOCK 	= $(wildcard sockets/*.cpp)
UTIL	= $(wildcard utils/*.cpp)
CONF	= $(wildcard config_parser/*.cpp)

HDR		= $(wildcard *.hpp)

OBJ		= $(patsubst %.cpp, obj/%.o,$(SRC)) \
		  $(patsubst %.cpp, obj/%.o,$(SERV)) \
		  $(patsubst %.cpp, obj/%.o,$(SOCK)) \
		  $(patsubst %.cpp, obj/%.o,$(UTIL)) \
		  $(patsubst %.cpp, obj/%.o,$(CONF))

DEP		= $(patsubst %.o, %.d,$(OBJ))
# CFLAGS	= -g -fstandalone-debug

#CFLAGS	= -g -std=c++98
CFLAGS	= -Wall -Werror -Wextra -Wshadow -Wno-shadow -std=c++98
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

obj/config_parser/%.o: config_parser/%.cpp $(HDR) | obj
	$(CC) $(CFLAGS) -MMD -c $< -o $@

obj:
	mkdir -p obj
	mkdir -p obj/servers
	mkdir -p obj/sockets
	mkdir -p obj/utils
	mkdir -p obj/config_parser

clean:
	$(RM) $(DEP)
	$(RM) $(OBJ)
	rmdir obj/servers
	rmdir obj/sockets
	rmdir obj/utils
	rmdir obj/config_parser
	rmdir obj

fclean: clean
	$(RM) $(NAME)

re: fclean all


-include $(OBJ:.o=.d)

.PHONY: all clean fclean re