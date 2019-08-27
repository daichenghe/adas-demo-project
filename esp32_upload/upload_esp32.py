#!/usr/bin/env python
import os
import time

key_world = 'bin'
file_list = []


def foreach(rootDir,file):
    print "start..."
    start_time = time.time()
    list1=[]
    for root,dirs,files in os.walk(rootDir):
		files = sorted(files,key=lambda x: os.path.getmtime(os.path.join(rootDir, x)))
		#print files
		for filename in files:
			#print "for each file!!!!",filename
			result = key_world in filename		
			if result != True:
				continue
			filepath = os.path.join(root, filename)
			file_list.append(filepath)
			#print filepath	
    #print filepath,'\r\n'
    end_time = time.time()
    #print "end,%.2fs" % (end_time - start_time)
	
	
foreach('./',".txt")


print "find bin: "		
for i in range(len(file_list)):
	print "%d  - %s\r\n" % (i, file_list[i])
	
print "put your upload esp32 bin file"
upload_file_num = raw_input()
print "select com channel"
com = raw_input()
cmd = 'python ' + './parse_upload.py ' + '--chip esp32 ' + '--port ' + com + ' --baud 115200 ' \
+ '--before default_reset ' + '--after hard_reset write_flash -z ' + '--flash_mode dio ' \
+ '--flash_freq 40m ' + '--flash_size detect 0x10000 ' + file_list[int(upload_file_num)]
os.system(cmd)

