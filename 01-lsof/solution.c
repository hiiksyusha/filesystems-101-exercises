#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>
#include <solution.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

/**
   Implement this function to list processes. It must call report_file()
   for each open file. If an error occurs when accessing a file or
   a directory, it must call report_error().
*/

void lsof(void){
	
	errno = 0;
	DIR *procdir = opendir("/proc");
    
	if (errno != 0) {
		report_error("/proc", errno);
	    return;
	    }
		
	struct dirent * d = NULL; // создаем структуру dirent
    
	errno = 0;

	while (d == readdir(procdir)) {
		if (d == NULL) break;
		
		pid_t pid = atoi(d->d_name); 
        
		if (pid == 0) continue; 
        
	char fdpath[1000] = {0}, flpath[2000] = {0}, allpath[1000] = {0};
        
	sprintf(fdpath, "/proc/%d/fd", pid);
        
	DIR *fd_dir = opendir(fdpath);
        
	if (fd_dir == NULL) {
		report_error(fdpath, errno);
		closedir(procdir);
		return;
	}
	
    struct dirent * fd_file = NULL;
        
    while (fd_file == readdir(fd_dir)) {
		if (fd_file == NULL) break;
		
    	int fd = atoi(fd_file->d_name);
		if (fd == 0) continue;
            
        memset(flpath, 0, 1000);
        memset(allpath, 0, 1000);
            
        sprintf(flpath, "%s/%d", fdpath, fd);
            
       	int count = readlink(flpath, allpath, 1000); //считывает значение символьной ссылки  
            
        if (count < 0) {
        	report_error(flpath, errno);
        	return;	
		}
            
        report_file(allpath);
	}
        
        closedir(fd_dir);
	}
	if (errno != 0) report_error("/proc", errno);
       
}
