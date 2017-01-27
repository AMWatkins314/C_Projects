/*********************************************************
* PROGRAM NAME: check_hamcode.c
*
* USAGE: check_hamcode
*
* INPUT: 1) The maximum length of the Hamming Code
*        2) The parity of the check bits (even=0, odd=1)
*        3) The Hamming Code  
* 
* OUTPUT: 1) The erroneous bit (if any) 
*         2) The corrected Hamming Code (if there was an error)
*
* DESCRIPTION: A program to check a Hamming Code for a single-bit error based on choosing from a menu of choices.
*         The choices are: 1) Enter Parameters, 2) Check Hamming Code, 3) Quit Program.
*
*********************************************************/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Read in the max length of the Hamming code and the parity to use */
void read_params(int *ml, int *p){
   printf("Enter the maximum length of the Hamming code: ");
   scanf("%d", ml);

   printf("Enter the parity to use (0=even, 1=odd): ");
   scanf("%d", p);
  
   return;
} // END OF read_params


/* Read in Hamming code and check for errors - if there are errors, output where and the corrected code */
void check_hamcode(char **hc, int p){
   int hlen   = 0; // The actual length of the Hamming code entered
   int pb_cnt = 0; // The number of parity bits
   int p_val  = p; // The parity type which will keep getting XOR'd to find the parity bit value
   int err_p  = 0; // The position of the error, if any
   int i, j, k = 0;

   printf("Enter the Hamming code: ");
   scanf("%s", *hc);
 
   hlen = strlen(*hc);

   if(!hlen){
      printf("ERROR: Hamming code string must not be empty.\n");
      return;
   }

   pb_cnt = (int)ceil( log(hlen)/log(2.0) );

   /* For each start parity bit (power of 2) */ 
   for(i=1; i<hlen; i*=2){
      p_val = p; // Reset for each Parity bit calculation
      /* For each starting block of length i */
      for(j=i; j<=hlen; j+=(2*i)){
         /* For each bit in the block */
         for(k=j; (k<(j+i) && k<=hlen); k++){
            // Check Parity
            p_val ^= (((*hc)[hlen-k]) - '0');
         } 
      } 
      err_p += (p_val*i);
   }

   if(err_p){
      ((*hc)[hlen-err_p]) = ((*hc)[hlen-err_p] == '1') ? '0' : '1'; // Incorrect bit, so flip it
      printf("There is an error in bit: %d\n", err_p);
      printf("The corrected Hamming code is: %s\n", (*hc));
   }
   else{
      printf("There is no bit error.\n");
   }
 
   return;
} // END of check_hamcode


/* Display Menu of Options for User */ 
void display_menu(int *option){
   printf("\nHamming Code Checker:\n");
   printf("---------------------\n");
   printf("1) Enter parameters\n");
   printf("2) Check Hamming code\n");
   printf("3) Quit\n");
   printf("Enter selection: ");

   scanf("%d", option);

   return;
} // END OF display_menu


int main(){
   int user_opt = 0; // User's chosen option initialized to 0 so menu displays the first time
   int max_hlen = 0; // The maximum length the Hamming code can be
   int parity   = 0; // The parity to use for the Hamming code, 0=even and 1=odd
   char *hcode  = NULL; // The Hamming code

   // Read in the user's chosen option and perform the corresponding operation 
   // until the user decides to quit the program by selecting option 3
   do {
      // Switch on the User's chosen option
      // If the option is not valid, output the Menu again
      switch(user_opt){
         case 1: read_params(&max_hlen, &parity);
                 if(hcode){
                   free(hcode);
                   hcode=NULL;
                 }
                 if(max_hlen){
                    hcode = (char *)malloc(max_hlen * sizeof(char));
                 }
                 display_menu(&user_opt);
            break;
         case 2: check_hamcode(&hcode, parity);
                 display_menu(&user_opt);
            break;
         default: display_menu(&user_opt);
            break;
      }
   } while(user_opt != 3);

   if(hcode){
      free(hcode);
      hcode = NULL;
   }

   return(0); 
} // END OF main 

// EOF
