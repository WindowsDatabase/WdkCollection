

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 8.01.0622 */
/* @@MIDL_FILE_HEADING(  ) */



#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        EXTERN_C __declspec(selectany) const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif // !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, IID_IExplode,0x141DBAF0,0x55FB,0x11D1,0xB8,0x3E,0x00,0xA0,0xC9,0x33,0xBE,0x86);


MIDL_DEFINE_GUID(IID, IID_ICrShatter,0x63500AE1,0x0858,0x11D2,0x8C,0xE4,0x00,0xC0,0x4F,0x8E,0xCB,0x10);


MIDL_DEFINE_GUID(IID, IID_IDXTBlackHole,0xC3853C21,0x3F2E,0x11D2,0x99,0x00,0x00,0x00,0xF8,0x03,0xFF,0x7A);


MIDL_DEFINE_GUID(IID, IID_IDXTRoll,0x78F30B81,0x48AA,0x11D2,0x99,0x00,0x00,0x00,0xF8,0x03,0xFF,0x7A);


MIDL_DEFINE_GUID(IID, IID_IDXTSpin,0x3D2807C1,0x43DE,0x11D2,0x99,0x00,0x00,0x00,0xF8,0x03,0xFF,0x7A);


MIDL_DEFINE_GUID(IID, IID_IRipple,0x6DA4A05E,0x8E9E,0x11D1,0x90,0x4E,0x00,0xC0,0x4F,0xD9,0x18,0x9D);


MIDL_DEFINE_GUID(IID, IID_IHeightField,0x0492170A,0xB159,0x11d1,0x92,0x07,0x00,0x00,0xF8,0x75,0x8E,0x66);


MIDL_DEFINE_GUID(IID, IID_IDXTMetaStream,0x48654BC0,0xE51F,0x11D1,0xAA,0x1C,0x00,0x60,0x08,0x95,0xFB,0x99);


MIDL_DEFINE_GUID(IID, IID_IDXTText3D,0x50C4B592,0x7E8D,0x11d2,0x9B,0x4E,0x00,0xA0,0xC9,0x69,0x7C,0xD0);


MIDL_DEFINE_GUID(IID, IID_IDXTShapes,0x8241F013,0x84D3,0x11d2,0x97,0xE6,0x00,0x00,0xF8,0x03,0xFF,0x7A);


MIDL_DEFINE_GUID(IID, LIBID_DXTMSFT3Lib,0xE4BBA4C0,0x69B7,0x11d2,0x83,0x2D,0x00,0x00,0xF8,0x1F,0x59,0x95);


MIDL_DEFINE_GUID(CLSID, CLSID_Explode,0x141DBAF1,0x55FB,0x11D1,0xB8,0x3E,0x00,0xA0,0xC9,0x33,0xBE,0x86);


MIDL_DEFINE_GUID(CLSID, CLSID_ExplodeProp,0xC53059E1,0xE6E3,0x11d1,0xBA,0x12,0x00,0xC0,0x4F,0xB6,0xBD,0x36);


MIDL_DEFINE_GUID(CLSID, CLSID_Ripple,0x945F5842,0x3A8D,0x11D1,0x90,0x37,0x00,0xC0,0x4F,0xD9,0x18,0x9D);


MIDL_DEFINE_GUID(CLSID, CLSID_RipProp,0x945F5843,0x3A8D,0x11D1,0x90,0x37,0x00,0xC0,0x4F,0xD9,0x18,0x9D);


MIDL_DEFINE_GUID(CLSID, CLSID_HeightField,0x04921709,0xB159,0x11d1,0x92,0x07,0x00,0x00,0xF8,0x75,0x8E,0x66);


MIDL_DEFINE_GUID(CLSID, CLSID_HtFieldProp,0x7A8402E3,0xFBD6,0x11D1,0xB5,0xE0,0x00,0xAA,0x00,0x3B,0x60,0x61);


MIDL_DEFINE_GUID(CLSID, CLSID_DXTMetaStream,0x60A0C080,0xE505,0x11D1,0xAA,0x1C,0x00,0x60,0x08,0x95,0xFB,0x99);


MIDL_DEFINE_GUID(CLSID, CLSID_DXTMetaStreamProp,0xE3D77340,0xE505,0x11D1,0xAA,0x1C,0x00,0x60,0x08,0x95,0xFB,0x99);


MIDL_DEFINE_GUID(CLSID, CLSID_DXTText3D,0xD56F34F2,0x7E89,0x11d2,0x9B,0x4E,0x00,0xA0,0xC9,0x69,0x7C,0xD0);


MIDL_DEFINE_GUID(CLSID, CLSID_DXTText3DPP,0x50C4B593,0x7E8D,0x11d2,0x9B,0x4E,0x00,0xA0,0xC9,0x69,0x7C,0xD0);


MIDL_DEFINE_GUID(CLSID, CLSID_CrShatter,0x63500AE2,0x0858,0x11D2,0x8C,0xE4,0x00,0xC0,0x4F,0x8E,0xCB,0x10);


MIDL_DEFINE_GUID(CLSID, CLSID_CrShatterPP,0x99275F01,0x102E,0x11d2,0x8B,0x82,0x00,0xA0,0xC9,0x3C,0x09,0xB2);


MIDL_DEFINE_GUID(CLSID, CLSID_DXTBlackHole,0xC3853C22,0x3F2E,0x11D2,0x99,0x00,0x00,0x00,0xF8,0x03,0xFF,0x7A);


MIDL_DEFINE_GUID(CLSID, CLSID_DXTBlackHolePP,0xC3853C23,0x3F2E,0x11D2,0x99,0x00,0x00,0x00,0xF8,0x03,0xFF,0x7A);


MIDL_DEFINE_GUID(CLSID, CLSID_DXTRoll,0x78F30B82,0x48AA,0x11D2,0x99,0x00,0x00,0x00,0xF8,0x03,0xFF,0x7A);


MIDL_DEFINE_GUID(CLSID, CLSID_DXTRollPP,0x78F30B83,0x48AA,0x11D2,0x99,0x00,0x00,0x00,0xF8,0x03,0xFF,0x7A);


MIDL_DEFINE_GUID(CLSID, CLSID_DXTSpin,0x3D2807C2,0x43DE,0x11D2,0x99,0x00,0x00,0x00,0xF8,0x03,0xFF,0x7A);


MIDL_DEFINE_GUID(CLSID, CLSID_DXTSpinPP,0x3D2807C3,0x43DE,0x11D2,0x99,0x00,0x00,0x00,0xF8,0x03,0xFF,0x7A);


MIDL_DEFINE_GUID(CLSID, CLSID_DXTShapes,0x8241F015,0x84D3,0x11d2,0x97,0xE6,0x00,0x00,0xF8,0x03,0xFF,0x7A);


MIDL_DEFINE_GUID(CLSID, CLSID_DXTShapesPP,0x8241F016,0x84D3,0x11d2,0x97,0xE6,0x00,0x00,0xF8,0x03,0xFF,0x7A);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



