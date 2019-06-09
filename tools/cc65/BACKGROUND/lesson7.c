//for cc65, for NES
//constructs a simple metasprite, gets input, and moves him around
//and press 'start' = change background
//doug fraker 2015
//feel free to reuse any code here

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


//Globals
//our startup code initialized all values to zero
#pragma bss-name(push, "ZEROPAGE")
unsigned char NMI_flag;
unsigned char Frame_Count;
unsigned char index;
unsigned char index4;
unsigned char X1;
unsigned char Y1;
unsigned char state;
unsigned char state4;
unsigned char joypad1;
unsigned char joypad1old;
unsigned char joypad1test; 
unsigned char joypad2; 
unsigned char joypad2old;
unsigned char joypad2test;
unsigned char new_background;
unsigned char which_BGD;
int BGD_address;

#pragma bss-name(push, "OAM")
unsigned char SPRITES[256];
//OAM equals ram addresses 200-2ff




const unsigned char PALETTE[]={
0x1f, 0, 0x10, 0x20,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
0x1f, 0x37, 0x24, 1,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0};

const unsigned char MetaSprite_Y[] = {0, 0, 8, 8}; //relative y coordinates

const unsigned char MetaSprite_Tile[] = { //tile numbers
	2, 3, 0x12, 0x13, //right
	0, 1, 0x10, 0x11, //down
	6, 7, 0x16, 0x17, //left
	4, 5, 0x14, 0x15}; //up
	
#define Going_Right 0
#define Going_Down	1
#define Going_Left	2
#define Going_Up	3

const unsigned char MetaSprite_Attrib[] = {0, 0, 0, 0}; //attributes = flipping, palette

const unsigned char MetaSprite_X[] = {0, 8, 0, 8}; //relative x coordinates
//we are using 4 sprites, each one has a relative position from the top left sprite


//these are the 4 backgrounds
//generated with NES Screen Tool, with RLE option on
#include "BG/1.h" //called n1
#include "BG/2.h" //called n2
#include "BG/3.h" //called n3
#include "BG/4.h" //called n4

const int All_Backgrounds[] = {(int) &n1,(int) &n2,(int) &n3,(int) &n4 };
//the address of each background


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
	OAM_DMA = 2;	//push all the sprite data from the ram at 200-2ff to the sprite memory
	PPU_CTRL = 0x90; //screen is on, NMI on
	PPU_MASK = 0x1e;
	SCROLL = 0;
	SCROLL = 0;		//just double checking that the scroll is set to 0
	Get_Input();
}


void update_Sprites (void) {
	state4 = state << 2; //shift left 2 = multiply 4
	index4 = 0;
	for (index = 0; index < 4; ++index ){
		SPRITES[index4] = MetaSprite_Y[index] + Y1; //relative y + master y
		++index4;
		SPRITES[index4] = MetaSprite_Tile[index + state4]; //tile numbers
		++index4;
		SPRITES[index4] = MetaSprite_Attrib[index]; //attributes, all zero here
		++index4;
		SPRITES[index4] = MetaSprite_X[index] + X1; //relative x + master x
		++index4;
	}
}


void move_logic(void) {
	if ((joypad1 & RIGHT) > 0){
		state = Going_Right;
		++X1;
	}
	if ((joypad1 & LEFT) > 0){
		state = Going_Left;
		--X1;
	}
	if ((joypad1 & DOWN) > 0){
		state = Going_Down;
		++Y1;
	}
	if ((joypad1 & UP) > 0){
		state = Going_Up;
		--Y1;
	}
}


void check_Start(void){
	if ( ((joypad1 & START) > 0) && ((joypad1old & START) == 0) ){
		++new_background;
	}
}


void Draw_Background(void) {
	All_Off();
	PPU_ADDRESS = 0x20;
	PPU_ADDRESS = 0x00;
	BGD_address = All_Backgrounds[which_BGD];
	UnRLE(BGD_address);	//uncompresses our data
	Wait_Vblank();	//don't turn on screen until in v-blank
	All_On();
	++which_BGD;
	if (which_BGD == 4) //shuffles between 0-3
		which_BGD = 0;
}


void main (void) {
	All_Off(); //turn off screen
	X1 = 0x7f;
	Y1 = 0x77; //middle of screen
	Load_Palette();
	Reset_Scroll();
	All_On(); //turn on screen
	while (1){ //infinite loop
		while (NMI_flag == 0);//wait till NMI
		NMI_flag = 0;
		if (new_background > 0) {
			Draw_Background();
			new_background = 0;
		}
		every_frame();	//should be done first every v-blank
		move_logic();
		check_Start();
		update_Sprites();
	}
}
	
//inside the startup code, the NMI routine will ++NMI_flag and ++Frame_Count at each V-blank
	