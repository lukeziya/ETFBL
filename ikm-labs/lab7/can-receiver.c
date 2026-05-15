#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>

#include <linux/can.h>
#include <linux/can/raw.h>

int main()
{
	struct ifreq ifr;				/* CAN interface info struct */
	struct sockaddr_can addr;		/* CAN adddress info struct */
	struct can_frame frame;			/* CAN frame struct */
	struct can_filter rfilter[2];	/* CAN reception filter */
	int s;
	int nbytes;							/* SocketCAN handle */

	memset(&ifr, 0, sizeof(ifr));
	memset(&addr, 0, sizeof(addr));
	memset(&frame, 0, sizeof(frame));
	
	// TODO: Open a socket here
	
	s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (s < 0){
		perror("Socket opening failed");
		return 1;
	}
	
	
	/* Convert interface string "can0" to index */
	strcpy(ifr.ifr_name, "can0");
	ioctl(s, SIOCGIFINDEX, &ifr);
	
	/* Setup address for binding */
	addr.can_ifindex = ifr.ifr_ifindex;
	addr.can_family = AF_CAN;
	
	// TODO: Set the reception filter on this RAW socket
	
	rfilter[0].can_id = 0x34;
	rfilter[0].can_mask = CAN_SFF_MASK;
	
	rfilter[1].can_id = 0x123456 | CAN_EFF_FLAG;
	rfilter[1].can_mask = (CAN_SFF_MASK | CAN_EFF_FLAG);
	
	setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));
	
	
	
	
	// TODO: Bind socket to can0 interface
	
	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("Bind failed");
		return 1;
	}
		
	while(1)
	{
		// TODO: Read received frame and print on console
		
		nbytes = read(s, &frame, sizeof(struct can_frame));
		
		if (nbytes < 0) {
			perror("Read failed");
			return 1;
		}
		
		if (nbytes < sizeof(struct can_frame)) {
			fprintf(stderr, "Read : incomplite CAN frame\n");
			continue;
		}
		
		printf(" can0 	%03X	[%d] ", frame.can_id & CAN_EFF_MASK, frame.can_dlc);
		int i;
		for (i = 0; i < frame.can_dlc; i++) {
			printf("%02X ", frame.data[i]);
		}
		
		printf(" '%s'\n", frame.data);
		
	}
	
	close(s);
	
	return 0;
}
