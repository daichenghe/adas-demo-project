/*******************************************************************************
* File Name          : avm_view_config.h
* Author             : Duqf
* Revision           : 2.1
* Date               : 03/07/2018
* Description        : draw ui
*
* HISTORY***********************************************************************
* 03/07/2018  | sDas Basic 2.0                       | Duqf
*
*******************************************************************************/

#ifndef __AVM_VIEW_CONFIG_H__
#define __AVM_VIEW_CONFIG_H__

#define SCREEN_WIDTH	    (1280)
#define SCREEN_HEIGHT	(720)

#define SCREEN_DISPLAY_WIDTH    1280
#define SCREEN_DISPLAY_HEIGHT   720 


/**************************2D环视***********************/
#define SCREEN_X_2D               (0*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_Y_2D               (0*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)
#define SCREEN_WIDTH_2D           (256*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_HEIGHT_2D          (480*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)
/**************************END**************************/

/**************************SV环视***********************/
#define SCREEN_X_SV               (258*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_Y_SV               (0*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)
#define SCREEN_WIDTH_SV           (462*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_HEIGHT_SV          (480*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)
/**************************END**************************/


/**************************BSD**************************/
#define SCREEN_X_BSD              (352.0000*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_Y_BSD              (66.0000*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)
#define SCREEN_WIDTH_BSD          (320.0000*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_HEIGHT_BSD         (384.0000*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)
/**************************END**************************/



/**************************PER*************************/
#define SCREEN_X_PER_L             (0*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_Y_PER_L             (0*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)
#define SCREEN_WIDTH_PER_L         (241*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_HEIGHT_PER_L        (480*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)

#define SCREEN_X_PER_R             (482*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_Y_PER_R             (0*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)
#define SCREEN_WIDTH_PER_R         (237*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_HEIGHT_PER_R        (480*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)

#define SCREEN_X_PER_CENTER        (243*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_Y_PER_CENTER        (0*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)
#define SCREEN_WIDTH_PER_CENTER    (238*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_HEIGHT_PER_CENTER   (430*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)

/**************************END**************************/

/**************************SVFS*************************/
#define SCREEN_X_SVFS_L             (491*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_Y_SVFS_L             (0*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)
#define SCREEN_WIDTH_SVFS_L         (229*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_HEIGHT_SVFS_L        (480*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)

#define SCREEN_X_SVFS_R             (258*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_Y_SVFS_R             (0*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)
#define SCREEN_WIDTH_SVFS_R         (230*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_HEIGHT_SVFS_R        (480*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)
/**************************END**************************/

/***********************BACKUP************************/
#define SCREEN_X_BACKUP         (0*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_Y_BACKUP         (0*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)
#define SCREEN_WIDTH_BACKUP     (720*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_HEIGHT_BACKUP    (480*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)
/**************************END**************************/




/**************************原始视图***********************/
#define SCREEN_X_ORI              (2*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_Y_ORI              (91*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)
#define SCREEN_WIDTH_ORI          (796*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_HEIGHT_ORI         (629*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)

#define SCREEN_X_ORI_SPLIT_L      (0*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)  
#define SCREEN_Y_ORI_SPLIT_L      (0*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)  
#define SCREEN_WIDTH_ORI_SPLIT_L  (259*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)      
#define SCREEN_HEIGHT_ORI_SPLIT_L (239*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)        

#define SCREEN_X_ORI_SPLIT_R      (360*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)  
#define SCREEN_Y_ORI_SPLIT_R      (0*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)  
#define SCREEN_WIDTH_ORI_SPLIT_R  (359*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)      
#define SCREEN_HEIGHT_ORI_SPLIT_R (239*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)    

#define SCREEN_X_ORI_SPLIT_F      (0*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)  
#define SCREEN_Y_ORI_SPLIT_F      (240*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)  
#define SCREEN_WIDTH_ORI_SPLIT_F  (259*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)      
#define SCREEN_HEIGHT_ORI_SPLIT_F (239*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)    

#define SCREEN_X_ORI_SPLIT_B      (360*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)  
#define SCREEN_Y_ORI_SPLIT_B      (240*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)  
#define SCREEN_WIDTH_ORI_SPLIT_B  (359*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)      
#define SCREEN_HEIGHT_ORI_SPLIT_B (239*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)     
#define SCALE_X_ORI               1.5         //1.5
#define SCALE_Y_ORI               1.5         //1.5
#define SCALE_Z_ORI               1.5
/**************************END**************************/


/***********************原始单视显示************************/
#define SCREEN_X_ORC              (2*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_Y_ORC              (91*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)
#define SCREEN_WIDTH_ORC          (796*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_HEIGHT_ORC         (629*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)
#define SCALE_X_ORC               1.0         //1.5
#define SCALE_Y_ORC               1.0         //1.5
#define SCALE_Z_ORC               1.5

#define SCALE_X_CAL               1.0         //1.5
#define SCALE_Y_CAL               1.0         //1.5

/**************************END**************************/

/***********************单视放大************************/
#define SCREEN_X_ZOOM              (0*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_Y_ZOOM              (600*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)
#define SCREEN_WIDTH_ZOOM          (200*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_HEIGHT_ZOOM         (120*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)
#define SCALE_X_ZOOM               12         
#define SCALE_Y_ZOOM               12        
#define SCALE_Z_ZOOM               1
/***********************END************************/

/***********************LDWS************************/
#define LDWS_VIEW_X        (0*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define LDWS_VIEW_Y        (0*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)
#define LDWS_VIEW_WIDTH    (256*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define LDWS_VIEW_HEIGHT   (480*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)

/***********************END************************/


/***********************BSD************************/
#define BSD_VIEW_X        (350*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define BSD_VIEW_Y        (0*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)
#define BSD_VIEW_WIDTH    (320*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define BSD_VIEW_HEIGHT   (384*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)


#define BSD_W 100
#define BSD_H 146

#define BSD_X_L 22
#define BSD_Y_L 237

#define BSD_X_R 201
#define BSD_Y_R 237
/***********************END************************/


/****************************3D视图**************************/
#define SCREEN_X_3D            (258*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_Y_3D            (60*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_WIDTH_3D        (462*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_HEIGHT_3D       (360*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)
#define SCREEN_WIDTH_FLY3D     (1024*SCREEN_WIDTH/SCREEN_DISPLAY_WIDTH)
#define SCREEN_HEIGHT_FLY3D    (600*SCREEN_HEIGHT/SCREEN_DISPLAY_HEIGHT)

#define EYE_X_3D            0
#define EYE_Y_3D            3000
#define EYE_Z_3D            6500

/****************************END**************************/

#endif
