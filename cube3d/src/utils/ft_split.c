#include "ft_split.h"
#include "cube3d.h"

static int	countwords(char const *s, char c)
{
	int	i;
	int	resu;

	i = 0;
	resu = 0;
	while (s[i])
	{
		if ((s[i] != c && s[i + 1] == c) || (s[i] != c && s[i + 1] == 0))
			resu++;
		i++;
	}
	return (resu);
}

static void	free_tab(char **tab, int j)
{
	int	i;

	i = 0;
	while (i < j)
		free(tab[i++]);
	free(tab);
}

static char	*fill(char const *s, char c)
{
	int		i;
	int		deb;
	char	*resu;
	int		j;

	j = 0;
	i = 0;
	while (s[i] && s[i] == c)
		i++;
	if (s[i] == 0)
		return (NULL);
	deb = i;
	while (s[i] != c && s[i])
		i++;
	resu = malloc(sizeof(char) * (i - deb + 1));
	if (!resu)
		return (NULL);
	while (j < (i - deb))
	{
		resu[j] = s[deb + j];
		j++;
	}
	resu[j] = 0;
	return (resu);
}

char	**ft_split(char const *s, char c)
{
	int		i;
	int		j;
	char	**resu;

	i = 0;
	j = 0;
	resu = malloc(sizeof(char *) * (countwords(s, c) + 1));
	if (!resu)
		return (NULL);
	while (s[i])
	{
		if (s[i] != c)
		{
			resu[j] = fill(&s[i], c);
			if (!resu[j])
				return (free_tab(resu, j), NULL);
			j++;
			while (s[i] && s[i] != c)
				i++;
		}
		else
			i++;
	}
	resu[j] = NULL;
	return (resu);
}

void	ft_free_split(char **tab)
{
	int	i;

	if (!tab)
		return ;
	i = 0;
	while (tab[i])
	{
		free(tab[i]);
		i++;
	}
	free(tab);
}
