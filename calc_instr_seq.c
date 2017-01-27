/*********************************************************
* PROGRAM NAME: calc_instr_seq.c
*
* USAGE: calc_instr_seq
*
* INPUT: The number of the menu option desired (1 - 5).
*        If option 1 is chosen, user enters:
*        1) The number of instruction classes
*        2) The frequency of the machine in MHz for each class
*        3) The CPI for each class 
* 
* OUTPUT: Depends on the option.
*         Option 2: Outputs the average CPI of a sequence of instructions
*         Option 3: Outputs the total execution time of a sequence of instructions
*         Option 4: Outputs the MIPS of a sequence
*
* DESCRIPTION: This program calculates the output based on choosing from a menu of choices,
*              where each choice calls the appropriate procedure, where the choices are:
*              1) Enter parameters
*              2) Calculate average CPI of a sequence of instructions
*              3) Calculate total execution time of a sequence of instructions
*              4) Calculate MIPS of a sequence
*              5) Quit
*
* NOTES: Average CPI = Sum(CPI_i * Instruction_i) / Instructions_total 
*                    = Cycles_total / Instructions_total
*        Total Execution Time = Instructions_total * CPI_avg * Cycle_time
*                             = Instructions_total * (CPI_avg / Frequency) 
*                             = 1000 * Cycles_total / Frequency 
*        MIPS = Instructions_total / (Execution_Time_total / 10^6)
*             = Instructions_total / (Cycles_total / Frequency)  
*        
*********************************************************/

#include <stdio.h>
#include <stdlib.h>


/* Read in Parameters */
void read_params(int *freq, int *num_cycles, int *num_instr){
   int instr_class_cnt = 0; // Instruction Class Count - entered by user

   // Reset for new data set
   *num_cycles = 0;
   *num_instr = 0;

   printf("Enter the number of instruction classes: ");
   scanf("%d", &instr_class_cnt);

   printf("Enter the frequency of the machine (MHz): ");
   scanf("%d", freq);
  
   int index;
   for(index=1; index<=instr_class_cnt; index++){
      int cpi = 0;
      int instr_cnt = 0;

      printf("Enter CPI of class %d: ", index);
      scanf("%d", &cpi);

      printf("Enter instruction count of class %d (millions):", index);
      scanf("%d", &instr_cnt);

      *num_instr += instr_cnt;
      *num_cycles += (cpi * instr_cnt); 
   }

   return;
}

/* Calculate Average CPI */
void calc_avg_cpi(int *num_cycles, int *num_instr){
   float avg_cpi = (*num_instr != 0) ? ( (float)(*num_cycles)/(float)(*num_instr) )
                                     : 0.00;
   printf("The average CPI of the sequence is: %.2f\n", avg_cpi);

   return;
}

/* Calculate Total Execution Time */
void calc_total_exec_time(int *freq, int *num_cycles){
   float total_exec_time = (*freq != 0) ? ( 1000.00*(float)(*num_cycles)/(float)(*freq) )
                                        : 0.00;
   printf("The total CPU time of the sequence is: %.2f msec\n", total_exec_time);

   return;
}

/* Calculate MIPS */
void calc_mips(int *freq, int *num_cycles, int *num_instr){
   float mips = (*num_cycles != 0) ? ((float)(*num_instr)*(float)(*freq)/(float)(*num_cycles))
                                   : 0.00;
   printf("The total MIPS of the sequence is: %.2f\n", mips);

   return;
}


/* Display Menu of Options for User */ 
void display_menu(int *option){
   printf("\nMenu of Options:\n");
   printf("----------------\n");
   printf("1) Enter parameters\n");
   printf("2) Calculate average CPI of a sequence of instructions\n");
   printf("3) Calculate total execution time of a sequence of instructions\n");
   printf("4) Calculate MIPS of a sequence of instructions\n");
   printf("5) Quit\n\n");
   printf("Enter selection: ");

   scanf("%d", option);

   return;
}


int main() {

   int user_opt = 0; // User's chosen option initialized to 0 so menu displays the first time
   int frequency = 0;
   int total_cycles = 0;
   int total_instructions = 0;

   // Read in the user's chosen option and perform the corresponding operation 
   // until the user decides to quit the program by selecting option 5 
   do {
      // Switch on the User's chosen option
      // If the option is not valid, output the Menu again
      switch(user_opt){
         case 1: read_params(&frequency, &total_cycles, &total_instructions);
                 display_menu(&user_opt);
            break;
         case 2: calc_avg_cpi(&total_cycles, &total_instructions);
                 display_menu(&user_opt);
            break;
         case 3: calc_total_exec_time(&frequency, &total_cycles);
                 display_menu(&user_opt);
            break;
         case 4: calc_mips(&frequency, &total_cycles, &total_instructions);
                 display_menu(&user_opt);
            break;
         default: display_menu(&user_opt);
            break;
      }
   } while(user_opt != 5);

   return(0); 
} // end main()

// EOF
