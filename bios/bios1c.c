#include	"compiler.h"
#include	"parts.h"
#include	"timemng.h"
#include	"cpucore.h"
#include	"pccore.h"
#include	"iocore.h"
#include	"calendar.h"
#include	"bios.h"
#include	"biosmem.h"


void bios0x1c(void) {

	UINT8	buf[6];

	switch(CPU_AH) {
		case 0x00:					// get system timer
			calendar_get(buf);
			MEMR_WRITES(CPU_ES, CPU_BX, buf, 6);
			break;

		case 0x01:					// put system timer
			MEMR_READS(CPU_ES, CPU_BX, buf, 6);
			mem[MEMB_MSW8] = buf[0];
			calendar_set(buf);
			break;

		case 0x02:					// set interval timer (single)
			SETBIOSMEM16(0x0001c, CPU_BX);
			SETBIOSMEM16(0x0001e, CPU_ES);
			SETBIOSMEM16(0x0058a, CPU_CX);
			iocore_out8(0x77, 0x36);
			/*FALLTHROUGH*/

		case 0x03:					// continue interval timer
			iocore_out8(0x71, 0x00);
			if (pccore.cpumode & CPUMODE_8MHZ) {
				iocore_out8(0x71, 0x4e);				// 4MHz
			}
			else {
				iocore_out8(0x71, 0x60);				// 5MHz
			}
			pic.pi[0].imr &= ~(PIC_SYSTEMTIMER);
			break;
	}
}

