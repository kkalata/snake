#include "../snake.h"

void PlaceDot(
    Position *dotPos,
    Snake *snake,
    Position otherDotPos
)
{
    int dotUnderSomeObject;
    do
    {   
        dotUnderSomeObject = 0;
        dotPos->x = rand() % BOARD_SECTION_WIDTH; // random number between 0 and board width
        dotPos->y = rand() % BOARD_SECTION_HEIGHT; // random number between 0 and board height
        
        SnakeSegment *snakeSegment = snake->segment;
        do
        {
            if (snakeSegment->pos.x == dotPos->x && snakeSegment->pos.y == dotPos->y)
            {
                dotUnderSomeObject = 1;
                break;
            }
            snakeSegment = snakeSegment->next;
        } while (snakeSegment != snake->segment);

        if (otherDotPos.x == dotPos->x && otherDotPos.y == dotPos->y)
        {
            dotUnderSomeObject = 1;
        }
    } while (dotUnderSomeObject);
}
