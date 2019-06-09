; ========== Transfer the location info of a specific sprite =======
transfer_location_info:
	ldx sprite_idx
	lda updateConstants, x
	tax
	lda OAM_BASE, x
	sta sprite_vertical
	lda OAM_BASE+3, x
	sta sprite_horizontal
	rts

restore_location_info:
	ldx sprite_idx
	lda updateConstants, x
	tax
	lda sprite_vertical
	sta OAM_BASE, x
	lda sprite_horizontal
	sta OAM_BASE+3, x
	rts

;;; outlines of the arrowhead
arrow_a:
	.byte $00, $00, $01, $01
arrow_b:
	.byte $08, $08, $06, $06
arrow_c:
	.byte $01, $01, $00, $00
arrow_d:
	.byte $06, $06, $08, $08
;;;
arrow_transfer:
	ldy arrow_direction	;transfer arrow collision coordinates
	lda Weapons_RAM+4
	clc
	adc arrow_a, y
	sta arrow_vertical
	lda Weapons_RAM+4
	clc
	adc arrow_b, y
	sta arrow_vertical+1
	lda Weapons_RAM+7
	clc
	adc arrow_c, y
	sta arrow_horizontal
	lda Weapons_RAM+7
	clc
	adc arrow_d, y
	sta arrow_horizontal+1
	rts


; ----- sword outlines
sword_a:	; outlines for the sword
	.byte $01, $03, $00, $00
sword_b:
	.byte $05, $03, $05, $05
sword_c:
	.byte $00, $00, $03, $01
sword_d:
	.byte $05, $05, $03, $01
;;;

sword_transfer:				; transfer sword collision coordinates
	ldy sprite_dir
	lda Weapons_RAM
	clc
	adc sword_a, y
	sta sword_vertical
	lda Weapons_RAM
	clc
	adc sword_b, y
	sta sword_vertical+1
	lda Weapons_RAM+3
	clc
	adc sword_c, y
	sta sword_horizontal
	lda Weapons_RAM+3
	clc
	adc sword_d, y
	sta sword_horizontal+1
	rts

PC_transfer:
	lda OAM_BASE
	clc
	adc #$01
	sta PC_vertical
	lda OAM_BASE
	clc
	adc #$0E
	sta PC_vertical+1
	lda OAM_BASE+3
	clc
	adc #$03
	sta PC_horizontal
	lda OAM_BASE+3
	clc
	adc #$0C
	sta PC_horizontal+1
	rts

; ==================== Arrow Logic ======================
arrow:
	lda Weapons_RAM+4	; see if the arrow is active
	cmp #$FE
	BNE :+
	jmp loadarrow
:
	lda arrow_direction
	bne :+
	lda Weapons_RAM+4
	sec
	sbc #arrow_speed
	sta Weapons_RAM+4

	jsr arrow_collision
	jmp loadarrow
:
	cmp #$01
	bne :+			;moving down
	lda Weapons_RAM+4
	clc
	adc #arrow_speed
	sta Weapons_RAM+4

	jsr arrow_collision

	jmp loadarrow
:
	cmp #$02
	bne :+			; moving right
	lda Weapons_RAM+7
	clc
	adc #arrow_speed
	sta Weapons_RAM+7

	jsr arrow_collision
	
	jmp loadarrow
:
	lda Weapons_RAM+7
	sec
	sbc #arrow_speed
	sta Weapons_RAM+7

	jsr arrow_collision

	jmp loadarrow
loadarrow:
	lda A_timer
	cmp #Atimer
	beq :+
	jmp graphics
:
	lda sprite_dir
	sta arrow_direction
	cmp #$00
	bne :+
	
	lda #$22	; load the UP graphics
	sta Weapons_RAM+5
	lda #%00000001
	sta Weapons_RAM+6
	
	lda sprite_vertical
	sec
	sbc #$08
	sta Weapons_RAM+4

	lda sprite_horizontal
	sec
	sbc #$01
	sta Weapons_RAM+7
	jmp graphics
:
	cmp #$01
	bne :+

	lda #$20	; load the DOWN graphics
	sta Weapons_RAM+5
	lda #%00000001
	sta Weapons_RAM+6

	lda sprite_vertical
	clc
	adc #$0D
	sta Weapons_RAM+4

	lda sprite_horizontal
	sec
	sbc #$01
	sta Weapons_RAM+7

	jmp graphics
:
	cmp #$02
	bne :+

	lda #$21		; the RIGHT Graphics
	sta Weapons_RAM+5
	lda #%00000001
	sta Weapons_RAM+6

	lda sprite_vertical
	clc
	adc #$05
	sta Weapons_RAM+4

	lda sprite_horizontal
	clc
	adc #$10
	sta Weapons_RAM+7

	jmp graphics
:
	lda #$21		; the left graphics
	sta Weapons_RAM+5
	lda #%01000001
	sta Weapons_RAM+6

	lda sprite_vertical
	clc
	adc #$05
	sta Weapons_RAM+4

	lda sprite_horizontal
	sec
	sbc #$08
	sta Weapons_RAM+7

	jmp graphics

graphics:			; make him look like he is shooting
	lda A_timer
	beq :+
	jsr shooting_graphics
:
	rts
;--------------------------------------------------------------------------

arrow_collision:
	lda Weapons_RAM+4	; arrow collision detection with walls
	cmp #$08
	bcc :+
	cmp #$E8
	bcs :+
	lda Weapons_RAM+7
	cmp #$02
	bcc :+
	cmp #$FE
	bcs :+
	jmp end
:
	lda #$FE
	sta Weapons_RAM+4
	STA Weapons_RAM+7
end:
	rts

;--------------------------------------------------------------------------

sword:		; graphics updates for the sword
	lda B_timer
	bne :+
	rts
:
	jsr shooting_graphics
	
	lda B_timer
	cmp #$01
	bne :+
	lda #$FE
	sta Weapons_RAM
	sta Weapons_RAM+3
	rts
:
	lda sprite_dir
	cmp #$00
	bne :+

	lda #%00000001
	sta Weapons_RAM+2
	lda #$22			;update the UP graphics
	sta Weapons_RAM+1

	lda sprite_vertical	; save sprite Y Position
	sec
	sbc #$07
	sta Weapons_RAM

	lda sprite_horizontal	; load sprite X Position
	sec
	sbc #$00
	sta Weapons_RAM+3
	rts
:
	cmp #$01
	bne :+

	lda #%00000001
	sta Weapons_RAM+2
	lda #$20
	sta Weapons_RAM+1

	lda sprite_vertical	; save sprite Y position
	clc
	adc #$0F
	sta Weapons_RAM

	lda sprite_horizontal 	;load sprite X position
	clc
	adc #$00
	sta Weapons_RAM+3
	rts
:
	cmp #$02
	bne :+

	lda #$00000001		; update the Right graphics
	sta Weapons_RAM+2
	lda #$21
	sta Weapons_RAM+1

	lda sprite_vertical	; save sprite Y position
	clc
	adc #$06
	sta Weapons_RAM
	lda sprite_horizontal	; load sprite X position
	clc
	adc #$0F
	sta Weapons_RAM+3
	rts
:
	lda #%01000001
	sta Weapons_RAM+2
	lda #$21		; update the LEFT graphics
	sta Weapons_RAM+1

	lda sprite_vertical
	clc
	adc #$06
	sta Weapons_RAM

	lda sprite_horizontal	; load sprite X position
	sec
	sbc #$07
	sta Weapons_RAM+3
done:
	RTS

;--------------------------------------------------------------------------

shooting_graphics:
	lda sprite_dir
	cmp #$00
	bne :+
	ldy #$60	;up
	jsr sprites_update
	jsr update_sprite_position
	rts
:
	cmp #$01
	bne :+
	ldy #$78	;down
	jsr sprites_update
	jsr update_sprite_position
	rts
:
	cmp #$02
	bne :+		;right
	ldy #$68
	jsr sprites_update
	jsr update_sprite_position
	rts
:
	ldy #$70	; left
	jsr sprites_update
	jsr update_sprite_position
	rts

