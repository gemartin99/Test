S_NAME = server
C_NAME = client

DEL_LINE =		\033[2K
ITALIC =		\033[3m
BOLD =			\033[1m
DEF_COLOR =		\033[0;39m
GRAY =			\033[0;90m
RED =			\033[0;91m
GREEN =			\033[0;92m
YELLOW =		\033[0;93m
BLUE =			\033[0;94m
MAGENTA =		\033[0;95m
CYAN =			\033[0;96m
WHITE =			\033[0;97m
BLACK =			\033[0;99m
ORANGE =		\033[38;5;209m
BROWN =			\033[38;2;184;143;29m
DARK_GRAY =		\033[38;5;234m
MID_GRAY =		\033[38;5;245m
DARK_GREEN =	\033[38;2;75;179;82m
DARK_YELLOW =	\033[38;5;143m

S_SRCS = server.cpp 
C_SRCS = 

INCLUDE = 
CC = c++
RM = rm -f
CFLAGS = -Wall -Wextra -Werror -std=c++98 -MMD 
SOBJS = ${S_SRCS:.cpp=.o}
COBJS = ${C_SRCS:.cpp=.o}
DEPS = ${S_SRCS:.cpp=.d}

%.o: %.cpp
	@echo "${BLUE} ◎ $(BROWN)Compiling   ${MAGENTA}→   $(CYAN)$< $(DEF_COLOR)"
	@$(CC) $(CFLAGS) -c $< -o $@

all:	${S_NAME} #${C_NAME}

-include ${DEPS}
${NAME}: ${SOBJS} ${COBJS}
		@${CC} ${CFLAGS} ${SOBJS} -o ${S_NAME}
		@echo "\n$(GREEN) Created $(S_NAME) ✓ $(DEF_COLOR)\n"
		@${CC} ${CFLAGS} ${COBJS} -o ${C_NAME}
		@echo "\n$(GREEN) Created $(C_NAME) ✓ $(DEF_COLOR)\n"



clean:
		@${RM} ${SOBJS} ${COBJS} ${DEPS}
		@echo "\n${BLUE} ◎ $(RED)All objects cleaned successfully ${BLUE}◎$(DEF_COLOR)\n"
fclean:
		@${RM} ${SOBJS} ${COBJS} ${DEPS} ${S_NAME} ${C_NAME}
		@echo "\n${BLUE} ◎ $(RED)All objects and executable cleaned successfully${BLUE} ◎$(DEF_COLOR)\n"

re: fclean all

.PHONY: all clean fclean re