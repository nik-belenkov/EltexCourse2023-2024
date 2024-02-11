#include <signal.h>
#include <stdio.h>
/* Функция my_handler – пользовательский
   обработчик сигнала */
void my_handler(int nsig) {
  switch (nsig) {
    case SIGHUP:
      printf("Receive signal %d\n", nsig);
      break;
    case SIGINT:
      printf("Receive signal %d\n", nsig);
      break;
    case SIGQUIT:
      printf("Receive signal %d\n", nsig);
      break;
    case SIGILL:
      printf("Receive signal %d\n", nsig);
      break;
    case SIGTRAP:
      printf("Receive signal %d\n", nsig);
      break;
    case SIGABRT:
      printf("Receive signal %d\n", nsig);
      break;
    case SIGFPE:
      printf("Receive signal %d\n", nsig);
      break;
    case SIGSEGV:
      printf("Receive signal %d\n", nsig);
      break;
    case SIGTERM:
      printf("Receive signal %d\n", nsig);
      break;
    case SIGPIPE:
      printf("Receive signal %d\n", nsig);
      break;
    default:
      signal(nsig, SIG_DFL);
  }
}
int main(void) {
  (void)signal(SIGHUP, my_handler);
  (void)signal(SIGINT, my_handler);
  (void)signal(SIGQUIT, my_handler);
  (void)signal(SIGILL, my_handler);
  (void)signal(SIGTRAP, my_handler);
  (void)signal(SIGABRT, my_handler);
  (void)signal(SIGFPE, my_handler);
  (void)signal(SIGSEGV, my_handler);
  (void)signal(SIGTERM, my_handler);
  (void)signal(SIGPIPE, my_handler);
  while (1)
    ;
  return 0;
}
