#ifndef M4_UTILS_DATETIME_DATE_TIME_H_
#define M4_UTILS_DATETIME_DATE_TIME_H_
#include "stdint.h"

typedef struct {
	uint8_t day;    	//!< Day: Starting at 1 for the first day
	uint8_t month;  	//!< Month: Starting at 1 for January
	uint8_t year;   	//!< Year in format YY (2000 - 2099)
	uint8_t hour;   	//!< Hour
	uint8_t minute; 	//!< Minute
	uint8_t second; 	//!< Second
} s_DateTime;

void date_time_init(void);
void date_time_set(uint8_t day, uint8_t hour, uint8_t minute, uint8_t second);

void date_time_update(void);

void date_time_get(uint8_t *days, uint8_t *hours, uint8_t *minutes, uint8_t *seconds);

#endif /* M4_UTILS_DATETIME_DATE_TIME_H_ */
