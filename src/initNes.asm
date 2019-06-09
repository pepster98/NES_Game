; ============================
; NMI ISR
; ============================
nmi_vector:
	pha				; Preseve A
	php
	
	lda #$00
	sta PPUCTRL			; Disable NMI
	sta vblank_flag

	lda #$80			; Bit 7, VBlank activity flag
@vbl_done:
	bit PPUSTATUS			; Check if vblank has finished
	bne @vbl_done			; Repeat until vblank is over

	lda #%10011011
	sta PPUCTRL			; Re-enable NMI
	
	plp
	pla				; Restore registers from stack

	rti

; ============================
; IRQ ISR
; ============================
irq_vector:
	rti

; ============================
; Entry vector
; ============================

reset_vector:
; Basic 6502 init, straight outta NESDev
	sei				; ignore IRQs
	cld				; No decimal mode, it isn't supported
	ldx #%00000100
	stx $4017			; Disable APU frame IRQ
	
	ldx #$ff
	txs				; Set up stack

; Clear some PPU registers
	inx				; X = 0 now
	stx PPUCTRL			; Disable NMI
	stx PPUMASK			; Disable rendering
	stx DMCFREQ			; Disable DMC IRQs

; Set an upper bank
	bank_load #$00

; Wait for first vblank
@waitvbl1:
	lda #$80
	bit PPUSTATUS
	bne @waitvbl1

; Wait for the PPU to go stable
	txa				; X still = 0; clear A with this
@clrmem:
	sta $000, x
	sta $100, x
	; Reserving $200 for OAM display list
	sta $300, x
	sta $400, x
	sta $500, x
	sta $600, x

	inx
	bne @clrmem

; One more vblank
@waitvbl2:
	lda #$80
	bit PPUSTATUS
	bne @waitvbl2

; PPU configuration for actual use
	ldx #%10001000		; Nominal PPUCTRL settings:
				; NMI enable
				; Slave mode (don't change this!)
				; 8x8 sprites
				; BG at $0000
				; SPR at $1000
				; VRAM auto-inc 1
				; Nametable at $2000
	stx ppuctrl_config
	stx PPUCTRL

	ldx #%00011110
	stx ppumask_config
	stx PPUMASK

	ppu_enable

	jmp main_entry ; GOTO main loop
