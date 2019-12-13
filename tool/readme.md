# rtk log for pi V1.00

> development environment: python  
> script running environment: Raspberry pie

RTK data logging script  

**step_1. Create a new folder named data_collect in the home directory of pi,and copy all the files in rtk_log_for_pi to data_collect     
**step_2. please refer to rc.local in rtk_log_for_pi to modify /etc/rc.local, add this code:  
> cd /home/data_collect  
> sudo ./run.sh &    
step_3. run.sh will auto run when powered on, it will start the data collect script when USB port insertion is detected  
		First insert the USB of RTK, then insert other USB     
step_4. the data file will be placed in a folder named by UTC day,and the data file will be named by the pi system time  

![未找到图片](data_path.png)  

