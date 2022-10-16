#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#define N 2000

/**
   Implement this function to list processes. It must call report_file()
   for each open file. If an error occurs when accessing a file or
   a directory, it must call report_error().
*/

void lsof(void){
	
	DIR *procdir = opendir("/proc");
    
	if (procdir == NULL) {
		report_error("/proc", errno);
	    return;
	    }
		
	struct dirent * d = NULL; // создаем структуру dirent
    
	errno = 0;

	while (d = readdir(procdir) != NULL) {
		
		pid_t pid = atoi(d->d_name); 
        
		if (pid == 0) continue; 
        
	char fdpath[N] = {0}, flpath[2*N] = {0}, allpath[N] = {0};
        
	sprintf(fdpath, "/proc/%d/fd", pid);
        
	DIR *fd_dir = opendir(fdpath);
        
	if (fd_dir == NULL) {
		report_error(fdpath, errno);
		closedir(procdir);
		return;
	}
	
    struct dirent * fd_file = NULL;
        
    while (fd_file = readdir(fd_dir) != NULL) {
		
    	int fd = atoi(fd_file->d_name);
		if (fd == 0) continue;
            
        memset(flpath, 0, N);
        memset(allpath, 0, N);
            
        sprintf(flpath, "%s/%d", fdpath, fd);
            
       	int count = readlink(flpath, allpath, N); //считывает значение символьной ссылки
            
        if (count < 0) {
        	report_error(flpath, errno);
        	return;	
		}
            
        report_file(allpath);
	}
        
        closedir(fd_dir);
	}
	if (errno != 0) report_error("/proc", errno);
    
    closedir(procdir);
       
}
