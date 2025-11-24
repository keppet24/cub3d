#include "cube3d.h"

static void	init_state(t_parser_state *state)
{
	int	i;

	i = 0;
	while (i < TEX_TOTAL)
	{
		state->tex_path[i] = NULL;
		i++;
	}
	state->floor.set = false;
	state->ceiling.set = false;
	state->map_lines = NULL;
	state->map_width = 0;
	state->map_height = 0;
	state->player_count = 0;
	state->player_dir = '\0';
	state->player_x = 0;
	state->player_y = 0;
}

static void	destroy_state(t_parser_state *state)
{
	int	i;

	i = 0;
	while (i < TEX_TOTAL)
	{
		free(state->tex_path[i]);
		state->tex_path[i] = NULL;
		i++;
	}
	i = 0;
	while (state->map_lines && i < (int)state->map_height)
	{
		free(state->map_lines[i]);
		i++;
	}
	free(state->map_lines);
}

int	parse(const char *path)
{
	t_parser_state	state;
	char			*first_map_line;
	int				fd;
	bool			ok;

	init_state(&state);
	fd = open(path, O_RDONLY);
	if (fd < 0)
	{
		print_error("Impossible d'ouvrir le fichier");
		return (1);
	}
	ok = parse_header(fd, &state, &first_map_line);
	if (ok)
		ok = parse_map(fd, &state, first_map_line);
	if (ok)
		ok = validate_map(&state);
	close(fd);
	destroy_state(&state);
	if (!ok)
		return (1);
	return (0);
}
