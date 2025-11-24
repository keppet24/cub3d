#include "parse_header_internal.h"

bool	is_map_char(char c)
{
	return (c == '0' || c == '1' || c == 'N' || c == 'S' || c == 'E'
		|| c == 'W');
}

bool	looks_like_map_line(const char *line)
{
	size_t	i;

	i = 0;
	while (line[i] && ft_isspace(line[i]))
		i++;
	if (!line[i])
		return (false);
	if ((line[i] == 'N' && line[i + 1] == 'O')
		|| (line[i] == 'S' && line[i + 1] == 'O')
		|| (line[i] == 'W' && line[i + 1] == 'E')
		|| (line[i] == 'E' && line[i + 1] == 'A')
		|| (line[i] == 'F' && ft_isspace(line[i + 1]))
		|| (line[i] == 'C' && ft_isspace(line[i + 1])))
		return (false);
	return (is_map_char(line[i]) || line[i] == ' ');
}

bool	header_complete(const t_parser_state *state)
{
	if (!state->tex_path[TEX_NO] || !state->tex_path[TEX_SO]
		|| !state->tex_path[TEX_WE] || !state->tex_path[TEX_EA])
		return (false);
	if (!state->floor.set || !state->ceiling.set)
		return (false);
	return (true);
}

bool	texture_payload_invalid(char *payload)
{
	size_t	j;

	trim_trailing_spaces(payload);
	if (*payload == '\0')
		return (true);
	j = 0;
	while (payload[j])
	{
		if (ft_isspace(payload[j]))
			return (true);
		j++;
	}
	return (false);
}

bool	texture_is_xpm(const char *path)
{
	size_t	len;

	len = ft_strlen(path);
	if (len < 4)
		return (true);
	if (path[len - 4] != '.' || path[len - 3] != 'x'
		|| path[len - 2] != 'p' || path[len - 1] != 'm')
		return (true);
	return (false);
}

bool	texture_unreadable(const char *path)
{
	int	fd;

	fd = open(path, O_RDONLY);
	if (fd < 0)
		return (true);
	close(fd);
	return (false);
}

bool	parse_component(const char *s, int *out)
{
	long	val;
	size_t	i;

	i = 0;
	while (s[i] && ft_isspace(s[i]))
		i++;
	if (!ft_isdigit(s[i]))
		return (false);
	val = 0;
	while (s[i] && ft_isdigit(s[i]))
		val = val * 10 + (s[i++] - '0');
	while (s[i] && ft_isspace(s[i]))
		i++;
	if (s[i] != '\0' || val < 0 || val > 255)
		return (false);
	*out = (int)val;
	return (true);
}
