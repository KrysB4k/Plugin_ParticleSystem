#pragma once
// FOR_YOU: here you can type your discoveries about tomb4 procedures
// if you mean use the C language to call them, you can create the
// prototypes of them like it has been done in the "DefTomb4Funct.h" file
// While if you wish use only assembly code, you can type here
// the #define ADR_NOME_PROC 0x404040  to remember and use in the asm
// code all addresses of the tomb4 procedures

// TYPE_HERE: prototype (c++ language), mnemonic constants for 
// addresses (assembly)

typedef long (__cdecl *TYPE_GetRandomDraw) (void);
typedef int (__cdecl *TYPE_GetSpheres)(Tr4ItemInfo* item, MeshSphere* ptr, int flags);
typedef int(__cdecl *TYPE_GetLaraJointPos) (phd_vector *pPos, int joint);
typedef void(__cdecl *TYPE_phd_GetVectorAngles) (int distX, int distY, int distZ, short* dest);
typedef void(__cdecl *TYPE_EffectNewRoom) (short fx_num, short room_num);
typedef void (__cdecl *TYPE_TestTriggersAtXYZ)(int x, int y, int z, short room_number, int heavy, int flags);
typedef long(__cdecl *TYPE_GetFreeSpark) (void);
typedef void(__cdecl *TYPE_TriggerFireFlame) (long x, long y, long z, long body_part, long type);
typedef void (__cdecl *TYPE_TriggerShockwave)(phd_vector *pos, int outer_rad_inner_rad, int speed, int life_b_g_r, int flags_angle);
typedef void (__cdecl *TYPE_TriggerBlood)(int x, int y, int z, int direction, int speed);
typedef void (__cdecl *TYPE_UpdateBeetles)(void);
typedef void (__cdecl *TYPE_UpdateFish)(void);

typedef void (__cdecl *TYPE_S_DrawSparks)(void);
typedef void (__cdecl *TYPE_S_DrawSplashes)(void);
typedef void (__cdecl *TYPE_DrawFish)(void);
typedef void (__cdecl *TYPE_DrawBeetles)(void);
typedef void (__cdecl *TYPE_DrawRopeList)(void);
typedef void (__cdecl *TYPE_DrawBubbles)(void);
typedef void (__cdecl *TYPE_DrawDebris)(void);

typedef void (__cdecl *TYPE_setXY4) (D3DTLVERTEX* v, long x1, long y1, long x2, long y2, long x3, long y3, long x4, long y4, long z, short* clip);
typedef long (__cdecl *TYPE_ClipLine) (long& x1, long& y1, long z1, long& x2, long& y2, long z2, long xmin, long ymin, long xmax, long ymax);
typedef void (__cdecl *TYPE_AddQuadSorted) (D3DTLVERTEX* v, short s1, short s2, short s3, short s4, TextureStruct* t, long l1);
typedef void (__cdecl *TYPE_AddLineSorted) (D3DTLVERTEX* v1, D3DTLVERTEX* v2, long l1);
typedef void(__cdecl *TYPE_phd_PutPolygons) (short* mesh, int clip);

extern TYPE_GetRandomDraw GetRandomDraw;
extern TYPE_GetSpheres GetSpheres;
extern TYPE_GetLaraJointPos GetLaraJointPos;
extern TYPE_phd_GetVectorAngles phd_GetVectorAngles;
extern TYPE_EffectNewRoom EffectNewRoom;
extern TYPE_TestTriggersAtXYZ TestTriggersAtXYZ;
extern TYPE_GetFreeSpark GetFreeSpark;
extern TYPE_TriggerFireFlame TriggerFireFlame;
extern TYPE_TriggerShockwave TriggerShockwave;
extern TYPE_TriggerBlood TriggerBlood;
extern TYPE_UpdateBeetles UpdateBeetles;
extern TYPE_UpdateFish UpdateFish;

extern TYPE_S_DrawSparks S_DrawSparks;
extern TYPE_S_DrawSplashes S_DrawSplashes;
extern TYPE_DrawFish DrawFish;
extern TYPE_DrawBeetles DrawBeetles;
extern TYPE_DrawRopeList DrawRopeList;
extern TYPE_DrawBubbles DrawBubbles;
extern TYPE_DrawDebris DrawDebris;

extern TYPE_setXY4 setXY4;
extern TYPE_ClipLine ClipLine;
extern TYPE_AddQuadSorted* AddQuadSorted; // function pointer
extern TYPE_AddLineSorted* AddLineSorted; // function pointer
extern TYPE_phd_PutPolygons phd_PutPolygons;