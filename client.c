#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include "inc/ft_printf.h"

int	ft_atoi(const char *str)
{
	int	i = 0;
	int	signe = 1;
	int	res = 0;

	while ((str[i] == ' ' || (str[i] >= 9 && str[i] <= 13)))
		i++;
	if (str[i] == '-' || str[i] == '+')
		if (str[i++] == '-')
			signe = -1;
	while (str[i] >= '0' && str[i] <= '9')
		res = res * 10 + (str[i++] - '0');
	return (res * signe);
}

void	send_char(pid_t pid, unsigned char c)
{
	for (int i = 0; i < 8; i++)
	{
		if ((c >> i) & 1)
			kill(pid, SIGUSR2);
		else
			kill(pid, SIGUSR1);
		usleep(100);
	}
}

int	main(int argc, char **argv)
{
	if (argc != 3)
	{
		ft_printf("Usage: %s <PID> <message>\n", argv[0]);
		return (1);
	}
	pid_t	pid = (pid_t)ft_atoi(argv[1]);
	char	*msg = argv[2];
	while (*msg)
		send_char(pid, *msg++);
	send_char(pid, '\0'); // Fin de message
	return (0);
}
