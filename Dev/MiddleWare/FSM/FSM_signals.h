/*
 * signals.h
 *
 *  Created on: Apr 27, 2025
 *      Author: Admin
 */

#ifndef UTILS_SIGNALS_H_
#define UTILS_SIGNALS_H_

/* Reserved signals for FSM */
enum reserved_signals {
    SIG_INIT,    /* initial transition */
    SIG_ENTRY,   /* state entry */
	SIG_UPDATE,	/* state update internally */
    SIG_EXIT,    /* state exit */
    SIG_USER     /* first user-defined signal */
};



/* Display modes */
enum display_mode {
    MODE_INIT,
    MODE_NORMAL,
    MODE_SETTING,
    MODE_OFF,
    MODE_ERROR
};

#endif /* UTILS_SIGNALS_H_ */
