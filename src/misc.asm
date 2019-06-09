misc_init:
; Build button comparison table
	lda #$80
	ldx #$00
@build_controller_table:
	sta button_table, x
	inx
	lsr
	bne @build_controller_table
	
	rts

updateConstants:	; constants for sprite_idx constant
	.byte $00, $10, $20, $30	;4 sprites for each meta sprite

;; random direction logic for NPCs
;; this logic queries the the player's direction to furhter randomize it
random_table:	;values entered at random for direction changes
	.byte $01, $00, $03, $02, $02, $01, $00, $00, $01, $03

; gets a random direction and stores it in randomDirection
random:
	INC randomDirection1	;random  direction counter
	LDA randomDirection1
	CMP #$0A
	BNE :+
	LDA sprite_dir	;playable character's direction
	sta randomDirection1
:
	LDX randomDirection1
	LDA random_table, X
	STA randomDirection	; actual direction in the movement routine
	RTS 

