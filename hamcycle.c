/*************************************
 * Name: hamcycle.c
 *
 * Input: A file containing an adjacency matrix.
 *
 * Output: Whether the adjacency matrix contains a Hamiltonian Cycle, and if so, the cycle path.
 *
 * Description: This program reads an adjacency matrix for a graph from a file and determines
 *         whether the graph has a Hamiltonian Cycle.
 *         Note that the algorithm does not necessarily run in polynomial time.
 *
 * **********************************/
#include <stdio.h>
#include <stdlib.h>

#define MAXSIZE 4096
#define TRUE 1
#define FALSE 0





void print_hamcycle(int *path, int nodes){
   printf("Yes, there is a Hamiltonian Cycle: ");
   int i;
   for(i=0; i<nodes; i++){
      printf("%d", path[i]+1);
   } 
   printf(".\n");
   
   return;
} // END OF print_hamcycle



int can_add_node(int node, int **graph, int nodes, int *path, int position){
   // Check if the node is connected to the previously added node
   if(graph[path[position-1]][node] == 0){
      return(FALSE);
   }

   // Check if the node is already in the path since can't include it twice
   int i;
   for(i=0; i<position; i++){
      if(path[i] == node){
         return(FALSE);
      }
   }

   return(TRUE);
} // END OF can_add_node



int check_hamcycle(int **graph, int nodes, int *path, int position){
   // If the last position is the size of the graph = path contains all nodes
   if(position == nodes){
      // If the last vertex in the path is connected to the first vertex (always 0)
      if(graph[path[position-1]][path[0]] == TRUE){
         // Return that there is a Hamiltonian Cycle
         return(TRUE);
      }
      else{
         return(FALSE);
      }
   }

   // Else keep checking nodes in the graph to find out which one connects to node 0
   int i;
   for(i=1; i<nodes; i++){
      if(can_add_node(i, graph, nodes, path, position)){
         path[position] = i;

         // Recurse to build path
         if(check_hamcycle(graph, nodes, path, position+1)){
            return(TRUE);
         }
         // Adding node didn't lead to Hamiltonian Cycle, so remove it from path
         else{
            path[position] = -1;
         }
      }
   }
   // No vertex can be added to the path
   return(FALSE);
}



int find_hamcycle(int **graph, int nodes){
   int path[nodes];
   int i;
   for(i=0; i<nodes; i++){
      path[i] = -1;
   }
   // Pick 0 as the first node to check
   path[0] = 0;

   if(check_hamcycle(graph, nodes, path, 1) == FALSE){
     printf("No, there is no Hamiltonian Cycle.\n");
     return(FALSE); 
   }
   else{
      print_hamcycle(path, nodes);
   }

   return(TRUE);
} // END OF find_hamcycle



int main(int argc, char *argv[]){
   FILE *fp;
   char line[MAXSIZE]; // buffer to hold the current file line using POSIX suggested max size
   int **graph = malloc(MAXSIZE * sizeof(int));
   int row = 0;
   int column = 0;

   if(argv[1]){
      printf("Reading %s from file.\n", argv[1]);
      fp = fopen(argv[1], "r");
      if(fp == NULL){
         printf("Error opening %s for reading.\n", argv[1]);
         exit(1);
      }
      while( fgets(line, sizeof(line), fp) ){
         char *nodelist = line;
         int node, n;
         graph[row] = malloc(MAXSIZE * sizeof(int));
 
         column = 0;
         while(sscanf(nodelist, "%d%n", &node, &n) == 1){
            graph[row][column] = node; 
            nodelist += n;
            column++;
         }
         row++;
      } 
      fclose(fp);
   }
   else{
      printf("This program requires that the file name containing an adjacency matrix be given as an argument.\n");
      exit(1);
   }

   find_hamcycle(graph, row);  

   return(0);
} // END OF main
