#include "minishell.h"

void ft_free(char **tokens)
{
    if (!tokens)
        return;

    size_t i = 0;
    while (tokens[i])
    {
        free(tokens[i]);
        i++;
    }
    free(tokens);
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
t_arg *new_arg(char *filename, char *variable, char *arg)
{
    t_arg *argument = (t_arg *)malloc(sizeof(t_arg));
    if (!argument)
        return NULL;
    argument->filename = filename ? strdup(filename) : NULL;
    argument->variable = variable ? strdup(variable) : NULL;
    argument->arg = arg ? strdup(arg) : NULL;
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
    if (!command->cmd)
    {
        free(command);
        return NULL;
    }
    
    command->cmd->redirections = NULL;
    command->cmd->simple_cmd = NULL;
    command->next = NULL;
    return command;
}

// 리다이렉션 추가 함수
void add_redirection(t_cmd *command, char *redirect, t_arg *arg)
{
    t_rdts *new_redirection = (t_rdts *)malloc(sizeof(t_rdts));
    if (!new_redirection)
        return;
    
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
    if (!new_simple_cmd)
        return;
    
    new_simple_cmd->basic_command = strdup(basic_command);
    new_simple_cmd->arg = arg;

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

// 가정: `new_option` 함수는 주어진 `data`로 t_option 구조체를 생성합니다.

void add_option(t_arg *arg, char *data)
{
    t_option *new_opt = new_option(data);
    if (!new_opt)
        return;

    // Check if data starts with `$` (variable)
    if (ft_strncmp(data, "$", 1) == 0)
    {
        arg->variable = ft_strdup(data);
    }
    // Check if data is enclosed in single or double quotes (argument)
    else if ((data[0] == '\'' && data[ft_strlen(data) - 1] == '\'') || 
             (data[0] == '\"' && data[ft_strlen(data) - 1] == '\"'))
    {
        arg->arg = ft_strdup(data);
    }
    // Check if data ends with ".txt" (filename)
    else if (strlen(data) >= 4 && ft_strncmp(data + ft_strlen(data) - 4, ".txt", 4) == 0)
    {
        arg->filename = ft_strdup(data);
    }
    else  // If none of the above, treat it as a regular option
    {
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
            // 리다이렉션 기호를 확인
            if ((ft_strncmp(tokens[j], ">", 1) == 0) ||
                (ft_strncmp(tokens[j], "<", 1) == 0) ||
                (ft_strncmp(tokens[j], ">>", 2) == 0) ||
                (ft_strncmp(tokens[j], "<<", 2) == 0))
            {
                // 리다이렉션 기호 다음에 파일명이 있는지 확인
                if (tokens[j + 1])
                {
                    // 리다이렉션 기호는 `redirect` 필드에, 파일명은 `arg` 필드에 저장
                    t_arg *arg = new_arg(tokens[j + 1], NULL, NULL);  // 파일명 저장
                    add_redirection(command->cmd, tokens[j], arg);     // 리다이렉션 기호 저장
                    j++;  // 파일명까지 사용했으므로 인덱스 증가
                }
            }
            else
            {
                // 기본 명령어 또는 옵션 추가
                if (j == 0)
                {
                    t_arg *arg = new_arg(NULL, NULL, NULL);
                    add_simple_command(command->cmd, tokens[j], arg);
                }
                else
                {
                    add_option(command->cmd->simple_cmd->arg, tokens[j]);
                }
            }
            j++;
        }

        // 명령어 구간이 끝났으므로 t_pipe의 index와 pipe 설정
        command->index = i;  // 현재 명령어 인덱스를 설정
        command->pipe = '|'; // `|`를 pipe 필드에 저장

        add_command(command_list, command);  // 명령어 리스트에 추가
        ft_free(tokens);  // 토큰 메모리 해제
        i++;
    }
    ft_free(commands);  // 명령어 메모리 해제
}

void print_commands(t_pipe *command_list)
{
    int cmd_num = 0;
    int pipe_num = 0;

    while (command_list)
    {
        // Command 출력
        printf("Command %d:\n", cmd_num++);
        
        t_s_cmd *simple_cmd = command_list->cmd->simple_cmd;
        if (simple_cmd)
        {
            printf("  Basic Command and Options:\n");
            printf("    %s\n", simple_cmd->basic_command);

            // t_arg 구조체의 각 필드를 구분하여 출력
            if (simple_cmd->arg)
            {
                printf("    Args:\n");

                // Filename
                if (simple_cmd->arg->filename)
                    printf("          Filename: %s\n", simple_cmd->arg->filename);

                // Option List
                t_option *opt = simple_cmd->arg->option;
                while (opt)
                {
                    printf("          Option: %s\n", opt->data);
                    opt = opt->next;
                }

                // Variable
                if (simple_cmd->arg->variable)
                    printf("          Variable: %s\n", simple_cmd->arg->variable);

                // Argument
                if (simple_cmd->arg->arg)
                    printf("          Arg: %s\n", simple_cmd->arg->arg);

                // Quote Flag (Placeholder for T_QUOTO_FLG type)
                if (simple_cmd->arg->quoto)
                    printf("          Quoto Flag: (display logic needed)\n");
            }
        }

        // 리다이렉션 출력
        t_rdts *redirection = command_list->cmd->redirections;
        if (redirection)
        {
            printf("  Redirections:\n");
            while (redirection)
            {
                printf("    Redirect: %s\n", redirection->redirect);

                if (redirection->arg)
                {
                    printf("    Args:\n");

                    // Filename
                    if (redirection->arg->filename)
                        printf("          Filename: %s\n", redirection->arg->filename);

                    // Option List
                    t_option *opt = redirection->arg->option;
                    while (opt)
                    {
                        printf("          Option: %s\n", opt->data);
                        opt = opt->next;
                    }

                    // Variable
                    if (redirection->arg->variable)
                        printf("          Variable: %s\n", redirection->arg->variable);

                    // Argument
                    if (redirection->arg->arg)
                        printf("          Arg: %s\n", redirection->arg->arg);

                    // Quote Flag (Placeholder for T_QUOTO_FLG type)
                    if (redirection->arg->quoto)
                        printf("          Quoto Flag: (display logic needed)\n");
                }
                redirection = redirection->next;
            }
        }

        // 다음 명령어가 있을 때만 Pipe 출력
        if (command_list->next && command_list->pipe == '|')
        {
            printf("\nPipe %d:\n", pipe_num++);
            printf("                %c\n", command_list->pipe);
        }

        command_list = command_list->next;

        // Command 간 줄바꿈
        if (command_list) {
            printf("\n");
        }
    }
}

void free_commands(t_pipe *command_list)
{
    while (command_list)
    {
        t_pipe *temp_pipe = command_list;
        t_cmd *cmd = temp_pipe->cmd;

        if (cmd)
        {
            t_s_cmd *simple_cmd = cmd->simple_cmd;
            if (simple_cmd)
            {
                // Free the basic command
                free(simple_cmd->basic_command);

                // Free each field in t_arg
                if (simple_cmd->arg)
                {
                    if (simple_cmd->arg->filename)
                        free(simple_cmd->arg->filename);
                    if (simple_cmd->arg->variable)
                        free(simple_cmd->arg->variable);
                    if (simple_cmd->arg->arg)
                        free(simple_cmd->arg->arg);
                    
                    // Free options list in t_arg
                    t_option *opt = simple_cmd->arg->option;
                    while (opt)
                    {
                        t_option *temp_opt = opt;
                        opt = opt->next;
                        free(temp_opt->data);  // Free each option's data
                        free(temp_opt);
                    }

                    // Free the t_arg itself
                    free(simple_cmd->arg);
                }
                
                // Free the simple command structure
                free(simple_cmd);
            }

            // Free each redirection in the command
            t_rdts *redir = cmd->redirections;
            while (redir)
            {
                t_rdts *temp_redir = redir;
                redir = redir->next;
                
                // Free redirection fields
                free(temp_redir->redirect);
                if (temp_redir->arg)
                {
                    if (temp_redir->arg->filename)
                        free(temp_redir->arg->filename);
                    if (temp_redir->arg->variable)
                        free(temp_redir->arg->variable);
                    if (temp_redir->arg->arg)
                        free(temp_redir->arg->arg);
                    
                    // Free options list in redirection's t_arg
                    t_option *opt = temp_redir->arg->option;
                    while (opt)
                    {
                        t_option *temp_opt = opt;
                        opt = opt->next;
                        free(temp_opt->data);
                        free(temp_opt);
                    }

                    // Free the t_arg structure itself
                    free(temp_redir->arg);
                }
                
                // Free the redirection structure
                free(temp_redir);
            }

            // Free the command structure
            free(cmd);
        }

        // Move to next command in the pipe list
        command_list = command_list->next;

        // Free the pipe structure
        free(temp_pipe);
    }
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Usage: %s \"command\"\n", argv[0]);
        return 1;
    }

    t_pipe *command_list = NULL;
    parse_and_store(argv[1], &command_list);  // 입력 문자열을 파싱하여 저장

    print_commands(command_list);  // 결과 출력
    free_commands(command_list);  // 메모리 해제

    return 0;
}
