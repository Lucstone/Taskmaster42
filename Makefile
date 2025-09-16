# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: lnaidu <lnaidu@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/09/16 09:52:08 by lnaidu            #+#    #+#              #
#    Updated: 2025/09/16 10:02:22 by lnaidu           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = taskmaster

CC = c++
CXXFLAGS = -Wall -Wextra -Werror
SCRS = 

#COLORS
RED = \033[1;31m
GREEN = \033[1;32m
YELLOW = \033[1;33m
DEFAULT = \033[0m

OBJS =${SCRS:.cpp=.o}

.cpp.o:
	@${CC} ${CXXFLAGS} -c $< -o $@

$(NAME): $(OBJS)
	@$(CC) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN) Computing all files !$(DEFAULT)" 

all : $(NAME)

clean :
	@rm -f $(OBJS)
	@echo "$(YELLOW)Deleting all files !$(DEFAULT)" 

fclean : clean
	@rm -f ./taskmaster
	@echo "$(RED) Cleeaning all files !$(DEFAULT)" 

re : fclean all

./PHONY : all clean fclean re 