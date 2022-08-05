; Listing generated by Microsoft (R) Optimizing Compiler Version 14.00.50727.762 

	TITLE	D:\CLEARSKY\sources\engine\xrEngine\perlin.cpp
	.686P
	.XMM
	include listing.inc
	.model	flat

INCLUDELIB OLDNAMES

EXTRN	__imp__rand:PROC
EXTRN	__imp__srand:PROC
PUBLIC	__real@3b800000
PUBLIC	?init@CPerlinNoise1D@@AAEXXZ			; CPerlinNoise1D::init
;	COMDAT __real@3b800000
; File d:\clearsky\sources\engine\xrengine\perlin.cpp
CONST	SEGMENT
__real@3b800000 DD 03b800000r			; 0.00390625
; Function compile flags: /Ogtpy
CONST	ENDS
;	COMDAT ?init@CPerlinNoise1D@@AAEXXZ
_TEXT	SEGMENT
?init@CPerlinNoise1D@@AAEXXZ PROC			; CPerlinNoise1D::init, COMDAT
; _this$ = ecx

; 30   : {

	push	ebx
	push	esi
	mov	ebx, ecx
	push	edi

; 31   : 	int i, j, k;
; 32   : 
; 33   : 	for (i = 0 ; i < B ; i++)

	xor	esi, esi
	lea	edi, DWORD PTR [ebx+2088]
	npad	3
$LL8@init:

; 34   : 	{
; 35   : 		p[i] = i;

	mov	DWORD PTR [edi-2080], esi

; 36   : 		g1[i] = (float)((rand() % (B + B)) - B) / B;

	call	DWORD PTR __imp__rand
	and	eax, -2147483137			; 800001ffH
	jns	SHORT $LN18@init
	dec	eax
	or	eax, -512				; fffffe00H
	inc	eax
$LN18@init:
	sub	eax, 256				; 00000100H
	cvtsi2ss xmm0, eax
	mulss	xmm0, DWORD PTR __real@3b800000
	movss	DWORD PTR [edi], xmm0
	add	esi, 1
	add	edi, 4
	cmp	esi, 256				; 00000100H
	jl	SHORT $LL8@init

; 37   : 	}
; 38   : 
; 39   : 	while (--i)

	sub	esi, 1
	je	SHORT $LN4@init
	lea	edi, DWORD PTR [ebx+esi*4+8]
	push	ebp
$LL5@init:

; 40   : 	{
; 41   : 		k = p[i];

	mov	ebp, DWORD PTR [edi]

; 42   : 		p[i] = p[j = rand() % B];

	call	DWORD PTR __imp__rand
	and	eax, -2147483393			; 800000ffH
	jns	SHORT $LN19@init
	dec	eax
	or	eax, -256				; ffffff00H
	inc	eax
$LN19@init:
	mov	ecx, DWORD PTR [ebx+eax*4+8]
	mov	DWORD PTR [edi], ecx
	sub	esi, 1
	sub	edi, 4
	test	esi, esi

; 43   : 		p[j] = k;

	mov	DWORD PTR [ebx+eax*4+8], ebp
	jne	SHORT $LL5@init
	pop	ebp
$LN4@init:
	pop	edi
	pop	esi

; 44   : 	}
; 45   : 
; 46   : 	for (i = 0 ; i < B + 2 ; i++)

	lea	eax, DWORD PTR [ebx+1032]
	mov	ecx, 43					; 0000002bH
	pop	ebx
$LL3@init:

; 47   : 	{
; 48   : 		p[B + i] = p[i];

	mov	edx, DWORD PTR [eax-1024]

; 49   : 		g1[B + i] = g1[i];

	fld	DWORD PTR [eax+1056]
	fstp	DWORD PTR [eax+2080]
	mov	DWORD PTR [eax], edx
	mov	edx, DWORD PTR [eax-1020]
	fld	DWORD PTR [eax+1060]
	fstp	DWORD PTR [eax+2084]
	mov	DWORD PTR [eax+4], edx
	mov	edx, DWORD PTR [eax-1016]
	fld	DWORD PTR [eax+1064]
	fstp	DWORD PTR [eax+2088]
	mov	DWORD PTR [eax+8], edx
	mov	edx, DWORD PTR [eax-1012]
	fld	DWORD PTR [eax+1068]
	fstp	DWORD PTR [eax+2092]
	mov	DWORD PTR [eax+12], edx
	mov	edx, DWORD PTR [eax-1008]
	fld	DWORD PTR [eax+1072]
	fstp	DWORD PTR [eax+2096]
	mov	DWORD PTR [eax+16], edx
	mov	edx, DWORD PTR [eax-1004]
	fld	DWORD PTR [eax+1076]
	fstp	DWORD PTR [eax+2100]
	mov	DWORD PTR [eax+20], edx
	add	eax, 24					; 00000018H
	sub	ecx, 1
	jne	$LL3@init

; 50   : 	}
; 51   : }

	ret	0
?init@CPerlinNoise1D@@AAEXXZ ENDP			; CPerlinNoise1D::init
PUBLIC	?normalize@CPerlinNoise3D@@AAEXQAM@Z		; CPerlinNoise3D::normalize
; Function compile flags: /Ogtpy
;	COMDAT ?normalize@CPerlinNoise3D@@AAEXQAM@Z
_TEXT	SEGMENT
tv170 = -12						; size = 4
tv166 = -8						; size = 4
tv162 = -4						; size = 4
_s$ = 8							; size = 4
_v$ = 8							; size = 4
?normalize@CPerlinNoise3D@@AAEXQAM@Z PROC		; CPerlinNoise3D::normalize, COMDAT
; _this$ = ecx

; 293  : {

	sub	esp, 12					; 0000000cH
	push	esi

; 294  : 	float s;
; 295  : 
; 296  : 	s = _sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);

	mov	esi, DWORD PTR _v$[esp+12]
	movss	xmm2, DWORD PTR [esi]
	movss	xmm1, DWORD PTR [esi+4]
	movss	xmm0, DWORD PTR [esi+8]
	movaps	xmm3, xmm2
	mulss	xmm3, xmm2
	movss	DWORD PTR tv170[esp+16], xmm2
	movaps	xmm2, xmm1
	mulss	xmm2, xmm1
	movss	DWORD PTR tv166[esp+16], xmm1
	movaps	xmm1, xmm0
	addss	xmm3, xmm2
	mulss	xmm1, xmm0
	push	ecx

; 297  : 	s = 1.0f/s;

	addss	xmm3, xmm1
	movss	DWORD PTR tv162[esp+20], xmm0
	movss	DWORD PTR [esp], xmm3
	call	_sqrtf
	fld1
	fdivrp	ST(1), ST(0)

; 298  : 
; 299  : 	v[0] = v[0] * s;

	movss	xmm0, DWORD PTR tv170[esp+20]
	add	esp, 4
	fstp	DWORD PTR _s$[esp+12]
	movss	xmm1, DWORD PTR _s$[esp+12]
	mulss	xmm0, xmm1
	movss	DWORD PTR [esi], xmm0

; 300  : 	v[1] = v[1] * s;

	movss	xmm0, DWORD PTR tv166[esp+16]
	mulss	xmm0, xmm1
	movss	DWORD PTR [esi+4], xmm0

; 301  : 	v[2] = v[2] * s;

	movss	xmm0, DWORD PTR tv162[esp+16]
	mulss	xmm0, xmm1
	movss	DWORD PTR [esi+8], xmm0
	pop	esi

; 302  : }

	add	esp, 12					; 0000000cH
	ret	4
?normalize@CPerlinNoise3D@@AAEXQAM@Z ENDP		; CPerlinNoise3D::normalize
_TEXT	ENDS
PUBLIC	?init@CPerlinNoise3D@@AAEXXZ			; CPerlinNoise3D::init
; Function compile flags: /Ogtpy
;	COMDAT ?init@CPerlinNoise3D@@AAEXXZ
_TEXT	SEGMENT
tv616 = -24						; size = 4
tv328 = -20						; size = 4
_s$236285 = -20						; size = 4
_this$ = -16						; size = 4
tv704 = -12						; size = 4
tv699 = -8						; size = 4
tv666 = -4						; size = 4
?init@CPerlinNoise3D@@AAEXXZ PROC			; CPerlinNoise3D::init, COMDAT
; _this$ = ecx

; 213  : {

	sub	esp, 24					; 00000018H
	push	esi
	push	edi

; 214  : 	int i, j, k;
; 215  : 
; 216  : 	for (i = 0 ; i < B ; i++)

	lea	eax, DWORD PTR [ecx+8]
	push	ebx
	mov	DWORD PTR _this$[esp+36], ecx
	xor	edi, edi
	mov	DWORD PTR tv616[esp+36], eax
	lea	esi, DWORD PTR [ecx+2096]
	push	ebp
	npad	6
$LL36@init@2:

; 217  : 	{
; 218  : 		p[i] = i;

	mov	ecx, DWORD PTR tv616[esp+40]
	lea	ebp, DWORD PTR [esi-8]
	mov	DWORD PTR [ecx], edi
	mov	ebx, ebp
	mov	DWORD PTR tv328[esp+40], 3
$LL11@init@2:

; 219  : 		for (j = 0 ; j < 3 ; j++)
; 220  : 			g3[i][j] = (float)((rand() % (B + B)) - B) / B;

	call	DWORD PTR __imp__rand
	and	eax, -2147483137			; 800001ffH
	jns	SHORT $LN38@init@2
	dec	eax
	or	eax, -512				; fffffe00H
	inc	eax
$LN38@init@2:
	sub	eax, 256				; 00000100H
	cvtsi2ss xmm0, eax
	mulss	xmm0, DWORD PTR __real@3b800000
	movss	DWORD PTR [ebx], xmm0
	add	ebx, 4
	sub	DWORD PTR tv328[esp+40], 1
	jne	SHORT $LL11@init@2

; 221  : 		normalize(g3[i]);

	movss	xmm0, DWORD PTR [esi]
	movss	xmm2, DWORD PTR [ebp]
	movss	xmm1, DWORD PTR [esi-4]
	movaps	xmm3, xmm0
	mulss	xmm3, xmm0
	movss	DWORD PTR tv666[esp+40], xmm0
	movaps	xmm0, xmm2
	mulss	xmm0, xmm2
	addss	xmm3, xmm0
	movaps	xmm0, xmm1
	mulss	xmm0, xmm1
	push	ecx
	addss	xmm3, xmm0
	movss	DWORD PTR tv699[esp+44], xmm1
	movss	DWORD PTR tv704[esp+44], xmm2
	movss	DWORD PTR [esp], xmm3
	call	_sqrtf
	fld1
	fdivrp	ST(1), ST(0)
	movss	xmm0, DWORD PTR tv704[esp+44]
	add	DWORD PTR tv616[esp+44], 4
	add	edi, 1
	add	esp, 4
	add	esi, 12					; 0000000cH
	cmp	edi, 256				; 00000100H
	fstp	DWORD PTR _s$236285[esp+40]
	movss	xmm1, DWORD PTR _s$236285[esp+40]
	mulss	xmm0, xmm1
	movss	DWORD PTR [ebp], xmm0
	movss	xmm0, DWORD PTR tv699[esp+40]
	mulss	xmm0, xmm1
	movss	DWORD PTR [esi-16], xmm0
	movss	xmm0, DWORD PTR tv666[esp+40]
	mulss	xmm0, xmm1
	movss	DWORD PTR [esi-12], xmm0
	jl	$LL36@init@2

; 222  : 	}
; 223  : 
; 224  : 	while (--i)

	sub	edi, 1
	mov	ebp, DWORD PTR _this$[esp+40]
	je	SHORT $LN7@init@2
	lea	esi, DWORD PTR [ebp+edi*4+8]
$LL8@init@2:

; 225  : 	{
; 226  : 		k = p[i];

	mov	ebx, DWORD PTR [esi]

; 227  : 		p[i] = p[j = rand() % B];

	call	DWORD PTR __imp__rand
	and	eax, -2147483393			; 800000ffH
	jns	SHORT $LN39@init@2
	dec	eax
	or	eax, -256				; ffffff00H
	inc	eax
$LN39@init@2:
	mov	edx, DWORD PTR [ebp+eax*4+8]
	mov	DWORD PTR [esi], edx
	sub	edi, 1
	sub	esi, 4
	test	edi, edi

; 228  : 		p[j] = k;

	mov	DWORD PTR [ebp+eax*4+8], ebx
	jne	SHORT $LL8@init@2
$LN7@init@2:

; 229  : 	}
; 230  : 
; 231  : 	for (i = 0 ; i < B + 2 ; i++)

	lea	ecx, DWORD PTR [ebp+1032]
	lea	eax, DWORD PTR [ebp+5160]
	pop	ebp
	mov	edx, 258				; 00000102H
	pop	ebx
	npad	3
$LL6@init@2:

; 232  : 	{
; 233  : 		p[B + i] = p[i];

	mov	esi, DWORD PTR [ecx-1024]
	mov	DWORD PTR [ecx], esi

; 234  : 		for (j = 0 ; j < 3 ; j++)
; 235  : 			g3[B + i][j] = g3[i][j];

	fld	DWORD PTR [eax-3072]
	fstp	DWORD PTR [eax]
	add	ecx, 4
	fld	DWORD PTR [eax-3068]
	add	eax, 12					; 0000000cH
	sub	edx, 1
	fstp	DWORD PTR [eax-8]
	fld	DWORD PTR [eax-3076]
	fstp	DWORD PTR [eax-4]
	jne	SHORT $LL6@init@2
	pop	edi
	pop	esi

; 236  : 	}
; 237  : }

	add	esp, 24					; 00000018H
	ret	0
?init@CPerlinNoise3D@@AAEXXZ ENDP			; CPerlinNoise3D::init
_TEXT	ENDS
PUBLIC	?normalize@CPerlinNoise2D@@AAEXQAM@Z		; CPerlinNoise2D::normalize
; Function compile flags: /Ogtpy
;	COMDAT ?normalize@CPerlinNoise2D@@AAEXQAM@Z
_TEXT	SEGMENT
tv133 = -8						; size = 4
tv130 = -4						; size = 4
_s$ = 8							; size = 4
_v$ = 8							; size = 4
?normalize@CPerlinNoise2D@@AAEXQAM@Z PROC		; CPerlinNoise2D::normalize, COMDAT
; _this$ = ecx

; 184  : {

	sub	esp, 12					; 0000000cH
	push	esi

; 185  : 	float s;
; 186  : 
; 187  : 	s = _sqrt(v[0] * v[0] + v[1] * v[1]);

	mov	esi, DWORD PTR _v$[esp+12]
	movss	xmm1, DWORD PTR [esi]
	movss	xmm0, DWORD PTR [esi+4]
	movaps	xmm2, xmm1
	mulss	xmm2, xmm1
	movss	DWORD PTR tv133[esp+16], xmm1
	movaps	xmm1, xmm0
	mulss	xmm1, xmm0
	push	ecx
	addss	xmm2, xmm1
	movss	DWORD PTR tv130[esp+20], xmm0
	movss	DWORD PTR [esp], xmm2
	call	_sqrtf

; 188  : 	s = 1.0f/s;

	fld1
	fdivrp	ST(1), ST(0)

; 189  : 	v[0] = v[0] * s;

	movss	xmm0, DWORD PTR tv133[esp+20]
	add	esp, 4
	fstp	DWORD PTR _s$[esp+12]
	movss	xmm1, DWORD PTR _s$[esp+12]
	mulss	xmm0, xmm1
	movss	DWORD PTR [esi], xmm0

; 190  : 	v[1] = v[1] * s;

	movss	xmm0, DWORD PTR tv130[esp+16]
	mulss	xmm0, xmm1
	movss	DWORD PTR [esi+4], xmm0

; 191  : }

	pop	esi
	add	esp, 12					; 0000000cH
	ret	4
?normalize@CPerlinNoise2D@@AAEXQAM@Z ENDP		; CPerlinNoise2D::normalize
_TEXT	ENDS
PUBLIC	?init@CPerlinNoise2D@@AAEXXZ			; CPerlinNoise2D::init
; Function compile flags: /Ogtpy
;	COMDAT ?init@CPerlinNoise2D@@AAEXXZ
_TEXT	SEGMENT
tv486 = -20						; size = 4
_this$ = -16						; size = 4
_s$236323 = -12						; size = 4
tv500 = -8						; size = 4
tv495 = -4						; size = 4
?init@CPerlinNoise2D@@AAEXXZ PROC			; CPerlinNoise2D::init, COMDAT
; _this$ = ecx

; 114  : {

	sub	esp, 20					; 00000014H
	push	ebx
	push	ebp
	push	esi
	push	edi
	mov	DWORD PTR _this$[esp+36], ecx

; 115  : 	int i, j, k;
; 116  : 
; 117  : 	for (i = 0 ; i < B ; i++)

	xor	ebx, ebx
	lea	edi, DWORD PTR [ecx+2088]
	add	ecx, 8
	mov	DWORD PTR tv486[esp+36], ecx
$LN14@init@3:

; 118  : 	{
; 119  : 		p[i] = i;

	mov	eax, DWORD PTR tv486[esp+36]
	mov	DWORD PTR [eax], ebx
	mov	esi, edi
	mov	ebp, 2
$LL11@init@3:

; 120  : 		for (j = 0 ; j < 2 ; j++)
; 121  : 			g2[i][j] = (float)((rand() % (B + B)) - B) / B;

	call	DWORD PTR __imp__rand
	and	eax, -2147483137			; 800001ffH
	jns	SHORT $LN37@init@3
	dec	eax
	or	eax, -512				; fffffe00H
	inc	eax
$LN37@init@3:
	sub	eax, 256				; 00000100H
	cvtsi2ss xmm0, eax
	mulss	xmm0, DWORD PTR __real@3b800000
	movss	DWORD PTR [esi], xmm0
	add	esi, 4
	sub	ebp, 1
	jne	SHORT $LL11@init@3

; 122  : 		normalize(g2[i]);

	movss	xmm1, DWORD PTR [edi]
	movss	xmm0, DWORD PTR [edi+4]
	movaps	xmm2, xmm1
	mulss	xmm2, xmm1
	movss	DWORD PTR tv500[esp+36], xmm1
	movaps	xmm1, xmm0
	mulss	xmm1, xmm0
	push	ecx
	addss	xmm2, xmm1
	movss	DWORD PTR tv495[esp+40], xmm0
	movss	DWORD PTR [esp], xmm2
	call	_sqrtf
	fld1
	fdivrp	ST(1), ST(0)
	movss	xmm0, DWORD PTR tv500[esp+40]
	add	DWORD PTR tv486[esp+40], 4
	add	ebx, 1
	add	esp, 4
	cmp	ebx, 256				; 00000100H
	fstp	DWORD PTR _s$236323[esp+36]
	movss	xmm1, DWORD PTR _s$236323[esp+36]
	mulss	xmm0, xmm1
	movss	DWORD PTR [edi], xmm0
	movss	xmm0, DWORD PTR tv495[esp+36]
	mulss	xmm0, xmm1
	movss	DWORD PTR [edi+4], xmm0
	mov	edi, esi
	jl	$LN14@init@3

; 123  : 	}
; 124  : 
; 125  : 	while (--i)

	sub	ebx, 1
	mov	ebp, DWORD PTR _this$[esp+36]
	je	SHORT $LN7@init@3
	lea	esi, DWORD PTR [ebp+ebx*4+8]
	npad	5
$LL8@init@3:

; 126  : 	{
; 127  : 		k = p[i];

	mov	edi, DWORD PTR [esi]

; 128  : 		p[i] = p[j = rand() % B];

	call	DWORD PTR __imp__rand
	and	eax, -2147483393			; 800000ffH
	jns	SHORT $LN38@init@3
	dec	eax
	or	eax, -256				; ffffff00H
	inc	eax
$LN38@init@3:
	mov	ecx, DWORD PTR [ebp+eax*4+8]
	mov	DWORD PTR [esi], ecx
	sub	ebx, 1
	sub	esi, 4
	test	ebx, ebx

; 129  : 		p[j] = k;

	mov	DWORD PTR [ebp+eax*4+8], edi
	jne	SHORT $LL8@init@3
$LN7@init@3:

; 130  : 	}
; 131  : 
; 132  : 	for (i = 0 ; i < B + 2 ; i++)

	lea	ecx, DWORD PTR [ebp+1032]
	lea	eax, DWORD PTR [ebp+4136]
	mov	edx, 258				; 00000102H
	npad	5
$LL6@init@3:

; 133  : 	{
; 134  : 		p[B + i] = p[i];

	mov	esi, DWORD PTR [ecx-1024]
	mov	DWORD PTR [ecx], esi

; 135  : 		for (j = 0 ; j < 2 ; j++)
; 136  : 			g2[B + i][j] = g2[i][j];

	fld	DWORD PTR [eax-2048]
	fstp	DWORD PTR [eax]
	add	ecx, 4
	fld	DWORD PTR [eax-2044]
	add	eax, 8
	sub	edx, 1
	fstp	DWORD PTR [eax-4]
	jne	SHORT $LL6@init@3

; 137  : 	}
; 138  : }

	pop	edi
	pop	esi
	pop	ebp
	pop	ebx
	add	esp, 20					; 00000014H
	ret	0
?init@CPerlinNoise2D@@AAEXXZ ENDP			; CPerlinNoise2D::init
_TEXT	ENDS
PUBLIC	__real@45800000
PUBLIC	?noise@CPerlinNoise1D@@AAEMM@Z			; CPerlinNoise1D::noise
;	COMDAT __real@45800000
CONST	SEGMENT
__real@45800000 DD 045800000r			; 4096
; Function compile flags: /Ogtpy
CONST	ENDS
;	COMDAT ?noise@CPerlinNoise1D@@AAEMM@Z
_TEXT	SEGMENT
_u$ = -4						; size = 4
_rx0$ = 8						; size = 4
_arg$ = 8						; size = 4
?noise@CPerlinNoise1D@@AAEMM@Z PROC			; CPerlinNoise1D::noise, COMDAT
; _this$ = ecx

; 54   : {

	push	ecx
	push	esi
	mov	esi, ecx

; 55   : 	int bx0, bx1;
; 56   : 	float rx0, rx1, sx, t, u, v, vec[1];
; 57   : 
; 58   : 	vec[0] = arg;
; 59   : 
; 60   : 	if (!mReady){ 

	cmp	BYTE PTR [esi+4], 0
	jne	SHORT $LN1@noise

; 61   : 		srand(mSeed);

	mov	eax, DWORD PTR [esi]
	push	eax
	call	DWORD PTR __imp__srand
	add	esp, 4

; 62   : 		mReady = true;
; 63   : 		init();

	mov	ecx, esi
	mov	BYTE PTR [esi+4], 1
	call	?init@CPerlinNoise1D@@AAEXXZ		; CPerlinNoise1D::init
$LN1@noise:

; 64   : 	}
; 65   : 
; 66   : 	setup(0, bx0,bx1, rx0,rx1);

	movss	xmm0, DWORD PTR _arg$[esp+4]
	addss	xmm0, DWORD PTR __real@45800000
	cvttss2si ecx, xmm0
	cvtsi2ss xmm1, ecx
	mov	eax, ecx
	subss	xmm0, xmm1
	and	eax, 255				; 000000ffH

; 67   : 
; 68   : 	sx = s_curve(rx0);
; 69   : 
; 70   : 	u = rx0 * g1[ p[ bx0 ] ];

	mov	ecx, DWORD PTR [esi+eax*4+8]
	movss	xmm1, DWORD PTR [esi+ecx*4+2088]
	movss	DWORD PTR _rx0$[esp+4], xmm0
	fld	DWORD PTR _rx0$[esp+4]
	add	eax, 1
	fld	DWORD PTR __real@3f800000
	and	eax, 255				; 000000ffH

; 71   : 	v = rx1 * g1[ p[ bx1 ] ];

	mov	edx, DWORD PTR [esi+eax*4+8]
	fsubr	ST(0), ST(1)
	mulss	xmm1, xmm0
	movss	DWORD PTR _u$[esp+8], xmm1
	fmul	DWORD PTR [esi+edx*4+2088]
	pop	esi

; 72   : 
; 73   : 	return lerp(sx, u, v);

	fld	DWORD PTR _u$[esp+4]
	fsub	ST(1), ST(0)
	fld	ST(2)
	fadd	ST(0), ST(0)
	fsubr	DWORD PTR _drop_angle
	fmul	ST(0), ST(3)
	fmulp	ST(3), ST(0)
	fxch	ST(1)
	fmulp	ST(2), ST(0)
	faddp	ST(1), ST(0)

; 74   : }

	pop	ecx
	ret	4
?noise@CPerlinNoise1D@@AAEMM@Z ENDP			; CPerlinNoise1D::noise
PUBLIC	?noise@CPerlinNoise3D@@AAEMABU?$_vector3@M@@@Z	; CPerlinNoise3D::noise
; Function compile flags: /Ogtpy
;	COMDAT ?noise@CPerlinNoise3D@@AAEMABU?$_vector3@M@@@Z
_TEXT	SEGMENT
_rz1$ = -32						; size = 4
_u$ = -28						; size = 4
_j$ = -28						; size = 4
_a$ = -24						; size = 4
_b00$ = -24						; size = 4
_sy$ = -20						; size = 4
_ry1$ = -16						; size = 4
_rx1$ = -12						; size = 4
_t$ = -8						; size = 4
_c$ = -4						; size = 4
_rz0$ = 8						; size = 4
_vec$ = 8						; size = 4
?noise@CPerlinNoise3D@@AAEMABU?$_vector3@M@@@Z PROC	; CPerlinNoise3D::noise, COMDAT
; _this$ = ecx

; 240  : {

	sub	esp, 32					; 00000020H
	push	ebx
	push	ebp
	push	esi
	mov	esi, ecx

; 241  : 	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
; 242  : 	float rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
; 243  : 	int i, j;
; 244  : 
; 245  : 	if (!mReady){ 

	cmp	BYTE PTR [esi+4], 0
	push	edi
	jne	SHORT $LN1@noise@2

; 246  : 		srand(mSeed);

	mov	eax, DWORD PTR [esi]
	push	eax
	call	DWORD PTR __imp__srand
	add	esp, 4

; 247  : 		mReady = true;
; 248  : 		init();

	mov	ecx, esi
	mov	BYTE PTR [esi+4], 1
	call	?init@CPerlinNoise3D@@AAEXXZ		; CPerlinNoise3D::init
$LN1@noise@2:

; 249  : 	}
; 250  : 
; 251  : 	setup(0, bx0,bx1, rx0,rx1);

	mov	eax, DWORD PTR _vec$[esp+44]
	movss	xmm4, DWORD PTR __real@45800000
	movss	xmm0, DWORD PTR [eax]

; 252  : 	setup(1, by0,by1, ry0,ry1);

	movss	xmm2, DWORD PTR [eax+4]
	movss	xmm5, DWORD PTR __real@3f800000
	addss	xmm0, xmm4
	addss	xmm2, xmm4
	cvttss2si ecx, xmm0
	cvtsi2ss xmm1, ecx
	subss	xmm0, xmm1
	mov	edi, ecx
	and	edi, 255				; 000000ffH
	cvttss2si ecx, xmm2
	cvtsi2ss xmm1, ecx
	subss	xmm2, xmm1

; 253  : 	setup(2, bz0,bz1, rz0,rz1);

	movss	xmm1, DWORD PTR [eax+8]
	addss	xmm1, xmm4
	cvttss2si ebp, xmm1
	add	edi, 1
	cvtsi2ss xmm4, ebp
	mov	eax, ebp

; 254  : 
; 255  : 	i = p[ bx0 ];

	mov	ebp, DWORD PTR [esi+edi*4+4]
	and	edi, 255				; 000000ffH

; 256  : 	j = p[ bx1 ];

	mov	edi, DWORD PTR [esi+edi*4+8]
	mov	edx, ecx
	and	edx, 255				; 000000ffH
	mov	DWORD PTR _j$[esp+48], edi

; 257  : 
; 258  : 	b00 = p[ i + by0 ];

	lea	edi, DWORD PTR [edx+ebp]
	mov	edi, DWORD PTR [esi+edi*4+8]
	subss	xmm1, xmm4
	lea	ebx, DWORD PTR [edx+1]
	movss	DWORD PTR _rz0$[esp+44], xmm1
	subss	xmm1, xmm5
	mov	DWORD PTR _b00$[esp+48], edi

; 259  : 	b10 = p[ j + by0 ];

	mov	edi, DWORD PTR _j$[esp+48]
	add	edi, edx

; 260  : 	b01 = p[ i + by1 ];
; 261  : 	b11 = p[ j + by1 ];

	mov	edx, DWORD PTR _j$[esp+48]
	mov	edi, DWORD PTR [esi+edi*4+8]
	and	eax, 255				; 000000ffH
	movss	DWORD PTR _rz1$[esp+48], xmm1

; 262  : 
; 263  : 	t  = s_curve(rx0);

	movss	xmm1, DWORD PTR _drop_angle
	and	ebx, 255				; 000000ffH
	movaps	xmm4, xmm1
	lea	ecx, DWORD PTR [eax+1]
	add	ebp, ebx
	mov	ebp, DWORD PTR [esi+ebp*4+8]
	and	ecx, 255				; 000000ffH
	add	edx, ebx
	mov	ebx, DWORD PTR [esi+edx*4+8]

; 264  : 	sy = s_curve(ry0);
; 265  : 	sz = s_curve(rz0);
; 266  : 
; 267  : #define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )
; 268  : 
; 269  : 	q = g3[ b00 + bz0 ] ; u = at3(rx0,ry0,rz0);

	mov	edx, DWORD PTR _b00$[esp+48]
	movaps	xmm3, xmm0
	subss	xmm3, xmm5
	movaps	xmm6, xmm2
	subss	xmm6, xmm5
	movss	xmm5, DWORD PTR __real@40000000
	movaps	xmm7, xmm0
	mulss	xmm7, xmm5
	subss	xmm4, xmm7
	lea	edx, DWORD PTR [edx+eax+174]
	movaps	xmm7, xmm2
	mulss	xmm7, xmm5
	subss	xmm1, xmm7
	movss	xmm7, DWORD PTR _rz0$[esp+44]
	mulss	xmm1, xmm2
	mulss	xmm1, xmm2
	movss	DWORD PTR _sy$[esp+48], xmm1
	lea	edx, DWORD PTR [edx+edx*2]
	movss	xmm5, DWORD PTR [esi+edx*4+8]
	movss	xmm1, DWORD PTR [esi+edx*4+4]
	lea	edx, DWORD PTR [esi+edx*4]
	mulss	xmm1, xmm2
	mulss	xmm5, xmm7
	addss	xmm5, xmm1
	mulss	xmm4, xmm0
	movaps	xmm1, xmm0
	mulss	xmm1, DWORD PTR [edx]
	mulss	xmm4, xmm0
	movss	DWORD PTR _rx1$[esp+48], xmm3
	movss	DWORD PTR _ry1$[esp+48], xmm6
	movss	DWORD PTR _t$[esp+48], xmm4
	addss	xmm5, xmm1

; 270  : 	q = g3[ b10 + bz0 ] ; v = at3(rx1,ry0,rz0);

	lea	edx, DWORD PTR [edi+eax+174]
	lea	edx, DWORD PTR [edx+edx*2]
	movss	xmm1, DWORD PTR [esi+edx*4+8]
	mulss	xmm1, xmm7
	movss	xmm7, DWORD PTR [esi+edx*4+4]
	mulss	xmm7, xmm2
	addss	xmm1, xmm7
	lea	edx, DWORD PTR [esi+edx*4]
	movaps	xmm7, xmm3
	mulss	xmm7, DWORD PTR [edx]

; 271  : 	a = lerp(t, u, v);

	addss	xmm1, xmm7
	subss	xmm1, xmm5

; 272  : 
; 273  : 	q = g3[ b01 + bz0 ] ; u = at3(rx0,ry1,rz0);

	lea	edx, DWORD PTR [eax+ebp+174]
	mulss	xmm1, xmm4
	addss	xmm1, xmm5

; 274  : 	q = g3[ b11 + bz0 ] ; v = at3(rx1,ry1,rz0);

	lea	eax, DWORD PTR [ebx+eax+174]
	lea	edx, DWORD PTR [edx+edx*2]
	movss	xmm5, DWORD PTR [esi+edx*4+8]
	mulss	xmm5, DWORD PTR _rz0$[esp+44]
	movss	xmm7, DWORD PTR [esi+edx*4+4]
	mulss	xmm7, xmm6
	addss	xmm5, xmm7
	lea	edx, DWORD PTR [esi+edx*4]
	lea	eax, DWORD PTR [eax+eax*2]
	lea	eax, DWORD PTR [esi+eax*4]
	movaps	xmm7, xmm0
	mulss	xmm7, DWORD PTR [edx]
	addss	xmm5, xmm7
	movss	xmm7, DWORD PTR [eax+8]
	mulss	xmm7, DWORD PTR _rz0$[esp+44]

; 275  : 	b = lerp(t, u, v);
; 276  : 
; 277  : 	c = lerp(sy, a, b);
; 278  : 
; 279  : 	q = g3[ b00 + bz1 ] ; u = at3(rx0,ry0,rz1);

	mov	edx, DWORD PTR _b00$[esp+48]
	movss	DWORD PTR _u$[esp+48], xmm5
	movss	xmm5, DWORD PTR [eax+4]
	mulss	xmm5, xmm6
	addss	xmm7, xmm5
	movaps	xmm5, xmm3
	mulss	xmm5, DWORD PTR [eax]
	addss	xmm7, xmm5
	movss	xmm5, DWORD PTR _u$[esp+48]
	subss	xmm7, xmm5
	mulss	xmm7, xmm4
	addss	xmm7, xmm5
	movss	xmm5, DWORD PTR _rz1$[esp+48]
	subss	xmm7, xmm1
	mulss	xmm7, DWORD PTR _sy$[esp+48]
	addss	xmm7, xmm1
	movss	DWORD PTR _c$[esp+48], xmm7
	lea	eax, DWORD PTR [edx+ecx+174]
	lea	eax, DWORD PTR [eax+eax*2]
	movss	xmm1, DWORD PTR [esi+eax*4+8]
	movss	xmm7, DWORD PTR [esi+eax*4+4]
	mulss	xmm7, xmm2
	lea	eax, DWORD PTR [esi+eax*4]
	mulss	xmm1, xmm5
	addss	xmm1, xmm7
	movaps	xmm7, xmm0
	mulss	xmm7, DWORD PTR [eax]
	addss	xmm1, xmm7

; 280  : 	q = g3[ b10 + bz1 ] ; v = at3(rx1,ry0,rz1);

	lea	eax, DWORD PTR [edi+ecx+174]
	lea	edx, DWORD PTR [eax+eax*2]
	movss	xmm7, DWORD PTR [esi+edx*4+8]
	mulss	xmm3, DWORD PTR [esi+edx*4]
	lea	eax, DWORD PTR [esi+edx*4]
	mulss	xmm7, xmm5
	movss	xmm5, DWORD PTR [eax+4]
	mulss	xmm5, xmm2
	addss	xmm7, xmm5

; 281  : 	a = lerp(t, u, v);

	addss	xmm7, xmm3
	subss	xmm7, xmm1

; 282  : 
; 283  : 	q = g3[ b01 + bz1 ] ; u = at3(rx0,ry1,rz1);

	lea	eax, DWORD PTR [ecx+ebp+174]
	lea	eax, DWORD PTR [eax+eax*2]
	movss	xmm2, DWORD PTR [esi+eax*4+4]
	mulss	xmm0, DWORD PTR [esi+eax*4]
	lea	eax, DWORD PTR [esi+eax*4]
	mulss	xmm7, xmm4
	addss	xmm7, xmm1
	movss	xmm1, DWORD PTR [eax+8]
	mulss	xmm1, DWORD PTR _rz1$[esp+48]
	mulss	xmm2, xmm6
	movss	DWORD PTR _a$[esp+48], xmm7
	addss	xmm1, xmm2

; 284  : 	q = g3[ b11 + bz1 ] ; v = at3(rx1,ry1,rz1);

	lea	ecx, DWORD PTR [ebx+ecx+174]
	lea	ecx, DWORD PTR [ecx+ecx*2]
	fld	DWORD PTR [esi+ecx*4+8]
	lea	esi, DWORD PTR [esi+ecx*4]
	fmul	DWORD PTR _rz1$[esp+48]
	addss	xmm1, xmm0
	fld	DWORD PTR [esi+4]
	movss	DWORD PTR _u$[esp+48], xmm1
	fmul	DWORD PTR _ry1$[esp+48]
	pop	edi
	faddp	ST(1), ST(0)
	fld	DWORD PTR _rx1$[esp+44]
	fmul	DWORD PTR [esi]
	pop	esi
	pop	ebp
	pop	ebx

; 285  : 	b = lerp(t, u, v);
; 286  : 
; 287  : 	d = lerp(sy, a, b);
; 288  : 
; 289  : 	return lerp(sz, c, d);

	faddp	ST(1), ST(0)
	fld	DWORD PTR _u$[esp+32]
	fsub	ST(1), ST(0)
	fxch	ST(1)
	fmul	DWORD PTR _t$[esp+32]
	faddp	ST(1), ST(0)
	fld	DWORD PTR _a$[esp+32]
	fsub	ST(1), ST(0)
	fxch	ST(1)
	fmul	DWORD PTR _sy$[esp+32]
	faddp	ST(1), ST(0)
	fld	DWORD PTR _c$[esp+32]
	fsub	ST(1), ST(0)
	fld	DWORD PTR _rz0$[esp+28]
	fld	ST(0)
	fadd	ST(0), ST(1)
	fsubr	DWORD PTR _drop_angle
	fmul	ST(0), ST(1)
	fmulp	ST(1), ST(0)
	fmulp	ST(2), ST(0)
	faddp	ST(1), ST(0)

; 290  : }

	add	esp, 32					; 00000020H
	ret	4
?noise@CPerlinNoise3D@@AAEMABU?$_vector3@M@@@Z ENDP	; CPerlinNoise3D::noise
_TEXT	ENDS
PUBLIC	?noise@CPerlinNoise2D@@AAEMABU?$_vector2@M@@@Z	; CPerlinNoise2D::noise
; Function compile flags: /Ogtpy
;	COMDAT ?noise@CPerlinNoise2D@@AAEMABU?$_vector2@M@@@Z
_TEXT	SEGMENT
_rx1$ = -20						; size = 4
_u$ = -16						; size = 4
_sx$ = -12						; size = 4
_a$ = -8						; size = 4
_ry0$ = -4						; size = 4
_ry1$ = 8						; size = 4
_vec$ = 8						; size = 4
?noise@CPerlinNoise2D@@AAEMABU?$_vector2@M@@@Z PROC	; CPerlinNoise2D::noise, COMDAT
; _this$ = ecx

; 141  : {

	sub	esp, 20					; 00000014H
	push	ebx
	push	esi
	mov	esi, ecx

; 142  : 	int bx0, bx1, by0, by1, b00, b10, b01, b11;
; 143  : 	float rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
; 144  : 	int i, j;
; 145  : 
; 146  : 	if (!mReady){ 

	cmp	BYTE PTR [esi+4], 0
	push	edi
	jne	SHORT $LN1@noise@3

; 147  : 		srand(mSeed);

	mov	eax, DWORD PTR [esi]
	push	eax
	call	DWORD PTR __imp__srand
	add	esp, 4

; 148  : 		mReady = true;
; 149  : 		init();

	mov	ecx, esi
	mov	BYTE PTR [esi+4], 1
	call	?init@CPerlinNoise2D@@AAEXXZ		; CPerlinNoise2D::init
$LN1@noise@3:

; 150  : 	}
; 151  : 
; 152  : 	setup(0,bx0,bx1,rx0,rx1);

	mov	edx, DWORD PTR _vec$[esp+28]
	movss	xmm0, DWORD PTR [edx]
	movss	xmm2, DWORD PTR __real@45800000
	movss	xmm3, DWORD PTR __real@3f800000
	addss	xmm0, xmm2
	cvttss2si eax, xmm0
	cvtsi2ss xmm1, eax
	subss	xmm0, xmm1

; 153  : 	setup(1,by0,by1,ry0,ry1);

	movss	xmm1, DWORD PTR [edx+4]
	addss	xmm1, xmm2
	mov	ecx, eax
	cvttss2si edi, xmm1
	and	ecx, 255				; 000000ffH
	cvtsi2ss xmm2, edi
	subss	xmm1, xmm2

; 154  : 
; 155  : 	i = p[bx0];
; 156  : 	j = p[bx1];
; 157  : 
; 158  : 	b00 = p[i + by0];
; 159  : 	b10 = p[j + by0];
; 160  : 	b01 = p[i + by1];
; 161  : 	b11 = p[j + by1];
; 162  : 
; 163  : 	sx = s_curve(rx0);

	movss	xmm2, DWORD PTR _drop_angle
	mov	eax, edi
	mov	edi, DWORD PTR [esi+ecx*4+8]
	and	eax, 255				; 000000ffH

; 164  : 	sy = s_curve(ry0);
; 165  : 
; 166  : #define at2(rx,ry) ( rx * q[0] + ry * q[1] )
; 167  : 
; 168  : 	q = g2[b00];

	lea	ebx, DWORD PTR [edi+eax]
	mov	ebx, DWORD PTR [esi+ebx*4+8]

; 169  : 	u = at2(rx0,ry0);

	movss	xmm6, DWORD PTR [esi+ebx*8+2088]
	add	ecx, 1
	lea	edx, DWORD PTR [eax+1]
	movaps	xmm5, xmm1
	subss	xmm5, xmm3
	and	ecx, 255				; 000000ffH
	mov	ecx, DWORD PTR [esi+ecx*4+8]

; 170  : 	q = g2[b10];

	add	eax, ecx
	mov	eax, DWORD PTR [esi+eax*4+8]
	movaps	xmm4, xmm0
	subss	xmm4, xmm3
	mulss	xmm6, xmm0
	movaps	xmm3, xmm0
	mulss	xmm3, DWORD PTR __real@40000000
	subss	xmm2, xmm3
	movss	xmm3, DWORD PTR [esi+ebx*8+2092]
	mulss	xmm3, xmm1
	lea	ebx, DWORD PTR [esi+ebx*8+2088]
	addss	xmm3, xmm6

; 171  : 	v = at2(rx1,ry0);

	movss	xmm6, DWORD PTR [esi+eax*8+2092]
	mulss	xmm6, xmm1
	lea	eax, DWORD PTR [esi+eax*8+2088]
	movss	DWORD PTR _ry0$[esp+32], xmm1
	movss	xmm1, DWORD PTR [eax]
	and	edx, 255				; 000000ffH
	mulss	xmm1, xmm4
	addss	xmm6, xmm1

; 172  : 	a = lerp(sx, u, v);
; 173  : 
; 174  : 	q = g2[b01];

	add	edi, edx
	mov	eax, DWORD PTR [esi+edi*4+8]

; 175  : 	u = at2(rx0,ry1);

	movss	xmm1, DWORD PTR [esi+eax*8+2092]
	mulss	xmm2, xmm0
	mulss	xmm2, xmm0
	lea	eax, DWORD PTR [esi+eax*8+2088]
	movss	DWORD PTR _sx$[esp+32], xmm2
	subss	xmm6, xmm3
	mulss	xmm6, xmm2
	movss	xmm2, DWORD PTR [eax]

; 176  : 	q = g2[b11];

	add	ecx, edx
	mov	ecx, DWORD PTR [esi+ecx*4+8]

; 177  : 	v = at2(rx1,ry1);

	fld	DWORD PTR [esi+ecx*8+2092]
	lea	eax, DWORD PTR [esi+ecx*8+2088]
	movss	DWORD PTR _ry1$[esp+28], xmm5
	fmul	DWORD PTR _ry1$[esp+28]
	movss	DWORD PTR _rx1$[esp+32], xmm4
	fld	DWORD PTR [eax]
	mulss	xmm1, xmm5
	fmul	DWORD PTR _rx1$[esp+32]
	mulss	xmm2, xmm0
	addss	xmm1, xmm2
	movss	DWORD PTR _u$[esp+32], xmm1
	faddp	ST(1), ST(0)
	addss	xmm6, xmm3

; 178  : 	b = lerp(sx, u, v);

	fld	DWORD PTR _u$[esp+32]
	movss	DWORD PTR _a$[esp+32], xmm6
	fsub	ST(1), ST(0)
	fxch	ST(1)
	fmul	DWORD PTR _sx$[esp+32]
	faddp	ST(1), ST(0)

; 179  : 
; 180  : 	return lerp(sy, a, b);

	fld	DWORD PTR _a$[esp+32]
	fsub	ST(1), ST(0)
	fld	DWORD PTR _ry0$[esp+32]

; 181  : }

	pop	edi
	fld	ST(0)
	pop	esi
	fadd	ST(0), ST(1)
	pop	ebx
	fsubr	DWORD PTR _drop_angle
	fmul	ST(0), ST(1)
	fmulp	ST(1), ST(0)
	fmulp	ST(2), ST(0)
	faddp	ST(1), ST(0)
	add	esp, 20					; 00000014H
	ret	4
?noise@CPerlinNoise2D@@AAEMABU?$_vector2@M@@@Z ENDP	; CPerlinNoise2D::noise
_TEXT	ENDS
PUBLIC	?GetContinious@CPerlinNoise1D@@QAEMM@Z		; CPerlinNoise1D::GetContinious
; Function compile flags: /Ogtpy
;	COMDAT ?GetContinious@CPerlinNoise1D@@QAEMM@Z
_TEXT	SEGMENT
_amp$ = -12						; size = 4
_result$ = -8						; size = 4
tv181 = -4						; size = 4
_v$ = 8							; size = 4
?GetContinious@CPerlinNoise1D@@QAEMM@Z PROC		; CPerlinNoise1D::GetContinious, COMDAT
; _this$ = ecx

; 91   : 	float t_v=v;
; 92   : 	if (mPrevContiniousTime!=0.0f)

	xorps	xmm3, xmm3
	movss	xmm1, DWORD PTR _v$[esp-4]
	sub	esp, 12					; 0000000cH
	push	esi
	push	edi
	mov	edi, ecx
	movss	xmm0, DWORD PTR [edi+4144]
	ucomiss	xmm0, xmm3
	lahf
	test	ah, 68					; 00000044H
	movaps	xmm2, xmm1
	jnp	SHORT $LN4@GetContini

; 93   : 	{
; 94   : 		v-=mPrevContiniousTime;

	subss	xmm1, xmm0
$LN4@GetContini:

; 95   : 	}
; 96   : 	mPrevContiniousTime=t_v;
; 97   : 	float result	= 0.0f;
; 98   : 	float amp		= mAmplitude;

	movss	xmm0, DWORD PTR [edi+2072]
	movss	DWORD PTR _amp$[esp+20], xmm0

; 99   : 	v				*=mFrequency;

	movss	xmm0, DWORD PTR [edi+2068]

; 100  : 	for(int i=0; i<mOctaves; i++){

	xor	esi, esi
	cmp	DWORD PTR [edi+2064], esi
	mulss	xmm0, xmm1
	movss	DWORD PTR [edi+4144], xmm2
	movss	DWORD PTR _result$[esp+20], xmm3
	movss	DWORD PTR _v$[esp+16], xmm0
	jle	$LN31@GetContini
	push	ebx
	lea	ebx, DWORD PTR [edi+2076]
	jmp	SHORT $LN3@GetContini
	npad	3
$LL33@GetContini:
	fstp	ST(0)
$LN3@GetContini:

; 101  : 		float		octave_time=mTimes[i];

	mov	ecx, ebx
	call	?begin@?$_Impl_vector@MV?$xalloc@M@@@priv@stlp_std@@QAEPAMXZ ; stlp_std::priv::_Impl_vector<float,xalloc<float> >::begin
	movss	xmm0, DWORD PTR [eax+esi*4]

; 102  : 		mTimes[i]	=octave_time+v;

	addss	xmm0, DWORD PTR _v$[esp+20]
	mov	ecx, ebx
	movss	DWORD PTR tv181[esp+24], xmm0
	call	?begin@?$_Impl_vector@MV?$xalloc@M@@@priv@stlp_std@@QAEPAMXZ ; stlp_std::priv::_Impl_vector<float,xalloc<float> >::begin

; 103  : 		result		+= noise(octave_time+v)*amp;

	fld	DWORD PTR tv181[esp+24]
	movss	xmm0, DWORD PTR tv181[esp+24]
	push	ecx
	mov	ecx, edi
	fstp	DWORD PTR [esp]
	movss	DWORD PTR [eax+esi*4], xmm0
	call	?noise@CPerlinNoise1D@@AAEMM@Z		; CPerlinNoise1D::noise
	fmul	DWORD PTR _amp$[esp+24]

; 104  : 		v			*= 2.0f;

	movss	xmm0, DWORD PTR _v$[esp+20]
	mulss	xmm0, DWORD PTR __real@40000000
	fadd	DWORD PTR _result$[esp+24]
	movss	DWORD PTR _v$[esp+20], xmm0

; 105  : 		amp			*= 0.5f;

	movss	xmm0, DWORD PTR _amp$[esp+24]
	mulss	xmm0, DWORD PTR __real@3f000000
	add	esi, 1
	fst	DWORD PTR _result$[esp+24]
	cmp	esi, DWORD PTR [edi+2064]
	movss	DWORD PTR _amp$[esp+24], xmm0
	jl	SHORT $LL33@GetContini
	pop	ebx
	pop	edi
	pop	esi

; 108  : }

	add	esp, 12					; 0000000cH
	ret	4
$LN31@GetContini:

; 106  : 	}
; 107  : 	return result;

	fld	DWORD PTR _result$[esp+20]
	pop	edi
	pop	esi

; 108  : }

	add	esp, 12					; 0000000cH
	ret	4
?GetContinious@CPerlinNoise1D@@QAEMM@Z ENDP		; CPerlinNoise1D::GetContinious
_TEXT	ENDS
PUBLIC	?Get@CPerlinNoise1D@@QAEMM@Z			; CPerlinNoise1D::Get
; Function compile flags: /Ogtpy
;	COMDAT ?Get@CPerlinNoise1D@@QAEMM@Z
_TEXT	SEGMENT
_amp$ = -8						; size = 4
_result$ = -4						; size = 4
_v$ = 8							; size = 4
?Get@CPerlinNoise1D@@QAEMM@Z PROC			; CPerlinNoise1D::Get, COMDAT
; _this$ = ecx

; 77   : {

	sub	esp, 8

; 78   : 	float result	= 0.0f;

	xorps	xmm0, xmm0
	push	esi
	mov	esi, ecx
	movss	DWORD PTR _result$[esp+12], xmm0

; 79   : 	float amp		= mAmplitude;

	movss	xmm0, DWORD PTR [esi+2072]
	push	edi
	movss	DWORD PTR _amp$[esp+16], xmm0

; 80   : 	v				*=mFrequency;

	movss	xmm0, DWORD PTR [esi+2068]
	mulss	xmm0, DWORD PTR _v$[esp+12]

; 81   : 	for(int i=0; i<mOctaves; i++){

	xor	edi, edi
	cmp	DWORD PTR [esi+2064], edi
	movss	DWORD PTR _v$[esp+12], xmm0
	jg	SHORT $LN3@Get

; 85   : 	}
; 86   : 	return result;

	fld	DWORD PTR _result$[esp+16]
	pop	edi
	pop	esi

; 87   : }

	add	esp, 8
	ret	4
$LL10@Get:

; 81   : 	for(int i=0; i<mOctaves; i++){

	fstp	ST(0)
$LN3@Get:

; 82   : 		result		+= noise(v)*amp;

	fld	DWORD PTR _v$[esp+12]
	push	ecx
	mov	ecx, esi
	fstp	DWORD PTR [esp]
	call	?noise@CPerlinNoise1D@@AAEMM@Z		; CPerlinNoise1D::noise
	fmul	DWORD PTR _amp$[esp+16]

; 83   : 		v			*= 2.0f;

	movss	xmm0, DWORD PTR _v$[esp+12]
	mulss	xmm0, DWORD PTR __real@40000000
	fadd	DWORD PTR _result$[esp+16]
	movss	DWORD PTR _v$[esp+12], xmm0

; 84   : 		amp			*= 0.5f;

	movss	xmm0, DWORD PTR _amp$[esp+16]
	mulss	xmm0, DWORD PTR __real@3f000000
	add	edi, 1
	fst	DWORD PTR _result$[esp+16]
	cmp	edi, DWORD PTR [esi+2064]
	movss	DWORD PTR _amp$[esp+16], xmm0
	jl	SHORT $LL10@Get
	pop	edi
	pop	esi

; 87   : }

	add	esp, 8
	ret	4
?Get@CPerlinNoise1D@@QAEMM@Z ENDP			; CPerlinNoise1D::Get
PUBLIC	?Get@CPerlinNoise3D@@QAEMMMM@Z			; CPerlinNoise3D::Get
; Function compile flags: /Ogtpy
;	COMDAT ?Get@CPerlinNoise3D@@QAEMMMM@Z
_TEXT	SEGMENT
_amp$ = -20						; size = 4
_result$ = -16						; size = 4
_vec$ = -12						; size = 12
_x$ = 8							; size = 4
_y$ = 12						; size = 4
_z$ = 16						; size = 4
?Get@CPerlinNoise3D@@QAEMMMM@Z PROC			; CPerlinNoise3D::Get, COMDAT
; _this$ = ecx

; 305  : {

	sub	esp, 20					; 00000014H

; 306  : 	Fvector3 vec	= {x,y,z};
; 307  : 	float result	= 0.0f;

	xorps	xmm0, xmm0
	push	esi
	mov	esi, ecx
	movss	DWORD PTR _result$[esp+24], xmm0

; 308  : 	float amp		= mAmplitude;

	movss	xmm0, DWORD PTR [esi+2072]
	movss	DWORD PTR _amp$[esp+24], xmm0

; 309  : 	vec[0]			*=mFrequency;

	movss	xmm0, DWORD PTR [esi+2068]
	movaps	xmm1, xmm0
	mulss	xmm1, DWORD PTR _x$[esp+20]
	push	edi
	movss	DWORD PTR _vec$[esp+28], xmm1

; 310  : 	vec[1]			*=mFrequency;

	movaps	xmm1, xmm0
	mulss	xmm1, DWORD PTR _y$[esp+24]

; 311  : 	vec[2]			*=mFrequency;

	mulss	xmm0, DWORD PTR _z$[esp+24]

; 312  : 	for(int i=0; i<mOctaves; i++){

	xor	edi, edi
	cmp	DWORD PTR [esi+2064], edi
	movss	DWORD PTR _vec$[esp+32], xmm1
	movss	DWORD PTR _vec$[esp+36], xmm0
	jg	SHORT $LN3@Get@2

; 318  : 	}
; 319  : 	return result;

	fld	DWORD PTR _result$[esp+28]
	pop	edi
	pop	esi

; 320  : }

	add	esp, 20					; 00000014H
	ret	12					; 0000000cH
$LL22@Get@2:

; 312  : 	for(int i=0; i<mOctaves; i++){

	fstp	ST(0)
$LN3@Get@2:

; 313  : 		result		+= noise(vec)*amp;

	lea	eax, DWORD PTR _vec$[esp+28]
	push	eax
	mov	ecx, esi
	call	?noise@CPerlinNoise3D@@AAEMABU?$_vector3@M@@@Z ; CPerlinNoise3D::noise
	fmul	DWORD PTR _amp$[esp+28]

; 314  : 		vec[0]		*= 2.0f;

	movss	xmm0, DWORD PTR __real@40000000
	movss	xmm1, DWORD PTR _vec$[esp+28]
	fadd	DWORD PTR _result$[esp+28]
	mulss	xmm1, xmm0
	movss	DWORD PTR _vec$[esp+28], xmm1

; 315  : 		vec[1]		*= 2.0f;

	movss	xmm1, DWORD PTR _vec$[esp+32]
	mulss	xmm1, xmm0
	fst	DWORD PTR _result$[esp+28]
	movss	DWORD PTR _vec$[esp+32], xmm1

; 316  : 		vec[2]		*= 2.0f;

	movss	xmm1, DWORD PTR _vec$[esp+36]
	mulss	xmm1, xmm0

; 317  : 		amp			*= 0.5f;

	movss	xmm0, DWORD PTR _amp$[esp+28]
	mulss	xmm0, DWORD PTR __real@3f000000
	add	edi, 1
	cmp	edi, DWORD PTR [esi+2064]
	movss	DWORD PTR _vec$[esp+36], xmm1
	movss	DWORD PTR _amp$[esp+28], xmm0
	jl	SHORT $LL22@Get@2
	pop	edi
	pop	esi

; 320  : }

	add	esp, 20					; 00000014H
	ret	12					; 0000000cH
?Get@CPerlinNoise3D@@QAEMMMM@Z ENDP			; CPerlinNoise3D::Get
_TEXT	ENDS
PUBLIC	?Get@CPerlinNoise2D@@QAEMMM@Z			; CPerlinNoise2D::Get
; Function compile flags: /Ogtpy
;	COMDAT ?Get@CPerlinNoise2D@@QAEMMM@Z
_TEXT	SEGMENT
_amp$ = -16						; size = 4
_result$ = -12						; size = 4
_vec$ = -8						; size = 8
_x$ = 8							; size = 4
_y$ = 12						; size = 4
?Get@CPerlinNoise2D@@QAEMMM@Z PROC			; CPerlinNoise2D::Get, COMDAT
; _this$ = ecx

; 194  : {

	push	ebp
	mov	ebp, esp
	and	esp, -8					; fffffff8H
	sub	esp, 16					; 00000010H

; 195  : 	Fvector2 vec	= {x,y};
; 196  : 	float result	= 0.0f;

	xorps	xmm0, xmm0
	push	esi
	mov	esi, ecx
	movss	DWORD PTR _result$[esp+20], xmm0

; 197  : 	float amp		= mAmplitude;

	movss	xmm0, DWORD PTR [esi+2072]
	movss	DWORD PTR _amp$[esp+20], xmm0

; 198  : 	vec[0]			*=mFrequency;

	movss	xmm0, DWORD PTR [esi+2068]
	push	edi
	movaps	xmm1, xmm0
	mulss	xmm1, DWORD PTR _x$[ebp]

; 199  : 	vec[1]			*=mFrequency;

	mulss	xmm0, DWORD PTR _y$[ebp]

; 200  : 	for(int i=0; i<mOctaves; i++){

	xor	edi, edi
	cmp	DWORD PTR [esi+2064], edi
	movss	DWORD PTR _vec$[esp+24], xmm1
	movss	DWORD PTR _vec$[esp+28], xmm0
	jg	SHORT $LN3@Get@3

; 205  : 	}
; 206  : 	return result;

	fld	DWORD PTR _result$[esp+24]

; 207  : }

	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp
	ret	8
	npad	5
$LL18@Get@3:

; 200  : 	for(int i=0; i<mOctaves; i++){

	fstp	ST(0)
$LN3@Get@3:

; 201  : 		result		+= noise(vec)*amp;

	lea	eax, DWORD PTR _vec$[esp+24]
	push	eax
	mov	ecx, esi
	call	?noise@CPerlinNoise2D@@AAEMABU?$_vector2@M@@@Z ; CPerlinNoise2D::noise
	fmul	DWORD PTR _amp$[esp+24]

; 202  : 		vec[0]		*= 2.0f;

	movss	xmm0, DWORD PTR __real@40000000
	movss	xmm1, DWORD PTR _vec$[esp+24]
	fadd	DWORD PTR _result$[esp+24]
	mulss	xmm1, xmm0
	movss	DWORD PTR _vec$[esp+24], xmm1

; 203  : 		vec[1]		*= 2.0f;

	movss	xmm1, DWORD PTR _vec$[esp+28]
	mulss	xmm1, xmm0
	fst	DWORD PTR _result$[esp+24]

; 204  : 		amp			*= 0.5f;

	movss	xmm0, DWORD PTR _amp$[esp+24]
	mulss	xmm0, DWORD PTR __real@3f000000
	add	edi, 1
	cmp	edi, DWORD PTR [esi+2064]
	movss	DWORD PTR _vec$[esp+28], xmm1
	movss	DWORD PTR _amp$[esp+24], xmm0
	jl	SHORT $LL18@Get@3

; 207  : }

	pop	edi
	pop	esi
	mov	esp, ebp
	pop	ebp
	ret	8
?Get@CPerlinNoise2D@@QAEMMM@Z ENDP			; CPerlinNoise2D::Get
END
