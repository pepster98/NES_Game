.segment "BANKF"
.include "enemy_animation.asm"

randomenemydirection:			;these are random  numbers used with
	.byte $57, $CD, $AF, $05, $BC	;randomDirection2 to make enemies switch dir

enemy_init:
	lda sprite_count
	sta sprite_idx
	ldx sprite_idx
	lda #$01
	sta sprite_speed, x
	sta sprite_dir, x

	lda sprite_idx
	clc
	adc sprite_idx
	sta temp5
	clc
	adc #01
	sta temp6
	
	ldx temp5
	ldy temp6

	lda #<enemy_graphics
	sta sprite_pointer, x
	lda #>enemy_graphics
	sta sprite_pointer, y

	inc sprite_count
	rts

enemy_update:
	ldx sprite_idx

	lda randomDirection2	;check randomDirection2 with the values stored in randomenemydirection
	cmp randomenemydirection, x	;this is in place of a routine you would have in collision detection
	beq:+
	cmp randomenemydirection+1, x
	bne EnemyDone
:
	lda randomDirection
	sta sprite_dir, x
EnemyDone:
	lda sprite_dir, x
	bne :+
	jmp enemy_up
:
	cmp #$01
	bne :+
	jmp enemy_down
:
	cmp #$02
	bne :+
	jmp enemy_right
:
	jmp enemy_left

enemy_up:
	ldx sprite_idx
	inc sprite_anim, x
	lda #$00
	sta sprite_dir, x
	ldy updateConstants, x
	lda OAM_BASE, y
	sec
	sbc sprite_speed, x
	sta OAM_BASE, y
	rts

enemy_down:
	ldx sprite_idx
	inc sprite_anim, x
	lda #$01
	sta sprite_dir, x
	ldy updateConstants, x
	lda OAM_BASE, y
	clc
	adc sprite_speed, x
	sta OAM_BASE, y
	rts

enemy_right:
	ldx sprite_idx
	inc sprite_anim, x
	lda #$02
	sta sprite_dir, x
	ldy updateConstants, x
	lda OAM_BASE+3, y
	clc
	adc sprite_speed, x
	sta OAM_BASE+3, y

	rts

enemy_left:
	ldx sprite_idx
	inc sprite_anim, x
	lda #$03
	sta sprite_dir, x
	ldy updateConstants, x
	lda OAM_BASE+3, y
	sec
	sbc sprite_speed, x
	sta OAM_BASE+3, y
	rts

; change the enemy's direction
; random routine called when a collision occurs
direction_change:
	ldx sprite_idx
	ldy updateConstants, x
	lda OAM_BASE, y
	sec
	sbc #$0A
	sta OAM_BASE, y
	rts


enemy_collision:	;check collision with the arrows, sword, PC
	ldx sprite_idx	; load the sprite number
	lda updateConstants, x	;check the outline of the enmey vs the weapon
	tax			;change the constant to the x variable
	lda OAM_BASE, x	;load the vertical position of the dominate enemy sprite
	clc			; make sure that the carry flag is clear
	adc #$01		;add the offset for the up direction for the NPC
	cmp arrow_vertical+1, y	;compare the top of the NPC with the bottom of the weapon in question
	bcc :+			;if the top of the NPC box is above the bottom of the weapon box, branch to :+
	rts			;else, RTS
:
	lda OAM_BASE, x	; load the vertical sprite position
	clc
	adc #$0E	; add the offset for the bottom of the metasprite
	cmp arrow_vertical, y	;compare with the top of the weapon blui box
	bcs :+	;if the bottom of the NPC box is below the top of the weapons box, branch to :+
	rts	; else, RTS
:	; so far we have determined that at least 1 pixel of the defined collision boxes share the same vertical space on the screen, now we check horizonatl
	lda OAM_BASE+3, x	; load the sprite horizontal position
	clc
	adc #$03	; add the offset to the left side of the metasprite
	cmp arrow_horizontal+1, y	; compare this with the right side of the weapon sprite
	bcc :+	;if the left side of the NPC box is to the left of the right side of the weapons box, branch to :+
	rts	; else, RTS
:
	lda OAM_BASE+3, x
	clc
	adc #$0C	; add the defined offset to get the right side of the enemy metasprite
	cmp arrow_horizontal, y	;compare to the left side of the weapons sprite
	bcs :+	;if the right side of the NPC is to the right of the left side of the weapons box, branch to :+
	rts	;else, RTS
:	; we have determined that they share the same horzontal space
	; now that we've confirmed a hit
	cpy #$00	; we need to decide what weapon hit. $00 is for arrows
	bne :+
	jmp arrow_hit	; do the arrow collision stuff
:
	cpy #$02	;was it the mace?
	bne :+
	jmp sword_hit
:
	jmp PC_hit	; all thats left is the PC impact
;;;;;;;
; if the enemy is hit by a sword
sword_hit:

	jsr direction_change

	lda #$01	; set the sword timer to $01 to reset the sword graphics
	sta B_timer
	rts

arrow_hit:
	lda #$FE	; make the arrow inactive
	sta Weapons_RAM+4
	sta Weapons_RAM+7

	jsr direction_change
	rts

PC_hit:
	jsr direction_change
	rts

