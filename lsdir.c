/*********************************************************
* PROGRAM NAME: lsdir.c
*
* INPUT: 
*     (OPTIONAL) STRING: The directory that should have its subdirs listed. 
*
* OUTPUT: The subdirectories of the given or current working directory. 
*
* DESCRIPTION: Recursively lists all subdirectories for the specified directory. 
*              If no directory is specified, the current directory is used.
*               
*********************************************************/
#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* ARG dirname: the full or relative path directory name whose subdirectories should be output */
void list_directories(char *dirname){
   DIR *startdir_ptr;
   struct dirent *currentdir_ptr;
   struct stat filestat;

   startdir_ptr = opendir(dirname); 

   if(startdir_ptr != NULL){
      while( (currentdir_ptr = readdir(startdir_ptr)) != NULL){
         char *dname     = currentdir_ptr->d_name;
         int dname_len   = strlen(dname);
         int dirname_len = strlen(dirname); 
        
         // Construct the full path to the directory currently being looked at 
         char *name = malloc( (dname_len + dirname_len +1)*sizeof(char) );
         strcpy(name, dirname);
         strcat(name, dname); 
         strcat(name, "/"); 

         // Only output if the directory really is a directory and not a file 
         if( (stat(name, &filestat) != -1) && S_ISDIR(filestat.st_mode) ){
            // Get the directories under this directory if dir is not . or ..
            if(strcmp(dname, ".") != 0 && strcmp(dname, "..") != 0 ) {
               printf("%s\n", dname);
               list_directories(name);
            }
         }
         if(name){
            free(name);
            name = NULL;
         }
      }
      if(closedir(startdir_ptr) == -1){
         fprintf(stderr, "lsdir: closedir: %s\n", strerror(errno));
      }
   }
   else{
      fprintf(stderr, "lsdir: %s: %s\n", dirname, strerror(errno));
   }     

   return;
} // END OF list_directories 



int main(int argc, char *argv[]){

   // Get the directories to recursively list the subdirectories for 
   if(argc>1){
      int i;
      for(i=1; i<argc; i++){
         char *filename = argv[i];
         int filename_len = strlen(argv[i]);

         // Append a '/' to the end of the dir 
         if(filename[filename_len - 1] != '/'){
            // Prepend the current dir to filenames if they do not already have an absolute or relative path 
            if( (*filename != '/') && (*filename != '.' && *(filename++) != '/') ){
               filename = (char *)malloc((filename_len + 3)*sizeof(char));
               strcpy(filename, "./");
               strcat(filename, argv[i]);
               strcat(filename, "/");
            }
            // Do not prepend the current dir since an absolute or relative path is already given
            else{
               filename = (char *)malloc((filename_len + 1)*sizeof(char));
               strcpy(filename, argv[i]);
               strcat(filename, "/");
            }
         }
         // Do not Append a '/' to the end of the dir since there is already a '/' 
         else{
            // Prepend the current dir to filenames if they do not already have an absolute or relative path 
            if( (*filename != '/') && (*filename != '.' && *(filename++) != '/') ){
               filename = (char *)malloc((filename_len + 2)*sizeof(char));
               strcpy(filename, "./");
               strcat(filename, argv[i]);
            }
            // Do not prepend the current dir since an absolute or relative path is already given
            else{
               filename = (char *)malloc(filename_len*sizeof(char));
               strcpy(filename, argv[i]);
            }
         }

         list_directories(filename);

         if(filename){
            free(filename);
            filename = NULL;
         }
      }
   }
   // Else list the subdirectories for the current working directory 
   else{
      list_directories("./");
   }
  
   return(0);
} // END OF main

//END OF FILE
