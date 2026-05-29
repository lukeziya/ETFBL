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
	
	/* TODO: Open a socket here */
	s = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (s < 0) {
		perror("socket");
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
	
	/* TODO: Bind socket to can0 interface */
	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		close(s);
		return 1;
	}

	/* TODO: Fill frame info and send */

	/* --- Frame 1: ID=0x034, data="hello" (5 bytes) --- */
	frame.can_id  = 0x034;
	frame.can_dlc = 5;
	frame.data[0] = 'h';
	frame.data[1] = 'e';
	frame.data[2] = 'l';
	frame.data[3] = 'l';
	frame.data[4] = 'o';

	if (write(s, &frame, sizeof(frame)) != sizeof(frame)) {
		perror("write frame 1");
		close(s);
		return 1;
	}
	printf("Sent frame 1: ID=0x%03X DLC=%d data=\"hello\"\n",
	       frame.can_id, frame.can_dlc);

	/* --- Frame 2: ID=0x123456 (extended 29-bit), data="world" (5 bytes) --- */
	memset(&frame, 0, sizeof(frame));
	frame.can_id  = 0x123456 | CAN_EFF_FLAG;  /* set EFF flag for 29-bit ID */
	frame.can_dlc = 5;
	frame.data[0] = 'w';
	frame.data[1] = 'o';
	frame.data[2] = 'r';
	frame.data[3] = 'l';
	frame.data[4] = 'd';

	if (write(s, &frame, sizeof(frame)) != sizeof(frame)) {
		perror("write frame 2");
		close(s);
		return 1;
	}
	printf("Sent frame 2: ID=0x%08X DLC=%d data=\"world\"\n",
	       frame.can_id & CAN_EFF_MASK, frame.can_dlc);

	close(s);
	
	return 0;
}
