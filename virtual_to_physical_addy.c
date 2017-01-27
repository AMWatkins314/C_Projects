/*********************************************************
* PROGRAM NAME: virtual_to_physical_addy.c
*
* USAGE: virtual_to_physical_addy
*
* INPUT: int Main Memory Size (in words)
*        int Page Size (words/page)
*        int Replacement Policy (0 = Least Recently Used, 1 = First In First Out
*        int Virtual Memory Address to Access 
* 
* OUTPUT: Page fault or the physical address that the virtual memory address maps to.
*         The Page Table. 
*
* DESCRIPTION: Program to translate a virtual address to a physical address,
*         involving a custom sized fully associative page table.
*         User is presented with options to enter parameters (Main Memory Size, 
*         Page Size, Replacement Policy). 
*         User can then choose to access a Virtual Memory Address or
*         print the Page Table or Quit. 
*
*********************************************************/
#include <stdio.h>
#include <stdlib.h>


typedef struct page_table_row{
   int virtual_page;
   int page_frame;
} ROW;




/* Read in Parameters Main Memory Size, Page Size, and Replacement Policy */ 
int read_params(ROW **pt, int *mm_sz, int *p_sz, int *rp){
   int num_entries = 0;
   int i = 0;

   printf("\nEnter main memory size (words): ");
   scanf("%d", mm_sz);

   printf("Enter page size (words): ");
   scanf("%d", p_sz);
  
   printf("Enter replacement policy (0=LRU, 1=FIFO): ");
   scanf("%d", rp);

   num_entries = (*mm_sz)/(*p_sz);

   // Deallocate old page table if it exists
   if(*pt){
      free(*pt);
      *pt = NULL;
   }

   // Allocate memory for the page table
   (*pt) = (num_entries) ? (ROW *)malloc(num_entries*sizeof(ROW)) : NULL;

   // Initialize the page table entries
   for(i=0; i<num_entries; i++){
      ((*pt)[i]).virtual_page = -1; // initialize to -1 to indicate not used
   }

   return(num_entries);
} // END OF read_params




/* Map the virtual address and find where it is in the page table or add it using the replacement policy */
void map_virtual_address(ROW **pt, int p_sz, int num_entries, int rp){
   int v_page    = 0;
   int vm_addy   = 0;
   int index     = 0;

   printf("\nEnter virtual memory address to access: ");
   scanf("%d", &vm_addy);

   v_page = vm_addy/p_sz;

   /*
   * Check page table for hit or miss
   * index<num_entries means don't go beyond the end of the table
   * (*pt)[index].virtual_page == -1 means you hit the empty pages so virtual page doesn't exist in the page table
   * (*pt)[index].virtual_page != v_page means you hit a non-empty page and it doesn't match the virtual page
   */
   while( (index<num_entries) && ((*pt)[index].virtual_page != -1) && ((*pt)[index].virtual_page != v_page) ){
      index++; // keep searching the page table to see if v_page is in it
   }

   // CASE 1: MISS - You searched the entire full table and did not find the virtual page
   if(index == num_entries){
      printf("Page fault!\n");

      // Put the new virtual page in the last spot in the table after popping off the first entry
      int pg_frame = ((*pt)[0]).page_frame;

      int i;
      for(i=0; i<(num_entries-1); i++){
         ((*pt)[i]).virtual_page = ((*pt)[(i+1)]).virtual_page;
         ((*pt)[i]).page_frame   = ((*pt)[(i+1)]).page_frame;
      } 
      ((*pt)[(num_entries-1)]).virtual_page = v_page;
      ((*pt)[(num_entries-1)]).page_frame   = pg_frame;
   }   
   // CASE 2: MISS - You searched the table until hitting the empty spots and did not find the virtual page
   else if(((*pt)[index]).virtual_page == -1){
      printf("Page fault!\n");

      // Put the new virtual page in the first empty spot int the page table (which is at index)
      ((*pt)[index]).virtual_page = v_page;
      ((*pt)[index]).page_frame   = index;
   }
   // CASE 3: HIT  - You found the virtual page when searching the page table so (*pt)[index].virtual_page == v_page 
   else{
      int i = index;
      // Use the replacement policy to move the virtual page to the correct place in the page table
      if(rp == 1) {
        /* Do nothing for FIFO replacement policy */
      }
      if(rp == 0){
         /* Implement LRU replacement policy */   
         int pg_frame = ((*pt)[index]).page_frame;

         for(i=index; i<(num_entries-1); i++){
            if(((*pt)[i]).virtual_page != -1){
               (*pt)[i] = (*pt)[(i+1)];
            }
            else{
               break;
            }
         }   
         ((*pt)[i]).virtual_page = v_page;
         ((*pt)[i]).page_frame   = pg_frame;
      }

      int offset = vm_addy%p_sz; 
      int phys_addy = ((((*pt)[i]).page_frame)*p_sz) + offset;

      printf("\nVirtual address %d maps to physical address %d\n", vm_addy, phys_addy);
   }
   
   return;
} // END OF map_virtual_address



/* Display the Page Table */
void print_page_table(ROW *pt, int num_entries){
   int i = 0;

   // Make sure the page table exists before reading from it
   if(pt != NULL){
      for(i=0; i<num_entries; i++){
         if((pt[i]).virtual_page != -1){
            printf("VP %d --> PF %d\n", (pt[i]).virtual_page, (pt[i]).page_frame); 
         }
         // Don't output the empty page table entries
         else{
            break;
         }
      }
   }

   return;
} // END OF print_page_table



/* Display Menu of Options for User */ 
void display_menu(int *option){
   printf("\nVirtual address mapping:\n");
   printf("------------------------\n");
   printf("1) Enter parameters\n");
   printf("2) Map virtual address\n");
   printf("3) Print page table\n");
   printf("4) Quit\n\n");
   printf("Enter selection: ");

   scanf("%d", option);

   return;
} // END OF display_menu



/* MAIN Function */
int main() {
   int user_opt     = 0; // User's chosen option initialized to 0 so menu displays the first time
   int mainmem_size = 0;
   int page_size    = 0;
   int r_policy     = 0; // The replacement policy to use, 0=LRU and 1=FIFO
   int entries_cnt  = 0; // The total number of entries in the page table

   ROW *page_table  = NULL;

   // Read in the user's chosen option and perform the corresponding operation 
   // until the user decides to quit the program by selecting option 3
   do {
      // Switch on the User's chosen option
      // If the option is not valid, output the Menu again
      switch(user_opt){
         case 1: entries_cnt = read_params(&page_table, &mainmem_size, &page_size, &r_policy);
                 display_menu(&user_opt);
            break;
         case 2: map_virtual_address(&page_table, page_size, entries_cnt, r_policy); 
                 display_menu(&user_opt);
            break;
         case 3: print_page_table(page_table, entries_cnt); 
                 display_menu(&user_opt);
            break;
         default: display_menu(&user_opt);
            break;
      }
   } while(user_opt != 4);

   if(page_table){
      free(page_table);
      page_table = NULL;
   }

   return(0); 
} // END OF main 

// EOF
