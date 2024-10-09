#pragma once

#include "..\StdAfx.h"

#include "..\trng\bass.h"		// prototypes for extra sound library: bass.dll
#include "..\trng\Tomb_NextGeneration.h" // mnemonic constants defined in tomb_NextGeneration.dll
#include "..\definitions\structures.h" // structure of tomb4 program and trng dll
#include "..\definitions\DefTomb4Funct.h" // defines of tomb4 procedure prototypes
#include "..\definitions\macros.h"  // definition of macros
#include "..\trng\trng.h" // list of trng functions imported from trng.cpp source. 

#include <script.h>

#include "..\definitions\constants_mine.h"
#include "..\definitions\structures_mine.h"
#include "..\definitions\Tomb4Discoveries_mine.h"
#include "..\definitions\Tomb4Globals.h"
#include "..\definitions\macros_mine.h"
#include "..\particles\luabridge.h"
#include "..\particles\vector3f.h"
#include "..\particles\noise.h"
#include "..\particles\particle.h"
#include "..\particles\utilities.h"
#include "..\particles\diagnostics.h"
