#include "minishell.h"

static char	**ft_free_all(char **str)
{
	size_t	i;

	i = 0;
	while (str[i])
		free(str[i++]);
	free(str);
	return (0);
}

static size_t	ft_result_word(const char *s, char c)
{
	size_t	index;
	size_t	finish;

	index = 0;
	finish = 0;
	while (s[index])
	{
		while (s[index] == c)
			index++;
		if (s[index] && s[index] != c)
		{
			finish++;
			while (s[index] != c && s[index])
				index++;
		}
	}
	return (finish);
}

static char	*plus_str(char const *s, char c)
{
	size_t	i;
	char	*dest;

	i = 0;
	while (s[i] && s[i] != c)
		i++;
	dest = (char *)ft_calloc(sizeof(char) * i + 1, sizeof(char));
	if (!dest)
		return (0);
	i = 0;
	while (s[i] && s[i] != c)
	{
		dest[i] = s[i];
		i++;
	}
	return (dest);
}

static int	all_str_put(char const *s, char c, char **str, size_t i)
{
	size_t	j;

	j = 0;
	while (s[i])
	{
		while (s[i] && s[i] == c)
			i++;
		if (s[i] && s[i] != c)
		{
			str[j++] = plus_str(&s[i], c);
			if (!str)
				ft_free_all(str);
		}
		while (s[i] != c && s[i])
			i++;
	}
	return (1);
}

char	**ft_split(char const *s, char c)
{
	size_t	finish;
	char	**str;
	size_t	i;

	if (!s)
		return (0);
	finish = ft_result_word(s, c);
	i = 0;
	str = (char **)ft_calloc((finish + 1), sizeof(char *));
	if (!str)
		return (0);
	all_str_put(s, c, str, i);
	return (str);
}