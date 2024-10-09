#include "..\definitions\includes.h"

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

static ulong randomHashInt(ulong i0)
{
	ulong z0 = (i0 * 1831267127) ^ i0;
	ulong z1 = (z0 * 3915839201) ^ (z0 >> 20);
	ulong z2 = (z1 * 1561867961) ^ (z1 >> 24);
	return z2;
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

		auto& pgroup = partGroups[part->groupIndex];

		if (part->emitterIndex >= 0 && !pgroup.ScreenSpace)
		{
			int cutoff = -1;

			if (pgroup.attach.cutoff > 0)
			{
				cutoff = pgroup.attach.cutoff;
				if (pgroup.attach.random > 1)
					cutoff += (randomHashInt(1725364589 + i) % pgroup.attach.random);
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

		if (!Script::ExecuteFunction(pgroup.updateIndex, part))
			Script::DeleteFunction(&pgroup.updateIndex);

		if (pgroup.ScreenSpace)
			part->vel.z = part->accel.z = 0;

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

		auto& pgroup = partGroups[part->groupIndex];

		if (part->emitterIndex >= 0 && !pgroup.ScreenSpace)
		{
			int cutoff = -1;

			if (pgroup.attach.cutoff > 0)
			{
				cutoff = pgroup.attach.cutoff;
				if (pgroup.attach.random > 1)
					cutoff += (randomHashInt(1725364589 + i) % pgroup.attach.random);
			}

			if ((part->lifeSpan - part->lifeCounter) > cutoff)
				part->Detach();
		}

		if (!Script::ExecuteFunction(pgroup.updateIndex, part))
			Script::DeleteFunction(&pgroup.updateIndex);

		if (pgroup.ScreenSpace)
			part->vel.z = part->accel.z = 0;

		part->vel += part->accel;
		part->pos += part->vel;
		part->rot.x += part->rotVel.x;
		part->rot.y += part->rotVel.y;
		part->rot.z += part->rotVel.z;

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

		Diagnostics::IncrementValue(DIAGNOSTICS_SPRITE, 1);

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

			if (pgroup.drawMode > DrawMode::DRAW_SQUARE)
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
			if (pgroup.drawMode > DrawMode::DRAW_SQUARE)
			{
				float size = part->sizeCust;
				auto vel = part->vel;

				if (pgroup.LineIgnoreVel)
					vel = vel.normalized(); // ignore speed contribution to particle's size
				else
					size *= (1.0f / 32.0f); // else scale down size

				vel *= size;

				if (part->emitterIndex >= 0)
				{
					if (pgroup.attach.tether == TETHER_ROTATING)
					{
						auto item = lara_item;
						if (part->emitterIndex >= 0 && part->emitterIndex < level_items)
							item = &items[part->emitterIndex];

						if (part->emitterNode >= 0) // if attached to specific mesh node of item
						{
							vel = GetJointPos(item, part->emitterNode, Round(vel.x), Round(vel.y), Round(vel.z)) - GetJointPos(item, part->emitterNode, 0, 0, 0);
						}
						else // no mesh node, use item's pos
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
			Diagnostics::IncrementValue(DIAGNOSTICS_MESH, 1);

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
	{
		if (!Script::ExecuteFunction(partGroups[i].initIndex, nullptr))
			Script::DeleteFunction(&partGroups[i].initIndex);
	}
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
	const auto& tether = ParticleFactory::partGroups[groupIndex].attach.tether;

	if (tether != TetherType::TETHER_NONE && emitterIndex >= 0 && emitterIndex < level_items)
	{
		auto item = &items[emitterIndex];

		auto relPos = pos;

		if (emitterNode >= 0) // if attached to specific mesh node of item
		{
			if (tether == TetherType::TETHER_ROTATING)
				relPos = GetJointPos(item, emitterNode, Round(relPos.x), Round(relPos.y), Round(relPos.z));
			else
				relPos += GetJointPos(item, emitterNode, 0, 0, 0);
		}
		else // no mesh node, use item's pos
		{
			if (tether == TetherType::TETHER_ROTATING)
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
	if (itemIndex < 0 || itemIndex >= level_items)
		return;

	auto item = &items[itemIndex];
	const auto& tether = ParticleFactory::partGroups[groupIndex].attach.tether;
	Vector3f relPos;

	if (tether != TetherType::TETHER_NONE)
	{
		if (node >= 0)
		{
			if (tether == TetherType::TETHER_ROTATING)
				relPos = GetJointPos(item, node, Round(relPos.x), Round(relPos.y), Round(relPos.z));
			else
				relPos += GetJointPos(item, node, 0, 0, 0);
		}
		else if (itemIndex >= 0)
		{
			if (tether == TetherType::TETHER_ROTATING)
				relPos = RotatePoint3D(relPos, item->pos.xRot, item->pos.yRot, item->pos.zRot);

			relPos.x += item->pos.xPos;
			relPos.y += item->pos.yPos;
			relPos.z += item->pos.zPos;

			node = -1;
		}
	}

	pos = (AbsPos() - relPos);
	emitterIndex = itemIndex;
	emitterNode = (char)node;
}


void BaseParticle::Detach()
{
	pos = AbsPos();

	emitterIndex = -1;
	emitterNode = -1;
}


bool BaseParticle::CollideWalls(float rebound)
{
	const auto& tether = ParticleFactory::partGroups[groupIndex].attach.tether;

	if (tether == TetherType::TETHER_NONE || emitterIndex < 0)
	{
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
	const auto& tether = ParticleFactory::partGroups[groupIndex].attach.tether;

	if (tether == TetherType::TETHER_NONE || emitterIndex < 0)
	{
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
	if (item)
	{
		StrBoxCollisione* bounds = GetBoundsAccurate((StrItemTr4*)item);
		auto p = Round(AbsPos());

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


Vector3f BaseParticle::FollowTarget(const Vector3f& v, float maxSpeed, float distInner, float distOuter)
{
	auto dirVect = v - pos;

	float dist = dirVect.magnitude();

	if (dist <= distInner)
		return Vector3f();

	if (dist > maxSpeed)
		dirVect *= maxSpeed / dist;

	return dirVect * InverseLerp(distInner, distOuter, dist);
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


Vector3f BaseParticle::AttractToItem(Tr4ItemInfo* item, float radius, float factor)
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

	spriteIndex = (ushort)(usedSprite);
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

	if (pgroup.drawMode > DrawMode::DRAW_SQUARE) // line or arrow
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

				float sx = dx * SIN_PI_6;
				float cx = dx * COS_PI_6;
				float sy = dy * SIN_PI_6;
				float cy = dy * COS_PI_6;

				v[1].rhw = (v[0].rhw + v[1].rhw) * 0.5f;
				v[1].sz = (v[0].sz + v[1].sz) * 0.5f;

				v[1].sx = x1 + cx + sy;
				v[1].sy = y1 + cy - sx;

				(*AddLineSorted)(&v[0], &v[1], 6);

				v[1].sx = x1 + cx - sy;
				v[1].sy = y1 + cy + sx;

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

		if (sizeRatio)
		{
			xsize = (sizeRatio + 32768.0f) / 65536.0f;
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

			TextureStruct tex;
			tex.drawtype = pgroup.blendingMode;

			if (!pgroup.drawMode)
			{
				SpriteStruct* sprite = (spriteinfo + objects[pgroup.spriteSlot].mesh_index + spriteIndex);
				tex.tpage = sprite->tpage;
				tex.u1 = sprite->x1;
				tex.v1 = sprite->y1;
				tex.u2 = sprite->x2;
				tex.v2 = sprite->y1;
				tex.u3 = sprite->x2;
				tex.v3 = sprite->y2;
				tex.u4 = sprite->x1;
				tex.v4 = sprite->y2;
			}
			else
			{
				tex.flag = 0;
				tex.tpage = 0;
			}

			(*AddQuadSorted)(v, 0, 1, 2, 3, &tex, 0);
		}
	}
}


// ************ Mesh Particle methods ****************

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

	int dy = GetOrientDiff(rot.y, ANG(phi));
	int dx = GetOrientDiff(rot.x, ANG(theta));

	rotVel.y = Round(dy * factor);
	rotVel.x = Round(dx * factor);
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

	int dy = GetOrientDiff(rot.y, ANG(phi));
	int dx = GetOrientDiff(rot.x, ANG(theta));

	factor = Clamp(factor, 0.0f, 1.0f);

	rotVel.y = Round(dy * factor);
	rotVel.x = Round(dx * factor);
}


void MeshParticle::Shatter()
{
	static Tr4ShatterItem shatter;
	int meshindex = objects[object].mesh_index + (mesh * 2);
	short** meshpp = &meshes[meshindex];
	shatter.meshp = *meshpp;

	auto p = Round(pos);
	shatter.sphere.x = p.x;
	shatter.sphere.y = p.y;
	shatter.sphere.z = p.z;
	shatter.yRot = rot.y;

	shatter.bit = 0;
	shatter.flags = 0;
	ShatterObject(&shatter, 0, -32, roomIndex, 0);

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
		phd_RotYXZ(rot.y, rot.x, rot.z);

		if (scale.x != 16384 || scale.y != 16384 || scale.z != 16384)
		{
			StrTripla sVect = { scale.x, scale.y, scale.z };

			ScaleCurrentMatrix(&sVect);
		}

		int meshindex = objects[object].mesh_index + (mesh * 2);
		auto meshpp = &meshes[meshindex];

		long oldAlpha = GlobalAlpha;

		if (transparency)
			GlobalAlpha = (255 - transparency) << 24;

		item.pos.xPos = pos.x;
		item.pos.yPos = pos.y;
		item.pos.zPos = pos.z;
		item.pos.xRot = rot.x;
		item.pos.yRot = rot.y;
		item.pos.zRot = rot.z;
		item.room_number = roomIndex;

		Tr4MeshData* meshData = (Tr4MeshData*)*meshpp;

		if (meshData->prelight)
			item.shade = ConvertTo16BitBGR(tint);
		else
			item.shade = -1;

		short frame[6] = { 0, 0, 0, 0, 0, 0 };

		CalculateObjectLighting(&item, frame);

		phd_PutPolygons(*meshpp, -1);
		GlobalAlpha = oldAlpha;
	}

	phd_PopMatrix();
}
