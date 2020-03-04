	.module	crt0
	.globl	_main
	.globl	l__INITIALIZER
	.globl	s__INITIALIZED
	.globl	s__INITIALIZER

	.include "rom.inc"
	.area	_CODE
init:
	call	ROM_CLEAR_SCREEN
	;; initialize global variables
	call	gsinit
	;; call main() function
	call	_main
_exit:
	;; clean up
	call	ROM_CLEAR_SCREEN
	call	ROM_INIT_GRAPHICS
	call	ROM_LOAD_CHARS
	call	ROM_LOAD_PAL
	ret

_getkey::
	call	ROM_GET_KEY
	ld	l, a
	ret

	;; Ordering of segments for the linker.
	.area	_HOME
	.area	_CODE
	.area	_INITIALIZER
	.area	_GSINIT
	.area	_GSFINAL

	.area	_DATA
	.area	_INITIALIZED
	.area	_BSEG
	.area	_BSS
	.area	_HEAP

	.area	_GSINIT
gsinit:
	ld	bc, #l__INITIALIZER
	ld	a, b
	or	a, c
	jr	Z, gsinit_next
	ld	de, #s__INITIALIZED
	ld	hl, #s__INITIALIZER
	ldir
gsinit_next:
	.area	_GSFINAL
	ret
