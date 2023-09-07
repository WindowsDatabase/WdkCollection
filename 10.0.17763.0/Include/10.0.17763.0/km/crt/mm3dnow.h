//$TAG BIZDEV
//  $IPCategory:     
//  $DealPointID:    41652
//  $AgreementName:  
//  $AgreementType:  
//  $ExternalOrigin: 
//$ENDTAG

//$TAG ENGR 
//  $Owner:    bryant
//  $Module:   crts_crtw32
//
//$ENDTAG

/* Copyright (c) Microsoft. All rights reserved. */

/*
 * mm3dnow.h
 *
 */

#pragma once
#ifndef __midl
#ifndef _MM3DNOW_H_INCLUDED
#define _MM3DNOW_H_INCLUDED

#include <crtdefs.h>
#include <mmintrin.h>
#include <xmmintrin.h>

#if defined __cplusplus
extern "C" { /* Intrinsics use C name-mangling.  */
#endif /* __cplusplus */

/* 3DNOW intrinsics */

void _m_femms(void);
__m64 _m_pavgusb(__m64, __m64);
__m64 _m_pf2id(__m64);
__m64 _m_pfacc(__m64, __m64);
__m64 _m_pfadd(__m64, __m64);
__m64 _m_pfcmpeq(__m64, __m64);
__m64 _m_pfcmpge(__m64, __m64);
__m64 _m_pfcmpgt(__m64, __m64);
__m64 _m_pfmax(__m64, __m64);
__m64 _m_pfmin(__m64, __m64);
__m64 _m_pfmul(__m64, __m64);
__m64 _m_pfrcp(__m64);
__m64 _m_pfrcpit1(__m64, __m64);
__m64 _m_pfrcpit2(__m64, __m64);
__m64 _m_pfrsqrt(__m64);
__m64 _m_pfrsqit1(__m64, __m64);
__m64 _m_pfsub(__m64, __m64);
__m64 _m_pfsubr(__m64, __m64);
__m64 _m_pi2fd(__m64);
__m64 _m_pmulhrw(__m64, __m64);
void _m_prefetch(void*);
void _m_prefetchw(volatile const void*_Source);

__m64 _m_from_float(float);
float _m_to_float(__m64);

/* Athlon DSP intrinsics */

__m64 _m_pf2iw(__m64);
__m64 _m_pfnacc(__m64, __m64);
__m64 _m_pfpnacc(__m64, __m64);
__m64 _m_pi2fw(__m64);
__m64 _m_pswapd(__m64);

#if defined __cplusplus
}; /* End "C" */
#endif /* __cplusplus */

#endif
#endif
/* 88bf0570-3001-4e78-a5f2-be5765546192 */ 
