#ifndef MINISHELL_H
#define MINISHELL_H

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
    TRUE,
    FALSE
}   T_QUOTO_FLG;

typedef struct
{
    T_TYPE type;//토큰의 타입
    int value;//숫자인 경우 그 값
} Token;

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
}               t_rdt;

typedef struct s_s_cmd
{
    char        *basic_command;
    struct s_arg    *option;
}               t_s_cmd;

typedef struct s_arg
{
    char        *filename;
    struct s_option        *option;
    char        *variable;
    char        *arg;
    T_QUOTO_FLG     *quoto;
}               t_arg;

typedef struct s_option
{
    struct s_option        *next;
    char                    *data;
}               t_option;

#endif