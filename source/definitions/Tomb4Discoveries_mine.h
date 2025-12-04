#pragma once
// FOR_YOU: here you can type your discoveries about tomb4 procedures
// if you mean use the C language to call them, you can create the
// prototypes of them like it has been done in the "DefTomb4Funct.h" file
// While if you wish use only assembly code, you can type here
// the #define ADR_NOME_PROC 0x404040  to remember and use in the asm
// code all addresses of the tomb4 procedures

// TYPE_HERE: prototype (c++ language), mnemonic constants for 
// addresses (assembly)

#include "structures_mine.h"

typedef long (__cdecl *TYPE_GetRandomDraw) (void);
typedef int (__cdecl *TYPE_GetSpheres)(Tr4ItemInfo* item, MeshSphere* ptr, int flags);
typedef int (__cdecl *TYPE_GetLaraJointPos) (phd_vector *pPos, int joint);
typedef void (__cdecl *TYPE_phd_GetVectorAngles) (int distX, int distY, int distZ, short* dest);
typedef void (__cdecl *TYPE_phd_PushUnitMatrix) (void);
typedef void (__cdecl *TYPE_EffectNewRoom) (short fx_num, short room_num);
typedef void (__cdecl *TYPE_TestTriggersAtXYZ)(int x, int y, int z, short room_number, int heavy, int flags);
typedef long (__cdecl *TYPE_GetFreeSpark) (void);
typedef void (__cdecl *TYPE_TriggerShockwave)(phd_vector *pos, int outer_rad_inner_rad, int speed, int life_b_g_r, int flags_angle);
typedef long (__cdecl *TYPE_GetFrames) (Tr4ItemInfo* item, short** frm, long* rate);
typedef void (__cdecl *TYPE_InitInterpolate2) (long frac, long rate);
typedef void (__cdecl *TYPE_InterpolateMatrix) (void);
typedef void (__cdecl *TYPE_CalcLaraMatrices)(int flag);
typedef void (__cdecl *TYPE_gar_RotYXZsuperpack) (short** pprot, long skip);
typedef void (__cdecl *TYPE_gar_RotYXZsuperpack_I) (short** rot1, short** rot2, long skip);
typedef void (__cdecl *TYPE_phd_TranslateRel) (long x, long y, long z);
typedef void (__cdecl *TYPE_phd_TranslateRel_I) (long x, long y, long z);
typedef void (__cdecl *TYPE_phd_TranslateRel_ID)(long x1, long y1, long z1, long x2, long y2, long z2);
typedef void (__cdecl *TYPE_phd_RotY_I) (short angle);
typedef void (__cdecl *TYPE_phd_RotX_I) (short angle);
typedef void (__cdecl *TYPE_phd_RotZ_I) (short angle);
typedef void (__cdecl *TYPE_phd_RotX) (short angle);
typedef void (__cdecl *TYPE_phd_RotZ) (short angle);
typedef void (__cdecl *TYPE_phd_PushMatrix_I) (void);
typedef void (__cdecl *TYPE_phd_PopMatrix_I) (void);
typedef long (__cdecl *TYPE_phd_atan)(long x, long y);
typedef long (__cdecl *TYPE_phd_sqrt)(long x);

typedef void (__cdecl *TYPE_UpdateLightning)(void);
typedef void (__cdecl *TYPE_DrawLightning)(void);
typedef void (__cdecl *TYPE_S_DrawSparks)(void);

typedef void (__cdecl *TYPE_setXY4) (D3DTLVERTEX* v, long x1, long y1, long x2, long y2, long x3, long y3, long x4, long y4, long z, short* clip);
typedef void (__cdecl *TYPE_setXYZ4) (D3DTLVERTEX* v, long x1, long y1, long z1, long x2, long y2, long z2, long x3, long y3, long z3, long x4, long y4, long z4, short* clip);
typedef void (__cdecl *TYPE_setXYZ3) (D3DTLVERTEX* v, long x1, long y1, long z1, long x2, long y2, long z2, long x3, long y3, long z3, short* clip);
typedef long (__cdecl *TYPE_ClipLine) (long& x1, long& y1, long z1, long& x2, long& y2, long z2, long xmin, long ymin, long xmax, long ymax);
typedef void (__cdecl *TYPE_AddLineSorted) (D3DTLVERTEX* v1, D3DTLVERTEX* v2, long l1);
typedef void (__cdecl *TYPE_AddTriSorted) (D3DTLVERTEX* v, short s1, short s2, short s3, TextureStruct* t, long l1);
typedef void (__cdecl *TYPE_AddQuadSorted) (D3DTLVERTEX* v, short s1, short s2, short s3, short s4, TextureStruct* t, long l1);
typedef void (__cdecl *TYPE_phd_PutPolygons) (short* mesh, int clip);
typedef void (__cdecl *TYPE_CalculateObjectLighting) (Tr4ItemInfo* pItem, short* pFrame);

extern TYPE_GetRandomDraw GetRandomDraw;
extern TYPE_GetSpheres GetSpheres;
extern TYPE_GetLaraJointPos GetLaraJointPos;
extern TYPE_phd_GetVectorAngles phd_GetVectorAngles;
extern TYPE_phd_PushUnitMatrix phd_PushUnitMatrix;
extern TYPE_EffectNewRoom EffectNewRoom;
extern TYPE_TestTriggersAtXYZ TestTriggersAtXYZ;
extern TYPE_GetFreeSpark GetFreeSpark;
extern TYPE_TriggerShockwave TriggerShockwave;
extern TYPE_GetFrames GetFrames;
extern TYPE_InitInterpolate2 InitInterpolate2;
extern TYPE_InterpolateMatrix InterpolateMatrix;
extern TYPE_CalcLaraMatrices CalcLaraMatrices;
extern TYPE_gar_RotYXZsuperpack gar_RotYXZsuperpack;
extern TYPE_gar_RotYXZsuperpack_I gar_RotYXZsuperpack_I;
extern TYPE_phd_TranslateRel phd_TranslateRel;
extern TYPE_phd_TranslateRel_I phd_TranslateRel_I;
extern TYPE_phd_TranslateRel_ID phd_TranslateRel_ID;
extern TYPE_phd_RotX phd_RotX;
extern TYPE_phd_RotZ phd_RotZ;
extern TYPE_phd_RotX_I phd_RotX_I;
extern TYPE_phd_RotY_I phd_RotY_I;
extern TYPE_phd_RotZ_I phd_RotZ_I;
extern TYPE_phd_PushMatrix_I phd_PushMatrix_I;
extern TYPE_phd_PopMatrix_I phd_PopMatrix_I;
extern TYPE_phd_atan phd_atan;
extern TYPE_phd_sqrt phd_sqrt;

extern TYPE_UpdateLightning UpdateLightning;
extern TYPE_DrawLightning DrawLightning;
extern TYPE_S_DrawSparks S_DrawSparks;

extern TYPE_setXY4 setXY4;
extern TYPE_setXYZ4 setXYZ4;
extern TYPE_setXYZ3 setXYZ3;
extern TYPE_ClipLine ClipLine;
extern TYPE_AddLineSorted* AddLineSorted; // function pointer
extern TYPE_AddTriSorted* AddTriSorted; // function pointer
extern TYPE_AddQuadSorted* AddQuadSorted; // function pointer
extern TYPE_phd_PutPolygons phd_PutPolygons;
extern TYPE_CalculateObjectLighting CalculateObjectLighting;
