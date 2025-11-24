/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: othmaneettaqi <othmaneettaqi@student.42    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 14:23:53 by othmaneetta       #+#    #+#             */
/*   Updated: 2025/06/17 15:37:51 by othmaneetta      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cube3d.h"

static char    *append(char *str, char c, int len)
{
    char    *new = malloc(len + 2);
    int        i = 0;
    while (i < len)
    {
        new[i] = str[i];
        i++;
    }
    new[i++] = c;
    new[i] = '\0';
    free(str);
    return new;
}

char	*get_next_line(int fd)
{
    static char    buff[BUFFER_SIZE];
    static int    readed = 0;
    static int    i = 0;
    char        *line = NULL;
    int            len = 0;

    if (fd < 0 || BUFFER_SIZE <= 0)
        return NULL;
    while (1)
    {
        if (i >= readed)
        {
            readed = read(fd, buff, BUFFER_SIZE);
            i = 0;
            if (readed <= 0)
                return line;
        }
        while (i < readed)
        {
            line = append(line, buff[i], len++);
            if (buff[i++] == '\n')
                return line;
        }
    }
}
