#include "..\definitions\includes.h"

namespace Diagnostics
{
	double performanceMultiplier;
	int activeSpriteParticles;
	int activeMeshParticles;
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
	PrintString(8, 10 * font_height, 6, (char*)Script::FormatString("Sprite Particles: %d", activeSpriteParticles), 0);
	PrintString(8, 11 * font_height, 6, (char*)Script::FormatString("Mesh Particles: %d", activeMeshParticles), 0);
	PrintString(8, 12 * font_height, 6, (char*)Script::FormatString("Memory: %d", Script::GarbageCount()), 0);
}

void Diagnostics::Initialise()
{
	LARGE_INTEGER frequency;

	QueryPerformanceFrequency(&frequency);
	Diagnostics::performanceMultiplier = (double)1000 / frequency.QuadPart;
}

void Diagnostics::ResetFrame()
{
	activeSpriteParticles = 0;
	activeMeshParticles = 0;
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
			auto part = dynamic_cast<BaseParticle*>((LuaObject*)Script::ToData(1));
			if (!part)
				return Script::TypeError(1, "Particle");
			float radius = Script::ToNumber(2);
			float factor = Script::ToNumber(3);
			part->vel += part->BoidAlignmentRule(radius, factor);
			return 0;
		}
		if (!strcmp(function, "boidCohesion"))
		{
			auto part = dynamic_cast<BaseParticle*>((LuaObject*)Script::ToData(1));
			if (!part)
				return Script::TypeError(1, "Particle");
			float radius = Script::ToNumber(2);
			float factor = Script::ToNumber(3);
			part->vel += part->BoidCohesionRule(radius, factor);
			return 0;
		}
		if (!strcmp(function, "boidSeparation"))
		{
			auto part = dynamic_cast<BaseParticle*>((LuaObject*)Script::ToData(1));
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
			i = ParticleFactory::GetFreeSpritePart();
			ParticleFactory::spriteParts[i].groupIndex = group->groupIndex;
			Script::PushData(&ParticleFactory::spriteParts[i]);
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
		if (!strcmp(function, "particleAnimate"))
		{
			auto part = dynamic_cast<BaseParticle*>((LuaObject*)Script::ToData(1));
			if (!part)
				return Script::TypeError(1, "Particle");
			int start = Script::ToInteger(2);
			int end = Script::ToInteger(3);
			int framerate = Script::ToInteger(4);
			part->Animate(start, end, framerate);
			return 0;
		}
		if (!strcmp(function, "particleCollideFloors"))
		{
			auto part = dynamic_cast<BaseParticle*>((LuaObject*)Script::ToData(1));
			if (!part)
				return Script::TypeError(1, "Particle");
			float rebound = Script::ToNumber(2);
			float minbounce = Script::ToNumber(3);
			int margin = Script::ToInteger(4);
			bool accurate = Script::ToBoolean(5);
			Script::PushBoolean(part->CollideFloors(rebound, minbounce, margin, accurate));
			return 1;
		}
		if (!strcmp(function, "particleCollideWalls"))
		{
			auto part = dynamic_cast<BaseParticle*>((LuaObject*)Script::ToData(1));
			if (!part)
				return Script::TypeError(1, "Particle");
			float rebound = Script::ToNumber(2);
			Script::PushBoolean(part->CollideWalls(rebound));
			return 1;
		}
		if (!strcmp(function, "particleHoming"))
		{
			auto part = dynamic_cast<BaseParticle*>((LuaObject*)Script::ToData(1));
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
				part->TargetHoming(item, node, factor, accel, predict);
			}
			
			return 0;
		}
		if (!strcmp(function, "particleLimitSpeed"))
		{
			auto part = dynamic_cast<BaseParticle*>((LuaObject*)Script::ToData(1));
			if (!part)
				return Script::TypeError(1, "Particle");
			float speedMax = Script::ToNumber(2);
			part->LimitSpeed(speedMax);
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
	case 'r':
		if (!strcmp(field, "random"))
		{
			Script::PushInteger(random);
			return 1;
		}
		break;
	case 't':
		if (!strcmp(field, "tether"))
		{
			Script::PushInteger(tether);
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
	case 'r':
		if (!strcmp(field, "random"))
			random = Clamp(Script::ToInteger(1), 0, 32767);
		break;
	case 't':
		if (!strcmp(field, "tether"))
			tether = static_cast<TetherType>(Clamp(Script::ToInteger(1), 0, 1));
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
	case 'd':
		if (!strcmp(field, "drawMode"))
		{
			Script::PushInteger(drawMode);
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
	case 's':
		if (!strcmp(field, "saved"))
		{
			Script::PushBoolean(Saved);
			return 1;
		}
		if (!strcmp(field, "screenSpace"))
		{
			Script::PushBoolean(ScreenSpace);
			return 1;
		}
		if (!strcmp(field, "spriteSlot"))
		{
			Script::PushInteger(spriteSlot);
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
	case 'd':
		if (!strcmp(field, "drawMode"))
			drawMode = static_cast<DrawMode>(Clamp(Script::ToInteger(1), 0, 3));
		break;
	case 'l':
		if (!strcmp(field, "lineIgnoreVel"))
			LineIgnoreVel = Script::ToBoolean(1);
		break;
	case 's':
		if (!strcmp(field, "saved"))
			Saved = Script::ToBoolean(1);
		else if (!strcmp(field, "screenSpace"))
			ScreenSpace = Script::ToBoolean(1);
		else if (!strcmp(field, "spriteSlot"))
		{
			spriteSlot = Script::ToInteger(1);
			if (spriteSlot != SLOT_DEFAULT_SPRITES && spriteSlot != SLOT_MISC_SPRITES && spriteSlot != SLOT_CUSTOM_SPRITES)
				spriteSlot = SLOT_DEFAULT_SPRITES;
		}
		break;
	}
}

// ************  namespace ParticleFactory  ****************

namespace ParticleFactory
{
	ulong gameTick;

	int nextSpritePart;
	SpriteParticle spriteParts[MAX_SPRITEPARTS];
	int nextMeshPart;
	MeshParticle meshParts[MAX_MESHPARTS];
	int nextPartGroup;
	ParticleGroup partGroups[MAX_PARTGROUPS];

	FunctionType caller;
};


int ParticleFactory::GetFreeSpritePart()
{
	int i, free;
	SpriteParticle* part;
	
	for (part = &spriteParts[nextSpritePart], free = nextSpritePart, i = 0; i < MAX_SPRITEPARTS; ++i)
	{
		if (!part->lifeCounter)
		{
			nextSpritePart = (free + 1) % MAX_SPRITEPARTS;

			spriteParts[free] = SpriteParticle();
			spriteParts[free].emitterIndex = -1;
			spriteParts[free].emitterNode = -1;
			return free;
		}

		if (free == MAX_SPRITEPARTS-1)
		{
			part = &spriteParts[0];
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
	part = &spriteParts[0];

	for (i = 0; i < MAX_SPRITEPARTS; ++i, ++part)
	{
		if (part->lifeCounter < eldest)
		{
			free = i;
			eldest = part->lifeCounter;
		}
	}

	nextSpritePart = (free + 1) % MAX_SPRITEPARTS;

	spriteParts[free] = SpriteParticle();
	spriteParts[free].emitterIndex = -1;
	spriteParts[free].emitterNode = -1;
	return free;
}


int ParticleFactory::GetFreeMeshPart()
{
	int i, free;
	MeshParticle* part;

	for (part = &meshParts[nextMeshPart], free = nextMeshPart, i = 0; i < MAX_MESHPARTS; ++i)
	{
		if (!part->lifeCounter)
		{
			nextMeshPart = (free + 1) % MAX_MESHPARTS;

			meshParts[free] = MeshParticle();
			meshParts[free].emitterIndex = -1;
			meshParts[free].emitterNode = -1;
			return free;
		}

		if (free == MAX_MESHPARTS - 1)
		{
			part = &meshParts[0];
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
	part = &meshParts[0];

	for (i = 0; i < MAX_MESHPARTS; ++i, ++part)
	{
		if (part->lifeCounter < eldest)
		{
			free = i;
			eldest = part->lifeCounter;
		}
	}

	nextMeshPart = (free + 1) % MAX_MESHPARTS;

	meshParts[free] = MeshParticle();
	meshParts[free].emitterIndex = -1;
	meshParts[free].emitterNode = -1;
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
	for (int i = 0; i < MAX_SPRITEPARTS; i++)
		spriteParts[i] = SpriteParticle();
	nextSpritePart = 0;

	for (int i = 0; i < MAX_MESHPARTS; i++)
		meshParts[i] = MeshParticle();
	nextMeshPart = 0;
}

void ParticleFactory::ClearPartGroups()
{
	for (int i = 0; i < MAX_PARTGROUPS; i++)
		partGroups[i] = ParticleGroup();
	nextPartGroup = 0;
}

void ParticleFactory::UpdateParts()
{
	UpdateSprites();
	UpdateMeshes();
	gameTick++;
}

void ParticleFactory::UpdateSprites()
{
	SpriteParticle* part = &spriteParts[0];

	ParticleFactory::caller = FUNCTION_UPDATE;
	Script::PreFunctionLoop();
	for (int i = 0; i < MAX_SPRITEPARTS; ++i, ++part)
	{
		if (part->lifeCounter <= 0)
			continue;

		const auto& pgroup = partGroups[part->groupIndex];

		if (part->emitterIndex >= 0 || part->emitterNode >= 0)
		{
			int cutoff = -1;

			if (pgroup.attach.cutoff > 0)
			{
				cutoff = pgroup.attach.cutoff;
				if (pgroup.attach.random > 1)
					cutoff += (GetRandomDraw() % pgroup.attach.random);
			}

			if ((part->lifeSpan - part->lifeCounter) > cutoff)
				part->Detach();
		}

		int fadetime = part->lifeSpan;
		int lifefactor = (part->lifeSpan - part->lifeCounter);

		if (part->colorFadeTime)
		{
			if (part->colorFadeTime < 0 && part->lifeSpan >(-part->colorFadeTime))
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


void ParticleFactory::UpdateMeshes()
{
	auto part = &meshParts[0];

	ParticleFactory::caller = FUNCTION_UPDATE;
	Script::PreFunctionLoop();
	for (int i = 0; i < MAX_MESHPARTS; ++i, ++part)
	{
		if (part->lifeCounter <= 0)
			continue;

		const auto& pgroup = partGroups[part->groupIndex];

		if (part->emitterIndex >= 0 || part->emitterNode >= 0)
		{
			int cutoff = -1;

			if (pgroup.attach.cutoff > 0)
			{
				cutoff = pgroup.attach.cutoff;
				if (pgroup.attach.random > 1)
					cutoff += (GetRandomDraw() % pgroup.attach.random);
			}

			if ((part->lifeSpan - part->lifeCounter) > cutoff)
				part->Detach();
		}

		Script::ExecuteFunction(pgroup.updateIndex, part);

		part->vel += part->accel;
		part->pos += part->vel;
		part->rotX += part->rotVelX;
		part->rotY += part->rotVelY;
		part->rotZ += part->rotVelZ;

		--part->lifeCounter;
	}

	Script::PostFunctionLoop();
}


void ParticleFactory::DrawParts()
{
	DrawSprites();
	DrawMeshes();
}


void ParticleFactory::DrawSprites()
{
	phd_PushMatrix();
	phd_TranslateAbs(lara_item->pos.xPos, lara_item->pos.yPos, lara_item->pos.zPos);

	SpriteParticle* part = &ParticleFactory::spriteParts[0];

	int x1 = 0, y1 = 0, z1 = 0;
	for (int i = 0; i < MAX_SPRITEPARTS; ++i, ++part)
	{
		if (part->lifeCounter <= 0)
			continue;

		Diagnostics::activeSpriteParticles++;

		const auto& pgroup = ParticleFactory::partGroups[part->groupIndex];

		if (pgroup.drawMode == DrawMode::DRAW_NONE)
			continue;

		long viewCoords[6] = { 0,0,0,0,0,0 };

		if (pgroup.ScreenSpace)
		{
			if (part->pos.x < -1.0f || part->pos.x > 2.0f || part->pos.y < -1.0f || part->pos.y > 2.0f)
			{
				part->lifeCounter = 0;
				continue;
			}

			viewCoords[0] = Round(part->pos.x * phd_winxmax);
			viewCoords[1] = Round(part->pos.y * phd_winxmax);
			viewCoords[2] = Round(part->pos.z + f_mznear);

			if (pgroup.drawMode)
			{
				float size = part->sizeCust;
				auto vel = part->vel;

				size *= (1.0f / 32.0f);

				if (pgroup.LineIgnoreVel)
					vel = vel.normalized() * (1.0f / 32.0f);

				vel *= size;

				float xf = (part->pos.x - vel.x) * phd_winxmax;
				float yf = (part->pos.y - vel.y) * phd_winxmax;

				viewCoords[3] = Round(xf);
				viewCoords[4] = Round(yf);
				viewCoords[5] = viewCoords[2];
			}
		}
		else
		{
			auto partPos = part->AbsPos();

			x1 = Round(partPos.x);
			y1 = Round(partPos.y);
			z1 = Round(partPos.z);

			x1 -= lara_item->pos.xPos;
			y1 -= lara_item->pos.yPos;
			z1 -= lara_item->pos.zPos;

			if (x1 < -MAX_DRAWDIST || x1 > MAX_DRAWDIST ||
				y1 < -MAX_DRAWDIST || y1 > MAX_DRAWDIST ||
				z1 < -MAX_DRAWDIST || z1 > MAX_DRAWDIST)
			{
				part->lifeCounter = 0;
				continue;
			}

			// convert from world coordinates to screen coordinates
			long result[3] = { 0, 0, 0 };

			result[0] = (phd_mxptr[M00] * x1 + phd_mxptr[M01] * y1 + phd_mxptr[M02] * z1 + phd_mxptr[M03]);
			result[1] = (phd_mxptr[M10] * x1 + phd_mxptr[M11] * y1 + phd_mxptr[M12] * z1 + phd_mxptr[M13]);
			result[2] = (phd_mxptr[M20] * x1 + phd_mxptr[M21] * y1 + phd_mxptr[M22] * z1 + phd_mxptr[M23]);

			float zv = f_persp / float(result[2]);
			viewCoords[0] = Round(result[0] * zv + f_centerx);
			viewCoords[1] = Round(result[1] * zv + f_centery);
			viewCoords[2] = result[2] >> 14;

			// if particle is a line do world to screen transform for second vertex
			if (pgroup.drawMode)
			{
				float size = part->sizeCust;
				auto vel = part->vel;

				if (pgroup.LineIgnoreVel)
					vel = vel.normalized(); // ignore speed contribution to particle's size
				else
					size *= (1.0f / 32.0f); // else scale down size

				vel *= size;

				if (part->emitterIndex >= 0 || part->emitterNode >= 0)
				{
					if (pgroup.attach.tether == TETHER_ROTATING)
					{
						auto item = lara_item;
						if (part->emitterIndex >= 0 && part->emitterIndex < level_items)
							item = &items[part->emitterIndex];

						if (part->emitterNode >= 0) // if attached to specific mesh node of item
						{
							int node = Clamp(part->emitterNode, 0, objects[item->object_number].nmeshes);

							vel = GetJointPos(item, node, Round(vel.x), Round(vel.y), Round(vel.z));
						}
						else if (part->emitterIndex >= 0) // no mesh node, use item's pos
						{
							vel = RotatePoint3D(vel, item->pos.xRot, item->pos.yRot, item->pos.zRot);
						}
					}
				}

				x1 = Round(x1 - vel.x);
				y1 = Round(y1 - vel.y);
				z1 = Round(z1 - vel.z);

				result[0] = (phd_mxptr[M00] * x1 + phd_mxptr[M01] * y1 + phd_mxptr[M02] * z1 + phd_mxptr[M03]);
				result[1] = (phd_mxptr[M10] * x1 + phd_mxptr[M11] * y1 + phd_mxptr[M12] * z1 + phd_mxptr[M13]);
				result[2] = (phd_mxptr[M20] * x1 + phd_mxptr[M21] * y1 + phd_mxptr[M22] * z1 + phd_mxptr[M23]);

				zv = f_persp / float(result[2]);
				viewCoords[3] = Round(result[0] * zv + f_centerx);
				viewCoords[4] = Round(result[1] * zv + f_centery);
				viewCoords[5] = result[2] >> 14;
			}
		}

		long minSize = 4;

		// draw the particle to the given screen coordinates
		part->DrawSpritePart(pgroup, viewCoords, minSize);
	}

	phd_PopMatrix();
}


void ParticleFactory::DrawMeshes()
{
	auto part = &meshParts[0];

	for (int i = 0; i < MAX_MESHPARTS; ++i, ++part)
	{
		if (part->lifeCounter > 0)
		{
			if (part->object >= 0 && objects[part->object].loaded)
				part->DrawMeshPart();
		}
	}
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


// ************ Base Particle methods  ****************

float BaseParticle::Parameter()
{
	return (lifeSpan - lifeCounter) / float(lifeSpan);
}


void BaseParticle::LimitSpeed(float speedMax)
{
	float speed = vel.magnitude();
	if (speed > speedMax)
		vel *= (speedMax / speed);
}


Vector3f BaseParticle::AbsPos()
{
	auto absPos = pos;

	if (emitterIndex >= 0 || emitterNode >= 0)
	{
		auto item = lara_item;
		if (emitterIndex >= 0 && emitterIndex < level_items)
			item = &items[emitterIndex];

		const auto& pgroup = ParticleFactory::partGroups[groupIndex];
		auto relPos = pos;

		if (emitterNode >= 0) // if attached to specific mesh node of item
		{
			int node = Clamp(emitterNode, 0, objects[item->object_number].nmeshes);

			if (pgroup.attach.tether == TetherType::TETHER_ROTATING)
				relPos = GetJointPos(item, node, Round(relPos.x), Round(relPos.y), Round(relPos.z));
			else
				relPos += GetJointPos(item, node, 0, 0, 0);
		}
		else if (emitterIndex >= 0) // no mesh node, use item's pos
		{
			if (pgroup.attach.tether == TetherType::TETHER_ROTATING)
				relPos = RotatePoint3D(relPos, item->pos.xRot, item->pos.yRot, item->pos.zRot);

			relPos.x += item->pos.xPos;
			relPos.y += item->pos.yPos;
			relPos.z += item->pos.zPos;
		}

		absPos = relPos;
	}

	return absPos;
}


void BaseParticle::Attach(int itemIndex, int node)
{
	auto item = lara_item;
	if (itemIndex >= 0 && itemIndex < level_items)
		item = &items[itemIndex];
	else
		itemIndex = -1;

	Vector3f relPos;
	const auto& pgroup = ParticleFactory::partGroups[groupIndex];

	if (node >= 0)
	{
		node = Clamp(node, 0, objects[item->object_number].nmeshes);

		if (pgroup.attach.tether == TetherType::TETHER_ROTATING)
			relPos = GetJointPos(item, node, Round(relPos.x), Round(relPos.y), Round(relPos.z));
		else
			relPos += GetJointPos(item, node, 0, 0, 0);
	}
	else if (itemIndex >= 0)
	{
		if (pgroup.attach.tether == TetherType::TETHER_ROTATING)
			relPos = RotatePoint3D(relPos, item->pos.xRot, item->pos.yRot, item->pos.zRot);

		relPos.x += item->pos.xPos;
		relPos.y += item->pos.yPos;
		relPos.z += item->pos.zPos;

		node = -1;
	}

	pos = (AbsPos() - relPos);
	emitterIndex = itemIndex;
	emitterNode = node;
}


void BaseParticle::Detach()
{
	pos = AbsPos();

	emitterIndex = -1;
	emitterNode = -1;
}


bool BaseParticle::CollideWalls(float rebound)
{
	if (emitterIndex < 0 && emitterNode < 0)
	{
		rebound = Clamp(rebound, 0.0f, 1.0f);

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


bool BaseParticle::CollideFloors(float rebound, float minBounce, int collMargin, bool accurate)
{
	if (emitterIndex < 0 && emitterNode < 0)
	{
		rebound = Clamp(rebound, 0.0f, 1.0f);
		if (minBounce < 0)
			minBounce = 0.0f;

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
			{
				pos.y = fh;
				vel.y = 0;
			}
				

			if (pos.y > fh)
				pos.y = fh;
			else if (pos.y < ch)
				pos.y = ch;

			return true;
		}
	}

	return false;
}


bool BaseParticle::CollidedWithItem(Tr4ItemInfo* item, int radius)
{
	if (item && emitterIndex < 0 && emitterNode < 0)
	{
		StrBoxCollisione* bounds = GetBoundsAccurate((StrItemTr4*)item);
		auto p = Round(pos);

		int y = p.y - item->pos.yPos;
		if (y >= bounds->MinY - radius && y <= bounds->MaxY + radius)
		{
			int dx = p.x - item->pos.xPos;
			int dz = p.z - item->pos.zPos;
			float s = sin(RAD(item->pos.yRot));
			float c = cos(RAD(item->pos.yRot));
			int x = int(dx * c - dz * s);
			int z = int(dx * s + dz * c);

			if (x >= bounds->MinX - radius &&
				x <= radius + bounds->MaxX &&
				z >= bounds->MinZ - radius &&
				z <= radius + bounds->MaxZ) { return true;}
		}
	}

	return false;
}


Vector3f BaseParticle::FollowTarget(const Vector3f& v, float maxSpeed, float distFactor, float distCutOff)
{
	auto dirVect = v - pos;

	float dist = dirVect.magnitude();

	dist -= distCutOff;

	if (dist <= 0)
		return Vector3f();

	if (dist > maxSpeed)
		dirVect *= maxSpeed / dist;

	return dirVect * (dist / (dist + distFactor));
}


Vector3f BaseParticle::WindVelocities(float factor)
{
	factor = Clamp(factor, 0.0f, 1.0f);

	return Vector3f(SmokeWindX * factor, 0, SmokeWindZ * factor);
}


bool BaseParticle::TargetHoming(Tr4ItemInfo* item, int targetNode, float homingFactor, float homingAccel, bool predict)
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


Vector3f BaseParticle::SplinePos(const Vector3f v[], int arrSize, float t)
{
	if (arrSize < 2)
		return pos;

	if (arrSize == 2)
		return v[0].lerp(v[1], t);

	int div = arrSize - 1;
	int index = int(t * div);
	float tp = (t * div) - index;

	Vector3f splinePos;

	if (!index)
		splinePos = SplineStart(v[index], v[index + 1], v[index + 2], tp);
	else if (index + 1 == div)
		splinePos = SplineEnd(v[index - 1], v[index], v[index + 1], tp);
	else
		splinePos = Spline(v[index - 1], v[index], v[index + 1], v[index + 2], tp);

	return splinePos;
}


Vector3f BaseParticle::SplineVel(const Vector3f v[], int arrSize, float t)
{
	Vector3f splineVel;
	int div = arrSize - 1;

	if (div < 1)
		return splineVel;

	if (div == 1)
		return (v[1] - v[0]) * (1.0f / lifeSpan);

	int index = int(t * div);
	float tp = (t * div) - index;

	if (!index)
		splineVel = SplineStartDerivative(v[index], v[index + 1], v[index + 2], tp);
	else if (index + 1 == div)
		splineVel = SplineEndDerivative(v[index - 1], v[index], v[index + 1], tp);
	else
		splineVel = SplineDerivative(v[index - 1], v[index], v[index + 1], v[index + 2], tp);

	return splineVel * (float(div) / lifeSpan);
}


Vector3f BaseParticle::AvoidRoomGeometry(int wallMargin, int floorMargin, float factor)
{
	Vector3f v;
	auto p = Round(pos);

	short room = roomIndex;

	if (TestForWall(p.x + wallMargin, p.y, p.z, &room))
		v.x -= factor;

	if (TestForWall(p.x - wallMargin, p.y, p.z, &(room = roomIndex)))
		v.x += factor;

	if (TestForWall(p.x, p.y, p.z + wallMargin, &(room = roomIndex)))
		v.z -= factor;

	if (TestForWall(p.x, p.y, p.z - wallMargin, &(room = roomIndex)))
		v.z += factor;

	Tr4FloorInfo* floor = (Tr4FloorInfo*)GetFloor(p.x, p.y, p.z, &roomIndex);

	int h = GetHeight(floor, p.x, p.y, p.z) - p.y;
	if (h < floorMargin)
		v.y -= factor;

	int c = p.y - GetCeiling(floor, p.x, p.y, p.z);
	if (c < floorMargin)
		v.y += factor;

	return v;
}


Vector3f BaseParticle::AvoidItem(Tr4ItemInfo* item, float radius, float factor)
{
	Vector3f v;

	Vector3f ipos(item->pos.xPos, item->pos.yPos, item->pos.zPos);

	if (SimpleDist(ipos, pos) < radius)
	{
		float dist = RealDist(ipos, pos);

		if (dist < radius)
			v = (ipos - pos) * (1 / dist) * factor;
	}

	return v;
}


// ************ Sprite Particle methods  ****************

int SpriteParticle::Index(const char* field)
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
		if (!strcmp(field, "skew"))
		{
			Script::PushNumber(skew / 32768.0f);
			return 1;
		}
		if (!strcmp(field, "spriteIndex"))
		{
			Script::PushInteger(spriteIndex);
			return 1;
		}
		break;
	case 't':
		if (!strcmp(field, "t"))
		{
			Script::PushNumber(Parameter());
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


void SpriteParticle::NewIndex(const char* field)
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
		else if (!strcmp(field, "skew"))
			skew = Clamp(Round(Script::ToNumber(1) * 32768), -32768, 32767);
		else if (!strcmp(field, "spriteIndex"))
			spriteIndex = Clamp(Script::ToInteger(1), 0, 255);
		break;
	}
}


void SpriteParticle::Animate(int startSprite, int endSprite, int frameRate)
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


Vector3f SpriteParticle::BoidSeparationRule(float radius, float factor)
{
	Vector3f v;

	for (int i = 0; i < MAX_SPRITEPARTS; ++i)
	{
		auto part = &ParticleFactory::spriteParts[i];
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


Vector3f SpriteParticle::BoidCohesionRule(float radius, float factor)
{
	Vector3f v;

	int neighbors = 0;
	for (int i = 0; i < MAX_SPRITEPARTS; ++i)
	{
		auto part = &ParticleFactory::spriteParts[i];
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


Vector3f SpriteParticle::BoidAlignmentRule(float radius, float factor)
{
	Vector3f v;

	int neighbors = 0;
	for (int i = 0; i < MAX_SPRITEPARTS; ++i)
	{
		auto part = &ParticleFactory::spriteParts[i];
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


void SpriteParticle::DrawSpritePart(const ParticleGroup& pgroup, long* const view, long smallest_size)
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
			float s = lifeDif / float(fadeIn);
			cR = Round(Lerp(0, cR, s));
			cG = Round(Lerp(0, cG, s));
			cB = Round(Lerp(0, cB, s));
		}
	}

	if (fadeOut)
	{
		if (lifeCounter < fadeOut)
		{
			float s = lifeCounter / float(fadeOut);
			cR = Round(Lerp(0, cR, s));
			cG = Round(Lerp(0, cG, s));
			cB = Round(Lerp(0, cB, s));
		}
	}

	if (pgroup.drawMode) // line or arrow
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

			if (pgroup.drawMode == DrawMode::DRAW_ARROW)
			{
				float dx = (x2 - x1) * 0.5f;
				float dy = (y2 - y1) * 0.5f;

				float angle = M_PI / 6.0f;

				float rx = dx * cos(-angle) - dy * sin(-angle);
				float ry = dy * cos(-angle) + dx * sin(-angle);

				v[1].sx = float(x1 + rx);
				v[1].sy = float(y1 + ry);

				(*AddLineSorted)(&v[0], &v[1], 6);

				rx = dx * cos(angle) - dy * sin(angle);
				ry = dy * cos(angle) + dx * sin(angle);

				v[1].sx = float(x1 + rx);
				v[1].sy = float(y1 + ry);

				(*AddLineSorted)(&v[0], &v[1], 6);
			}
		}
	}
	else
	{
		float size;

		if (pgroup.ScreenSpace)
			size = sizeCust;
		else
		{
			size = phd_persp * sizeCust / float(z1);

			if (size < smallest_size)
				size = smallest_size;
		}

		float xsize = 0.5f, ysize = 0.5f;

		if (skew)
		{
			xsize = (skew + 32768.0f) / 65536.0f;
			ysize = 1 - xsize;
		}

		float s1h = size * xsize;
		float s2h = size * ysize;

		long xmin = view[0] + int(s1h);
		long xmax = view[0] - int(s1h);
		long ymin = view[1] + int(s2h);
		long ymax = view[1] - int(s2h);

		if (xmin >= phd_winxmin && xmax < phd_winxmax && ymin >= phd_winymin && ymax < phd_winymax)
		{
			D3DTLVERTEX v[4];
			long x1, y1, x2, y2, x3, y3, x4, y4;

			if (rot)
			{
				float s = sin(RAD(rot));
				float c = cos(RAD(rot));

				float sx1 = (-s1h * s);
				float sx2 = (s1h * s);

				float sy1 = (-s2h * s);
				float sy2 = (s2h * s);

				float cx1 = (-s1h * c);
				float cx2 = (s1h * c);

				float cy1 = (-s2h * c);
				float cy2 = (s2h * c);

				x1 = Round(sx1 - cy1 + view[0]);
				x2 = Round(sx2 - cy1 + view[0]);
				x3 = Round(sx2 - cy2 + view[0]);
				x4 = Round(sx1 - cy2 + view[0]);

				y1 = Round(cx1 + sy1 + view[1]);
				y2 = Round(cx2 + sy1 + view[1]);
				y3 = Round(cx2 + sy2 + view[1]);
				y4 = Round(cx1 + sy2 + view[1]);

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

			SpriteStruct* sprite = (spriteinfo + objects[pgroup.spriteSlot].mesh_index + spriteIndex);

			TextureStruct tex;

			tex.drawtype = pgroup.blendingMode;

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
}


// ************ Mesh Particle methods ****************

int MeshParticle::Index(const char* field)
{
	return 0;
}


void MeshParticle::NewIndex(const char* field)
{

}


void MeshParticle::Animate(int startMesh, int endMesh, int frameRate)
{
	int length = endMesh - startMesh;
	if (!frameRate || length <= 0)
		return;

	int usedMesh = startMesh;

	if (frameRate > 0)
		usedMesh += ((lifeSpan - lifeCounter) / frameRate) % (length + 1);
	else
	{
		// repeat animation cycle (-framerate) times through particle lifespan
		int numCycles = -frameRate;
		usedMesh = int(Lerp(float(usedMesh), float(endMesh), fmod(numCycles * Parameter(), 1.0f)));
	}

	mesh = (uchar)(usedMesh);
}


void MeshParticle::AlignToVel(float factor, bool invert)
{
	float correction = float(M_PI_2);

	float phi = -(atan2(vel.z, vel.x) - correction);
	float theta = atan2(sqrt(vel.x * vel.x + vel.z * vel.z), vel.y) - correction;

	if (invert)
	{
		theta = -theta;
		phi += float(M_PI);
	}

	int dy = GetOrientDiff(rotY, ANG(phi));
	int dx = GetOrientDiff(rotX, ANG(theta));

	factor = Clamp(factor, 0.0f, 1.0f);

	rotVelY = Round(dy * factor);
	rotVelX = Round(dx * factor);
}


void MeshParticle::AlignToTarget(const Vector3f& v, float factor, bool invert)
{
	float correction = float(M_PI_2);

	auto dir = v - pos;
	float phi = -(atan2(dir.z, dir.x) - correction);
	float theta = atan2(sqrt(dir.x * dir.x + dir.z * dir.z), dir.y) - correction;

	if (invert)
	{
		theta = -theta;
		phi += float(M_PI);
	}

	int dy = GetOrientDiff(rotY, ANG(phi));
	int dx = GetOrientDiff(rotX, ANG(theta));

	factor = Clamp(factor, 0.0f, 1.0f);

	rotVelY = Round(dy * factor);
	rotVelX = Round(dx * factor);
}


void MeshParticle::Shatter()
{
	int meshindex = objects[object].mesh_index + (mesh * 2);
	short** meshpp = &meshes[meshindex];
	ShatterItem.meshp = *meshpp;

	auto p = Round(pos);
	ShatterItem.sphere.x = p.x;
	ShatterItem.sphere.y = p.y;
	ShatterItem.sphere.z = p.z;
	ShatterItem.yRot = rotY;

	ShatterItem.bit = 0;
	ShatterItem.flags = 0;
	ShatterObject(&ShatterItem, 0, -32, roomIndex, 0);

	lifeCounter = 1;
}


Vector3f MeshParticle::BoidSeparationRule(float radius, float factor)
{
	Vector3f v;

	for (int i = 0; i < MAX_MESHPARTS; ++i)
	{
		auto part = &ParticleFactory::meshParts[i];
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


Vector3f MeshParticle::BoidCohesionRule(float radius, float factor)
{
	Vector3f v;

	int neighbors = 0;
	for (int i = 0; i < MAX_MESHPARTS; ++i)
	{
		auto part = &ParticleFactory::meshParts[i];
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


Vector3f MeshParticle::BoidAlignmentRule(float radius, float factor)
{
	Vector3f v;

	int neighbors = 0;
	for (int i = 0; i < MAX_MESHPARTS; ++i)
	{
		auto part = &ParticleFactory::meshParts[i];
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


void MeshParticle::DrawMeshPart()
{
	phd_vector projPos = Round(AbsPos());

	const auto& pgroup = ParticleFactory::partGroups[groupIndex];

	int testx = projPos.x - lara_item->pos.xPos;
	int testy = projPos.y - lara_item->pos.yPos;
	int testz = projPos.z - lara_item->pos.zPos;

	if (testx < -MAX_DRAWDIST || testx > MAX_DRAWDIST ||
		testy < -MAX_DRAWDIST || testy > MAX_DRAWDIST ||
		testz < -MAX_DRAWDIST || testz > MAX_DRAWDIST)
	{
		lifeCounter = 0;
		return;
	}

	phd_PushMatrix();
	phd_TranslateAbs(projPos.x, projPos.y, projPos.z);

	if (phd_mxptr[M23] > phd_znear && phd_mxptr[M23] < phd_zfar)
	{
		phd_RotYXZ(rotY, rotX, rotZ);

		if (scaleX || scaleY || scaleZ)
		{
			int sx = (scaleX + 128) << 7;
			int sy = (scaleY + 128) << 7;
			int sz = (scaleZ + 128) << 7;

			StrTripla sVect = { sx, sy, sz };

			ScaleCurrentMatrix(&sVect);
		}

		int meshindex = objects[object].mesh_index + (mesh * 2);
		auto meshpp = &meshes[meshindex];

		long oldAlpha = GlobalAlpha;

		if (transparency)
			GlobalAlpha = (255 - transparency) << 24;


		/* TODO: allow static as well as dynamic lighting for meshparts */


		phd_PutPolygons(*meshpp, -1);
		GlobalAlpha = oldAlpha;
	}

	phd_PopMatrix();
}
