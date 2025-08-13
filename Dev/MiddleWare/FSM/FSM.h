#ifndef FSM_H
#define FSM_H

#include <stdint.h>
#include <stdbool.h>
#include "FSM_signals.h"

typedef uint16_t signal_t; /* event signal */

/* Event base class */
typedef struct {
    signal_t sig; /* event signal */
    /* event parameters added in subclasses of event */
} event_t;

/*---------------------------------------------------------------------------*/
/* Finite State Machine facilities... */
typedef struct fsm_t fsm_t; /* forward declaration */

typedef enum {
    TRAN_STATUS,
    HANDLED_STATUS,
    IGNORED_STATUS,
} state_t;

typedef state_t (*state_handler_t)(fsm_t * const me, event_t const * const e);

#define TRAN(target_) (((fsm_t *)me)->state = (state_handler_t)(target_), TRAN_STATUS)

struct fsm_t {
    state_handler_t state; /* the "state variable" */
};

void fsm_ctor(fsm_t * const me, state_handler_t initial);
void fsm_init(fsm_t * const me, event_t const * const e);
void fsm_dispatch(fsm_t * const me, event_t const * const e);


#endif /* FSM_H */
