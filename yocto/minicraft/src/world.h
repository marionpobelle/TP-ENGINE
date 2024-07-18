#ifndef __WORLD_H__
#define __WORLD_H__

#include "external/gl/glew.h"
#include "external/gl/freeglut.h" 
#include "engine/utils/types_3d.h"
#include "cube.h"
#include "chunk.h"
#include "noise/modified_perlin.h"

class MWorld
{
public :
	typedef uint8 MAxis;
	static const int AXIS_X = 0b00000001;
	static const int AXIS_Y = 0b00000010;
	static const int AXIS_Z = 0b00000100;

	#ifdef _DEBUG
	static const int MAT_SIZE = 1; //en nombre de chunks
	#else
	static const int MAT_SIZE = 3; //en nombre de chunks
	#endif // DEBUG

	static const int MAT_HEIGHT = 1; //en nombre de chunks
	static const int MAT_SIZE_CUBES = (MAT_SIZE * MChunk::CHUNK_SIZE);
	static const int MAT_HEIGHT_CUBES = (MAT_HEIGHT * MChunk::CHUNK_SIZE);
	static const int MAT_SIZE_METERS = (MAT_SIZE * MChunk::CHUNK_SIZE * MCube::CUBE_SIZE);
	static const int MAT_HEIGHT_METERS = (MAT_HEIGHT * MChunk::CHUNK_SIZE  * MCube::CUBE_SIZE);

	MChunk * Chunks[MAT_SIZE][MAT_SIZE][MAT_HEIGHT];

	MPerlin Perlin;
	float PerlinHeightCut = 150.0f;
	
	MWorld()
	{
		//On crée les chunks
		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
					Chunks[x][y][z] = new MChunk(x,y,z);

		for(int x=0;x<MAT_SIZE;x++)
			for(int y=0;y<MAT_SIZE;y++)
				for(int z=0;z<MAT_HEIGHT;z++)
				{
					MChunk * cxPrev = NULL;
					if(x > 0)
						cxPrev = Chunks[x-1][y][z];
					MChunk * cxNext = NULL;
					if(x < MAT_SIZE-1)
						cxNext = Chunks[x+1][y][z];

					MChunk * cyPrev = NULL;
					if(y > 0)
						cyPrev = Chunks[x][y-1][z];
					MChunk * cyNext = NULL;
					if(y < MAT_SIZE-1)
						cyNext = Chunks[x][y+1][z];

					MChunk * czPrev = NULL;
					if(z > 0)
						czPrev = Chunks[x][y][z-1];
					MChunk * czNext = NULL;
					if(z < MAT_HEIGHT-1)
						czNext = Chunks[x][y][z+1];

					Chunks[x][y][z]->setVoisins(cxPrev,cxNext,cyPrev,cyNext,czPrev,czNext);
				}

					
	}

	inline MCube * getCube(int x, int y, int z)
	{	
		if(x < 0)x = 0;
		if(y < 0)y = 0;
		if(z < 0)z = 0;
		if(x >= MAT_SIZE * MChunk::CHUNK_SIZE) x = (MAT_SIZE * MChunk::CHUNK_SIZE)-1;
		if(y >= MAT_SIZE * MChunk::CHUNK_SIZE) y = (MAT_SIZE * MChunk::CHUNK_SIZE)-1;
		if(z >= MAT_HEIGHT * MChunk::CHUNK_SIZE) z = (MAT_HEIGHT * MChunk::CHUNK_SIZE)-1;

		return &(Chunks[x / MChunk::CHUNK_SIZE][y / MChunk::CHUNK_SIZE][z / MChunk::CHUNK_SIZE]->_Cubes[x % MChunk::CHUNK_SIZE][y % MChunk::CHUNK_SIZE][z % MChunk::CHUNK_SIZE]);
	}

    void updateCube(int x, int y, int z)
	{	
		if(x < 0)x = 0;
		if(y < 0)y = 0;
		if(z < 0)z = 0;
		if(x >= MAT_SIZE * MChunk::CHUNK_SIZE)x = (MAT_SIZE * MChunk::CHUNK_SIZE)-1;
		if(y >= MAT_SIZE * MChunk::CHUNK_SIZE)y = (MAT_SIZE * MChunk::CHUNK_SIZE)-1;
		if (z >= MAT_HEIGHT * MChunk::CHUNK_SIZE)z = (MAT_HEIGHT * MChunk::CHUNK_SIZE) - 1; 
		
		Chunks[x / MChunk::CHUNK_SIZE][y / MChunk::CHUNK_SIZE][z / MChunk::CHUNK_SIZE]->disableHiddenCubes();
		Chunks[x / MChunk::CHUNK_SIZE][y / MChunk::CHUNK_SIZE][z / MChunk::CHUNK_SIZE]->toVbos();
		
		if ((x - 1) / MChunk::CHUNK_SIZE > 0 && (x - 1) / MChunk::CHUNK_SIZE != x / MChunk::CHUNK_SIZE) {
			Chunks[(x - 1) / MChunk::CHUNK_SIZE][y / MChunk::CHUNK_SIZE][z / MChunk::CHUNK_SIZE]->disableHiddenCubes();
			Chunks[(x - 1) / MChunk::CHUNK_SIZE][y / MChunk::CHUNK_SIZE][z / MChunk::CHUNK_SIZE]->toVbos();
		}

		if ((x + 1) / MChunk::CHUNK_SIZE < MAT_SIZE && (x + 1) / MChunk::CHUNK_SIZE != x / MChunk::CHUNK_SIZE) {
			Chunks[(x + 1) / MChunk::CHUNK_SIZE][y / MChunk::CHUNK_SIZE][z / MChunk::CHUNK_SIZE]->disableHiddenCubes();
			Chunks[(x + 1) / MChunk::CHUNK_SIZE][y / MChunk::CHUNK_SIZE][z / MChunk::CHUNK_SIZE]->toVbos();
		}

		if ((y - 1) / MChunk::CHUNK_SIZE > 0 && (y - 1) / MChunk::CHUNK_SIZE != y / MChunk::CHUNK_SIZE) {
			Chunks[x / MChunk::CHUNK_SIZE][(y - 1) / MChunk::CHUNK_SIZE][z / MChunk::CHUNK_SIZE]->disableHiddenCubes();
			Chunks[x / MChunk::CHUNK_SIZE][(y - 1) / MChunk::CHUNK_SIZE][z / MChunk::CHUNK_SIZE]->toVbos();
		}

		if ((y + 1) / MChunk::CHUNK_SIZE < MAT_SIZE && (y + 1) / MChunk::CHUNK_SIZE != y / MChunk::CHUNK_SIZE) {
			Chunks[x / MChunk::CHUNK_SIZE][(y + 1) / MChunk::CHUNK_SIZE][z / MChunk::CHUNK_SIZE]->disableHiddenCubes();
			Chunks[x / MChunk::CHUNK_SIZE][(y + 1) / MChunk::CHUNK_SIZE][z / MChunk::CHUNK_SIZE]->toVbos();
		}

		if ((z - 1) / MChunk::CHUNK_SIZE > 0 && (z - 1) / MChunk::CHUNK_SIZE != z / MChunk::CHUNK_SIZE) {
			Chunks[x / MChunk::CHUNK_SIZE][y / MChunk::CHUNK_SIZE][(z - 1) / MChunk::CHUNK_SIZE]->disableHiddenCubes();
			Chunks[x / MChunk::CHUNK_SIZE][y / MChunk::CHUNK_SIZE][(z - 1) / MChunk::CHUNK_SIZE]->toVbos();
		}

		if ((z + 1) / MChunk::CHUNK_SIZE < MAT_HEIGHT && (z + 1) / MChunk::CHUNK_SIZE != z / MChunk::CHUNK_SIZE) {
			Chunks[x / MChunk::CHUNK_SIZE][y / MChunk::CHUNK_SIZE][(z + 1) / MChunk::CHUNK_SIZE]->disableHiddenCubes();
			Chunks[x / MChunk::CHUNK_SIZE][y / MChunk::CHUNK_SIZE][(z + 1) / MChunk::CHUNK_SIZE]->toVbos();
		}
		
	}

	void deleteCube(int x, int y, int z)
	{
		MCube * cube = getCube(x,y,z);
		cube->setType(MCube::CUBE_AIR);
		cube->setDraw(false);
		cube = getCube(x-1,y,z);
		updateCube(x,y,z);	
	}
			
	void init_world(int seed)
	{
		YLog::log(YLog::USER_INFO, (toString("Creation du monde seed ") + toString(seed)).c_str());

		srand(seed);
		Perlin.updateVecs();

		Perlin.setZDecay(MWorld::MAT_HEIGHT_CUBES - 5, 0.5f);

		//Reset du monde
		for (int x = 0; x < MAT_SIZE; x++)
			for (int y = 0; y < MAT_SIZE; y++)
				for (int z = 0; z < MAT_HEIGHT; z++)
					Chunks[x][y][z]->reset();

		for (int x = 0; x < MAT_SIZE_CUBES; x++)
			for (int y = 0; y < MAT_SIZE_CUBES; y++)
				for (int z = 0; z < MAT_HEIGHT_CUBES; z++)
				{
					Perlin.DoPenaltyMiddle = true;
					Perlin.setFreq(0.04f);
					//float val = Perlin.sample((float)x, (float)y, (float)z);
					float val = Perlin.sample((float)x, (float)y, (float)z) - (float)z / PerlinHeightCut;
					Perlin.DoPenaltyMiddle = false;
					Perlin.setFreq(0.2f);
					val -= (1.0f - max(val, Perlin.sample((float)x, (float)y, (float)z))) / 20.0f;

					MCube* cube = getCube(x, y, z);

					if (val > 0.5f)
						cube->setType(MCube::CUBE_HERBE);
					if (val > 0.51f)
						cube->setType(MCube::CUBE_TERRE);
					if (val < 0.5 && z <= 0.1)
						cube->setType(MCube::CUBE_EAU);
					if (val > 0.56)
						cube->setType(MCube::CUBE_EAU);
				}

		for (int x = 0; x < MAT_SIZE; x++)
			for (int y = 0; y < MAT_SIZE; y++)
				for (int z = 0; z < MAT_HEIGHT; z++)
					Chunks[x][y][z]->disableHiddenCubes();

		add_world_to_vbo();
	}

	void add_world_to_vbo(void)
	{
		for (int x = 0; x<MAT_SIZE; x++)
			for (int y = 0; y<MAT_SIZE; y++)
				for (int z = 0; z<MAT_HEIGHT; z++)
				{
					Chunks[x][y][z]->toVbos();
				}
	}
	
	//Boites de collisions plus petites que deux cubes
	MAxis getMinCol(YVec3f pos, YVec3f dir, float width, float height, float & valueColMin, bool oneShot)
	{
		int x = (int)(pos.X / MCube::CUBE_SIZE);
		int y = (int)(pos.Y / MCube::CUBE_SIZE);
		int z = (int)(pos.Z / MCube::CUBE_SIZE);

		int xNext = (int)((pos.X + width / 2.0f) / MCube::CUBE_SIZE);
		int yNext = (int)((pos.Y + width / 2.0f) / MCube::CUBE_SIZE);
		int zNext = (int)((pos.Z + height / 2.0f) / MCube::CUBE_SIZE);

		int xPrev = (int)((pos.X - width / 2.0f) / MCube::CUBE_SIZE);
		int yPrev = (int)((pos.Y - width / 2.0f) / MCube::CUBE_SIZE);
		int zPrev = (int)((pos.Z - height / 2.0f) / MCube::CUBE_SIZE);

		if (x < 0)	x = 0;
		if (y < 0)	y = 0;
		if (z < 0)	z = 0;

		if (xPrev < 0)	xPrev = 0;
		if (yPrev < 0)	yPrev = 0;
		if (zPrev < 0)	zPrev = 0;

		if (xNext < 0)	xNext = 0;
		if (yNext < 0)	yNext = 0;
		if (zNext < 0)	zNext = 0;

		if (x >= MAT_SIZE_CUBES)	x = MAT_SIZE_CUBES - 1;
		if (y >= MAT_SIZE_CUBES)	y = MAT_SIZE_CUBES - 1;
		if (z >= MAT_HEIGHT_CUBES)	z = MAT_HEIGHT_CUBES - 1;

		if (xPrev >= MAT_SIZE_CUBES)	xPrev = MAT_SIZE_CUBES - 1;
		if (yPrev >= MAT_SIZE_CUBES)	yPrev = MAT_SIZE_CUBES - 1;
		if (zPrev >= MAT_HEIGHT_CUBES)	zPrev = MAT_HEIGHT_CUBES - 1;

		if (xNext >= MAT_SIZE_CUBES)	xNext = MAT_SIZE_CUBES - 1;
		if (yNext >= MAT_SIZE_CUBES)	yNext = MAT_SIZE_CUBES - 1;
		if (zNext >= MAT_HEIGHT_CUBES)	zNext = MAT_HEIGHT_CUBES - 1;

		//On fait chaque axe
		MAxis axis = 0x00;
		valueColMin = oneShot ? 0.5f : 10000.0f;
		float seuil = 0.0000001f;
		float prodScalMin = 1.0f;
		if (dir.getSqrSize() > 1)
			dir.normalize();

		//On verif tout les 4 angles de gauche
		if (getCube(xPrev, yPrev, zPrev)->isSolid() ||
			getCube(xPrev, yPrev, zNext)->isSolid() ||
			getCube(xPrev, yNext, zPrev)->isSolid() ||
			getCube(xPrev, yNext, zNext)->isSolid())
		{
			//On verif que resoudre cette collision est utile
			if (!(getCube(xPrev + 1, yPrev, zPrev)->isSolid() ||
				getCube(xPrev + 1, yPrev, zNext)->isSolid() ||
				getCube(xPrev + 1, yNext, zPrev)->isSolid() ||
				getCube(xPrev + 1, yNext, zNext)->isSolid()) || !oneShot)
			{
				float depassement = ((xPrev + 1) * MCube::CUBE_SIZE) - (pos.X - width / 2.0f);
				float prodScal = abs(dir.X);
				if (abs(depassement) > seuil)
					if (abs(depassement) < abs(valueColMin))
					{
						prodScalMin = prodScal;
						valueColMin = depassement;
						axis = AXIS_X;
					}
			}
		}

		//float depassementx2 = (xNext * NYCube::CUBE_SIZE) - (pos.X + width / 2.0f);

		//On verif tout les 4 angles de droite
		if (getCube(xNext, yPrev, zPrev)->isSolid() ||
			getCube(xNext, yPrev, zNext)->isSolid() ||
			getCube(xNext, yNext, zPrev)->isSolid() ||
			getCube(xNext, yNext, zNext)->isSolid())
		{
			//On verif que resoudre cette collision est utile
			if (!(getCube(xNext - 1, yPrev, zPrev)->isSolid() ||
				getCube(xNext - 1, yPrev, zNext)->isSolid() ||
				getCube(xNext - 1, yNext, zPrev)->isSolid() ||
				getCube(xNext - 1, yNext, zNext)->isSolid()) || !oneShot)
			{
				float depassement = (xNext * MCube::CUBE_SIZE) - (pos.X + width / 2.0f);
				float prodScal = abs(dir.X);
				if (abs(depassement) > seuil)
					if (abs(depassement) < abs(valueColMin))
					{
						prodScalMin = prodScal;
						valueColMin = depassement;
						axis = AXIS_X;
					}
			}
		}

		//float depassementy1 = (yNext * NYCube::CUBE_SIZE) - (pos.Y + width / 2.0f);

		//On verif tout les 4 angles de devant
		if (getCube(xPrev, yNext, zPrev)->isSolid() ||
			getCube(xPrev, yNext, zNext)->isSolid() ||
			getCube(xNext, yNext, zPrev)->isSolid() ||
			getCube(xNext, yNext, zNext)->isSolid())
		{
			//On verif que resoudre cette collision est utile
			if (!(getCube(xPrev, yNext - 1, zPrev)->isSolid() ||
				getCube(xPrev, yNext - 1, zNext)->isSolid() ||
				getCube(xNext, yNext - 1, zPrev)->isSolid() ||
				getCube(xNext, yNext - 1, zNext)->isSolid()) || !oneShot)
			{
				float depassement = (yNext * MCube::CUBE_SIZE) - (pos.Y + width / 2.0f);
				float prodScal = abs(dir.Y);
				if (abs(depassement) > seuil)
					if (abs(depassement) < abs(valueColMin))
					{
						prodScalMin = prodScal;
						valueColMin = depassement;
						axis = AXIS_Y;
					}
			}
		}

		//float depassementy2 = ((yPrev + 1) * NYCube::CUBE_SIZE) - (pos.Y - width / 2.0f);

		//On verif tout les 4 angles de derriere
		if (getCube(xPrev, yPrev, zPrev)->isSolid() ||
			getCube(xPrev, yPrev, zNext)->isSolid() ||
			getCube(xNext, yPrev, zPrev)->isSolid() ||
			getCube(xNext, yPrev, zNext)->isSolid())
		{
			//On verif que resoudre cette collision est utile
			if (!(getCube(xPrev, yPrev + 1, zPrev)->isSolid() ||
				getCube(xPrev, yPrev + 1, zNext)->isSolid() ||
				getCube(xNext, yPrev + 1, zPrev)->isSolid() ||
				getCube(xNext, yPrev + 1, zNext)->isSolid()) || !oneShot)
			{
				float depassement = ((yPrev + 1) * MCube::CUBE_SIZE) - (pos.Y - width / 2.0f);
				float prodScal = abs(dir.Y);
				if (abs(depassement) > seuil)
					if (abs(depassement) < abs(valueColMin))
					{
						prodScalMin = prodScal;
						valueColMin = depassement;
						axis = AXIS_Y;
					}
			}
		}

		//On verif tout les 4 angles du haut
		if (getCube(xPrev, yPrev, zNext)->isSolid() ||
			getCube(xPrev, yNext, zNext)->isSolid() ||
			getCube(xNext, yPrev, zNext)->isSolid() ||
			getCube(xNext, yNext, zNext)->isSolid())
		{
			//On verif que resoudre cette collision est utile
			if (!(getCube(xPrev, yPrev, zNext - 1)->isSolid() ||
				getCube(xPrev, yNext, zNext - 1)->isSolid() ||
				getCube(xNext, yPrev, zNext - 1)->isSolid() ||
				getCube(xNext, yNext, zNext - 1)->isSolid()) || !oneShot)
			{
				float depassement = (zNext * MCube::CUBE_SIZE) - (pos.Z + height / 2.0f);
				float prodScal = abs(dir.Z);
				if (abs(depassement) > seuil)
					if (abs(depassement) < abs(valueColMin))
					{
						prodScalMin = prodScal;
						valueColMin = depassement;
						axis = AXIS_Z;
					}
			}
		}

		//On verif tout les 4 angles du bas
		if (getCube(xPrev, yPrev, zPrev)->isSolid() ||
			getCube(xPrev, yNext, zPrev)->isSolid() ||
			getCube(xNext, yPrev, zPrev)->isSolid() ||
			getCube(xNext, yNext, zPrev)->isSolid())
		{
			//On verif que resoudre cette collision est utile
			if (!(getCube(xPrev, yPrev, zPrev + 1)->isSolid() ||
				getCube(xPrev, yNext, zPrev + 1)->isSolid() ||
				getCube(xNext, yPrev, zPrev + 1)->isSolid() ||
				getCube(xNext, yNext, zPrev + 1)->isSolid()) || !oneShot)
			{
				float depassement = ((zPrev + 1) * MCube::CUBE_SIZE) - (pos.Z - height / 2.0f);
				float prodScal = abs(dir.Z);
				if (abs(depassement) > seuil)
					if (abs(depassement) < abs(valueColMin))
					{
						prodScalMin = prodScal;
						valueColMin = depassement;
						axis = AXIS_Z;
					}
			}
		}

		return axis;
	}
		
	void render_world_basic(GLuint shader, YVbo* vboCube)
	{
		for (int x = MAT_SIZE_CUBES - 1; x >= 0; x--)
			for (int y = MAT_SIZE_CUBES - 1; y >= 0; y--)
				for (int z = MAT_HEIGHT_CUBES - 1; z >= 0; z--)
				{
					MCube* cube = getCube(x, y, z);
					MCube::MCubeType type = cube->getType();

					//Si il faut le dessiner
					if (type != MCube::CUBE_AIR && cube->getDraw())
					{
						YColor color(40.0f / 255.0f, 25.0f / 255.0f, 0.0f, 1.0f);
						if (type == MCube::CUBE_EAU)
							color = YColor(0, 0, 1, 0.5);
						if (type == MCube::CUBE_HERBE)
							color = YColor(0, 1, 0, 1);

						glPushMatrix();
						glTranslatef(x * MCube::CUBE_SIZE, y * MCube::CUBE_SIZE, z * MCube::CUBE_SIZE);
						YRenderer::getInstance()->updateMatricesFromOgl();
						YRenderer::getInstance()->sendMatricesToShader(shader);
						GLuint var = glGetUniformLocation(shader, "cube_color");
						glUniform4f(var, color.R, color.V, color.B, color.A);
						vboCube->render();
						glPopMatrix();
					}
				}
	}

	void render_world_vbo(bool debug, bool doTransparent)
	{
		glDisable(GL_BLEND);
		for (int x = 0; x < MAT_SIZE; x++)
			for (int y = 0; y < MAT_SIZE; y++)
				for (int z = MAT_HEIGHT - 1; z >= 0; z--)
				{
					glPushMatrix();
					glTranslatef((float)(x * MChunk::CHUNK_SIZE * MCube::CUBE_SIZE), (float)(y * MChunk::CHUNK_SIZE * MCube::CUBE_SIZE), (float)(z * MChunk::CHUNK_SIZE * MCube::CUBE_SIZE));
					YRenderer::getInstance()->updateMatricesFromOgl();
					YRenderer::getInstance()->sendMatricesToShader(YRenderer::CURRENT_SHADER);
					Chunks[x][y][z]->render(false);
					glPopMatrix();
				}


		glEnable(GL_BLEND);
		if (doTransparent)
		{
			for (int x = 0; x < MAT_SIZE; x++)
				for (int y = 0; y < MAT_SIZE; y++)
					for (int z = 0; z < MAT_HEIGHT; z++)
					{
						glPushMatrix();
						glTranslatef((float)(x * MChunk::CHUNK_SIZE * MCube::CUBE_SIZE), (float)(y * MChunk::CHUNK_SIZE * MCube::CUBE_SIZE), (float)(z * MChunk::CHUNK_SIZE * MCube::CUBE_SIZE));
						YRenderer::getInstance()->updateMatricesFromOgl();
						YRenderer::getInstance()->sendMatricesToShader(YRenderer::CURRENT_SHADER);
						Chunks[x][y][z]->render(true);
						glPopMatrix();
					}
		}
		glDepthMask(true);
	}

	/**
	* Attention ce code n'est pas optimal, il est compréhensible. Il existe de nombreuses
	* versions optimisées de ce calcul.
	*/
	inline bool intersecDroitePlan(const YVec3f& debSegment, const  YVec3f& finSegment, const YVec3f& p1Plan, const YVec3f& p2Plan, const YVec3f& p3Plan, YVec3f& inter)
	{
		//Equation du plan :
		YVec3f nrmlAuPlan = (p1Plan - p2Plan).cross(p3Plan - p2Plan); //On a les a,b,c du ax+by+cz+d = 0
		float d = -(nrmlAuPlan.X * p2Plan.X + nrmlAuPlan.Y * p2Plan.Y + nrmlAuPlan.Z * p2Plan.Z); //On remarque que c'est un produit scalaire...

		//Equation de droite :
		YVec3f dirDroite = finSegment - debSegment;

		//On resout l'équation de plan
		float nominateur = -d - nrmlAuPlan.X * debSegment.X - nrmlAuPlan.Y * debSegment.Y - nrmlAuPlan.Z * debSegment.Z;
		float denominateur = nrmlAuPlan.X * dirDroite.X + nrmlAuPlan.Y * dirDroite.Y + nrmlAuPlan.Z * dirDroite.Z;

		if (denominateur == 0)
			return false;

		//Calcul de l'intersection
		float t = nominateur / denominateur;
		inter = debSegment + (dirDroite * t);

		//Si point avant le debut du segment
		if (t < 0 || t > 1)
			return false;

		return true;
	}

	/**
	* Attention ce code n'est pas optimal, il est compréhensible. Il existe de nombreuses
	* versions optimisées de ce calcul. Il faut donner les points dans l'ordre (CW ou CCW)
	*/
	inline bool intersecDroiteCubeFace(const YVec3f& debSegment, const YVec3f& finSegment, const YVec3f& p1, const YVec3f& p2, const YVec3f& p3, const  YVec3f& p4, YVec3f& inter)
	{
		//On calcule l'intersection
		bool res = intersecDroitePlan(debSegment, finSegment, p1, p2, p4, inter);

		if (!res)
			return false;

		//On fait les produits vectoriels
		YVec3f v1 = p2 - p1;
		YVec3f v2 = p3 - p2;
		YVec3f v3 = p4 - p3;
		YVec3f v4 = p1 - p4;

		YVec3f n1 = v1.cross(inter - p1);
		YVec3f n2 = v2.cross(inter - p2);
		YVec3f n3 = v3.cross(inter - p3);
		YVec3f n4 = v4.cross(inter - p4);

		//on compare le signe des produits scalaires
		float ps1 = n1.dot(n2);
		float ps2 = n2.dot(n3);
		float ps3 = n3.dot(n4);

		if (ps1 >= 0 && ps2 >= 0 && ps3 >= 0)
			return true;

		return false;
	}
	

	bool getRayCollision(const YVec3f& debSegment, const YVec3f& finSegment, YVec3f& inter, int& xCube, int& yCube, int& zCube)
	{
		float len = (finSegment - debSegment).getSize();

		int x = (int)(debSegment.X / MCube::CUBE_SIZE);
		int y = (int)(debSegment.Y / MCube::CUBE_SIZE);
		int z = (int)(debSegment.Z / MCube::CUBE_SIZE);

		int l = (int)(len / MCube::CUBE_SIZE) + 1;

		int xDeb = x - l;
		int yDeb = y - l;
		int zDeb = z - l;

		int xFin = x + l;
		int yFin = y + l;
		int zFin = z + l;

		if (xDeb < 0)
			xDeb = 0;
		if (yDeb < 0)
			yDeb = 0;
		if (zDeb < 0)
			zDeb = 0;

		if (xFin >= MAT_SIZE_CUBES)
			xFin = MAT_SIZE_CUBES - 1;
		if (yFin >= MAT_SIZE_CUBES)
			yFin = MAT_SIZE_CUBES - 1;
		if (zFin >= MAT_HEIGHT_CUBES)
			zFin = MAT_HEIGHT_CUBES - 1;

		float minDist = -1;
		YVec3f interTmp;
		for (x = xDeb; x <= xFin; x++)
			for (y = yDeb; y <= yFin; y++)
				for (z = zDeb; z <= zFin; z++)
				{
					if (getCube(x, y, z)->isSolid())
					{
						if (getRayCollisionWithCube(debSegment, finSegment, x, y, z, interTmp))
						{
							if ((debSegment - interTmp).getSqrSize() < minDist || minDist == -1)
							{
								minDist = (debSegment - interTmp).getSqrSize();
								inter = interTmp;
								xCube = x;
								yCube = y;
								zCube = z;

							}
						}
					}
				}

		if (minDist != -1)
			return true;

		return false;

	}

	/**
	* De meme cette fonction peut être grandement opitimisée, on a priviligié la clarté
	*/
	bool getRayCollisionWithCube(const YVec3f& debSegment, const YVec3f& finSegment, int x, int y, int z, YVec3f& inter)
	{
		float minDist = -1;
		YVec3f interTemp;

		//Face1
		if (intersecDroiteCubeFace(debSegment, finSegment,
			YVec3f((x + 0) * MCube::CUBE_SIZE, (y + 0) * MCube::CUBE_SIZE, (z + 0) * MCube::CUBE_SIZE),
			YVec3f((x + 1) * MCube::CUBE_SIZE, (y + 0) * MCube::CUBE_SIZE, (z + 0) * MCube::CUBE_SIZE),
			YVec3f((x + 1) * MCube::CUBE_SIZE, (y + 0) * MCube::CUBE_SIZE, (z + 1) * MCube::CUBE_SIZE),
			YVec3f((x + 0) * MCube::CUBE_SIZE, (y + 0) * MCube::CUBE_SIZE, (z + 1) * MCube::CUBE_SIZE),
			interTemp))
		{
			if ((interTemp - debSegment).getSqrSize() < minDist || minDist == -1)
			{
				minDist = (interTemp - debSegment).getSqrSize();
				inter = interTemp;
			}
		}

		//Face2
		if (intersecDroiteCubeFace(debSegment, finSegment,
			YVec3f((x + 0) * MCube::CUBE_SIZE, (y + 1) * MCube::CUBE_SIZE, (z + 0) * MCube::CUBE_SIZE),
			YVec3f((x + 1) * MCube::CUBE_SIZE, (y + 1) * MCube::CUBE_SIZE, (z + 0) * MCube::CUBE_SIZE),
			YVec3f((x + 1) * MCube::CUBE_SIZE, (y + 1) * MCube::CUBE_SIZE, (z + 1) * MCube::CUBE_SIZE),
			YVec3f((x + 0) * MCube::CUBE_SIZE, (y + 1) * MCube::CUBE_SIZE, (z + 1) * MCube::CUBE_SIZE),
			interTemp))
		{
			if ((interTemp - debSegment).getSqrSize() < minDist || minDist == -1)
			{
				minDist = (interTemp - debSegment).getSqrSize();
				inter = interTemp;
			}
		}

		//Face3
		if (intersecDroiteCubeFace(debSegment, finSegment,
			YVec3f((x + 0) * MCube::CUBE_SIZE, (y + 0) * MCube::CUBE_SIZE, (z + 0) * MCube::CUBE_SIZE),
			YVec3f((x + 1) * MCube::CUBE_SIZE, (y + 0) * MCube::CUBE_SIZE, (z + 0) * MCube::CUBE_SIZE),
			YVec3f((x + 1) * MCube::CUBE_SIZE, (y + 1) * MCube::CUBE_SIZE, (z + 0) * MCube::CUBE_SIZE),
			YVec3f((x + 0) * MCube::CUBE_SIZE, (y + 1) * MCube::CUBE_SIZE, (z + 0) * MCube::CUBE_SIZE),
			interTemp))
		{
			if ((interTemp - debSegment).getSqrSize() < minDist || minDist == -1)
			{
				minDist = (interTemp - debSegment).getSqrSize();
				inter = interTemp;
			}
		}

		//Face4
		if (intersecDroiteCubeFace(debSegment, finSegment,
			YVec3f((x + 0) * MCube::CUBE_SIZE, (y + 0) * MCube::CUBE_SIZE, (z + 1) * MCube::CUBE_SIZE),
			YVec3f((x + 1) * MCube::CUBE_SIZE, (y + 0) * MCube::CUBE_SIZE, (z + 1) * MCube::CUBE_SIZE),
			YVec3f((x + 1) * MCube::CUBE_SIZE, (y + 1) * MCube::CUBE_SIZE, (z + 1) * MCube::CUBE_SIZE),
			YVec3f((x + 0) * MCube::CUBE_SIZE, (y + 1) * MCube::CUBE_SIZE, (z + 1) * MCube::CUBE_SIZE),
			interTemp))
		{
			if ((interTemp - debSegment).getSqrSize() < minDist || minDist == -1)
			{
				minDist = (interTemp - debSegment).getSqrSize();
				inter = interTemp;
			}
		}

		//Face5
		if (intersecDroiteCubeFace(debSegment, finSegment,
			YVec3f((x + 0) * MCube::CUBE_SIZE, (y + 0) * MCube::CUBE_SIZE, (z + 0) * MCube::CUBE_SIZE),
			YVec3f((x + 0) * MCube::CUBE_SIZE, (y + 1) * MCube::CUBE_SIZE, (z + 0) * MCube::CUBE_SIZE),
			YVec3f((x + 0) * MCube::CUBE_SIZE, (y + 1) * MCube::CUBE_SIZE, (z + 1) * MCube::CUBE_SIZE),
			YVec3f((x + 0) * MCube::CUBE_SIZE, (y + 0) * MCube::CUBE_SIZE, (z + 1) * MCube::CUBE_SIZE),
			interTemp))
		{
			if ((interTemp - debSegment).getSqrSize() < minDist || minDist == -1)
			{
				minDist = (interTemp - debSegment).getSqrSize();
				inter = interTemp;
			}
		}

		//Face6
		if (intersecDroiteCubeFace(debSegment, finSegment,
			YVec3f((x + 1) * MCube::CUBE_SIZE, (y + 0) * MCube::CUBE_SIZE, (z + 0) * MCube::CUBE_SIZE),
			YVec3f((x + 1) * MCube::CUBE_SIZE, (y + 1) * MCube::CUBE_SIZE, (z + 0) * MCube::CUBE_SIZE),
			YVec3f((x + 1) * MCube::CUBE_SIZE, (y + 1) * MCube::CUBE_SIZE, (z + 1) * MCube::CUBE_SIZE),
			YVec3f((x + 1) * MCube::CUBE_SIZE, (y + 0) * MCube::CUBE_SIZE, (z + 1) * MCube::CUBE_SIZE),
			interTemp))
		{
			if ((interTemp - debSegment).getSqrSize() < minDist || minDist == -1)
			{
				minDist = (interTemp - debSegment).getSqrSize();
				inter = interTemp;
			}
		}


		if (minDist < 0)
			return false;

		return true;
	}

	bool getRayCollisionGrounded(const YVec3f& debSegment, const YVec3f& finSegment, YVec3f& inter, int& xCube, int& yCube, int& zCube)
	{
		float len = (finSegment - debSegment).getSize();

		int x = (int)(debSegment.X / MCube::CUBE_SIZE);
		int y = (int)(debSegment.Y / MCube::CUBE_SIZE);
		int z = (int)(debSegment.Z / MCube::CUBE_SIZE);

		int l = (int)(len / MCube::CUBE_SIZE) + 1;

		int xDeb = x - l;
		int yDeb = y - l;
		int zDeb = z - l;

		int xFin = x + l;
		int yFin = y + l;
		int zFin = z + l;

		if (xDeb < 0)
			xDeb = 0;
		if (yDeb < 0)
			yDeb = 0;
		if (zDeb < 0)
			zDeb = 0;

		if (xFin >= MAT_SIZE_CUBES)
			xFin = MAT_SIZE_CUBES - 1;
		if (yFin >= MAT_SIZE_CUBES)
			yFin = MAT_SIZE_CUBES - 1;
		if (zFin >= MAT_HEIGHT_CUBES)
			zFin = MAT_HEIGHT_CUBES - 1;

		float minDist = -1;
		YVec3f interTmp;
		for (x = xDeb; x <= xFin; x++)
			for (y = yDeb; y <= yFin; y++)
				for (z = zDeb; z <= zFin; z++)
				{
					if (getCube(x, y, z)->getType() != MCube::CUBE_AIR)
					{
						if (getRayCollisionWithCube(debSegment, finSegment, x, y, z, interTmp))
						{
							if ((debSegment - interTmp).getSqrSize() < minDist || minDist == -1)
							{
								minDist = (debSegment - interTmp).getSqrSize();
								inter = interTmp;
								xCube = x;
								yCube = y;
								zCube = z;

							}
						}
					}
				}

		if (minDist != -1)
			return true;

		return false;

	}
};





#endif