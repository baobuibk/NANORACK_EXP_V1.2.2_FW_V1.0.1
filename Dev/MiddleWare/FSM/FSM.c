#include "fsm.h"
#include <stdlib.h>
#include <string.h>
#include "atomic.h"
/*---------------------------------------------------------------------------*/
/* Finite State Machine facilities... */
static event_t const entry_evt = { SIG_ENTRY };
static event_t const exit_evt  = { SIG_EXIT };

void fsm_ctor(fsm_t * const me, state_handler_t initial) {
    me->state = initial;
}

void fsm_init(fsm_t * const me, event_t const * const e) {
    (*me->state)(me, e);
    (*me->state)(me, &entry_evt);
}

void fsm_dispatch(fsm_t * const me, event_t const * const e) {
    state_t status;
    state_handler_t prev_state = me->state; /* save for later */

    status = (*me->state)(me, e);

    if (status == TRAN_STATUS) { /* transition taken? */
        (*prev_state)(me, &exit_evt);
        (*me->state)(me, &entry_evt);
    }
}

