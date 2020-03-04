	.module	fastmath

	.area	_CODE
_smul16x16::
	ld	iy, #2
	add	iy, sp
	ld 	c, 0 (iy)
	ld 	b, 1 (iy)
	ld 	e, 2 (iy)
	ld 	d, 3 (iy)

mult_de_bc:
	ld	hl, #0

	; optimised 1st iteration
	sla	e
	rl	d
	jr	nc, begin
	ld	h, b
	ld	l, c

begin:	; main loop
	ld	a, #15
loop:
	add	hl, hl
	rl	e
	rl	d
	jr	nc, skip
	add	hl, bc
	jr	nc, skip
	inc	de
skip:
	dec	a
	jr	nz, loop

	; sign correction
	xor	a
	ex	de, hl
	bit	7, 1 (iy)
	jr	z, apos
	ld	b, 3 (iy)
	ld	c, 2 (iy)
	sbc	hl, bc
apos:
	bit	7, 3 (iy)
	jr	z, bpos
	ld	b, 1 (iy)
	ld	c, 0 (iy)
	sbc	hl, bc
bpos:
	ex	de, hl
	ret


; input: hl (16-bit integer unsigned)
; output: a = log2(hl) (rounded down and from -1 to 15) (8-bit integer signed)
_log2::
	pop	bc
	pop	hl
	push	hl
	push	bc

	ld	a, #16
	scf
log2loop:
	adc	hl, hl
	dec	a
	jr	nc, log2loop

	ld l, a
	ret
