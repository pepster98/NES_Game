; iNES Header
.include "header.asm"
.include "ram.asm"
.include "cool_macros.asm"	; Some useful macros

; ============================
; PRG bank F
;=============================
.segment "BANKF"
.include "player.asm"
.include "enemy.asm"
.include "nes.asm"
.include "misc.asm"
.include "sprite.asm"
.include "utils.asm"
.include "bg.asm"
.include "weapons.asm"

;; Turn on the NES
.include "initNes.asm"

; =============================================================================
; ====                            Program Begin                            ====
; =============================================================================

main_entry:
	jsr misc_init
	jsr spr_init
	
	lda #$01
	sta rand_seed

title_screen:
	
	jsr wait_nmi	; frame sync
	
	ppu_disable	; turn off the PPU
	
	bank_load #$0D	; load bank D
	ldx #<title_comp	; load the high byte of the screen
	ldy #>title_comp	; load the low byte of the screen
	lda #$20		; load the address to store the screen data
	jsr unpack_nt		; unpack the nametable into $2000
	
	bank_load #$0E	;load bank E
	;get palette in there
	ppu_load_bg_palette text_palette
	;writing these backwards because I messed up...
	ppu_write_8kbit text_chr, #$00		; write text grpahics data
	ppu_write_8kbit text_chr + $0400, #$04
	ppu_write_32kbit chr_data, #$10		;load in sprite chr
	bank_load #$00	; load back into bank 0
	
	lda #$00
	sta temp8
	
	ppu_enable	;turn on the PPU

@wait_nostart:
	jsr read_joy_safe	; read joypads
	jsr wait_nmi 		; frame sync
	jsr read_joy_safe	; read joypads
	bne @wait_nostart	; if start isnt pressed, loop @wait_nostart
	jsr wait_nmi
		
@title_screen_top_loop:
	;Logic
	key_isdown pad_1, btn_start
	jmp start_game
:
	
	jsr wait_nmi_srand
	
	;VBlank
	ppu_disable
	spr_dma
	ppu_load_scroll xscroll, yscroll
	ppu_enable
	jmp @title_screen_top_loop
	
	
start_game:
	ppu_disable	; turn of the PPU

	bank_load #$0D	; load in bank D
	ldx #<house	; load the high byte of the level nametable
	ldy #>house ; load the low byte of the level nametable
	lda #$20		; load the address of the level's nametable
	jsr unpack_nt	; unpack the compressed nametable into $2000
	ppu_load_attribute house_atr, #$23 ;load the attibute table

	bank_load #$0E	;load in bank E
	ppu_load_bg_palette palette_data	; load the background palette
	ppu_load_spr_palette spr_palette	; load the sprite palette data
	ppu_write_32kbit chr_data, #$10		; load sprite chr
	ppu_write_32kbit bg_data, #$00		; load in the NESTTLESET
	ppu_load_scroll xscroll, yscroll
	bank_load #$00	;load back into bank 0

	jsr player_init		; initialize our player
	jsr enemy_init		;initialize an enemy
	jsr enemy_init
	jsr enemy_init
	
	ppu_enable		; turn on the PPU
	

main_top_loop:

	; Run game logic here
	;jsr read_joy_safe
	jsr strobe_controllers
	
	inc randomDirection2
	jsr random
	
	; update the player
	lda #$00
	sta sprite_idx
	sta sprite_ptrNumber
	; handle the player input
	jsr player_handle_input
	; do the player updates
	jsr sprite_animation
	jsr Sprite_Loading
	jsr update_sprite_position

	jsr transfer_location_info

	jsr arrow

; weapon delays
arrow_dec:
	lda A_timer
	beq :+
	dec A_timer
:
	jsr sword

sword_dec:
	lda B_timer
	beq :+
	dec B_timer
:
	jsr PC_transfer
	jsr sword_transfer
	jsr arrow_transfer
	
UpdateENEMIES:
	lda #$01	; this loop updates the enemies one at at time
	sta sprite_idx	; start with enemy one
	lda #$02	
	sta sprite_ptrNumber
:
	ldx sprite_idx
	lda #$01
	sta sprite_speed, x
	jsr enemy_update

	jsr sprite_animation
	jsr Sprite_Loading
	jsr update_sprite_position

	ldy #$00	;arrow collision
	jsr enemy_collision
	ldy #$02	;sword collision
	jsr enemy_collision
	ldy #$04	;Playable Character
	jsr enemy_collision

	inc sprite_idx
	inc sprite_ptrNumber
	inc sprite_ptrNumber
	lda sprite_idx
	cmp sprite_count
	bne :-
UpdateEnemiesDone:

	; End of game	logic frame; wait for NMI (vblank) to begin
	jsr wait_nmi
	
	; Commit VRAM updates while PPU is disabled in vblank
	ppu_disable
	
	ppu_load_scroll xscroll, yscroll
	spr_dma
	
	; Re-enable PPU for the start of a new frame
	ppu_enable
	
	jmp main_top_loop; loop forever


;====================================================
; BANK E - Resource Bank
;====================================================
.segment "BANKE"

text_chr:
	.incbin "resources/TEXT.chr"

text_palette:
	.byte $0f, $17, $20, $00, $0f, $30, $26, $05, $0f, $27, $29, $00, $0f, $2c, $2c, $33

chr_data:
	.incbin "resources/GFX.chr"

bg_data:
	.incbin "resources/NESTILESET1.chr"

palette_data:
	.incbin "resources/gfx.pal"

spr_palette:
	.incbin "resources/spr.pal"

;====================================================
; BANK D - Name table and Attribute table Bank
;====================================================
.segment "BANKD"

title_comp:
	.incbin "resources/title_table.bin"
	
house:
	.incbin "resources/house.bin"
	
house_atr:
	.incbin "resources/house.atr"

topfield:
	.incbin "resources/topfield.bin"

topfield_atr:
	.incbin "resources/topfield.atr"

bottomfield:
	.incbin "resources/bottomfield.bin"

bottomfield_atr:
	.incbin "resources/bottomfield.atr"

leftfield:
	.incbin "resources/leftfield.bin"

leftfield_atr:
	.incbin "resources/leftfield.atr"

rightfield:
	.incbin "resources/rightfield.bin"

rightfield_atr:
	.incbin "resources/rightfield.atr"

; These are needed to boot the NES.
.segment "VECTORS"

	.addr	nmi_vector	; Every vblank, this ISR is executed.
	.addr	reset_vector	; On bootup or reset, execution begins here.
	.addr	irq_vector	; Triggered by external hardware in the
				; game cartridge, this ISR is executed. A
				; software break (BRK) will do it as well.
