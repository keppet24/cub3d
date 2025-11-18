/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cube3d.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: othmaneettaqi <othmaneettaqi@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 11:40:59 by othmaneetta       #+#    #+#             */
/*   Updated: 2025/06/17 16:26:26 by othmaneetta      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUBE3D_H
# define CUBE3D_H

# include <stdlib.h>
# include <stdbool.h>
# include <unistd.h>
# include <fcntl.h>
# include "src/utils/ft_split.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1
#endif

typedef enum e_texture
{
	TEX_NO = 0,
	TEX_SO,
	TEX_WE,
	TEX_EA,
	TEX_TOTAL
}	t_texture;

typedef struct s_color
{
	int		r;
	int		g;
	int		b;
	bool	set;
}	t_color;

typedef struct s_parser_state
{
	char	*tex_path[TEX_TOTAL];
	t_color	floor;
	t_color	ceiling;
	char	**map_lines;
	size_t	map_width;
	size_t	map_height;
	int		player_count;
	int		player_x;
	int		player_y;
	char	player_dir;
}	t_parser_state;

char    *get_next_line(int fd);
int		parse(const char *path);
bool	parse_header(int fd, t_parser_state *state, char **first_map_line);
bool	parse_map(int fd, t_parser_state *state, char *first_map_line);
size_t	ft_strlen(const char *s);
bool	ft_isdigit(char c);
bool	ft_isspace(char c);
char	*ft_strdup(const char *s);
void	ft_free_split(char **tab);
void	print_error(const char *msg);

#endif
