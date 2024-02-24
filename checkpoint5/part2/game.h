#ifndef __GAME_H__
#define __GAME_H__
void GameInit(void);
void SetGameLevel(unsigned char level);
unsigned char GameStart(void);
void MoveDinosaur(unsigned char direction);
void BoardRefresh(void);
unsigned char GameOver(void);
unsigned char GetScore(void);
unsigned char GetDinosaurRow(void);
unsigned char GetGameBoard(unsigned char row, unsigned char col);
#endif // __GAME_H__
