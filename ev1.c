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

	int fd;
	uint8_t evtype_bitmask[EV_MAX/8 + 1] = { };
	int yalv;

  	while (1) {
		fd = open("/dev/input/event15", O_RDONLY);
		if (fd >= 0)
			break;
	}

	if (ioctl(fd, EVIOCGBIT(0, EV_MAX), evtype_bitmask) < 0) {
		perror("evdev ioctl");
	}

	printf("Supported event types:\n");

  for (yalv = 0; yalv < EV_MAX; yalv++) {
      if (test_bit(yalv, evtype_bitmask)) {
	  /* this means that the bit is set in the event types list */
	  printf("  Event type 0x%02x ", yalv);
	  switch ( yalv)
	      {
	      case EV_KEY :
		  printf(" (Keys or Buttons)\n");
		  break;
	      case EV_REL :
		  printf(" (Relative Axes)\n");
		  break;
	      case EV_ABS :
		  printf(" (Absolute Axes)\n");
		  break;
	      case EV_MSC :
		  printf(" (Something miscellaneous)\n");
		  break;
	      case EV_LED :
		  printf(" (LEDs)\n");
		  break;
	      case EV_SND :
		  printf(" (Sounds)\n");
		  break;
	      case EV_REP :
		  printf(" (Repeat)\n");
		  break;
	      case EV_FF :
		  printf(" (Force Feedback)\n");
	      default:
		  printf(" (Unknown event type: 0x%04hx)\n", yalv);
	      }		 
      }
  }

  close(fd);

  exit(0);
}
