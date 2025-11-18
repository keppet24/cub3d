#include "parse_header_internal.h"

void	trim_newline(char *line)
{
	size_t	len;

	if (!line)
		return ;
	len = ft_strlen(line);
	if (len > 0 && line[len - 1] == '\n')
		line[len - 1] = '\0';
}

bool	line_is_empty(const char *line)
{
	size_t	i;

	if (!line)
		return (true);
	i = 0;
	while (line[i])
	{
		if (!ft_isspace(line[i]))
			return (false);
		i++;
	}
	return (true);
}

void	trim_trailing_spaces(char *str)
{
	size_t	len;

	len = ft_strlen(str);
	while (len > 0 && ft_isspace(str[len - 1]))
	{
		str[len - 1] = '\0';
		len--;
	}
}
