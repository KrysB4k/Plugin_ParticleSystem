#define MAX_MYPROGR_ACTIONS 100
#define MAX_ASSIGN_SLOT_MINE 200

#define ENDLESS_DURATE 0xFFFF // to use as number of frames to set an infinite durate
#define NGTAG_END_SEQUENCE		   0   // signals the end of ng token sequence
#define NGTAG_PROGRESSIVE_ACTIONS  1
#define NGTAG_LOCAL_DATA           2
#define NGTAG_GLOBAL_DATA          3
#define NGTAG_PARTICLES			   4
#define AXN_FREE  0  // this record is to free an action record. You type this value in ActionType to free a action progress record
// --------- END PRESET CONSTANTS ---------------------------------
// type here the constant name for new progressive action you create
// use progressive number to have always different value for each AXN_ constant/action

#define MAX_PARTS 2048
#define MAX_PARTGROUPS 256
#define MAX_PROJECTILES 256
#define MAX_MESHES 32

#define HALF_ANGLE 32768

#define phd_winxmin *(short*)(0x753C04)
#define phd_winxmax *(short*)(0x753BEC)
#define phd_winymin *(short*)(0x753BBC)
#define phd_winymax *(short*)(0x753C10)
#define phd_persp *(long*)(0x753BDC)
#define phd_zfar *(long*)(0x753BC8)
#define phd_znear *(long*)(0x753BF0)

#define f_persp *(float*)(0x753C2C)
#define f_mpersp *(float*)(0x753BA8)
#define f_moneopersp *(float*)(0x753C24)
#define f_a *(float*)(0x753B9C)
#define f_boo *(float*)(0x753BF4)
#define f_centerx *(float*)(0x753BFC)
#define f_centery *(float*)(0x753C00)
#define f_moneoznear *(float*)(0x753C28)
#define f_mznear *(float*)(0x753BA4)

#define phd_mxptr (*(long**)(0x7E70C8))
#define scratchpad (char*)(0x80D6A0)

#define spriteinfo (*(SpriteStruct**)(0x533994))
#define objects ((Tr4ObjectInfo*)(0x52B720))
#define items (*(Tr4ItemInfo**)(0x7FE16C))
#define anims (*(Tr4AnimStruct**)0x533938)
#define effects (*(Tr4FXInfo**)0x8011C0)
#define meshes (*(short***)(0x533950))

#define ShatterItem (*(Tr4ShatterItem*)(0x7FE840))


#define wibble (*(long*)0x4BF238)

#define clipflags (*(short**)(0x753854))
#define rcossin_tbl ((short*)(0x4B34D0))

#define lara_item (*(Tr4ItemInfo**)(0x80E01C))
#define lara_gun_type (*(short*)(0x80DEC4))
#define lara_gun_status (*(short*)(0x80DEC2))
#define lara_left_arm (*(Tr4LaraArm*)(0x80DF72))
#define lara_general_ptr (*(DWORD*)(0x80DFC0))
#define input (*(DWORD*)(0x5355D8))

#define laraspheres ((MeshSphere*)0x7F7000)
#define itemspheres ((MeshSphere*)0x7F6DC0)

#define camera ((Tr4CameraInfo*)0x7FE700)

#define	SP_DEADLYFLAME	1
#define	SP_SCALE		2
#define	SP_DEADLYFIRE	4
#define	SP_DEF		8
#define	SP_ROTATE		16
#define	SP_KEEP		32
#define	SP_FX		64
#define	SP_ITEM		128
#define	SP_WIND		256
#define	SP_EXPDEF		512
#define	SP_DAMAGE		1024
#define	SP_UNDERWEXP	2048
#define	SP_NODEATTATCH	4096


#define phd_PopMatrix() (phd_mxptr -= 12)

// MPS_ flags
// you should type here your MPS_ flags for plugin command.
// then you'll test the presence of these flags checking the Trng.MainPluginFlags  value using "&" operator
// please: do not use the value 0x40000000 (MPS_DISABLE) because it has been already set by trng engine