#include "minishell.h"

// MORE TESTS

static t_mini	*mini_init(void)
{
	t_mini	*mini;

	mini = malloc(sizeof(t_mini));
	if (!mini)
		return (NULL);
	mini->full_cmd = NULL;
	mini->full_path = NULL;
	mini->infile = STDIN_FILENO;
	mini->outfile = STDOUT_FILENO;
	return (mini);
}

/**
 * @brief This function put spaces between tokens if necessary
 * @return a string
 */
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

static int	ft_count_words(const char *s, char *c, int i[2])
{
	enum QuoteState	quote_state;

	quote_state = NO_QUOTE;
	printf("ft_count_words: %s\n", s);
	/*this needs to be removed its triggering me :( */
	while (s[i[0]] != '\0')
	{
		// Check if not a delimiter or within quotes
		while ((!ft_strchr(c, s[i[0]]) || quote_state != NO_QUOTE)
			&& s[i[0]] != '\0')
		{
			// Toggle quote state when encountering quotes
			if (quote_state == NO_QUOTE && (s[i[0]] == '\'' || s[i[0]] == '\"'))
			{
				if (s[i[0]] == '\'')
					quote_state = SINGLE_QUOTE;
				else if (s[i[0]] == '\"')
					quote_state = DOUBLE_QUOTE;
			}
			else if ((quote_state == SINGLE_QUOTE && s[i[0]] == '\'')
					|| (quote_state == DOUBLE_QUOTE && s[i[0]] == '\"'))
			{
				quote_state = NO_QUOTE;
			}
			i[0]++;
		}
		i[1]++;
		// Skip delimiters to avoid infinite loops
		while (ft_strchr(c, s[i[0]]) && s[i[0]] != '\0')
			i[0]++;
	}
	return (i[1]);
}

/**
 * @brief This function splits words without removing the quotes
 * @return array of splitted strings.
 */
char	**split_with_quotes(const char *s, char *del)
{
	char			**arr;
	int				words_len;
	int				s_len;
	enum QuoteState	quote_state;
	int				i[3] = {0};
	int				counts[2] = {0};

	quote_state = NO_QUOTE;
	words_len = ft_count_words(s, del, counts);
	arr = malloc((words_len + 1) * sizeof(char *));
	if (arr == NULL)
		return (NULL);
	s_len = ft_strlen(s);
	// Iterate over the string to split words
	while (s[i[0]] != '\0')
	{
		// Skip delimiter characters
		while (ft_strchr(del, s[i[0]]) && s[i[0]] != '\0')
			i[0]++;
		i[1] = i[0];
		// Traverse the word, considering quotes
		while ((!ft_strchr(del, s[i[0]]) || quote_state != NO_QUOTE)
			&& s[i[0]] != '\0')
		{
			// Enter quote state if a quote is encountered
			if (quote_state == NO_QUOTE && (s[i[0]] == '\'' || s[i[0]] == '\"'))
			{
				if (s[i[0]] == '\'')
					quote_state = SINGLE_QUOTE;
				else
					quote_state = DOUBLE_QUOTE;
			}
			// Exit quote state if the matching quote is encountered again
			else if ((quote_state == SINGLE_QUOTE && s[i[0]] == '\'')
					|| (quote_state == DOUBLE_QUOTE && s[i[0]] == '\"'))
				quote_state = NO_QUOTE;
			i[0]++;
		}
		// Add the word to the array, handle empty case properly
		if (i[1] >= s_len)
			arr[i[2]++] = "\0";
		else
			arr[i[2]++] = ft_substr(s, i[1], i[0] - i[1]);
	}
	if (quote_state != NO_QUOTE)
	{
		fprintf(stderr, "Error: Unclosed quotes detected\n");
		free(arr); // Free the allocated memory if an error occurs
		return (NULL);
	}
	arr[words_len] = NULL;
	return (arr);
}

char	**ft_extend_matrix(char **matrix, char *new_entry)
{
	int		i;
	char	**new_matrix;

	i = 0;
	while (matrix && matrix[i]) // Count existing elements
		i++;
	// Allocate memory for new matrix (existing elements + 1 new entry + NULL)
	new_matrix = malloc((i + 2) * sizeof(char *));
	if (!new_matrix)
		return (NULL);
	i = 0;
	while (matrix && matrix[i])
	{
		new_matrix[i] = matrix[i]; // Copy old entries
		i++;
	}
	new_matrix[i] = ft_strdup(new_entry); // Add new entry
	new_matrix[i + 1] = NULL;             // Null-terminate the array
	free(matrix);
	return (new_matrix);
}

int	is_redirection(char *arg)
{
	if (!arg)
		return (0);                  // Null argument is not a redirection
	if (ft_strcmp(arg, ">") == 0 ||  // Output redirection
		ft_strcmp(arg, ">>") == 0 || // Append output redirection
		ft_strcmp(arg, "<") == 0 ||  // Input redirection
		ft_strcmp(arg, "<<") == 0)   // Here-document redirection
	{
		return (1); // It is a redirection operator
	}
	return (0); // Not a redirection operator
}

/**
 * @brief Creates linked lists of commands split by pipes ('|').
 * Parses the input `args` array and creates separate linked
 * lists for each set of commands
 * and arguments divided by pipes. Returns an array of linked lists.
 * @param args Array of strings representing the parsed command input.
 * @return An array of linked lists,
 * each representing a command chain. The array ends with NULL.
 */
t_list	*fill_nodes(char **args)
{
	t_list	*cmds;
	t_list	*last_cmd;
	char	**temp[2];
	int		i;
	t_mini	*first_mini;

	i = -1;
	cmds = NULL;
	temp[1] = args;
	while (args[++i])
	{
		last_cmd = ft_lsttraverse(cmds);
		if (i == 0 || (args[i][0] == '|' && args[i + 1] && args[i + 1][0]))
		{
			i += (args[i][0] == '|');
			ft_lst_insertattail(&cmds, ft_lst_newlist(mini_init()));
			last_cmd = ft_lsttraverse(cmds);
		}
		first_mini = (t_mini *)last_cmd->content;
		// Check if the current argument is a redirection operator
		if (!is_redirection(args[i])) // Skip if it's a redirection
		{
			// Only add to full_cmd if it's not a redirection
			first_mini->full_cmd = ft_extend_matrix(first_mini->full_cmd,
					args[i]);
		}
		// Handle redirection but skip adding it to the command list
		get_redir(&first_mini, args, &i);
		printf("args[%d]: %s\n", i, args[i]);
		if (!args[i])
			break ;
	}
	return (cmds);
}

void	parse_command(char *input, t_data *data)
{
	t_prompt	test;
	char		*new_str;
	char		*expanded_str;
	char		**args;
	int			i;
	t_list		*cmd_node;
	t_mini		*mini_cmd;
	char		*trimmed_arg;
	int			saved_stdin;
	int			saved_stdout;
	pid_t		pid;
	int			status;

	new_str = token_spacer(input);
	if (!new_str)
		return ;
	expanded_str = expand_env_vars(new_str, data);
	free(new_str);
	args = split_with_quotes(expanded_str, " ");
	free(expanded_str);
	if (!args || args[0] == NULL)
	{
		free(args);
		return ;
	}
	i = 0;
	while (args[i])
	{
		printf("args:%s\n", args[i]);
		trimmed_arg = ft_strtrim_all(args[i]);
		args[i] = trimmed_arg;
		i++;
	}
	test.cmds = fill_nodes(args);
	print_cmds(test.cmds);

	if (test.cmds && test.cmds->next != NULL)
	{
		execute_pipes(test.cmds, data);
	}
	else
	{
		cmd_node = test.cmds;
		while (cmd_node)
		{
			mini_cmd = (t_mini *)cmd_node->content;
			if (mini_cmd && mini_cmd->full_cmd && mini_cmd->full_cmd[0])
			{
				// Check if it's a built-in command
				if (is_builtin(mini_cmd->full_cmd[0]))
				{
					// Save original file descriptors for stdin and stdout
					saved_stdin = dup(STDIN_FILENO);
					saved_stdout = dup(STDOUT_FILENO);
					// Apply redirection for built-ins
					if (mini_cmd->infile != STDIN_FILENO)
					{
						dup2(mini_cmd->infile, STDIN_FILENO);
						close(mini_cmd->infile);
					}
					if (mini_cmd->outfile != STDOUT_FILENO)
					{
						dup2(mini_cmd->outfile, STDOUT_FILENO);
						close(mini_cmd->outfile);
					}
					// Execute the built-in command
					execute_builtin(mini_cmd->full_cmd, data);
					// Restore standard input/output after the built-in execution
					dup2(saved_stdin, STDIN_FILENO);
					dup2(saved_stdout, STDOUT_FILENO);
					// Close saved descriptors
					close(saved_stdin);
					close(saved_stdout);
				}
				else
				{
					// For non-built-in commands (external commands)
					pid = fork();
					if (pid == 0)
					{
						// Apply redirection before executing the command
						if (mini_cmd->infile != STDIN_FILENO)
						{
							dup2(mini_cmd->infile, STDIN_FILENO);
							close(mini_cmd->infile);
						}
						if (mini_cmd->outfile != STDOUT_FILENO)
						{
							dup2(mini_cmd->outfile, STDOUT_FILENO);
							close(mini_cmd->outfile);
						}
						execute_command(mini_cmd->full_cmd);
						exit(EXIT_FAILURE);
					}
					else if (pid < 0)
					{
						perror("minishell");
					}
					else
					{
						waitpid(pid, &status, 0);
					}
				}
				check_and_update_shlvl(data, mini_cmd); // it's here the shlvl checking!!!!!!
			}
			cmd_node = cmd_node->next;
		}
	}
	i = 0;
	while (args[i])
	{
		free(args[i]);
		i++;
	}
	free(args);
}

// void	parse_command(char *input, t_data *data)
//{
//	t_prompt	test;
//	char		*new_str;
//	char		*expanded_str;
//	int			has_heredoc;
//	char		*delimiter;
//	char		*heredoc_pos;
//	char		**args;
//	int			i;
//	t_list		*cmd_node;
//	t_mini		*mini_cmd;
//	int			has_pipe;

//	has_heredoc = 0;
//	delimiter = NULL;
//	has_pipe = 0;
//	new_str = token_spacer(input);
//	if (!new_str)
//		return ;
//	expanded_str = expand_env_vars(new_str, data);
//	free(new_str);
//	args = split_with_quotes(expanded_str, " ");
//	free(expanded_str);
//	if (!args || args[0] == NULL)
//	{
//		free(args);
//		return ;
//	}
//	test.cmds = fill_nodes(args);
//	cmd_node = test.cmds;
//	while (cmd_node)
//	{
//		mini_cmd = (t_mini *)cmd_node->content;
//		if (mini_cmd && mini_cmd->full_cmd && mini_cmd->full_cmd[0])
//		{
//			if (ft_strchr(mini_cmd->full_cmd[0], '|'))
//			{
//				has_pipe = 1;
//				break ;
//			}
//		}
//		cmd_node = cmd_node->next;
//	}
//	if (has_pipe)
//	{
//		execute_pipes(test.cmds);
//	}
//	else
//	{
//		cmd_node = test.cmds;
//		while (cmd_node)
//		{
//			mini_cmd = (t_mini *)cmd_node->content;
//			if (mini_cmd && mini_cmd->full_cmd && mini_cmd->full_cmd[0])
//			{
//				if (is_builtin(mini_cmd->full_cmd[0]))
//				{
//					execute_builtin(mini_cmd->full_cmd, data);
//				}
//				else
//				{
//					execute_command(mini_cmd->full_cmd);
//				}
//			}
//			cmd_node = cmd_node->next;
//		}
//	i = 0;
//	while (args[i])
//	{
//		free(args[i]);
//		i++;
//	}
//	free(args);
//	//print_cmds(test.cmds);
//	}
//}

// int	main(void)
// {
// 	char input[] = "echo '\"Hello, \\\"World\\\"\" and 'it\\'s a test";
// 	char *processed = process_quotes(input);

// 	if (processed)
// 	{
// 		printf("Processed: %s\n", processed);
// 		free(processed);
// 	}

// 	return (0);
// }