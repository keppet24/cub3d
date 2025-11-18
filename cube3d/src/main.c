#include "cube3d.h"

static bool	is_cub_file(const char *path)
{
	size_t	len;

	if (!path)
		return (false);
	len = ft_strlen(path);
	if (len <= 4)
		return (false);
	if (path[len - 4] != '.' || path[len - 3] != 'c' || path[len - 2] != 'u'
		|| path[len - 1] != 'b')
		return (false);
	if (path[len - 5] == '.' || path[len - 5] == '/')
		return (false);
	return (true);
}

int	main(int ac, char **av)
{
	if (ac != 2)
	{
		print_error("Nombre d'arguments invalide");
		return (1);
	}
	if (!is_cub_file(av[1]))
	{
		print_error("Le fichier doit se terminer par .cub");
		return (1);
	}
	if (parse(av[1]) != 0)
		return (1);
	return (0);
}
