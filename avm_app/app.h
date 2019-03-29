#ifndef __APP__H__
#define __APP__H__

#ifdef T3BOARD
#include "DecoderHal.h"

extern DecoderHal * get_player(void);
extern dec_status_  get_player_state(void);
extern void set_player_state(dec_status_ state);

#endif

#define AVM_STATE_ACTIVE 	(1)
#define AVM_sTATE_INACTIVE	(0)

extern int g_sysquit;
#endif