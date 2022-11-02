#include "includes.h"

TYPE_GetRandomDraw GetRandomDraw = (TYPE_GetRandomDraw) 0x48EB70;
TYPE_GetSpheres GetSpheres = (TYPE_GetSpheres) 0x45EE70;
TYPE_GetLaraJointPos GetLaraJointPos = (TYPE_GetLaraJointPos)0x41D890;
TYPE_phd_GetVectorAngles phd_GetVectorAngles = (TYPE_phd_GetVectorAngles)0x48E710;
TYPE_EffectNewRoom EffectNewRoom = (TYPE_EffectNewRoom) 0x453F60;
TYPE_TestTriggersAtXYZ TestTriggersAtXYZ = (TYPE_TestTriggersAtXYZ) 0x460E80;
TYPE_GetFreeSpark GetFreeSpark = (TYPE_GetFreeSpark)0x433830;
TYPE_TriggerFireFlame TriggerFireFlame = (TYPE_TriggerFireFlame)0x434BB0;
TYPE_TriggerShockwave TriggerShockwave = (TYPE_TriggerShockwave) 0x43A420;
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
TYPE_phd_PutPolygons phd_PutPolygons = (TYPE_phd_PutPolygons) 0x47BB30;