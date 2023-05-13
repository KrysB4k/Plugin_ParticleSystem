#include "..\definitions\includes.h"

namespace Diagnostics
{
	double performanceMultiplier;
	int activeParticles;
	double initTime;
	double updateTime;
	double drawTime;
	double initTimePeak;
	double updateTimePeak;
	double drawTimePeak;
}

double Diagnostics::Time(void (*targetFunction)())
{
	LARGE_INTEGER startTime, endTime;

	QueryPerformanceCounter(&startTime);
	targetFunction();
	QueryPerformanceCounter(&endTime);
	return performanceMultiplier * (endTime.QuadPart - startTime.QuadPart);
}

void Diagnostics::SetPeaks()
{
	if (initTime > initTimePeak)
		initTimePeak = initTime;
	if (updateTime > updateTimePeak)
		updateTimePeak = updateTime;
	if (drawTime > drawTimePeak)
		drawTimePeak = drawTime;
}

void Diagnostics::Print()
{
	PrintString(8, 2 * font_height, 6, (char*)Script::FormatString("Init: %.1f", initTime), 0);
	PrintString(8, 3 * font_height, 6, (char*)Script::FormatString("    peak: %.1f", initTimePeak), 0);
	PrintString(8, 4 * font_height, 6, (char*)Script::FormatString("Update: %.1f", updateTime), 0);
	PrintString(8, 5 * font_height, 6, (char*)Script::FormatString("    peak: %.1f", updateTimePeak), 0);
	PrintString(8, 6 * font_height, 6, (char*)Script::FormatString("Draw: %.1f", drawTime), 0);
	PrintString(8, 7 * font_height, 6, (char*)Script::FormatString("    peak: %.1f", drawTimePeak), 0);
	PrintString(8, 10 * font_height, 6, (char*)Script::FormatString("Particles: %d", activeParticles), 0);
	PrintString(8, 11 * font_height, 6, (char*)Script::FormatString("Memory: %d", Script::GarbageCount()), 0);
}

void Diagnostics::Initialise()
{
	LARGE_INTEGER frequency;

	QueryPerformanceFrequency(&frequency);
	Diagnostics::performanceMultiplier = (double)1000 / frequency.QuadPart;
}

void Diagnostics::ResetFrame()
{
	activeParticles = 0;
	initTime = 0;
	updateTime = 0;
	drawTime = 0;
}

void Diagnostics::ResetLevel()
{
	initTimePeak = 0;
	updateTimePeak = 0;
	drawTimePeak = 0;
}

int LuaBridge::Call(const char* function)
{
	int i, init, update;
	ParticleGroup* group;
	float lower, upper;

	switch (function[0])
	{
	case 'b':
		if (!strcmp(function, "boidAlignment"))
		{
			auto part = dynamic_cast<Particle*>((LuaObject*)Script::ToData(1));
			if (!part)
				return Script::TypeError(1, "Particle");
			float radius = Script::ToNumber(2);
			float factor = Script::ToNumber(3);
			part->vel += part->BoidAlignmentRule(radius, factor);
			return 0;
		}
		if (!strcmp(function, "boidCohesion"))
		{
			auto part = dynamic_cast<Particle*>((LuaObject*)Script::ToData(1));
			if (!part)
				return Script::TypeError(1, "Particle");
			float radius = Script::ToNumber(2);
			float factor = Script::ToNumber(3);
			part->vel += part->BoidCohesionRule(radius, factor);
			return 0;
		}
		if (!strcmp(function, "boidSeparation"))
		{
			auto part = dynamic_cast<Particle*>((LuaObject*)Script::ToData(1));
			if (!part)
				return Script::TypeError(1, "Particle");
			float radius = Script::ToNumber(2);
			float factor = Script::ToNumber(3);
			part->vel += part->BoidSeparationRule(radius, factor);
			return 0;
		}
		break;
	case 'c':
		if (!strcmp(function, "cos"))
		{
			Script::PushNumber(cosf(Script::ToNumber(1)));
			return 1;
		}
		if (!strcmp(function, "createGroup"))
		{
			if (ParticleFactory::caller != FUNCTION_LIBRARY)
				return 0;
			init = Script::StoreFunction(1);
			update = Script::StoreFunction(2);
			i = ParticleFactory::GetFreeParticleGroup();
			if (i == -1)
				return 0;
			ParticleFactory::partGroups[i].initIndex = init;
			ParticleFactory::partGroups[i].updateIndex = update;
			Script::PushData(&ParticleFactory::partGroups[i]);
			return 1;
		}
		if (!strcmp(function, "createParticle"))
		{
			if (ParticleFactory::caller != FUNCTION_INIT && ParticleFactory::caller != FUNCTION_UPDATE)
				return 0;
			group = dynamic_cast<ParticleGroup*>((LuaObject*)Script::ToData(1));
			if (!group)
				return Script::TypeError(1, "ParticleGroup");
			i = ParticleFactory::GetFreeParticle();
			ParticleFactory::parts[i].groupIndex = group->groupIndex;
			Script::PushData(&ParticleFactory::parts[i]);
			return 1;
		}
		break;
	case 'g':
		if (!strcmp(function, "getTombIndex"))
		{
			Script::PushInteger(FromNgleIndexToTomb4Index(Script::ToInteger(1)));
			return 1;
		}
		if (!strcmp(function, "getLaraIndex"))
		{
			Script::PushInteger(*Trng.pGlobTomb4->pAdr->pLaraIndex);
			return 1;
		}
		if (!strcmp(function, "getItemRoom"))
		{
			int index = Script::ToInteger(1);
			if (index >= 0 && index < level_items)
			{
				auto item = &items[index];
				Script::PushInteger(item->room_number);
				return 1;
			}
			return 0;
		}
		break;
	case 'p':
		if (!strcmp(function, "print"))
		{
			Script::Print();
			return 0;
		}
		if (!strcmp(function, "particleCollideFloors"))
		{
			auto part = dynamic_cast<Particle*>((LuaObject*)Script::ToData(1));
			if (!part)
				return Script::TypeError(1, "Particle");
			float rebound = Script::ToNumber(2);
			float minbounce = Script::ToNumber(3);
			int margin = Script::ToInteger(4);
			bool accurate = Script::ToBoolean(5);
			Script::PushBoolean(part->ParticleCollideFloors(rebound, minbounce, margin, accurate));
			return 1;
		}
		if (!strcmp(function, "particleCollideWalls"))
		{
			auto part = dynamic_cast<Particle*>((LuaObject*)Script::ToData(1));
			if (!part)
				return Script::TypeError(1, "Particle");
			float rebound = Script::ToNumber(2);
			Script::PushBoolean(part->ParticleCollideWalls(rebound));
			return 1;
		}
		if (!strcmp(function, "particleHoming"))
		{
			auto part = dynamic_cast<Particle*>((LuaObject*)Script::ToData(1));
			if (!part)
				return Script::TypeError(1, "Particle");
			int index = Script::ToInteger(2);
			int node = Script::ToInteger(3);
			float factor = Script::ToNumber(4);
			float accel = Script::ToNumber(5);
			bool predict = Script::ToBoolean(6);

			if (index >= 0 && index < level_items)
			{
				auto item = &items[index];
				part->ParticleHoming(item, node, factor, accel, predict);
			}
			
			return 0;
		}
		if (!strcmp(function, "particleLimitSpeed"))
		{
			auto part = dynamic_cast<Particle*>((LuaObject*)Script::ToData(1));
			if (!part)
				return Script::TypeError(1, "Particle");
			float speedMax = Script::ToNumber(2);
			part->ParticleLimitSpeed(speedMax);
			return 0;
		}
		break;
	case 'r':
		if (!strcmp(function, "randfloat"))
		{
			lower = Script::ToNumber(1);
			upper = Script::ToNumber(2);
			Script::PushNumber(fabsf(upper - lower) * GetRandom() + fminf(lower, upper));
			return 1;
		}
		if (!strcmp(function, "randint"))
		{
			lower = Script::ToInteger(1);
			upper = Script::ToInteger(2);
			Script::PushInteger(floorf((fabsf(upper - lower) + 1) * GetRandom() + fminf(lower, upper)));
			return 1;
		}
		break;
	case 's':
		if (!strcmp(function, "sin"))
		{
			Script::PushNumber(sinf(Script::ToNumber(1)));
			return 1;
		}
		break;
	}
	return 0;
}

int ColorRGB::Index(const char* field)
{
	switch (field[0])
	{
	case 'b':
		if (!strcmp(field, "b"))
		{
			Script::PushNumber((float)B / 255);
			return 1;
		}
		break;
	case 'g':
		if (!strcmp(field, "g"))
		{
			Script::PushNumber((float)G / 255);
			return 1;
		}
		break;
	case 'r':
		if (!strcmp(field, "r"))
		{
			Script::PushNumber((float)R / 255);
			return 1;
		}
		break;
	}
	return 0;
}

void ColorRGB::NewIndex(const char* field)
{
	switch (field[0])
	{
	case 'b':
		if (!strcmp(field, "b"))
			B = Clamp(int(255 * Script::ToNumber(1)), 0, 255);
		break;
	case 'g':
		if (!strcmp(field, "g"))
			G = Clamp(int(255 * Script::ToNumber(1)), 0, 255);
		break;
	case 'r':
		if (!strcmp(field, "r"))
			R = Clamp(int(255 * Script::ToNumber(1)), 0, 255);
		break;
	}
}

int NodeAttachment::Index(const char* field)
{
	switch (field[0])
	{
	case 'c':
		if (!strcmp(field, "cutoff"))
		{
			Script::PushInteger(cutoff);
			return 1;
		}
		break;
	case 'o':
		if (!strcmp(field, "offX"))
		{
			Script::PushInteger(offX);
			return 1;
		}
		if (!strcmp(field, "offY"))
		{
			Script::PushInteger(offY);
			return 1;
		}
		if (!strcmp(field, "offZ"))
		{
			Script::PushInteger(offZ);
			return 1;
		}
		break;
	case 'r':
		if (!strcmp(field, "random"))
		{
			Script::PushInteger(random);
			return 1;
		}
		break;
	}
	return 0;
}

void NodeAttachment::NewIndex(const char* field)
{
	switch (field[0])
	{
	case 'c':
		if (!strcmp(field, "cutoff"))
			cutoff = Clamp(Script::ToInteger(1), 0, 32767);
		break;
	case 'o':
		if (!strcmp(field, "offX"))
			offX = Script::ToInteger(1);
		else if (!strcmp(field, "offY"))
			offY = Script::ToInteger(1);
		else if (!strcmp(field, "offZ"))
			offZ = Script::ToInteger(1);
		break;
	case 'r':
		if (!strcmp(field, "random"))
			random = Clamp(Script::ToInteger(1), 0, 32767);
		break;
	}
}

int ParticleGroup::Index(const char* field)
{
	switch (field[0])
	{
	case 'b':
		if (!strcmp(field, "blendingMode"))
		{
			Script::PushInteger(blendingMode);
			return 1;
		}
		break;
	case 'l':
		if (!strcmp(field, "lineIgnoreVel"))
		{
			Script::PushBoolean(LineIgnoreVel);
			return 1;
		}
		break;
	case 'n':
		if (!strcmp(field, "noPerspective"))
		{
			Script::PushBoolean(NoPerspective);
			return 1;
		}
		break;
	case 's':
		if (!strcmp(field, "saved"))
		{
			Script::PushBoolean(Saved);
			return 1;
		}
		if (!strcmp(field, "spriteSlot"))
		{
			Script::PushInteger(spriteSlot);
			return 1;
		}
		break;
	case 'w':
		if (!strcmp(field, "windAffected"))
		{
			Script::PushBoolean(WindAffected);
			return 1;
		}
		break;
	}
	return 0;
}

void ParticleGroup::NewIndex(const char* field)
{
	if (ParticleFactory::caller != FUNCTION_LIBRARY)
		return;
	switch (field[0])
	{
	case 'b':
		if (!strcmp(field, "blendingMode"))
			blendingMode = Clamp(Script::ToInteger(1), 0, 13);
		break;
	case 'l':
		if (!strcmp(field, "lineIgnoreVel"))
			LineIgnoreVel = Script::ToBoolean(1);
		break;
	case 'n':
		if (!strcmp(field, "noPerspective"))
			NoPerspective = Script::ToBoolean(1);
		break;
	case 's':
		if (!strcmp(field, "saved"))
			Saved = Script::ToBoolean(1);
		else if (!strcmp(field, "spriteSlot"))
		{
			spriteSlot = Script::ToInteger(1);
			if (spriteSlot != SLOT_DEFAULT_SPRITES && spriteSlot != SLOT_MISC_SPRITES && spriteSlot != SLOT_CUSTOM_SPRITES)
				spriteSlot = SLOT_DEFAULT_SPRITES;
		}
		break;
	case 'w':
		if (!strcmp(field, "windAffected"))
			WindAffected = Script::ToBoolean(1);
		break;
	}
}

// ************  namespace ParticleFactory  ****************

namespace ParticleFactory
{
	int nextPart;
	Particle parts[MAX_PARTS];
	int nextPartGroup;
	ParticleGroup partGroups[MAX_PARTGROUPS];
	FunctionType caller;
};


int ParticleFactory::GetFreeParticle()
{
	int i, free;
	Particle* part;
	
	for (part = &parts[nextPart], free = nextPart, i = 0; i < MAX_PARTS; ++i)
	{
		if (!part->lifeCounter)
		{
			nextPart = (free + 1) % MAX_PARTS;

			parts[free] = Particle();
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

	nextPart = (free + 1) % MAX_PARTS;

	parts[free] = Particle();
	parts[free].emitterIndex = -1;
	parts[free].emitterNode = -1;
	return free;
}

int ParticleFactory::GetFreeParticleGroup()
{
	int free;

	free = nextPartGroup;
	if (free < MAX_PARTGROUPS)
	{
		nextPartGroup++;
		partGroups[free] = ParticleGroup();
		partGroups[free].groupIndex = free;
		partGroups[free].blendingMode = 2;
		partGroups[free].spriteSlot = SLOT_DEFAULT_SPRITES;
		return free;
	}
	return -1;
}

void ParticleFactory::ClearParts()
{
	for (int i = 0; i < MAX_PARTS; i++)
		parts[i] = Particle();
	nextPart = 0;
}

void ParticleFactory::ClearPartGroups()
{
	for (int i = 0; i < MAX_PARTGROUPS; i++)
		partGroups[i] = ParticleGroup();
	nextPartGroup = 0;
}

void ParticleFactory::UpdateParts()
{
	Particle* part = &ParticleFactory::parts[0];

	ParticleFactory::caller = FUNCTION_UPDATE;
	Script::PreFunctionLoop();
	for (int i = 0; i < MAX_PARTS; ++i, ++part)
	{
		if (part->lifeCounter <= 0)
			continue;

		const auto &pgroup = ParticleFactory::partGroups[part->groupIndex];

		if (part->emitterIndex >= 0 || part->emitterNode >= 0)
		{
			int cutoff = -1;
			// particle attachment cutoff 
			if (pgroup.attach.cutoff > 0)
			{
				cutoff = pgroup.attach.cutoff;
				if (pgroup.attach.random > 1)
					cutoff += (GetRandomDraw() % pgroup.attach.random);
			}

			if ((part->lifeSpan - part->lifeCounter) > cutoff)
				part->ParticleDeattach();
		}


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

		float t = Clamp(lifefactor / float(fadetime), 0.0f, 1.0f);
		part->colCust = Lerp(part->colStart, part->colEnd, t);

		t = part->Parameter();
		part->sizeCust = Round(Lerp(float(part->sizeStart), float(part->sizeEnd), t));

		Script::ExecuteFunction(pgroup.updateIndex, part);
		
		part->vel += part->accel;
		part->pos += part->vel;
		part->rot += part->rotVel;

		--part->lifeCounter;
	}
	Script::PostFunctionLoop();
}


void ParticleFactory::DrawParts()
{
	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.xPos, lara_item->pos.yPos, lara_item->pos.zPos);

	long* mptr = phd_mxptr;

	Particle* part = &ParticleFactory::parts[0];

	int x1 = 0, y1 = 0, z1 = 0;
	for (int i = 0; i < MAX_PARTS; ++i, ++part)
	{
		if (part->lifeCounter <= 0)
			continue;

		Diagnostics::activeParticles++;

		auto partPos = part->ParticleAbsPos();

		x1 = Round(partPos.x);
		y1 = Round(partPos.y);
		z1 = Round(partPos.z);

		const auto &pgroup = ParticleFactory::partGroups[part->groupIndex];

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
		if (pgroup.spriteSlot <= 0)
		{
			float size = part->sizeCust;
			auto vel = part->vel;

			if (pgroup.LineIgnoreVel)
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
		part->DrawParticle(pgroup, viewCoords, minSize);
	}

	phd_PopMatrix();
}

void ParticleFactory::InitParts()
{
	ParticleFactory::caller = FUNCTION_INIT;
	Script::PreFunctionLoop();
	for (int i = 0; i < nextPartGroup; i++)
		Script::ExecuteFunction(partGroups[i].initIndex, nullptr);
	Script::PostFunctionLoop();
}

void ParticleFactory::InitPartGroups()
{
	ParticleFactory::caller = FUNCTION_LIBRARY;
	Script::PreFunctionLoop();
	Script::LoadFunctions("EffectsLibrary.lua");
	Script::PostFunctionLoop();
}

// ************  Particle public methods  ****************

int Particle::Index(const char* field)
{
	switch (field[0])
	{
	case 'a':
		if (!strcmp(field, "accel"))
		{
			Script::PushData(&accel);
			return 1;
		}
		break;
	case 'c':
		if (!strcmp(field, "colCust"))
		{
			Script::PushData(&colCust);
			return 1;
		}
		if (!strcmp(field, "colEnd"))
		{
			Script::PushData(&colEnd);
			return 1;
		}
		if (!strcmp(field, "colStart"))
		{
			Script::PushData(&colStart);
			return 1;
		}
		if (!strcmp(field, "colorFadeTime"))
		{
			Script::PushInteger(colorFadeTime);
			return 1;
		}
		break;
	case 'e':
		if (!strcmp(field, "emitterIndex"))
		{
			Script::PushInteger(emitterIndex);
			return 1;
		}
		if (!strcmp(field, "emitterNode"))
		{
			Script::PushInteger(emitterNode);
			return 1;
		}
		break;
	case 'f':
		if (!strcmp(field, "fadeIn"))
		{
			Script::PushInteger(fadeIn);
			return 1;
		}
		if (!strcmp(field, "fadeOut"))
		{
			Script::PushInteger(fadeOut);
			return 1;
		}
		break;
	case 'l':
		if (!strcmp(field, "lifeCounter"))
		{
			Script::PushInteger(lifeCounter);
			return 1;
		}
		if (!strcmp(field, "lifeSpan"))
		{
			Script::PushInteger(lifeSpan);
			return 1;
		}
		break;
	case 'p':
		if (!strcmp(field, "pos"))
		{
			Script::PushData(&pos);
			return 1;
		}
		break;
	case 'r':
		if (!strcmp(field, "roomIndex"))
		{
			Script::PushInteger(roomIndex);
			return 1;
		}
		if (!strcmp(field, "rot"))
		{
			Script::PushNumber(ShortToRad(rot));
			return 1;
		}
		if (!strcmp(field, "rotVel"))
		{
			Script::PushNumber(ShortToRad(rotVel));
			return 1;
		}
		break;
	case 's':
		if (!strcmp(field, "sizeCust"))
		{
			Script::PushInteger(sizeCust);
			return 1;
		}
		if (!strcmp(field, "sizeEnd"))
		{
			Script::PushInteger(sizeEnd);
			return 1;
		}
		if (!strcmp(field, "sizeStart"))
		{
			Script::PushInteger(sizeStart);
			return 1;
		}
		if (!strcmp(field, "spriteIndex"))
		{
			Script::PushInteger(spriteIndex);
			return 1;
		}
		break;
	case 'v':
		if (!strcmp(field, "vel"))
		{
			Script::PushData(&vel);
			return 1;
		}
		break;
	}
	return 0;
}

void Particle::NewIndex(const char* field)
{
	if (ParticleFactory::caller != FUNCTION_INIT && ParticleFactory::caller != FUNCTION_UPDATE)
		return;
	switch (field[0])
	{
	case 'c':
		if (!strcmp(field, "colorFadeTime"))
			colorFadeTime = Clamp(Script::ToInteger(1), -32768, 32767);
		break;
	case 'e':
		if (!strcmp(field, "emitterIndex"))
		{
			emitterIndex = Clamp(Script::ToInteger(1), -1, level_items - 1);
			if (emitterIndex != -1)
				emitterNode = Clamp(emitterNode, -1, objects[items[emitterIndex].object_number].nmeshes - 1);
			else
				emitterNode = -1;
		}
		else if (!strcmp(field, "emitterNode"))
		{
			if (emitterIndex != -1)
				emitterNode = Clamp(Script::ToInteger(1), -1, objects[items[emitterIndex].object_number].nmeshes - 1);
		}
		break;
	case 'f':
		if (!strcmp(field, "fadeIn"))
			fadeIn = Clamp(Script::ToInteger(1), 0, 255);
		else if (!strcmp(field, "fadeOut"))
			fadeOut = Clamp(Script::ToInteger(1), 0, 255);
		break;
	case 'l':
		if (!strcmp(field, "lifeCounter"))
			lifeCounter = Clamp(Script::ToInteger(1), 0, lifeSpan);
		else if (!strcmp(field, "lifeSpan"))
		{
			lifeSpan = Clamp(Script::ToInteger(1), 0, 32767);
			lifeCounter = Clamp(lifeCounter, 0, lifeSpan);
		}
		break;
	case 'r':
		if (!strcmp(field, "roomIndex"))
			roomIndex = Clamp(Script::ToInteger(1), 0, number_rooms - 1);
		else if (!strcmp(field, "rot"))
			rot = RadToShort(Script::ToNumber(1));
		else if (!strcmp(field, "rotVel"))
			rotVel = RadToShort(Script::ToNumber(1));
		break;
	case 's':
		if (!strcmp(field, "sizeCust"))
			sizeCust = Clamp(Script::ToInteger(1), 0, 65535);
		else if (!strcmp(field, "sizeEnd"))
			sizeEnd = Clamp(Script::ToInteger(1), 0, 65535);
		else if (!strcmp(field, "sizeStart"))
			sizeStart = Clamp(Script::ToInteger(1), 0, 65535);
		else if (!strcmp(field, "spriteIndex"))
			spriteIndex = Clamp(Script::ToInteger(1), 0, 255);
		break;
	}
}

float Particle::Parameter()
{
	return (lifeSpan - lifeCounter) / float(lifeSpan);
}


void Particle::ParticleLimitSpeed(float speedMax)
{
	float speed = vel.magnitude();
	if (speed > speedMax)
		vel *= (speedMax/speed);
}


void Particle::ParticleAnimate(unsigned int startSprite, unsigned int endSprite, int frameRate)
{
	int length = endSprite - startSprite;
	if (!frameRate || length <= 0)
		return;

	int usedSprite = startSprite;

	if (frameRate > 0)
		usedSprite += ((lifeSpan - lifeCounter) / frameRate) % (length + 1);
	else
	{
		// repeat animation cycle (-framerate) times through particle lifespan
		int numCycles = -frameRate;
		usedSprite = int(Lerp(float(usedSprite), float(usedSprite + length), fmod(numCycles * Parameter(), 1.0f)));
	}

	spriteIndex = (uchar)(usedSprite);
}


Vector3f Particle::ParticleAbsPos()
{
	auto absPos = pos;

	if (emitterIndex >= 0 || emitterNode >= 0)
	{
		auto item = lara_item;
		if (emitterIndex >= 0 && emitterIndex < Trng.pGlobTomb4->pAdr->TotItemsMax)
			item = &items[emitterIndex];

		Vector3f relPos;

		if (emitterNode >= 0) // if attached to specific mesh node of item
		{	
			const auto &pgroup = ParticleFactory::partGroups[groupIndex];

			relPos.x = pgroup.attach.offX;
			relPos.y = pgroup.attach.offY;
			relPos.z = pgroup.attach.offZ;

			int node = emitterNode & 0x1F;

			relPos = GetJointPos(item, node, int(relPos.x), int(relPos.y), int(relPos.z));
		}
		else if (emitterIndex >= 0) // no mesh node, use item's pos
		{
			relPos.x = item->pos.xPos;
			relPos.y = item->pos.yPos;
			relPos.z = item->pos.zPos;
		}

		absPos += relPos;
	}

	return absPos;
}


void Particle::ParticleAttach(int itemIndex, int node)
{
	auto item = lara_item;
	if (itemIndex >= 0 && itemIndex < Trng.pGlobTomb4->pAdr->TotItemsMax)
		item = &items[itemIndex];
	else
		itemIndex = -1;

	Vector3f relPos;

	if (node >= 0) // if attached to specific mesh node of item
	{	
		const auto &pgroup = ParticleFactory::partGroups[groupIndex];

		relPos.x = pgroup.attach.offX;
		relPos.y = pgroup.attach.offY;
		relPos.z = pgroup.attach.offZ;

		node = node & 0x1F;

		if (item == lara_item)
			node = (node < 14) ? node : 14;

		relPos = GetJointPos(item, node, int(relPos.x), int(relPos.y), int(relPos.z));
	}
	else if (emitterIndex >= 0) // no mesh node, use item's pos
	{
		node = -1;
		relPos.x = item->pos.xPos;
		relPos.y = item->pos.yPos;
		relPos.z = item->pos.zPos;
	}

	emitterIndex = itemIndex;
	emitterNode = node;
	pos = (ParticleAbsPos() - relPos);
}


void Particle::ParticleDeattach()
{
	pos = ParticleAbsPos();
	emitterIndex = -1;
	emitterNode = -1;
}


bool Particle::ParticleCollideWalls(float rebound)
{
	if (emitterIndex < 0 && emitterNode < 0)
	{
		if (rebound < 0)
			rebound = 1.0f;

		auto testp = Round(pos + vel);
		auto p = Round(pos);

		if ((p.x >> 10) != (testp.x >> 10) || (p.z >> 10) != (testp.z >> 10))
		{
			short tRoom = roomIndex;
			int wallCurrentPos = TestForWall(p.x, p.y, p.z, &tRoom);

			if (roomIndex != tRoom)
				roomIndex = tRoom;

			if (!wallCurrentPos)
			{
				bool wallCollision = false;

				if (TestForWall(p.x, p.y, testp.z, &tRoom))
				{
					vel.z = -(vel.z * rebound);
					wallCollision = true;
				}
				if (TestForWall(testp.x, p.y, p.z, &tRoom))
				{
					vel.x = -(vel.x * rebound);
					wallCollision = true;
				}

				return wallCollision;
			}
		}
	}

	return false;
}


bool Particle::ParticleCollideFloors(float rebound, float minBounce, int collMargin, bool accurate)
{
	if (emitterIndex < 0 && emitterNode < 0)
	{
		if (rebound < 0)
			rebound = 1.0f;

		auto testp = Round(pos + vel);
		short tRoom = roomIndex;

		Tr4FloorInfo* floor = (Tr4FloorInfo*)GetFloor(testp.x, testp.y, testp.z, &tRoom);

		if (roomIndex != tRoom)
			roomIndex = tRoom;

		int height = GetHeight(floor, testp.x, testp.y, testp.z);
		int fh = height - collMargin;
		int ch = GetCeiling(floor, testp.x, testp.y, testp.z);

		if (height != (-0x7F00) && (testp.y >= fh || testp.y <= ch))
		{
			if (abs(vel.y) > minBounce)
			{
				if (testp.y > fh && accurate)
				{
					auto n = GetSlopeNormal(floor, testp.x, testp.y, testp.z);

					// get reflection vector off slope surface
					auto reflected = vel - (n * vel.dot(n) * 2);
					vel = reflected * rebound;
				}
				else
					vel.y = -(vel.y * rebound);

			}
			else if (testp.y >= fh)
				pos.y = fh;

			if (pos.y > fh)
				pos.y = fh;
			else if (pos.y < ch)
				pos.y = ch;

			return true;
		}
	}

	return false;
}


Vector3f Particle::ParticleFollow(const Vector3f& v, float factor, float maxSpeed)
{
	auto dirVect = v - pos;
	if (dirVect.magnitude() > maxSpeed)
		dirVect *= maxSpeed / dirVect.magnitude();

	return dirVect * factor;
}


bool Particle::ParticleHoming(Tr4ItemInfo* item, int targetNode, float homingFactor, float homingAccel, bool predict)
{
	if (!item)
		return false;

	targetNode = Clamp(targetNode, 0, objects[item->object_number].nmeshes);

	auto targetPos = GetJointPos(item, targetNode, 0, 0, 0);

	if (predict)
	{
		float s = sin(RAD(item->pos.yRot));
		float c = cos(RAD(item->pos.yRot));

		Vector3f heading(item->speed * s, item->fallspeed, item->speed * c);

		float time = RealDist(pos, targetPos) / vel.magnitude();

		targetPos += (heading * time);
	}

	auto dir = (targetPos - pos).normalized();

	if (homingFactor < 0)
	{
		dir = -dir;
		homingFactor = -homingFactor;
	}

	auto slerpVel = vel.normalized().slerp(dir, homingFactor);
	vel = slerpVel * (vel.magnitude() + homingAccel);

	return true;
}


Vector3f Particle::BoidSeparationRule(float radius, float factor)
{
	Vector3f v;

	for (int i = 0; i < MAX_PARTS; ++i)
	{
		auto part = &ParticleFactory::parts[i];
		if (part->lifeCounter <= 0 || part == this)
			continue;

		if (part->groupIndex != groupIndex)
			continue;

		if (SimpleDist(pos, part->pos) < radius)
		{
			if (CheckDistFast(pos, part->pos, radius) < 0)
				v -= (part->pos - pos);
		}
	}

	return v * factor;
}


Vector3f Particle::BoidCohesionRule(float radius, float factor)
{
	Vector3f v;

	int neighbors = 0;
	for (int i = 0; i < MAX_PARTS; ++i)
	{
		auto part = &ParticleFactory::parts[i];
		if (part->lifeCounter <= 0 || part == this)
			continue;

		if (part->groupIndex != groupIndex)
			continue;

		if (SimpleDist(pos, part->pos) < radius)
		{
			if (CheckDistFast(pos, part->pos, radius) < 0)
			{
				v += part->pos;
				++neighbors;
			}
		}
	}

	if (neighbors)
	{
		v *= (1.0f / neighbors);
		v = (v - pos) * factor;
	}

	return v;
}


Vector3f Particle::BoidAlignmentRule(float radius, float factor)
{
	Vector3f v;

	int neighbors = 0;
	for (int i = 0; i < MAX_PARTS; ++i)
	{
		auto part = &ParticleFactory::parts[i];
		if (part->lifeCounter <= 0 || part == this)
			continue;

		if (part->groupIndex != groupIndex)
			continue;

		if (SimpleDist(pos, part->pos) < radius)
		{
			if (CheckDistFast(pos, part->pos, radius) < 0)
			{
				v += part->vel;
				++neighbors;
			}
		}
	}

	if (neighbors)
	{
		v *= (1.0f / neighbors);
		v = (v - vel) * factor;
	}

	return v;
}


void Particle::DrawParticle(const ParticleGroup& pgroup, long* const view, long smallest_size)
{
	long z1 = view[2];
	
	if (z1 <= 0)
		return;
	if (z1 >= 0x5000)
		return;


	int cR = colCust.R;
	int cG = colCust.G;
	int cB = colCust.B;	

	if (fadeIn)
	{
		int lifeDif = lifeSpan - lifeCounter;
		if (lifeDif < fadeIn)
		{
			float s = lifeDif/float(fadeIn);
			cR = Round(Lerp(0, cR, s));
			cG = Round(Lerp(0, cG, s));
			cB = Round(Lerp(0, cB, s));
		}
	}

	if (fadeOut)
	{
		if (lifeCounter < fadeOut)
		{
			float s = lifeCounter/float(fadeOut);
			cR = Round(Lerp(0, cR, s));
			cG = Round(Lerp(0, cG, s));
			cB = Round(Lerp(0, cB, s));
		}
	}


	int spriteSlot = -1;

	spriteSlot = pgroup.spriteSlot;

	if (spriteSlot > 0)
	{
		int s1, s2;

		if (pgroup.NoPerspective)
			s1 = s2 = sizeCust;
		else
		{
			s1 = Round(phd_persp * sizeCust / z1);

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

			if (rot)
			{
				float sin1 = sin(RAD(rot));
				float cos1 = cos(RAD(rot));

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
			
			
			if ((spriteSlot != SLOT_MISC_SPRITES && spriteSlot != SLOT_CUSTOM_SPRITES) ||
				!objects[spriteSlot].loaded)
			{
				spriteSlot = SLOT_DEFAULT_SPRITES;
			}

			SpriteStruct* sprite = (spriteinfo + objects[spriteSlot].mesh_index + spriteIndex);

			TextureStruct tex;

			tex.drawtype = pgroup.blendingMode;

			tex.tpage = sprite->tpage;
			tex.u1 =	sprite->x1;
			tex.v1 =	sprite->y1;
			tex.u2 =	sprite->x2;
			tex.v2 =	sprite->y1;
			tex.u3 =	sprite->x2;
			tex.v3 =	sprite->y2;
			tex.u4 =	sprite->x1;
			tex.v4 =	sprite->y2;

			(*AddQuadSorted)(v, 0, 1, 2, 3, &tex, 0);
		}
	}
	else
	{
		long x1 = view[0];
		long y1 = view[1];
		long z1 = view[2];
		long x2 = view[3];
		long y2 = view[4];
		long z2 = view[5];
		
		if (ClipLine(x1, y1, z1, x2, y2, z2, phd_winxmin, phd_winymin, phd_winxmax, phd_winymax))
		{
			D3DTLVERTEX v[2];

			long c1 = RGBA(cR, cG, cB, 0xFF);
			long c2 = RGBA(cR >> 2, cG >> 2, cB >> 2, 0xFF);

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
