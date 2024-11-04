#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **ft_split(const char *str, char delimiter);  // 이미 구현된 ft_split 선언
void free_split(char **split);  // split 배열을 해제하는 함수 선언

// 리다이렉션을 저장할 구조체
typedef struct s_rdts
{
    struct s_rdts *next;
    char *redirect;
    struct s_arg *arg;
} t_rdts;

// 옵션 리스트를 저장할 구조체
typedef struct s_option
{
    struct s_option *next;    // 다음 옵션으로의 포인터
    char *data;               // 옵션 데이터 (예: "-l", "-a")
} t_option;

// 인자 구조체, 파일명, 옵션, 변수 등을 저장
typedef struct s_arg
{
    char *filename;          // 파일 이름
    t_option *option;        // 옵션 리스트
    char *variable;          // 변수명 (예: "$HOME")
    char *arg;               // 인자 문자열 (기타 인자)
    int quoto;               // 따옴표 플래그 (0: 없음, 1: 있음)
} t_arg;

// 간단한 명령어와 옵션을 저장할 구조체
typedef struct s_s_cmd
{
    char *basic_command;     // 기본 명령어 (예: "echo", "ls")
    t_arg *option;           // 옵션을 저장하는 t_arg 구조체 포인터
} t_s_cmd;

// 메인 명령어 구조체, 리다이렉션과 간단한 명령어를 포함
typedef struct s_cmd
{
    t_rdts *redirections;    // 리다이렉션 리스트
    t_s_cmd *simple_cmd;     // 간단한 명령어 구조체
} t_cmd;

// 명령어 리스트를 보관하기 위한 구조체 (각 명령어에 대한 토큰 리스트를 가짐)
typedef struct s_pipe
{
    t_cmd *cmd;
    struct s_pipe *next;
} t_pipe;

#include <stdlib.h>
#include <string.h>

// 문자열의 개수를 세는 함수
static int count_tokens(const char *str, char delimiter)
{
    int count = 0;
    int in_token = 0;

    while (*str)
    {
        if (*str != delimiter && !in_token)
        {
            in_token = 1;
            count++;
        }
        else if (*str == delimiter)
        {
            in_token = 0;
        }
        str++;
    }
    return count;
}

// 각 토큰을 동적으로 할당하여 배열에 저장하는 함수
static char *copy_token(const char *start, size_t length)
{
    char *token = (char *)malloc(length + 1);  // +1 for null terminator
    if (!token)
        return NULL;
    strncpy(token, start, length);
    token[length] = '\0';
    return token;
}

// 문자열을 delimiter로 나누어 배열에 저장
char **ft_split(const char *str, char delimiter)
{
    if (!str)
        return NULL;

    int token_count = count_tokens(str, delimiter);
    char **result = (char **)malloc((token_count + 1) * sizeof(char *));  // +1 for NULL terminator
    if (!result)
        return NULL;

    int i = 0;
    while (*str)
    {
        if (*str != delimiter)
        {
            const char *start = str;
            size_t length = 0;

            // 토큰의 길이 측정
            while (*str && *str != delimiter)
            {
                length++;
                str++;
            }

            result[i] = copy_token(start, length);
            if (!result[i])  // 메모리 할당 실패 시 해제
            {
                free_split(result);
                return NULL;
            }
            i++;
        }
        else
        {
            str++;
        }
    }
    result[i] = NULL;  // 배열 마지막에 NULL 추가
    return result;
}

// 동적 할당된 문자열 배열을 해제하는 함수
void free_split(char **split)
{
    if (!split)
        return;

    for (int i = 0; split[i]; i++)
    {
        free(split[i]);
    }
    free(split);
}

// 새로운 옵션 노드 생성 함수
t_option *new_option(char *data)
{
    t_option *option = (t_option *)malloc(sizeof(t_option));
    if (!option)
        return NULL;
    option->data = strdup(data);
    option->next = NULL;
    return option;
}

// 새 인자 노드 생성 함수
t_arg *new_arg(char *filename, char *variable, char *arg, int quoto)
{
    t_arg *argument = (t_arg *)malloc(sizeof(t_arg));
    if (!argument)
        return NULL;
    argument->filename = filename ? strdup(filename) : NULL;
    argument->variable = variable ? strdup(variable) : NULL;
    argument->arg = arg ? strdup(arg) : NULL;
    argument->quoto = quoto;
    argument->option = NULL;
    return argument;
}

// 새 명령어 노드 생성 함수
t_pipe *new_command()
{
    t_pipe *command = (t_pipe *)malloc(sizeof(t_pipe));
    if (!command)
        return NULL;
    command->cmd = (t_cmd *)malloc(sizeof(t_cmd));
    command->cmd->redirections = NULL;
    command->cmd->simple_cmd = NULL;
    command->next = NULL;
    return command;
}

// 리다이렉션 추가 함수
void add_redirection(t_cmd *command, char *redirect, t_arg *arg)
{
    t_rdts *new_redirection = (t_rdts *)malloc(sizeof(t_rdts));
    new_redirection->redirect = strdup(redirect);
    new_redirection->arg = arg;
    new_redirection->next = NULL;

    if (!command->redirections)
    {
        command->redirections = new_redirection;
    }
    else
    {
        t_rdts *temp = command->redirections;
        while (temp->next)
            temp = temp->next;
        temp->next = new_redirection;
    }
}

// 간단한 명령어 추가 함수
void add_simple_command(t_cmd *command, char *basic_command, t_arg *arg)
{
    t_s_cmd *new_simple_cmd = (t_s_cmd *)malloc(sizeof(t_s_cmd));
    new_simple_cmd->basic_command = strdup(basic_command);
    new_simple_cmd->option = arg;

    command->simple_cmd = new_simple_cmd;
}

// 명령어를 명령어 리스트에 추가
void add_command(t_pipe **command_list, t_pipe *command)
{
    if (!*command_list)
    {
        *command_list = command;
    }
    else
    {
        t_pipe *temp = *command_list;
        while (temp->next)
            temp = temp->next;
        temp->next = command;
    }
}

// 옵션을 인자에 추가
void add_option(t_arg *arg, char *data)
{
    t_option *new_opt = new_option(data);
    if (!arg->option)
    {
        arg->option = new_opt;
    }
    else
    {
        t_option *temp = arg->option;
        while (temp->next)
            temp = temp->next;
        temp->next = new_opt;
    }
}
void parse_and_store(char *input, t_pipe **command_list)
{
    char **commands = ft_split(input, '|');  // | 기준으로 분리
    size_t i = 0;

    while (commands[i])
    {
        t_pipe *command = new_command();  // 새 명령어 노드 생성
        if (!command)  // 할당 실패 시 종료
            break;

        char **tokens = ft_split(commands[i], ' ');  // 공백 기준으로 분리
        size_t j = 0;
        while (tokens[j])
        {
            if (strcmp(tokens[j], ">") == 0 || strcmp(tokens[j], "<") == 0 ||
                strcmp(tokens[j], ">>") == 0 || strcmp(tokens[j], "<<") == 0)
            {
                if (tokens[j + 1])  // 리다이렉션 뒤에 오는 인자가 존재할 때만 추가
                {
                    t_arg *arg = new_arg(tokens[j + 1], NULL, NULL);
                    add_redirection(command->cmd, tokens[j], arg);
                    j++;  // 다음 인자는 이미 사용했으므로 건너뜀
                }
            }
            else
            {
                // 기본 명령어 또는 옵션 추가
                if (j == 0)
                {
                    t_arg *arg = new_arg(NULL, NULL, NULL);
                    add_simple_command(command->cmd, tokens[j], arg);
                    printf("Debug: Added basic command '%s'\n", tokens[j]);  // 디버깅 출력
                }
                else
                {
                    add_option(command->cmd->simple_cmd->option, tokens[j]);
                    printf("Debug: Added option '%s' to command '%s'\n", tokens[j], tokens[0]);  // 디버깅 출력
                }
            }
            j++;
        }

        add_command(command_list, command);  // 명령어 리스트에 추가
        ft_free(tokens);  // 토큰 메모리 해제
        i++;
    }
    ft_free(commands);  // 명령어 메모리 해제
}

// 명령어 리스트와 토큰들을 출력하는 함수
void print_commands(t_pipe *command_list)
{
    int cmd_num = 0;
    while (command_list)
    {
        printf("Command %d:\n", cmd_num++);
        
        t_s_cmd *simple_cmd = command_list->cmd->simple_cmd;
        printf("  Basic Command and Options:\n");
        printf("    %s\n", simple_cmd->basic_command);

        t_option *opt = simple_cmd->option->option;
        while (opt)
        {
            printf("      Option: %s\n", opt->data);
            opt = opt->next;
        }

        t_rdts *redirection = command_list->cmd->redirections;
        printf("  Redirections:\n");
        while (redirection)
        {
            printf("    Redirect: %s %s\n", redirection->redirect, redirection->arg->filename);
            redirection = redirection->next;
        }
        
        command_list = command_list->next;
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("사용법: %s \"명령어\"\n", argv[0]);
        return 1;
    }

    t_pipe *command_list = NULL;
    parse_and_store(argv[1], &command_list);  // 입력 문자열을 파싱하여 저장

    print_commands(command_list);  // 결과 출력

    // 메모리 해제 필요
    return 0;
}
