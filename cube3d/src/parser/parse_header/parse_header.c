#include "parse_header_internal.h"

static bool	parse_texture_path(t_parser_state *state, int idx, char *payload)
{
	char	*dup;

	while (*payload && ft_isspace(*payload))
		payload++;
	if (*payload == '\0')
		return (print_error("Chemin de texture manquant"), false);
	if (state->tex_path[idx])
		return (print_error("Texture dupliquee"), false);
	if (texture_payload_invalid(payload))
		return (print_error("Chemin de texture invalide"), false);
	if (texture_is_xpm(payload))
		return (print_error("Texture doit etre en .xpm"), false);
	if (texture_unreadable(payload))
		return (print_error("Fichier texture inaccessible"), false);
	dup = ft_strdup(payload);
	if (!dup)
		return (print_error("Allocation echouee"), false);
	state->tex_path[idx] = dup;
	return (true);
}

static bool	parse_color_value(const char *payload, t_color *color, const char *label)
{
	char	**parts;
	int		values[3];
	int		i;

	if (color->set)
	{
		print_error("Couleur dupliquee");
		return (false);
	}
	parts = ft_split(payload, ',');
	if (!parts)
	{
		print_error(label);
		return (false);
	}
	i = 0;
	while (i < 3)
	{
		if (!parse_component(parts[i], &values[i]))
		{
			ft_free_split(parts);
			print_error(label);
			return (false);
		}
		i++;
	}
	ft_free_split(parts);
	color->r = values[0];
	color->g = values[1];
	color->b = values[2];
	color->set = true;
	return (true);
}

static bool	handle_identifier(char *line, t_parser_state *state)
{
	size_t	i;

	i = 0;
	if (ft_isspace(line[i]))
		return (print_error("Identifiant avec espaces en debut de ligne"),
			false);
	if (line[i] == 'N' && line[i + 1] == 'O' && ft_isspace(line[i + 2]))
		return (parse_texture_path(state, TEX_NO, line + i + 2));
	if (line[i] == 'S' && line[i + 1] == 'O' && ft_isspace(line[i + 2]))
		return (parse_texture_path(state, TEX_SO, line + i + 2));
	if (line[i] == 'W' && line[i + 1] == 'E' && ft_isspace(line[i + 2]))
		return (parse_texture_path(state, TEX_WE, line + i + 2));
	if (line[i] == 'E' && line[i + 1] == 'A' && ft_isspace(line[i + 2]))
		return (parse_texture_path(state, TEX_EA, line + i + 2));
	if (line[i] == 'F' && ft_isspace(line[i + 1]))
		return (parse_color_value(line + i + 1, &state->floor,
				"Couleur F invalide"));
	if (line[i] == 'C' && ft_isspace(line[i + 1]))
		return (parse_color_value(line + i + 1, &state->ceiling,
				"Couleur C invalide"));
	print_error("Identifiant inconnu");
	return (false);
}

static bool	handle_map_line(t_parser_state *state, char **first_map_line,
		char *line)
{
	if (!header_complete(state))
	{
		print_error("Identifiants manquants avant la carte");
		free(line);
		return (false);
	}
	*first_map_line = line;
	return (true);
}

bool	parse_header(int fd, t_parser_state *state, char **first_map_line)
{
	char	*line;

	*first_map_line = NULL;
	while ((line = get_next_line(fd)))
	{
		trim_newline(line);
		if (line_is_empty(line))
		{
			free(line);
			continue ;
		}
		if (looks_like_map_line(line))
			return (handle_map_line(state, first_map_line, line));
		if (!handle_identifier(line, state))
			return (free(line), false);
		free(line);
	}
	print_error("Carte manquante");
	return (false);
}
