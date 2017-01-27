/*********************************************************
* PROGRAM NAME: LineNumber.c
*
* INPUT: 
*     (OPTIONAL) STRING: The name of the file to output line-by-line,
*                        with the line numbers prepended.  
* OUTPUT:
*     File or STDIN content pre-pended by line numbers.
*
* DESCRIPTION: This program reads in a file from the keyboard or, if string arguments are 
*              given, treats each string as a file name and reads those files if they exist.
*              Output is the keyboard input or file content, with each content line pre-
*              pended by the line number.
*********************************************************/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
   char line[4096]; // buffer to hold the current line using POSIX suggested max size
   int linenum = 1; // the current line number 

   // Determine whether reading from a file or from STDIN
   // If only the executable name was given on the command line then argc=1 so read from STDIN
   if (argc == 1){
      char *output = malloc(sizeof(char) * 1024); // allocate memory to hold the output
      sprintf(output, "\n");

      // Read from STDIN until the user enters CTRL-D aka End of File (EOF)
      while(!feof(stdin)) {
         while( fgets(line, sizeof(line), stdin) != NULL){
            sprintf(output, "%s%d\t%s", output, linenum, line);
            linenum++;
         }
      }
      printf("%s\n", output); 
      free(output);
   }
   // Else there are arguments so treat them as filenames and read from them if they exist
   else if(argc > 1){
      int index = 1; // index to loop through the file name arguments
      char *filename; // use to point to the location in memory of the filenames

      // Loop through the file names
      // Note: index starts at 1 since argv[0] is the program name
      for(index=1; index<argc; index++) {
         filename = argv[index];
         printf("***** %s *****\n", filename);

         FILE *fp;
         fp = fopen(filename, "r");

         if(fp == NULL){
            fprintf(stderr, "LineNumber: %s: %s\n", filename, strerror(perror));
         }
         // If the file open was successful, read the file line by line until EOF
         // Output each line with a line number pre-pended 
         else{
            while( fgets(line, sizeof(line), fp) ){
               printf("%d\t%s", linenum, line); 
               linenum++;
            }
            printf("\n");
            // Reset the linenum back to 1
            linenum = 1;

            fclose(fp);
         }
      }
   }
   else{
      printf("ERROR: argc < 1");
   }
   return(0);
}

//END
