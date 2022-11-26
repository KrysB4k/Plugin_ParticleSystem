#include "..\definitions\includes.h"


// ************  namespace ParticleFactory  ****************

namespace ParticleFactory
{
	int nextPart;
	Particle parts[MAX_PARTS];
	ParticleGroup partGroups[MAX_PARTGROUPS];
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

			ClearMemory(&parts[free], sizeof(Particle));
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

	ClearMemory(&parts[free], sizeof(Particle));
	parts[free].emitterIndex = -1;
	parts[free].emitterNode = -1;
	return free;
}


void ParticleFactory::ClearParts()
{
	ClearMemory(parts, sizeof(Particle) * MAX_PARTS);
	nextPart = 0;
}


void ParticleFactory::UpdateParts()
{
	Particle* part = &ParticleFactory::parts[0];

	for (int i = 0; i < MAX_PARTS; ++i, ++part)
	{
		if (part->lifeCounter <= 0)
			continue;

		const auto &pgroup = ParticleFactory::partGroups[part->groupIndex];

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

		float t = lifefactor/float(fadetime);
		if (t < 0.0f)
			t = 0.0f;
		if (t > 1.0f)
			t = 1.0f;

		part->colCust = Lerp(part->colStart, part->colEnd, t);

		t = part->Parameter();
		part->sizeCust = Round(Lerp(float(part->sizeStart), float(part->sizeEnd), t));

		part->UpdateParticle(pgroup.updateIndex);
		
		part->vel += part->accel;
		part->pos += part->vel;
		part->rot += part->rotVel;

		--part->lifeCounter;
	}
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

		auto partPos = part->ParticleAbsPos();

		x1 = Round(partPos.x);
		y1 = Round(partPos.y);
		z1 = Round(partPos.z);

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


// ************  Particle public methods  ****************

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
	if (rebound < 0)
		rebound = 1.0f;

	auto testp = Round(pos + vel);

	short tRoom = roomIndex;
	int wallStatus = TestForWall(testp.x, testp.y, testp.z, &tRoom);
	if (roomIndex != tRoom)
		roomIndex = tRoom;

	if (wallStatus)
	{
		if (TestForWall(Round(pos.x), testp.y, testp.z, &tRoom))
			vel.z = -(vel.z * rebound);
		if (TestForWall(testp.x, testp.y, Round(pos.z), &tRoom))
			vel.x = -(vel.x * rebound);

		return true;
	}

	return false;
}


bool Particle::ParticleCollideFloors(float rebound, float friction, bool accurate)
{
	auto testp = Round(pos + vel);
	short tRoom = roomIndex;

	Tr4FloorInfo* floor = (Tr4FloorInfo*) GetFloor(testp.x, testp.y, testp.z, &tRoom);
	int fh = GetHeight(floor, testp.x, testp.y, testp.z);
	int ch = GetCeiling(floor, testp.x, testp.y, testp.z);
	if (roomIndex != tRoom)
		roomIndex = tRoom;

	if (rebound < 0)
		rebound = 1.0f;

	if (fh != (-0x7F00) && (testp.y > fh || testp.y < ch))
	{
		if (friction > 0)
		{
			vel.x *= friction;
			vel.z *= friction;
		}

		if (testp.y > fh && accurate)
		{
			auto n = GetSlopeNormal(floor, testp.x, testp.y, testp.z);

			// get reflection vector off slope surface
			auto reflected = vel - (n * vel.dot(n) * 2);
			vel = reflected * rebound;
		}
		else
			vel.y = -(vel.y * rebound);

		return true;
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

	short meshnum = objects[item->object_number].nmeshes;
	if (targetNode < 0)
		targetNode = GetRandomControl() % meshnum; // target random node on object
	else
		targetNode = (targetNode >= meshnum) ? (meshnum-1) : targetNode;

	auto targetPos = GetJointPos(item, targetNode, 0, 0, 0);

	if (predict)
	{
		float s = sin(RAD(item->pos.yRot));
		float c = cos(RAD(item->pos.yRot));

		Vector3f heading(item->speed * s, item->fallspeed,  item->speed * c);

		float time = RealDist(pos, targetPos) / vel.magnitude();

		targetPos += (heading * time);
	}

	auto dir = (targetPos - pos).normalized();

	if (homingFactor < 0.0f)
	{
		dir = -dir;
		homingFactor = -homingFactor;
	}
		
	auto slerpVel = vel.normalized().slerp(dir, homingFactor);
	vel = slerpVel * (vel.magnitude() + homingAccel);

	return true;
}


// stub for effect updating function
void Particle::UpdateParticle(int updateIndex)
{

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
