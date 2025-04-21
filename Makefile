# Noms des exécutables
NAME_SERVER = server
NAME_CLIENT = client

# Dossiers
PRINTF_DIR = printf_42

# Sources
SRCS_SERVER = serveur.c 
SRCS_CLIENT = client.c 

PRINTF_SRCS = \
	$(PRINTF_DIR)/ft_printf.c \
	$(PRINTF_DIR)/ft_utils.c \
	$(PRINTF_DIR)/ft_handle_conversion.c \
	$(PRINTF_DIR)/ft_handle_format.c \
	$(PRINTF_DIR)/ft_print_char.c \
	$(PRINTF_DIR)/ft_print_int.c \
	$(PRINTF_DIR)/ft_print_string.c \
	$(PRINTF_DIR)/ft_print_unsigned.c \
	$(PRINTF_DIR)/ft_print_pointer.c \
	$(PRINTF_DIR)/ft_print_hex.c

# Objets
OBJS_SERVER = $(SRCS_SERVER:.c=.o)
OBJS_CLIENT = $(SRCS_CLIENT:.c=.o)
OBJS_PRINTF = $(PRINTF_SRCS:.c=.o)

# Compilation
CC = gcc
CFLAGS = -Wall -Wextra -Werror
INCLUDES = -I$(PRINTF_DIR)

# Règles
all: $(NAME_SERVER) $(NAME_CLIENT)

$(NAME_SERVER): $(OBJS_SERVER) $(OBJS_PRINTF)
	$(CC) $(CFLAGS) -o $@ $^

$(NAME_CLIENT): $(OBJS_CLIENT) $(OBJS_PRINTF)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS_SERVER) $(OBJS_CLIENT) $(OBJS_PRINTF)

fclean: clean
	rm -f $(NAME_SERVER) $(NAME_CLIENT)

re: fclean all

.PHONY: all clean fclean re
