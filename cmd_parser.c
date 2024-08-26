#include "minishell.h"

// MORE TESTS

char	*space_terminator(char *s)
{
	char	*new_str;
	int		i;
	int		j;

	i = 0;
	j = 0;
	while (s[i] != '\0')
	{
		while (s[i] == ' ')
			i++;
		i++;
		j++;
	}
	new_str = malloc(sizeof(char) * (j + 1));
	i = 0;
	j = 0;
	while (s[i] != '\0')
	{
		while (s[i] == ' ')
			i++;
		new_str[j++] = s[i++];
	}
	new_str[j] = '\0';
	return (new_str);
}

char	*token_spacer(char *s)
{
	char	*new_str;
	int		i;
	int		j;

	i = 0;
	j = 0;
	while (s[i] != '\0')
	{
		if (s[i] == '\\' || s[i] == '<' || s[i] == '|' || s[i] == '>')
		{
			if (i > 0 && s[i - 1] != ' ')
				j++;
			j++;
			if (s[i + 1] != ' ')
				j++;
		}
		else
			j++;
		i++;
	}
	new_str = malloc(sizeof(char) * (j + 1));
	if (!new_str)
		return (NULL);
	i = 0;
	j = 0;
	while (s[i] != '\0')
	{
		if (s[i] == '\\' || s[i] == '<' || s[i] == '|' || s[i] == '>')
		{
			if (i > 0 && s[i - 1] != ' ')
				new_str[j++] = ' ';
			new_str[j++] = s[i];
			if ((s[i + 1] == '<' && s[i] == '<') || (s[i + 1] == '>'
					&& s[i] == '>'))
			{
				new_str[j++] = s[++i];
			}
			if (s[i + 1] != ' ')
				new_str[j++] = ' ';
		}
		else
		{
			new_str[j++] = s[i];
		}
		i++;
	}
	new_str[j] = '\0';
	return (new_str);
}

void	parse_command(char *input)
{
	char	**args;
	int		i;
	char	*new_str;

	// char	*temp;
	// int		start;
	// int		end;
	i = 0;
	// start = 0;
	new_str = token_spacer(input);
	printf("%s\n", new_str);
	// FT_SPLIT MUST NOT BE MODIFIED. I CHANGED MY MIND XD we can check the next node if it is a flag. took too long to figure this out. it is a shame. this turned out into my diary. hope patrick won't read this. anyway next job can be categorizing them.
	args = ft_split(new_str, ' ');
	while (args[i])
	{
		printf("args[%d]:%s\n", i, args[i]);
		i++;
	}
	if (args[0] == NULL)
		return ;
	if (is_builtin(args[0]))
	{
		execute_builtin(args);
	}
	else
	{
		execute_command(args);
	}
}

// int	main(void)
// {
// 	printf("%s\n", space_terminator("lsttest | cat -e   <<her_doc>file1 >> file2"));
// }