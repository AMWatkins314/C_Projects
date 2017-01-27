/*********************************************************
* PROGRAM NAME: myls.c
*
* INPUT: 
*     (OPTIONAL) STRING: The name of the file(s) to output the permissions for. 
*
* OUTPUT: The permissions of all files and directory files in the user's home directory or
*         If file(s) are specified then output the permissions of each of them only.
*
* DESCRIPTION: Behaves like 'ls -l' but only outputs the permissions. 
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

#define PERMS_SIZE 3;


int main(int argc, char *argv[]){

   int i = 0; // Index for looping through filenames
   int retval = 0; // Return value from stat
   int uid  = getuid(); // Get the user's ID
   int guid = getgid(); // Get the user's Group ID
   int oid  = 0; // The file owner's ID
   int goid = 0; // The file owner's Group ID 

   char filetype  = '?'; // The file type reported by stat 
   char fperms[3] = {'-', '-', '-'}; // The users's read, write, execute perms for the file

   struct passwd *pw;
   struct stat filestat;

   pw = getpwuid(uid); // Get the password struct for the User
   char *users_home_dir = pw->pw_dir; // Get the User's home directory
   char *filename = NULL; // To hold the current filename

   int homedir_len  = strlen(users_home_dir) + 1; // Add the trailing backslash 
   int filename_len = 0;

   // Output the file permissions for the user for the files given as arguments 
   if(argc>1){
      for(i=1; i<argc; i++){
         filename_len = strlen(argv[i]);

         // Prepend the User's home dir to relative path file names 
         if( *(argv[i]) != '/'){
             filename = (char *)malloc((homedir_len + filename_len)*sizeof(char));
             strcpy(filename, users_home_dir);
             strcat(filename,"/");
             strcat(filename, argv[i]);
         }
         else{
             filename = (char *)malloc(filename_len*sizeof(char));
             strcpy(filename, argv[i]);
         }
         if( (stat(filename, &filestat)) != (-1) ){
            oid  = filestat.st_uid;
            goid = filestat.st_gid;
 
            filetype = (S_ISREG(filestat.st_mode))  ? '-' : ( 
                       (S_ISDIR(filestat.st_mode))  ? 'd' : ( 
                       (S_ISCHR(filestat.st_mode))  ? 'c' : (
                       (S_ISBLK(filestat.st_mode))  ? 'b' : (
                       (S_ISFIFO(filestat.st_mode)) ? 'p' : ( 
                       (S_ISLNK(filestat.st_mode))  ? 'l' : (
                       (S_ISSOCK(filestat.st_mode)) ? 's' : '?' 
                       )))))); 
            
            // If the user is the owner, then output the Owner permissions for the file
            if(uid == oid){
               fperms[0] = (filestat.st_mode & S_IRUSR) ? 'r' : '-';
               fperms[1] = (filestat.st_mode & S_IWUSR) ? 'w' : '-';
               fperms[2] = (filestat.st_mode & S_IXUSR) ? 'x' : '-';
            }
            // Else, if the user is in the owner's group, then output the Group permissions
            else if(guid == goid){
               fperms[0] = (filestat.st_mode & S_IRGRP) ? 'r' : '-';
               fperms[1] = (filestat.st_mode & S_IWGRP) ? 'w' : '-';
               fperms[2] = (filestat.st_mode & S_IXGRP) ? 'x' : '-';
            }
            // Else, the user has the World permissions
            else{
               fperms[0] = (filestat.st_mode & S_IROTH) ? 'r' : '-';
               fperms[1] = (filestat.st_mode & S_IWOTH) ? 'w' : '-';
               fperms[2] = (filestat.st_mode & S_IXOTH) ? 'x' : '-';
            }
            printf("%c%s\t%s\n", filetype, fperms, filename);
         }
         else{
            fprintf(stderr, "myls: %s: %s\n", argv[i], strerror(errno));
         }     
         free(filename);
         filename = NULL;
      }
   }
   // Else output the file permissions for the user for all files in their home directory
   else{
      DIR *homedir;
      struct dirent *dir;

      if(homedir = opendir(users_home_dir)){
         while(dir = readdir(homedir)){
            // Omit hidden files
            if( (dir->d_name)[0] != '.'){
               filename_len = strlen(dir->d_name);
               filename = (char *)malloc((homedir_len + filename_len)*sizeof(char));
               strcpy(filename, users_home_dir);
               strcat(filename,"/");
               strcat(filename, dir->d_name);

               if( (stat(filename, &filestat)) != (-1) ){
                  oid  = filestat.st_uid;
                  goid = filestat.st_gid;
 
                  filetype = (S_ISREG(filestat.st_mode))  ? '-' : ( 
                             (S_ISDIR(filestat.st_mode))  ? 'd' : ( 
                             (S_ISCHR(filestat.st_mode))  ? 'c' : (
                             (S_ISBLK(filestat.st_mode))  ? 'b' : (
                             (S_ISFIFO(filestat.st_mode)) ? 'p' : ( 
                             (S_ISLNK(filestat.st_mode))  ? 'l' : (
                             (S_ISSOCK(filestat.st_mode)) ? 's' : '?' 
                             )))))); 
            
                  // If the user is the owner, then output the Owner permissions for the file
                  if(uid == oid){
                     fperms[0] = (filestat.st_mode & S_IRUSR) ? 'r' : '-';
                     fperms[1] = (filestat.st_mode & S_IWUSR) ? 'w' : '-';
                     fperms[2] = (filestat.st_mode & S_IXUSR) ? 'x' : '-';
                  }
                  // Else, if the user is in the owner's group, then output the Group permissions
                  else if(guid == goid){
                     fperms[0] = (filestat.st_mode & S_IRGRP) ? 'r' : '-';
                     fperms[1] = (filestat.st_mode & S_IWGRP) ? 'w' : '-';
                     fperms[2] = (filestat.st_mode & S_IXGRP) ? 'x' : '-';
                  }
                  // Else, the user has the World permissions
                  else{
                     fperms[0] = (filestat.st_mode & S_IROTH) ? 'r' : '-';
                     fperms[1] = (filestat.st_mode & S_IWOTH) ? 'w' : '-';
                     fperms[2] = (filestat.st_mode & S_IXOTH) ? 'x' : '-';
                  }
                  printf("%c%s\t%s\n", filetype, fperms, filename);
               }

               free(filename);
               filename = NULL;
            }
         }
         closedir(homedir);
      } 
   }
  
   return(0);
}

//END
