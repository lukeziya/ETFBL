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
	int s;							/* SocketCAN handle */
	int nbytes;
	int i;

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
	
	/* TODO: Set the reception filter on this RAW socket
	 *
	 * Prihvatamo samo okvire koje šalje naš can-sender:
	 *   - rfilter[0]: standardni okvir ID=0x034  (mask=CAN_SFF_MASK -> egzaktno poklapanje)
	 *   - rfilter[1]: prošireni okvir ID=0x123456 (mask=CAN_EFF_MASK -> egzaktno poklapanje,
	 *                 CAN_EFF_FLAG mora biti set da bismo izabrali 29-bitne okvire)
	 */
	rfilter[0].can_id   = 0x034;
	rfilter[0].can_mask = CAN_SFF_MASK;

	rfilter[1].can_id   = 0x123456 | CAN_EFF_FLAG;
	rfilter[1].can_mask = CAN_EFF_MASK | CAN_EFF_FLAG;

	setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));

	/* TODO: Bind socket to can0 interface */
	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		close(s);
		return 1;
	}

	printf("Listening on can0 (filter: 0x034 | 0x123456) ...\n");
		
	while(1)
	{
		/* TODO: Read received frame and print on console */
		nbytes = read(s, &frame, sizeof(frame));

		if (nbytes < 0) {
			perror("read");
			break;
		}

		if (nbytes < (int)sizeof(frame)) {
			fprintf(stderr, "read: incomplete CAN frame\n");
			continue;
		}

		/* Ispis slično candump-u:  can0  ID#DLC  [data bytes]  '(ASCII)' */
		if (frame.can_id & CAN_EFF_FLAG) {
			/* Prošireni (29-bitni) identifikator */
			printf("can0  %08X  [%d] ",
			       frame.can_id & CAN_EFF_MASK,
			       frame.can_dlc);
		} else if (frame.can_id & CAN_RTR_FLAG) {
			/* Remote Request okvir */
			printf("can0  %03X  R  [%d] ",
			       frame.can_id & CAN_SFF_MASK,
			       frame.can_dlc);
		} else {
			/* Standardni (11-bitni) identifikator */
			printf("can0  %03X  [%d] ",
			       frame.can_id & CAN_SFF_MASK,
			       frame.can_dlc);
		}

		/* Hex bajti */
		for (i = 0; i < frame.can_dlc; i++) {
			printf("%02X ", frame.data[i]);
		}

		/* ASCII prikaz (printabilni znakovi) */
		printf("  '");
		for (i = 0; i < frame.can_dlc; i++) {
			printf("%c", (frame.data[i] >= 0x20 && frame.data[i] < 0x7F)
			             ? frame.data[i] : '.');
		}
		printf("'\n");
	}
	
	close(s);
	
	return 0;
}
