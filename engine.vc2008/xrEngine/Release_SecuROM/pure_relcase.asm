; Listing generated by Microsoft (R) Optimizing Compiler Version 14.00.50727.762 

	TITLE	D:\CLEARSKY\sources\engine\xrEngine\pure_relcase.cpp
	.686P
	.XMM
	include listing.inc
	.model	flat

INCLUDELIB OLDNAMES

PUBLIC	??_Epure_relcase@@UAEPAXI@Z			; pure_relcase::`vector deleting destructor'
PUBLIC	??_R4pure_relcase@@6B@				; pure_relcase::`RTTI Complete Object Locator'
PUBLIC	??_R3pure_relcase@@8				; pure_relcase::`RTTI Class Hierarchy Descriptor'
PUBLIC	??_R2pure_relcase@@8				; pure_relcase::`RTTI Base Class Array'
PUBLIC	??_R1A@?0A@EA@pure_relcase@@8			; pure_relcase::`RTTI Base Class Descriptor at (0,-1,0,64)'
PUBLIC	??_R0?AVpure_relcase@@@8			; pure_relcase `RTTI Type Descriptor'
PUBLIC	??_7pure_relcase@@6B@				; pure_relcase::`vftable'
;	COMDAT ??_7pure_relcase@@6B@
CONST	SEGMENT
??_7pure_relcase@@6B@ DD FLAT:??_R4pure_relcase@@6B@	; pure_relcase::`vftable'
	DD	FLAT:??_Epure_relcase@@UAEPAXI@Z
CONST	ENDS
;	COMDAT ??_R4pure_relcase@@6B@
rdata$r	SEGMENT
??_R4pure_relcase@@6B@ DD 00H				; pure_relcase::`RTTI Complete Object Locator'
	DD	00H
	DD	00H
	DD	FLAT:??_R0?AVpure_relcase@@@8
	DD	FLAT:??_R3pure_relcase@@8
rdata$r	ENDS
;	COMDAT ??_R3pure_relcase@@8
rdata$r	SEGMENT
??_R3pure_relcase@@8 DD 00H				; pure_relcase::`RTTI Class Hierarchy Descriptor'
	DD	00H
	DD	01H
	DD	FLAT:??_R2pure_relcase@@8
rdata$r	ENDS
;	COMDAT ??_R2pure_relcase@@8
rdata$r	SEGMENT
??_R2pure_relcase@@8 DD FLAT:??_R1A@?0A@EA@pure_relcase@@8 ; pure_relcase::`RTTI Base Class Array'
rdata$r	ENDS
;	COMDAT ??_R1A@?0A@EA@pure_relcase@@8
rdata$r	SEGMENT
??_R1A@?0A@EA@pure_relcase@@8 DD FLAT:??_R0?AVpure_relcase@@@8 ; pure_relcase::`RTTI Base Class Descriptor at (0,-1,0,64)'
	DD	00H
	DD	00H
	DD	0ffffffffH
	DD	00H
	DD	040H
	DD	FLAT:??_R3pure_relcase@@8
rdata$r	ENDS
;	COMDAT ??_R0?AVpure_relcase@@@8
_DATA	SEGMENT
??_R0?AVpure_relcase@@@8 DD FLAT:??_7type_info@@6B@	; pure_relcase `RTTI Type Descriptor'
	DD	00H
	DB	'.?AVpure_relcase@@', 00H
PUBLIC	??4pure_relcase@@QAEAAV0@ABV0@@Z		; pure_relcase::operator=
; Function compile flags: /Ogtpy
;	COMDAT ??4pure_relcase@@QAEAAV0@ABV0@@Z
_TEXT	SEGMENT
___that$ = 8						; size = 4
??4pure_relcase@@QAEAAV0@ABV0@@Z PROC			; pure_relcase::operator=, COMDAT
; _this$ = ecx
	mov	eax, ecx
	mov	ecx, DWORD PTR ___that$[esp-4]
	mov	edx, DWORD PTR [ecx+4]
	mov	DWORD PTR [eax+4], edx
	ret	4
??4pure_relcase@@QAEAAV0@ABV0@@Z ENDP			; pure_relcase::operator=
_TEXT	ENDS
PUBLIC	??0pure_relcase@@QAE@ABV0@@Z			; pure_relcase::pure_relcase
; Function compile flags: /Ogtpy
;	COMDAT ??0pure_relcase@@QAE@ABV0@@Z
_TEXT	SEGMENT
___that$ = 8						; size = 4
??0pure_relcase@@QAE@ABV0@@Z PROC			; pure_relcase::pure_relcase, COMDAT
; _this$ = ecx
	mov	eax, ecx
	mov	ecx, DWORD PTR ___that$[esp-4]
	mov	DWORD PTR [eax], OFFSET ??_7pure_relcase@@6B@
	mov	edx, DWORD PTR [ecx+4]
	mov	DWORD PTR [eax+4], edx
	ret	4
??0pure_relcase@@QAE@ABV0@@Z ENDP			; pure_relcase::pure_relcase
PUBLIC	??1pure_relcase@@UAE@XZ				; pure_relcase::~pure_relcase
; Function compile flags: /Ogtpy
; File d:\clearsky\sources\engine\xrengine\pure_relcase.cpp
;	COMDAT ??1pure_relcase@@UAE@XZ
_TEXT	SEGMENT
??1pure_relcase@@UAE@XZ PROC				; pure_relcase::~pure_relcase, COMDAT
; _this$ = ecx

; 7    : {

	mov	DWORD PTR [ecx], OFFSET ??_7pure_relcase@@6B@

; 8    : 	if (g_pGameLevel) 

	mov	eax, DWORD PTR ?g_pGameLevel@@3PAVIGame_Level@@A ; g_pGameLevel
	test	eax, eax
	je	SHORT $LN1@pure_relca

; 9    : 		g_pGameLevel->Objects.relcase_unregister	(&m_ID);

	add	ecx, 4
	push	ecx
	lea	ecx, DWORD PTR [eax+76]
	call	?relcase_unregister@CObjectList@@QAEXPAH@Z ; CObjectList::relcase_unregister
$LN1@pure_relca:

; 10   : }

	ret	0
??1pure_relcase@@UAE@XZ ENDP				; pure_relcase::~pure_relcase
; Function compile flags: /Ogtpy
_TEXT	ENDS
;	COMDAT ??_Epure_relcase@@UAEPAXI@Z
_TEXT	SEGMENT
$T394267 = -8						; size = 4
tv194 = -4						; size = 4
___flags$ = 8						; size = 4
??_Epure_relcase@@UAEPAXI@Z PROC			; pure_relcase::`vector deleting destructor', COMDAT
; _this$ = ecx
	sub	esp, 8
	push	ebx
	mov	bl, BYTE PTR ___flags$[esp+8]
	test	bl, 2
	push	ebp
	mov	ebp, ecx
	je	$LN3@vector@74
	mov	eax, DWORD PTR [ebp-4]
	push	edi
	lea	edi, DWORD PTR [ebp-4]
	lea	ebp, DWORD PTR [ebp+eax*8]
	sub	eax, 1
	mov	DWORD PTR tv194[esp+20], edi
	mov	DWORD PTR $T394267[esp+20], eax
	js	SHORT $LN71@vector@74
	push	esi
	npad	3
$LL7@vector@74:
	sub	ebp, 8
	mov	DWORD PTR [ebp], OFFSET ??_7pure_relcase@@6B@
	mov	esi, DWORD PTR ?g_pGameLevel@@3PAVIGame_Level@@A ; g_pGameLevel
	test	esi, esi
	je	SHORT $LN55@vector@74
	add	esi, 262280				; 00040088H
	mov	ecx, esi
	call	?end@?$_Impl_vector@USRelcasePair@CObjectList@@V?$xalloc@USRelcasePair@CObjectList@@@@@priv@stlp_std@@QAEPAUSRelcasePair@CObjectList@@XZ ; stlp_std::priv::_Impl_vector<CObjectList::SRelcasePair,xalloc<CObjectList::SRelcasePair> >::end
	mov	ebx, DWORD PTR [ebp+4]
	mov	edi, eax
	mov	ecx, esi
	sub	edi, 12					; 0000000cH
	call	?begin@?$_Impl_vector@USRelcasePair@CObjectList@@V?$xalloc@USRelcasePair@CObjectList@@@@@priv@stlp_std@@QAEPAUSRelcasePair@CObjectList@@XZ ; stlp_std::priv::_Impl_vector<CObjectList::SRelcasePair,xalloc<CObjectList::SRelcasePair> >::begin
	lea	ecx, DWORD PTR [ebx+ebx*2]
	push	edi
	lea	ecx, DWORD PTR [eax+ecx*4]
	call	??4SRelcasePair@CObjectList@@QAEAAU01@ABU01@@Z
	mov	ecx, esi
	call	?end@?$_Impl_vector@USRelcasePair@CObjectList@@V?$xalloc@USRelcasePair@CObjectList@@@@@priv@stlp_std@@QAEPAUSRelcasePair@CObjectList@@XZ ; stlp_std::priv::_Impl_vector<CObjectList::SRelcasePair,xalloc<CObjectList::SRelcasePair> >::end
	mov	edx, DWORD PTR [eax-12]
	mov	edi, DWORD PTR tv194[esp+24]
	sub	eax, 12					; 0000000cH
	mov	eax, DWORD PTR [ebp+4]
	mov	DWORD PTR [edx], eax
	add	DWORD PTR [esi+4], -12			; fffffff4H
$LN55@vector@74:
	sub	DWORD PTR $T394267[esp+24], 1
	jns	SHORT $LL7@vector@74
	mov	bl, BYTE PTR ___flags$[esp+20]
	pop	esi
$LN71@vector@74:
	test	bl, 1
	je	SHORT $LN59@vector@74
	test	edi, edi
	je	SHORT $LN59@vector@74
	mov	ecx, DWORD PTR __imp_?Memory@@3VxrMemory@@A
	push	edi
	call	DWORD PTR __imp_?mem_free@xrMemory@@QAEXPAX@Z
$LN59@vector@74:
	mov	eax, edi
	pop	edi
	pop	ebp
	pop	ebx
	add	esp, 8
	ret	4
$LN3@vector@74:
	mov	DWORD PTR [ebp], OFFSET ??_7pure_relcase@@6B@
	mov	eax, DWORD PTR ?g_pGameLevel@@3PAVIGame_Level@@A ; g_pGameLevel
	test	eax, eax
	je	SHORT $LN62@vector@74
	lea	ecx, DWORD PTR [ebp+4]
	push	ecx
	lea	ecx, DWORD PTR [eax+76]
	call	?relcase_unregister@CObjectList@@QAEXPAH@Z ; CObjectList::relcase_unregister
$LN62@vector@74:
	test	bl, 1
	je	SHORT $LN67@vector@74
	mov	ecx, DWORD PTR __imp_?Memory@@3VxrMemory@@A
	push	ebp
	call	DWORD PTR __imp_?mem_free@xrMemory@@QAEXPAX@Z
$LN67@vector@74:
	mov	eax, ebp
	pop	ebp
	pop	ebx
	add	esp, 8
	ret	4
??_Epure_relcase@@UAEPAXI@Z ENDP			; pure_relcase::`vector deleting destructor'
_TEXT	ENDS
PUBLIC	??_Gpure_relcase@@UAEPAXI@Z			; pure_relcase::`scalar deleting destructor'
; Function compile flags: /Ogtpy
;	COMDAT ??_Gpure_relcase@@UAEPAXI@Z
_TEXT	SEGMENT
___flags$ = 8						; size = 4
??_Gpure_relcase@@UAEPAXI@Z PROC			; pure_relcase::`scalar deleting destructor', COMDAT
; _this$ = ecx
	push	esi
	mov	esi, ecx
	mov	DWORD PTR [esi], OFFSET ??_7pure_relcase@@6B@
	mov	eax, DWORD PTR ?g_pGameLevel@@3PAVIGame_Level@@A ; g_pGameLevel
	test	eax, eax
	je	SHORT $LN4@scalar@48
	lea	ecx, DWORD PTR [esi+4]
	push	ecx
	lea	ecx, DWORD PTR [eax+76]
	call	?relcase_unregister@CObjectList@@QAEXPAH@Z ; CObjectList::relcase_unregister
$LN4@scalar@48:
	test	BYTE PTR ___flags$[esp], 1
	je	SHORT $LN12@scalar@48
	mov	ecx, DWORD PTR __imp_?Memory@@3VxrMemory@@A
	push	esi
	call	DWORD PTR __imp_?mem_free@xrMemory@@QAEXPAX@Z
$LN12@scalar@48:
	mov	eax, esi
	pop	esi
	ret	4
??_Gpure_relcase@@UAEPAXI@Z ENDP			; pure_relcase::`scalar deleting destructor'
END