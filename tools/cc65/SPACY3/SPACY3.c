//for cc65, for NES
//complete space shooter game, pt 3
//doug fraker 2015
//feel free to reuse any code here

void* __fastcall__ memcpy (void* dest, const void* src, int count);

void __fastcall__ Shuffle_Sprites(void);

void __fastcall__ Sprite_Zero(void);

void __fastcall__ Reset(void);

void __fastcall__ Blank_sprite3(void);

void __fastcall__ Play_Fx(unsigned char effect);

void __fastcall__ Reset_Music(void);

void __fastcall__ Play_Music(unsigned char song);

void __fastcall__ Music_Update(void);

void __fastcall__ Wait_Vblank(void);

void __fastcall__ UnRLE(int data);

void __fastcall__ Get_Input(void); //this calls an asm function, written in asm4c.s
//it will read both joypads and store their reads in joypad1 and joypad2
//The buttons come in the order of A, B, Select, Start, Up, Down, Left, Right

#define PPU_CTRL		*((unsigned char*)0x2000)
#define PPU_MASK		*((unsigned char*)0x2001)
#define PPU_STATUS		*((unsigned char*)0x2002)
#define OAM_ADDRESS		*((unsigned char*)0x2003)
#define SCROLL			*((unsigned char*)0x2005)
#define PPU_ADDRESS		*((unsigned char*)0x2006)
#define PPU_DATA		*((unsigned char*)0x2007)
#define OAM_DMA			*((unsigned char*)0x4014)


#define RIGHT		0x01
#define LEFT		0x02
#define DOWN		0x04
#define UP			0x08
#define START		0x10
#define SELECT		0x20
#define B_BUTTON	0x40
#define A_BUTTON	0x80


#define TITLE_MODE		0
#define RUN_GAME_MODE	1
#define PAUSE_MODE		2
#define GAME_OVER_MODE	3
#define VICTORY_MODE	4
#define BOSS_MODE		5


//Globals
//our startup code initialized all values to zero
#pragma bss-name(push, "ZEROPAGE")
unsigned char NMI_flag;
unsigned char Frame_Count;
unsigned char index;
unsigned char index4;
unsigned char X1;
unsigned char X1old;
unsigned char Y1;
unsigned char joypad1;
unsigned char joypad1old;
unsigned char joypad1test; 
unsigned char joypad2; 
unsigned char joypad2old;
unsigned char joypad2test;
int BGD_address;
//Hscroll always zero
unsigned char Vert_scroll;
unsigned char Vert_scroll2;
signed char X_speed; //signed char = -128 to 127
signed char Y_speed; //signed char = -128 to 127
unsigned char collision;
unsigned char collision2;
unsigned char temp;
unsigned char temp2;
unsigned char song;	//which song
unsigned char Game_Mode; //00 = title, 01 = Game, 02 = Pause, 03 = Game Over, 04 = Victory, 5 Boss
unsigned char Wait;
unsigned char Wait2;
unsigned char Score1;
unsigned char Score2;
unsigned char Score3;
unsigned char Lives;
unsigned char Bullet1x;
unsigned char Bullet1y; //a y position of zero = off
unsigned char Bullet2x;
unsigned char Bullet2y;
unsigned char Bullet3x;
unsigned char Bullet3y;
unsigned char Bullet4x;
unsigned char Bullet4y;
unsigned char Bullet5x;
unsigned char Bullet5y;
unsigned char Bullet6x;
unsigned char Bullet6y;
unsigned char Bullet7x; //7 sets of bullets allowed on screen at once
unsigned char Bullet7y; //it was 8, but the 8th one wasn't being used
unsigned char shuffle;
unsigned char bullet_Wait;
unsigned char Enemy1_Type; //0 = off, 1-2 = type
unsigned char Enemy1x;
unsigned char Enemy1y;
unsigned char Enemy2_Type;
unsigned char Enemy2x;
unsigned char Enemy2y;
unsigned char Enemy3_Type;
unsigned char Enemy3x;
unsigned char Enemy3y;
unsigned char Enemy4_Type;
unsigned char Enemy4x;
unsigned char Enemy4y; //4 on screen at once. pretty basic.
unsigned char Tile_Number;

unsigned char objectAx;
unsigned char objectAy;
unsigned char sizeAx;
unsigned char sizeAy;
unsigned char objectBx;
unsigned char objectBy;
unsigned char sizeBx;
unsigned char sizeBy;
unsigned char objectA_right;
unsigned char objectB_right;
unsigned char objectA_bottom;
unsigned char objectB_bottom;
unsigned char injury_Timer;
unsigned char Death_Flag;
unsigned char Victory_Flag;
unsigned char Master_Clock;
unsigned char Spawn_Index;
unsigned char Boss_Health;
unsigned char Boss_Move;
unsigned char Boss_Count;
unsigned char BossX;
//BossY = 0x28 always, in this game
unsigned char Boss_Laser_1x;
unsigned char Boss_Laser_1y; //>f0 = off
unsigned char Boss_Laser_2x;
unsigned char Boss_Laser_2y;
unsigned char Boss_Laser_3x;
unsigned char Boss_Laser_3y; //>f0 = off
unsigned char Boss_Laser_4x;
unsigned char Boss_Laser_4y;
unsigned char SFx_Wait;


#pragma bss-name(push, "OAM")
//OAM equals ram addresses 200+
unsigned char FILLER[0x40];//40
	//everything below here is shuffled
unsigned char SPRITE_BULLET[0x40];//80
unsigned char SPRITE_ENEMY[0x40];//c0
unsigned char SPRITE_BULLET_E[0x40];

unsigned char SPRITE_ZERO[4]; //final output from 300-3ff
unsigned char SPRITE_TEXT[0x1c];
unsigned char SPRITE_SHIP[0x20];
unsigned char SPRITES2[0xc0]; 


#include "BG/Title.h"
#include "BG/Stars.h"


//Title Screen, and starting sprite colors
const unsigned char PALETTE[]={
0x0f, 2, 0x13, 0x24,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0x10, 0x20,
0x0f, 0x01, 0x28, 0x22,  0, 0x18, 0x1b, 0x38,  0, 2, 0x13, 0x33,  0, 0x17, 0x1a, 0x39};

	
const unsigned char MetaSprite_Y[] = {0, 0, 8, 8, 16, 16}; //relative y coordinates

const unsigned char MetaSprite_Tile[] = { //tile numbers, right
	0, 1, 0x10, 0x11, 2, 3}; //only 1 ship animation

const unsigned char MetaSprite_X[] = {0, 8, 0, 8, 0, 8}; //relative x coordinates
//we are using 6 sprites, each one has a relative position from the top left sprite

const unsigned char HUD[] = {"SCORE:"};

const unsigned char HUD2[] = {"LIVES:"};

const unsigned char Enemy_Tiles[] ={
0x04, 0x05, 0x14, 0x15, 0x06, 0x07, 0x16, 0x17};

const unsigned char Spawn_Timing[] ={
2, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33,
36, 39, 42, 45, 48, 51, 54, 57, 60, 63, 66,
68, 70, 72, 74, 76, 78, 80, 82, 84, 86, 88,
90, 92, 94, 96, 98, 100, 102, 104, 106, 108, 110,
0xff};	

const unsigned char Spawn_Array[] ={
1, 2, 3, 4, 2, 1, 2, 4, 4, 1, 3, 2, 3, 4, 5, 5, 1, 2, 3, 1, 2, 5, 
1, 1, 2, 4, 3, 1, 4, 5, 4, 1, 2, 2, 3, 1, 5, 3, 1, 2, 5, 5, 4, 3};

void All_Off(void) {
	PPU_CTRL = 0;
	PPU_MASK = 0; 
}
	
void All_On(void) {
	PPU_CTRL = 0x90; //screen is on, NMI on
	PPU_MASK = 0x1e; 
}		
	
void Reset_Scroll (void) {
	PPU_ADDRESS = 0;
	PPU_ADDRESS = 0;
	SCROLL = 0;
	SCROLL = 0;
}
	
void Load_Palette(void) {
	PPU_ADDRESS = 0x3f;
	PPU_ADDRESS = 0x00;
	for( index = 0; index < sizeof(PALETTE); ++index ){
		PPU_DATA = PALETTE[index];
	}
}

void every_frame(void) {
	//do any PPU updates first
	OAM_ADDRESS = 0;
	OAM_DMA = 3;	//push all the sprite data from the ram at 300-3ff to the sprite memory ##changed
	SCROLL = 0;
	SCROLL = 0;	//setting the new scroll position
	PPU_CTRL = 0x90; //screen is on, NMI on
	PPU_MASK = 0x1e;	
	Get_Input();
}

void every_frame2(void) { //during game play
	//HUD UPDATE FIRST!!!
	PPU_ADDRESS = 0x24;
	PPU_ADDRESS = 0x6a;
	PPU_DATA = Score1+0x30;
	PPU_DATA = Score2+0x30;
	PPU_DATA = Score3+0x30;
	PPU_DATA = 0x30; //dummy score, last 3 always zero 
	PPU_DATA = 0x30;
	PPU_DATA = 0x30;
	PPU_ADDRESS = 0x24;
	PPU_ADDRESS = 0x7a;
	PPU_DATA = Lives+0x30;
	
	OAM_ADDRESS = 0;
	OAM_DMA = 3;	//push all the sprite data from the ram at 300-3ff to the sprite memory
	SCROLL = 0;
	SCROLL = 0;	//setting the new scroll position
	PPU_CTRL = 0x91; //screen is on, NMI on, show the HUD at the top of the screen
	PPU_MASK = 0x1e;	
	Get_Input();
}


void update_Sprites (void) {
	index4 = 0;
	for (index = 0; index < 6; ++index ){
		SPRITE_SHIP[index4] = MetaSprite_Y[index] + Y1; //relative y + master y
		++index4;
		SPRITE_SHIP[index4] = MetaSprite_Tile[index]; //tile numbers
		++index4;
		SPRITE_SHIP[index4] = 0; //attributes, all zero here
		++index4;
		SPRITE_SHIP[index4] = MetaSprite_X[index] + X1; //relative x + master x
		++index4;
	}
	
	//handle explosions if crash
	if (injury_Timer > 0 && injury_Timer <= 0x1c){ //explosion tiles
		SPRITE_SHIP[1] = 0x20;
		SPRITE_SHIP[5] = 0x20;
		SPRITE_SHIP[9] = 0x20;
		SPRITE_SHIP[13] = 0x20;
		SPRITE_SHIP[17] = 0x20;
		SPRITE_SHIP[21] = 0x20;
	}
	
	if (injury_Timer > 0x1c && injury_Timer <= 0x28){
		SPRITE_SHIP[1] = 0x0c;
		SPRITE_SHIP[5] = 0x0d;
		SPRITE_SHIP[9] = 0x1c;
		SPRITE_SHIP[13] = 0x1d;
	}
	if (injury_Timer > 0x28 && injury_Timer <= 0x34){
		SPRITE_SHIP[1] = 0x0a;
		SPRITE_SHIP[5] = 0x0b;
		SPRITE_SHIP[9] = 0x1a;
		SPRITE_SHIP[13] = 0x1b;
	}
	if (injury_Timer > 0x34){
		SPRITE_SHIP[1] = 0x08;
		SPRITE_SHIP[5] = 0x09;
		SPRITE_SHIP[9] = 0x18;
		SPRITE_SHIP[13] = 0x19;
	}
	if (injury_Timer != 0)
		--injury_Timer;
		
	
	//bullet sprite update was handled in the movement code
	
	index4 = 0;
	if (Enemy1_Type != 0){
		Tile_Number = (Enemy1_Type - 1) << 2; //type of ship
		for (index = 0; index < 4; ++index){
			SPRITE_ENEMY[index4] = MetaSprite_Y[index] + Enemy1y;
			++index4;
			SPRITE_ENEMY[index4] = Enemy_Tiles[index + Tile_Number];
			++index4;
			SPRITE_ENEMY[index4] = Enemy1_Type; //attributes, change later
			++index4;
			SPRITE_ENEMY[index4] = MetaSprite_X[index] + Enemy1x;
			++index4;
		}
	}
	
	if (Enemy2_Type != 0){
		Tile_Number = (Enemy2_Type - 1) << 2; //type of ship
		for (index = 0; index < 4; ++index){
			SPRITE_ENEMY[index4] = MetaSprite_Y[index] + Enemy2y;
			++index4;
			SPRITE_ENEMY[index4] = Enemy_Tiles[index + Tile_Number];
			++index4;
			SPRITE_ENEMY[index4] = Enemy2_Type; //attributes, change later
			++index4;
			SPRITE_ENEMY[index4] = MetaSprite_X[index] + Enemy2x;
			++index4;
		}
	}
	
	if (Enemy3_Type != 0){
		Tile_Number = (Enemy3_Type - 1) << 2; //type of ship
		for (index = 0; index < 4; ++index){
			SPRITE_ENEMY[index4] = MetaSprite_Y[index] + Enemy3y;
			++index4;
			SPRITE_ENEMY[index4] = Enemy_Tiles[index + Tile_Number];
			++index4;
			SPRITE_ENEMY[index4] = Enemy3_Type; //attributes, change later
			++index4;
			SPRITE_ENEMY[index4] = MetaSprite_X[index] + Enemy3x;
			++index4;
		}
	}
	
	if (Enemy4_Type != 0){
		Tile_Number = (Enemy4_Type - 1) << 2; //type of ship
		for (index = 0; index < 4; ++index){
			//index4 = (index << 2);
			SPRITE_ENEMY[index4] = MetaSprite_Y[index] + Enemy4y;
			++index4;
			SPRITE_ENEMY[index4] = Enemy_Tiles[index + Tile_Number];
			++index4;
			SPRITE_ENEMY[index4] = Enemy4_Type; //attributes, change later
			++index4;
			SPRITE_ENEMY[index4] = MetaSprite_X[index] + Enemy4x;
			++index4;
		}
	}
	
	//clean up unused enemy sprites, put them off screen
	while (index4 < 0x40){
		SPRITE_ENEMY[index4] = 0xf8;
		index4 += 4;
	}
}


void move_logic(void) {
	if (injury_Timer == 0){	
		if ((joypad1 & (RIGHT|LEFT)) == 0){ //no L or R
			//apply friction, if no L or R
			if (X_speed > 0){ //if positive, going right
				--X_speed;
			}
			if (X_speed > 0){ //if positive, going right
				--X_speed;
			}
			if (X_speed < 0){ //going left
				++X_speed;;
			}
			if (X_speed < 0){ //going left
				++X_speed;;
			}
		}
		
		if ((joypad1 & RIGHT) > 0){
			if (X_speed >= 0){ //going right
				X_speed += 2;
			}
			else {//going left
				X_speed += 4;//double friction
			}
		}
		if ((joypad1 & LEFT) > 0){
			if (X_speed < 0){ //going left
				X_speed -= 2;
			}
			else {//going right
				X_speed -= 4;//double friction
			}
		}
		
		//physics here
		//max speeds
		if (X_speed >= 0){ //going right
			if (X_speed > 0x34){
				X_speed = 0x34;
			}
		}
		else {
			if (X_speed < (-0x24)){
				X_speed = (-0x24); //0xe0
			}
		}
			

		//move
		X1old = X1;
		X1 += X_speed>>4;
		if (X1 > 0xf0){//too far right or left
			X1 = X1old;
			X_speed = 7; //was 0, but I was getting a pause on Right
		}
		if (X1 == 0) X1 = 1; //fix bullet too far left issue
	}
	
	//scroll up
	--Vert_scroll;
	if (Vert_scroll > 0xef)
		Vert_scroll = 0xef; //screen is only 240 pixels high
	
	//bullets shoot
	if ((bullet_Wait == 0)&&(injury_Timer == 0)){
		if ((joypad1 & (B_BUTTON|A_BUTTON)) > 0){
			if (SFx_Wait == 0){
				Play_Fx(0);
			}
			bullet_Wait = 0x14;
			if (Bullet1y >= 0xf0){ //if y > f0 = it's off
				Bullet1y = 0xca;
				Bullet1x = X1;
			}
			else if (Bullet2y >= 0xf0){
				Bullet2y = 0xca;
				Bullet2x = X1;
			}
			else if (Bullet3y >= 0xf0){
				Bullet3y = 0xca;
				Bullet3x = X1;
			}
			else if (Bullet4y >= 0xf0){
				Bullet4y = 0xca;
				Bullet4x = X1;
			}
			else if (Bullet5y >= 0xf0){
				Bullet5y = 0xca;
				Bullet5x = X1;
			}
			else if (Bullet6y >= 0xf0){
				Bullet6y = 0xca;
				Bullet6x = X1;
			}
			else if (Bullet7y >= 0xf0){
				Bullet7y = 0xca;
				Bullet7x = X1;
			}	
		}
	}
	if(bullet_Wait != 0)
		--bullet_Wait;
	
	//bullets move
	if (Bullet1y < 0xf0){
		--Bullet1y;
		--Bullet1y;
		SPRITE_BULLET[0] = Bullet1y;
		SPRITE_BULLET[4] = Bullet1y;
		SPRITE_BULLET[3] = Bullet1x - 1;
		SPRITE_BULLET[7] = Bullet1x + 13;
		SPRITE_BULLET[1] = 0x12; //which tile
		SPRITE_BULLET[5] = 0x12;
	}
	else {
		SPRITE_BULLET[0] = 0xf8;
		SPRITE_BULLET[4] = 0xf8;
	}
	if (Bullet2y < 0xf0){
		--Bullet2y;
		--Bullet2y;
		SPRITE_BULLET[8] = Bullet2y;
		SPRITE_BULLET[12] = Bullet2y;
		SPRITE_BULLET[11] = Bullet2x - 1;
		SPRITE_BULLET[15] = Bullet2x + 13;
		SPRITE_BULLET[9] = 0x12; //which tile
		SPRITE_BULLET[13] = 0x12;
	}
	else {
		SPRITE_BULLET[8] = 0xf8;
		SPRITE_BULLET[12] = 0xf8;
	}
	if (Bullet3y < 0xf0){
		--Bullet3y;
		--Bullet3y;
		SPRITE_BULLET[16] = Bullet3y;
		SPRITE_BULLET[20] = Bullet3y;
		SPRITE_BULLET[19] = Bullet3x - 1;
		SPRITE_BULLET[23] = Bullet3x + 13;
		SPRITE_BULLET[17] = 0x12; //which tile
		SPRITE_BULLET[21] = 0x12;
	}
	else {
		SPRITE_BULLET[16] = 0xf8;
		SPRITE_BULLET[20] = 0xf8;
	}
	if (Bullet4y < 0xf0){
		--Bullet4y;
		--Bullet4y;
		SPRITE_BULLET[24] = Bullet4y;
		SPRITE_BULLET[28] = Bullet4y;
		SPRITE_BULLET[27] = Bullet4x - 1;
		SPRITE_BULLET[31] = Bullet4x + 13;
		SPRITE_BULLET[25] = 0x12; //which tile
		SPRITE_BULLET[29] = 0x12;
	}
	else {
		SPRITE_BULLET[24] = 0xf8;
		SPRITE_BULLET[28] = 0xf8;
	}
	if (Bullet5y < 0xf0){
		--Bullet5y;
		--Bullet5y;
		SPRITE_BULLET[32] = Bullet5y;
		SPRITE_BULLET[36] = Bullet5y;
		SPRITE_BULLET[35] = Bullet5x - 1;
		SPRITE_BULLET[39] = Bullet5x + 13;
		SPRITE_BULLET[33] = 0x12; //which tile
		SPRITE_BULLET[37] = 0x12;
	}
	else {
		SPRITE_BULLET[32] = 0xf8;
		SPRITE_BULLET[36] = 0xf8;
	}
	if (Bullet6y < 0xf0){
		--Bullet6y;
		--Bullet6y;
		SPRITE_BULLET[40] = Bullet6y;
		SPRITE_BULLET[44] = Bullet6y;
		SPRITE_BULLET[43] = Bullet6x - 1;
		SPRITE_BULLET[47] = Bullet6x + 13;
		SPRITE_BULLET[41] = 0x12; //which tile
		SPRITE_BULLET[45] = 0x12;
	}
	else {
		SPRITE_BULLET[40] = 0xf8;
		SPRITE_BULLET[44] = 0xf8;
	}
	if (Bullet7y < 0xf0){
		--Bullet7y;
		--Bullet7y;
		SPRITE_BULLET[48] = Bullet7y;
		SPRITE_BULLET[52] = Bullet7y;
		SPRITE_BULLET[51] = Bullet7x - 1;
		SPRITE_BULLET[55] = Bullet7x + 13;
		SPRITE_BULLET[49] = 0x12; //which tile
		SPRITE_BULLET[53] = 0x12;
	}
	
	
	//move enemy ships
	if (Game_Mode == RUN_GAME_MODE){
		temp2 = (Frame_Count & 1);//0 or 1
		temp = temp2 + 2;//2 or 3
		if(Enemy1_Type == 1) {
			Enemy1y += 2;
			if((Frame_Count & 0x1f) > 15){
				Enemy1x += temp2;;
			}
			else{
				Enemy1x -= temp2;
			}
		}
		if(Enemy1_Type == 2) {
			Enemy1y += temp;
			if((Frame_Count & 0x01) == 1){
				if (X1 > Enemy1x){
					++Enemy1x;
				}
				else{
					--Enemy1x;
				}
			}
		}
		if (Enemy1y > 0xf0){
			Enemy1_Type = 0;
		}
			
		if(Enemy2_Type == 1) {
			Enemy2y += 2;
			if((Frame_Count & 0x1f) > 15){
				Enemy2x += temp2;;
			}
			else{
				Enemy2x -= temp2;
			}
		}
		if(Enemy2_Type == 2) {
			Enemy2y += temp;
			if((Frame_Count & 0x01) == 1){
				if (X1 > Enemy2x){
					++Enemy2x;
				}
				else{
					--Enemy2x;
				}
			}
		}
		if (Enemy2y > 0xf0)
			Enemy2_Type = 0;
		
		if(Enemy3_Type == 1) {
			Enemy3y += 2;
			if((Frame_Count & 0x1f) > 15){
				Enemy3x += temp2;
			}
			else{
				Enemy3x -= temp2;
			}
		}
		if(Enemy3_Type == 2) {
			Enemy3y += temp;
			if((Frame_Count & 0x01) == 1){
				if (X1 > Enemy3x){
					++Enemy3x;
				}
				else{
					--Enemy3x;
				}
			}
		}
		if (Enemy3y > 0xf0)
			Enemy3_Type = 0;
		
		if(Enemy4_Type == 1) {
			Enemy4y += 2;
			if((Frame_Count & 0x1f) > 15){
				Enemy4x += temp2;
			}
			else{
				Enemy4x -= temp2;
			}
		}
		if(Enemy4_Type == 2) {
			Enemy4y += temp;
			if((Frame_Count & 0x01) == 1){
				if (X1 > Enemy4x){
					++Enemy4x;
				}
				else{
					--Enemy4x;
				}
			}
		}
		if (Enemy4y > 0xf0){
			Enemy4_Type = 0;
		}
	}
}


void Draw_Title(void) {
	PPU_ADDRESS = 0x20; //address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	BGD_address = (int)&Title;
	UnRLE(BGD_address);	//uncompresses our data
	
	PPU_ADDRESS = 0x24; //draw the HUD on opposite nametable
	PPU_ADDRESS = 0x64;
	for(index=0;index < sizeof(HUD);++index){
		PPU_DATA = HUD[index];
	}
	PPU_ADDRESS = 0x24;
	PPU_ADDRESS = 0x74;
	for(index=0;index < sizeof(HUD);++index){
		PPU_DATA = HUD2[index];
	}
	//attribute table HUD
	PPU_ADDRESS = 0x27;
	PPU_ADDRESS = 0xc0;
	for(index=0;index < 8;++index){
		PPU_DATA = 0xff;
	}
}


void Draw_Background(void) {
	All_Off();
	PPU_ADDRESS = 0x20; //address of nametable #0 = 0x2000
	PPU_ADDRESS = 0x00;
	BGD_address = (int)&Stars;
	UnRLE(BGD_address);	//uncompresses our data
	PPU_ADDRESS = 0x3f; //address of BG#0 Palette
	PPU_ADDRESS = 0x01;
	PPU_DATA = 3;
	PPU_DATA = 0x27;
	PPU_DATA = 5;
	Wait_Vblank();
	SCROLL = 0;
	SCROLL = 0;
	PPU_CTRL = 0x91; //screen is on, NMI on
	PPU_MASK = 0x1e;
}


void Turn_On_Pause_Sprites(void){
	SPRITE_TEXT[0] = 0x70;
	SPRITE_TEXT[1] = (char)'P';
	SPRITE_TEXT[2] = 0;
	SPRITE_TEXT[3] = 0x70;
	SPRITE_TEXT[4] = 0x70;
	SPRITE_TEXT[5] = (char)'a';
	SPRITE_TEXT[6] = 0;
	SPRITE_TEXT[7] = 0x78;
	SPRITE_TEXT[8] = 0x70;
	SPRITE_TEXT[9] = (char)'u';
	SPRITE_TEXT[10] = 0;
	SPRITE_TEXT[11] = 0x80;
	SPRITE_TEXT[12] = 0x70;
	SPRITE_TEXT[13] = (char)'s';
	SPRITE_TEXT[14] = 0;
	SPRITE_TEXT[15] = 0x88;
	SPRITE_TEXT[16] = 0x70;
	SPRITE_TEXT[17] = (char)'e';
	SPRITE_TEXT[18] = 0;
	SPRITE_TEXT[19] = 0x90;
}


void Turn_On_Game_Over_Sprites(void){
	SPRITE_TEXT[0] = 0x70;
	SPRITE_TEXT[1] = (char)'G';
	SPRITE_TEXT[2] = 0;
	SPRITE_TEXT[3] = 0x60;
	SPRITE_TEXT[4] = 0x70;
	SPRITE_TEXT[5] = (char)'a';
	SPRITE_TEXT[6] = 0;
	SPRITE_TEXT[7] = 0x68;
	SPRITE_TEXT[8] = 0x70;
	SPRITE_TEXT[9] = (char)'m';
	SPRITE_TEXT[10] = 0;
	SPRITE_TEXT[11] = 0x70;
	SPRITE_TEXT[12] = 0x70;
	SPRITE_TEXT[13] = (char)'e';
	SPRITE_TEXT[14] = 0;
	SPRITE_TEXT[15] = 0x78;
	SPRITE_TEXT[16] = 0x70;
	SPRITE_TEXT[17] = (char)'O';
	SPRITE_TEXT[18] = 0;
	SPRITE_TEXT[19] = 0x88;
	SPRITE_TEXT[20] = 0x70;
	SPRITE_TEXT[21] = (char)'v';
	SPRITE_TEXT[22] = 0;
	SPRITE_TEXT[23] = 0x90;
	SPRITE_TEXT[24] = 0x70;
	SPRITE_TEXT[25] = (char)'e';
	SPRITE_TEXT[26] = 0;
	SPRITE_TEXT[27] = 0x98;
	SPRITE_TEXT[28] = 0x70;
	SPRITE_TEXT[29] = (char)'r';
	SPRITE_TEXT[30] = 0;
	SPRITE_TEXT[31] = 0xa0;
}


void Turn_On_Victory_Sprites(){
	SPRITE_TEXT[0] = 0x70;
	SPRITE_TEXT[1] = (char)'V';
	SPRITE_TEXT[2] = 0;
	SPRITE_TEXT[3] = 0x60;
	SPRITE_TEXT[4] = 0x70;
	SPRITE_TEXT[5] = (char)'i';
	SPRITE_TEXT[6] = 0;
	SPRITE_TEXT[7] = 0x68;
	SPRITE_TEXT[8] = 0x70;
	SPRITE_TEXT[9] = (char)'c';
	SPRITE_TEXT[10] = 0;
	SPRITE_TEXT[11] = 0x70;
	SPRITE_TEXT[12] = 0x70;
	SPRITE_TEXT[13] = (char)'t';
	SPRITE_TEXT[14] = 0;
	SPRITE_TEXT[15] = 0x78;
	SPRITE_TEXT[16] = 0x70;
	SPRITE_TEXT[17] = (char)'o';
	SPRITE_TEXT[18] = 0;
	SPRITE_TEXT[19] = 0x80;
	SPRITE_TEXT[20] = 0x70;
	SPRITE_TEXT[21] = (char)'r';
	SPRITE_TEXT[22] = 0;
	SPRITE_TEXT[23] = 0x88;
	SPRITE_TEXT[24] = 0x70;
	SPRITE_TEXT[25] = (char)'y';
	SPRITE_TEXT[26] = 0;
	SPRITE_TEXT[27] = 0x90;
	SPRITE_TEXT[28] = 0x70;
	SPRITE_TEXT[29] = (char)'!';
	SPRITE_TEXT[30] = 0;
	SPRITE_TEXT[31] = 0x98;
}


void Set_Sprite_Zero(void){
	SPRITE_ZERO[0] = 0x1d;
	SPRITE_ZERO[1] = 0xff;
	SPRITE_ZERO[2] = 0x20;
	SPRITE_ZERO[3] = 0xb9;
}


void All_Collisions(void){
	
	//note, the x and y are assumed to be the left and top side
	
	objectA_right = objectAx + sizeAx;
	if (objectA_right < objectAx)
		objectA_right = 0xff;
	
	objectB_right = objectBx + sizeBx;
	if (objectB_right < objectBx)
		objectB_right = 0xff;
	
	objectA_bottom = objectAy + sizeAy;
	if (objectA_bottom < objectAy)
		objectA_bottom = 0xff;
	
	objectB_bottom = objectBy + sizeBy;
	if (objectB_bottom < objectBy)
		objectB_bottom = 0xff;
	
	//the actual collision check
	if (objectAx <= objectB_right && objectA_right >= objectBx && objectAy <= objectB_bottom && objectA_bottom >= objectBy){
		++collision;
		++collision2;
	}
}


void Check_Each_Bullet(void){
	
	if (Bullet1y < 0xf0){
		collision2 = 0;
		objectBx = Bullet1x;
		objectBy = Bullet1y;
		sizeBx = 0x12;
		sizeBy = 0x5;
		All_Collisions();
		if (collision2 != 0)
			Bullet1y = 0;
	}
	
	if (Bullet2y < 0xf0){
		collision2 = 0;
		objectBx = Bullet2x;
		objectBy = Bullet2y;
		//sizeBx = 0x12; //redundant
		//sizeBy = 0x5;
		All_Collisions();
		if (collision2 != 0)
			Bullet2y = 0;
	}

	
	if (Bullet3y < 0xf0){
		collision2 = 0;
		objectBx = Bullet3x;
		objectBy = Bullet3y;
		//sizeBx = 0x12;
		//sizeBy = 0x5;
		All_Collisions();
		if (collision2 != 0)
			Bullet3y = 0;
	}


	if (Bullet4y < 0xf0){
		collision2 = 0;
		objectBx = Bullet4x;
		objectBy = Bullet4y;
		//sizeBx = 0x12;
		//sizeBy = 0x5;
		All_Collisions();
		if (collision2 != 0)
			Bullet4y = 0;
	}

	if (Bullet5y < 0xf0){
		collision2 = 0;
		objectBx = Bullet5x;
		objectBy = Bullet5y;
		//sizeBx = 0x12;
		//sizeBy = 0x5;
		All_Collisions();
		if (collision2 != 0)
			Bullet5y = 0;
	}

	if (Bullet6y < 0xf0){
		collision2 = 0;
		objectBx = Bullet6x;
		objectBy = Bullet6y;
		//sizeBx = 0x12;
		//sizeBy = 0x5;
		All_Collisions();
		if (collision2 != 0)
			Bullet6y = 0;
	}

	if (Bullet7y < 0xf0){
		collision2 = 0;
		objectBx = Bullet7x;
		objectBy = Bullet7y;
		//sizeBx = 0x12;
		//sizeBy = 0x5;
		All_Collisions();
		if (collision2 != 0)
			Bullet7y = 0;
	}
}


void collision_Test(void){
	//test if your bullets hit the enemy
	if (Enemy1_Type != 0){ //zero = off
		objectAx = Enemy1x;
		objectAy = Enemy1y;
		sizeAx = 0x10;
		sizeAy = 0x10;
		collision = 0;
		Check_Each_Bullet();
		if (collision != 0){
			Enemy1_Type = 0;
			++Score3;
			Play_Fx(2);
		}
	}
	
	if (Enemy2_Type != 0){
		objectAx = Enemy2x;
		objectAy = Enemy2y;
		//sizeAx = 0x10; //redundant, all enemies are always 16x16
		//sizeAy = 0x10;
		collision = 0;
		Check_Each_Bullet();
		if (collision != 0){
			Enemy2_Type = 0;
			++Score3;
			Play_Fx(2);
		}
	}
	
	if (Enemy3_Type != 0){
		objectAx = Enemy3x;
		objectAy = Enemy3y;
		//sizeAx = 0x10;
		//sizeAy = 0x10;
		collision = 0;
		Check_Each_Bullet();
		if (collision != 0){
			Enemy3_Type = 0;
			++Score3;
			Play_Fx(2);
		}
	}
	
	if (Enemy4_Type != 0){
		objectAx = Enemy4x;
		objectAy = Enemy4y;
		//sizeAx = 0x10;
		//sizeAy = 0x10;
		collision = 0;
		Check_Each_Bullet();
		if (collision != 0){
			Enemy4_Type = 0;
			++Score3;
			Play_Fx(2);
		}
	}

	
	//check if enemy collides with us
	collision = 0;
	objectBx = X1;
	objectBy = Y1;
	sizeAx = 0x10;
	sizeAy = 0x10;
	sizeBx = 0x10;
	sizeBy = 0x12;
	if (Enemy1_Type != 0){
		objectAx = Enemy1x;
		objectAy = Enemy1y;
		All_Collisions();
	}
	if (Enemy2_Type != 0){
		objectAx = Enemy2x;
		objectAy = Enemy2y;
		All_Collisions();
	}
	if (Enemy3_Type != 0){
		objectAx = Enemy3x;
		objectAy = Enemy3y;
		All_Collisions();
	}
	if (Enemy4_Type != 0){
		objectAx = Enemy4x;
		objectAy = Enemy4y;
		All_Collisions();
	}
	if (collision != 0){
		Play_Fx(2);
		injury_Timer = 0x40;
		--Lives;
		if (Lives == 0)
			++Death_Flag;
	}	
}


void Adjust_Score(void){
		//adjust scoreboard
	if (Score3 > 9){
		++Score2;
		Score3 -= 10;
	}
	if (Score2 > 9){
		++Score1;
		Score2 -= 10;
	}
	if (Score1 > 9){
		Score1 = 0;
		Score2 = 0;
		Score3 = 0;
	}
}


void Spawn_Enemies(void){
	temp = 0;
	if ((Frame_Count & 0x3f) == 1) //about every second
		++Master_Clock;
	if (Master_Clock == Spawn_Timing[Spawn_Index]){
		temp = Spawn_Array[Spawn_Index];
		++Spawn_Index;
		if (Spawn_Index > sizeof(Spawn_Timing))
			Spawn_Index = 0;
	}
	if (temp == 1){ //pattern 1
		Enemy1_Type = 1;
		Enemy1x = 0x10;
		Enemy1y = 0x10;
		Enemy2_Type = 1;
		Enemy2x = 0x40;
		Enemy2y = 0;
		Enemy3_Type = 1;
		Enemy3x = 0xb0;
		Enemy3y = 0x10;
		Enemy4_Type = 1;
		Enemy4x = 0xe0;
		Enemy4y = 0;
	}
	if (temp == 2){ //pattern 2
		Enemy1_Type = 1;
		Enemy1x = 0x30;
		Enemy1y = 0x10;
		Enemy2_Type = 1;
		Enemy2x = 0x60;
		Enemy2y = 0;
		Enemy3_Type = 1;
		Enemy3x = 0xa0;
		Enemy3y = 0;
		Enemy4_Type = 1;
		Enemy4x = 0xd0;
		Enemy4y = 0x10;
	}
	if (temp == 3){ //pattern 3
		Enemy1_Type = 2;
		Enemy1x = 0x50;
		Enemy1y = 0;
		Enemy2_Type = 2;
		Enemy2x = 0xb0;
		Enemy2y = 0;
	}
	if (temp == 4){ //pattern 4
		Enemy1_Type = 2;
		Enemy1x = 0x18;
		Enemy1y = 0x10;
		Enemy2_Type = 1;
		Enemy2x = 0x80;
		Enemy2y = 0;
		Enemy3_Type = 2;
		Enemy3x = 0xc8;
		Enemy3y = 0x10;
	}
	if (temp == 5){ //pattern 5
		Enemy1_Type = 2;
		Enemy1x = 0x10;
		Enemy1y = 0x10;
		Enemy2_Type = 2;
		Enemy2x = 0xd8;
		Enemy2y = 8;
		Enemy3_Type = 1;
		Enemy3x = 0x50;
		Enemy3y = 0;
		Enemy4_Type = 2;
		Enemy4x = 0x80;
		Enemy4y = 0x10;
	}
}


const unsigned char Boss_Start_Sprites[] = {
0x28, 0xa0, 2, 0x40,   0x28, 0xa1, 2, 0x48,   0x28, 0xa2, 2, 0x50,
0x28, 0xa3, 2, 0x58,   0x28, 0xa4, 2, 0x60,   0x28, 0xa5, 2, 0x68, 
0x30, 0xb0, 2, 0x40,   0x30, 0xb1, 2, 0x48,   0x30, 0xb2, 2, 0x50,
0x30, 0xb3, 2, 0x58,   0x30, 0xb4, 2, 0x60,   0x30, 0xb5, 2, 0x68, 
0x38, 0xc0, 2, 0x40,   0x38, 0xc1, 2, 0x48,   0x38, 0xc2, 2, 0x50,
0x38, 0xc3, 2, 0x58,   0x38, 0xc4, 2, 0x60,   0x38, 0xc5, 2, 0x68, 
0x40, 0xd0, 2, 0x40,   0x40, 0xd1, 2, 0x48,   0x40, 0xd2, 2, 0x50,
0x40, 0xd3, 2, 0x58,   0x40, 0xd4, 2, 0x60,   0x40, 0xd5, 2, 0x68, 
0x48, 0xe0, 2, 0x40,   0x48, 0xe1, 2, 0x48,   0x48, 0xe2, 2, 0x50,
0x48, 0xe3, 2, 0x58,   0x48, 0xe4, 2, 0x60,   0x48, 0xe5, 2, 0x68,
};


void Boss_Init (void) {
	Boss_Health = 0x20;
	BossX = 40;
	Boss_Laser_1y = 0xf8;
	Boss_Laser_2y = 0xf8;
	Boss_Laser_3y = 0xf8;
	Boss_Laser_4y = 0xf8;
	memcpy (SPRITES2+0x40, Boss_Start_Sprites, sizeof(Boss_Start_Sprites));//0x78
}


void Shuffle_Sprites2(void){ //doesn't shuffle sprites. boss mode.
	memcpy (SPRITES2, SPRITE_BULLET, 0x40);
}


//Boss Moves
void Boss_Moves(void){
	if (Boss_Count == 0){
		++Boss_Move;
		Boss_Count = 0x40;
		if (Boss_Move == 4)
			Boss_Move = 0;
	}
	else{
		--Boss_Count;
	}
	
	if (Boss_Move == 0){
		--BossX;
		--BossX;
	}
	else if (Boss_Move == 1){
		Play_Fx(1);
		SFx_Wait = 0x10;
		Boss_Laser_1x = 0x35;
		Boss_Laser_1y = 0x3a;
		Boss_Laser_2x = 0x48;
		Boss_Laser_2y = 0x3a;
		++Boss_Move;
	}
	else if (Boss_Move == 2){
		++BossX;
		++BossX;
	}
	else if (Boss_Move == 3){
		Play_Fx(1);
		SFx_Wait = 0x10;
		Boss_Laser_3x = 0xb6;
		Boss_Laser_3y = 0x3a;
		Boss_Laser_4x = 0xc8;
		Boss_Laser_4y = 0x3a;
		Boss_Move = 0;
	}
	if(SFx_Wait != 0)
		--SFx_Wait;
	
	//move Boss
	SPRITES2[0x43] = BossX;
	SPRITES2[0x5b] = BossX;
	SPRITES2[0x73] = BossX;
	SPRITES2[0x8b] = BossX;
	SPRITES2[0xa3] = BossX;
	temp = BossX + 8;
	SPRITES2[0x47] = temp;
	SPRITES2[0x5f] = temp;
	SPRITES2[0x77] = temp;
	SPRITES2[0x8f] = temp;
	SPRITES2[0xa7] = temp;
	temp += 8;
	SPRITES2[0x4b] = temp;
	SPRITES2[0x63] = temp;
	SPRITES2[0x7b] = temp;
	SPRITES2[0x93] = temp;
	SPRITES2[0xab] = temp;
	temp += 8;
	SPRITES2[0x4f] = temp;
	SPRITES2[0x67] = temp;
	SPRITES2[0x7f] = temp;
	SPRITES2[0x97] = temp;
	SPRITES2[0xaf] = temp;
	temp += 8;
	SPRITES2[0x53] = temp;
	SPRITES2[0x6b] = temp;
	SPRITES2[0x83] = temp;
	SPRITES2[0x9b] = temp;
	SPRITES2[0xb3] = temp;
	temp += 8;
	SPRITES2[0x57] = temp;
	SPRITES2[0x6f] = temp;
	SPRITES2[0x87] = temp;
	SPRITES2[0x9f] = temp;
	SPRITES2[0xb7] = temp;
	
	//move Lasers
	if (Boss_Laser_1y < 0xf0){
		++Boss_Laser_1y;
		++Boss_Laser_1y;
		SPRITE_TEXT[0] = Boss_Laser_1y;
		SPRITE_TEXT[1] = 0x80;
		SPRITE_TEXT[2] = 1;
		SPRITE_TEXT[3] = Boss_Laser_1x;
		SPRITE_TEXT[4] = Boss_Laser_1y + 8;
		SPRITE_TEXT[5] = 0x90;
		SPRITE_TEXT[6] = 1;
		SPRITE_TEXT[7] = Boss_Laser_1x;
	}
	else {
		SPRITE_TEXT[0] = 0xf8;
		SPRITE_TEXT[4] = 0xf8;
	}
	if (Boss_Laser_2y < 0xf0){
		++Boss_Laser_2y;
		++Boss_Laser_2y;
		if (Boss_Laser_2x < X1){ //one laser chases you
			++Boss_Laser_2x;
		}
		else {
			--Boss_Laser_2x;
		}
		SPRITE_TEXT[8] = Boss_Laser_2y;
		SPRITE_TEXT[9] = 0x80;
		SPRITE_TEXT[10] = 1;
		SPRITE_TEXT[11] = Boss_Laser_2x;
		SPRITE_TEXT[12] = Boss_Laser_2y + 8;
		SPRITE_TEXT[13] = 0x90;
		SPRITE_TEXT[14] = 1;
		SPRITE_TEXT[15] = Boss_Laser_2x;
	}
	else {
		SPRITE_TEXT[8] = 0xf8;
		SPRITE_TEXT[12] = 0xf8;
	}
	if (Boss_Laser_3y < 0xf0){
		++Boss_Laser_3y;
		++Boss_Laser_3y;
		if (Boss_Laser_3x < X1){ //one laser chases you
			++Boss_Laser_3x;
		}
		else {
			--Boss_Laser_3x;
		}
		SPRITE_TEXT[16] = Boss_Laser_3y;
		SPRITE_TEXT[17] = 0x80;
		SPRITE_TEXT[18] = 1;
		SPRITE_TEXT[19] = Boss_Laser_3x;
		SPRITE_TEXT[20] = Boss_Laser_3y + 8;
		SPRITE_TEXT[21] = 0x90;
		SPRITE_TEXT[22] = 1;
		SPRITE_TEXT[23] = Boss_Laser_3x;
	}
	else {
		SPRITE_TEXT[16] = 0xf8;
		SPRITE_TEXT[20] = 0xf8;
	}
	if (Boss_Laser_4y < 0xf0){
		++Boss_Laser_4y;
		++Boss_Laser_4y;
		SPRITE_SHIP[24] = Boss_Laser_4y;
		SPRITE_SHIP[25] = 0x80;
		SPRITE_SHIP[26] = 1;
		SPRITE_SHIP[27] = Boss_Laser_4x;
		SPRITE_SHIP[28] = Boss_Laser_4y + 8;
		SPRITE_SHIP[29] = 0x90;
		SPRITE_SHIP[30] = 1;
		SPRITE_SHIP[31] = Boss_Laser_4x;
	}
	else {
		SPRITE_SHIP[24] = 0xf8;
		SPRITE_SHIP[28] = 0xf8;
	}
}


void collision_Boss (void){
	//test me hitting the boss
	objectAx = BossX;
	objectAy = 0x28;
	sizeAx = 0x30;
	sizeAy = 0x28;
	collision = 0;
	Check_Each_Bullet();
	if (collision != 0){
		--Boss_Health;
	}
	if(Boss_Health == 0){
		Play_Fx(3);
		++Victory_Flag;
	}
	
	//test boss lasers hitting me
	collision = 0;
	objectAx = Boss_Laser_1x;
	objectAy = Boss_Laser_1y;
	sizeAx = 0x3;
	sizeAy = 0x10;
	objectBx = X1;
	objectBy = Y1;
	sizeBx = 0x10;
	sizeBy = 0x12;
	All_Collisions();
	
	objectAx = Boss_Laser_2x;
	objectAy = Boss_Laser_2y;
	sizeAx = 0x3;
	sizeAy = 0x10;
	All_Collisions();
	
	objectAx = Boss_Laser_3x;
	objectAy = Boss_Laser_3y;
	sizeAx = 0x3;
	sizeAy = 0x10;
	All_Collisions();
	
	objectAx = Boss_Laser_4x;
	objectAy = Boss_Laser_4y;
	sizeAx = 0x3;
	sizeAy = 0x10;
	All_Collisions();
	
	if (collision != 0){
		Play_Fx(2);
		injury_Timer = 0x54;
		--Lives;
		if (Lives == 0)
			++Death_Flag;
	}
}


void Restore_Boss_Y(void){//make sure boss doesn't disappear after Pause
//now I'm regretting making the word "Pause" a sprite.
	SPRITES2[0x40] = 0x28;
	SPRITES2[0x44] = 0x28;
	SPRITES2[0x48] = 0x28;
	SPRITES2[0x4c] = 0x28;
	SPRITES2[0x50] = 0x28;
	SPRITES2[0x54] = 0x28;
	SPRITES2[0x58] = 0x30;
	SPRITES2[0x5c] = 0x30;
	SPRITES2[0x60] = 0x30;
	SPRITES2[0x64] = 0x30;
	SPRITES2[0x68] = 0x30;
	SPRITES2[0x6c] = 0x30;
	SPRITES2[0x70] = 0x38;
	SPRITES2[0x74] = 0x38;
	SPRITES2[0x78] = 0x38;
	SPRITES2[0x7c] = 0x38;
	SPRITES2[0x80] = 0x38;
	SPRITES2[0x84] = 0x38;
	SPRITES2[0x88] = 0x40;
	SPRITES2[0x8c] = 0x40;
	SPRITES2[0x90] = 0x40;
	SPRITES2[0x94] = 0x40;
	SPRITES2[0x98] = 0x40;
	SPRITES2[0x9c] = 0x40;
	SPRITES2[0xa0] = 0x48;
	SPRITES2[0xa4] = 0x48;
	SPRITES2[0xa8] = 0x48;
	SPRITES2[0xac] = 0x48;
	SPRITES2[0xb0] = 0x48;
	SPRITES2[0xb4] = 0x48;
}



void main (void) {
	All_Off(); //turn off screen
	Draw_Title();
	X1 = 0x80;
	Y1 = 0xd0; //bottom of screen
	Wait = 0;
	Wait2 = 0;
	Lives = 3;
	joypad1 = 0xff; //fix a bug, reset is wiping joypad1old
	Load_Palette();
	Reset_Scroll();
	
	Reset_Music(); //note, this is famitone init, and I added the music data address. see famitone2.s
	Play_Music(song);//song = 0
	
	Wait_Vblank();
	All_On();
	while (1){ //infinite loop
		while (Game_Mode == TITLE_MODE){ //Title Screen
			while (NMI_flag == 0);//wait till NMI
			NMI_flag = 0;
			every_frame(); //get input
			if (((joypad1old & START) == 0)&&((joypad1 & START) != 0)){
				Game_Mode = RUN_GAME_MODE;
				Set_Sprite_Zero();
				Draw_Background();
				Reset_Music();
				song = 1;
				Play_Music(song);
			}
			
			Music_Update();
		}
		
		
		while (Game_Mode == RUN_GAME_MODE){ //Game Mode
			while (NMI_flag == 0);//wait till NMI
			NMI_flag = 0;
			every_frame2();	//should be done first every v-blank
			Vert_scroll2 = ((Vert_scroll & 0xF8) << 2);
			Sprite_Zero();
			PPU_ADDRESS = 0;
			SCROLL = Vert_scroll;
			SCROLL = 0;
			PPU_ADDRESS = Vert_scroll2;
			
			
			Spawn_Enemies();
			move_logic();
			if (injury_Timer == 0)
				collision_Test();
			Adjust_Score();
			update_Sprites();
			Shuffle_Sprites();
			Music_Update();
			if (((joypad1old & START) == 0)&&((joypad1 & START) != 0)){
				Blank_sprite3();
				Turn_On_Pause_Sprites();
				Reset_Music();
				song = 2;
				Play_Music(song);
				Game_Mode = PAUSE_MODE;
			}
			
			if (Master_Clock >= 0x72){
				Blank_sprite3();
				Set_Sprite_Zero();
				Boss_Init();
				Game_Mode = BOSS_MODE;
			}
			
			if (Death_Flag != 0){
				Blank_sprite3();
				Turn_On_Game_Over_Sprites();
				Reset_Music();
				song = 3;
				Play_Music(song);
				Game_Mode = GAME_OVER_MODE;
			}
			//PPU_MASK = 0x1f; //for debugging
			//++*((unsigned char*)0xff); //for debugging, scanline 171, still ok
		}

		
		while (Game_Mode == PAUSE_MODE){ //Pause	
			while (NMI_flag == 0);//wait till NMI
			NMI_flag = 0;
			every_frame();
			Music_Update();
			if (((joypad1old & START) == 0)&&((joypad1 & START) != 0)){
				Blank_sprite3();
				Set_Sprite_Zero();
				Reset_Music();
				song = 1;
				Play_Music(song);
				if (Master_Clock < 0x72){
					Game_Mode=RUN_GAME_MODE;
				}
				else {
					Game_Mode=BOSS_MODE;
					Restore_Boss_Y();
				}
			}
		}
		
		
		while (Game_Mode == GAME_OVER_MODE){ //Game Over
			while (NMI_flag == 0);//wait till NMI
			NMI_flag = 0;
			every_frame();
			Music_Update();
			++Wait;
			if (Wait == 0)
				++Wait2;
			if (Wait2 == 2){
				All_Off();
				Reset();
			}
		}
		
		
		while (Game_Mode == VICTORY_MODE){ //Victory
			while (NMI_flag == 0);//wait till NMI
			NMI_flag = 0;
			every_frame();
			Music_Update();
			if (((joypad1old & START) == 0)&&((joypad1 & START) != 0)){
				All_Off();
				Reset();
			}	
		}
		
		
		while (Game_Mode == BOSS_MODE){ //Boss Mode
			while (NMI_flag == 0);//wait till NMI
			NMI_flag = 0;
			every_frame2();	//should be done first every v-blank
			Vert_scroll2 = ((Vert_scroll & 0xF8) << 2);
			Sprite_Zero();
			PPU_ADDRESS = 0;
			SCROLL = Vert_scroll;
			SCROLL = 0;
			PPU_ADDRESS = Vert_scroll2;
			
			if (Victory_Flag == 0){
				move_logic();
				Boss_Moves();
				if (injury_Timer == 0)
					collision_Boss();
				update_Sprites();
				Shuffle_Sprites2();
				Music_Update();
				if (((joypad1old & START) == 0)&&((joypad1 & START) != 0)){
					Blank_sprite3();
					Turn_On_Pause_Sprites();
					Reset_Music();
					song = 2;
					Play_Music(song);
					Game_Mode = PAUSE_MODE;
				}
			}
			
			if (Victory_Flag > 0){ //wait 3 seconds
				++Victory_Flag;
				if ((Frame_Count & 3) == 0){
					++Score3;//keep adding to the score
					Adjust_Score();
				}
			}
				
			if ((Victory_Flag & 3)>1){ //flicker the screen
				PPU_MASK = 0xfe;//set all emphasis bits = darker
			}
			
			if (Victory_Flag >= 0xc0){ //Victory
				All_Off();
				Blank_sprite3();
				Turn_On_Victory_Sprites();
				Reset_Music();
				song = 4;
				Play_Music(song);
				All_On();
				Game_Mode = VICTORY_MODE;
			}
			
			if (Death_Flag != 0){
				Blank_sprite3();
				Turn_On_Game_Over_Sprites();
				Reset_Music();
				song = 3;
				Play_Music(song);
				Game_Mode = GAME_OVER_MODE;
			}
		}
	}
}
	
//inside the startup code, the NMI routine will ++NMI_flag and ++Frame_Count at each V-blank
