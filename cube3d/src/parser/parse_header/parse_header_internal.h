#ifndef PARSE_HEADER_INTERNAL_H
# define PARSE_HEADER_INTERNAL_H

# include "cube3d.h"

void	trim_newline(char *line);
bool	line_is_empty(const char *line);
void	trim_trailing_spaces(char *str);
bool	is_map_char(char c);
bool	looks_like_map_line(const char *line);
bool	header_complete(const t_parser_state *state);
bool	texture_payload_invalid(char *payload);
bool	parse_component(const char *s, int *out);

#endif
