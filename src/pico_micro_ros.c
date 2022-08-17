#include "../include/pico_micro_ros.h"

void init_rmw_uros()
{
     rmw_uros_set_custom_transport(
		true, // Framing enabled here. Using Stream-oriented mode.
		NULL, // void * args,
		pico_serial_transport_open,
		pico_serial_transport_close,
		pico_serial_transport_write,
		pico_serial_transport_read
	);
}