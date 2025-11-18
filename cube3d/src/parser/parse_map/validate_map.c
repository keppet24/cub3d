#include "parse_map_internal.h"

static bool	check_border_line(const char *line)
{
	size_t	i;

	i = 0;
	while (line[i])
	{
		if (line[i] != '1' && line[i] != ' ')
			return (print_error("Carte ouverte sur le bord"), false);
		i++;
	}
	return (true);
}

static bool	get_content_start(const char *line, size_t *start)
{
	size_t	end;

	*start = 0;
	while (line[*start] == ' ')
		(*start)++;
	end = ft_strlen(line);
	while (end > *start && line[end - 1] == ' ')
		end--;
	if (*start == end || line[*start] != '1' || line[end - 1] != '1')
		return (print_error("Carte non fermee sur les bords"), false);
	return (true);
}

static char	safe_char(t_parser_state *st, size_t y, size_t x)
{
	size_t	len;
	char	*line;

	line = st->map_lines[y];
	len = ft_strlen(line);
	if (x >= len)
		return (' ');
	return (line[x]);
}

static bool	space_neighbors_ok(t_parser_state *st, size_t y, size_t x)
{
	char	up;
	char	down;
	char	left;
	char	right;

	up = safe_char(st, y - 1, x);
	down = safe_char(st, y + 1, x);
	if (x == 0)
		left = ' ';
	else
		left = safe_char(st, y, x - 1);
	right = safe_char(st, y, x + 1);
	if ((up != '1' && up != ' ' && up != '0')
		|| (down != '1' && down != ' ' && down != '0')
		|| (left != '1' && left != ' ' && left != '0')
		|| (right != '1' && right != ' ' && right != '0'))
		return (false);
	return (true);
}

static bool	zero_neighbors_closed(t_parser_state *st, size_t y, size_t x)
{
	size_t	len_up;
	size_t	len_down;
	size_t	len_curr;

	len_curr = ft_strlen(st->map_lines[y]);
	len_up = ft_strlen(st->map_lines[y - 1]);
	len_down = ft_strlen(st->map_lines[y + 1]);
	if (x >= len_up || x >= len_down || x + 1 >= len_curr || x == 0)
		return (false);
	return (true);
}

static bool	scan_row(t_parser_state *st, size_t y)
{
	size_t	len;
	size_t	start;
	size_t	x;

	len = ft_strlen(st->map_lines[y]);
	if (!get_content_start(st->map_lines[y], &start))
		return (false);
	x = start;
	while (x < len)
	{
		if (st->map_lines[y][x] == ' ' && !space_neighbors_ok(st, y, x))
			return (print_error("Espace adjacent a vide"), false);
		if (st->map_lines[y][x] == '0' && !zero_neighbors_closed(st, y, x))
			return (print_error("Carte non fermee verticalement"), false);
		x++;
	}
	return (true);
}

bool	validate_map(t_parser_state *st)
{
	size_t	y;

	if (!st->map_lines || st->map_height == 0)
		return (print_error("Carte absente"), false);
	if (st->player_count != 1)
		return (print_error("Joueur absent ou multiple"), false);
	y = 0;
	while (y < st->map_height)
	{
		if (y == 0 || y + 1 == st->map_height)
		{
			if (!check_border_line(st->map_lines[y]))
				return (false);
		}
		else if (!scan_row(st, y))
			return (false);
		y++;
	}
	return (true);
}
