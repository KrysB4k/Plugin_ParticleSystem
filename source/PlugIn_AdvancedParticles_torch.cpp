// FOR_YOU:
// This is the main source of your plugin.
// It's in this source you'll type all your assembly or c++ code and all 
// the variables and memory zones you need.

// ************  Top/Header section ************
#include "stdafx.h"
#include <string.h>
#include <stdio.h>
#include "bass.h"		// prototypes for extra sound library: bass.dll
	// Following header files will be updated for every new version of 
	// the tomb_NextGeneration.dll, so it's better you don't change them
	//  because they will be replaced for any new update.

#include "Tomb_NextGeneration.h" // mnemonic constants defined in tomb_NextGeneration.dll
#include "structures.h" // structure of tomb4 program and trng dll
#include "DefTomb4Funct.h" // defines of tomb4 procedure prototypes
#include "functions.h"  // assigments of real tomb4 address to each tomb4 procedures
#include "macros.h"  // definition of macros

	// FOR_YOU:
	// While the followings are the headers you can use 
	// to type your structures, constants and new tomb4 procedures you 
	// discovered. 
	// Following files are only yours and trng will not ever change them:
#include "macros_mine.h"  // here you define your own macros
#include "constants_mine.h" // here you define your mnemonic constants
#include "structures_mine.h" // here you type your structure definitions
#include "Tomb4Discoveries_mine.h" // here type tomb4 procedures you discovered

#include "trng.h" // list of trng functions imported from trng.cpp source. 

#pragma warning( error : 4706 )
#pragma warning(disable: 4996)

// ************  Early function declarations ************
// these declarations are here due to the funcs being used inside ASM code
void DrawParts();
void UpdateParts();
int GetFreeParticle();

// ************  Global Variables Section *************

// FOR_YOU:
// here you type the variables or memory zones you wish use in different
// C++ procedures.
// If you use the assembly you'll have to type ALL your variables in this
// section.
// for example if you wish have a 32 bits signed variable to store big
// numbers you can type:
// TYPE_HERE:
// int MyNumber;
// and then you can use it in asm in this way:
//		mov  eax, [MyNumber]
// or
//      mov eax, dword ptr [MyNumber]
// same speech for wide memory zones.
// If you wish having a memory zone of 13000 bytes you can type:
//  BYTE MyZone[13000];
// and then you can use it in this way:
//     mov  al, MyNumber[ecx]  ; read the ECXth byte and copy it in AL
// or also:
//     mov al, [MyNumber+ecx] ; same result of above instruction

// Variables and memory zone to TYPE_HERE:

HINSTANCE MyDllInstance=NULL;  // the instance handle of your dll

extern char BufferLog[4096]; // temporary global buffer to host error and warning messages

// FOR_YOU: If you mean create code patch (changes in tomb4 code to call your code in the plugin dll,
// you MUST change the MyTomb4PatcherAddress value (here below) 
// You can discover a free tomb4 zone using TrngPatcher program, with the 
// menu command "Tools->Get Assignment of Free Memory range"
// Note: choose an address, in the given range, terminating with hex 
// digits: 0,4,8, or C
// because it's usefull that the address was a multiple by 4
// If you chose an address used from other plugins you'll get an error and
// the game will be aborted
// note: if you don't mean use code patches you can let 0x0 in following line
DWORD MyTomb4PatcherAddress = 0x6708E0; // <- TYPE_HERE: the new address you chose
								
// this text will contain your plugin name (omitting .dll extension).
// it will be used to give a title to messagebox for error messages or warnings
char TexMyPluginName[80];  
// in this MyData structure you can store all global variables for your plugin
// you have to define them in structures_mine.h source inside structure "StrMyData" or for variable
// to save and restore in savegames, in "StrSavegameGlobalData" or "StrSavegameLocalData" structures
StrMyData MyData;


int next_part;
Particle parts[MAX_PARTS];
ParticleData partdata[256];


FlameColorInfo flameColor[8];
FlameColorInfo torchColor[8];
FlameColorInfo laraTorch;
FlameColorInfo laraNormalBurn;
FlameColorInfo laraMagicBurn;


const FlameColorInfo standard = {
	0,		0,
	0xFF,	0x01,	//sR
	0x60,	0x20,	//sG
	0x30,	0x01,	//sB
	0xC0,	0x40,	//dR
	0x80,	0x40,	//dG
	0x20,	0x01,	//dB
	0xC0,	0x40,	//dynR
	0x60,	0x20,	//dynG
	0x00,	0x40	//dynB
//	base  rand
};

const FlameColorInfo magical = {	
	0,		0,
	0x30,	0x01,	//sR
	0xFF,	0x01,	//sG
	0x60,	0x20,	//sB
	0x20,	0x01,	//dR
	0xC0,	0x40,	//dG
	0x80,	0x40,	//dB
	0x00,	0x40,	//dynR
	0xC0,	0x40,	//dynG
	0x60,	0x20	//dynB
//	base  rand
};


// ************  Utilities section  ****************

#define RAD(a) ((a) * float(M_PI) / HALF_ANGLE)
#define ANG(a) round((a) * HALF_ANGLE / float(M_PI))
#define RGBA(r, g, b, a) (b | (g << 8) | (r << 16) | (a << 24))

#define phd_PopMatrix() (phd_mxptr -= 12)

// ************  Patcher Functions section  ***************
// Note: please, don't change or remove the C++ procedure you find here,
// because they are necessary to do work the plugin with the trng dll
// Anyway in many of these functions you can add your code


void TriggerTorchFlameFix(short item_number, int Node, FlameColorInfo *fcol)
{
	int size;
	Particle *part;

	part = &parts[GetFreeParticle()];

	int sR, sG, sB;
	int dR, dG, dB;

	sR = fcol->base_sR + (GetRandomControl()%fcol->rand_sR);
	sG = fcol->base_sG + (GetRandomControl()%fcol->rand_sG);
	sB = fcol->base_sB + (GetRandomControl()%fcol->rand_sB);

	dR = fcol->base_dR + (GetRandomControl()%fcol->rand_dR);
	dG = fcol->base_dG + (GetRandomControl()%fcol->rand_dG);
	dB = fcol->base_dB + (GetRandomControl()%fcol->rand_dB);
	
	part->R1 = (sR > 255) ? 255 : sR;
	part->G1 = (sG > 255) ? 255 : sG;
	part->B1 = (sB > 255) ? 255 : sB;

	part->R2 = (dR > 255) ? 255 : dR;
	part->G2 = (dG > 255) ? 255 : dG;
	part->B2 = (dB > 255) ? 255 : dB;

	part->R1 = (sR > 255) ? 255 : sR;
	part->G1 = (sG > 255) ? 255 : sG;
	part->B1 = (sB > 255) ? 255 : sB;

	part->R2 = (dR > 255) ? 255 : dR;
	part->G2 = (dG > 255) ? 255 : dG;
	part->B2 = (dB > 255) ? 255 : dB;

	part->FadeIn = 0;
	part->FadeOut = 8;
	part->LifeSpan = part->Life = (GetRandomControl()&7)+24;

	part->Pos.x = ((GetRandomControl()&15)-8);
	part->Pos.y = 0;
	part->Pos.z = ((GetRandomControl()&15)-8);

	part->Vel.x = ((GetRandomControl()&255)-128);
	part->Vel.y = -(GetRandomControl()&15)-16;
	part->Vel.z = ((GetRandomControl()&255)-128);
	part->Vel *= 1/32.0f;
	part->Acc.x = -part->Vel.x/32.0f;
	part->Acc.z = -part->Vel.z/32.0f;
	part->Acc.y = (-(GetRandomControl()&31)-8)/32.0f;

	size = ((GetRandomControl()&31)+80);
	part->startSize = size*2;
	part->destSize = size/8;

	part->Rot = GetRandomControl()*2;
	if (GetRandomControl()&1)
		part->AngVel = -(GetRandomControl()&15)-16;
	else
		part->AngVel = (GetRandomControl()&15)+16;

	part->DataIndex = 255;
	part->SpriteIndex = 1;
	//part->SpriteSeq = 2;
	//part->FrameRate = -1;
	if (!Node)
	{
		part->EmitterIndex = -1;
		part->EmitterNode = 14 + 0x40;
	}
	else
	{
		part->EmitterIndex = item_number;
		part->EmitterNode = 0x40;
	}

	SoundEffect(150, &items[item_number].pos, 0);
}


void DoLoneTorchFlame(Tr4ItemInfo *item)
{
	short index = item->item_flags[0];
	if (index < 0)
		index = 0;
	else if (index > 5)
		index = 5;

	auto fcol = &torchColor[index];

	int dynR = fcol->base_dynR + (GetRandomControl()%fcol->rand_dynR);
	int dynG = fcol->base_dynG + (GetRandomControl()%fcol->rand_dynG);
	int dynB = fcol->base_dynB + (GetRandomControl()%fcol->rand_dynB);
	if (dynR > 255)
		dynR = 255;
	if (dynG > 255)
		dynG = 255;
	if (dynB > 255)
		dynB = 255;

	TriggerDynamic(item->pos.xPos, item->pos.yPos-256, item->pos.zPos, 12-(GetRandomControl()&1), dynR, dynG, dynB);
	TriggerTorchFlameFix(short(item - items), 1, fcol);
}


void DoHandTorchFlame()
{
	short throwAnim = objects[SLOT_TORCH_ANIM].anim_index + 1;
	short throwFrame = anims[throwAnim].frame_base + 5;

	if (lara_left_arm.animNum == throwAnim && lara_left_arm.frameNum >= (throwFrame + 7))
		return;

	auto fcol = &laraTorch;
	int r = fcol->base_dynR + (GetRandomControl()%fcol->rand_dynR);
	int g = fcol->base_dynG + (GetRandomControl()%fcol->rand_dynG);
	int b = fcol->base_dynB + (GetRandomControl()%fcol->rand_dynB);
	if (r > 255)
		r = 255;
	if (g > 255)
		g = 255;
	if (b > 255)
		b = 255;

	phd_vector pos = {-32, 64, 256};
	GetLaraJointPos(&pos, 14);

	TriggerDynamic(pos.x, pos.y, pos.z, 12-(GetRandomControl()&1), r, g, b);

	if (lara_left_arm.frameNum >= throwFrame)
		return;

	TriggerTorchFlameFix(*Trng.pGlobTomb4->pAdr->pLaraIndex, 0, &laraTorch);
}


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


int Patch_CustLoneTorch(void)
{
	static BYTE VetBytes[]={0x66, 0xB8, 0x02, 0x00, 0xFF, 0x25, 0xE0, 0x08, 0x67, 0x00};

	return ApplyCodePatch(0x41FBB8, VetBytes, 10);
}


int Patch_CustHandTorch(void)
{
	static BYTE VetBytes[]={0x66, 0xB8, 0x03, 0x00, 0xFF, 0x25, 0xE0, 0x08, 0x67, 0x00, 0x90, 0x90, 0x90, 0x90, 0x90};

	return ApplyCodePatch(0x41F87A, VetBytes, 15);
}


// FOR_YOU: In this function you insert the callings of functions used to change tomb4 code
// You can get these functions, in the correct format, using Trng Core -> Asm Souce Editor -> Debugging menu
// program
// note: if there is a conflict the function will return "false"
// and the tomb4 program will abort with an error message
bool CreateMyCodePatches(void)
{
	// the call of the code patch to TYPE_HERE:
	// example: 
	// SET_PATCH(Path_RedirCollision)
	// to call the function Patch_RedirCollision() created with TrngPatcher program (command Assmembly->Create Dynamic Patch Generator)

	if (SetReservedDataZone(0x6708E0, 128) != APPC_OK) return false;

	SET_PATCH(Patch_UpdateParts);
	SET_PATCH(Patch_DrawParts);
	SET_PATCH(Patch_CustLoneTorch);
	SET_PATCH(Patch_CustHandTorch);

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


BEGIN_ASM_PROC(Patch_02)
	PUSH ESI
	CALL DoLoneTorchFlame
	ADD ESP, 4h
	MOV EDI, 41FC0Ch
	JMP EDI
END_ASM_PROC


BEGIN_ASM_PROC(Patch_03)
	CALL DoHandTorchFlame
	MOV ECX, 41F914h
	JMP ECX
END_ASM_PROC


// FOR_YOU: In the SubPatchArray you'll type all procedure names of your code in the dll you
// wish were called from tomb4 code.
// type them in the order of ax value. So first asm proc in the list, will be called
// with ax=0, while the second in the list will be called with ax=1 ect.

void *SubPatchArray[] = {
// TYPE_HERE your asm procedure names to call from tomb4 code
	&Patch_00,
	&Patch_01,
	&Patch_02,
	&Patch_03,
	NULL
};


// ************  MainPatcher() asm procedure  *****************

// FOR_YOU: This is your main patcher procedure.
// All your patch in tomb4 code will call this procedure passing to it, in ax register,
// the number of subpatch to call

BEGIN_ASM_PROC(MainPatcher)
	and eax, 0ffffh
	mov eax, dword ptr [SubPatchArray+eax*4];
	jmp eax
END_ASM_PROC



// ************  Helper Functions *****************


int Round(float x)
{
	return (x > 0.0f) ? int(x+0.5f) : int(x-0.5f);
}


float Lerp(float a, float b, float t)
{
	return a + t*(b-a);
}


float LerpClamp(float a, float b, float t)
{

	if (t > 1.0f)
		t = 1.0f;
	else if (t < 0.0f)
		t = 0.0f;

	return a + t*(b-a);
}


float InverseLerp(float val1, float val2, float x, bool clamp)
{
	float t = (x - val1) / (val2 - val1);

	if (clamp)
	{
		if (t < 0.0f)
			t = 0.0f;
		else if (t > 1.0f)
			t = 1.0f;
	}

	return t;
}


float SimpleDist(const Vector3f &c1, const Vector3f &c2)
{
	Vector3f d = (c1 - c2);
	d.x = abs(d.x);
	d.y = abs(d.y);
	d.z = abs(d.z);

	float maxdist = d.x;
	if (d.y > maxdist)
		maxdist = d.y;
	if (d.z > maxdist)
		maxdist = d.z;

	return maxdist;
}


int TestForWall(int x, int y, int z, short* room)
{
	void *floor = GetFloor(x, y, z, room);

	int h = GetHeight(floor, x, y, z);
	int c = GetCeiling(floor, x, y, z);

	if (h != 0x8100)
	{
		if ((h-y) > 0 && (c-y) < 0)
			return 0; // No wall there

		return 2; // Block (floor or ceiling)
	}

	return 1; // Wall there!
}


StrBoxCollisione GetTransformedBounds(Tr4ItemInfo *item)
{
	StrBoxCollisione *bbox = GetBoundsAccurate((StrItemTr4*)item);
	StrBoxCollisione res = {bbox->MinX, bbox->MaxX, bbox->MinY, bbox->MaxY, bbox->MinZ, bbox->MaxZ};

	if (item->pos.yRot)
	{
		float angle = -RAD(item->pos.yRot);
		float s1 = sin(angle);
		float c1 = cos(angle);

		int minx = bbox->MinX;
		int maxx = bbox->MaxX;
		int minz = bbox->MinZ;
		int maxz = bbox->MaxZ;

		phd_vector vert[4];

		vert[0].x = minx, vert[0].y = 0, vert[0].z = minz;
		vert[1].x = maxx, vert[1].y = 0, vert[1].z = minz;
		vert[2].x = minx, vert[2].y = 0, vert[2].z = maxz;
		vert[3].x = maxx, vert[3].y = 0, vert[3].z = maxz;

		int minxr = 0x7FFF, maxxr = -0x7FFF;
		int minzr = 0x7FFF, maxzr = -0x7FFF;

		for (int i = 0; i < 4; ++i)
		{
			int newx = vert[i].x*c1 - vert[i].z*s1;
			int newz = vert[i].z*c1 + vert[i].x*s1;

			if (minxr > newx)
				minxr = newx;
			if (maxxr < newx)
				maxxr = newx;
			if (minzr > newz)
				minzr = newz;
			if (maxzr < newz)
				maxzr = newz;
		}

		res.MinX = minxr;
		res.MinZ = minzr;
		res.MaxX = maxxr;
		res.MaxZ = maxzr;
	}

	return res;
}


int GetFreeParticle()
{
	int i, free;
	Particle* part;
	
	for (part = &parts[next_part], free = next_part, i = 0; i < MAX_PARTS; ++i)
	{
		if (!part->Life)
		{
			next_part = (free + 1) % MAX_PARTS;

			ClearMemory(&parts[free], sizeof(Particle));
			parts[free].EmitterIndex = -1;
			parts[free].EmitterNode = -1;
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
		if (part->Life < eldest)
		{
			free = i;
			eldest = part->Life;
		}
	}

	next_part = (free + 1) % MAX_PARTS;

	ClearMemory(&parts[free], sizeof(Particle));
	parts[free].EmitterIndex = -1;
	parts[free].EmitterNode = -1;
	return free;
}


void UpdateParticle(Particle* part, ParticleData* pdata)
{
	if (!part || !pdata)
		return;

	// switch case based on field value in ParticleData?
	switch (pdata->Control)
	{
	case 1:
	default:
		int t = part->LifeSpan - part->Life;
		part->Vel.x = sin(5*RAD(t*512))*32.0f;
		part->Vel.y = cos(3*RAD(t*512))*32.0f;
		part->Vel.z = -16 + sin(4*RAD(t*512))*32.0f;
		break;
	}
}


int TestCollisionSpheres(Particle* part, Tr4ItemInfo* item)
{
	int flags = 0;
	int num = 0;

	if (part && item)
	{
		MeshSphere SphereList[34];
		num = GetSpheres(item, SphereList, 1);

		for (int i = 0; i < num; ++i)
		{
			auto sph = &SphereList[i];
			int r = sph->r;
			if (r > 0)
			{
				int x = Round(part->Pos.x) - sph->x;
				int y = Round(part->Pos.y) - sph->y;
				int z = Round(part->Pos.z) - sph->z;

				if ((x*x + y*y + z*z) < r*r)
					flags |= (1 << i);
			}
		}
	}

	return flags;
}


Tr4ItemInfo* FindNearestTarget(Particle* part, ParticleData* pdata, short objNum)
{
	int nearest = 0x7FFFFFFF;
	Tr4ItemInfo* ret = nullptr;

	if (part && pdata)
	{
		for (int i = 0; i < Trng.pGlobTomb4->pAdr->TotItemsMax; ++i)
		{
			auto item = &items[i];
			if ((item->object_number == objNum) && (item->active))
			{
				auto v = Vector3f(item->pos.xPos, item->pos.yPos, item->pos.zPos);
				if (SimpleDist(v, part->Pos) < pdata->HomingRange)
				{
					int dist = int(v.distance(part->Pos));
					if (dist < nearest)
					{
						ret = item;
						nearest = dist;
					}
				}
			}
		}
	}

	return ret;
}


void ParticleHoming(Particle* part, ParticleData* pdata, Tr4ItemInfo *item, int node = -1)
{
	if (!part || !pdata || !item)
		return;

	float s = sin(RAD(-item->pos.yRot));
	float c = cos(RAD(-item->pos.yRot));

	Vector3f predict(item->speed * s, 0/*item->fallspeed*/,  item->speed * c);

	short meshnum = objects[item->object_number].nmeshes;
	if (node < 0)
		node = GetRandomControl() % meshnum;
	else
		node = (node >= meshnum) ? (meshnum-1) : node;

	StrMovePosition pos = {0, 0, 0};
	GetJointAbsPosition((StrItemTr4*)item, &pos, node);

	Vector3f ip(pos.RelX, pos.RelY, pos.RelZ);
	ip += predict;

	if (int(SimpleDist(ip, part->Pos)) > pdata->HomingRange)
		return;

	int dist = int(ip.distance(part->Pos));
	if (dist > pdata->HomingRange)
		return;

	auto dir = (ip - part->Pos).normalized();
	float homefact = pdata->HomingFactor;
	if (homefact < 0.0f)
	{
		dir = -dir;
		homefact = -homefact;
	}
		
	auto interp = part->Vel.normalized().slerp(dir, homefact);
	part->Vel = interp * (part->Vel.magnitude() + pdata->HomingAccel);
}


bool ParticleCollideWalls(Particle* part, ParticleData* pdata)
{
	if (!part || !pdata)
		return false;

	auto p = (part->Pos + part->Vel).to_phdvect();

	short tRoom = part->RoomIndex;
	int status = TestForWall(p.x, p.y, p.z, &tRoom);
	if (part->RoomIndex != tRoom)
		part->RoomIndex = tRoom;

	if (status)
	{
		if (TestForWall(part->Pos.x, p.y, p.z, &tRoom))
			part->Vel.z = -part->Vel.z * (1-pdata->ColDampening);
		if (TestForWall(p.x, p.y, part->Pos.z, &tRoom))
			part->Vel.x = -part->Vel.x * (1-pdata->ColDampening);

		return true;
	}

	return false;
}


Vector3f GetSlopeNormal(Tr4FloorInfo *floor, int x, int y, int z)
{
	Vector3f dummy(0, -1, 0);
	if (!floor)
		return dummy;

	WORD tilts = GetTiltType(floor, x, y, z);
	signed char tilt_x = tilts & 0xFF;
	signed char tilt_z = tilts >> 8;

	return Vector3f(float(-tilt_x), -4.0f, float(-tilt_z)).normalized();
}


bool ParticleCollideFloors(Particle* part, ParticleData* pdata, float friction = 0.0f)
{
	if (!part || !pdata)
		return false;

	auto p = (part->Pos + part->Vel).to_phdvect();
	short tRoom = part->RoomIndex;

	Tr4FloorInfo* floor = (Tr4FloorInfo*) GetFloor(p.x, p.y, p.z, &tRoom);
	int fh = GetHeight(floor, p.x, p.y, p.z);
	int ch = GetCeiling(floor, p.x, p.y, p.z);
	if (part->RoomIndex != tRoom)
		part->RoomIndex = tRoom;

	if (p.y > fh || p.y < ch)
	{
		if (p.y > fh && pdata->AccurateFloorCol)
		{
			auto n = GetSlopeNormal(floor, p.x, p.y, p.z);

			auto refl = part->Vel - (n * part->Vel.dot(n) * 2);
			part->Vel = refl * (1-pdata->ColDampening);
			if (friction)
			{
				part->Vel.x *= friction;
				part->Vel.z *= friction;
			}
		}
		else
			part->Vel.y = -part->Vel.y * (1-pdata->ColDampening);

		return true;
	}

	return false;
}


void UpdateParts()
{
	Particle* part = &parts[0];

	for (int i = 0; i < MAX_PARTS; ++i, ++part)
	{
		if (part->Life <= 0)
			continue;

		part->Vel += part->Acc;
		auto pdata = &partdata[part->DataIndex];

		if (pdata)
		{
			UpdateParticle(part, pdata);

			if (pdata && pdata->MaxSpeed > 0)
			{
				float speed = part->Vel.magnitude();
				if (speed > pdata->MaxSpeed)
					part->Vel *= pdata->MaxSpeed / speed;
			}
		}
		
		part->Rot += part->AngVel;
		part->Pos += part->Vel;

		--part->Life;
	}
}

// process color, rotation, size, blending mode and texture, call DX functions for quad/line draw
void DrawParticle(Particle* const part, ParticleData* const pdata, int smallest_size, long* const view)
{
	if (!part)
		return;

	long z1 = view[2];
	
	if (z1 <= 0)
		return;

	if (z1 >= 0x5000)
	{
		part->Life = 0;
		return;
	}

	int fadetime = part->LifeSpan;
	if (part->ColorFadeTime > 0 && (part->ColorFadeTime < part->LifeSpan))
		fadetime = part->ColorFadeTime;

	float t = (part->LifeSpan - part->Life)/float(fadetime);
	if (t < 0.0f)
		t = 0.0f;
	if (t > 1.0f)
		t = 1.0f;
	
	BYTE cR = part->R1;
	BYTE cG = part->G1;
	BYTE cB = part->B1;
			
	//if (!part->ColorMode)
	{
		cR = Round(Lerp(float(part->R1), float(part->R2), t));
		cG = Round(Lerp(float(part->G1), float(part->G2), t));
		cB = Round(Lerp(float(part->B1), float(part->B2), t));
	}

	if (part->FadeIn)
	{
		int lifedif = part->LifeSpan - part->Life;
		if (lifedif < part->FadeIn)
		{
			float s = lifedif/float(part->FadeIn);
			cR = Round(Lerp(0, cR, s));
			cG = Round(Lerp(0, cG, s));
			cB = Round(Lerp(0, cB, s));
		}
	}

	if (part->FadeOut)
	{
		if (part->Life < part->FadeOut)
		{
			float s = part->Life/float(part->FadeOut);
			cR = Round(Lerp(0, cR, s));
			cG = Round(Lerp(0, cG, s));
			cB = Round(Lerp(0, cB, s));
		}
	}


	short SpriteSlot = -1;

	if (pdata)
		SpriteSlot = pdata->SpriteSlot;

	if (SpriteSlot > 0)
	{
		int s1, s2;
		float size = Lerp(part->destSize, part->startSize, t);
		int sizeint = Round(size);

		if (pdata->NoPerspective)
			s1 = s2 = sizeint;
		else
		{
			s1 = Round(phd_persp * size / z1);

			if (s1 < smallest_size)
				s1 = smallest_size;

			s2 = s1;
		}
		

		int s1h = s1 >> 1;
		int s2h = s2 >> 1;
		
		long x1, y1, x2, y2, x3, y3, x4, y4;
		x1 = view[0];
		y1 = view[1];

		if (x1 + s1h >= phd_winxmin && x1 - s1h < phd_winxmax && y1 + s2h >= phd_winymin && y1 - s2h < phd_winymax)
		{

			D3DTLVERTEX v[4];

			if (part->Rot)
			{
				float sin1 = sin(RAD(part->Rot));
				float cos1 = cos(RAD(part->Rot));

				int sx1 = Round(-s1h * sin1);
				int sx2 = Round(s1h * sin1);

				int sy1 = Round(-s2h * sin1);
				int sy2 = Round(s2h * sin1);

				int cx1 = Round(-s1h * cos1);
				int cx2 = Round(s1h * cos1);

				int cy1 = Round(-s2h * cos1);
				int cy2 = Round(s2h * cos1);

				x1 = sx1 - cy1 + view[0];
				x2 = sx2 - cy1 + view[0];
				x3 = sx2 - cy2 + view[0];
				x4 = sx1 - cy2 + view[0];

				y1 = cx1 + sy1 + view[1];
				y2 = cx2 + sy1 + view[1];
				y3 = cx2 + sy2 + view[1];
				y4 = cx1 + sy2 + view[1];

				setXY4(v, x1, y1, x2, y2, x3, y3, x4, y4, z1, clipflags);
			}
			else
			{
				x1 = view[0] - s1h;
				x2 = view[0] + s1h;

				y1 = view[1] - s2h;
				y2 = view[1] + s2h;

				setXY4(v, x1, y1, x2, y1, x2, y2, x1, y2, z1, clipflags);
			}

			long c1 = RGBA(cR, cG, cB, 0xFF);

			v[0].color = c1;
			v[1].color = c1;
			v[2].color = c1;
			v[3].color = c1;
			v[0].specular = 0xFF000000;
			v[1].specular = 0xFF000000;
			v[2].specular = 0xFF000000;
			v[3].specular = 0xFF000000;
			
			int usedsprite = part->SpriteIndex;

#if 0 // particle sprite animation
			if (part->SpriteSeq > 1)
			{
				if (part->FrameRate > 0)
				{
					int lifedif = part->LifeSpan - part->Life;
					usedsprite += ((lifedif/part->FrameRate) % part->SpriteSeq);
				}
				else
				{
					float s = part->Life/float(part->LifeSpan);
					usedsprite = Round(Lerp(part->SpriteIndex + (part->SpriteSeq-1), part->SpriteIndex, s)) - 1;
				}
			}
#endif

			int spriteslot = SLOT_DEFAULT_SPRITES;

			if ((pdata->SpriteSlot == SLOT_MISC_SPRITES || pdata->SpriteSlot == SLOT_CUSTOM_SPRITES) &&
				objects[pdata->SpriteSlot].loaded)
			{
				spriteslot = pdata->SpriteSlot;
			}

			SpriteStruct* sprite = (spriteinfo + objects[spriteslot].mesh_index + usedsprite);

			TextureStruct tex;
			tex.drawtype = pdata->BlendingMode;

			tex.tpage = sprite->tpage;
			tex.u1 = sprite->x1;
			tex.v1 = sprite->y1;
			tex.u2 = sprite->x2;
			tex.v2 = sprite->y1;
			tex.u3 = sprite->x2;
			tex.v3 = sprite->y2;
			tex.u4 = sprite->x1;
			tex.v4 = sprite->y2;

			(*AddQuadSorted)(v, 0, 1, 2, 3, &tex, 0);
		}
	}
	else
	{
		D3DTLVERTEX v[2];

		long x1 = view[0];
		long y1 = view[1];
		long z1 = view[2];
		long x2 = view[3];
		long y2 = view[4];
		long z2 = view[5];

		long c1 = RGBA(cR, cG, cB, 0xFF);
		long c2 = RGBA(cR >> 2, cG >> 2, cB >> 2, 0xFF);
		
		if (ClipLine(x1, y1, z1, x2, y2, z2, phd_winxmin, phd_winymin, phd_winxmax, phd_winymax))
		{
			v[0].sx = float(x1);
			v[0].sy = float(y1);
			v[0].rhw = f_mpersp / z1 * f_moneopersp;
			v[0].sz = f_a - v[0].rhw * f_boo;
			v[0].color = c1;
			v[0].specular = 0xFF000000;

			v[1].sx = float(x2);
			v[1].sy = float(y2);
			v[1].rhw = f_mpersp / z2 * f_moneopersp;
			v[1].sz = f_a - v[1].rhw * f_boo;
			v[1].color = c2;
			v[1].specular = 0xFF000000;

			(*AddLineSorted)(&v[0], &v[1], 6);
		}
	}
}

// prepare the draw matrix and loop through all particles - process item/mesh attachment, convert pos from world to screen coordinates
void DrawParts()
{
	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.xPos, lara_item->pos.yPos, lara_item->pos.zPos);

	long* mptr = phd_mxptr;

	Particle* part = &parts[0];

	int x1 = 0, y1 = 0, z1 = 0;
	for (int i = 0; i < MAX_PARTS; ++i, ++part)
	{
		if (part->Life <= 0)
			continue;

		x1 = Round(part->Pos.x);
		y1 = Round(part->Pos.y);
		z1 = Round(part->Pos.z);

		auto pdata = &partdata[part->DataIndex];

		// item and/or mesh attachment of particle
		if (part->EmitterIndex >= 0)
		{
			int index = part->EmitterIndex;
			if (index > Trng.pGlobTomb4->pAdr->TotItemsMax)
				index = *Trng.pGlobTomb4->pAdr->pLaraIndex;

			auto item = &items[index];

			if (part->EmitterNode >= 0) // if attached to specific mesh node of item
			{
				phd_vector posNode = {0, 0, 0};
				if (part->EmitterNode & 0x40) // hack for burning_torch
				{
					posNode.x = -16, posNode.y = -8, posNode.z = 160;
				}
				else if (pdata)
				{
					posNode.x = pdata->Offset.x;
					posNode.y = pdata->Offset.y;
					posNode.z = pdata->Offset.z;
				}

				int Node = part->EmitterNode & 0x1F;

				GetJointAbsPosition((StrItemTr4*)item, (StrMovePosition*)(&posNode), Node);

				x1 += posNode.x;
				y1 += posNode.y;
				z1 += posNode.z;
			}
			else // no mesh node, use item's pos
			{
				x1 += item->pos.xPos;
				y1 += item->pos.yPos;
				z1 += item->pos.zPos;
			}

			int cutoff = 0x10000;
			if (pdata && pdata->AttachRandom)
				cutoff = pdata->AttachCutoff + (GetRandomDraw() % pdata->AttachRandom);
			else
				cutoff = (GetRandomDraw()&3)+8;
			
			if ((part->LifeSpan - part->Life) > cutoff)
			{
				part->Pos.x = x1;
				part->Pos.y = y1;
				part->Pos.z = z1;
				part->EmitterIndex = -1;
				part->EmitterNode = -1;
			}
		}
		else if (part->EmitterNode >= 0) // if EmitterNode >= 0 when EmitterIndex < 0, use Lara meshes
		{
			phd_vector posNode = {0, 0, 0};
			int Node = part->EmitterNode;
			if (Node & 0x40) // hack for burning_torch
			{
				posNode.x = -16, posNode.y = 40, posNode.z = 160;
				Node &= ~(0x40);
			}
			else if (pdata)
			{
				posNode.x = pdata->Offset.x;
				posNode.y = pdata->Offset.y;
				posNode.z = pdata->Offset.z;
			}

			GetLaraJointPos(&posNode, (Node > 14) ? 14 : Node);

			x1 += posNode.x;
			y1 += posNode.y;
			z1 += posNode.z;

			int cutoff = 0x10000;
			if (pdata && pdata->AttachRandom)
				cutoff = pdata->AttachCutoff + (GetRandomDraw() % pdata->AttachRandom);
			else
				cutoff = (GetRandomDraw()&3)+8;
			
			if ((part->LifeSpan - part->Life) > cutoff)
			{
				part->Pos.x = x1;
				part->Pos.y = y1;
				part->Pos.z = z1;
				part->EmitterNode = -1;
			}
		}

		x1 -= lara_item->pos.xPos;
		y1 -= lara_item->pos.yPos;
		z1 -= lara_item->pos.zPos;

		if (x1 < -20480 || x1 > 20480 ||
			y1 < -20480 || y1 > 20480 ||
			z1 < -20480 || z1 > 20480)
		{
			part->Life = 0;
			continue;
		}


		// convert from world coordinates to screen coordinates
		long result[3] = {0, 0, 0};
		long TempMesh[3] = {0, 0, 0};
		long viewcoords[6] = {0,0,0,0,0,0};

		TempMesh[0] = x1;
		TempMesh[1] = y1;
		TempMesh[2] = z1;

		result[0] = (mptr[M00] * TempMesh[0] + mptr[M01] * TempMesh[1] + mptr[M02] * TempMesh[2] + mptr[M03]);
		result[1] = (mptr[M10] * TempMesh[0] + mptr[M11] * TempMesh[1] + mptr[M12] * TempMesh[2] + mptr[M13]);
		result[2] = (mptr[M20] * TempMesh[0] + mptr[M21] * TempMesh[1] + mptr[M22] * TempMesh[2] + mptr[M23]);

		float zv = f_persp / float(result[2]);
		viewcoords[0] = Round(result[0] * zv + f_centerx);
		viewcoords[1] = Round(result[1] * zv + f_centery);
		viewcoords[2]  = result[2] >> 14;

		// if particle is a line do world to screen transform for second vertex
		if (!pdata || pdata->SpriteSlot <= 0)
		{
			float size = Lerp(part->destSize, part->startSize, part->Life/float(part->LifeSpan));
			auto vel = part->Vel;

			if (pdata && pdata->LineIgnoreVel)
				vel = vel.normalized(); // ignore speed contribution to line particle's size
			else
				size *= (1.0f/32.0f); // scale down size

			vel *= size;

			TempMesh[0] = Round(x1 - vel.x);
			TempMesh[1] = Round(y1 - vel.y);
			TempMesh[2] = Round(z1 - vel.z);

			result[0] = (mptr[M00] * TempMesh[0] + mptr[M01] * TempMesh[1] + mptr[M02] * TempMesh[2] + mptr[M03]);
			result[1] = (mptr[M10] * TempMesh[0] + mptr[M11] * TempMesh[1] + mptr[M12] * TempMesh[2] + mptr[M13]);
			result[2] = (mptr[M20] * TempMesh[0] + mptr[M21] * TempMesh[1] + mptr[M22] * TempMesh[2] + mptr[M23]);

			zv = f_persp/float(result[2]);
			viewcoords[3] = Round(result[0] * zv + f_centerx);
			viewcoords[4] = Round(result[1] * zv + f_centery);
			viewcoords[5]  = result[2]>>14;

		}

		long minw = 4;

		// draw the particle to the given screen coordinates
		DrawParticle(part, pdata, minw, viewcoords);
	}

	phd_PopMatrix();
}


void SimpleParticle(int x, int y, int z, short room)
{
	//if (!(*Trng.pGlobTomb4->pAdr->pFrameCounter & 7))
	{
		int index = GetFreeParticle();
		Particle *part = &parts[index];

		int xrand = x + (GetRandomControl()&127) - 64;
		int yrand = y + (GetRandomControl()&127) - 64;
		int zrand = z + (GetRandomControl()&127) - 64;

		part->RoomIndex = room;

		part->Pos = Vector3f(xrand, yrand, zrand);
		//part->Vel = Vector3f((GetRandomControl()&31)-16, -((GetRandomControl()&15)+80), (GetRandomControl()&31)-16);
		//part->Acc = Vector3f(0, 3.0f, 0);

		part->Life = part->LifeSpan = 270 + (GetRandomControl()&15);
		
		//part->SpriteIndex = 0;
		//part->Rot = part->AngVel = 0;


		part->R1 = 255;
		part->B1 = 160 + (GetRandomControl()&63);
		part->G2 = 96 + (GetRandomControl()&63);
		part->B2 = 160 + (GetRandomControl()&31);
		part->R2 = part->G1 = 0;
		

		part->startSize = 20+(GetRandomControl()&31);
		part->destSize = 20+(GetRandomControl()&31);
		//part->dSize = 96 + (GetRandomControl()&31);
		part->FadeIn = 15;
		part->FadeOut = 15;
		part->EmitterIndex = -1;
		part->EmitterNode = -1;

		part->DataIndex = 0;
	}
}


void ParticleEmitterControl(short item_number)
{
	auto item = &items[item_number];

	if (TriggerActive((StrItemTr4*)item))
	{
		switch (item->trigger_flags)
		{
		case 0:
			SimpleParticle(item->pos.xPos, item->pos.yPos, item->pos.zPos, item->room_number);
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
	
	/* unused callbacks which you may uncomment if you wish */

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


