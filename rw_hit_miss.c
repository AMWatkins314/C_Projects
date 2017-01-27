/*********************************************************
* PROGRAM NAME: rw_hit_miss.c
*
* USAGE: rw_hit_miss
*
* INPUT:  Int - Main Memory Size
*         Int - Cache Size
*         Int - Cache Block Size
*         Int - Main Memory Address to read from/write to
*         Int - 0 to read from or 1 to write to the cache
*         Int - The value to write to cache if writing
* 
* OUTPUT: Whether there was a read/write hit or miss.
*         Also, what the given Cache word, block, tag, and value is for the specified main memory address. 
*
* DESCRIPTION: A program to simulate reading and writing to a custom-sized
*              direct-mapped cache involving a custom-sized main memory. 
*
*********************************************************/
#include <stdio.h>
#include <stdlib.h>


typedef struct cache_row{
   int tag;
   int *block;
} CACHE_ROW;

/* Read in Parameters Main Memory Size, Cache Size, and Block Size and create Main Memory and Cache */
void read_params(int **mm, CACHE_ROW **c, int *mm_sz, int *c_sz, int *b_sz, int *c_row_cnt){
   int i  = 0;
   *mm_sz = 0;
   *c_sz  = 0;
   *b_sz  = 0;
   *c_row_cnt = 0;

   if(*mm){
      free(*mm);
      *mm = NULL;
   }
   if(*c && *c_row_cnt>0){
      for(i=0; i<(*c_row_cnt); i++){
         if(((*c)[i]).block){
            free(((*c)[i]).block);   
            ((*c)[i]).block = NULL;
         }
      }
      free(*c);
      *c = NULL;
   }

   printf("Enter main memory size (words): ");
   scanf("%d", mm_sz);

   printf("Enter cache size (words): ");
   scanf("%d", c_sz);
  
   printf("Enter block size (words/block): ");
   scanf("%d", b_sz);

   // Initialize Main Memory
   (*mm) = (*mm_sz) ? (int *)malloc((*mm_sz)*sizeof(int)) : NULL;

   for(i=0; i<(*mm_sz); i++){
      ((*mm)[i]) = (*mm_sz)-i;
   }
   
   // Initialize Cache
   (*c_row_cnt) = (*b_sz) ? (*c_sz)/(*b_sz) : 0;
   (*c) = (*c_row_cnt) ? (CACHE_ROW *)malloc((*c_row_cnt)*sizeof(CACHE_ROW)) : NULL; 

   for(i=0; i<(*c_row_cnt); i++){
      ((*c)[i]).tag = -1; // initialize to an invalid tag value to indicate empty
      ((*c)[i]).block = NULL;
   }   

   return;
} // END OF read_params


/* Read From or Write to Cache */
void read_write_cache(int **mm, CACHE_ROW **cache, int mm_sz, int c_sz, int b_sz){
   int selection = -1; 
   int mm_addr   = 0;
   int write_val = 0;
   int word      = 0;
   int block     = 0;
   int tag       = 0;
   int i         = 0;

   if(!mm_sz || !c_sz){
      printf("Error: Size of Main Memory and/or Cache is 0\n");
      return;
   }

   printf("Select read (0) or write (1): ");
   scanf("%d", &selection); 
  
   // READ FROM CACHE
   if(selection == 0){
      printf("Enter main memory address to read from: ");
      scanf("%d", &mm_addr);

      if(mm_addr > mm_sz){
         printf("Error: Main Memory Address entered is greater than the size of the Main Memory\n");
         return;
      }
      if(b_sz > c_sz){
         printf("Error: Block Size is greater than Cache Size\n");
         return;
      }

      word  = (b_sz) ? mm_addr%b_sz : 0;
      block = (b_sz) ? (mm_addr%c_sz)/b_sz : 0;
      tag   = (c_sz) ? mm_addr/c_sz : 0; 

      // Read Hit
      if(((*cache)[block]).tag == tag){
         printf("Read hit!\n");
      }
      // Read Miss
      else{
         printf("Read miss!\n");

         // Allocate if uninitialized
         if(((*cache)[block]).tag == -1){
            ((*cache)[block]).block = (int *)malloc((word+1)*sizeof(int));
         }
         int base_addr = mm_addr-word;

         // Update Cache from Main Memory
         for(i=0; i<b_sz; i++){
            ((*cache)[block]).block[i] = ((*mm)[base_addr+i]);
         } 
         ((*cache)[block]).tag = tag;
      }
   }
   // WRITE TO CACHE
   else if(selection == 1){
      printf("Enter main memory address to write to: ");
      scanf("%d", &mm_addr);
      printf("Enter value to write: ");
      scanf("%d", &write_val);

      word  = (b_sz) ? mm_addr%b_sz : 0;
      block = (b_sz) ? (mm_addr%c_sz)/b_sz : 0;
      tag   = (c_sz) ? mm_addr/c_sz : 0; 

      // Write Hit
      if(((*cache)[block]).tag == tag){
         printf("Write hit!\n");
      }
      // Write Miss
      else{
         printf("Write miss!\n");

         // Allocate if uninitialized
         if(((*cache)[block]).tag == -1){
            ((*cache)[block]).block = (int *)malloc((word+1)*sizeof(int));
         } 
         else{
            int base_addr = mm_addr-word;

            // Update Cache from Main Memory 
            for(i=0; i<b_sz; i++){
               ((*cache)[block]).block[i] = ((*mm)[base_addr+i]);
            } 
         }
         ((*cache)[block]).tag = tag;
      }
      // Write the new value to Cache
      ((*cache)[block]).block[word] = write_val;

      // Write the new value through from Cache to Main Memory
      ((*mm)[mm_addr]) = write_val;
   }
   else{
      printf("Bad selection made. Returning to Main Menu."); 
      return;
   }

   // Output the information about the Cache
   printf("Word %d of block %d with tag %d contains value %d\n", word, block, tag, ((*cache)[block]).block[word]);

   return;
}


/* Display Menu of Options for User */ 
void display_menu(int *option){
   printf("\nCache memory allocation and mapping:\n");
   printf("----------------\n");
   printf("1) Enter parameters\n");
   printf("2) Access cache for reading/writing and transfer data\n");
   printf("3) Quit\n");
   printf("Enter selection: ");

   scanf("%d", option);

   return;
} // END OF display_menu


int main() {
   int user_opt        = 0; // User's chosen option initialized to 0 so menu displays the first time
   int mainmem_size    = 0;
   int cache_size      = 0;
   int cblock_size     = 0;
   int cache_row_count = 0;

   int *main_memory = NULL;
   CACHE_ROW *cache = NULL;

   // Read in the user's chosen option and perform the corresponding operation 
   // until the user decides to quit the program by selecting option 3
   do {
      // Switch on the User's chosen option
      // If the option is not valid, output the Menu again
      switch(user_opt){
         case 1: read_params(&main_memory, &cache, &mainmem_size, &cache_size, &cblock_size, &cache_row_count);
                 display_menu(&user_opt);
            break;
         case 2: read_write_cache(&main_memory, &cache, mainmem_size, cache_size, cblock_size);
                 display_menu(&user_opt);
            break;
         default: display_menu(&user_opt);
            break;
      }
   } while(user_opt != 3);

   if(main_memory){
      free(main_memory);
      main_memory = NULL;
   }
   if(cache){
      int i;
      for(i=0; i<cache_row_count; i++){
         if(cache[i].block){
            free(cache[i].block);   
            cache[i].block = NULL;
         }
      }
      free(cache);
      cache = NULL;
   }

   return(0); 
} // END OF main 

// EOF
