#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include "inc/ft_printf.h"

static void	handle_signal(int sig, siginfo_t *info, void *context)
{
	static int	bit = 0;
	static unsigned char c = 0;
	static pid_t	client_pid = 0;

	(void)context;
	if (client_pid != info->si_pid)
	{
		bit = 0;
		c = 0;
		client_pid = info->si_pid;
	}

	c |= (sig == SIGUSR2) << bit;
	bit++;
	if (bit == 8)
	{
		ft_printf("%c", c);
		bit = 0;
		c = 0;
	}
}

int	main(void)
{
	struct sigaction	sa;

	ft_printf("Server PID: %d\n", getpid());
	sa.sa_sigaction = handle_signal;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;
	sigaction(SIGUSR1, &sa, NULL);
	sigaction(SIGUSR2, &sa, NULL);
	while (1)
		pause();
}
