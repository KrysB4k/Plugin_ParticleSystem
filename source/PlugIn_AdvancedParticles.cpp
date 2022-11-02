// ************  Top/Header section ************
#include "stdafx.h"
#include <string.h>
#include <stdio.h>


#include "includes.h"


#pragma warning( error : 4706 )
#pragma warning( disable : 4996)

// ************  Early function declarations ************

void DrawParts();
void UpdateParts();

// ************  Global Variables Section *************

HINSTANCE MyDllInstance=NULL;  // the instance handle of your dll

extern char BufferLog[4096]; // temporary global buffer to host error and warning messages

DWORD MyTomb4PatcherAddress = 0x6708E0;
								
char TexMyPluginName[80];  

StrMyData MyData;

int next_part;
Particle parts[MAX_PARTS];

ParticleGroup partgroups[MAX_PARTGROUPS];
/* ParticleGroup elements should be initializable from Lua
(e.g. at level start) but not modifiable during level runtime */


unsigned long effect_timer;


// ************  Utilities section  ****************

#define phd_PopMatrix() (phd_mxptr -= 12)


// ************  Patcher Functions section  ***************

int Patch_UpdateParts(void)
{
	static BYTE VetBytes[]={0x66, 0xB8, 0x0, 0x0, 0xFF, 0x15, 0xE0, 0x8, 0x67, 0x0};

	return ApplyCodePatch(0x4491C1, VetBytes, 10);
}


int Patch_DrawParts(void)
{
	static BYTE VetBytes[]={0x66, 0xB8, 0x1, 0x0, 0xFF, 0x15, 0xE0, 0x8, 0x67, 0x0};

	return ApplyCodePatch(0x44E85D, VetBytes, 10);
}


bool CreateMyCodePatches(void)
{
	if (SetReservedDataZone(0x6708E0, 128) != APPC_OK) return false;

	SET_PATCH(Patch_UpdateParts);
	SET_PATCH(Patch_DrawParts);

	return true;
}

// ************  Assembly Procedures section  ******************


BEGIN_ASM_PROC(Patch_00)
	CALL UpdateBeetles
	CALL UpdateFish
	CALL UpdateParts
	RETN
END_ASM_PROC


BEGIN_ASM_PROC(Patch_01)
	CALL DrawRopeList
	CALL S_DrawSparks
	CALL DrawParts
	RETN
END_ASM_PROC


void *SubPatchArray[] = {
// TYPE_HERE your asm procedure names to call from tomb4 code
	&Patch_00,
	&Patch_01,
	NULL
};



BEGIN_ASM_PROC(MainPatcher)
	and eax, 0ffffh
	mov eax, dword ptr [SubPatchArray+eax*4];
	jmp eax
END_ASM_PROC



// ************  Particle associated functions - accessible in Lua, unless stated otherwise ************

int GetFreeParticle()
{
	int i, free;
	Particle* part;
	
	for (part = &parts[next_part], free = next_part, i = 0; i < MAX_PARTS; ++i)
	{
		if (!part->lifeCounter)
		{
			next_part = (free + 1) % MAX_PARTS;

			ClearMemory(&parts[free], sizeof(Particle));
			parts[free].emitterIndex = -1;
			parts[free].emitterNode = -1;
			return free;
		}

		if (free == MAX_PARTS-1)
		{
			part = &parts[0];
			free = 0;
		}
		else
		{
			part++;
			free++;
		}
	}
	
	int eldest = 0x7FFFFFFF;
	free = 0;
	part = &parts[0];

	for (i = 0; i < MAX_PARTS; ++i, ++part)
	{
		if (part->lifeCounter < eldest)
		{
			free = i;
			eldest = part->lifeCounter;
		}
	}

	next_part = (free + 1) % MAX_PARTS;

	ClearMemory(&parts[free], sizeof(Particle));
	parts[free].emitterIndex = -1;
	parts[free].emitterNode = -1;
	return free;
}


// defining custom particle update functions as below should be possible

void MyParticleUpdate(Particle *part)
{
	//e.g some parametric curve

	float t = part->Parameter();

	part->vel.x = sin(9 * t * float(M_PI)) * 64;
	part->vel.y = cos(12 * t * float(M_PI)) * 64;
	part->vel.z = -20 + sin(18 * t * float(M_PI)) * 48;

	part->colCust = HSLtoRGB(2 * t * 360, 1.0f, 0.5f);
}


void PhysicsParticles(Particle* part)
{
	part->ParticleCollideFloors(0.75f, 0.75f, false);
}


void SensorParticleUpdate(Particle* part)
{
	part->ParticleAnimate(35, 49, 1);
}


// there will need to be some Lua interface to assign update functions to ParticleGroups
void UpdateParticle(Particle* part, int index)
{
	if (!part)
		return;

	// a switch case for testing purposes, but perhaps passing actual functions as argument will be better
	switch (index)
	{
	case 1:
		MyParticleUpdate(part);
		break;
	
	case 2:
		PhysicsParticles(part);
		break;

	case 3:
		break;

	default:
		break;
	}
}


// Update and Draw procedures - should not be available from Lua, used only by the plugin

void UpdateParts()
{
	Particle* part = &parts[0];

	for (int i = 0; i < MAX_PARTS; ++i, ++part)
	{
		if (part->lifeCounter <= 0)
			continue;

		auto pdata = &partgroups[part->groupIndex];

		int fadetime = part->lifeSpan;
		int lifefactor = (part->lifeSpan - part->lifeCounter);

		if (part->colorFadeTime)
		{
			if (part->colorFadeTime < 0 && part->lifeSpan > (-part->colorFadeTime))
			{
				fadetime = -part->colorFadeTime;
				lifefactor = -(part->lifeCounter + part->colorFadeTime);
			}
			else if (part->lifeSpan > part->colorFadeTime)
				fadetime = part->colorFadeTime;
		}

		float t = lifefactor/float(fadetime);
		if (t < 0.0f)
			t = 0.0f;
		if (t > 1.0f)
			t = 1.0f;

		part->colCust.R = Round(Lerp(float(part->colStart.R), float(part->colEnd.R), t));
		part->colCust.G = Round(Lerp(float(part->colStart.G), float(part->colEnd.G), t));
		part->colCust.B = Round(Lerp(float(part->colStart.B), float(part->colEnd.B), t));

		if (pdata)
			UpdateParticle(part, pdata->updateIndex);
		
		part->vel += part->accel;
		part->pos += part->vel;
		part->rot += part->angVel;

		--part->lifeCounter;
	}

	++effect_timer;
}


void DrawParts()
{
	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.xPos, lara_item->pos.yPos, lara_item->pos.zPos);

	long* mptr = phd_mxptr;

	Particle* part = &parts[0];

	int x1 = 0, y1 = 0, z1 = 0;
	for (int i = 0; i < MAX_PARTS; ++i, ++part)
	{
		if (part->lifeCounter <= 0)
			continue;

		x1 = Round(part->pos.x);
		y1 = Round(part->pos.y);
		z1 = Round(part->pos.z);

		auto pdata = &partgroups[part->groupIndex];

		// item and/or mesh attachment of particle
		if (part->emitterIndex >= 0)
		{
			int index = part->emitterIndex;
			if (index > Trng.pGlobTomb4->pAdr->TotItemsMax)
				index = *Trng.pGlobTomb4->pAdr->pLaraIndex;

			auto item = &items[index];

			if (part->emitterNode >= 0) // if attached to specific mesh node of item
			{
				phd_vector posNode(0, 0, 0);
				
				if (pdata)
				{
					posNode.x = pdata->attach.offX;
					posNode.y = pdata->attach.offY;
					posNode.z = pdata->attach.offZ;
				}

				int node = part->emitterNode & 0x1F;

				auto jointPos = GetJointPos(item, node, posNode.x, posNode.y, posNode.z);

				x1 += Round(jointPos.x);
				y1 += Round(jointPos.y);
				z1 += Round(jointPos.z);
			}
			else // no mesh node, use item's pos
			{
				x1 += item->pos.xPos;
				y1 += item->pos.yPos;
				z1 += item->pos.zPos;
			}

			int cutoff = -1;
			// particle attachment cutoff 
			if (pdata && pdata->attach.cutoff > 0)
			{
				cutoff = pdata->attach.cutoff;
				if (pdata->attach.random> 1)
					cutoff += (GetRandomDraw() % pdata->attach.random);
			}

			if ((part->lifeSpan - part->lifeCounter) > cutoff)
			{
				part->pos.x = x1;
				part->pos.y = y1;
				part->pos.z = z1;
				part->emitterIndex = -1;
				part->emitterNode = -1;
			}
		}
		else if (part->emitterNode >= 0) // if EmitterNode >= 0 when EmitterIndex < 0, use Lara meshes
		{
			phd_vector posNode(0, 0, 0);
			int node = (part->emitterNode > 14) ? 14 : part->emitterNode;
			
			if (pdata)
			{
				posNode.x = pdata->attach.offX;
				posNode.y = pdata->attach.offY;
				posNode.z = pdata->attach.offZ;
			}

			GetLaraJointPos(&posNode, node);

			x1 += posNode.x;
			y1 += posNode.y;
			z1 += posNode.z;

			int cutoff = -1;
			// particle attachment cutoff 
			if (pdata && pdata->attach.cutoff > 0)
			{
				cutoff = pdata->attach.cutoff;
				if (pdata->attach.random > 1)
					cutoff += (GetRandomDraw() % pdata->attach.random);
			}

			if ((part->lifeSpan - part->lifeCounter) > cutoff)
			{
				part->pos.x = x1;
				part->pos.y = y1;
				part->pos.z = z1;
				part->emitterIndex = -1;
				part->emitterNode = -1;
			}
		}

		x1 -= lara_item->pos.xPos;
		y1 -= lara_item->pos.yPos;
		z1 -= lara_item->pos.zPos;

		if (x1 < -20480 || x1 > 20480 ||
			y1 < -20480 || y1 > 20480 ||
			z1 < -20480 || z1 > 20480)
		{
			part->lifeCounter = 0;
			continue;
		}


		// convert from world coordinates to screen coordinates
		long result[3] = {0, 0, 0};
		long TempMesh[3] = {0, 0, 0};
		long viewCoords[6] = {0,0,0,0,0,0};

		TempMesh[0] = x1;
		TempMesh[1] = y1;
		TempMesh[2] = z1;

		result[0] = (mptr[M00] * TempMesh[0] + mptr[M01] * TempMesh[1] + mptr[M02] * TempMesh[2] + mptr[M03]);
		result[1] = (mptr[M10] * TempMesh[0] + mptr[M11] * TempMesh[1] + mptr[M12] * TempMesh[2] + mptr[M13]);
		result[2] = (mptr[M20] * TempMesh[0] + mptr[M21] * TempMesh[1] + mptr[M22] * TempMesh[2] + mptr[M23]);

		float zv = f_persp / float(result[2]);
		viewCoords[0] = Round(result[0] * zv + f_centerx);
		viewCoords[1] = Round(result[1] * zv + f_centery);
		viewCoords[2] = result[2] >> 14;

		// if particle is a line do world to screen transform for second vertex
		if (!pdata || pdata->spriteSlot <= 0)
		{
			float size = Lerp(part->destSize, part->startSize, part->lifeCounter/float(part->lifeSpan));
			auto vel = part->vel;

			if (pdata && pdata->LineIgnoreVel)
				vel = vel.normalized(); // ignore speed contribution to particle's size
			else
				size *= (1.0f/32.0f); // else scale down size

			vel *= size;

			TempMesh[0] = Round(x1 - vel.x);
			TempMesh[1] = Round(y1 - vel.y);
			TempMesh[2] = Round(z1 - vel.z);

			result[0] = (mptr[M00] * TempMesh[0] + mptr[M01] * TempMesh[1] + mptr[M02] * TempMesh[2] + mptr[M03]);
			result[1] = (mptr[M10] * TempMesh[0] + mptr[M11] * TempMesh[1] + mptr[M12] * TempMesh[2] + mptr[M13]);
			result[2] = (mptr[M20] * TempMesh[0] + mptr[M21] * TempMesh[1] + mptr[M22] * TempMesh[2] + mptr[M23]);

			zv = f_persp / float(result[2]);
			viewCoords[3] = Round(result[0] * zv + f_centerx);
			viewCoords[4] = Round(result[1] * zv + f_centery);
			viewCoords[5] = result[2] >> 14;
		}


		long minSize = 4;

		// draw the particle to the given screen coordinates
		part->DrawParticle(pdata, viewCoords, minSize);
	}

	phd_PopMatrix();
}


// it should be possible to write in Lua a particle initialization function, as in examples below


// method 1 - passing absolute x, y, z and room coordinates directly
void LineParticle(int x, int y, int z, short room)
{
	int index = GetFreeParticle();
	Particle *part = &parts[index];

	// we add the absolute coordinates
	int xrand = x + (GetRandomControl()&127) - 64;
	int yrand = y + (GetRandomControl()&127) - 64;
	int zrand = z + (GetRandomControl()&127) - 64;

	part->roomIndex = room;

	part->pos = Vector3f(xrand, yrand, zrand);

	part->lifeCounter = part->lifeSpan = 240;
	
	part->fadeIn = 15;
	part->fadeOut = 15;
	
	part->rot = part->angVel = 0;

	part->colStart = part->colEnd = ColorRGB();
	
	part->startSize = part->destSize = 32 + (GetRandomControl()&15);

	part->groupIndex = 0;
}


// method 2 - passing item index and using the de-attachment feature to spawn it at the item's location
void SpriteParticle(short itemNum)
{
	if (GetRandomControl()&3)
		return;

	int index = GetFreeParticle();
	Particle *part = &parts[index];

	// we don't need absolute coordinates here - they will be calculated when de-attaching the particle
	int xrand = (GetRandomControl()&255) - 128;
	int yrand = (GetRandomControl()&255) - 128;
	int zrand = (GetRandomControl()&255) - 128;

	part->roomIndex = items[itemNum].room_number; // get the item's room number

	part->pos = Vector3f(xrand, yrand, zrand);

	part->vel.x = (GetRandomControl()&63) - 32;
	part->vel.y = (GetRandomControl()&31) - 160;
	part->vel.z = (GetRandomControl()&63) - 32;

	part->accel = Vector3f(0, 5.0f, 0);

	part->lifeCounter = part->lifeSpan = 112 + (GetRandomControl()&15);
	part->colorFadeTime = part->lifeSpan/2;
	
	part->fadeIn = 15;
	part->fadeOut = 15;
	
	part->spriteIndex = 14;
	part->rot = part->angVel = 0;

	part->colStart = ColorRGB(255, 192 + (GetRandomControl()&63), 0);
	part->colEnd = ColorRGB(192 + (GetRandomControl()&63), (GetRandomControl()&31), 0);

	part->startSize = 32 + (GetRandomControl()&31);
	part->destSize = 48 + (GetRandomControl()&31);

	// we give the emitter index - the particle then gets de-attached from the emitter in the draw function
	part->emitterIndex = itemNum;
	part->groupIndex = 1;
}


// method 3 - spawn particle at position of Lara's mesh
// no arguments needed, if the particle won't require any external input (item number, room number, etc)
void LaraHeadParticle()
{
	int index = GetFreeParticle();
	Particle *part = &parts[index];

	// as in method 2, absolute coordinates are unnecessary
	int xrand = (GetRandomControl()&127) - 64;
	int yrand = 0;
	int zrand = (GetRandomControl()&127) - 64;

	part->pos = Vector3f(xrand, yrand, zrand);

	part->vel.x = (GetRandomControl()&15) - 8;
	part->vel.y = (GetRandomControl()&7) - 32;
	part->vel.z = (GetRandomControl()&15)- 8;

	part->accel.y = 1.5f;

	part->lifeCounter = part->lifeSpan = 40 + (GetRandomControl()&15);
	
	part->fadeIn = 10;
	part->fadeOut = 10;
	
	part->spriteIndex = 11;
	part->rot = part->angVel = 0;

	part->colStart.R = part->colEnd.R = 0;

	part->colStart.B = 192 + (GetRandomControl()&63);
	part->colStart.G = 192 + (GetRandomControl()&63);

	if (GetRandomControl()&1)
	{
		part->colEnd.G = 128 + (GetRandomControl()&63);
		part->colEnd.B = 0;
	}
	else
	{
		part->colEnd.B = 128 + (GetRandomControl()&63);
		part->colEnd.G = 0;
	}
	
	part->startSize = 16 + (GetRandomControl()&15);
	part->destSize = part->startSize * 2;

	part->emitterNode = 8; // spawn from Lara's head mesh
	// if no emitterIndex is set, the plugin will implicitly use Lara's meshes

	part->groupIndex = 2;
}


// one way of spawning particles is through an emitter nullmesh object
void ParticleEmitterControl(short item_number)
{
	auto item = &items[item_number];

	if (TriggerActive((StrItemTr4*)item))
	{
		// based on OCB value, as with FLEP smoke emitter OCBs ? (to discuss)
		switch (item->trigger_flags)
		{
		case 0:
			LineParticle(item->pos.xPos, item->pos.yPos, item->pos.zPos, item->room_number);
			break;
		case 1:
			SpriteParticle(item_number);
			break;
		case 2:
			LaraHeadParticle();
			break;
		default:
			break;
		}
	}
}

// ************  CallBack functions section  *****************

// TYPE_HERE: your callback function

void cbInitProgram(int NumberLoadedPlugins, char *VetPluginNames[]) 
{

	// save number of plugins (enclosed yours and the tomb_nextgeneration.dll ) and their names
	// these data will be used to locate IDs of any loaded plugins using FindPluginID() function
	Trng.TotPlugins = NumberLoadedPlugins;
	Trng.pVetPluginNames = VetPluginNames;

	// clear all my global variables
	ClearMemory(&MyData, sizeof(StrMyData));
}


void cbInitGame(void)
{
	// here you can initialize your global data for whole adventure
	// this procedure will be called only once, before loading title level

}


void cbInitLevel(int LevelNow, int LevelOld, DWORD FIL_Flags)
{
	// here you can initialize specific items of currnet level.
	// it will be called only once for level, when all items has been already initialized
	// and just a moment before entering in main game cycle.
	ClearMemory(parts, sizeof(Particle) * MAX_PARTS);
	next_part = 0;
	effect_timer = 0;


	// initialize some ParticleGroups

	partgroups[0].blendingMode = POLY_COLORADD;
	partgroups[0].updateIndex = 1;

	partgroups[1].spriteSlot = SLOT_DEFAULT_SPRITES;
	partgroups[1].blendingMode = POLY_COLORADD;
	partgroups[1].updateIndex = 2;
	partgroups[1].attach.cutoff = 0; // this will de-attach the particle from the emitter immediately after spawning

	partgroups[2].spriteSlot = SLOT_DEFAULT_SPRITES;
	partgroups[2].blendingMode = POLY_COLORADD;
	partgroups[2].attach.offY = -80;

	partgroups[3].spriteSlot = SLOT_DEFAULT_SPRITES;
	partgroups[3].blendingMode = POLY_COLORADD;
	partgroups[3].updateIndex = 3;
}

// called everytime player save the game (but also when lara move from a level to another HUB saving). 
// in this procedure your plugin will save its own data in savegame
// SavingType inform about the kind of saving it is happening (SAVT_... values)
// pAdrZone is a byte pointer where this procedure will save the address from where begin data to save in savegame
// this function will return the size of data to save
// note: It's better you don't change anything of this code. It will save byself of data you stored in MyData.Save
// structure. If you wish adding other variables to save, just you add new fields in Structures of MyData.Save 
DWORD cbSaveMyData(BYTE **pAdrZone, int SavingType)
{
	DWORD SizeData;
	int i;
	static WORD *pVetExtras;
	int TotNWords;
	int TotNewActions;


	if (SavingType & SAVT_COMPLETED) {
		// this call is not to save data but only it is a confirm that the previous saving has been completed
		// now we can free the temporary memory used to save the data in previous call
		if (pVetExtras != NULL) {
			FreeMemory(pVetExtras);
			pVetExtras=NULL;
		}

		return 0;
	}


	TotNWords=0;
	pVetExtras = (WORD *) GetMemory(16);
	// save id of my plugin in first word

	pVetExtras[TotNWords++] = Trng.IdMyPlugin;

	if (SavingType & SAVT_LOCAL_DATA) {
		// save local data

		// save Local structure
		AddNGToken(NGTAG_LOCAL_DATA, NO_ARRAY, sizeof(StrSavegameLocalData), &MyData.Save.Local, 
						&pVetExtras, &TotNWords);


		// save all (currently enabled) progressive actions
		// before saving, compact progressive action array to remove intermediate free records
		TotNewActions=0;

		for (i=0;i<MyData.TotProgrActions;i++) {
			if (MyData.VetProgrActions[i].ActionType != AXN_FREE) {

				MyData.VetProgrActions[TotNewActions] = MyData.VetProgrActions[i];

				TotNewActions++;
			}
		}
		// update new valuese after recompatting
		MyData.LastProgrActionIndex =0;
		MyData.TotProgrActions= TotNewActions;

		// store all progressive action records
		AddNGToken(NGTAG_PROGRESSIVE_ACTIONS, MyData.TotProgrActions, sizeof(StrProgressiveAction), 
				&MyData.VetProgrActions[0], &pVetExtras, &TotNWords);



	}

	if (SavingType & SAVT_GLOBAL_DATA) {
		// save global data
		AddNGToken(NGTAG_GLOBAL_DATA, NO_ARRAY, sizeof(StrSavegameGlobalData), &MyData.Save.Global , 
						&pVetExtras, &TotNWords);
	}
	// write final sequence
	AddTokenFinalSequence(&pVetExtras, &TotNWords);

	// return to trng the infos about start of memory where there are our data and their size:
	*pAdrZone = (BYTE *) pVetExtras;
	SizeData = TotNWords * 2;

	return SizeData;
	

}
// called when a savegame will be loaded (but also when lara move from a level to another)
// pAdrZone will point to memory zone with data just loaded from savegame
// SizeData is the size of data pointed by pAdrZone
// note: it's better you don't change anything of this code. It's alread thought to reload all data you saved in MyData.Save 
// structure. There is no need of changes by you
void cbLoadMyData(BYTE *pAdrZone, DWORD SizeData)
{

	WORD *pVetExtras;
	StrParseNGField  ParseField;
	int Indice;
	int i;
	WORD TotActions;

	
	pVetExtras = (WORD*) pAdrZone;

	Indice=0;

	while (ParseNgField(pVetExtras ,Indice, &ParseField)==true) {
		
		// recover different ng token
		switch (ParseField.Type) {
		case NGTAG_LOCAL_DATA:
			// local data
			memcpy(&MyData.Save.Local, ParseField.pData, sizeof(StrSavegameLocalData));
			break;

		case NGTAG_GLOBAL_DATA:
			// global data
			memcpy(&MyData.Save.Global, ParseField.pData, sizeof(StrSavegameGlobalData));
			break;
			
		case NGTAG_PROGRESSIVE_ACTIONS:
			// progressive actions
			i= ParseField.StartDataIndex;
			// read tot actions value
			TotActions = pVetExtras[i++];
			// copy all tot records
			memcpy(&MyData.VetProgrActions[0], &pVetExtras[i], sizeof(StrProgressiveAction) * TotActions);
			MyData.TotProgrActions = TotActions;
			break;
		}
		Indice= ParseField.NextIndex; 
	}

}
// free memory used to store all data about your customize commands loaded in previous level
void FreeMemoryCustomize(void)
{
	int i;

	for (i=0;i<MyData.BaseCustomizeMine.TotCustomize;i++) {
		FreeMemory(MyData.BaseCustomizeMine.pVetCustomize[i].pVetArg);
	}

	if (MyData.BaseCustomizeMine.TotCustomize > 0) {
		FreeMemory(MyData.BaseCustomizeMine.pVetCustomize);
		MyData.BaseCustomizeMine.TotCustomize=0;
	}


	MyData.BaseCustomizeMine.pVetCustomize=NULL;
}

// free memory used to store all data about your parameters commands loaded in previous level
void FreeMemoryParameters(void)
{
	int i;

	for (i=0;i<MyData.BaseParametersMine.TotParameters;i++) {
		FreeMemory(MyData.BaseParametersMine.pVetParameters[i].pVetArg);
	}

	if (MyData.BaseParametersMine.TotParameters > 0) {
		FreeMemory(MyData.BaseParametersMine.pVetParameters);
		MyData.BaseParametersMine.TotParameters=0;
	}

	MyData.BaseParametersMine.pVetParameters=NULL;
}

// this procedure will be called at end of any level
// you can type here code to free resources allocated for level (that quits now)
void FreeLevelResources(void)
{

	// free memory used to store all data about your customize commands loaded in previous level
	FreeMemoryCustomize();
	// free memory used to store all data about your parameters commands loaded in previous level
	FreeMemoryParameters();
	MyData.BaseAssignSlotMine.TotAssign=0;

}
// it will be called before beginning the loading for a new level.
// you can type here code to initialise all variables used for level (to clear old values changed by previous level)
// and to free resources allocated in old level since now we'are going to another new level.

void cbInitLoadNewLevel(void)
{
	int i;

	StrProgressiveAction *pAction;

	// clear all LOCAL variables
	ClearMemory(&MyData.Save.Local,sizeof(StrSavegameLocalData));

	// clear progressive actions
	pAction= &MyData.VetProgrActions[0];

	for (i=0;i<MyData.TotProgrActions;i++) {
		if (pAction->ActionType != AXN_FREE) {
			// here you could analise to free resoruce allocated from this specific action

			pAction->ActionType = AXN_FREE;
		}
	}

	MyData.TotProgrActions=0;
	MyData.LastProgrActionIndex=0;

	// here you can initialise other variables of MyData different than Local and progressive actions
	// free resources allocate in previous level
	FreeLevelResources();
}



// this procedure will be called everytime a flipeffect of yours will be engaged
// you have to elaborate it and then return a TRET_.. value (most common is TRET_PERFORM_ONCE_AND_GO)
int cbFlipEffectMine(WORD FlipIndex, WORD Timer, WORD Extra, WORD ActivationMode)
{
	int RetValue;
	WORD TimerFull;

	RetValue = enumTRET.PERFORM_ONCE_AND_GO;
	// if the flip has no Extra paremeter you can handle a Timer value with values upto 32767
	// in this case you'll use the following TimerFull variable, where (with following code) we set a unique big number 
	// pasting togheter the timer+extra arguments:
	TimerFull = Timer | (Extra << 8);

	switch (FlipIndex) {
		// here type the "case Number:" for each flipeffect number. At end of the code you'll use the "break;" instruction to signal the code ending
		// Note: when you'll add your first "case Number:" then you can remove the following "case -1: and break;" instructions
	case -1: 
		break;
	default:
		SendToLog("WARNING: Flipeffect trigger number %d has not been handled in cbFlipEffectMine() function", FlipIndex);
		break;
	}

	// if there was the one-shot button enabled, return TRET_PERFORM_NEVER_MORE
	if (ActivationMode & enumSCANF.BUTTON_ONE_SHOT) RetValue= enumTRET.PERFORM_NEVER_MORE; 
	return RetValue;
}
// this procedure will be called everytime an action trigger of yours will be engaged 
// you have to elaborate it and then return a TRET_.. value (most common is TRET_PERFORM_ONCE_AND_GO)
int cbActionMine(WORD ActionIndex, int ItemIndex, WORD Extra, WORD ActivationMode)
{
	int RetValue;
	
	RetValue=TRET_PERFORM_ONCE_AND_GO;

	switch (ActionIndex) {
		// type here the code per your action trigger.
		// add "case Number:" and complete the code with "break;" instruction
	case -1:
		// note: remove this "case -1:" and its "break;" it has been added only to avoid warning messages about empty switch
		break;
	default:
		SendToLog("WARNING: action trigger number %d has not been handled in cbActionMine() function", ActionIndex);
		break;
	}
	
	// if there was the one-shot button enabled, return TRET_PERFORM_NEVER_MORE
	if (ActivationMode & enumSCANF.BUTTON_ONE_SHOT) RetValue= enumTRET.PERFORM_NEVER_MORE;
	return RetValue;


}

// this procedure will be called everytime a conditional trigger of yours will be engaged
// you have to elaborate it and then return a CTRET_.. value (most common is CTRET_ONLY_ONCE_ON_TRUE)
int cbConditionMine(WORD ConditionIndex, int ItemIndex, WORD Extra, WORD ActivationMode)
{
	int RetValue;
	
	RetValue=CTRET_ONLY_ONCE_ON_TRUE;

	switch (ConditionIndex){
		// type here the code for your condition trigger, inserting the code in the section
		// beginning with "case NumberOfAction:" and ending with row "break;"
	case -1:
		// note: remove this "case -1:" and its "break;" it has been added only to avoid warning messages about empty switch
		break;
	default:
		SendToLog("WARNING: condition trigger number %d has not been handled in cbConditionMine() function", ConditionIndex);
		break;


	}
	return RetValue;
	  
}

// this procedure vill be called for each Customize=CUST_... command read from script
// having one of yours CUST_ constant
// CustomizeValue will be the value of your CUST_ constant
// NumberOfItems will be the number of following Item (signed 16 bit values) following
// the CUST_ constant in the customize= script command
// pItemArray is the array with all NumberOfItems arguments of customize command
void cbCustomizeMine(WORD CustomizeValue, int NumberOfItems, short *pItemArray)
{
	// here you can replace this default management of anonymous customize commands
	// with your procedure where you can recognize each different CUST_ value and 
	// save its arguments in meaningful names fields, or elaboriting them immediatly
	// when it is possible (warning: in this moment nothing of level it has been yet loaded, excepting the script section)

	// ----- default management (optional)----
	// all customize values will be saved in MyData structure
	DWORD SizeMem;
	StrGenericCustomize *pMyCust;
	int TotCust;

	// ask memory to have another (new) record of StrGenericCustomize structure
	TotCust= MyData.BaseCustomizeMine.TotCustomize;
	TotCust++;
	SizeMem = TotCust * sizeof(StrGenericCustomize);
	MyData.BaseCustomizeMine.pVetCustomize = 
				(StrGenericCustomize *) ResizeMemory(MyData.BaseCustomizeMine.pVetCustomize, SizeMem);

	pMyCust = & MyData.BaseCustomizeMine.pVetCustomize[TotCust-1];
	
	// now require memory for all arguments (NumberOfItems) store in pItemArray

	pMyCust->pVetArg = (short *) GetMemory(2 * NumberOfItems);
	// copy data
	pMyCust->NArguments = NumberOfItems;
	memcpy(pMyCust->pVetArg, pItemArray, 2*NumberOfItems);
	pMyCust->CustValue = CustomizeValue;

	MyData.BaseCustomizeMine.TotCustomize= TotCust;
	// ---- end of default managemnt for generic customize -------------	
}
// callback called everytime in current level section of the script it has been found an AssignSlot command
// with one of your OBJ_ constants
void cbAssignSlotMine(WORD Slot, WORD ObjType)
{
	int i;

	i = MyData.BaseAssignSlotMine.TotAssign;

	if (i >= MAX_ASSIGN_SLOT_MINE) {
		SendToLog("ERROR: too many AssignSlot= commands for current plugin");
		return;
	}

	MyData.BaseAssignSlotMine.VetAssignSlot[i].MioSlot = Slot;
	MyData.BaseAssignSlotMine.VetAssignSlot[i].TipoSlot = ObjType;
	MyData.BaseAssignSlotMine.TotAssign++;

}
// this procedure vill be called for each Parameters=PARAM_... command read from script
// having one of yours PARAM_ constants
// ParameterValue will be the value of your PARAM_ constant
// NumberOfItems will be the number of following Item (signed 16 bit values) following
// the PARAM_ constant in the customize= script command
// pItemArray is the array with all NumberOfItems arguments of Parameter command
void cbParametersMine(WORD ParameterValue, int NumberOfItems, short *pItemArray)
{
	// here you can replace this default management of anonymous parameters commands
	// with your procedure where you can recognize each different Param_ value and 
	// save its arguments in meaningful names fields, or elaboriting them immediatly
	// when it is possible (warning: in this moment nothing of level it has been yet loaded, excepting the script section)

	// ----- default management (optional)----
	// all parameters values will be saved in MyData structure
	DWORD SizeMem;
	StrGenericParameters *pMyParam;
	int TotParam;

	// ask memory to have another (new) record of StrGenericparameters structure
	TotParam= MyData.BaseParametersMine.TotParameters;
	TotParam++;
	SizeMem = TotParam * sizeof(StrGenericParameters);
	MyData.BaseParametersMine.pVetParameters = 
		(StrGenericParameters *) ResizeMemory(MyData.BaseParametersMine.pVetParameters, SizeMem);

	pMyParam = & MyData.BaseParametersMine.pVetParameters[TotParam-1];

	pMyParam->ParamValue = ParameterValue;
	// now require memory for all arguments (NumberOfItems) store in pItemArray
	pMyParam->pVetArg = (short *) GetMemory(2 * NumberOfItems);
	// copy data
	pMyParam->NArguments = NumberOfItems;
	memcpy(pMyParam->pVetArg, pItemArray, 2*NumberOfItems);

	MyData.BaseParametersMine.TotParameters= TotParam;
	// ---- end of default managemnt for generic parameters -------------


}

// this procedure will be called every game cycle (at begin of cycle)
void cbCycleBegin(void)
{

}

// Not yet linked! To link it add to RequireMyCallBacks() function the row:
//  	GET_CALLBACK(CB_CYCLE_END, 0, 0, cbCycleEnd);
// this procedure will be called everygame cycle, at end.
// you have to return a RET_CYCLE_ value
int cbCycleEnd(void)
{


	return RET_CYCLE_CONTINUE;	
}

// this function will be called for each your (common) progressive action to be peformed
void PerformMyProgrAction(StrProgressiveAction *pAction)
{


	switch (pAction->ActionType) {
// replace the "case -1:" with your first "case AXN_...:" progressive action to manage)		
	case -1:
		break;

	}

}
// callback called from trng for each frame in game cycle to perform your (common) progressive action
void cbProgrActionMine(void)
{
	int i;
	StrProgressiveAction *pAction;

	pAction = &MyData.VetProgrActions[0];
	for (i=0;i<MyData.TotProgrActions;i++) {
		if (pAction->ActionType != AXN_FREE) {
			PerformMyProgrAction(pAction);
		}
		pAction++;
	}


}


// inside this function you'll type call to functions to intialise your new objects or customize that olds.
// this callback will be called at start of loading new level and a bit after having started to load level data
void cbInitObjects(void) 
{
	auto obj = &objects[SLOT_UW_PROPULSOR];
	obj->ceiling = nullptr;
	obj->floor = nullptr;
	obj->draw_routine = nullptr;
	obj->collision = nullptr;
	obj->initialise = nullptr;
	obj->control = ParticleEmitterControl;
}


// FOR_YOU:
// in this function RequireMyCallBacks() you'll type
// a list of:
//		GET_CALLBACK(CB_..., ,)
// one for each callback you need
bool RequireMyCallBacks(void)
{
// ************  RequireMyCallBacks() function  *****************
	// protype of GET_CALLBACK:
	// GET_CALLBACK(CallBackCB, CBT_Flags, Index, MyProcToCall)
	
	GET_CALLBACK(CB_INIT_PROGRAM, 0, 0, cbInitProgram)
	GET_CALLBACK(CB_INIT_LEVEL, 0,0, cbInitLevel)
	GET_CALLBACK(CB_INIT_LOAD_NEW_LEVEL, 0,0, cbInitLoadNewLevel)
	GET_CALLBACK(CB_SAVING_GAME, 0, 0, cbSaveMyData)
	GET_CALLBACK(CB_LOADING_GAME, 0, 0, cbLoadMyData)
	GET_CALLBACK(CB_ASSIGN_SLOT_MINE, 0,0, cbAssignSlotMine)
	GET_CALLBACK(CB_INIT_OBJECTS, 0, 0, cbInitObjects)
	
	/* unused callbacks which you may uncomment for personal use */

	//GET_CALLBACK(CB_CUSTOMIZE_MINE, 0,0, cbCustomizeMine)
	//GET_CALLBACK(CB_INIT_GAME, 0, 0, cbInitGame)
	//GET_CALLBACK(CB_FLIPEFFECT_MINE, 0, 0, cbFlipEffectMine)
	//GET_CALLBACK(CB_ACTION_MINE, 0,0, cbActionMine)
	//GET_CALLBACK(CB_CONDITION_MINE,0,0,cbConditionMine)
	//GET_CALLBACK(CB_PARAMETER_MINE, 0, 0, cbParametersMine)
	//GET_CALLBACK(CB_CYCLE_BEGIN, 0, 0, cbCycleBegin)
	//GET_CALLBACK(CB_PROGR_ACTION_MINE, 0, 0, cbProgrActionMine)
	
	return true;
}
// FOR_YOU:
// This function will be the first code to be executed of your plugin
// It happens when trng will load your plugin.
// In this moment no directX graphic is yet active, so you can show
// message boxes (TryMessageBox) to advise your players or level designer 
// about something
// Note: if you wish abort all (because there is an error or something
// is missing) you have to exit from this function returning: false
bool InitializeAll(void)
{
// ************  InitializeAll() function  ****************
	//  perform all your patches
	CALL_CHECK(CreateMyCodePatches)

	// call the function that requires all callback you need
	CALL_CHECK(RequireMyCallBacks)

	// TYPE_HERE: code to allocate global resource to use in the whole game

	return true;
}

// FOR_YOU: Tyis function will be called when tomb4 game is to be closed.
// you should type in this function further codes to free the global
// resource you had allocated in the InitializeAll() function 
void ReleaseAll(void)
{
// ************  ReleaseAll() function  ******************
	FreeLevelResources();
}


BOOL APIENTRY DllMain( HINSTANCE hInstanceDll, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved)
{

    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			MyDllInstance = hInstanceDll;
			GetTrngInfo();
			// control per check control value about size and alignment with globtomb4 structure
			if (CheckControlGlobTomb4() == false) return FALSE;

			if  (InitializeAll()==false) {
				return FALSE;
			}
			return TRUE;
			

		case DLL_PROCESS_DETACH:
			ReleaseAll();
			break;
    }
    return TRUE;
}


