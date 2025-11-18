NAME		:= cub3d
CC			:= cc
CFLAGS		:= -Wall -Wextra -Werror

OBJDIR		:= obj
SRCS		:= src/main.c \
			   src/error.c \
			   src/utils/string_utils.c \
			   src/utils/ft_split.c \
			   src/parser/parser.c \
			   src/parser/parse_header/parse_header_utils.c \
			   src/parser/parse_header/parse_header_checks.c \
			   src/parser/parse_header/parse_header.c \
			   src/parser/parse_map/parse_map.c \
			   src/parser/parse_map/validate_map.c \
			   get_next_line.c
OBJS		:= $(addprefix $(OBJDIR)/,$(SRCS:.c=.o))

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -I. -o $(NAME)

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I. -c $< -o $@

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
