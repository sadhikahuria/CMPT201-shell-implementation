#ifndef SHELL_H
#define SHELL_H

#define INPUT_SIZE 300
#define HISTORY 10

/*struct history_set {
  int pos;
  char *command;
};
*/
void int_to_str(int N, char *str);

int execute_history(char *argv);

void add_history(char *command);

int print_prompt();

int get_input(char *input);

int get_exit(int argc, char *args[]);
int get_pwd(int argc, char *args[]);
int get_cd(int argc, char *args[]);
int get_help(int argc, char *args[]);

void sigint_handler(int sig);

void print_history();

int execute(char *command);

int execute_history(char *command);
#endif
