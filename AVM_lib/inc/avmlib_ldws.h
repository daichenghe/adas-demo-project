/*******************************************************************************
* File Name          : avmlib_ldws.h
* Author             : Jerry
* Revision           : 2.1
* Date               : 15/07/2016
* Description        : Library Interface 
*******************************************************************************/
#ifndef _LDWS_H_
#define _LDWS_H_

#include <stdio.h>

/* Extern define **************************************************************/

/* Extern typedef *************************************************************/

#define LDWS_LINE_DETECT_NONE	(0)
#define LDWS_LINE_DETECT_EXIST	(1)

typedef enum
{
	WARNING_OFF,
	WARNING_ON,
}ldws_warning_t;

typedef enum
{
	SENSITIVITY_LOW,
	SENSITIVITY_MID,
	SENSITIVITY_HIGH,
}ldws_sensitivity_t;

typedef enum
{
	STRATEGY_DOUBLE_LINE,
	STRATEGY_SINGLE_LINE,
}ldws_strategy_t;

typedef enum
{
	TURN_SIGNAL_NONE,
	TURN_SIGNAL_LEFT,
	TURN_SIGNAL_RIGHT,
	TURN_SIGNAL_DOUBLE,

}vehicle_turn_signal_t;

typedef struct ldws_input_type
{
	int vehicle_width;
	int vehicle_length;
	double vehicle_speed;
	double vehicle_steering_angle;
	
	vehicle_turn_signal_t 	vehicle_signal;
	ldws_sensitivity_t 		ldws_sensitivity;
	ldws_strategy_t			ldws_strategy;
	
	int avm_scale;
	int avm_mask_widht;
	int avm_mask_length;
}ldws_input_t;

typedef struct ldws_output_type
{
	int ldws_left_dist;
	int ldws_left_heading;
	int ldws_left_detect_status;
	ldws_warning_t ldws_left_warning_status;
	
	int ldws_right_dist;
	int ldws_right_heading;
	int ldws_right_detect_status;
	ldws_warning_t ldws_right_warning_status;

}ldws_output_t;

typedef struct ldws_type
{
	ldws_input_t 	ldws_input_info;
	ldws_output_t 	ldws_output_info;
	unsigned char 	*ldws_input_buf;
}ldws_type_t;

AVM_ERR avmlib_ldws_init(ldws_type_t *ldws_obj);
AVM_ERR	avmlib_ldws_reset();
AVM_ERR avmlib_ldws_process(ldws_type_t *ldws_obj);


#endif