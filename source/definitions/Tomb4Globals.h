// tomb4 globals

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
#define clipflags (*(short**)(0x753854))

#define spriteinfo (*(SpriteStruct**)(0x533994))
#define objects ((Tr4ObjectInfo*)(0x52B720))
#define items (*(Tr4ItemInfo**)(0x7FE16C))
#define anims (*(Tr4AnimStruct**)0x533938)
#define effects (*(Tr4FXInfo**)0x8011C0)
#define meshes (*(short***)(0x533950))
#define input (*(DWORD*)(0x5355D8))

#define lara_item (*(Tr4ItemInfo**)(0x80E01C))
#define ShatterItem (*(Tr4ShatterItem*)(0x7FE840))
#define laraspheres ((MeshSphere*)0x7F7000)
#define itemspheres ((MeshSphere*)0x7F6DC0)

#define wibble (*(long*)0x4BF238)

#define rcossin_tbl ((short*)(0x4B34D0))

#define camera ((Tr4CameraInfo*)0x7FE700)

#define phd_PopMatrix() (phd_mxptr -= 12)

#define level_items (*(long*)0x4BF564)
#define number_rooms (*(short*)0x53392C)

#define font_height (*(long*)0x7F4F30)
