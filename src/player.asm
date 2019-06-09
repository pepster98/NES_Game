.segment "BANKF"
.include "player_animation.asm"

player_init:
	
	lda #$00
	sta sprite_idx
	ldx sprite_idx
	lda #$01				;load 1
	sta sprite_speed, x		; sprite_speed(x) = 1
	sta sprite_dir, x		; sprite_dir(x) = 1
	;set the sprite graphics pointer
	lda #<player_graphics
	sta sprite_pointer
	lda #>player_graphics
	sta sprite_pointer+1

	inc sprite_count
	rts


player_handle_input:

ReadA:
	lda pad_1_pressed
	and #%10000000
	beq :+

	lda Weapons_RAM+4	; see if the arrow is ready to fire again
	cmp #$FE
	bne :+
	
	lda #Atimer	;reset the timer so that the arrow is fired next frame
	sta A_timer
:

ReadB:
	lda pad_1_pressed
	and #%01000000
	beq :+

	lda B_timer	;check and see if the sword is still out
	bne :+

	lda #Btimer	;reset the timer if it was not pressed last time, but is now
	sta B_timer
:

ReadSelect:
	lda pad_1_pressed
	and #%00100000
	beq :+
:

ReadStart:
	lda pad_1_pressed
	and #%00010000
	beq :+
	; pause logic here
:

ReadUp:
	lda pad_1
	and #%00001000
	beq :+
	jsr playerMoveUp

	jmp HandleInputDone
:

ReadDown:
	lda pad_1
	and #%00000100
	beq :+
	jsr playerMoveDown

	jmp HandleInputDone
:

ReadLeft:
	lda pad_1
	and #%00000010
	beq :+
	jsr playerMoveLeft

	jmp HandleInputDone
:
	
ReadRight:
	lda pad_1
	and #%00000001
	beq HandleInputDone
	jsr playerMoveRight

HandleInputDone:

	rts

playerMoveUp:
	lda #$00
	sta sprite_dir

	lda OAM_BASE
	sec
	sbc sprite_speed
	sta OAM_BASE

	inc sprite_anim

	rts

playerMoveDown:
	lda #$01
	sta sprite_dir

	lda OAM_BASE
	clc
	adc sprite_speed
	sta OAM_BASE
	
	inc sprite_anim

	rts

playerMoveRight:
	lda #$02
	sta sprite_dir

	lda OAM_BASE+3
	clc
	adc sprite_speed
	sta OAM_BASE+3
	
	inc sprite_anim

	rts

playerMoveLeft:
	lda #$03
	sta sprite_dir

	lda OAM_BASE+3
	sec
	sbc sprite_speed
	sta OAM_BASE+3

	inc sprite_anim

	rts
