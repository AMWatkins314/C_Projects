/*********************************************************
* PROGRAM NAME: calc_prog_perf.c
*
* USAGE: calc_prog_perf
*
* INPUT: The number of instructions then instructions in the form of rx=ry+rz where x, y and z are integers
* 
* OUTPUT:  The chart total cycles on a 6-stage pipeline
*          The chart total cycles on a 6-stage superscalar
*
* DESCRIPTION: Run the program and select option 1 to enter in the information, then select from options 2, 3 and 4. 
*          This program calculates the performance of a program with dependent arithmetic instructions, by simulating
*          the execution on (i) a simple 6-stage pipeline architecture (ii) a superscalar 6-stage architecture of 
*          degree 2 (2 sets of functional units).
*
*********************************************************/
#include <stdio.h>
#include <stdlib.h>

#define INSTRLEN 8

int num_instr  = 0; // The total number of instructions 

typedef struct instructions{
   int dest;
   int src1;
   int src2;
   int delay;
} INSTR;



/* Read in Parameters Number of Instructions and Actual Instructions to Pipeline */ 
void read_params(INSTR **s){
   char myinstr[INSTRLEN];

   printf("Enter total number of instructions: \n");
   scanf("%d", &num_instr);

   (*s) = (INSTR *)malloc( (1+num_instr) * sizeof(INSTR) ); 
   (*s)[0].dest = -1;

   int i;
   for(i = 1; i <= num_instr; i++){
      printf("%d) ", i);
      scanf("%s", myinstr);

      (*s)[i].dest = myinstr[1];
      (*s)[i].src1 = myinstr[4];
      (*s)[i].src2 = myinstr[7];  
   }

   return;
} // END OF read_params



/* Display the Chart */
void print_chart(INSTR **s){

   int i, j;
   for(i = 1; i <= num_instr; i++){
      printf("%d)", i);
      for(j = 1; j <= (*s)[i].delay; j++){
         printf("\t");
      }
      printf("FI\tDI\tCO\tFO\tEI\tWO\n");
   }

   return;
} // END OF print_chart



void calc_pipeline_cycles(INSTR **s){
   int delay = 0;
   int overlap = 0;

   (*s)[1].delay = 0;

   int i;
   for(i = 2; i <= num_instr; i++){
      delay = 0;

      // Check for dependencies 2-away 
      if( ((*s)[i-2].dest == (*s)[i].src1) || ((*s)[i-2].dest == (*s)[i].src2) ){
         if(overlap == 0){
            overlap = 1;
            delay = 1;
         }
         else{ //overlap = 1
            overlap = 0;
            delay = 0;
         }
      }
      else{
         overlap = 0;
      }
      // Check for dependencies between adjacent instructions 
      if( ((*s)[i-1].dest == (*s)[i].src1) || ((*s)[i-1].dest == (*s)[i].src2) ){
         delay = 2;
         overlap = 1;
      }
      (*s)[i].delay = delay + (*s)[i-1].delay + 1;
   }

   printf("Total number of cycles: %d\n", (*s)[num_instr].delay + 6);
   print_chart(s);

   return;
} // END of calc_pipeline_cycles



void calc_superscalar_cycles(INSTR **s){
   int delay = 0;
   int pipe_delay = 1;
   int overlap1 = 0;
   int overlap2 = 0;
   int overlap3 = 0;
   int overlap4 = 0;
   int overlap5 = 0;

   (*s)[1].delay = 0;

   int i;
   for(i = 2; i <= num_instr; i++){
      delay = 0;
      pipe_delay = 1 - pipe_delay;

      // Check for dependencies 5-away
      if(i >= 5){
         if(((*s)[i-5].dest == (*s)[i].src1) || ((*s)[i-5].dest == (*s)[i].src2)){
            if( (overlap1 == 0) && (overlap2 == 0) && (overlap3 == 0) && (overlap4 == 0) ){
               overlap5 = 1;
               if(pipe_delay == 1){
                  delay = 0;
               }
               else{
                  delay = 0;
               }
               pipe_delay = 1;
            }
            else{
               overlap5 = 0;
            }
         }
         else{
            overlap5 = 0;
         }
      }
      // Check for dependencies 4-away
      if(i >= 4){
         if(((*s)[i-4].dest == (*s)[i].src1) || ((*s)[i-4].dest == (*s)[i].src2)){
            if( (overlap1 == 0) && (overlap2 == 0) && (overlap3 == 0) ){
               overlap4 = 1;
               if(pipe_delay == 1){
                  delay = 1;
               }
               else{
                  delay = 0;
               }
               pipe_delay = 1;
            }
            else{
               overlap4 = 0;
            }
         }
         else{
            overlap4 = 0;
         }
      }
      // Check for dependencies 3-away
      if(i >= 3){
         if(((*s)[i-3].dest == (*s)[i].src1) || ((*s)[i-3].dest == (*s)[i].src2)){
            if( (overlap1 == 0) && (overlap2 == 0) ){
               overlap3 = 1;
               if(pipe_delay == 1){
                  delay = 1;
               }
               else{
                  delay = 1;
               }
               pipe_delay = 1;
            }
            else{
               overlap3 = 0;
            }
         }
         else{
            overlap3 = 0;
         }
      }
      // Check for dependencies 2-away
      if(i >= 2){
         if(((*s)[i-2].dest == (*s)[i].src1) || ((*s)[i-2].dest == (*s)[i].src2)){
            if(overlap1 == 0){
               overlap2 = 1;
               if(pipe_delay == 1){
                  delay = 2;
               }
               else{
                  delay = 1;
               }
               pipe_delay = 1;
            }
            else{
               overlap2 = 0;
            }
         }
         else{
            overlap2 = 0;
         }
      }
      // Check for adjacent dependencies
      if(i >= 1){
         if(((*s)[i-1].dest == (*s)[i].src1) || ((*s)[i-1].dest == (*s)[i].src2)){
            overlap1 = 1;
            delay = 2;
            pipe_delay = 1;
         }
      }
      else{
         overlap1 = 0;
      }

      (*s)[i].delay = delay + (*s)[i-1].delay + pipe_delay;
   }
   printf("Total number of cycles: %d\n", (*s)[num_instr].delay + 6);
   print_chart(s);

   return;
} // END of calc_superscalar_cycles



/* Display Menu of Options for User */ 
void display_menu(int *option){
   printf("\nPipelined/Superscalar Instruction Performance:\n");
   printf("------------------------------------------------\n");
   printf("1) Enter instructions\n");
   printf("2) Calculate/chart total cycles on a 6-stage pipelined architecture\n");
   printf("3) Calculate/chart total cycles on a 6-stage superscalar architecture\n");
   printf("4) Quit\n\n");
   printf("Enter selection: ");

   scanf("%d", option);

   return;
} // END OF display_menu



/* MAIN Function */
int main() {
   int user_opt = 0; // User's chosen option initialized to 0 so menu displays the first time
   INSTR *set = NULL;

   // Read in the user's chosen option and perform the corresponding operation 
   // until the user decides to quit the program by selecting option 3
   do {
      // Switch on the User's chosen option
      // If the option is not valid, output the Menu again
      switch(user_opt){
         case 1: read_params(&set);
                 display_menu(&user_opt);
            break;
         case 2: calc_pipeline_cycles(&set); 
                 display_menu(&user_opt);
            break;
         case 3: calc_superscalar_cycles(&set); 
                 display_menu(&user_opt);
            break;
         default: display_menu(&user_opt);
            break;
      }
   } while(user_opt != 4);

   if(set){
      free(set);
      set = NULL;
   }

   return(0); 
} // END OF main 

// EOF
