#ifndef INDIGO_REVISIONS_H
#define INDIGO_REVISIONS_H

/* system_rev values for our devices */

/* system revisions */
#define DEVICE_UNKNOWN 0
#define DEVICE_STARTERKIT 1
#define DEVICE_1_0 2
#define DEVICE_1_1 3
#define DEVICE_2_0 4
#define	DEVICE_2_1 5
#define DEVICE_STARTERKIT_9G45 6
#define DEVICE_2_2 7

#define SYSTEM_REV_MASK 0xfffful
#define GPIOPERIPH_SELECT_MASK 0xffff0000ul

#define indigo_revision (system_rev & SYSTEM_REV_MASK)
#define indigo_hardware (system_rev & GPIOPERIPH_SELECT_MASK)

#endif  /* INDIGO_REVISIONS_H */
