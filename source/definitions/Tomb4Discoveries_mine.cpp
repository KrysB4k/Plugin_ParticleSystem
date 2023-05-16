#include "..\definitions\includes.h"

TYPE_GetRandomDraw GetRandomDraw = (TYPE_GetRandomDraw) 0x48EB70;
TYPE_GetSpheres GetSpheres = (TYPE_GetSpheres) 0x45EE70;
TYPE_GetLaraJointPos GetLaraJointPos = (TYPE_GetLaraJointPos) 0x41D890;
TYPE_phd_GetVectorAngles phd_GetVectorAngles = (TYPE_phd_GetVectorAngles) 0x48E710;
TYPE_phd_PushUnitMatrix phd_PushUnitMatrix = (TYPE_phd_PushUnitMatrix) 0x48DF90;
TYPE_EffectNewRoom EffectNewRoom = (TYPE_EffectNewRoom) 0x453F60;
TYPE_TestTriggersAtXYZ TestTriggersAtXYZ = (TYPE_TestTriggersAtXYZ) 0x460E80;
TYPE_GetFreeSpark GetFreeSpark = (TYPE_GetFreeSpark) 0x433830;
TYPE_TriggerShockwave TriggerShockwave = (TYPE_TriggerShockwave) 0x43A420;
TYPE_GetFrames GetFrames = (TYPE_GetFrames) 0x450450;

TYPE_UpdateLightning UpdateLightning = (TYPE_UpdateLightning) 0x43A910;
TYPE_DrawLightning DrawLightning = (TYPE_DrawLightning) 0x48B760;

TYPE_setXY4 setXY4 = (TYPE_setXY4) 0x483420;
TYPE_setXYZ3 setXYZ3 = (TYPE_setXYZ3) 0x483BD0;
TYPE_setXYZ4 setXYZ4 = (TYPE_setXYZ4) 0x483770;
TYPE_ClipLine ClipLine = (TYPE_ClipLine) 0x486410;
TYPE_AddLineSorted* AddLineSorted = (TYPE_AddLineSorted*) 0x5339A0; // function pointer
TYPE_AddTriSorted* AddTriSorted = (TYPE_AddTriSorted*) 0x53399C;	// function pointer
TYPE_AddQuadSorted* AddQuadSorted = (TYPE_AddQuadSorted*) 0x5339A4; // function pointer
TYPE_phd_PutPolygons phd_PutPolygons = (TYPE_phd_PutPolygons) 0x47BB30;