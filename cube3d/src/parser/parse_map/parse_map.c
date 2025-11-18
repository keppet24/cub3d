#include "parse_map_internal.h"

/* init_map_buffer
 * Params: buf (buffer de lignes)
 * Retour: none
 * Appelé par: parse_map (avant tout push)
 * Exemple concret:
 *   t_map_buffer buf;
 *   init_map_buffer(&buf); // buf.lines=NULL, size=0, capacity=0
 */
static void	init_map_buffer(t_map_buffer *buf)
{
	buf->lines = NULL;
	buf->size = 0;
	buf->capacity = 0;
}

/* free_map_buffer
 * Params: buf (buffer de lignes)
 * Retour: none
 * Appelé par: parse_map en erreur, destroy_state pour nettoyer
 * Exemple concret:
 *   free_map_buffer(&buf); // libère chaque buf.lines[i] puis buf.lines
 */
static void	free_map_buffer(t_map_buffer *buf)
{
	size_t	i;

	if (!buf->lines)
		return ;
	i = 0;
	while (i < buf->size)
	{
		free(buf->lines[i]);
		i++;
	}
	free(buf->lines);
}

/* push_line_resize
 * Params: buf (buffer)
 * Retour: bool (false si alloc échoue)
 * Appelé par: push_line
 * Exemple concret:
 *   if (!push_line_resize(&buf)) return false;
 */
static bool	push_line_resize(t_map_buffer *buf)
{
	char	**tmp;
	size_t	newcap;
	size_t	i;

	if (buf->size != buf->capacity)
		return (true);
	if (buf->capacity == 0)
		newcap = 8;
	else
		newcap = buf->capacity * 2;
	tmp = malloc(sizeof(char *) * newcap);
	if (!tmp)
		return (false);
	i = 0;
	while (i < buf->size)
	{
		tmp[i] = buf->lines[i];
		i++;
	}
	free(buf->lines);
	buf->lines = tmp;
	buf->capacity = newcap;
	return (true);
}

/* push_line
 * Params: buf (buffer), line (à dupliquer), st (parser_state pour width/height)
 * Retour: bool (false si alloc échoue)
 * Appelé par: handle_map_line
 * Exemple concret:
 *   push_line(&buf, "11111", st);
 *   push_line(&buf, "10N01", st); // met à jour map_width/height si besoin
 */
static bool	push_line(t_map_buffer *buf, const char *line, t_parser_state *st)
{
	size_t	len;

	if (!push_line_resize(buf))
		return (false);
	buf->lines[buf->size] = ft_strdup(line);
	if (!buf->lines[buf->size])
		return (false);
	len = ft_strlen(line);
	if (len > st->map_width)
		st->map_width = len;
	buf->size++;
	st->map_height = buf->size;
	return (true);
}

/* handle_map_line
 * Params: buf, st, line (ligne brute de carte)
 * Retour: bool (false si charset invalide, joueur multiple, ligne vide)
 * Appelé par: parse_map (pour first_map_line et les suivantes)
 * Exemple concret:
 *   handle_map_line(&buf, st, "10N01"); // charset OK, joueur trouvé, remplacé par '0'
 *   handle_map_line(&buf, st, "10A01"); // renvoie false (caractère invalide)
 */
static bool	handle_map_line(t_map_buffer *buf, t_parser_state *st, char *line)
{
	size_t	i;

	if (line[0] == '\0')
		return (print_error("Ligne vide dans la carte"), false);
	i = 0;
	while (line[i])
	{
		if (line[i] == '\t')
			line[i] = ' ';
		if (line[i] == 'N' || line[i] == 'S' || line[i] == 'E' || line[i] == 'W')
		{
			if (st->player_count++ > 0)
				return (print_error("Joueur multiple"), false);
			st->player_dir = line[i];
			st->player_x = (int)i;
			st->player_y = (int)buf->size;
			line[i] = '0';
		}
		else if (line[i] != '0' && line[i] != '1' && line[i] != ' ')
			return (print_error("Caractere invalide dans la carte"), false);
		i++;
	}
	return (push_line(buf, line, st));
}

/* parse_map
 * Params: fd (descripteur déjà positionné), state (parser_state), first_map_line (première ligne de carte)
 * Retour: bool (false si erreur ou joueur absent/multiple)
 * Appelé par: parse (src/parser/parser.c)
 * Exemple concret:
 *   // first_map_line = "11111"
 *   parse_map(fd, &state, first_map_line);
 *   // remplit state->map_lines, map_width/height et position joueur unique
 */
bool	parse_map(int fd, t_parser_state *state, char *first_map_line)
{
	t_map_buffer	buf;
	char			*line;
	bool			ok;

	init_map_buffer(&buf);
	ok = handle_map_line(&buf, state, first_map_line);
	free(first_map_line);
	while (ok && (line = get_next_line(fd)))
	{
		trim_newline(line);
		ok = handle_map_line(&buf, state, line);
		free(line);
	}
	if (!ok || state->player_count != 1)
	{
		if (ok && state->player_count != 1)
			print_error("Joueur absent ou multiple");
		free_map_buffer(&buf);
		return (false);
	}
	state->map_lines = buf.lines;
	return (true);
}
