/*
 * Gopiggy.h
 *
 *  Created on: 2012-1-10
 *      Author: singlaive
 */

#ifndef GOPIGGY_H_
#define GOPIGGY_H_

#define MIN_SPEED 	 6
#define MAX_SPEED    15
#define TRACK_LENGTH 600
#define MAX_NUM_PIGS 10
#define APPLE_DROP_TIMEOUT 5
#define APPLE_SHOWUP_OFFSET 15 /* Seconds. After this period of time since game starts, the first apple is available. */
#define APPLE_SHOWUP_INTERVAL 2 /* SEconds. Once the former appler gets dropped or caught, the next apple shall be produced after this time. */

typedef struct
{
	char * name;
	int current_speed;
	int average_speed;
	int current_position;
	char * status;
	long time_period;
	int num_apples_caught;
}piggy_t;


#endif /* GOPIGGY_H_ */
