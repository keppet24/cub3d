#ifndef PARSE_MAP_INTERNAL_H
# define PARSE_MAP_INTERNAL_H

# include "../parse_header/parse_header_internal.h"

typedef struct s_map_buffer
{
	char	**lines;
	size_t	size;
	size_t	capacity;
}	t_map_buffer;

#endif
