// Generates a map and allows the play to move through it.
// Potential difficulties: realtime player input, rougelike style
// Done by 1:30 pm
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

#define AUTORUN 0  // If set to 1, bot will auto-explore the map

#define NORTH 0
#define SOUTH 1
#define EAST 2
#define WEST 3
#define HEIGHT 50
#define WIDTH 50

#define EXPLORED 2

void clear_screen(void){
  int counter = 100;
  while(counter--) printf("\n");
  return;
}

void print_screen(void)
{
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

void print_map(int map[HEIGHT][WIDTH], int playerx, int playery)
{
    int ix,iy;
    for(iy = 0;iy < HEIGHT;iy++){
        for(ix = 0;ix < WIDTH;ix++){
            if((iy == playery) && (ix == playerx)){
                printf("@");
            } else if(map[iy][ix] == 1){
                printf(".");
            } else if(map[iy][ix] == EXPLORED){
                printf("0");
            } else {
		            printf(" ");
	    };
          //  printf("%d",map[iy][ix]);
        }
        printf("\n");
    }
    return;
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    int i,x,y,pathx,pathy,newx,newy;
    x = WIDTH/2;
    y = HEIGHT/2;
    pathx = newx = x;
    pathy = newy = y;
    char input = ' ';
    int dir = 0;
    int map[HEIGHT][WIDTH] = {{0}};
	
    while(1){ // Main game loop
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
    };
 
    //Display map. Player picks a path
    
    newy = y;
    newx = x;
    if(AUTORUN == 1){
      char choices[4] = "wasd";
      input = choices[rand()%4];
     // usleep(10000);
    } else { 
      clear_screen();
      print_map(map,x,y);	
      input = getchar();
    }
    if(input == 'w'){
            newy--;
    } else if(input == 's'){
            newy++;
    } else if(input == 'a'){
            newx--;
    } else if(input == 'd'){
            newx++;
    };
    // Follow the path if valid
    if(map[newy][newx] != 0) {
        x = newx;
        y = newy;
    };
	    
    };
    
    return 0;
}
