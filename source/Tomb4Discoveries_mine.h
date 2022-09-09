
// FOR_YOU: here you can type your discoveries about tomb4 procedures
// if you mean use the C language to call them, you can create the
// prototypes of them like it has been done in the "DefTomb4Funct.h" file
// and "functions.h" file.
// While if you wish use only assembly code, you can type here
// the #define ADR_NOME_PROC 0x404040  to remember and use in the asm
// code all addresses of the tomb4 procedures

// TYPE_HERE: prototype (c++ language), mnemonic constants for 
// addresses (assembly)

typedef long (__cdecl *TYPE_GetRandomDraw) (void);
typedef int (__cdecl *TYPE_GetSpheres)(Tr4ItemInfo* item, MeshSphere* ptr, int flags);
typedef int(__cdecl *TYPE_GetLaraJointPos) (phd_vector *pPos, int joint);
typedef void(__cdecl *TYPE_EffectNewRoom) (short fx_num, short room_num);
typedef void (__cdecl *TYPE_TestTriggersAtXYZ)(int x, int y, int z, short room_number, int heavy, int flags);
typedef long(__cdecl *TYPE_GetFreeSpark) (void);
typedef void(__cdecl *TYPE_TriggerFireFlame) (long x, long y, long z, long body_part, long type);
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

TYPE_GetRandomDraw GetRandomDraw = (TYPE_GetRandomDraw) 0x48EB70;
TYPE_GetSpheres GetSpheres = (TYPE_GetSpheres) 0x45EE70;
TYPE_GetLaraJointPos GetLaraJointPos = (TYPE_GetLaraJointPos)0x41D890;
TYPE_EffectNewRoom EffectNewRoom = (TYPE_EffectNewRoom) 0x453F60;
TYPE_TestTriggersAtXYZ TestTriggersAtXYZ = (TYPE_TestTriggersAtXYZ) 0x460E80;
TYPE_GetFreeSpark GetFreeSpark = (TYPE_GetFreeSpark)0x433830;
TYPE_TriggerFireFlame TriggerFireFlame = (TYPE_TriggerFireFlame)0x434BB0;
TYPE_TriggerBlood TriggerBlood = (TYPE_TriggerBlood) 0x4388E0;
TYPE_UpdateBeetles UpdateBeetles = (TYPE_UpdateBeetles) 0x40E2F0;
TYPE_UpdateFish UpdateFish = (TYPE_UpdateFish) 0x403850;

TYPE_S_DrawSparks S_DrawSparks = (TYPE_S_DrawSparks) 0x439B40;
TYPE_S_DrawSplashes S_DrawSplashes = (TYPE_S_DrawSplashes) 0x484280;
TYPE_DrawFish DrawFish = (TYPE_DrawFish) 0x403BF0;
TYPE_DrawBeetles DrawBeetles = (TYPE_DrawBeetles) 0x40E550;
TYPE_DrawRopeList DrawRopeList = (TYPE_DrawRopeList) 0x4587F0;
TYPE_DrawBubbles DrawBubbles = (TYPE_DrawBubbles) 0x483F20;
TYPE_DrawDebris DrawDebris = (TYPE_DrawDebris) 0x487940;

TYPE_setXY4 setXY4 = (TYPE_setXY4) 0x483420;
TYPE_ClipLine ClipLine = (TYPE_ClipLine) 0x486410;
TYPE_AddQuadSorted* AddQuadSorted = (TYPE_AddQuadSorted*) 0x5339A4; // function pointer
TYPE_AddLineSorted* AddLineSorted = (TYPE_AddLineSorted*) 0x5339A0; // function pointer










