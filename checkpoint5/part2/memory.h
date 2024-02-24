//----------------dino-----------------
__data __at (0x20) char SharedBuffer;
__data __at (0x21) char full;
__data __at (0x22) char mutex;
__data __at (0x23) char empty;
__data __at (0x24) char tmp;
__data __at (0x25) char last_input;
__data __at (0x26) unsigned char j;

//----------------preemptive--------------
__data __at (0x27) char thread_bitmap;
__data __at (0x28) char current_thread;
__data __at (0x29) char saved_sp[4];
__data __at (0x2D) char i; // for loop
__data __at (0x2E) char new_thread;
__data __at (0x2F) char sp_temp;

//-----------------game-------------------
__data __at (0x30) unsigned char game_board0[2];
__data __at (0x32) unsigned char game_board1[2];
__data __at (0x34) unsigned char msb_tmp;
// 0~9 for difficulty, 10 for game over, 11 for not yet started 
__data __at (0x35) unsigned char game_state;
//the counter is for the pace of the game
__data __at (0x36) unsigned int interrupt_counter;
__data __at (0x38) char dinosaur_row;
__data __at (0x39) char score;

//----------------lcd---------------------
__data __at (0x3A) unsigned char lcd_ready;