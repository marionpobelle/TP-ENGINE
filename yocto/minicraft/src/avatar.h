#ifndef __AVATAR__
#define __AVATAR__

#include "engine/utils/types_3d.h"
#include "engine/render/camera.h"
#include "engine/utils/timer.h"
#include "world.h"

class MAvatar
{
public:
	YVec3f Position;
	YVec3f Speed;

	bool Move;
	bool Jump;
	float Height;
	float CurrentHeight;
	float Width;
	bool avance;
	bool recule;
	bool gauche;
	bool droite;
	bool Standing;
	bool InWater;
	bool Crouch;
	bool Run;

	YCamera* Cam;
	MWorld* World;

	YTimer _TimerStanding;

	MAvatar(YCamera* cam, MWorld* world)
	{
		Position = YVec3f((MWorld::MAT_SIZE_METERS) / 2, (MWorld::MAT_SIZE_METERS) / 2, (MWorld::MAT_HEIGHT_METERS));
		Height = 1.8f;
		CurrentHeight = Height;
		Width = 0.3f;
		Cam = cam;
		avance = false;
		recule = false;
		gauche = false;
		droite = false;
		Standing = false;
		Jump = false;
		World = world;
		InWater = false;
		Crouch = false;
		Run = false;
	}


	void render(void)
	{
		//YRenderer::getInstance()->drawSolidCube(Width / 2);
	}

	void update(float elapsed)
	{
		if (elapsed > 1.0f / 60.0f)
			elapsed = 1.0f / 60.0f;

		//Par defaut, on applique la gravité (-100 sur Z), la moitie si dans l'eau
		YVec3f force = YVec3f(0, 0, -1) * 9.81f;
		if (InWater)
			force = YVec3f(0, 0, -1) * 0.5f;

		float lastheight = CurrentHeight;
		CurrentHeight = Height;
		if (Crouch)
			CurrentHeight = Height / 2;

		//Pour ne pas s'enfoncer dans le sol en une frame quand on se releve
		if (CurrentHeight > lastheight)
			Position.Z += Height / 4;

		//Si l'avatar n'est pas au sol, alors il ne peut pas sauter
		/*if (!Standing && !InWater) //On jump tout le temps
		Jump = false;*/

		float accel = 40;
		if (Crouch)
			accel = 20;
		if (!Standing)
			accel = 5;
		if (Run)
			accel = 80;

		YVec3f forward(Cam->Direction.X, Cam->Direction.Y, 0);
		forward.normalize();
		YVec3f right(Cam->RightVec.X, Cam->RightVec.Y, 0);
		right.normalize();

		//On applique les controles en fonction de l'accélération
		if (avance)
			force += forward * accel;
		if (recule)
			force += forward * -accel;
		if (gauche)
			force += right * -accel;
		if (droite)
			force += right * accel;


		//On applique le jump
		if (Jump)
		{
			force += YVec3f(0, 0, 1) * 10.0f / elapsed; //(impulsion, pas fonction du temps)
			Jump = false;
		}

		//On applique les forces en fonction du temps écoulé
		Speed += force * elapsed;

		//On met une limite a sa vitesse horizontale
		float speedmax = 70;
		if (Crouch)
			speedmax = 45;
		if (!Standing)
			speedmax = 70;
		if (Run)
			speedmax = 140;

		YVec3f horSpeed = Speed;
		horSpeed.Z = 0;
		if (horSpeed.getSize() > speedmax)
		{
			horSpeed.normalize();
			horSpeed *= speedmax;
			Speed.X = horSpeed.X;
			Speed.Y = horSpeed.Y;
		}

		//On le déplace, en sauvegardant son ancienne position
		YVec3f oldPosition = Position;
		Position += (Speed * elapsed);

		//YLog::log(YLog::ENGINE_INFO, ("zS " + toString(Speed.Z)).c_str());

		if (_TimerStanding.getElapsedSeconds() > 0.01)
			Standing = false;
		for (int pass = 0; pass < 2; pass++)
		{
			for (int i = 0; i < 6; i++)
			{
				float valueColMin = 0;
				MWorld::MAxis axis = World->getMinCol(Position, Speed, Width, CurrentHeight, valueColMin, i < 3);
				//YLog::log(YLog::ENGINE_INFO,"check");
				if (axis != 0)
				{
					//valueColMin = nymax(nyabs(valueColMin), 0.0001f) * (valueColMin > 0 ? 1.0f : -1.0f);
					if (axis & MWorld::AXIS_X)
					{
						//YLog::log(YLog::ENGINE_INFO,("x " + toString(valueColMin)).c_str());
						Position.X += valueColMin + 0.001 * sign(valueColMin);
						Speed.X = 0;
					}
					if (axis & MWorld::AXIS_Y)
					{
						//YLog::log(YLog::ENGINE_INFO, ("y " + toString(valueColMin)).c_str());
						Position.Y += valueColMin + 0.001 * sign(valueColMin);
						Speed.Y = 0;
					}
					if (axis & MWorld::AXIS_Z)
					{
						//YLog::log(YLog::ENGINE_INFO, ("z " + toString(valueColMin)).c_str());
						Speed.Z = 0;
						Position.Z += valueColMin + 0.001 * sign(valueColMin);
						Standing = true;
						_TimerStanding.start();
					}
				}
			}
		}

		int x = (int)(Position.X / MCube::CUBE_SIZE);
		int y = (int)(Position.Y / MCube::CUBE_SIZE);
		int z = (int)(Position.Z / MCube::CUBE_SIZE);

		//Escaliers
		float floatheight = 1.0f;
		float zpied = Position.Z - (Height / 2.0f);
		float zfloatpied = zpied - floatheight;
		int izCubeDessousFloat = (int)((zfloatpied) / MCube::CUBE_SIZE);
		float zCubeDessous2Float = zfloatpied - MCube::CUBE_SIZE;
		int izCubeDessous2Float = (int)(zCubeDessous2Float / MCube::CUBE_SIZE);


		//Si on est dans l'eau
		InWater = false;
		if (World->getCube(x, y, z)->getType() == MCube::CUBE_EAU)
			InWater = true;

		if (InWater)
		{
			//Standing = true;
			Speed *= pow(0.2f, elapsed);
		}

		if (Standing)
			Speed *= pow(0.01f, elapsed);

	}
};

#endif