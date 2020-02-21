// Generates a map and allows the play to move through it.
// To descend to a new map, store the current map and generate a new one. 
// To ascend to previous map, load the old one and store the previous map.
// Saving and loading has been tested & it works..
// Each map has one portal. So it is a linear downward map.

// Plan: test saving/loading array of game states with only position and map
//       , then expand to all other variables (such as portalx, portaly, etc)

// When entering a new level, update the levels list with the current level and generate a new one.

// There is always an up-staircase at the center of each level

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define AUTORUN 0  // If set to 1, bot will auto-explore the map
#define AUTORUN_LENGTH 50000 // How many turns to auto-explore

#define NORTH 0
#define SOUTH 1
#define EAST 2
#define WEST 3
#define HEIGHT 20
#define WIDTH 70
#define DEPTH 100

#define EXPLORED 2

struct level{
  int map[HEIGHT][WIDTH];
  int x,y;
  int generated; // If 0, it is empty. If 1, it has already been partially explored
  int portal_exists;
  int portalx,portaly;
  int prev_level, next_level; // Index into array of levels, 
};

void clear_screen(void){
  int counter = 100;
  while(counter--) printf("\n");
  return;
}


void print_dir(int dir)
{
    char *string;
    if(dir == NORTH){
        string = "North";
    } else if(dir == SOUTH){
        string = "South";
    } else if(dir == WEST){
        string = "West";
    } else if(dir == EAST){
        string = "East";
    };
    printf("%s",string);
    return;
}

void print_map(int map[HEIGHT][WIDTH], int playerx, int playery, int portalx, int portaly)
{
    int ix,iy;
    for(iy = 0;iy < HEIGHT;iy++){
        for(ix = 0;ix < WIDTH;ix++){
            if((iy == playery) && (ix == playerx)){
                printf("@");
            } else if(map[iy][ix] == 1){
                printf(".");
            } else if(map[iy][ix] == EXPLORED){
                if((ix == portalx) && (iy == portaly)){
                  printf(">");
                } else if ((ix == (WIDTH/2)) && (iy == (HEIGHT/2))){
                 printf("<");
                } else {
                  printf("0");
                } 
            } else {
		            printf(" ");
            }
	    };
          //  printf("%d",map[iy][ix]);
        
        printf("\n");
    }
    return;
}

void load_savegame(struct level levels[DEPTH])
{
  FILE *save;
  save = fopen("save","rb");
  int success = 0;
  success = fread (levels , sizeof(struct level), DEPTH, save); // load the map 
  printf("success = %d",success);
  fclose(save);
  return;
}

int main(int argc, char **argv)
{
    system("/bin/stty -ignbrk -brkint -ignpar -parmrk -inpck -istrip -inlcr -igncr -icrnl -ixon -ixoff -iuclc -imaxbel -isig -icanon -echo min 1 time 0");
    // All stty _input_ options included in "raw" mode 

    srand(time(NULL));
    FILE *save;
    int i,ix,iy,x,y,pathx,pathy,newx,newy,portal_exists,portalx,portaly,curr_depth,maxdepth;
    int next_level,prev_level;
    next_level = prev_level = 0;
    curr_depth = 0; // The current Z-level of the player, which # level player is on
    maxdepth = 1;
    portal_exists = 0;
    portalx = portaly = 0;
    x = WIDTH/2;
    y = HEIGHT/2;
    pathx = newx = x;
    pathy = newy = y;
    char input = ' ';
    int dir = 0;
    int map[HEIGHT][WIDTH] = {{0}};
    int counter = AUTORUN_LENGTH;

    struct level levels[DEPTH];
    for(i = 0;i < DEPTH; i++){ // Initialize empty levels
      for(iy = 0;iy < HEIGHT;iy++){
        for(ix = 0;ix < WIDTH;ix++){
          levels[i].map[iy][ix] = 0;
        }
      }
      levels[i].x = WIDTH/2;
      levels[i].y = HEIGHT/2;
      levels[i].generated = 0;
      levels[i].prev_level = 0;
      levels[i].next_level = 0;
    }
    levels[0].generated = 1;

    while((input != 'q') && (AUTORUN == 0 ? 1 : (counter--))){ // Main game loop
    // In autorun mode, stop when counter reaches 0

    //generate new random paths (current node is now marked as explored)
    if(map[y][x] != EXPLORED){
    	for(i = 0;i < 2;i++){
        	pathx = x;
        	pathy = y;
        	dir = rand()%4;
            	if(dir == NORTH){
                	pathy++;
            	} else if(dir == SOUTH){
                	pathy--;
            	} else if(dir == WEST){
                	pathx--;
            	} else if(dir == EAST){
                	pathx++;
            	};
            	if(map[pathy][pathx] != EXPLORED) map[pathy][pathx] = 1;   
    	};
   	 map[y][x] = EXPLORED; 
     if((portal_exists == 0) && ((rand()%30) == 1) && (x != (WIDTH/2)) && (y != (HEIGHT/2)))
     { //Sometimes you discover a portal
        portal_exists = 1;
        portalx = x;
        portaly = y;
     }
    };
 
    //Display map. Player picks a path
    
    
    if(AUTORUN == 1){
      char choices[6] = "wasd><";
      input = choices[rand()%4];
     // usleep(10000);
    } else { 
      clear_screen();
      print_map(map,x,y,portalx,portaly);	
      printf("x = %d, y = %d, curr_depth = %d",x,y,curr_depth);
      input = getchar();
    }
    newy = y;
    newx = x;
    if(input == 'w'){
            newy--;
    } else if(input == 's'){
            newy++;
    } else if(input == 'a'){
            newx--;
    } else if(input == 'd'){
            newx++;
    } else if((input == '<') && (curr_depth > 0) && (x == (WIDTH/2)) && (y == (HEIGHT/2))){
      for(iy = 0;iy < HEIGHT;iy++){ // Copy current map into savegame struct (saving it)
        for(ix = 0;ix < WIDTH;ix++){
          levels[curr_depth].map[iy][ix] = map[iy][ix];  
        }
      }
      levels[curr_depth].generated = 1;
      levels[curr_depth].x = x;
      levels[curr_depth].y = y;
      levels[curr_depth].portal_exists = portal_exists;
      levels[curr_depth].portalx = portalx;
      levels[curr_depth].portaly = portaly;
      levels[curr_depth].next_level = curr_depth + 1;
      levels[curr_depth].prev_level = curr_depth - 1;
      curr_depth--;

      // Load the new map
      for(iy = 0;iy < HEIGHT;iy++){
        for(ix = 0;ix < WIDTH;ix++){
          map[iy][ix] = levels[curr_depth].map[iy][ix]; 
        }
      }
      newx = levels[curr_depth].x;
      newy = levels[curr_depth].y;
      portal_exists = levels[curr_depth].portal_exists;
      portalx = levels[curr_depth].portalx;
      portaly = levels[curr_depth].portaly;
      next_level = levels[curr_depth].next_level;
      prev_level = levels[curr_depth].prev_level;

    } else if((input == '>') && ((curr_depth+1) < DEPTH) && (portalx == x) && (portaly == y)){
      for(iy = 0;iy < HEIGHT;iy++){ // Copy current map into savegame struct (saving it)
        for(ix = 0;ix < WIDTH;ix++){
          levels[curr_depth].map[iy][ix] = map[iy][ix];  
        }
      }
      levels[curr_depth].generated = 1;
      levels[curr_depth].x = x;
      levels[curr_depth].y = y;
      levels[curr_depth].portal_exists = portal_exists;
      levels[curr_depth].portalx = portalx;
      levels[curr_depth].portaly = portaly;
      levels[curr_depth].next_level = curr_depth + 1;
      levels[curr_depth].prev_level = curr_depth - 1;

      if(levels[curr_depth+1].generated == 1){ // if already explored partially, load it
        curr_depth++;
        for(iy = 0;iy < HEIGHT;iy++){
          for(ix = 0;ix < WIDTH;ix++) map[iy][ix] = levels[curr_depth].map[iy][ix]; 
        }
        newx = levels[curr_depth].x;
        newy = levels[curr_depth].y;
        portal_exists = levels[curr_depth].portal_exists;
        portalx = levels[curr_depth].portalx;
        portaly = levels[curr_depth].portaly;
        next_level = levels[curr_depth].next_level;
        prev_level = levels[curr_depth].prev_level;
      } else {
        curr_depth++;
        // Generate a new map    
        for(ix = 0;ix < WIDTH;ix++){
          for(iy = 0;iy < HEIGHT;iy++){
            map[iy][ix] = 0;
          }
        }
        portal_exists = 0;
        portalx = portaly = 0;
        x = WIDTH/2;
        y = HEIGHT/2;
        pathx = newx = x;
        pathy = newy = y;
        next_level = curr_depth + 1;
        prev_level = curr_depth - 1;
        int map[HEIGHT][WIDTH] = {{0}};
      }
      
    /*
    // write a levels array with the first element defined, just as a test
      save = fopen ("save","wb");
      int success = 0;
      int pos[2] = {x,y};
      for(iy = 0;iy < HEIGHT;iy++){
        for(ix = 0;ix < WIDTH;ix++){
          levels[0].map[iy][ix] = map[iy][ix];  // Copy current map into savegame struct
        }
      }
      levels[0].x = x;
      levels[0].y = y;
      levels[0].generated = 0;
    

    //fwrite (map , sizeof(int) * WIDTH, HEIGHT, save); // WORKS
    //success = fwrite (pos,sizeof(int),2,save); // Write x and y positions
      success = fwrite (levels,sizeof(struct level),DEPTH,save);
      printf("success = %d\n",success);
      fclose (save);
*/

      
      
    };
    // Follow the path if valid
    if(map[newy][newx] != 0) {
        x = newx;
        y = newy;
    };
  
    };
    
    if(AUTORUN == 1) print_map(map,x,y,portalx,portaly);
    system("stty cooked echo");
    return 0;
}
