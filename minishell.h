/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msoklova <msoklova@student.42heilbronn.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/18 14:16:08 by msoklova          #+#    #+#             */
/*   Updated: 2024/08/21 12:25:46 by msoklova         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <errno.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/wait.h>
# include <unistd.h>
# include "libft/libft.h"

/* Readline Functions */
# include <readline/history.h>
# include <readline/readline.h>

/* token type  */
typedef enum e_ttype
{
	NOTOKEN,
	WORD,
	PIPE,
	REDIRECTION
}	t_ttype;

/* save tokens somewhere */
typedef struct s_token
{
	char			*value;
	t_ttype			ttype;
	struct s_token	*next;
}	t_token;

/* token functions */
t_ttype	check_type(char *value);

/* Function prototypes */
void	init_shell(void);
void	handle_signals(int signo);
void	parse_command(char *input);
void	execute_command(char **args);
int		is_builtin(char *command);
void	execute_builtin(char **args);

#endif
