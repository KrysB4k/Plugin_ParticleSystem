#pragma once
#include "structures_mine.h"

#define phd_winxmin (*(short*)(0x753C04))
#define phd_winxmax (*(short*)(0x753BEC))
#define phd_winymin (*(short*)(0x753BBC))
#define phd_winymax (*(short*)(0x753C10))
#define phd_persp (*(long*)(0x753BDC))
#define phd_zfar (*(long*)(0x753BC8))
#define phd_znear (*(long*)(0x753BF0))

#define f_persp (*(float*)(0x753C2C))
#define f_mpersp (*(float*)(0x753BA8))
#define f_moneopersp (*(float*)(0x753C24))
#define f_a (*(float*)(0x753B9C))
#define f_boo (*(float*)(0x753BF4))
#define f_centerx (*(float*)(0x753BFC))
#define f_centery (*(float*)(0x753C00))
#define f_moneoznear (*(float*)(0x753C28))
#define f_mznear (*(float*)(0x753BA4))

#define phd_mxptr (*(long**)(0x7E70C8))
#define IMptr (*(long**)(0x7FE004))
#define lara_joint_matrices ((long*)(0x80E8E0))
#define scratchpad ((char*)(0x80D6A0))
#define clipflags (*(short**)(0x753854))

#define spriteinfo (*(SpriteStruct**)(0x533994))
#define objects ((Tr4ObjectInfo*)(0x52B720))
#define items (*(Tr4ItemInfo**)(0x7FE16C))
#define anims (*(Tr4AnimStruct**)(0x533938))
#define meshes (*(short***)(0x533950))
#define bones (*(long**)(0x533958))
#define input (*(DWORD*)(0x5355D8))
#define rooms (*(Tr4RoomInfo**)(0x533934))

#define lara_info (*(Tr4LaraInfo*)(0x80DEC0))
#define lara_item (*(Tr4ItemInfo**)(0x80E01C))
#define ShatterItem (*(Tr4ShatterItem*)(0x7FE840))
#define laraspheres ((MeshSphere*)(0x7F7000))
#define itemspheres ((MeshSphere*)(0x7F6DC0))

#define wibble (*(long*)(0x4BF238))

#define GlobalAlpha (*(long*)(0x4AB7B8))

#define SmokeWindX (*(long*)(0x804D40))
#define SmokeWindZ (*(long*)(0x804D44))

#define rcossin_tbl ((short*)(0x4B34D0))

#define camera ((Tr4CameraInfo*)(0x7FE700))

#define phd_PopMatrix() (phd_mxptr -= 12)

#define level_items (*(long*)(0x4BF564))
#define number_rooms (*(short*)(0x53392C))

#define font_height (*(long*)(0x7F4F30))

#define gfFilenameOffset (*(short**)(0x7FD1CC))
#define gfFilenameWad (*(char**)(0x7FD16C))
#define gfCurrentLevel (*(char*)(0x7FD170))
#define gfLevelNames ((char*)(0x7FD1A0))
#define gfStringOffset (*(ushort**)(0x7FD154))
#define gfStringWad (*(char**)(0x7FD198))
