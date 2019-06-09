.segment "BANKF"

update_sprite_position:
	ldx sprite_idx
	lda updateConstants, x
	tax
	;---Vertical Updates---
	lda OAM_BASE, x
	sta OAM_BASE+4, x
	clc
	adc #$08
	sta OAM_BASE+8, x
	sta OAM_BASE+12, x
	;---Horizontal Updates---
	lda OAM_BASE+3, x
	sta OAM_BASE+11, x
	clc
	adc #$08
	sta OAM_BASE+7, x
	sta OAM_BASE+15, x

	rts

sprite_animation:
	ldx sprite_idx

	lda sprite_anim, x
	cmp #sprite_frame1
	bne :+
	lda #$00
	sta sprite_frame, x
	jmp animDone
:
	cmp #sprite_frame2
	bne :+
	lda #$01
	sta sprite_frame, x
	jmp animDone
:
	cmp #sprite_frame3
	bne :+
	lda #$00
	sta sprite_frame, x
	jmp animDone
:
	cmp #sprite_frame4
	bne :+
	lda #$02
	sta sprite_frame, x
animDone:
	lda sprite_anim, x
	cmp #sprite_frame4
	bne :+
	sec
	sbc #sprite_frame4
	sta sprite_anim, x
:
	rts


;animation directional indicators.
NM_up:
	.byte $00, $08, $10
NM_down:
	.byte $18, $20, $28
NM_right:
	.byte $30, $38, $40
NM_left:
	.byte $48, $50, $58
	
	
;move a sprite up
sprite_up:
	ldx sprite_idx
	inc sprite_anim, x
	ldy updateConstants,x
	lda #$00
	sta sprite_dir, x
	lda OAM_BASE,y
	sec
	sbc sprite_speed, x
	sta OAM_BASE,y
	rts

;move a sprite down
sprite_down:
	ldx sprite_idx
	inc sprite_anim, x
	lda #$01
	sta sprite_dir, x
	ldy updateConstants,x
	lda OAM_BASE,y
	clc
	adc sprite_speed, x
	sta OAM_BASE,y
	rts

;move a sprite right
sprite_right:
	ldx sprite_idx
	inc sprite_anim, x
	lda #$02
	sta sprite_dir, x
	ldy updateConstants,x
	lda OAM_BASE+3,y
	clc
	adc sprite_speed, x
	sta OAM_BASE+3,y
	rts

;move a sprite left
sprite_left:
	ldx sprite_idx	
	inc sprite_anim, x
	lda #$03
	sta sprite_dir, x
	ldy updateConstants,x
	lda OAM_BASE+3,y
	sec
	sbc sprite_speed, x
	sta OAM_BASE+3,y
	rts

PC_up:
	.byte $00,$08,$10                ;the indexes to the various frames in the spritegraphics file
PC_down:
	.byte $18,$20,$28
PC_right:
	.byte $30,$38,$40
PC_left:
	.byte $48,$50,$58


; updates the frame that is displayed for each meta sprite 1,2,1 or 3
Sprite_Loading:
	ldx sprite_idx
	lda sprite_dir,x
	beq next
	cmp #$01
	beq next1
	cmp #$02
	beq next2
	jmp next3
next:
	lda sprite_frame, x
	tax
	lda PC_up, x
	tay
	jmp sprites_update
next1:
	lda sprite_frame, x
	tax
	lda PC_down, x
	tay
	jmp sprites_update
next2:
	lda sprite_frame, x
	tax
	lda PC_right, x
	tay
	jmp sprites_update
next3:
	lda sprite_frame, x
	tax
	lda PC_left, x
	tay
	jmp sprites_update
	
sprites_update:
	ldx sprite_ptrNumber
	lda sprite_pointer, x
	sta spriteGraphicsPointer
	inx
	lda sprite_pointer, x
	sta spriteGraphicsPointer+1
tileChange:
	LDX sprite_idx
  	LDA updateConstants,x
  	TAX
 
	LDA (spriteGraphicsPointer),y   ;read the tile from the "spritegraphics"
	STA OAM_BASE+1, x	;sub file and store it into memeory.
	INY
 	LDA (spriteGraphicsPointer),y
  	STA OAM_BASE+5, x
  	INY
  	LDA (spriteGraphicsPointer),y
  	STA OAM_BASE+9, x
  	INY
  	LDA (spriteGraphicsPointer),y
  	STA OAM_BASE+13, x
  	INY
 	LDA (spriteGraphicsPointer),y
  	STA OAM_BASE+2, x
  	INY
  	LDA (spriteGraphicsPointer),y
 	STA OAM_BASE+6, x
  	INY
  	LDA (spriteGraphicsPointer),y
  	STA OAM_BASE+10, x
  	INY
  	LDA (spriteGraphicsPointer),y
  	STA OAM_BASE+14, x

  	RTS


update_sprites:
	lda #$00	;updates each metasprite one at a time
	sta sprite_idx	;start with sprite 0 (our guy)
	sta sprite_ptrNumber
:
	jsr sprite_animation	;find out which fame the sprite is on.
	jsr Sprite_Loading	;updat the sprite tile graphics
	jsr update_sprite_position	;update the sprites x y
	
	inc sprite_idx	;increment the sprite number for the next iteration
	inc sprite_ptrNumber	;there are address for the gprahics data
	inc sprite_ptrNumber	;keep at 2x the sprite_idx
	lda sprite_idx
	cmp sprite_count
	bne :-

	rts
