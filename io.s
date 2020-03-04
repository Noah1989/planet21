	.module io
	.area _CODE

; ---------------------------------
; void blkin(unsigned char *buf, __sfr *port, unsigned char len)
; ---------------------------------
_blkin::
	pop	de
	pop	hl
	pop	bc
	push	bc
	push	hl
	push	de
	inir
	ret

; ---------------------------------
; void blkout(unsigned char *buf, __sfr *port, unsigned char len)
; ---------------------------------
_blkout::
	pop	de
	pop	hl
	pop	bc
	push	bc
	push	hl
	push	de
	otir
	ret
