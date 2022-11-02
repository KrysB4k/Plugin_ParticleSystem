#include "includes.h"

extern ParticleGroup partgroups[MAX_PARTGROUPS];


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
		usedSprite = Round(Lerp(float(usedSprite), float(usedSprite+length), fmod(numCycles * Parameter(), 1.0f)));
	}

	spriteIndex = (uchar)(usedSprite);
}


Vector3f Particle::ParticleAbsPos()
{
	auto absPos = pos;

	if (emitterIndex >= 0)
	{
		int index = emitterIndex;
		if (index > Trng.pGlobTomb4->pAdr->TotItemsMax)
			index = *Trng.pGlobTomb4->pAdr->pLaraIndex;

		auto item = &items[index];

		Vector3f relPos;

		if (emitterNode >= 0) // if attached to specific mesh node of item
		{	
			auto pdata = &partgroups[groupIndex];

			if (pdata)
			{
				relPos.x = pdata->attach.offX;
				relPos.y = pdata->attach.offY;
				relPos.z = pdata->attach.offZ;
			}

			int node = emitterNode & 0x1F;

			relPos = GetJointPos(item, node, int(relPos.x), int(relPos.y), int(relPos.z));
		}
		else // no mesh node, use item's pos
		{
			relPos.x = item->pos.xPos;
			relPos.y = item->pos.yPos;
			relPos.z = item->pos.zPos;
		}

		absPos += relPos;
	}

	return absPos;
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


void Particle::DrawParticle(ParticleGroup* const pdata, long* const view, long smallest_size)
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

	if (pdata)
		spriteSlot = pdata->spriteSlot;

	if (spriteSlot > 0)
	{
		int s1, s2;
		float size = Lerp(startSize, destSize, Parameter());
		int sizeInt = Round(size);

		if (pdata->NoPerspective)
			s1 = s2 = sizeInt;
		else
		{
			s1 = Round(phd_persp * size / z1);

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

			tex.drawtype = pdata->blendingMode;

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