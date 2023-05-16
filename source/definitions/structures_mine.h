// let below pragma directive at top of this source
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

/*
typedef struct control_list
{
	int size;
	Tr4ItemInfo* nodes[CTRLPOINTS_MAX];
}control_list;*/

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
	//int wp;
	//Tr4ItemInfo* waypoints[32];
	//control_list pointlist[SEQUENCE_MAX];
	//StrGenericParameters* params[SEQUENCE_MAX];
	StrProgressiveAction VetProgrActions[MAX_MYPROGR_ACTIONS];
	StrBaseGenericCustomize BaseCustomizeMine;  // stored all your customize script commands for current level
	StrBaseGenericParameters BaseParametersMine; // store of all your parameters= script commands of current level
	StrBaseAssignSlotMine BaseAssignSlotMine; // stored all AssignSlot= command script commands of current level
}MyDataFields;
// ----------------- END PRESET STRUCTURE ZONE ---------------------------------------


// tomb4 SPARKS definition as reference
typedef struct
{
	long		x;				//00
	long		y;				//04
	long		z;				//08

	short		Xvel;			//0C
	short		Yvel;			//0E
	short		Zvel;			//10		
	short		Gravity;		//12
	short		RotAng;			//14
	short		Flags;			//16

	unsigned char	sSize;		//18
	unsigned char	dSize;		//19
	unsigned char	Size;		//1A
	unsigned char	Friction; 	//1B

	unsigned char	Scalar;		//1C
	unsigned char	Def;		//1D
	signed char		RotAdd;		//1E
	signed char		MaxYvel;	//1F

	unsigned char	On;			//20
	unsigned char	sR;			//21
	unsigned char	sG;  		//22
	unsigned char	sB;  		//23

	unsigned char	dR;			//24
	unsigned char	dG;  		//25
	unsigned char	dB;  		//26
	unsigned char	R;			//27

	unsigned char	G;  			//28
	unsigned char	B;  			//29
	unsigned char	ColFadeSpeed;	//2A
	unsigned char	FadeToBlack;	//2B

	unsigned char	sLife;			//2C
	unsigned char 	Life;			//2D
	unsigned char	TransType;		//2E
	unsigned char	extras;			//2F

	signed char		Dynamic;		//30
	unsigned char	FxObj;			//31
	unsigned char	RoomNumber; 	//32
	unsigned char	NodeNumber;		//33

} SPARKS;	//34


struct phd_vector
{
	int x, y, z;

	phd_vector(int a, int b, int c) : x(a), y(b), z(c) {}
};


struct phd_3dpos
{
	int xPos, yPos, zPos;
	short xRot, yRot, zRot;
};


struct Tr4ItemInfo
{
	int floor;
	DWORD touch_bits;
	DWORD mesh_bits;
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
	WORD box_number;
	short timer;
	short flags;
	short shade;
	short trigger_flags;
	short carried_item;
	short after_death;
	WORD fired_weapon;
	short item_flags[4];
	void* data;
	phd_3dpos pos;
	BYTE lightdata[5528];
	DWORD active : 1;
	DWORD status : 2;
	DWORD gravity_status : 1;
	DWORD hit_status : 1;
	DWORD collidable : 1;
	DWORD looked_at : 1;
	DWORD dynamic_light : 1;
	DWORD poisoned : 1;
	DWORD ai_bits : 5;
	DWORD really_active : 1;
	DWORD InDrawRoom : 1;
	DWORD meshswap_meshbits;
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
	WORD bite_offset;
	WORD loaded : 1;
	WORD intelligent : 1;
	WORD non_lot : 1;
	WORD save_position : 1;
	WORD save_hitpoints : 1;
	WORD save_flags : 1;
	WORD save_anim : 1;
	WORD semi_transparent : 1;
	WORD water_creature : 1;
	WORD using_drawanimating_item : 1;
	WORD HitEffect : 2;
	WORD undead : 1;
	WORD save_mesh : 1;
	void (*draw_routine_extra)(Tr4ItemInfo* item);
	DWORD explodable_meshbits;
	DWORD padfuck;
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

	WORD slopes_are_walls : 2;
	WORD slopes_are_pits : 1;
	WORD lava_is_pit : 1;
	WORD enable_baddie_push : 1;
	WORD enable_spaz : 1;
	WORD hit_ceiling : 1;

}; //86


struct Tr4FloorInfo // 08
{
	short index;     // 00
	short box_flags; // 02
	unsigned char pit_room;  // 04
	signed char floor;     // 05
	unsigned char sky_room;  // 06
	signed char ceiling;   // 07
};


struct Tr4AnimStruct
{
	short* frame_ptr;
	BYTE frame_rate;
	BYTE frame_size;

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

	DWORD color;
	DWORD specular;

	float tu;
	float tv;
};


struct TextureStruct
{
	WORD drawtype;
	WORD tpage;
	WORD flag;
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
	WORD tpage;
	WORD offset;
	WORD width;
	WORD height;
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


enum BlendMode
{
	POLY_TEXTURE,
	POLY_DECAL,
	POLY_COLORADD,
	POLY_SEMITRANS,
	POLY_NOZBUFFER,
	POLY_COLORSUB,
	LINE_COLORADD,
	POLY_SEMITRANS_ZBUFFER,
};


struct EffectBite
{
	int x, y, z;
	int node;
};


// let below pragma directive at bottom of this source
#pragma pack(2)