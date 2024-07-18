#pragma once

#include "engine/render/renderer.h"
#include "engine/render/vbo.h"
#include "cube.h"

/**
  * On utilise des chunks pour que si on modifie juste un cube, on ait pas
  * besoin de recharger toute la carte dans le buffer, mais juste le chunk en question
  */
class MChunk
{
	public :

		static const int CHUNK_SIZE = 128; ///< Taille d'un chunk en nombre de cubes (n*n*n)
		MCube _Cubes[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE]; ///< Cubes contenus dans le chunk

		YVbo * VboOpaque = NULL;
		YVbo * VboTransparent = NULL;

		int _NbVertices_Opaque;
		int _NbVertices_Transparent;

		MChunk * Voisins[6];

		int _XPos, _YPos, _ZPos; ///< Position du chunk dans le monde

		MChunk(int x, int y, int z)
		{
			memset(Voisins, 0x00, sizeof(void*)* 6);
			_XPos = x;
			_YPos = y;
			_ZPos = z;
		}

		/*
		Creation des VBO
		*/

		//On met le chunk ddans son VBO
		void toVbos(void)
		{
			SAFEDELETE(VboOpaque);
			SAFEDELETE(VboTransparent);

			//On compte juste
			foreachVisibleTriangle(true, &_NbVertices_Opaque, &_NbVertices_Transparent, VboOpaque, VboTransparent);

			//Creation des vbo
			VboOpaque = new YVbo(4, _NbVertices_Opaque, YVbo::PACK_BY_ELEMENT_TYPE);
			VboTransparent = new YVbo(4, _NbVertices_Transparent, YVbo::PACK_BY_ELEMENT_TYPE);

			VboOpaque->setElementDescription(0, YVbo::Element(3)); //Sommet
			VboOpaque->setElementDescription(1, YVbo::Element(3)); //Normale
			VboOpaque->setElementDescription(2, YVbo::Element(2)); //Uv
			VboOpaque->setElementDescription(3, YVbo::Element(1)); //Type

			VboTransparent->setElementDescription(0, YVbo::Element(3)); //Sommet
			VboTransparent->setElementDescription(1, YVbo::Element(3)); //Normale
			VboTransparent->setElementDescription(2, YVbo::Element(2)); //Uv
			VboTransparent->setElementDescription(3, YVbo::Element(1)); //Type

			VboOpaque->createVboCpu();
			VboTransparent->createVboCpu();

			//On remplit les VBOs
			foreachVisibleTriangle(false, &_NbVertices_Opaque, &_NbVertices_Transparent, VboOpaque, VboTransparent);

			VboOpaque->createVboGpu();
			VboTransparent->createVboGpu();

			VboOpaque->deleteVboCpu();
			VboTransparent->deleteVboCpu();

		}

		//Ajoute un quad du cube. Attention CCW
		int addQuadToVbo(YVbo* vbo, int iVertice, YVec3f& a, YVec3f& b, YVec3f& c, YVec3f& d, float type)
		{
			YVec3f normal = (b - a).cross(d - a);
			normal.normalize();

			vbo->setElementValue(0, iVertice, a.X, a.Y, a.Z);
			vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
			vbo->setElementValue(2, iVertice, 0, 0);
			vbo->setElementValue(3, iVertice, type);

			iVertice++;

			vbo->setElementValue(0, iVertice, b.X, b.Y, b.Z);
			vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
			vbo->setElementValue(2, iVertice, 1, 0);
			vbo->setElementValue(3, iVertice, type);

			iVertice++;

			vbo->setElementValue(0, iVertice, c.X, c.Y, c.Z);
			vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
			vbo->setElementValue(2, iVertice, 1, 1);
			vbo->setElementValue(3, iVertice, type);

			iVertice++;

			vbo->setElementValue(0, iVertice, a.X, a.Y, a.Z);
			vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
			vbo->setElementValue(2, iVertice, 0, 0);
			vbo->setElementValue(3, iVertice, type);

			iVertice++;

			vbo->setElementValue(0, iVertice, c.X, c.Y, c.Z);
			vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
			vbo->setElementValue(2, iVertice, 1, 1);
			vbo->setElementValue(3, iVertice, type);

			iVertice++;

			vbo->setElementValue(0, iVertice, d.X, d.Y, d.Z);
			vbo->setElementValue(1, iVertice, normal.X, normal.Y, normal.Z);
			vbo->setElementValue(2, iVertice, 0, 1);
			vbo->setElementValue(3, iVertice, type);

			iVertice++;

			return 6;
		}

		//Permet de compter les triangles ou des les ajouter aux VBO
		void foreachVisibleTriangle(bool countOnly, int* nbVertOpaque, int* nbVertTransp, YVbo* VboOpaque, YVbo* VboTrasparent)
		{
			int type = 0;
			MCube* cube;
			int nbVertices = 0;
			int iVerticeOpaque = 0;
			int iVerticeTransp = 0;
			int* iVertice = &iVerticeOpaque;
			//On parcourt tous nos cubes
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				for (int y = 0; y < CHUNK_SIZE; y++)
				{
					for (int z = 0; z < CHUNK_SIZE; z++)
					{
						cube = &(_Cubes[x][y][z]);
						type = cube->getType();

						if (cube->getDraw() && type != MCube::CUBE_AIR)
						{
							//Position du cube (coin bas gauche face avant)
							float xPos = x * (float)MCube::CUBE_SIZE;
							float yPos = y * (float)MCube::CUBE_SIZE;
							float zPos = z * (float)MCube::CUBE_SIZE;

							YVec3f a(xPos + MCube::CUBE_SIZE, yPos, zPos);
							YVec3f b(xPos + MCube::CUBE_SIZE, yPos + MCube::CUBE_SIZE, zPos);
							YVec3f c(xPos + MCube::CUBE_SIZE, yPos + MCube::CUBE_SIZE, zPos + MCube::CUBE_SIZE);
							YVec3f d(xPos + MCube::CUBE_SIZE, yPos, zPos + MCube::CUBE_SIZE);
							YVec3f e(xPos, yPos, zPos);
							YVec3f f(xPos, yPos + MCube::CUBE_SIZE, zPos);
							YVec3f g(xPos, yPos + MCube::CUBE_SIZE, zPos + MCube::CUBE_SIZE);
							YVec3f h(xPos, yPos, zPos + MCube::CUBE_SIZE);

							MCube* cubeXPrev = NULL;
							MCube* cubeXNext = NULL;
							MCube* cubeYPrev = NULL;
							MCube* cubeYNext = NULL;
							MCube* cubeZPrev = NULL;
							MCube* cubeZNext = NULL;

							get_surrounding_cubes(
								x, y, z,
								&cubeXPrev, &cubeXNext,
								&cubeYPrev, &cubeYNext,
								&cubeZPrev, &cubeZNext);

							iVertice = &iVerticeTransp;
							if (countOnly)
								iVertice = nbVertTransp;
							YVbo* vbo = VboTransparent;
							if (cube->isOpaque()) {
								iVertice = &iVerticeOpaque;
								if (countOnly)
									iVertice = nbVertOpaque;
								vbo = VboOpaque;
							}

							//Premier QUAD (x+)
							if (cubeXNext == NULL ||
								(cube->isOpaque() && !cubeXNext->isOpaque()) || //Je suis un cube opaque et le cube a cote de moi est transparent
								(!cube->isOpaque() && cubeXNext->getType() != type)) //Je suis un cube transparent et le cube a cote de moi est de l'air (on rend le transparent qu'au contact de l'air)
							{
								if (!countOnly && cube->getType() != MCube::CUBE_EAU)
									addQuadToVbo(vbo, *iVertice, a, b, c, d, type); //x+
								*iVertice += 6;
							}

							//Second QUAD (x-)
							if (cubeXPrev == NULL ||
								(cube->isOpaque() && !cubeXPrev->isOpaque()) || //Je suis un cube opaque et le cube a cote de moi est transparent
								(!cube->isOpaque() && cubeXPrev->getType() != type)) //Je suis un cube transparent et le cube a cote de moi est de l'air (on rend le transparent qu'au contact de l'air)
							{
								if (!countOnly && cube->getType() != MCube::CUBE_EAU)
									addQuadToVbo(vbo, *iVertice, f, e, h, g, type); //x-
								*iVertice += 6;
							}


							//Troisieme QUAD (y+)
							if (cubeYNext == NULL ||
								(cube->isOpaque() && !cubeYNext->isOpaque()) || //Je suis un cube opaque et le cube a cote de moi est transparent
								(!cube->isOpaque() && cubeYNext->getType() != type)) //Je suis un cube transparent et le cube a cote de moi est de l'air (on rend le transparent qu'au contact de l'air)
							{
								if (!countOnly && cube->getType() != MCube::CUBE_EAU)
									addQuadToVbo(vbo, *iVertice, b, f, g, c, type); //y+
								*iVertice += 6;
							}

							//Quatrieme QUAD (y-)
							if (cubeYPrev == NULL ||
								(cube->isOpaque() && !cubeYPrev->isOpaque()) || //Je suis un cube opaque et le cube a cote de moi est transparent
								(!cube->isOpaque() && cubeYPrev->getType() != type)) //Je suis un cube transparent et le cube a cote de moi est de l'air (on rend le transparent qu'au contact de l'air)
							{
								if (!countOnly && cube->getType() != MCube::CUBE_EAU)
									addQuadToVbo(vbo, *iVertice, e, a, d, h, type); //y-
								*iVertice += 6;
							}

							//Cinquieme QUAD (z+)
							if (cubeZNext == NULL ||
								(cube->isOpaque() && !cubeZNext->isOpaque()) || //Je suis un cube opaque et le cube a cote de moi est transparent
								(!cube->isOpaque() && cubeZNext->getType() != type)) //Je suis un cube transparent et le cube a cote de moi est de l'air (on rend le transparent qu'au contact de l'air)
							{
								if (!countOnly)
									addQuadToVbo(vbo, *iVertice, c, g, h, d, type); //z+
								*iVertice += 6;
							}

							//Sixième QUAD (le z-)
							if (cubeZPrev == NULL ||
								(cube->isOpaque() && !cubeZPrev->isOpaque()) || //Je suis un cube opaque et le cube a cote de moi est transparent
								(!cube->isOpaque() && cubeZPrev->getType() != type)) //Je suis un cube transparent et le cube a cote de moi est de l'air (on rend le transparent qu'au contact d'un cube !=)
							{
								if (!countOnly && cube->getType() != MCube::CUBE_EAU)
									addQuadToVbo(vbo, *iVertice, e, f, b, a, type); //z-
								*iVertice += 6;
							}
						}
					}
				}
			}
		}

		/*
		Gestion du chunk
		*/

		void reset(void)
		{
			for(int x=0;x<CHUNK_SIZE;x++)
				for(int y=0;y<CHUNK_SIZE;y++)
					for(int z=0;z<CHUNK_SIZE;z++)
					{
						_Cubes[x][y][z].setDraw(false);
						_Cubes[x][y][z].setType(MCube::CUBE_AIR);
					}
		}

		void setVoisins(MChunk * xprev, MChunk * xnext, MChunk * yprev, MChunk * ynext, MChunk * zprev, MChunk * znext)
		{
			Voisins[0] = xprev;
			Voisins[1] = xnext;
			Voisins[2] = yprev;
			Voisins[3] = ynext;
			Voisins[4] = zprev;
			Voisins[5] = znext;
		}

		void get_surrounding_cubes(int x, int y, int z, MCube ** cubeXPrev, MCube ** cubeXNext,
			MCube ** cubeYPrev, MCube ** cubeYNext,
			MCube ** cubeZPrev, MCube ** cubeZNext)
		{

			*cubeXPrev = NULL;
			*cubeXNext = NULL;
			*cubeYPrev = NULL;
			*cubeYNext = NULL;
			*cubeZPrev = NULL;
			*cubeZNext = NULL;

			if (x == 0 && Voisins[0] != NULL)
				*cubeXPrev = &(Voisins[0]->_Cubes[CHUNK_SIZE - 1][y][z]);
			else if (x > 0)
				*cubeXPrev = &(_Cubes[x - 1][y][z]);

			if (x == CHUNK_SIZE - 1 && Voisins[1] != NULL)
				*cubeXNext = &(Voisins[1]->_Cubes[0][y][z]);
			else if (x < CHUNK_SIZE - 1)
				*cubeXNext = &(_Cubes[x + 1][y][z]);

			if (y == 0 && Voisins[2] != NULL)
				*cubeYPrev = &(Voisins[2]->_Cubes[x][CHUNK_SIZE - 1][z]);
			else if (y > 0)
				*cubeYPrev = &(_Cubes[x][y - 1][z]);

			if (y == CHUNK_SIZE - 1 && Voisins[3] != NULL)
				*cubeYNext = &(Voisins[3]->_Cubes[x][0][z]);
			else if (y < CHUNK_SIZE - 1)
				*cubeYNext = &(_Cubes[x][y + 1][z]);

			if (z == 0 && Voisins[4] != NULL)
				*cubeZPrev = &(Voisins[4]->_Cubes[x][y][CHUNK_SIZE - 1]);
			else if (z > 0)
				*cubeZPrev = &(_Cubes[x][y][z - 1]);

			if (z == CHUNK_SIZE - 1 && Voisins[5] != NULL)
				*cubeZNext = &(Voisins[5]->_Cubes[x][y][0]);
			else if (z < CHUNK_SIZE - 1)
				*cubeZNext = &(_Cubes[x][y][z + 1]);
		}

		void render(bool transparent)
		{
			if (transparent)
				VboTransparent->render();
			else
				VboOpaque->render();
		}

		/**
		  * On verifie si le cube peut être vu
		  */
		bool test_hidden(int x, int y, int z)
		{
			MCube * cubeXPrev = NULL; 
			MCube * cubeXNext = NULL; 
			MCube * cubeYPrev = NULL; 
			MCube * cubeYNext = NULL; 
			MCube * cubeZPrev = NULL; 
			MCube * cubeZNext = NULL; 

			if(x == 0 && Voisins[0] != NULL)
				cubeXPrev = &(Voisins[0]->_Cubes[CHUNK_SIZE-1][y][z]);
			else if(x > 0)
				cubeXPrev = &(_Cubes[x-1][y][z]);

			if(x == CHUNK_SIZE-1 && Voisins[1] != NULL)
				cubeXNext = &(Voisins[1]->_Cubes[0][y][z]);
			else if(x < CHUNK_SIZE-1)
				cubeXNext = &(_Cubes[x+1][y][z]);

			if(y == 0 && Voisins[2] != NULL)
				cubeYPrev = &(Voisins[2]->_Cubes[x][CHUNK_SIZE-1][z]);
			else if(y > 0)
				cubeYPrev = &(_Cubes[x][y-1][z]);

			if(y == CHUNK_SIZE-1 && Voisins[3] != NULL)
				cubeYNext = &(Voisins[3]->_Cubes[x][0][z]);
			else if(y < CHUNK_SIZE-1)
				cubeYNext = &(_Cubes[x][y+1][z]);

			if(z == 0 && Voisins[4] != NULL)
				cubeZPrev = &(Voisins[4]->_Cubes[x][y][CHUNK_SIZE-1]);
			else if(z > 0)
				cubeZPrev = &(_Cubes[x][y][z-1]);

			if(z == CHUNK_SIZE-1 && Voisins[5] != NULL)
				cubeZNext = &(Voisins[5]->_Cubes[x][y][0]);
			else if(z < CHUNK_SIZE-1)
				cubeZNext = &(_Cubes[x][y][z+1]);

			if( cubeXPrev == NULL || cubeXNext == NULL ||
				cubeYPrev == NULL || cubeYNext == NULL ||
				cubeZPrev == NULL || cubeZNext == NULL )
				return false;

			if (cubeXPrev->isOpaque() == true && //droite
				cubeXNext->isOpaque() == true && //gauche
				cubeYPrev->isOpaque() == true && //haut
				cubeYNext->isOpaque() == true && //bas
				cubeZPrev->isOpaque() == true && //devant
				cubeZNext->isOpaque() == true)  //derriere
				return true;
			return false;
		}

		void disableHiddenCubes(void)
		{
			for(int x=0;x<CHUNK_SIZE;x++)
				for(int y=0;y<CHUNK_SIZE;y++)
					for(int z=0;z<CHUNK_SIZE;z++)
					{
						_Cubes[x][y][z].setDraw(true);
						if(test_hidden(x,y,z))
							_Cubes[x][y][z].setDraw(false);
					}
		}


};