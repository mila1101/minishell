
#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/libft.h"
# include <dirent.h>
# include <errno.h>
# include <fcntl.h>
# include <signal.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/ioctl.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <unistd.h>

/* Readline Functions */
# include <readline/history.h>
# include <readline/readline.h>

typedef struct s_prompt
{
	t_list				*cmds;
	char				**envp;
	pid_t				pid;
}						t_prompt;

typedef struct s_mini
{
	char				**full_cmd;
	char				*full_path;
	int					infile;
	int					outfile;
}						t_mini;

enum					QuoteState
{
	NO_QUOTE,
	SINGLE_QUOTE,
	DOUBLE_QUOTE
};

// Enum for count tracking
enum					Counts
{
	COUNT_INDEX,
	COUNT_WORDS
};

/* token type  */
typedef enum e_ttype
{
	NOTOKEN,
	WORD,
	PIPE,
	REDIRECTION
}						t_ttype;

/* save tokens somewhere */
typedef struct s_token
{
	char				*value;
	t_ttype				ttype;
	struct s_token		*next;
}						t_token;

typedef struct s_env
{
	char				*name;
	char				*value;
	struct s_env		*next;
}						t_env;

typedef struct s_shlvl_node
{
	int					shlvl_value;
	struct s_shlvl_node	*next;
}						t_shlvl_node;

typedef struct s_data
{
	int					fd_copy;
	t_env				*env_list;
	int					mini_count;
	int					prev_exit_stat;
	t_shlvl_node		*shlvl_history;
}						t_data;

typedef struct s_split_state
{
	int					i[3];
	int					counts[2];
	enum QuoteState		quote_state;
}						t_split_state;

typedef struct s_trim_info
{
	int					i;
	int					j;
	enum QuoteState		state;
	char				*trimmed;
}						t_trim_info;

typedef struct s_pipe_e_st
{
	int pipefd[2];          // File descriptors for pipes
	int in_fd;              // Input file descriptor for redirection
	pid_t pid;              // Process ID for forked processes
	int status;             // Status of the child processes
	pid_t child_pids[1024]; // Array of child process IDs
	int child_count;        // Number of child processes
	int i;                  // Loop index
}						t_pipe_e_st;

/* token functions */
t_ttype					check_type(char *value);

/* builtin commands */
char					*free_strjoin(char *str1, const char *str2);

/* export functions */
void					update_pwd(t_env **env_list);
void					update_old_pwd(t_env **env_list);

/* env functions */
char					*ft_strndup(const char *s1, size_t n);
t_env					*init_env_list(char **envp);
void					print_env_list(t_env *env);
void					free_env_list(t_env **env);
void					update_env(t_env **env_list, char *name, char *value);
// void				add_env(t_data *data, char *env_var);
// void				ft_store_env(t_data *data, char **env);
// void				initialize(t_data *data, char **env);
// void				free_env_list(t_data *data);
// void				print_env(t_data *data);

/* export functions */
// void					export_var(char **argv, t_env **env_list);
void					export_var(char **argv, t_env **env_list, t_data *data);
void					execute_export(char **argv, t_data *data);

/* unset functions */
void					unset_var(char *name, t_env **env_list);
void					execute_unset(char **argv, t_data *data);

/* signals */
// void	set_signal_fn(void);
// void	set_sig(int sig);

/* dollar sign */
void					*find_env_ref(t_env *env_list, char *name);
char					*expand_env_vars(char *input, t_data *data);
// char	*expand_env_vars(char *input, t_data *data, int is_echo);

/* Function prototypes */
// void				init_shell(void);
void					init_shell(t_data *data);
void					handle_signals(int signo);
void					parse_command(char **args, t_data *data,
							t_prompt *test);
void					free_strarray(char **array);
char					*find_slash(char *comm);
char					*find_exec(char *comm);
// void				execute_command(char **args);
void					execute_command(char **args, t_data *data);
int						is_builtin(char *command);
void					execute_builtin(char **args, t_data *data);
int						heredoc_handler(char *str[2], char *del);
void					execute_pipes(t_list *cmds, t_data *data);
void					update_shlvl(t_data *data, t_mini *mini_cmd);
void					check_and_update_shlvl(t_data *data, t_mini *mini_cmd);
void					free_shlvl_history(t_data *data);
void					clean_up(t_data *data);
void					free_strarray(char **array);

/* Trim Functions */
char					*ft_strtrim_all(const char *s1);
// int is_only_pwd(t_mini *mini);

/* Redirection Functions */
int						get_fd(int oldfd, char *path, int flags[2],
							t_data *data);
// void					outfile1(t_mini **node, char **args, int *i);
void					get_redir(t_mini **node, char **args, int *i,
							t_data *data);
int						is_redir(char *arg);
void					outfile1(t_mini **node, char **args, int *i,
							t_data *data);
void					outfile2(t_mini **node, char **args, int *i,
							t_data *data);
void					infile1(t_mini **node, char **args, int *i,
							t_data *data);
void					infile2(t_mini **node, char **args, int *i,
							t_data *data);

/* Print Functions */
void					print_cmds(t_list *cmds);

/* Parsing Functions */
char					**split_with_quotes(const char *s, char *del);
char					*token_spacer(char *s);
t_list					*fill_nodes(char **args, t_data *data);
void					free_content(void *content);
char					**ft_extend_matrix(char **matrix, char *new_entry);
void					ft_free_matrix(char ***m);
static t_mini			*mini_init(void);
void					free_mini(void *content);
void					parsing_handler(char *input, t_data *data);

/* Builtin Functions*/
void					b_exit(char **args, t_data *data);
void					b_echo(char **args);
void					b_pwd(void);
void					b_cd(char **args, t_data *data);

/* Pipe Functions */
void					wait_for_all_children(t_pipe_e_st *e_st);
void					setup_pipe(t_pipe_e_st *e_st);
void					child_process_helper(t_pipe_e_st *e_st,
							t_list *cmd_node, t_mini *mini_cmd, t_data *data);

/* Error Functions */
int						handle_syntax_error(char **args);

/* utils*/
t_data					*initialize_data(char **envp);
void					cleanup(t_data *data);
void					curr_loop(t_env *curr, char *value, char *name);
void					export_error(t_data *data, char *name, char *value);
int	is_valid_var_name(const char *name);

# ifndef DEBUG
#  define DEBUG 0
# endif

#endif