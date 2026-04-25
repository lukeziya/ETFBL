#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/select.h>

int main ()
{
	int fd;						// serial port file descriptor
	struct termios config, old_stdin_config, target_stdin_config;		// configuration termios structure
	unsigned char *sbuf = "Hello Serial!\n";	// send buffer initialized with some data
	int sbuf_len = strlen(sbuf);		// send buffer length
	unsigned char rbuf;			// receive buffer
	
	// TODO: open serial port and check for errors while opening
	fd = open("dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd < 0)
		fprintf(stderr, "Failed to open serial port. Check if it is used by another device.\n");
		
	// get attributes
	if (tcgetattr(fd, &config) < 0)
	{
		fprintf(stdout, "Unable to get configuration.\n");
		return -1;
	}
	
	// TODO: make raw config
	cfmakeraw(&config);

	// TODO: set the speed to 19200 baudrate
	cfsetispeed(&config, B19200);
	cfsetospeed(&config, B19200);
	
	// min one character, timeout disabled
	config.c_cc[VMIN] = 1;
	config.c_cc[VTIME] = 0;

	// apply the config immediately
	if (tcsetattr(fd, TCSANOW, &config) < 0)
	{
		fprintf(stdout, "Unable to set the configuration.\n");
		return -1;
	}
	
	tcgetattr(STDIN_FILENO, &old_stdin_config);
	target_stdin_config = old_stdin_config;
	cfmakeraw(&target_stdin_config);
	tcsetattr(STDIN_FILENO, TCSANOW, &target_stdin_config);
	
	printf("Slanje pokrenuto. Pritisni 'q' za prekid...\r\n");
	
	// loop infinitely until q(uit) character is received
	while (1)
	{
	
		struct timeval tv = {0, 0};
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(STDIN_FILENO, &readfds);
		
		if (select(STDIN_FILENO + 1, &readfds, NULL, NULL, &tv) > 0)
			{
			char ch;
			if (read(STDIN_FILENO, &ch, 1) > 0 && ch == 'q')
				{
				printf("\r\nDetektovano 'q'. Prekidam...\r\n");
				break;
				}
			}
		// read data from serial port
		if (read(fd, &rbuf, 1) > 0)
		{
			// check for quit condition to break the infinite loop
			if (rbuf == 'q')
				break;
			// else print received character to the console
			else
			{
				putchar(rbuf);
				fflush(stdout);
			}
		}
		
		// send some data
		if (*sbuf != '\0')
				write(fd, sbuf++, 1);
		else
			sbuf -= sbuf_len;
		
		// wait 100ms to let data be processed
		usleep(100000);
	}

	// close the serial port
	tcsetattr(STDIN_FILENO, TCSANOW, &old_stdin_config);
	close(fd);
	return 0;
}
