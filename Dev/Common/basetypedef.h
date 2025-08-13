/*
 * basetypedef.h
 *
 *  Created on: Nov 20, 2024
 *      Author: SANG HUYNH
 */
#ifndef INC_BASETYPEDEF_H_
#define INC_BASETYPEDEF_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported types ------------------------------------------------------------*/
typedef enum t_Status
{
    STS_FIRST = 0,
    STS_DONE = STS_FIRST,
    STS_ERROR,
    STS_ITEMS_NOT_FOUND,
    STS_INVALID_PARAMETERS,
    STS_OUT_OF_HEAP,
    STS_CRC_ERROR,
    STS_OUT_OF_MEMORY,
    STS_INVALID_RESPONSE,
    STS_MEM_WRITE_FAILED,
    STS_MEM_READ_FAILED,
    STS_NOT_ENABLED,
    STS_LAST = STS_NOT_ENABLED
} t_Status;

typedef enum {NO = 0, YES = !NO} YesNoStatus;

typedef enum {STOP = 0, START = !STOP} StartStopStatus;

typedef enum {CLOSE = 0, OPEN = !CLOSE} OpenCloseStatus;

//typedef enum {RESET = 0, SET = ! RESET} FlagStatus, ITStatus;

#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

//typedef enum {ERROR = 0, SUCCESS = ! ERROR} ErrorStatus;

typedef enum
{
    E_OK       = 0x00U,  /* Operation successful */
    E_ERROR    = 0x01U,  /* Operation failed */
    E_BUSY     = 0x02U,  /* Resource is busy */
    E_TIMEOUT  = 0x03U   /* Operation timed out */
} Std_ReturnType;

/* Exported defines ----------------------------------------------------------*/
#define TRUE                        (uint16_t)(1 == 1)
#define FALSE                       (uint16_t)(1 == 0)
#ifndef NULL
#define NULL                        0
#endif

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define  MIN(a, b)      (((a) < (b)) ? (a) : (b))
#define  MAX(a, b)      (((a) > (b)) ? (a) : (b))

/* Exported functions ------------------------------------------------------- */


#ifdef __cplusplus
}
#endif

#endif /* INC_BASETYPEDEF_H_ */
