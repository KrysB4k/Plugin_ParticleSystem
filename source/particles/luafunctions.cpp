#include "../definitions/DefTomb4Funct.h"
#include "../definitions/Tomb4Discoveries_mine.h"
#include "../definitions/Tomb4Globals.h"
#include "../definitions/macros_mine.h"
#include "../trng/trng.h"
#include "particle.h"
#include "noise.h"
#include "utilities.h"
#include "logger.h"

using namespace LuaHelpers;
using namespace Utilities;

namespace LuaFunctions
{
	struct AbsFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushNumber(GetMathResult(1, fabsf));
			return 1;
		}
	};

	struct AcosFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushNumber(GetMathResult(1, acosf));
			return 1;
		}
	};

	struct AsinFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushNumber(GetMathResult(1, asinf));
			return 1;
		}
	};

	struct AtanFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushNumber(GetMathResult(1, atanf));
			return 1;
		}
	};

	struct Atan2Function final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushNumber(GetMathResult(1, 2, atan2f));
			return 1;
		}
	};

	struct BindFunctionFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			CheckCaller(FunctionType::FUNCTION_LEVEL, "bindFunction");

			int index = GetClampedInteger(1, 1, MAX_FUNCREFS, false) - 1;
			if (Particles::functionRefs[index].ref != SCRIPT_REFNIL)
				Script::Throw(FormatString("index= %d is already registered for a different Lua function", index + 1));

			Particles::functionRefs[index].ref = GetFunction(2);
			Particles::functionRefs[index].type = FUNCTION_BIND;
			return 0;
		}
	};

	struct BindGroupFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			CheckCaller(FunctionType::FUNCTION_LEVEL, "bindGroup");

			int index = GetClampedInteger(1, 1, MAX_FUNCREFS, false) - 1;
			if (Particles::functionRefs[index].ref != SCRIPT_REFNIL)
				Script::Throw(FormatString("index= %d is already registered for a different Lua function", index + 1));

			auto group = GetData<Particles::ParticleGroup>(2);
			if (group->autoTrigger)
			{
				Script::EmitFailure("this group cannot be bound because it is auto triggered", Logger::Warning);
				return 0;
			}
			Particles::functionRefs[index].ref = group->initIndex;
			Particles::functionRefs[index].type = FUNCTION_INIT;
			return 0;
		}
	};

	struct BoidAlignmentFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto part = GetData<Particles::BaseParticle>(1);
			float radius = GetNumber(2);
			float factor = GetNumber(3);
			part->vel += part->BoidAlignmentRule(radius, factor);
			return 0;
		}
	};

	struct BoidCohesionFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto part = GetData<Particles::BaseParticle>(1);
			float radius = GetNumber(2);
			float factor = GetNumber(3);
			part->vel += part->BoidCohesionRule(radius, factor);
			return 0;
		}
	};

	struct BoidSeparationFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto part = GetData<Particles::BaseParticle>(1);
			float radius = GetNumber(2);
			float factor = GetNumber(3);
			part->vel += part->BoidSeparationRule(radius, factor);
			return 0;
		}
	};

	struct CeilFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushNumber(GetMathResult(1, ceilf));
			return 1;
		}
	};

	struct CbrtFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushNumber(GetMathResult(1, cbrtf));
			return 1;
		}
	};

	struct ClampFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			float x = GetNumber(1);
			float min = GetNumber(2);
			float max = GetNumber(3);
			Script::PushNumber(Clamp(x, min, max));
			return 1;
		}
	};

	struct CosFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushNumber(GetMathResult(1, cosf));
			return 1;
		}
	};

	struct CreateColorHSVFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			float h = GetNumber(1);
			float s = GetClampedNumber(2, 0.0f, 1.0f, false);
			float v = GetClampedNumber(3, 0.0f, 1.0f, false);
			ConstructManagedData<ColorRGB>(HSVtoRGB(h, s, v));
			return 1;
		}
	};

	struct CreateColorFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			uchar r = (uchar)GetClampedInteger(1, 0, UINT8_MAX, false);
			uchar g = (uchar)GetClampedInteger(2, 0, UINT8_MAX, false);
			uchar b = (uchar)GetClampedInteger(3, 0, UINT8_MAX, false);
			ConstructManagedData<ColorRGB>(r, g, b);
			return 1;
		}
	};

	struct CreateGroupFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			CheckCaller(FunctionType::FUNCTION_MODULE, "createGroup");

			int init = GetFunction(1);
			int update = GetFunction(2);

			int i = Particles::GetFreeParticleGroup();
			if (i == -1)
				Script::Throw("group creation failed, exceeded available particle group maximum");

			Particles::partGroups[i].initIndex = init;
			Particles::partGroups[i].updateIndex = update;
			Script::PushData(&Particles::partGroups[i]);
			return 1;
		}
	};
	
	struct CreateMeshPartFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			CheckCaller(FunctionType::FUNCTION_INIT | FunctionType::FUNCTION_UPDATE, "createMeshPart");

			auto group = GetData<Particles::ParticleGroup>(1);
			int i = Particles::GetFreeMeshPart();
			Particles::meshParts[i].groupIndex = group->groupIndex;
			Particles::meshParts[i].createdInCurrentLoop = true;

			Tr4ItemInfo* item = &Particles::meshParts[i].item;
			item->il.fcnt = -1;
			item->il.room_number = -1;
			item->il.RoomChange = 0;
			item->il.nCurrentLights = 0;
			item->il.nPrevLights = 0;
			item->il.ambient = -1;
			item->il.pCurrentLights = item->il.CurrentLights;
			item->il.pPrevLights = item->il.PrevLights;

			Script::PushData(&Particles::meshParts[i]);
			return 1;
		}
	};

	struct CreateModuleFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			CheckCaller(FunctionType::FUNCTION_MODULE, "createModule");

			int last = Particles::GetLastModule();
			if (last != -1 && Particles::modules[last].createdInCurrentModule)
				Script::Throw("module creation failed, a module already exists");

			int i = Particles::GetFreeModule();
			if (i == -1)
				Script::Throw("module creation failed, exceeded available module maximum");

			Particles::modules[i].createdInCurrentModule = true;
			Script::PushData(&Particles::modules[i]);
			return 1;
		}
	};

	struct CreatePerlinNoiseFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			CheckCaller(FunctionType::FUNCTION_MODULE, "createPerlinNoise");
			int args = GetArgCount(0, 1);
			if (args)
				ConstructManagedData<PerlinNoise>(GetInteger(1));
			else
				ConstructManagedData<PerlinNoise>();
			return 1;
		}
	};

	struct CreateSimplexNoiseFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			CheckCaller(FunctionType::FUNCTION_MODULE, "createSimplexNoise");
			int args = GetArgCount(0, 1);
			if (args)
				ConstructManagedData<SimplexNoise>(GetInteger(1));
			else
				ConstructManagedData<SimplexNoise>();
			return 1;
		}
	};

	struct CreateSpritePartFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			CheckCaller(FunctionType::FUNCTION_INIT | FunctionType::FUNCTION_UPDATE, "createSpritePart");
			auto group = GetData<Particles::ParticleGroup>(1);
			int i = Particles::GetFreeSpritePart();
			Particles::spriteParts[i].groupIndex = group->groupIndex;
			Particles::spriteParts[i].createdInCurrentLoop = true;
			Script::PushData(&Particles::spriteParts[i]);
			return 1;
		}
	};

	struct CreateVectorFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			float x = GetNumber(1);
			float y = GetNumber(2);
			float z = GetNumber(3);
			ConstructManagedData<Vector3f>(x, y, z);
			return 1;
		}
	};

	struct DegToRadFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushNumber(DegToRad(GetNumber(1)));
			return 1;
		}
	};

	struct DistanceFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto vec1 = GetData<LuaObjectClassPosition>(1);
			auto vec2 = GetData<LuaObjectClassPosition>(2);
			float x = vec2->GetX() - vec1->GetX();
			float y = vec2->GetY() - vec1->GetY();
			float z = vec2->GetZ() - vec1->GetZ();
			Script::PushNumber(sqrtf(x * x + y * y + z * z));
			return 1;
		}
	};

	struct DistCompareFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto vec1 = GetData<LuaObjectClassPosition>(1);
			auto vec2 = GetData<LuaObjectClassPosition>(2);
			float dist = GetNumber(3);
			float x = vec2->GetX() - vec1->GetX();
			float y = vec2->GetY() - vec1->GetY();
			float z = vec2->GetZ() - vec1->GetZ();
			float difsqr = x * x + y * y + z * z;
			dist *= dist;
			int res = 0;
			if (difsqr > dist)
				res = 1;
			else if (difsqr < dist)
				res = -1;
			Script::PushInteger(res);
			return 1;
		}
	};

	struct ExpFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushNumber(GetMathResult(1, expf));
			return 1;
		}
	};

	struct FindNearestTargetFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Vector3f vec = static_cast<Vector3f>(*GetData<LuaObjectClassPosition>(1));
			float radius = GetNumber(2);
			std::vector<short> slotList(GetTable(3));
			for (ulong i = 0; i < slotList.size(); i++)
				slotList[i] = GetClampedInteger(i + 4, SLOT_LARA, SLOT_NEW_SLOT18, true);
			Script::PushInteger(FindNearestTarget(vec, radius, slotList.data(), slotList.size()));
			return 1;
		}
	};

	struct FloorFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushNumber(GetMathResult(1, floorf));
			return 1;
		}
	};

	struct FmodFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushNumber(GetMathResult(1, 2, fmodf));
			return 1;
		}
	};

	struct GetGameTickFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushInteger(MyData.Save.Global.gameTick);
			return 1;
		}
	};

	struct GetItemInfoFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushData(&LuaGlobals::LuaItemArray[VerifyItemIndex(1)]);
			return 1;
		}
	};

	struct GetItemJointPosFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto item = &items[VerifyItemIndex(1)];
			int joint = GetClampedInteger(2, 0, objects[item->object_number].nmeshes - 1, false);
			int offX = GetInteger(3);
			int offY = GetInteger(4);
			int offZ = GetInteger(5);
			ConstructManagedData<Vector3f>(GetJointPos(item, joint, offX, offY, offZ));
			return 1;
		}
	};

	struct GetItemRoomFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto item = &items[VerifyItemIndex(1)];
			Script::PushInteger(item->room_number);
			return 1;
		}
	};

	struct GetLaraIndexFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushInteger(lara_info.item_number);
			return 1;
		}
	};

	struct GetSelectedItemFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushInteger(Trng.pGlobTomb4->ItemIndexSelected);
			return 1;
		}
	};

	struct GetTombIndexFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushInteger(GetTombIndexByNGLEIndex(1));
			return 1;
		}
	};

	struct IntervalFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			int count = GetArgCount(1, 2);

			int interval = GetInteger(1);
			if (interval < 1)
				Script::Throw("interval must be a positive integer");

			int remainder = 0;
			if (count > 1)
				remainder = GetClampedInteger(1, 0, (interval - 1), true);

			bool check = true;
			if ((MyData.Save.Global.gameTick - remainder) % interval)
				check = false;

			Script::PushBoolean(check);
			return 1;
		}
	};

	struct InvokeInitFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			CheckCaller(FunctionType::FUNCTION_BIND, "invokeInit");
			Particles::CallerGuard guard(FUNCTION_INIT);
			auto group = GetData<Particles::ParticleGroup>(1);
			if (group->autoTrigger)
			{
				Script::EmitFailure("this group cannot be invoked because it is auto triggered", Logger::Warning);
				return 0;
			}
			Script::PreFunctionLoop();
			if (!Script::ExecuteFunction(group->initIndex, nullptr))
				Script::DeleteFunction(&group->initIndex);
			Script::PostFunctionLoop();
			return 0;
		}
	};

	struct KillPartsOfGroupFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto group = GetData<Particles::ParticleGroup>(1);
			
			if (group)
			{
				for (int i = 0; i < MAX_SPRITEPARTS; i++)
				{
					auto part = &Particles::spriteParts[i];
					if (part->groupIndex == group->groupIndex && part->lifeCounter > 0)
					{
						Script::DeleteTable(part->data.table);
						part->lifeCounter = 0;
					}

				}
				for (int i = 0; i < MAX_MESHPARTS; i++)
				{
					auto part = &Particles::meshParts[i];
					if (part->groupIndex == group->groupIndex && part->lifeCounter > 0)
					{
						Script::DeleteTable(part->data.table);
						part->lifeCounter = 0;
					}
				}
			}

			return 0;
		}
	};

	struct LaraBurnFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			LaraBurn();
			return 0;
		}
	};

	struct LerpFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			float a = GetNumber(1);
			float b = GetNumber(2);
			float t = GetClampedNumber(3, 0.0f, 1.0f, false);
			Script::PushNumber(Lerp(a, b, t));
			return 1;
		}
	};

	struct LerpInverseFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			float val1 = GetNumber(1);
			float val2 = GetNumber(2);
			float x = GetNumber(3);
			Script::PushNumber(InverseLerp(val1, val2, x));
			return 1;
		}
	};

	struct LogFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushNumber(GetMathResult(1, logf));
			return 1;
		}
	};

	struct MeshAlignVelocityFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto part = GetData<Particles::MeshParticle>(1);
			float factor = GetClampedNumber(2, 0.0f, 1.0f, false);
			bool invert = GetBoolean(3);
			part->AlignToVel(factor, invert);
			return 0;
		}
	};

	struct MeshLookAtTargetFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto part = GetData<Particles::MeshParticle>(1);
			Vector3f vector = static_cast<Vector3f>(*GetData<LuaObjectClassPosition>(2));
			float factor = GetClampedNumber(3, 0.0f, 1.0f, false);
			bool invert = GetBoolean(4);
			part->AlignToTarget(vector, factor, invert);
			return 0;
		}
	};

	struct MeshShatterFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto part = GetData<Particles::MeshParticle>(1);
			part->Shatter();
			return 0;
		}
	};

	struct NoiseFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			float scale, x, y, z, w;

			int count = GetArgCount(3, 6);
			auto noise = GetData<Noise>(1);

			switch (count)
			{
			case 3:
				scale = GetNumber(2);
				x = GetNumber(3);
				if (scale)
					x /= scale;
				Script::PushNumber(noise->Noise1D(x));
				return 1;

			case 4:
				scale = GetNumber(2);
				x = GetNumber(3);
				y = GetNumber(4);
				if (scale)
				{
					scale = 1.0f / scale;
					x *= scale;
					y *= scale;
				}
				Script::PushNumber(noise->Noise2D(x, y));
				return 1;

			case 5:
				scale = GetNumber(2);
				x = GetNumber(3);
				y = GetNumber(4);
				z = GetNumber(5);
				if (scale)
				{
					scale = 1.0f / scale;
					x *= scale;
					y *= scale;
					z *= scale;
				}
				Script::PushNumber(noise->Noise3D(x, y, z));
				return 1;

			default:
				scale = GetNumber(2);
				x = GetNumber(3);
				y = GetNumber(4);
				z = GetNumber(5);
				w = GetNumber(6);
				if (scale)
				{
					scale = 1.0f / scale;
					x *= scale;
					y *= scale;
					z *= scale;
					w *= scale;
				}
				Script::PushNumber(noise->Noise4D(x, y, z, w));
				return 1;
			}
		}
	};

	struct NoiseCurlFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			float scale, x, y, z;
			Vector3f vector;
			int count = GetArgCount(4, 5);
			auto noise = GetData<Noise>(1);

			switch (count)
			{
			case 4:
				scale = GetNumber(2);
				x = GetNumber(3);
				y = GetNumber(4);
				if (scale)
				{
					scale = 1.0f / scale;
					x *= scale;
					y *= scale;
				}
				vector = noise->Curl2D(x, y);
				ConstructManagedData<Vector3f>(vector.x, vector.y, 0.0f);
				return 1;

			default:
				scale = GetNumber(2);
				x = GetNumber(3);
				y = GetNumber(4);
				z = GetNumber(5);
				if (scale)
				{
					scale = 1.0f / scale;
					x *= scale;
					y *= scale;
					z *= scale;
				}
				vector = noise->Curl3D(x, y, z);
				ConstructManagedData<Vector3f>(vector.x, vector.y, vector.z);
				return 1;
			}
		}
	};

	struct NoiseCurlTimeFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			float scale, time, x, y, z;
			Vector3f vector;
			int count = GetArgCount(5, 6);
			auto noise = GetData<Noise>(1);

			switch (count)
			{
			case 5:
				scale = GetNumber(2);
				time = GetNumber(3);
				x = GetNumber(4);
				y = GetNumber(5);
				if (scale)
				{
					scale = 1.0f / scale;
					x *= scale;
					y *= scale;
				}
				vector = noise->Curl2DTime(time, x, y);
				ConstructManagedData<Vector3f>(vector.x, vector.y, 0.0f);
				return 1;

			default:
				scale = GetNumber(2);
				time = GetNumber(3);
				x = GetNumber(4);
				y = GetNumber(5);
				z = GetNumber(6);
				if (scale)
				{
					scale = 1.0f / scale;
					x *= scale;
					y *= scale;
					z *= scale;
				}
				vector = noise->Curl3DTime(time, x, y, z);
				ConstructManagedData<Vector3f>(vector.x, vector.y, vector.z);
				return 1;
			}
		}
	};

	struct ParticleAbsPosFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto part = GetData<Particles::BaseParticle>(1);
			ConstructManagedData<Vector3f>(part->AbsPos());
			return 1;
		}
	};

	struct ParticleAnimateFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto part = GetData<Particles::BaseParticle>(1);
			int start = GetInteger(2);
			int end = GetInteger(3);
			int framerate = GetInteger(4);
			part->Animate(start, end, framerate);
			return 0;
		}
	};

	struct ParticleAttachFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto part = GetData<Particles::BaseParticle>(1);
			int index = VerifyItemIndex(2);
			int node = GetClampedInteger(3, -1, objects[items[index].object_number].nmeshes - 1, false);
			part->Attach(index, node);
			return 0;
		}
	};

	struct ParticleAttractToItemFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto part = GetData<Particles::BaseParticle>(1);
			auto item = &items[VerifyItemIndex(2)];
			float radius = GetNumber(3);
			float factor = GetNumber(4);
			part->vel = part->AttractToItem(item, radius, factor);
			return 0;
		}
	};

	struct ParticleAvoidRoomGeometryFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto part = GetData<Particles::BaseParticle>(1);
			int wallMargin = GetInteger(2);
			int floorMargin = GetInteger(3);
			float factor = GetNumber(4);
			part->vel += part->AvoidRoomGeometry(wallMargin, floorMargin, factor);
			return 0;
		}
	};

	struct ParticleCollidedItemFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto part = GetData<Particles::BaseParticle>(1);
			auto item = &items[VerifyItemIndex(2)];
			int radius = GetInteger(3);
			Script::PushBoolean(part->CollidedWithItem(item, radius));
			return 1;
		}
	};

	struct ParticleCollideFloorsFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto part = GetData<Particles::BaseParticle>(1);
			bool bounce = GetBoolean(2);
			float rebound = GetClampedNumber(3, 0.0f, 1.0f, false);
			float minbounce = GetNumber(4);
			int margin = GetInteger(5);
			bool accurate = GetBoolean(6);
			Script::PushBoolean(part->CollideFloors(bounce, rebound, minbounce, margin, accurate));
			return 1;
		}
	};

	struct ParticleCollideWallsFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto part = GetData<Particles::BaseParticle>(1);
			bool bounce = GetBoolean(2);
			float rebound = GetClampedNumber(3, 0.0f, 1.0f, false);
			Script::PushBoolean(part->CollideWalls(bounce, rebound));
			return 1;
		}
	};

	struct ParticleDetachFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto part = GetData<Particles::BaseParticle>(1);
			part->Detach();
			return 0;
		}
	};

	struct ParticleFollowTargetFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto part = GetData<Particles::BaseParticle>(1);
			Vector3f vect = static_cast<Vector3f>(*GetData<LuaObjectClassPosition>(2));
			float maxSpeed = GetNumber(3);
			float distInner = GetNumber(4);
			float distOuter = GetNumber(5);
			part->vel = part->FollowTarget(vect, maxSpeed, distInner, distOuter);
			return 0;
		}
	};

	struct ParticleHomingFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto part = GetData<Particles::BaseParticle>(1);
			auto item = &items[VerifyItemIndex(2)];
			int node = item ? GetClampedInteger(3, 0, objects[item->object_number].nmeshes, false) : GetInteger(3);
			float factor = GetNumber(4);
			float accel = GetNumber(5);
			bool predict = GetBoolean(6);
			part->TargetHoming(item, node, factor, accel, predict);
			return 0;
		}
	};

	struct ParticleKillFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto part = GetData<Particles::BaseParticle>(1);
			Script::DeleteTable(part->data.table);
			part->lifeCounter = 0;
			return 0;
		}
	};

	struct ParticleLimitSpeedFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto part = GetData<Particles::BaseParticle>(1);
			float speedMax = GetNumber(2);
			part->LimitSpeed(speedMax);
			return 0;
		}
	};

	struct ParticleWindVelocityFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto part = GetData<Particles::BaseParticle>(1);
			float factor = GetNumber(2);
			part->vel += part->WindVelocities(factor);
			return 0;
		}
	};

	struct PerformTriggerGroupFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			int indexTG = GetClampedInteger(1, 1, 9999, true);
			bool state = PerformTriggerGroup(indexTG) ? true : false;
			Script::PushBoolean(state);
			return 1;
		}
	};

	struct PrintFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::Print();
			return 0;
		}
	};

	struct RadToDegFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushNumber(RadToDeg(GetNumber(1)));
			return 1;
		}
	};

	struct RandfloatFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			float lower = GetNumber(1);
			float upper = GetNumber(2);
			Script::PushNumber(fabsf(upper - lower) * GetRandom() + fminf(lower, upper));
			return 1;
		}
	};

	struct RandintFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			float lower = (float)GetInteger(1);
			float upper = (float)GetInteger(2);
			Script::PushInteger((int)floorf((fabsf(upper - lower) + 1) * GetRandom() + fminf(lower, upper)));
			return 1;
		}
	};

	struct RandomNegateFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushNumber(RandomNegate(GetNumber(1)));
			return 1;
		}
	};

	struct RequireFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			CheckCaller(FunctionType::FUNCTION_LEVEL, "require");
			RequireModule(1);
			return 1;
		}
	};

	struct RoundFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushInteger(lroundf(GetNumber(1)));
			return 1;
		}
	};

	struct SelectItemFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Trng.pGlobTomb4->ItemIndexSelected = Trng.pGlobTomb4->IndiceItemCondizione = VerifyItemIndex(1);
			return 0;
		}
	};

	struct SetLogLevelFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			CheckCaller(FunctionType::FUNCTION_LEVEL, "setLogLevel");
			LogLevel requested = static_cast<LogLevel>(GetClampedInteger(1, LogLevel::LOG_TRACE, LogLevel::LOG_FATAL, false));
			Logger::SetLogLevel(requested);
			return 0;
		}
	};

	struct SinFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushNumber(GetMathResult(1, sinf));
			return 1;
		}
	};

	struct SmoothStepFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushNumber(SmoothStep(GetNumber(1)));
			return 1;
		}
	};

	struct SoundEffectFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			phd_vector vec;
			phd_vector* pvec = nullptr ;

			int sfxIndex = GetInteger(1);

			if (!Script::IsNil(2))
			{
				auto pos = GetData<LuaObjectClassPosition>(2);
				vec.x = lround(pos->GetX());
				vec.y = lround(pos->GetY());
				vec.z = lround(pos->GetZ());
				pvec = &vec;
			}

			int flags = GetInteger(3);

			SoundEffect(sfxIndex, pvec, flags);
			return 0;
		}
	};

	struct SphericalToCartesianFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			float r = GetNumber(1);
			float theta = GetNumber(2);
			float phi = GetNumber(3);
			ConstructManagedData<Vector3f>(SphericalToCartesian(r, theta, phi));
			return 1;
		}
	};

	struct SplinePosItemsFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			float t = GetNumber(1);
			std::vector<Vector3f> vecList(GetTable(2));
			for (ulong i = 0; i < vecList.size(); i++)
				vecList[i] = GetItemPos(i + 3);
			ConstructManagedData<Vector3f>(SplinePos(t, vecList.data(), vecList.size()));
			return 1;
		}
	};

	struct SplinePosVectorsFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			float t = GetNumber(1);
			std::vector<Vector3f> vecList(GetTable(2));
			for (ulong i = 0; i < vecList.size(); i++)
				vecList[i] = static_cast<Vector3f>(*GetData<LuaObjectClassPosition>(i + 3));
			Vector3f vec = SplinePos(t, vecList.data(), vecList.size());
			ConstructManagedData<Vector3f>(vec);
			return 1;
		}
	};

	struct SplineVelItemsFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			float t = GetNumber(1);
			float duration = GetNumber(2);
			std::vector<Vector3f> vecList(GetTable(3));
			for (ulong i = 0; i < vecList.size(); i++)
				vecList[i] = GetItemPos(i + 4);
			ConstructManagedData<Vector3f>(SplineVel(t, duration, vecList.data(), vecList.size()));
			return 1;
		}
	};

	struct SplineVelVectorsFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			float t = GetNumber(1);
			float duration = GetNumber(2);
			std::vector<Vector3f> vecList(GetTable(3));
			for (ulong i = 0; i < vecList.size(); i++)
				vecList[i] = static_cast<Vector3f>(*GetData<LuaObjectClassPosition>(i + 4));
			Vector3f vec = SplineVel(t, duration, vecList.data(), vecList.size());
			ConstructManagedData<Vector3f>(vec);
			return 1;
		}
	};

	struct SqrtFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushNumber(GetMathResult(1, sqrtf));
			return 1;
		}
	};

	struct TanFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			Script::PushNumber(GetMathResult(1, tanf));
			return 1;
		}
	};

	struct TestCollisionSpheresFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto item = &items[VerifyItemIndex(1)];
			Vector3f vec = static_cast<Vector3f>(*GetData<LuaObjectClassPosition>(2));
			float radius = GetNumber(3);
			Script::PushInteger(TestCollisionSpheres(item, vec, radius));
			return 1;
		}
	};

	struct TriggerDynamicFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			int x = GetInteger(1);
			int y = GetInteger(2);
			int z = GetInteger(3);
			int intensity = GetInteger(4);
			int red = GetClampedInteger(5, 0, UINT8_MAX, false);
			int green = GetClampedInteger(6, 0, UINT8_MAX, false);
			int blue = GetClampedInteger(7, 0, UINT8_MAX, false);
			TriggerDynamic(x, y, z, intensity, red, green, blue);
			return 0;
		}
	};

	struct TriggerShockwaveFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			int x = GetInteger(1);
			int y = GetInteger(2);
			int z = GetInteger(3);
			int innerRad = GetClampedInteger(4, INT16_MIN, INT16_MAX, false);
			int outerRad = GetClampedInteger(5, INT16_MIN, INT16_MAX, false);
			int speed = GetInteger(6);
			int life = GetClampedInteger(7, 0, UINT8_MAX, false);
			int red = GetClampedInteger(8, 0, UINT8_MAX, false);
			int green = GetClampedInteger(9, 0, UINT8_MAX, false);
			int blue = GetClampedInteger(10, 0, UINT8_MAX, false);
			int xRot = RadToShort(GetNumber(11));
			int flags = GetInteger(12);
			auto vec = phd_vector(x, y, z);
			TriggerShockwave(&vec, innerRad | (outerRad << 16), speed, RGBA(red, green, blue, life), xRot | (flags << 16));
			return 0;
		}
	};

	struct VectorCrossFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto vec1 = GetData<LuaObjectClassPosition>(1);
			auto vec2 = GetData<LuaObjectClassPosition>(2);
			float x1 = vec1->GetX(), y1 = vec1->GetY(), z1 = vec1->GetZ();
			float x2 = vec2->GetX(), y2 = vec2->GetY(), z2 = vec2->GetZ();
			float x = y1 * z2 - z1 * y2;
			float y = z1 * x2 - x1 * z2;
			float z = x1 * y2 - y1 * x2;
			ConstructManagedData<Vector3f>(x, y, z);
			return 1;
		}
	};

	struct VectorDotFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto vec1 = GetData<LuaObjectClassPosition>(1);
			auto vec2 = GetData<LuaObjectClassPosition>(2);
			float dot = vec1->GetX() * vec2->GetX() + vec1->GetY() * vec2->GetY() + vec1->GetZ() * vec2->GetZ();
			Script::PushNumber(dot);
			return 1;
		}
	};

	struct VectorLengthFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto vec = GetData<LuaObjectClassPosition>(1);
			float x = vec->GetX(), y = vec->GetY(), z = vec->GetZ();
			float length = sqrtf(x * x + y * y + z * z);
			Script::PushNumber(length);
			return 1;
		}
	};

	struct VectorNormalizeFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto vec = GetData<LuaObjectClassPosition>(1);
			float x = vec->GetX(), y = vec->GetY(), z = vec->GetZ();
			if (x || y || z)
			{
				float norm = 1.0f / sqrtf(x * x + y * y + z * z);
				ConstructManagedData<Vector3f>(x * norm, y * norm, z * norm);
			}	
			else
				ConstructManagedData<Vector3f>(); // return zero-vector
			return 1;
		}
	};

	struct WrapAngleFunction final : public LuaObjectFunction
	{
		int Call() final
		{
			auto angle = fmodf(GetNumber(1) + (float)M_PI, 2.0f * (float)M_PI);
			if (angle < 0.0f)
				angle += 2.0f * (float)M_PI;
			Script::PushNumber(angle - (float)M_PI);
			return 1;
		}
	};

	AbsFunction AbsFunc;
	AcosFunction AcosFunc;
	AsinFunction AsinFunc;
	AtanFunction AtanFunc;
	Atan2Function Atan2Func;
	BindFunctionFunction BindFunctionFunc;
	BindGroupFunction BindGroupFunc;
	BoidAlignmentFunction BoidAlignmentFunc;
	BoidCohesionFunction BoidCohesionFunc;
	BoidSeparationFunction BoidSeparationFunc;
	CbrtFunction CbrtFunc;
	CeilFunction CeilFunc;
	ClampFunction ClampFunc;
	CosFunction CosFunc;
	CreateColorFunction CreateColorFunc;
	CreateColorHSVFunction CreateColorHSVFunc;
	CreateGroupFunction CreateGroupFunc;
	CreateMeshPartFunction CreateMeshPartFunc;
	CreateModuleFunction CreateModuleFunc;
	CreatePerlinNoiseFunction CreatePerlinNoiseFunc;
	CreateSimplexNoiseFunction CreateSimplexNoiseFunc;
	CreateSpritePartFunction CreateSpritePartFunc;
	CreateVectorFunction CreateVectorFunc;
	DegToRadFunction DegToRadFunc;
	DistanceFunction DistanceFunc;
	DistCompareFunction DistCompareFunc;
	ExpFunction ExpFunc;
	FindNearestTargetFunction FindNearestTargetFunc;
	FloorFunction FloorFunc;
	FmodFunction FmodFunc;
	GetGameTickFunction GetGameTickFunc;
	GetItemInfoFunction GetItemInfoFunc;
	GetItemJointPosFunction GetItemJointPosFunc;
	GetItemRoomFunction GetItemRoomFunc;
	GetLaraIndexFunction GetLaraIndexFunc;
	GetSelectedItemFunction GetSelectedItemFunc;
	GetTombIndexFunction GetTombIndexFunc;
	IntervalFunction IntervalFunc;
	InvokeInitFunction InvokeInitFunc;
	KillPartsOfGroupFunction KillPartsOfGroupFunc;
	LaraBurnFunction LaraBurnFunc;
	LerpFunction LerpFunc;
	LerpInverseFunction LerpInverseFunc;
	LogFunction LogFunc;
	MeshAlignVelocityFunction MeshAlignVelocityFunc;
	MeshLookAtTargetFunction MeshLookAtTargetFunc;
	MeshShatterFunction MeshShatterFunc;
	NoiseFunction NoiseFunc;
	NoiseCurlFunction NoiseCurlFunc;
	NoiseCurlTimeFunction NoiseCurlTimeFunc;
	ParticleAbsPosFunction ParticleAbsPosFunc;
	ParticleAnimateFunction ParticleAnimateFunc;
	ParticleAttachFunction ParticleAttachFunc;
	ParticleAttractToItemFunction ParticleAttractToItemFunc;
	ParticleAvoidRoomGeometryFunction ParticleAvoidRoomGeometryFunc;
	ParticleCollidedItemFunction ParticleCollidedItemFunc;
	ParticleCollideFloorsFunction ParticleCollideFloorsFunc;
	ParticleCollideWallsFunction ParticleCollideWallsFunc;
	ParticleDetachFunction ParticleDetachFunc;
	ParticleFollowTargetFunction ParticleFollowTargetFunc;
	ParticleHomingFunction ParticleHomingFunc;
	ParticleKillFunction ParticleKillFunc;
	ParticleLimitSpeedFunction ParticleLimitSpeedFunc;
	SplinePosItemsFunction SplinePosItemsFunc;
	SplinePosVectorsFunction SplinePosVectorsFunc;
	SplineVelItemsFunction SplineVelItemsFunc;
	SplineVelVectorsFunction SplineVelVectorsFunc;
	ParticleWindVelocityFunction ParticleWindVelocityFunc;
	PerformTriggerGroupFunction PerformTriggerGroupFunc;
	PrintFunction PrintFunc;
	RadToDegFunction RadToDegFunc;
	RandfloatFunction RandfloatFunc;
	RandintFunction RandintFunc;
	RandomNegateFunction RandomNegateFunc;
	RequireFunction RequireFunc;
	RoundFunction RoundFunc;
	SelectItemFunction SelectItemFunc;
	SetLogLevelFunction SetLogLevelFunc;
	SinFunction SinFunc;
	SmoothStepFunction SmoothStepFunc;
	SoundEffectFunction SoundEffectFunc;
	SphericalToCartesianFunction SphericalToCartesianFunc;
	SqrtFunction SqrtFunc;
	TanFunction TanFunc;
	TestCollisionSpheresFunction TestCollisionSpheresFunc;
	TriggerDynamicFunction TriggerDynamicFunc;
	TriggerShockwaveFunction TriggerShockwaveFunc;
	VectorCrossFunction VectorCrossFunc;
	VectorDotFunction VectorDotFunc;
	VectorLengthFunction VectorLengthFunc;
	VectorNormalizeFunction VectorNormalizeFunc;
	WrapAngleFunction WrapAngleFunc;

	LuaObject* RetrieveFunction(const char* field)
	{
		switch (field[0])
		{
		case 'a':
			if (!strcmp(field, "abs"))
				return &AbsFunc;
			if (!strcmp(field, "acos"))
				return &AcosFunc;
			if (!strcmp(field, "asin"))
				return &AsinFunc;
			if (!strcmp(field, "atan"))
				return &AtanFunc;
			if (!strcmp(field, "atan2"))
				return &Atan2Func;
			break;

		case 'b':
			if (!strcmp(field, "bindFunction"))
				return &BindFunctionFunc;
			if (!strcmp(field, "bindGroup"))
				return &BindGroupFunc;
			if (!strcmp(field, "boidAlignment"))
				return &BoidAlignmentFunc;
			if (!strcmp(field, "boidCohesion"))
				return &BoidCohesionFunc;
			if (!strcmp(field, "boidSeparation"))
				return &BoidSeparationFunc;
			break;

		case 'c':
			if (!strcmp(field, "cbrt"))
				return &CbrtFunc;
			if (!strcmp(field, "ceil"))
				return &CeilFunc;
			if (!strcmp(field, "clamp"))
				return &ClampFunc;
			if (!strcmp(field, "cos"))
				return &CosFunc;
			if (!strcmp(field, "createColor"))
				return &CreateColorFunc;
			if (!strcmp(field, "createColorHSV"))
				return &CreateColorHSVFunc;
			if (!strcmp(field, "createGroup"))
				return &CreateGroupFunc;
			if (!strcmp(field, "createMeshPart"))
				return &CreateMeshPartFunc;
			if (!strcmp(field, "createModule"))
				return &CreateModuleFunc;
			if (!strcmp(field, "createPerlinNoise"))
				return &CreatePerlinNoiseFunc;
			if (!strcmp(field, "createSimplexNoise"))
				return &CreateSimplexNoiseFunc;
			if (!strcmp(field, "createSpritePart"))
				return &CreateSpritePartFunc;
			if (!strcmp(field, "createVector"))
				return &CreateVectorFunc;
			break;

		case 'd':
			if (!strcmp(field, "degToRad"))
				return &DegToRadFunc;
			if (!strcmp(field, "distance"))
				return &DistanceFunc;
			if (!strcmp(field, "distCompare"))
				return &DistCompareFunc;
			break;

		case 'e':
			if (!strcmp(field, "exp"))
				return &ExpFunc;
			break;

		case 'f':
			if (!strcmp(field, "findNearestTarget"))
				return &FindNearestTargetFunc;
			if (!strcmp(field, "floor"))
				return &FindNearestTargetFunc;
			break;

		case 'g':
			if (!strcmp(field, "getGameTick"))
				return &GetGameTickFunc;
			if (!strcmp(field, "getItemJointPosition"))
				return &GetItemJointPosFunc;
			if (!strcmp(field, "getItemInfo"))
				return &GetItemInfoFunc;
			if (!strcmp(field, "getItemRoom"))
				return &GetItemRoomFunc;
			if (!strcmp(field, "getLaraIndex"))
				return &GetLaraIndexFunc;
			if (!strcmp(field, "getSelectedItemIndex"))
				return &GetSelectedItemFunc;
			if (!strcmp(field, "getTombIndex"))
				return &GetTombIndexFunc;
			break;

		case 'i':
			if (!strcmp(field, "interval"))
				return &IntervalFunc;
			if (!strcmp(field, "invokeInit"))
				return &InvokeInitFunc;
			break;

		case 'k':
			if (!strcmp(field, "killPartsOfGroup"))
				return &KillPartsOfGroupFunc;
			break;

		case 'l':
			if (!strcmp(field, "laraBurn"))
				return &LaraBurnFunc;
			if (!strcmp(field, "lerp"))
				return &LerpFunc;
			if (!strcmp(field, "lerpInverse"))
				return &LerpInverseFunc;
			if (!strcmp(field, "log"))
				return &LogFunc;
			break;

		case 'm':
			if (!strcmp(field, "meshAlignVelocity"))
				return &MeshAlignVelocityFunc;
			if (!strcmp(field, "meshLookAtTarget"))
				return &MeshLookAtTargetFunc;
			if (!strcmp(field, "meshShatter"))
				return &MeshShatterFunc;
			break;

		case 'n':
			if (!strcmp(field, "noise"))
				return &NoiseFunc;
			if (!strcmp(field, "noiseCurl"))
				return &NoiseCurlFunc;
			if (!strcmp(field, "noiseCurlTime"))
				return &NoiseCurlTimeFunc;
			break;

		case 'p':
			if (!strcmp(field, "particleAbsPosition"))
				return &ParticleAbsPosFunc;
			if (!strcmp(field, "particleAnimate"))
				return &ParticleAnimateFunc;
			if (!strcmp(field, "particleAttach"))
				return &ParticleAttachFunc;
			if (!strcmp(field, "particleAttractToItem"))
				return &ParticleAttractToItemFunc;
			if (!strcmp(field, "particleAvoidRoomGeometry"))
				return &ParticleAvoidRoomGeometryFunc;
			if (!strcmp(field, "particleCollidedItem"))
				return &ParticleCollidedItemFunc;
			if (!strcmp(field, "particleCollideFloors"))
				return &ParticleCollideFloorsFunc;
			if (!strcmp(field, "particleCollideWalls"))
				return &ParticleCollideWallsFunc;
			if (!strcmp(field, "particleDetach"))
				return &ParticleDetachFunc;
			if (!strcmp(field, "particleFollow"))
				return &ParticleFollowTargetFunc;
			if (!strcmp(field, "particleHoming"))
				return &ParticleHomingFunc;
			if (!strcmp(field, "particleKill"))
				return &ParticleKillFunc;
			if (!strcmp(field, "particleLimitSpeed"))
				return &ParticleLimitSpeedFunc;
			if (!strcmp(field, "particleWind"))
				return &ParticleWindVelocityFunc;
			if (!strcmp(field, "performTriggerGroup"))
				return &PerformTriggerGroupFunc;
			if (!strcmp(field, "print"))
				return &PrintFunc;
			break;

		case 'r':
			if (!strcmp(field, "radToDeg"))
				return &RadToDegFunc;
			if (!strcmp(field, "randfloat"))
				return &RandfloatFunc;
			if (!strcmp(field, "randint"))
				return &RandintFunc;
			if (!strcmp(field, "randomNegate"))
				return &RandomNegateFunc;
			if (!strcmp(field, "require"))
				return &RequireFunc;
			if (!strcmp(field, "round"))
				return &RoundFunc;
			break;

		case 's':
			if (!strcmp(field, "setLogLevel"))
				return &SetLogLevelFunc;
			if (!strcmp(field, "setSelectedItem"))
				return &SelectItemFunc;
			if (!strcmp(field, "sin"))
				return &SinFunc;
			if (!strcmp(field, "smoothStep"))
				return &SmoothStepFunc;
			if (!strcmp(field, "soundEffect"))
				return &SoundEffectFunc;
			if (!strcmp(field, "sphericalToCartesian"))
				return &SphericalToCartesianFunc;
			if (!strcmp(field, "splinePosItems"))
				return &SplinePosItemsFunc;
			if (!strcmp(field, "splinePosVectors"))
				return &SplinePosVectorsFunc;
			if (!strcmp(field, "splineVelItems"))
				return &SplineVelItemsFunc;
			if (!strcmp(field, "splineVelVectors"))
				return &SplineVelVectorsFunc;
			if (!strcmp(field, "sqrt"))
				return &SqrtFunc;
			break;

		case 't':
			if (!strcmp(field, "tan"))
				return &TanFunc;
			if (!strcmp(field, "testCollisionSpheres"))
				return &TestCollisionSpheresFunc;
			if (!strcmp(field, "triggerDynamicLight"))
				return &TriggerDynamicFunc;
			if (!strcmp(field, "triggerShockwave"))
				return &TriggerShockwaveFunc;
			break;

		case 'v':
			if (!strcmp(field, "vectorCross"))
				return &VectorCrossFunc;
			if (!strcmp(field, "vectorDot"))
				return &VectorDotFunc;
			if (!strcmp(field, "vectorLength"))
				return &VectorLengthFunc;
			if (!strcmp(field, "vectorNormalize"))
				return &VectorNormalizeFunc;
			break;

		case 'w':
			if (!strcmp(field, "wrapAngle"))
				return &WrapAngleFunc;
			break;
		}
		return nullptr;
	}
}
