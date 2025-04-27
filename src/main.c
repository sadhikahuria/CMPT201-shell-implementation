#define _POSIX_C_SOURCE 1
#include "../include/msgs.h"
#include "../include/shell.h"
#include <ctype.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  struct sigaction act;
  act.sa_handler = sigint_handler;
  act.sa_flags = 0;
  sigemptyset(&act.sa_mask);

  int ret = sigaction(SIGINT, &act, NULL);

  if (ret == -1) {
    write(STDERR_FILENO, "sigaction failed. existing\n",
          strlen("sigaction failed. existing\n"));
    exit(EXIT_FAILURE);
  }
  char input[INPUT_SIZE];
  while (1) {
    if (print_prompt() != -1) {
      if (get_input(input) != -1) {
        execute(input);
      }
    }
  }
}
