#pragma once
#include "structures.h"
#include "constants_mine.h"

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

#pragma pack(1)

// TYPE_HERE: here you can type your structure definitions like it has been done
// in the "structures.h" file for trng dll

// --------------- PRESET STRUCTURE ZONE -----------------------------
// Please, don't remove structures and fields you find in this "PRESET ZONE". They will be used by some
// preset functions of your plugin sources
// However, you can add new fields and structures in following structures, of course
typedef struct StrSavegameGlobalData {
	// FOR_YOU:
	// define here your variables that you wish were saved (and then restored) to/from savegame in GLOBAL section 
	//           (only one for all levels)
	// note: the size of this structure should be always even (if you add BYTE variable, remember to compensate that 
	//       with another BYTE vairable or placefolder)

	ulong gameTick;
	
}SavegameGlobalDataFields;

typedef struct StrSavegameLocalData {
	// FOR_YOU:
	// define here your variables that you wish were saved (and then restored) to/from savegame in LOCAL section (one for each different level)
	// note: the size of this structure should be always even (if you add BYTE variable, compensate it with another BYTE vairable or placefolder)

}SavegameLocalDataFields;


typedef struct StrSavegameData {
	StrSavegameGlobalData Global;
	StrSavegameLocalData  Local;
}SavegameDataFields;


typedef struct StrBaseAssignSlotMine {
	int TotAssign;
	StrRecordAssSlot VetAssignSlot[MAX_ASSIGN_SLOT_MINE];
}BaseAssignSlotMineFields;


typedef struct StrMyData {
	StrSavegameData Save;  // variable that it will be saved and restored to/from savegame
	// FOR_YOU:
	// define here all your global variables, i.e. those variables that will be seen from all procedures and keep 
	// their value between all cycles of the game.
	// after you defined a variabile like:
	// int  Alfa;
	// then you'll be able to access to them using syntax like:
	// MyData.Alfa = 3;
	// if (MyData.Alfa == 5) .
	int TotProgrActions;
	int LastProgrActionIndex; // used in case of overloading

	StrProgressiveAction VetProgrActions[MAX_MYPROGR_ACTIONS];
	StrBaseGenericCustomize BaseCustomizeMine;  // stored all your customize script commands for current level
	StrBaseGenericParameters BaseParametersMine; // store of all your parameters= script commands of current level
	StrBaseAssignSlotMine BaseAssignSlotMine; // stored all AssignSlot= command script commands of current level
}MyDataFields;
// ----------------- END PRESET STRUCTURE ZONE ---------------------------------------


// tomb4 SPARKS definition as reference
typedef struct
{
	long	x;				//00
	long	y;				//04
	long	z;				//08

	short	Xvel;			//0C
	short	Yvel;			//0E
	short	Zvel;			//10		
	short	Gravity;		//12
	short	RotAng;			//14
	short	Flags;			//16

	uchar	sSize;		//18
	uchar	dSize;		//19
	uchar	Size;		//1A
	uchar	Friction; 	//1B

	uchar	Scalar;		//1C
	uchar	Def;		//1D
	char	RotAdd;		//1E
	char	MaxYvel;	//1F

	uchar	On;			//20
	uchar	sR;			//21
	uchar	sG;  		//22
	uchar	sB;  		//23

	uchar	dR;			//24
	uchar	dG;  		//25
	uchar	dB;  		//26
	uchar	R;			//27

	uchar	G;  			//28
	uchar	B;  			//29
	uchar	ColFadeSpeed;	//2A
	uchar	FadeToBlack;	//2B

	uchar	sLife;			//2C
	uchar 	Life;			//2D
	uchar	TransType;		//2E
	uchar	extras;			//2F

	char	Dynamic;		//30
	uchar	FxObj;			//31
	uchar	RoomNumber; 	//32
	uchar	NodeNumber;		//33

} SPARKS;	//34


struct phd_vector
{
	int x, y, z;

	phd_vector() = default;

	phd_vector(int a, int b, int c) : x(a), y(b), z(c) {}
};


struct phd_3dpos
{
	int xPos, yPos, zPos;
	short xRot, yRot, zRot;
};

struct PCLIGHT
{
	float x;
	float y;
	float z;
	float r;
	float g;
	float b;
	long shadow;
	float Inner;
	float Outer;
	float InnerAngle;
	float OuterAngle;
	float Cutoff;
	float nx;
	float ny;
	float nz;
	long ix;
	long iy;
	long iz;
	long inx;
	long iny;
	long inz;
	float tr;
	float tg;
	float tb;
	float rs;
	float gs;
	float bs;
	long fcnt;
	uchar Type;
	uchar Active;
	phd_vector rlp;
	long Range;
};

struct ITEM_LIGHT
{
	long r;
	long g;
	long b;
	long ambient;
	long rs;
	long gs;
	long bs;
	long fcnt;
	PCLIGHT	CurrentLights[21];
	PCLIGHT	PrevLights[21];
	long nCurrentLights;
	long nPrevLights;
	long room_number;
	long RoomChange;
	phd_vector item_pos;
	void* pCurrentLights;
	void* pPrevLights;
};

struct Tr4ItemInfo
{
	int floor;
	ulong touch_bits;
	ulong mesh_bits;
	short object_number;
	short current_anim_state;
	short goal_anim_state;
	short required_anim_state;
	short anim_number;
	short frame_number;
	short room_number;
	short next_item;
	short next_active;
	short speed;
	short fallspeed;
	short hit_points;
	ushort box_number;
	short timer;
	short flags;
	short shade;
	short trigger_flags;
	short carried_item;
	short after_death;
	ushort fired_weapon;
	short item_flags[4];
	void* data;
	phd_3dpos pos;
	ITEM_LIGHT il;
	ulong active : 1;
	ulong status : 2;
	ulong gravity_status : 1;
	ulong hit_status : 1;
	ulong collidable : 1;
	ulong looked_at : 1;
	ulong dynamic_light : 1;
	ulong poisoned : 1;
	ulong ai_bits : 5;
	ulong really_active : 1;
	ulong InDrawRoom : 1;
	ulong meshswap_meshbits;
	short draw_room;
	short TOSSPAD;

}; // 15F6


struct Tr4ObjectInfo
{
	short nmeshes;
	short mesh_index;
	long bone_index;
	short* frame_base;
	void (*initialise)(short item_number);
	void (*control)(short item_number);
	void (*floor)(Tr4ItemInfo* item, long x, long y, long z, long* height);
	void (*ceiling)(Tr4ItemInfo* item, long x, long y, long z, long* height);
	void (*draw_routine)(Tr4ItemInfo* item);
	void (*collision)(short item_num, Tr4ItemInfo* laraitem, void* coll);
	short object_mip;
	short anim_index;
	short hit_points;
	short pivot_length;
	short radius;
	short shadow_size;
	ushort bite_offset;
	ushort loaded : 1;
	ushort intelligent : 1;
	ushort non_lot : 1;
	ushort save_position : 1;
	ushort save_hitpoints : 1;
	ushort save_flags : 1;
	ushort save_anim : 1;
	ushort semi_transparent : 1;
	ushort water_creature : 1;
	ushort using_drawanimating_item : 1;
	ushort HitEffect : 2;
	ushort undead : 1;
	ushort save_mesh : 1;
	void (*draw_routine_extra)(Tr4ItemInfo* item);
	ulong explodable_meshbits;
	ulong padfuck;
};

struct Tr4FxInfo
{
	phd_3dpos pos;
	short room_number;
	short object_number;
	short next_fx;
	short next_active;
	short speed;
	short fallspeed;
	short frame_number;
	short counter;
	short shade;
	short flag1;
	short flag2;
};

struct LARA_ARM
{
	short* frame_base;
	short frame_number;
	short anim_number;
	short lock;
	short y_rot;
	short x_rot;
	short z_rot;
	short flash_gun;
};

struct Tr4LaraInfo
{
	short item_number;
	short gun_status;
	short gun_type;
	short request_gun_type;
	short last_gun_type;
	short calc_fallspeed;
	short water_status;
	short climb_status;
	short pose_count;
	short hit_frame;
	short hit_direction;
	short air;
	short dive_count;
	short death_count;
	short current_active;
	short current_xvel;
	short current_yvel;
	short current_zvel;
	short spaz_effect_count;
	short flare_age;
	short vehicle;
	short weapon_item;
	short back_gun;
	short flare_frame;
	short poisoned;
	short dpoisoned;
	uchar electric;
	uchar wet[15];
	ushort flare_control_left : 1;
	ushort Unused1 : 1;
	ushort look : 1;
	ushort burn : 1;
	ushort keep_ducked : 1;
	ushort IsMoving : 1;
	ushort CanMonkeySwing : 1;
	ushort Unused2 : 1;
	ushort OnBeetleFloor : 1;
	ushort BurnGreen : 1;
	ushort IsDucked : 1;
	ushort has_fired : 1;
	ushort Busy : 1;
	ushort LitTorch : 1;
	ushort IsClimbing : 1;
	ushort Fired : 1;
	long water_surface_dist;
	phd_vector last_pos;
	Tr4FxInfo* spaz_effect;
	long mesh_effects;
	short* mesh_ptrs[15];
	Tr4ItemInfo* target;
	short target_angles[2];
	short turn_rate;
	short move_angle;
	short head_y_rot;
	short head_x_rot;
	short head_z_rot;
	short torso_y_rot;
	short torso_x_rot;
	short torso_z_rot;
	LARA_ARM left_arm;
	LARA_ARM right_arm;
	ushort holster;
	void* creature;
	long CornerX;
	long CornerZ;
	char RopeSegment;
	char RopeDirection;
	short RopeArcFront;
	short RopeArcBack;
	short RopeLastX;
	short RopeMaxXForward;
	short RopeMaxXBackward;
	long RopeDFrame;
	long RopeFrame;
	ushort RopeFrameRate;
	ushort RopeY;
	long RopePtr;
	void* GeneralPtr;
	long RopeOffset;
	ulong RopeDownVel;
	char RopeFlag;
	char MoveCount;
	long RopeCount;
	char pistols_type_carried;
	char uzis_type_carried;
	char shotgun_type_carried;
	char crossbow_type_carried;
	char grenade_type_carried;
	char sixshooter_type_carried;
	char lasersight;
	char binoculars;
	char crowbar;
	char mechanical_scarab;
	uchar small_water_skin;
	uchar big_water_skin;
	char examine1;
	char examine2;
	char examine3;
	char puzzleitems[12];
	ushort puzzleitemscombo;
	ushort keyitems;
	ushort keyitemscombo;
	ushort pickupitems;
	ushort pickupitemscombo;
	short questitems;
	short num_small_medipack;
	short num_large_medipack;
	short num_flares;
	short num_pistols_ammo;
	short num_uzi_ammo;
	short num_revolver_ammo;
	short num_shotgun_ammo1;
	short num_shotgun_ammo2;
	short num_grenade_ammo1;
	short num_grenade_ammo2;
	short num_grenade_ammo3;
	short num_crossbow_ammo1;
	short num_crossbow_ammo2;
	short num_crossbow_ammo3;
	char beetle_uses;
	char blindTimer;
	char location;
	char highest_location;
	char locationPad;
};

struct Tr4CollInfo
{
	int mid_floor, mid_ceiling, mid_type;
	int front_floor, front_ceiling, front_type;
	int left_floor, left_ceiling, left_type;
	int right_floor, right_ceiling, right_type;
	int left_floor2, left_ceiling2, left_type2;
	int right_floor2, right_ceiling2, right_type2;

	int radius;
	int bad_pos, bad_neg, bad_ceiling;

	phd_vector shift, old;

	short old_anim_state;
	short old_anim_number;
	short old_frame_number;

	short facing;
	short quadrant;

	short coll_type;
	short* trigger;

	signed char tilt_x;
	signed char tilt_z;

	char hit_by_baddie;
	char hit_static;

	ushort slopes_are_walls : 2;
	ushort slopes_are_pits : 1;
	ushort lava_is_pit : 1;
	ushort enable_baddie_push : 1;
	ushort enable_spaz : 1;
	ushort hit_ceiling : 1;

}; //86


struct Tr4FloorInfo // 08
{
	short index;     // 00
	short box_flags; // 02
	uchar pit_room;  // 04
	signed char floor;     // 05
	uchar sky_room;  // 06
	signed char ceiling;   // 07
};


struct Tr4AnimStruct
{
	short* frame_ptr;
	uchar frame_rate;
	uchar frame_size;

	short current_anim_state;

	int velocity;
	int acceleration;
	int Xvelocity;
	int Xacceleration;

	short frame_base;
	short frame_end;
	short jump_anim_num;
	short jump_frame_num;
	short number_changes;
	short change_index;
	short number_commands;
	short command_index;

}; // 28


enum CameraType
{
	CHASE_CAMERA,
	FIXED_CAMERA,
	LOOK_CAMERA,
	COMBAT_CAMERA,
	CINEMATIC_CAMERA,
	HEAVY_CAMERA,
};


struct GameVector
{
	long x;
	long y;
	long z;
	short room_number;
	short box_number;
};

struct ObjectVector
{
	long x;
	long y;
	long z;
	short data;
	short flags;
};

struct Tr4CameraInfo
{
	GameVector pos;
	GameVector target;
	CameraType type;
	CameraType old_type;
	long shift;
	long flags;
	long fixed_camera;
	long number_frames;
	long bounce;
	long underwater;
	long target_distance;
	short target_angle;
	short target_elevation;
	short actual_elevation;
	short actual_angle;
	short number;
	short last;
	short timer;
	short speed;
	Tr4ItemInfo* item;
	Tr4ItemInfo* last_item;
	ObjectVector* fixed;
	long mike_at_lara;
	phd_vector mike_pos;
};


struct MeshSphere
{
	int x, y, z;
	int r;
};


struct Tr4ShatterItem
{
	MeshSphere sphere;
	void* il;
	short* meshp;
	long bit;
	short yRot;
	short flags;
};


struct D3DTLVERTEX
{
	float sx;
	float sy;
	float sz;
	float rhw;

	ulong color;
	ulong specular;

	float tu;
	float tv;
};


struct TextureStruct
{
	ushort drawtype;
	ushort tpage;
	ushort flag;
	float u1;
	float v1;
	float u2;
	float v2;
	float u3;
	float v3;
	float u4;
	float v4;
};


struct SpriteStruct
{
	ushort tpage;
	ushort offset;
	ushort width;
	ushort height;
	float x1;	//left
	float y1;	//top
	float x2;	//right
	float y2;	//bottom
};


enum MatrixIndex
{
	M00,M01,M02,M03,
	M10,M11,M12,M13,
	M20,M21,M22,M23
};


struct EffectBite
{
	int x, y, z;
	int node;
};

struct Tr4LightInfo
{
	long x;
	long y;
	long z;
	uchar r;
	uchar g;
	uchar b;
	uchar Type;
	short Intensity;
	float Inner;
	float Outer;
	float Length;
	float Cutoff;
	float nx;
	float ny;
	float nz;
};

struct Tr4MeshInfo
{
	long x;
	long y;
	long z;
	short y_rot;
	short shade;
	short Flags;
	short static_number;
};

struct PCLIGHT_INFO
{
	float x;
	float y;
	float z;
	float r;
	float g;
	float b;
	long shadow;
	float Inner;
	float Outer;
	float InnerAngle;
	float OuterAngle;
	float Cutoff;
	float nx;
	float ny;
	float nz;
	long ix;
	long iy;
	long iz;
	long inx;
	long iny;
	long inz;
	uchar Type;
	uchar Pad;
};

struct Tr4RoomInfo
{
	short* data;
	short* door;
	Tr4FloorInfo* floor;
	Tr4LightInfo* light;
	Tr4MeshInfo* mesh;
	long x;
	long y;
	long z;
	long minfloor;
	long maxceiling;
	short x_size;
	short y_size;
	long ambient;
	short num_lights;
	short num_meshes;
	uchar ReverbType;
	uchar FlipNumber;
	char MeshEffect;
	char bound_active;
	short left;
	short right;
	short top;
	short bottom;
	short test_left;
	short test_right;
	short test_top;
	short test_bottom;
	short item_number;
	short fx_number;
	short flipped_room;
	ushort flags;
	long nVerts;
	long nWaterVerts;
	long nShoreVerts;
	LPVOID SourceVB;
	short* FaceData;
	float posx;
	float posy;
	float posz;
	float* vnormals;
	float* fnormals;
	long* prelight;
	long* prelightwater;
	long watercalc;
	float* verts;
	long gt3cnt;
	long gt4cnt;
	PCLIGHT_INFO* pclight;
};

struct Tr4MeshData
{
	short x;
	short y;
	short z;
	short r;
	short flags;
	short nVerts;
	short nNorms;
	short ngt4;
	short* gt4;
	short ngt3;
	short* gt3;
	long* prelight;
	LPVOID SourceVB;
	float* Normals;
};

struct DYNAMIC
{
	long x;
	long y;
	long z;
	uchar on;
	uchar r;
	uchar g;
	uchar b;
	ushort falloff;
	uchar used;
	uchar pad1[1];
	long FalloffScale;
};

// let below pragma directive at bottom of this source
#pragma pack(2)

extern StrMyData MyData;

extern const char* LevelScriptNames[MAX_LEVEL_SCRIPTS];
extern int LevelScriptNamesCount;
extern bool GlobalScriptIntegrityDefined, GlobalScriptIntegrity, LocalScriptIntegrityDefined, LocalScriptIntegrity;
