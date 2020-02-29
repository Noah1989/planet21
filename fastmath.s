  .module fastmath
  .globl  _smul16x16

  .area   _CODE
_smul16x16:
  ld iy, #2
  add iy, sp
  ld c, 0 (iy)
  ld b, 1 (iy)
  ld e, 2 (iy)
  ld d, 3 (iy)

  mult_de_bc:
     ld	hl, #0

     sla	e		; optimised 1st iteration
     rl	d
     jr	nc, begin
     ld	h, b
     ld	l, c
  begin:
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

  xor a
  ex de, hl
  bit 7, 1 (iy)
  jr z, apos
  ld b, 3 (iy)
  ld c, 2 (iy)
  sbc hl, bc
apos:
  bit 7, 3 (iy)
  jr z, bpos
  ld b, 1 (iy)
  ld c, 0 (iy)
  sbc hl, bc
bpos:
  ex de, hl

  ret
