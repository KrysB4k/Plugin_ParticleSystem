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

#define MAX_SPRITEPARTS 4096
#define MAX_MESHPARTS 1024
#define MAX_PARTGROUPS 1024
#define MAX_MESHES 32
#define MAX_PERLIN 16
#define MAX_SIMPLEX 16

#define MAX_DRAWDIST 20480
#define HALF_ANGLE 32768

#define SIN_PI_6 0.5f
#define COS_PI_6 0.8660254f

// MPS_ flags
// you should type here your MPS_ flags for plugin command.
// then you'll test the presence of these flags checking the Trng.MainPluginFlags  value using "&" operator
// please: do not use the value 0x40000000 (MPS_DISABLE) because it has been already set by trng engine