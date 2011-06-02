#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <asm/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include <linux/input.h>

/* this macro is used to tell if "bit" is set in "array"
 *  * it selects a byte from the array, and does a boolean AND 
 *   * operation with a byte that only has the relevant bit set. 
 *    * eg. to check for the 12th bit, we do (array[1] & 1<<4)
 *     */
#define test_bit(bit, array)    (array[bit/8] & (1<<(bit%8)))

int main (int argc, char **argv) {

  int fd = -1;
  uint8_t abs_bitmask[ABS_MAX/8 + 1];
  int yalv;

  /* ioctl() requires a file descriptor, so we check we got one, and then open it */
  if (argc != 2) {
    fprintf(stderr, "usage: %s event-device - probably /dev/input/evdev0\n", argv[0]);
    exit(1);
  }
  if ((fd = open(argv[1], O_RDONLY)) < 0) {
    perror("evdev open");
    exit(1);
  }

  memset(abs_bitmask, 0, sizeof(abs_bitmask));
  if (ioctl(fd, EVIOCGBIT(EV_ABS, sizeof(abs_bitmask)), abs_bitmask) < 0) {
      perror("evdev ioctl");
  }

  printf("Supported Absolute axes:\n");

  for (yalv = 0; yalv < ABS_MAX; yalv++) {
      if (test_bit(yalv, abs_bitmask)) {
	  /* this means that the bit is set in the axes list */
	  printf("  Absolute axis 0x%02x ", yalv);
	  switch ( yalv)
	      {
	      case ABS_X :
		  printf(" (X Axis)\n");
		  break;
	      case ABS_Y :
		  printf(" (Y Axis)\n");
		  break;
	      case ABS_Z :
		  printf(" (Z Axis)\n");
		  break;
	      case ABS_RX :
		  printf(" (X Rate Axis)\n");
		  break;
	      case ABS_RY :
		  printf(" (Y Rate Axis)\n");
		  break;
	      case ABS_RZ :
		  printf(" (Z Rate Axis)\n");
		  break;
	      case ABS_THROTTLE :
		  printf(" (Throttle)\n");
		  break;
	      case ABS_RUDDER :
		  printf(" (Rudder)\n");
		  break;
	      case ABS_WHEEL :
		  printf(" (Wheel)\n");
		  break;
	      case ABS_GAS :
		  printf(" (Accelerator)\n");
		  break;
	      case ABS_BRAKE :
		  printf(" (Brake)\n");
		  break;
	      case ABS_HAT0X :
		  printf(" (Hat zero, x axis)\n");
		  break;
	      case ABS_HAT0Y :
		  printf(" (Hat zero, y axis)\n");
		  break;
	      case ABS_HAT1X :
		  printf(" (Hat one, x axis)\n");
		  break;
	      case ABS_HAT1Y :
		  printf(" (Hat one, y axis)\n");
		  break;
	      case ABS_HAT2X :
		  printf(" (Hat two, x axis)\n");
		  break;
	      case ABS_HAT2Y :
		  printf(" (Hat two, y axis)\n");
		  break;
	      case ABS_HAT3X :
		  printf(" (Hat three, x axis)\n");
		  break;
	      case ABS_HAT3Y :
		  printf(" (Hat three, y axis)\n");
		  break;
	      case ABS_PRESSURE :
		  printf(" (Pressure)\n");
		  break;
	      case ABS_DISTANCE :
		  printf(" (Distance)\n");
		  break;
	      case ABS_TILT_X :
		  printf(" (Tilt, X axis)\n");
		  break;
	      case ABS_TILT_Y :
		  printf(" (Tilt, Y axis)\n");
		  break;
	      case ABS_MISC :
		  printf(" (Miscellaneous)\n");
		  break;
	      default:
		  printf(" (Unknown absolute feature)\n");
	      }		 
      }
  }

  close(fd);

  exit(0);
}
