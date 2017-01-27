/*********************************************************
* PROGRAM NAME: mywc_pf_pf.c (mywc_pf using pipes and forks)
*
* INPUT: 
*     (OPTIONAL) STRING: The name of the file(s) to perform wordcount on. 
*
* OUTPUT: The same as Unix's wc command for the c, l, and w options for 0+ files. 
*         The process ID for each process that processed a command line file (or the
*         one process that processed all files in the current directory) is appended
*         to the wc output.
*
* DESCRIPTION: Simulate Unix's wc command with the c, l, and w options for 0+ files. 
*              It creates as many processes as their are files on the command line.
*              If no file names are entered, then 1 process is created.
*
*********************************************************/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define STATSIZE 4


pid_t pid = 0; // parent waits until child process is complete
int stats_arr[STATSIZE] = {0, 0, 0, 0}; // holds line count, word count, byte count, and process id

/* Calculate the number of lines, bytes, and words in the file or stdin */
void * calculate_file_counts(FILE *stream){
   char c = '0';
   int prevchar = 0;
   int line_cnt = 0;
   int word_cnt = 0;
   int byte_cnt = 0;

   while( (c=getc(stream)) != EOF){
      byte_cnt += sizeof(c);

      if(c == '\n'){
         line_cnt++;

         if(prevchar){
            word_cnt++;
            prevchar = 0;
         }
      }
      else if(c == ' '){
         if(prevchar){
            word_cnt++;
            prevchar = 0;
         }
      }
      else{
         prevchar = 1;
      }
   }
   pid = getpid();

   stats_arr[0] = line_cnt;
   stats_arr[1] = word_cnt;
   stats_arr[2] = byte_cnt;
   stats_arr[3] = pid;

   return;
} // END OF calculate_file_counts


int main(int argc, char *argv[]){
   int line_cnt_flag = 0;
   int word_cnt_flag = 0;
   int byte_cnt_flag = 0;

   int arg_cnt  = 0;
   int file_cnt = 0;

   int fd[2]; // pipe file descriptor, fd[0] is for reading and fd[1] is for writing

   if(pipe(fd) == -1){
      fprintf(stderr, "mywc_pf: %s\n", strerror(errno)); 
      exit(1);
   }   

   char **filenames = (argc > 1) ? malloc((argc-1) * sizeof(char*)) : NULL; 

   // Process the command line arguments if there are any
   while(--argc){
      char *argstr = argv[++arg_cnt]; // Point to the current string to process      

      // Check if argstr is options
      if(*argstr == '-'){
         while(*(++argstr)){
            switch (*argstr) {
               case 'c': 
                  byte_cnt_flag = 1;
                  break;
               case 'l': 
                  line_cnt_flag = 1;
                  break;
               case 'w': 
                  word_cnt_flag = 1;
                  break;
               default:
                  printf("mywc_pf: invalid option -- '%c'\n", *argstr);
                  return(0); 
            }
         }
      }
      // Else argstr is a filename so record it so it can be processed later 
      else {
         filenames[file_cnt] = malloc(strlen(argstr) * sizeof(char)); 
         strcpy(filenames[file_cnt], argstr);
         file_cnt++;
      }
   }
   
   // If at least one option flag was specified
   if( line_cnt_flag | word_cnt_flag | byte_cnt_flag ){
      // If at least one file name was specified
      // Output the values corresponding to the flags followed by the file name and child process id
      if(file_cnt){
         int line_total = 0;
         int word_total = 0;
         int byte_total = 0;

         int i;
         for(i=0; i<file_cnt; i++){
            int flen = strlen(filenames[i]);
            char *fname = malloc(flen*sizeof(char));
            strcpy(fname, filenames[i]); 

            pid_t ret_pid = fork();

            if(ret_pid == 0){ // Child Process
               FILE *fp;

               fp = fopen(filenames[i], "r");

               if(fp == NULL){
                  fprintf(stderr, "mywc_pf: %s: %s\n", filenames[i], strerror(errno)); 
                  exit(1);
               }
               else{
                  calculate_file_counts(fp);
                  fclose(fp);

                  if(write(fd[1], stats_arr, STATSIZE*sizeof(int)) == -1){
                     fprintf(stderr, "mywc_pf: %s\n", strerror(errno)); 
                     exit(1);
                  };
               }
               return;
            }
            else if(ret_pid > 0){ // Parent Process
               wait(pid);    

               if(read(fd[0], stats_arr, STATSIZE*sizeof(int)) == -1){
                  fprintf(stderr, "mywc_pf: %s\n", strerror(errno)); 
                  exit(1);
               }
               if(line_cnt_flag){
                  line_total += stats_arr[0];
                  printf("%d ", stats_arr[0]);
               }
               if(word_cnt_flag){
                  word_total += stats_arr[1];
                  printf("%d ", stats_arr[1]);
               }
               if(byte_cnt_flag){
                  byte_total += stats_arr[2];
                  printf("%d ", stats_arr[2]);
               }
               printf("%s ", fname); 
               free(fname);
               printf("%d\n", stats_arr[3]); 
            }
            else{ // ERROR
               fprintf(stderr, "mywc_pf: %s\n", strerror(errno)); 
            }
         }
         // Output line, word, and byte count totals if more than one file was specified
         if(file_cnt > 1){
            if(line_cnt_flag){
               printf("%d ", line_total);
            }
            if(word_cnt_flag){
               printf("%d ", word_total);
            }
            if(byte_cnt_flag){
               printf("%d ", byte_total);
            }
            printf("total\n"); 
         }
      }
      // Else no file name so read from STDIN
      // Output the values corresponding to the flags and the child process id 
      else{
         pid_t ret_pid = fork();

         if(ret_pid == 0){ // Child Process
            calculate_file_counts(stdin);

            if(write(fd[1], stats_arr, STATSIZE*sizeof(int)) == -1){
               fprintf(stderr, "mywc_pf: %s\n", strerror(errno)); 
               exit(1);
            };

            return;
         }
         else if(ret_pid > 0){ // Parent Process
            wait(pid);    
     
            if(read(fd[0], stats_arr, STATSIZE*sizeof(int)) == -1){
               fprintf(stderr, "mywc_pf: %s\n", strerror(errno)); 
               exit(1);
            }
            if(line_cnt_flag){
               printf("%d ", stats_arr[0]);
            }
            if(word_cnt_flag){
               printf("%d ", stats_arr[1]);
            }
            if(byte_cnt_flag){
               printf("%d ", stats_arr[2]);
            }
            printf("%d\n", stats_arr[3]); 
         }
         else{ // ERROR
            fprintf(stderr, "mywc_pf: %s\n", strerror(errno)); 
         }
      }
   }
   // Else no option flags specified
   else{
      // If at least one file name was specified
      // Output line, word, and byte counts followed by the file name and child process id
      if(file_cnt){
         int line_total = 0;
         int word_total = 0;
         int byte_total = 0;

         int i;
         for(i=0; i<file_cnt; i++){
            int flen = strlen(filenames[i]);
            char *fname = malloc(flen*sizeof(char));
            strcpy(fname, filenames[i]); 

            pid_t ret_pid = fork();

            if(ret_pid == 0){ // Child Process
               FILE *fp;
               fp = fopen(filenames[i], "r");

               if(fp == NULL){
                  fprintf(stderr, "mywc_pf: %s: %s\n", filenames[i], strerror(errno)); 
                  exit(1);
               }
               else{
                  calculate_file_counts(fp);
                  fclose(fp);

                  if(write(fd[1], stats_arr, STATSIZE*sizeof(int)) == -1){
                     fprintf(stderr, "mywc_pf: %s\n", strerror(errno)); 
                     exit(1);
                  };
               }

               return;
            }
            else if(ret_pid > 0){ // Parent Process
               wait(pid);    
     
               if(read(fd[0], stats_arr, STATSIZE*sizeof(int)) == -1){
                  fprintf(stderr, "mywc_pf: %s\n", strerror(errno)); 
                  exit(1);
               }
               int i;
               for(i=0; i<STATSIZE-1; i++){
                  printf("%d ", stats_arr[i]);
               }
               line_total += stats_arr[0];
               word_total += stats_arr[1];
               byte_total += stats_arr[2];

               printf("%s ", fname); 
               free(fname);
               printf("%d\n", stats_arr[3]); 
            }
            else{ // ERROR
               fprintf(stderr, "mywc_pf: %s\n", strerror(errno)); 
            }
         }
         // Output line, word, and byte count totals if more than one file was specified
         if(file_cnt > 1){
            printf("%d ", line_total);
            printf("%d ", word_total);
            printf("%d ", byte_total);
            printf("total\n"); 
         }
      }
      // Else no file name so read from STDIN
      // Output line, word, and byte counts and child process id
      else{
         pid_t ret_pid = fork();

         if(ret_pid == 0){ // Child Process
            calculate_file_counts(stdin);

            if(write(fd[1], stats_arr, STATSIZE*sizeof(int)) == -1){
               fprintf(stderr, "mywc_pf: %s\n", strerror(errno)); 
               exit(1);
            };
            return;
         }
         else if(ret_pid > 0){ // Parent Process
            wait(pid);    
     
            if(read(fd[0], stats_arr, STATSIZE*sizeof(int)) == -1){
               fprintf(stderr, "mywc_pf: %s\n", strerror(errno)); 
               exit(1);
            }
            int i;
            for(i=0; i<STATSIZE; i++){
               printf("\t%d", stats_arr[i]);
            }
         }
         else{ // ERROR
            fprintf(stderr, "mywc_pf: %s\n", strerror(errno)); 
         }
      }
   }

   // Free pointers
   int i;
   for(i=0; i<file_cnt; i++){
      free(filenames[i]);
      filenames[i] = NULL;
   }
   free(filenames);
   filenames = NULL;

   return(0);
} // END OF main

//END
