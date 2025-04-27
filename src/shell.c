#define _POSIX_C_SOURCE 1
#include "../include/shell.h"
#include "../include/msgs.h"
#include <ctype.h>
#include <math.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static char history[HISTORY][INPUT_SIZE];
static int num_command = 0;

void int_to_str(int N, char *str) {
  int i = 0;
  while (N > 0) {
    str[i++] = N % 10 + '0';
    N /= 10;
  }

  str[i] = '\0';

  for (int j = 0, k = i - 1; j < k; j++, k--) {
    char temp = str[j];
    str[j] = str[k];
    str[k] = temp;
  }
}

int mod(int a, int b) {
  int c = a % b;
  return c < 0 ? c + b : c;
}

void add_history(char *command) { // char *args[]) {
  int index = mod(num_command, HISTORY);

  char buffer[3 + INPUT_SIZE + 3];
  snprintf(buffer, 3 + INPUT_SIZE, "%d\t%s", num_command, command);
  // int_to_str(num_command, buffer);

  // strcat(buffer, "\t");
  // strcat(buffer, command);

  strncpy(history[index], buffer, INPUT_SIZE - 1);
  history[index][INPUT_SIZE - 1] = '\0';
  num_command++;
}

int print_prompt() {

  char pwd[INPUT_SIZE];

  if (getcwd(pwd, sizeof(pwd)) != NULL) {
    write(STDOUT_FILENO, pwd, strlen(pwd));
    write(STDOUT_FILENO, "$ ", strlen("$ "));
    fflush(stdout);
    return 0;

  } else {
    char *no_pwd = FORMAT_MSG("shell", GETCWD_ERROR_MSG);
    write(STDERR_FILENO, no_pwd, strlen(no_pwd));
    return -1;
  }
}

int get_input(char *input) {
  ssize_t inputread = read(STDIN_FILENO, input, INPUT_SIZE);
  if (inputread == -1) {
    char *msg = FORMAT_MSG("shell", READ_ERROR_MSG);
    write(STDERR_FILENO, msg, strlen(msg));
    return -1;
  }

  if (inputread > 0) {
    input[inputread - 1] = '\0';
  }
  if (input[0] == '!') {
    return execute_history(input);
  }
  add_history(input);
  return 0;
}

int get_exit(int argc, char *args[]) {
  if (argc > 1) {
    char *msg = FORMAT_MSG("exit", TMA_MSG);
    write(STDERR_FILENO, msg, strlen(msg));
    return -1;
  }
  exit(0);
}

int get_pwd(int argc, char *args[]) {
  if (argc > 1) {
    char *msg = FORMAT_MSG("pwd", TMA_MSG);
    write(STDERR_FILENO, msg, strlen(msg));
    return -1;
  } else {
    char pwd[INPUT_SIZE];
    if (getcwd(pwd, sizeof(pwd)) != NULL) {
      write(STDOUT_FILENO, pwd, strlen(pwd));
      write(STDOUT_FILENO, "\n", strlen("\n"));
      return 0;
    } else {
      char *msg = FORMAT_MSG("shell", GETCWD_ERROR_MSG);
      write(STDERR_FILENO, msg, strlen(msg));
      return -1;
    }
  }

  return 0;
}

char previous_dir[INPUT_SIZE];

int get_cd(int argc, char *args[]) {
  char pathpre[INPUT_SIZE];
  if (argc > 2) {
    char *msg = FORMAT_MSG("cd", TMA_MSG);
    write(STDERR_FILENO, msg, strlen(msg));
    return -1;
  }

  char *path = args[1];

  if ((argc == 1) || (args[1][0] == '~')) {
    path = getpwuid(getuid())->pw_dir;
    if ((argc != 1) && (args[1][1] != '\0')) {
      snprintf(pathpre, sizeof(pathpre), "%s%s", path, args[1] + 1);
      path = pathpre;
    }
  } else if (strcmp(path, "-") == 0) {
    if (previous_dir[0] == '\0') {
      // CHECK THIS TIME BEFORE SUBMITTING
      char *msg = FORMAT_MSG("cd", CHDIR_ERROR_MSG);
      write(STDERR_FILENO, msg, strlen(msg));
      return -1;
    }
    char temp[INPUT_SIZE];
    strcpy(temp, previous_dir);
    path = temp;
  }

  char current[INPUT_SIZE];

  if (getcwd(current, sizeof(current)) != NULL) {
    strcpy(previous_dir, current);
  } else {
    write(STDERR_FILENO, "getcwd: failed\n", strlen("getcwd: failed\n"));
  }

  if (chdir(path) == -1) {
    char *msg = FORMAT_MSG("cd", CHDIR_ERROR_MSG);
    write(STDERR_FILENO, msg, strlen(msg));
    return -1;
  }

  return 0;
}

int get_help(int argc, char *args[]) {
  char *c_exit = FORMAT_MSG("exit", EXIT_HELP_MSG);
  char *c_pwd = FORMAT_MSG("pwd", PWD_HELP_MSG);
  char *c_cd = FORMAT_MSG("cd", CD_HELP_MSG);
  char *c_help = FORMAT_MSG("help", HELP_HELP_MSG);
  char *c_history = FORMAT_MSG("history", HISTORY_HELP_MSG);
  if (argc == 1) {
    write(STDOUT_FILENO, c_exit, strlen(c_exit));
    write(STDOUT_FILENO, c_pwd, strlen(c_pwd));
    write(STDOUT_FILENO, c_cd, strlen(c_cd));
    write(STDOUT_FILENO, c_help, strlen(c_help));
    write(STDOUT_FILENO, c_history, strlen(c_history));
    return 0;
  } else if (strcmp(args[1], "exit") == 0) {
    write(STDOUT_FILENO, c_exit, strlen(c_exit));
  } else if (strcmp(args[1], "pwd") == 0) {
    write(STDOUT_FILENO, c_pwd, strlen(c_pwd));
  } else if (strcmp(args[1], "cd") == 0) {
    write(STDOUT_FILENO, c_cd, strlen(c_cd));
  } else if (strcmp(args[1], "help") == 0) {
    write(STDOUT_FILENO, c_help, strlen(c_help));
  } else if (strcmp(args[1], "history") == 0) {
    write(STDOUT_FILENO, c_history, strlen(c_history));
  } else {
    {
      write(STDOUT_FILENO, args[1], strlen(args[1]));

      write(STDOUT_FILENO, ": external command or application\n",
            strlen(": external command or application\n"));
    }
  }
  return 0;
}

void sigint_handler(int sig) {

  (void)sig;
  char *help[1] = {"help"};
  // write(STDOUT_FILENO, "\nHELP MENU:\n", strlen("\nHELP MENU:\n"));
  get_help(1, help);
  //  print_prompt();
}

void print_history() {
  // int index = mod(num_command, HISTORY) -1;
  // printf("num_commands: %d\nstarting index: %d\n", num_command, index);
  for (int i = num_command - 1; i >= num_command - 10; i--) {
    int index = mod(i, HISTORY);
    if (history[index][0] != '\0') {
      write(STDOUT_FILENO, history[index], strlen(history[index]));
      write(STDOUT_FILENO, "\n", strlen("\n"));
    }
  }
}
int execute(char *command) {
  // argument
  char *args[INPUT_SIZE];
  int background = 0;
  char *arg;
  char *saveptr = NULL;
  int num = 0;

  arg = strtok_r(command, " ", &saveptr);

  while (arg != NULL) {
    if (strcmp(arg, "&") == 0) {
      background = 1;
    } else {
      args[num] = arg;

      num++;
    }
    arg = strtok_r(NULL, " ", &saveptr);
  }

  args[num] = NULL;
  if (args[0] == NULL) {
    return 0;
  }

  //  add_history(num, args);

  if (args[num - 1][strlen(args[num - 1]) - 1] == '&') {
    //     if (strcmp(args[num - 1], "&") == 0) {
    //     args[num - 1] = NULL;
    //}
    background = 1;
    args[num - 1][strlen(args[num - 1]) - 1] = '\0';
  }

  if (strcmp(args[0], "pwd") == 0) {
    get_pwd(num, args);
    return 0;
  }

  if (strcmp(args[0], "exit") == 0) {
    get_exit(num, args);
    return 0;
  }

  if (strcmp(args[0], "cd") == 0) {
    get_cd(num, args);
    return 0;
  }

  if (strcmp(args[0], "help") == 0) {
    get_help(num, args);
    return 0;
  }

  if (strcmp(args[0], "history") == 0) {
    if (num > 1) {
      char *msg = FORMAT_MSG("history", TMA_MSG);
      write(STDERR_FILENO, msg, strlen(msg));
      return -1;
    }
    print_history();
    return 0;
  }

  pid_t pid = fork();

  // if fork failed
  if (pid == -1) {
    char *msg = FORMAT_MSG("shell", FORK_ERROR_MSG);
    write(STDOUT_FILENO, msg, strlen(msg));
    return 0;
    // fail
  } else if (pid > 0) {

    if (background == 0) {
      if (waitpid(pid, NULL, 0) == -1) {
        char *msg = FORMAT_MSG("shell", WAIT_ERROR_MSG);
        write(STDERR_FILENO, msg, strlen(msg));
      }
    } else {
      // This still needs confirmation with the ussuage of sleep command. It
      // is used right now to make the output looks presentable
      sleep(1);
    }

    // parent
  } else {

    if (execvp(args[0], args) == -1) {
      char *msg = FORMAT_MSG("shell", EXEC_ERROR_MSG);
      write(STDERR_FILENO, msg, strlen(msg));
      _exit(EXIT_FAILURE);
      // child
    }
  }
  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;

  return 0;
}

int execute_history(char *command) {
  int i = 0;
  int index;
  char *read;
  char *saveptr = NULL;
  if (strcmp(command, "!!") == 0) {
    index = mod(num_command - 1, HISTORY);
    if (num_command == 0) {
      char *msg = FORMAT_MSG("history", HISTORY_NO_LAST_MSG);
      write(STDERR_FILENO, msg, strlen(msg));
      return -1;
    }
  } else {
    if (command[1] == '\0') {
      return -1;
    }
    read = strtok_r(command, "!", &saveptr);
    if (strcmp(read, "0") == 0) {
      i = 1;
    }
    index = atoi(read);
    if ((index == 0) || (index > num_command)) {
      if (i == 0) {
        char *msg = FORMAT_MSG("history", HISTORY_INVALID_MSG);
        write(STDERR_FILENO, msg, strlen(msg));
        return -1;
      }
    }

    index = mod(index, HISTORY);
  }
  saveptr = NULL;
  char store[INPUT_SIZE];
  strcpy(store, history[index]);
  read = strchr(store, '\t');
  if (!read || *(read + 1) == '\0') {
    return -1;
  }
  write(STDOUT_FILENO, (read + 1), strlen(read + 1));
  write(STDOUT_FILENO, "\n", strlen("\n"));
  execute(read + 1);

  return -1;
}
