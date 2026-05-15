#include <stdio.h>
#include <unistd.h>
#include <modbus.h>

#define SERVER_ID		1
#define COIL_ADDRESS		0
#define BCM_PIN_DE		17

int main()
{

	modbus_t *ctx;

	ctx = modbus_new_rtu("/dev/ttyAMA0", 9600, 'N', 8, 1);

	if (ctx == NULL) {
	fprintf(stderr, "Unable to create the libmodbus context!\n");
	return -1;
	}
	
	modbus_set_slave(ctx, SERVER_ID);
	modbus_set_debug(ctx, TRUE);
	
	if (modbus_connect(ctx) == -1) {
	fprintf(stderr, "Connection failed.\n");
	modbus_free(ctx);
	return -1;
	}
	modbus_rtu_set_rts(ctx, MODBUS_RTU_RTS_UP);
	// Do some communication over Modbus
	modbus_write_bit(ctx, COIL_ADDRESS, TRUE);
	sleep(5);
	modbus_write_bit(ctx, COIL_ADDRESS, FALSE);
	
	// Closing the connection
	
	modbus_close(ctx);
	modbus_free(ctx);

	return 0;
}
