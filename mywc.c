/*********************************************************
* PROGRAM NAME: mywc.c
*
* INPUT: 
*     (OPTIONAL) STRING: The name of the file to perform the word/line/byte counts on. 
*
* OUTPUT: The same as Unix's wc command for the c, l, and w options for 0+ files. 
*
* DESCRIPTION: Simulate Unix's wc command with the c, l, and w options for 0+ files. 
*********************************************************/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int stats_arr[3] = {0, 0, 0}; // Holds line, word, and byte counts to return

/* Calculate the number of lines, bytes, and words in the file or stdin */
void calculate_file_counts(FILE *stream){
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
   stats_arr[0] = line_cnt;
   stats_arr[1] = word_cnt;
   stats_arr[2] = byte_cnt;

   return;
} //END OF calculate_file_counts

int main(int argc, char *argv[]){
   int line_cnt_flag = 0;
   int word_cnt_flag = 0;
   int byte_cnt_flag = 0;

   int arg_cnt  = 0;
   int file_cnt = 0;

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
                  printf("mywc: invalid option -- '%c'\n", *argstr);
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
      // Output the values corresponding to the flags followed by the file name
      if(file_cnt){
         int line_total = 0;
         int word_total = 0;
         int byte_total = 0;

         int i;
         for(i=0; i<file_cnt; i++){
            FILE *fp;
            fp = fopen(filenames[i], "r");

            if(fp == NULL){
               fprintf(stderr, "mywc: %s: %s\n", filenames[i], strerror(errno)); 
            }
            else{
               calculate_file_counts(fp);
    
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
               printf("%s\n", filenames[i]); 

               fclose(fp);
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
      // Output the values corresponding to the flags 
      else{
         calculate_file_counts(stdin);
    
         if(line_cnt_flag){
            printf("%d ", stats_arr[0]);
         }
         if(word_cnt_flag){
            printf("%d ", stats_arr[1]);
         }
         if(byte_cnt_flag){
            printf("%d ", stats_arr[2]);
         }
         printf("\n");
      }
   }
   // Else no option flags specified
   else{
      // If at least one file name was specified
      // Output line, word, and byte counts followed by the file name
      if(file_cnt){
         int line_total = 0;
         int word_total = 0;
         int byte_total = 0;

         int i;
         for(i=0; i<file_cnt; i++){
            FILE *fp;
            fp = fopen(filenames[i], "r");

            if(fp == NULL){
               fprintf(stderr, "mywc: %s: %s\n", filenames[i], strerror(errno)); 
            }
            else{
               calculate_file_counts(fp);
    
               line_total += stats_arr[0];
               word_total += stats_arr[1];
               byte_total += stats_arr[2];

               printf("%d ", stats_arr[0]);
               printf("%d ", stats_arr[1]);
               printf("%d ", stats_arr[2]);
               printf("%s\n", filenames[i]); 

               fclose(fp);
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
      // Output line, word, and byte counts
      else{
         calculate_file_counts(stdin);
    
         printf("\t%d", stats_arr[0]);
         printf("\t%d", stats_arr[1]);
         printf("\t%d\n", stats_arr[2]);
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
}

//END
