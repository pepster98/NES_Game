; =============================
; Zero-page and main RAM
; Variables, flags, etc.
; =============================
.segment "ZEROPAGE"
; Fast variables
addr_ptr:	.res 2
temp:		.res 1
temp2:		.res 1
temp3:		.res 1
temp4:		.res 1
temp5:		.res 1
temp6:		.res 1
temp7:		.res 1
temp8:		.res 1
pad_1:		.res 1
pad_1_prev:	.res 1
pad_1_pressed:	.res 1
pad_2:		.res 1
pad_2_prev:	.res 1
pad_2_pressed:	.res 1
rand_seed: 	.res 1
sprite_speed:	.res 8
sprite_dir:	.res 8
sprite_anim:	.res 8
spriteGraphicsPointer: .res 2	;pointer for graphics updates
sprite_pointer: .res 8	;pointer for the graphics data of meta sprites
sprite_idx:	.res 1
sprite_count:	.res 1
sprite_frame:	.res 8
sprite_ptrNumber: .res 1

.segment "RAM"
; Flags for PPU control
ppumask_config:	.res 1
ppuctrl_config:	.res 1
vblank_flag:	.res 1
xscroll:	.res 2
yscroll:	.res 2 

GameState:	.res 2	;current game state, and state that is saved
GameStateOld:	.res 1	; old game state


randomDirection:	.res 1	;random direction counter
randomDirection1:	.res 1	; random direction counter
randomDirection2:	.res 1	; random direction counter

sprite_vertical:	.res 1	; vertical location of the active sprite
sprite_horizontal:	.res 1	; horizontal location of the active sprite

arrow_direction:	.res 1	; direction that the arrow is traveling

A_timer:	.res 1	; timer for the A button
B_timer:	.res 1	;timer for the B button

arrow_vertical:	.res 2	; vertical position of the arrow for collision
sword_vertical:	.res 2	;hammer
PC_vertical:	.res 2	;PC

arrow_horizontal:	.res 2	; horizontal position of the arrow for collision
sword_horizontal:	.res 2 	; hammer
PC_horizontal:		.res 2	;PC

room_idx:	.res 1	;which room to load
Y_Map:		.res 1	;Y map coordinate
X_Map:		.res 1	;X map coordinate

button_table:
btn_a:		.res 1
btn_b:		.res 1
btn_sel:	.res 1
btn_start:	.res 1
btn_up:		.res 1
btn_down:	.res 1
btn_left:	.res 1
btn_right:	.res 1

;=========Constants==========
sprite_frame1 = $0C
sprite_frame2 = $18
sprite_frame3 = $24
sprite_frame4 = $30

Weapons_RAM = $0240	; constant for the weapons updates
arrow_speed = $02

Btimer = $0F	; timer for the hammer
Atimer = $0C	; shooting pose timer for the arrows

;room switching locations
top_exit = $01
bottom_exit = $02
left_exit = $03
right_exit = $04


