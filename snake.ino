#include <stdlib.h>
#include <stdio.h>
#include <TFT.h>
#include <SPI.h>
 
/* TFT pins */
#define CS   10
#define DC   9
#define RESET  8

/* Pushbutton pins */
#define pbLeft 7
#define pbDown 6
#define pbUp 3
#define pbRight 2

/***********************************************************************
 * VARIABLES
 */

TFT myScreen = TFT(CS, DC, RESET);  /* x: 0-160  y: 0-128 */

int screenWidth = myScreen.width();
int screenHeight = myScreen.height();
int tileSize = 6;

long timestamp;
int growBy = 0;
int collision = 0;
int score = 5;
char scoreChars[5];

struct colour_struct{
  int r,g,b;
};
typedef struct colour_struct colour;

colour bgCol = {93,200,255};
colour snakeCol = {0,255,0};
colour foodCol = {253,230,62};

struct coord_struct{
  unsigned char x,y;
  struct coord_struct *next_ptr; 
};
typedef struct coord_struct coord;

coord *new_ptr;
coord *temp;
coord *head = NULL;

coord moveTo = {0,0};
coord foodTile;

/***********************************************************************
 * FUNCTIONS
 */

void startScreen(){
  /* Title */
  myScreen.background(245, 78, 253);
  myScreen.setTextSize(4);
  myScreen.text("Snake",10,10);
  myScreen.setTextSize(1);
  myScreen.text("press button to start",10, 80);
  
  while (moveTo.x == 0 && moveTo.y == 0){
    getInput();
    delay(5);
  }
  
  /* Background and framing */
  myScreen.fill(bgCol.r,bgCol.g,bgCol.b);
  myScreen.rect(3,2,screenWidth-6,screenHeight-4);

  /* The snake and food data before the game starts */
  for (int i = 11; i<=41; i+=6){
    addFirst(i,4);
    drawSegment(head->x, head->y);
  }
  /* TODO: "random placement, not on forbidden tiles"-function */
  foodTile.x = 5;
  foodTile.y = 4;
  drawFood(foodTile.x, foodTile.y);
}

/* Input the incremental move using the pushbuttons */
void getInput(){
    if (digitalRead(pbLeft) == HIGH){
      moveTo.x = -tileSize; 
      moveTo.y = 0;  
    }
    else if (digitalRead(pbDown) == HIGH){
      moveTo.x = 0; 
      moveTo.y = tileSize;  
    }
    else if (digitalRead(pbUp) == HIGH){
      moveTo.x = 0; 
      moveTo.y = -tileSize;  
    }
    else if (digitalRead(pbRight) == HIGH){
      moveTo.x = tileSize; 
      moveTo.y = 0;  
    }
}

/* Draw a segment of the snake */
void drawSegment(int x, int y){
  myScreen.noStroke();
  myScreen.fill(snakeCol.r, snakeCol.g, snakeCol.b);
  myScreen.rect(x,y,tileSize,tileSize);
}

/* Draw a food */
void drawFood(int x, int y){
  myScreen.noStroke();
  myScreen.fill(foodCol.r, foodCol.g, foodCol.b);
  myScreen.rect(x,y,tileSize,tileSize); 
}

/* "Erase" a segment of the snake */
void remSegment(int x, int y){
  myScreen.noStroke();
  myScreen.fill(bgCol.r, bgCol.g, bgCol.b);
  myScreen.rect(x,y,tileSize,tileSize);
}

/* Add x and y as a coord first in the linked list */
void addFirst(int x, int y){
    new_ptr = (coord *)malloc(sizeof(coord));
    new_ptr->x = x;
    new_ptr->y = y;
    new_ptr->next_ptr = head;
    head = new_ptr;
}

/* Remove the last item in the linked list */
void remLast(){
  coord *last;  
  temp = head;
  last = temp;
  while (temp != NULL && temp->next_ptr != NULL){
    last = temp;
    temp = temp->next_ptr;
  }
  if (last == temp){
    free(temp);
    head = NULL;
  } else {
    remSegment(temp->x,temp->y);
    free(last->next_ptr);
    last->next_ptr = NULL;  
  }
}

/* Check if a coordinate exist in the linked list */
int isMember(int x, int y){
   temp = head;
   while(temp){
     if (temp->x == x && temp->y ==y){
       return 1; 
     }
     temp = temp->next_ptr;
   }
   return 0;  
}

/* Check if the snake head is on a foodTile and should grow */
int isGrow(){
  if(head->x == foodTile.x && head->y == foodTile.y){
   return 1; 
  } else {
    return 0;
  }
}

/* Check if the snake head is on a forbidden tile and should die :/ */
int isCollision(){
   /* TODO: check for collision */
}

/* Print score etc before starting over */
void endScreen(){
  String scorestring = String(score);
  scorestring.toCharArray(scoreChars,5);
  
  myScreen.text("Game over", 40, 40);
  myScreen.text("Score:", 40, 50);
  myScreen.text(scoreChars, 60, 50); 
}

/***********************************************************************
 * SETUP
 */

void setup(){
  myScreen.begin();
  randomSeed(analogRead(0));
}

/***********************************************************************
 * LOOP
 */

void loop(){
  
  startScreen(); /* TODO: pause until button is pressed */
  
  /* Main brain game frame */
  while(!collision){

    /* Pushbutton input */
    getInput();
    delay(5); /* Just to be kind to the CPU */
    
    if (millis()-timestamp > 300){
      /* Update, draw, and check things */
      temp = head;
      temp->x += moveTo.x;
      temp->y += moveTo.y;
      addFirst(temp->x,temp->y);
      
      /* Draw */
      drawSegment(head->x, head->y);

      if (growBy){
        growBy--;
        score += 1;    
      }
      else {
        remLast();
      }
      
      if (isGrow()){
        growBy++;
        /* TODO: add new foodTile */
      }

      if (isCollision()){
        collision = 1;
      }
      timestamp = millis();
    }
  }
  
  endScreen();
  delay(5000); /* TODO: pause until button is pressed */
}
