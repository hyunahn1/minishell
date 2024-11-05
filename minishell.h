#ifndef MINISHELL_H
#define MINISHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    T_PIPE,
    T_REDIRECTION,
    T_BAISC_COMMAND,
    T_FILENAME,
    T_VARIABLE,
    T_OPTION
}   T_TYPE;

typedef enum
{
    T_DLESS,    //  <<
    T_DGREAT,   //  >>
    T_LESS,     //  <
    T_GREAT     //  >
}   T_RDT;


typedef enum
{
    EVEN,
    ODD
}   T_QUOTO_FLG;

typedef struct s_pipe
{
    struct s_cmd *cmd;
    int         index;  //인덱스 위치
    char        pipe;   //저장할 파이프 "|"
    struct s_pipe   *next;
}               t_pipe;

typedef struct s_cmd
{
    struct s_rdts        *redirections;
    struct s_s_cmd *simple_cmd;
}               t_cmd;

typedef struct s_rdts
{
    struct s_rdts    *next;
    char            *redirect;
    struct s_arg           *arg;
    T_RDT           RDT_TOKEN;
}               t_rdts;

typedef struct s_s_cmd
{
    char        *basic_command;
    struct s_arg    *arg;
}               t_s_cmd;

typedef struct s_arg
{
    char        *filename;
    struct s_option        *option;
    char        *variable;  //   $
    char        *arg;       //   ' "
    T_QUOTO_FLG     *quoto;
}               t_arg;

typedef struct s_option
{
    struct s_option        *next;
    char                    *data;
}               t_option;

char	**ft_split(char const *s, char c);

size_t	ft_strlen(const char *s);
void	ft_bzero(void *s, size_t n);
void	*ft_calloc(size_t nmemb, size_t size);
int	ft_strncmp(const char *s1, const char *s2, size_t n);
char	*ft_strdup(const char *s);

t_option *new_option(char *data);
t_arg *new_arg(char *filename, char *variable, char *arg);
t_pipe *new_command();
void add_redirection(t_cmd *command, char *redirect, t_arg *arg);
void add_simple_command(t_cmd *command, char *basic_command, t_arg *arg);
void add_command(t_pipe **command_list, t_pipe *command);
void add_option(t_arg *arg, char *data);
void parse_and_store(char *input, t_pipe **command_list);
void print_commands(t_pipe *command_list);
void ft_free(char **tokens);


#endif
