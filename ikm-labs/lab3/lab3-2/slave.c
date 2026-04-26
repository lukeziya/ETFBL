#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include <wiringPi.h>

int main ()
{
	int fd;
	char sbuf;
	struct termios config;
	struct termios savedconfig;
	

	if (wiringPiSetup () == -1)
	{
		fprintf (stdout, "Unable to start wiringPi.\n") ;
		return -1;
	}

	// setup output mode for BCM22 (WiringPi 3) used to control communication direction
	// BCM22 (WiringPi 3) = HIGH: RS-485 transmit mode
	// BCM22 (WiringPi 3) = LOW: RS-485 receive mode
	pinMode(3, OUTPUT);
	  
	// set it initially to be in receive mode
	digitalWrite(3, LOW);

	// TODO: open and configure serial port with 9600 baudrate
	fd = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY); // O_NDELAY mora biti iskljucen ako hocemo da se 						slikaju karakteri bez entera.
		if (fd < 0)
			fprintf(stderr, "Failed to open serial port. Check if it is used by another device.\n");
			
	tcgetattr(0, &savedconfig);
	tcgetattr(0, &config);
	config.c_cc[VMIN] = 1; // at least 1 char threshold
	config.c_cc[VTIME] = 0; //no delay
	cfsetispeed(&config, B9600);
	cfsetospeed(&config, B9600);
	tcsetattr(0, TCSANOW, &config);
	

	while (1)
	{
		// TODO: read the received byte and send it back via RS-485 network
		if (read(fd, &sbuf, 1) > 0)
				{
					putchar(sbuf);
					fflush(stdout);
				}
		digitalWrite(3, HIGH);
		write(fd, &sbuf, 1);
		tcdrain(fd);
		digitalWrite(3, LOW);
		
	}
	tcsetattr(0, TCSANOW, &savedconfig);
	close(fd);	// close the serial connection
	return 0;
}
