;asm funtions that can be called by the c code

.importzp _joypad1, _joypad1old, _joypad1test, _joypad2, _joypad2old, _joypad2test, _shuffle
.importzp _Frame_Count, _temp, _temp2
.export _Get_Input, _Wait_Vblank, _UnRLE, _Sprite_Zero, _Shuffle_Sprites


.segment "ZEROPAGE"
RLE_LOW:	.res 1
RLE_HIGH:	.res 1
RLE_TAG:	.res 1
RLE_BYTE:	.res 1

.segment "CODE"


;due to dmc audio playback occasionally messing with input, it is best to read twice and compare them
_Get_Input:
	lda _joypad1
	sta _joypad1old
	lda _joypad2
	sta _joypad2old
	
	ldx #$01	;strobe controller 1
	stx $4016
	dex
	stx $4016
	
	ldy #$08
Get_Input2:		;get first read, store them as a test
	lda $4016
	and #$03
	cmp #$01
	rol _joypad1test
	lda $4017
	and #$03
	cmp #$01
	rol _joypad2test
	dey
	bne Get_Input2
	
	ldx #$01	;restrobe strobe controller 1
	stx $4016
	dex
	stx $4016
	
	ldy #$08
Get_Input3:		;read again, store them as joypads
	lda $4016
	and #$03
	cmp #$01
	rol _joypad1
	lda $4017
	and #$03
	cmp #$01
	rol _joypad2
	dey
	bne Get_Input3
	
CompareInput:
	lda _joypad1
	cmp _joypad1test
	bne :+
	lda _joypad2
	cmp _joypad2test
	bne :+
	rts				;if same, done
	
:	lda _joypad1
	sta _joypad1test
	lda _joypad2
	sta _joypad2test
	ldy #$08
	jmp Get_Input3	;if different, reread

	
_Wait_Vblank:
	lda $2002
	bpl _Wait_Vblank
	rts
	
	
_UnRLE:
	tay
	stx <RLE_HIGH
	lda #0
	sta <RLE_LOW

	lda (RLE_LOW),y
	sta <RLE_TAG
	iny
	bne @1
	inc <RLE_HIGH
@1:
	lda (RLE_LOW),y
	iny
	bne @11
	inc <RLE_HIGH
@11:
	cmp <RLE_TAG
	beq @2
	sta $2007
	sta <RLE_BYTE
	bne @1
@2:
	lda (RLE_LOW),y
	beq @4
	iny
	bne @21
	inc <RLE_HIGH
@21:
	tax
	lda <RLE_BYTE
@3:
	sta $2007
	dex
	bne @3
	beq @1
@4:
	rts
	
	

_Sprite_Zero:
Sprite0:
	lda $2002
	and #$40
	bne Sprite0
Sprite0b:
	lda $2002
	and #$40
	beq Sprite0b
	rts
	

;from 240-2ff to 340-3ff + offset _shuffle
_Shuffle_Sprites:
	ldx _shuffle
	ldy #0
	lda #$04
	sta _temp
	lda #$30
	sta _temp2
ShuffleLoop:
	lda $0240, y
	sta $0340, x
	inx
	iny
	dec _temp
	bne ShuffleLoop
	lda #$04
	sta _temp
	cpx #$c0
	bne :+
	ldx #$00
:	dec _temp2
	bne ShuffleLoop

	lda _shuffle
	clc
	adc #$10
	cmp #$c0
	bcc :+
	lda #0
:	sta _shuffle
	rts
	
	