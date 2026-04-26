#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include <wiringPi.h>

int main ()
{
	int fd;
	char sbuf;
	char rbuf;
	struct termios config;
	struct termios savedconfig;

	
	// start the wiringPi library
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

	fd = open("/dev/serial0", O_RDWR | O_NOCTTY ); // O_NDELAY mora biti iskljucen ako hocemo da se slikaju karakteri bez entera.
		if (fd < 0)
			fprintf(stderr, "Failed to open serial port. Check if it is used by another device.\n");
				
	tcgetattr(0, &config);
	tcgetattr(0, &savedconfig);
	cfmakeraw(&config);
	config.c_lflag &=  ~(ECHO | ICANON); // ovo treba ako se hoce kanonski mod iskljuciti.
	config.c_cc[VMIN] = 1; // at least 1 char threshold
	config.c_cc[VTIME] = 0; //no delay
	cfsetispeed(&config, B9600);
	cfsetospeed(&config, B9600);
	tcsetattr(0, TCSANOW, &config);
	
	while (1)
	{
		sbuf = getchar();
		if (sbuf == 'q')
		{ 
			tcsetattr(0, TCSANOW, &savedconfig);
			close(fd);
			return 0;
		}
		else
		{
			if (sbuf != '\n')
			{
				digitalWrite(3, HIGH);	// enable RS-485 transmit mode
				
				// TODO: send the entered byte
				write(fd, &sbuf, 1);
				tcdrain(0);
				digitalWrite(3, LOW);
				
				usleep(50000);
	
					
			}
		}
		// TODO: read the received byte from serial port and display it in console
		if (read(fd, &rbuf, 1) > 0)
				{
					printf("Primljeni char je: %c\n\r", rbuf);
				}
	}
	tcsetattr(0, TCSANOW, &savedconfig);
	close(fd);	
	return 0 ;
}
