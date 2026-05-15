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
	struct ifreq ifr;			/* CAN interface info struct */
	struct sockaddr_can addr;	/* CAN adddress info struct */
	struct can_frame frame;		/* CAN frame struct */
	int s;						/* SocketCAN handle */

	memset(&ifr, 0, sizeof(ifr));
	memset(&addr, 0, sizeof(addr));
	memset(&frame, 0, sizeof(frame));
	
	// TODO: Open a socket here
	
	s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (s < 0) {
		perror("Socket opening failed");
		return 1;
	}
	
	/* Convert interface string "can0" to index */
	strcpy(ifr.ifr_name, "can0");
	ioctl(s, SIOCGIFINDEX, &ifr);
	
	/* Setup address for binding */
	addr.can_ifindex = ifr.ifr_ifindex;
	addr.can_family = AF_CAN;
	
	/* Disable reception filter on this RAW socket */
	setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);
	
	// TODO: Bind socket to can0 interface
	
	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("Bind failed");
		return 1;
	}
	
	// TODO: Fill frame info and send
	
	frame.can_id = 0x34;
	frame.can_dlc = 5;
	memcpy(frame.data, "hello", 5);
	write(s, &frame, sizeof(struct can_frame));
	
	frame.can_id = 0x123456 | CAN_EFF_FLAG;
	frame.can_dlc = 5;
	memcpy(frame.data, "world", 5);
	write(s, &frame, sizeof(struct can_frame));
	
	close(s);
	
	return 0;
}
