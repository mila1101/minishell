#include "minishell.h"

void	init_shell(t_data *data)
{
	signal(SIGINT, handle_signals);
	signal(SIGQUIT, SIG_IGN);
	data->mini_count = 1;
}

void	cleanup(t_data *data)
{
	if (data)
	{
		free_env_list(&(data->env_list)); // Free environment list
		free(data);                       // Free t_data struct itself
	}
	clear_history(); // Free the history list (from readline)
}

void	parsing_handler(char *input, t_data *data)
{
	t_prompt	test;
	int			i;

	test.cmds = NULL;
	parse_command(input, data, &test);
	// print_cmds(test.cmds);
	i = ft_lstsize(test.cmds);
	// printf("lstsize:%d\n", i);
	while (i-- > 0)
	{
		waitpid(-1, &data->prev_exit_stat, 0);
		printf("data->prev_exit_stat:%d\n", data->prev_exit_stat);
	}
}

int	main(int argc, char **argv, char **envp)
{
	char *input;
	t_data *data;

	if (argc != 1 || argv == NULL)
		exit(EXIT_FAILURE);
	data = malloc(sizeof(t_data));
	if (!data)
		exit(EXIT_FAILURE);
	data->shlvl_history = NULL;
	data->env_list = NULL;
	data->mini_count = 1;
	data->prev_exit_stat = 0;
	data->env_list = init_env_list(envp);
	if (!data->env_list)
	{
		cleanup(data);
		exit(EXIT_FAILURE);
	}
	init_shell(data);
	if (DEBUG)
	{
		input = getenv("DEBUG_INPUT");
		if (input == NULL)
		{
			input = "cat << test | grep te";
		}
		printf("Debug mode with input: %s\n", input);
		add_history(input);
		// parse_command(input, data);
	}
	else
	{
		while (1)
		{
			signal(SIGINT, handle_signals);
			signal(SIGQUIT, SIG_IGN);
			input = readline("minishell> ");
			if (!input)
			{
				printf("exit\n");
				break ;
			}
			if (input[0] != '\0')
				add_history(input);
			parsing_handler(input, data);
			// parse_command(input, data);
			free(input);
		}
		free_env_list(&(data->env_list));
		// free(data);
	}
	cleanup(data);
	return (0);
}