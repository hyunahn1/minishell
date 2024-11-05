#include "minishell.h"

size_t	ft_strlen(const char *s)
{
	size_t	i;

	i = 0;
	while (s[i])
		i++;
	return (i);
}

void	ft_bzero(void *s, size_t n)
{
	unsigned char	*j;
	size_t			i;

	j = s;
	i = 0;
	while (i < n)
		j[i++] = 0;
}

void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*src;

	if (nmemb == 0 || size == 0)
		return (malloc(0));
	if ((nmemb * size) / nmemb != size)
		return (0);
	src = malloc(size * nmemb);
	if (!(src))
		return (0);
	ft_bzero(src, (size * nmemb));
	return (src);
}

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	unsigned int	i;

	i = 0;
	if (n == 0)
		return (0);
	while (s1[i] == s2[i] && i + 1 < n && s1[i] != '\0')
	{
		i++;
	}
	if (s1[i] == s2[i])
		return (0);
	else
		return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

char	*ft_strdup(const char *s)
{
	char	*c;
	size_t	size;
	size_t	i;

	size = ft_strlen(s);
	c = (char *)malloc(sizeof(char) * size + 1);
	if (!(c))
		return (0);
	i = 0;
	while (s[i])
	{
		c[i] = s[i];
		i++;
	}
	c[i] = '\0';
	return ((char *)c);
}