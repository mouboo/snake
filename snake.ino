#include <SPI.h>
#include <TFT.h>
#include "structworkaround.h"

/* TFT pins */
#define LCD_CS   10
#define SD_CS 4
#define DC   9
#define RESET  8

/* Pushbutton pins */
#define PBLEFT 7
#define PBDOWN 6
#define PBUP 3
#define PBRIGHT 2

#define screenWidth 160
#define screenHeight 128
#define tileSize 6


TFT screen = TFT(LCD_CS, DC, RESET);

long timestamp = 0;
int score = 0;
int collision = 0;
int goX = 0;
int goY = 0;

colour bgCol = {93,200,255};
colour snakeCol = {245, 78, 253};
colour foodCol = {253,230,62};

coord *new_ptr;
coord *temp;
coord *head = NULL;

/*****************************************************************
 *  FUNCTIONS
 */

void 
addFirst(int x, int y){
    new_ptr = (coord *)malloc(sizeof(coord));
    new_ptr->x = x;
    new_ptr->y = y;
    new_ptr->next_ptr = head;
    head = new_ptr;
}

void 
remLast(){
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
    drawRect(temp->x,temp->y,bgCol); /* draw */
    free(last->next_ptr);
    last->next_ptr = NULL;  
  }
}
  
void
startScreen(){
  reset();
  
  screen.background(245, 78, 253);
  screen.setTextSize(4);
  screen.stroke(0,255,0);
  screen.text("Snake",10,10);
  screen.setTextSize(1);
  screen.text("press button to start",10, 80);
  
  while (goX == 0 && goY == 0){
    getInput();
    delay(5);
  }
  
  screen.stroke(0,0,0);
  screen.fill(bgCol.r,bgCol.g,bgCol.b);
  screen.rect(3,2,screenWidth-6,screenHeight-4);
  
  for (int i = 29; i<=53; i+=6){
    addFirst(i,52);
    drawRect(head->x, head->y,snakeCol);
  }
  
  putFood();
}
 
void
reset(){
  goX = 0;
  goY = 0;
  collision = 0;  
  score = 0;
  while(head){
    remLast();
  }
}

void 
getInput(){
    if (digitalRead(PBLEFT) == HIGH){
      goX = -tileSize; 
      goY = 0;  
    }
    else if (digitalRead(PBDOWN) == HIGH){
      goX = 0; 
      goY = tileSize;  
    }
    else if (digitalRead(PBUP) == HIGH){
      goX = 0; 
      goY = -tileSize;  
    }
    else if (digitalRead(PBRIGHT) == HIGH){
      goX = tileSize; 
      goY = 0;  
    }
}

int 
isGrow(){
  if(head->x == food.x && head->y == food.y){
    return 1; 
  } else {
    return 0;
  }
}

int
isCollision(){
  temp = head->next_ptr;
  while(temp){
    if (temp->x == head->x && temp->y == head->y){
      return 1;
    }
    temp = temp->next_ptr;  
  }
  if(head->x < 5 || head->x > 149 || head->y < 4 || head->y > 118){
    return 1;
  }
  return 0;
}
 
void
putFood(){
  int validPlace = 0;
  while (!validPlace){
    food.x = 5 + (int)random(0,24)*6;
    food.y = 4 + (int)random(0,19)*6;
    if (food.x != head->x && food.y != head->y){
      validPlace = 1; 
    }
  }
  drawRect(food.x, food.y, foodCol);
}

void 
drawRect(int x, int y, colour c){
  screen.noStroke();
  screen.fill(c.r, c.g, c.b);
  screen.rect(x,y,tileSize,tileSize);
}

void
endScreen(){
  char scoreChars[5];
  screen.background(0, 255, 0);
  screen.stroke(255,255,255);
  String scorestring = String(score);
  scorestring.toCharArray(scoreChars,5);
  
  screen.text("Game over", 40, 40);
  screen.text("Score:", 40, 50);
  screen.text(scoreChars, 80, 50);  
  delay(1000);
}

void
setup(){
  screen.begin();
  randomSeed(analogRead(0));
}

void
loop(){
  startScreen();
  while (!collision){
    getInput();
    delay(5);
    if (millis()-timestamp > 200){
      addFirst((head->x + goX), (head->y + goY));
      drawRect(head->x,head->y,snakeCol);
      if (food.growBy){
        food.growBy--;
        score += 1;
      }
      else {
        remLast();
      }
      if (isGrow()){
        food.growBy++;
        putFood(); 
      }
      if (isCollision()){
        collision = 1;
      }
      timestamp = millis();
    }
  }
  endScreen();  
}
