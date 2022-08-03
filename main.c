 
//https://github.com/AidanPDaley/SnakeGame/tree/main
// CS 355
// Snake Game
// Cassidy, Tori, Darby, Aidan
#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <time.h>
 
#define fruit_max 80
#define fruit_min 10
 
int lastDirection = 0; // last direction the snake moved
int speed = 5; // speed at which the snake moves
int score = 0; 

// Author: Cassidy
// Fruit =============================================================

struct fruit
{
   int row;
   int column;
   int value;
   time_t timeCreated; // what time the fruit was created
   int time; // how many seconds will it last
};
 
struct fruit *fruit;
 
// initiate the fruit
void init_fruit()
{
   fruit = malloc(sizeof (struct fruit));
}
 
//create a new piece of fruit
void new_fruit()
{
   //assign a random location of the fruit within the given snake pit
   fruit->row = (rand() % (LINES - 2)) + 1;
   fruit->column = (rand() % (COLS -2)) + 1;
 
   //randomly assign a fruit value between 1 and 9
   fruit->value = (rand() % 9) + 1;
   fruit->timeCreated = time(NULL);
   fruit->time = (rand() % 9) + 1;
}
 
//prints the fruit to the screen
void print_fruit()
{
   //print the fruit, first convert the fruit value to a string, then print
   char fruitvalue[3];
   move(fruit->row, fruit->column);
   sprintf(fruitvalue, "%d", fruit->value);
   addstr(fruitvalue);
}
// updates the fruits timer
// if timer has expired, create new fruit.
void updateFruit() {
  time_t currentTime = time(NULL);
  if (currentTime > fruit->timeCreated + fruit->time)
    new_fruit();
}

// Free Fruit Memory 
void freeFruit() {
  free(fruit);
}


// =================== SNAKE CODE =========================
// Author: Aidan Daley and Darby Scott
 
// This is the structure that the snake will be made of
// Each part of the body links to the next part of the body until you reach the tail.
typedef struct Snake {
   int x;
   int y;
   int lastX;
   int lastY;
   struct Snake* next;
} Snake; // not sure if this is actually needed...
 

// Initiates the snake onto the board at designated position.
Snake* snakeInit(int x, int y) {
   // Create the Snake head. Later we will add the body
   Snake *head = malloc(sizeof(Snake));   
  
   // Set position of the head on the game board.
   head->x = x;
   head->y = y;
  
   return head;
} // returns a memory address


// returns snake's head x coordinate
int getSnakeHeadX(Snake* snakeHead) {
  return snakeHead->x;
}

// returns snake's head y coordinate
int getSnakeHeadY(Snake* snakeHead) {
  return snakeHead->y;
} 
 
// This function takes a pointer to the head of snake and moves the snake in whatever way we determine
// By updating the x and y values of the snake.
void moveSnake(Snake* snakeHead, int newX, int newY) {
  
   // Updates the last position, this will be useful for growing the snake
   snakeHead->lastX = snakeHead->x;
   snakeHead->lastY = snakeHead->y;
  
   // Update the x and the y position
   snakeHead->x = newX;
   snakeHead->y = newY;
 
   // if we aren't at the tail of the snake, update the next body part.
   if (snakeHead->next != NULL)
       moveSnake(snakeHead->next, snakeHead->lastX, snakeHead->lastY);
}
 
// takes input from the user and moves the snake using the moveSnakeMethod.
// Author: Tori
int controlSnake(Snake* head) {
   // getch data type is char.
   switch (getch()) {        
         case KEY_UP:       
            moveSnake(head, head->x - 1,head->y + 0);
            lastDirection = 1;
        break;

        case KEY_DOWN:
            moveSnake(head, head->x + 1,head->y + 0);
            lastDirection = 2;
        break;

        case KEY_LEFT:
            moveSnake(head, head->x + 0,head->y + -1);
            lastDirection = 3;
        break;  

        case KEY_RIGHT:        
            moveSnake(head, head->x + 0,head->y + 1);  
            lastDirection = 4;    
        break;

        // if no input (getch returns ERROR), do last direction.
        case ERR:

          if (lastDirection == 0)
            lastDirection = rand() % 4;
          switch (lastDirection) {        
             case 1:       
                moveSnake(head, head->x - 1,head->y + 0);
            break;

            case 2:
                moveSnake(head, head->x + 1,head->y + 0);
            break;

            case 3:
                moveSnake(head, head->x + 0,head->y + -1);
            break;  

            case 4:        
                moveSnake(head, head->x + 0,head->y + 1);   
            break; 

            default:
               return 1;
          }
            
        break; // end of default movement

    default:
       return 1;
   }
   return 0;
}
 
 
// When the snake eats a fruit (which totally makes sense since snakes love fruit),
// the body grows.
void growSnake(Snake *head) {
   Snake *temp = head;
   while (temp->next != NULL) {
       temp = temp->next;
   }
  
   temp->next = snakeInit(temp->lastX, temp->lastY);
}
 
// Free snake memory
void killSnake(Snake* head) {
   Snake *temp = head;
   if (temp->next != NULL)
       temp = temp->next;
   free(temp);
}

// Check for collisions
// checks to see if the snake collides with itself
// takes two arguments: the head of the snake, and the body part to compare to the head.
// when this function is called, just pass the head for each.
int snakeCollision(Snake* head) {

  // if snake's head matches part of the body
  // return 1
  Snake *body = head;
  body = body->next;

  while (body != NULL) {
    if (getSnakeHeadX(head) == getSnakeHeadX(body) &&
        getSnakeHeadY(head) == getSnakeHeadY(body))
        return 1;
    else
      body = body->next;
  }

  // if snake hits a wall, return 1
  // otherwise, return 0, because the snake didn't hit anything
  if (getSnakeHeadX(head) == 0 || getSnakeHeadX(head) == LINES-1)
    return 1;
  if (getSnakeHeadY(head) == 0 || getSnakeHeadY(head) == COLS-1)
    return 1;

  return 0;
}

// Checks to see if the snake has "eaten" the fruit
// if the snake head has reached the same location as the fruit, then increase score
// 
void eatfruit(Snake *head)
{
   if (getSnakeHeadX(head) == fruit->row &&
       getSnakeHeadY(head) == fruit->column) {
       
       score += fruit->value;
      
      //loop to grow snake length of the fruit
      for (int i = 0; i < fruit->value; i++) {
          growSnake(head);
      }
      
      // create new fruit
      new_fruit();
      //increase speed
      if (speed > 1)
        speed -= 1;
      halfdelay(speed);
   }
}

 
// Draw Snake
// Author: Tori
void drawSnake(Snake *head) {
   Snake *temp = head;
  
   move(temp->x, temp->y);
   addstr("@");
 
   // if there is more of the snake, draw the next part
   if (temp->next != NULL) {
       temp = temp->next;
       drawSnake(temp);
   }
}

//============== End of Snake Code ========================
 
// Author: Cassidy
// Draws the board used in the game
void snakepit()
{   
  int i;
  int j;
  for(i = 0; i < LINES; i++)
  {
      for(j = 0; j < COLS; j++)
      {
           move(i,j);
           if(i == 0 || i == LINES -1)
           {
               standout(); // turn on the bold attribute
               addstr(" ");
           }
           if(j == 0 || j == COLS-1)
           {
               standout(); //turn on the bold attribute
               addstr(" ");
           }
         
      }
  }
  standend(); // turn off the bold attribute
  //keep score of game in the top left corner
  move(0, 2);
  addstr("SCORE:");
  char scorePrint[3]; // change the integer score into a char string
   move(0, 8);
   sprintf(scorePrint, "%d", score);
   addstr("  ");
   addstr(scorePrint);
}
 
 
// main method
int main()   
{     
   // ===================== Initializing Game ==========================
   initscr();
   // create the snake head as a pointer.
   Snake *head = snakeInit(LINES/2, COLS/2);    
   for (int i; i <= 2; i++)
       growSnake(head);
  
   noecho();
   curs_set(0);
   halfdelay(speed); // how many tenths of a second the snake waits before moving automatically
   keypad(stdscr, TRUE);
   init_fruit();
   new_fruit();
 
   // ========================= Game Loop ===============================
   while(1) {
       clear();             // Clear the screen
       snakepit();          // Draw board
       print_fruit();       // Draw Fruit
       eatfruit(head);      // Check to see if snake get fruit
       drawSnake(head);     // Draw Snake 
       updateFruit();       // Update fruit's timer
       
      
 
       // if any key but the arrow keys are pressed, end the game.
       int keyInput = controlSnake(head);
           if (keyInput == 1)
               break;

       // if snake collides with anything, end game.
       int collision = snakeCollision(head);
       if (collision == 1)
            break;


       // if the score is snake's length equals half perimeter of the border, end the game
       if (score + 3 >= (LINES + COLS)) {
          break;
       } 
 
       refresh();
   }

   // ========================= Ending the Game ===============================
   endwin(); // Close Window
   killSnake(head); // Free memory
   freeFruit();

   // End Game Message
   if (score + 3 > (LINES + COLS))
    printf("YOU WIN! Your score is %d\n", score);
   else 
    printf("GAME OVER! Better Luck Next Time!\nFinal Score: %d\n", score);

   return 0;
}

