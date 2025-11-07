# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/09/16 09:52:08 by lnaidu            #+#    #+#              #
#    Updated: 2025/11/07 03:02:34 by lnaidu           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# ===================== TASKMASTER ======================
NAME        = taskmaster
BONUS_NAME  = taskmaster_bonus

CXX         = c++
CXXFLAGS    = -Wall -Wextra -Werror -std=c++17 -Iinclude


COMMON_SRCS = $(filter-out ./src/main.cpp, $(wildcard ./src/*.cpp))

MAND_SRCS   = $(COMMON_SRCS) ./src/main.cpp
MAND_OBJS   = $(MAND_SRCS:.cpp=.o)


BONUS_SRCS  = $(COMMON_SRCS) ./bonus/main_bonus.cpp ./bonus/IPCServer.cpp
BONUS_OBJS  = $(BONUS_SRCS:.cpp=.o)


LIBS        = -lyaml-cpp -lpthread -lreadline

RED     = \033[1;31m
GREEN   = \033[1;32m
YELLOW  = \033[1;33m
DEFAULT = \033[0m


all: $(NAME)

bonus: $(BONUS_NAME)

$(NAME): $(MAND_OBJS)
	@$(CXX) $(CXXFLAGS) $(MAND_OBJS) -o $@ $(LIBS)
	@echo "$(GREEN)Built $(NAME) (mandatory)$(DEFAULT)"

$(BONUS_NAME): $(BONUS_OBJS)
	@$(CXX) $(CXXFLAGS) $(BONUS_OBJS) -o $@ $(LIBS)
	@echo "$(GREEN)Built Bonus$(BONUS_NAME) (bonus)$(DEFAULT)"


.cpp.o:
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@rm -f $(MAND_OBJS) $(BONUS_OBJS)
	@echo "$(YELLOW)Object files deleted$(DEFAULT)"

fclean: clean
	@rm -f $(NAME) $(BONUS_NAME)
	@echo "$(RED)Executables deleted$(DEFAULT)"

re: fclean all

.PHONY: all bonus clean fclean re
