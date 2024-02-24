#include <8051.h>
#include "game.h"
#include "memory.h"

void GameInit(void)
{
    //initialize the game board
    // 0  | 0 0 0 0 1 0 0 0 | 0 0 0 1 1 0 0 0 |
    // 1  | 0 1 0 0 0 0 1 0 | 1 0 0 0 0 0 0 1 |
    //Note that 8051 is small endian
    game_board0[0] = 0x08; game_board0[1] = 0x18;
    game_board1[0] = 0x42; game_board1[1] = 0x81;

    game_state = 15;
    interrupt_counter = 0;
    dinosaur_row = 0;
    score = 0;
    last_input = ' ';
}

void SetGameLevel(unsigned char level)
{
    if (level <= 9)
    {
        game_state = level;
    }
    else
    {
        game_state = 0;
    }
}

unsigned char GameStart(void)
{
    return game_state <=9;
}

void MoveDinosaur(unsigned char direction)
{
    if (direction == 2)
    {
        dinosaur_row = 0;
    }
    else if (direction == 8)
    {
        dinosaur_row = 1;
    }
}

void BoardRefresh(void)
{
    //Refresh the game board by shifting it left by one bit and check if the game is over.
    if (GameStart())
    {
        interrupt_counter = interrupt_counter + 1;
        if (interrupt_counter == ( 65 - game_state * 4 ) )
        {
            interrupt_counter = 0;
            //shift evry bit in the array to the left in round robin
            if( game_board0[1] & 0x80 ) msb_tmp = 0x01;
            else msb_tmp = 0;

            if(game_board0[0] & 0x80) msb_tmp |= 0x02;

            game_board0[0] = game_board0[0] + game_board0[0] + (msb_tmp & 0x01);
            game_board0[1] = game_board0[1] + game_board0[1] + (msb_tmp & 0x02);

            //check if the game is over
            if( (game_board0[0] & 0x80) && ( dinosaur_row == 0 ) )
            {
                game_state = 10;
            }
            else if( (game_board0[0] & 0x80) && ( dinosaur_row == 1 ) )
            {
                score = score + 1;
            }
            //shift evry bit in the array to the left in round robin
            if( game_board1[1] & 0x80 ) msb_tmp = 0x01;
            else msb_tmp = 0;

            if(game_board1[0] & 0x80) msb_tmp |= 0x02;

            game_board1[0] = game_board1[0] + game_board1[0] + (msb_tmp & 0x01);
            game_board1[1] = game_board1[1] + game_board1[1] + (msb_tmp & 0x02);
            
            //check if the game is over
            if( (game_board1[0] & 0x80) && ( dinosaur_row == 1 ) )
            {
                game_state = 10;
            }
            else if( (game_board1[0] & 0x80) && ( dinosaur_row == 0 ) )
            {
                score = score + 1;
            }
        }
    }
}

unsigned char GameOver(void)
{
    return game_state == 10;
}

unsigned char GetScore(void)
{
    return score;
}

unsigned char GetDinosaurRow(void)
{
    return dinosaur_row;
}

unsigned char GetGameBoard(unsigned char row, unsigned char col)
{ 
    //Note that 8051 is small endian
    //col /0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15
    //bit /7  6  5  4  3  2  1  0 15  14 13 12 11 10 9  8

    if( (row == 0) && (col == 0) ) return (game_board0[0] & 0x80)? 1:0;
    if( (row == 0) && (col == 1) ) return (game_board0[0] & 0x40)? 1:0;
    if( (row == 0) && (col == 2) ) return (game_board0[0] & 0x20)? 1:0;
    if( (row == 0) && (col == 3) ) return (game_board0[0] & 0x10)? 1:0;
    if( (row == 0) && (col == 4) ) return (game_board0[0] & 0x08)? 1:0;
    if( (row == 0) && (col == 5) ) return (game_board0[0] & 0x04)? 1:0;
    if( (row == 0) && (col == 6) ) return (game_board0[0] & 0x02)? 1:0;
    if( (row == 0) && (col == 7) ) return (game_board0[0] & 0x01)? 1:0;
    if( (row == 0) && (col == 8) ) return (game_board0[1] & 0x80)? 1:0;
    if( (row == 0) && (col == 9) ) return (game_board0[1] & 0x40)? 1:0;
    if( (row == 0) && (col == 10) ) return (game_board0[1] & 0x20)? 1:0;
    if( (row == 0) && (col == 11) ) return (game_board0[1] & 0x10)? 1:0;
    if( (row == 0) && (col == 12) ) return (game_board0[1] & 0x08)? 1:0;
    if( (row == 0) && (col == 13) ) return (game_board0[1] & 0x04)? 1:0;
    if( (row == 0) && (col == 14) ) return (game_board0[1] & 0x02)? 1:0;
    if( (row == 0) && (col == 15) ) return (game_board0[1] & 0x01)? 1:0;

    if( (row == 1) && (col == 0) ) return (game_board1[0] & 0x80)? 1:0;
    if( (row == 1) && (col == 1) ) return (game_board1[0] & 0x40)? 1:0;
    if( (row == 1) && (col == 2) ) return (game_board1[0] & 0x20)? 1:0;
    if( (row == 1) && (col == 3) ) return (game_board1[0] & 0x10)? 1:0;
    if( (row == 1) && (col == 4) ) return (game_board1[0] & 0x08)? 1:0;
    if( (row == 1) && (col == 5) ) return (game_board1[0] & 0x04)? 1:0;
    if( (row == 1) && (col == 6) ) return (game_board1[0] & 0x02)? 1:0;
    if( (row == 1) && (col == 7) ) return (game_board1[0] & 0x01)? 1:0;
    if( (row == 1) && (col == 8) ) return (game_board1[1] & 0x80)? 1:0;
    if( (row == 1) && (col == 9) ) return (game_board1[1] & 0x40)? 1:0;
    if( (row == 1) && (col == 10) ) return (game_board1[1] & 0x20)? 1:0;
    if( (row == 1) && (col == 11) ) return (game_board1[1] & 0x10)? 1:0;
    if( (row == 1) && (col == 12) ) return (game_board1[1] & 0x08)? 1:0;
    if( (row == 1) && (col == 13) ) return (game_board1[1] & 0x04)? 1:0;
    if( (row == 1) && (col == 14) ) return (game_board1[1] & 0x02)? 1:0;
    if( (row == 1) && (col == 15) ) return (game_board1[1] & 0x01)? 1:0;
    return -1;
}