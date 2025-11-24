#include "cube3d.h"

void	print_error(const char *msg)
{
	write(STDERR_FILENO, "Error\n", 6);
	if (msg)
	{
		write(STDERR_FILENO, msg, ft_strlen(msg));
		write(STDERR_FILENO, "\n", 1);
	}
}
