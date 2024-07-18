#ifndef __YOCTO__ENGINE_TEST__
#define __YOCTO__ENGINE_TEST__

#include "engine/engine.h"

#include "avatar.h"
#include "world.h"

class MEngineMinicraft : public YEngine {

public :

	YVbo* VboCube;
	YFbo* Fbo;

	MWorld* World;
	MAvatar* Avatar;

	GLuint ShaderCubeDebug; 
	GLuint ShaderSun;
	GLuint ShaderCube;
	GLuint ShaderWorld;
	GLuint PostProcessProg;

	YTexFile* MainTexture = NULL;
	YTexManager* TexManager = NULL;

	YColor SunColor = YColor(1.0f, 1.0f, 102.0f / 255.0f, 1.0f);
	YColor SkyColor = YColor(0.0f, 162.0f / 255.0f, 1.0f, 1.0f);

	YVec3<float> SunPosition;
	YVec3<float> SunDirection;

	//SYSTEMTIME LocalTime;
	float time;
	bool SunTimeOffset = false;

	//Gestion singleton
	static YEngine * getInstance()
	{
		if (Instance == NULL)
			Instance = new MEngineMinicraft();
		return Instance;
	}

	/*HANDLERS GENERAUX*/
	void loadShaders() {
		//Chargement du shader (dans loadShaders() pour etre lié à F5)
		ShaderSun = Renderer->createProgram("shaders/sun");
		ShaderCube = Renderer->createProgram("shaders/cube");
		ShaderWorld = Renderer->createProgram("shaders/world");
		PostProcessProg = Renderer->createProgram("shaders/postprocess");
	}

	void init() 
	{
		YLog::log(YLog::ENGINE_INFO,"Minicraft Started : initialisation");

		SunPosition = { 0, 0, 0 };
		SunDirection = { 0, 0, 0 };

		Renderer->setBackgroundColor(SkyColor);
		Renderer->Camera->setPosition(YVec3f(10, 10, 10));
		Renderer->Camera->setLookAt(YVec3f());

		Fbo = new YFbo(1);
		Fbo->init(Renderer->ScreenWidth, Renderer->ScreenHeight);

		//Pour créer le monde
		World = new MWorld();
		World->init_world(0);

		//Création de l'avatar
		Avatar = new MAvatar(Renderer->Camera, World);

		//Récupération de la texture
		MainTexture = TexManager->getInstance()->loadTexture("uv/atlas.png");
		TexManager->getInstance()->loadTextureToOgl(*MainTexture);

		//Creation du VBO
		VboCube = new YVbo(3, 36, YVbo::PACK_BY_ELEMENT_TYPE);

		//Définition du contenu du VBO
		VboCube->setElementDescription(0, YVbo::Element(3)); //Sommet
		VboCube->setElementDescription(1, YVbo::Element(3)); //Normale
		VboCube->setElementDescription(2, YVbo::Element(2)); //UV
		//On demande d'allouer la mémoire coté CPU
		VboCube->createVboCpu();

		//On ajoute les sommets
		int iVertice = 0;
		fillVBOCube(VboCube, iVertice);

		//On envoie le contenu au GPU
		VboCube->createVboGpu();

		//On relache la mémoire CPU
		VboCube->deleteVboCpu();

	}

	void addTriangle(YVbo* vbo, int& indexVertice, YVec3f trianglePoint1, YVec3f trianglePoint2, YVec3f trianglePoint3)
	{
		//Triangle 1
		vbo->setElementValue(0, indexVertice, trianglePoint1.X, trianglePoint1.Y, trianglePoint1.Z); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, indexVertice, 1, 0, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, indexVertice, 0, 0);      //UV (lié au layout(2) du shader)
		indexVertice++;
		//Triangle 2
		vbo->setElementValue(0, indexVertice, trianglePoint2.X, trianglePoint2.Y, trianglePoint2.Z); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, indexVertice, 1, 0, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, indexVertice, 0, 0);      //UV (lié au layout(2) du shader)
		indexVertice++;
		//Triangle 3
		vbo->setElementValue(0, indexVertice, trianglePoint3.X, trianglePoint3.Y, trianglePoint3.Z); //Sommet (lié au layout(0) du shader)
		vbo->setElementValue(1, indexVertice, 1, 0, 0);   //Normale (lié au layout(1) du shader)
		vbo->setElementValue(2, indexVertice, 0, 0);      //UV (lié au layout(2) du shader)
		indexVertice++;
	}

	void addQuad(YVbo* vbo, int& indexVertice, YVec3f point1, YVec3f point2, YVec3f point3, YVec3f point4)
	{
		addTriangle(vbo, indexVertice, point1, point2, point3);
		addTriangle(vbo, indexVertice, point1, point3, point4);

		// 4 ------ 3
		// |	    |
		// |	    |
		// |		|
		// 1 ------ 2  vers l'avant
		// 1->2->3 puis 1->3->4
	}

	void fillVBOCube(YVbo* vbo, int& iVertice)
	{
		addQuad(VboCube, iVertice, YVec3f(-1, -1, -1), YVec3f(1, -1, -1), YVec3f(1, 1, -1), YVec3f(-1, 1, -1));
		addQuad(VboCube, iVertice, YVec3f(1, -1, -1), YVec3f(1, -1, 1), YVec3f(1, 1, 1), YVec3f(1, 1, -1));
		addQuad(VboCube, iVertice, YVec3f(1, -1, 1), YVec3f(-1, -1, 1), YVec3f(-1, 1, 1), YVec3f(1, 1, 1));
		addQuad(VboCube, iVertice, YVec3f(-1, -1, 1), YVec3f(-1, -1, -1), YVec3f(-1, 1, -1), YVec3f(-1, 1, 1));
		addQuad(VboCube, iVertice, YVec3f(-1, 1, -1), YVec3f(1, 1, -1), YVec3f(1, 1, 1), YVec3f(-1, 1, 1));
		addQuad(VboCube, iVertice, YVec3f(-1, -1, 1), YVec3f(1, -1, 1), YVec3f(1, -1, -1), YVec3f(-1, -1, -1));
	}

	void update(float elapsed) 
	{
		updateLights(time);
		if (SunTimeOffset)
		{
			time += 0.5;
		}

		Avatar->update(elapsed);
		Avatar->Run = GetKeyState(VK_LSHIFT) & 0x80;
		Avatar->Crouch = GetKeyState(VK_LCONTROL) & 0x80;
		Renderer->Camera->moveTo(Avatar->Position + YVec3f(0, 0, Avatar->CurrentHeight / 2));
	}

	void renderObjects() 
	{
		Fbo->setAsOutFBO(true);

		glUseProgram(0);
		//Rendu des axes
		//glDisable(GL_LIGHTING);
		//glBegin(GL_LINES);
		//glColor3d(1, 0, 0);
		//glVertex3d(0, 0, 0);
		//glVertex3d(10000, 0, 0);
		//glColor3d(0, 1, 0);
		//glVertex3d(0, 0, 0);
		//glVertex3d(0, 10000, 0);
		//glColor3d(0, 0, 1);
		//glVertex3d(0, 0, 0);
		//glVertex3d(0, 0, 10000);
		//glEnd();		

		//-- Monde --
		/*glPushMatrix();
		glUseProgram(ShaderCube);
		World->render_world_basic(ShaderCube, VboCube);
		glPopMatrix();*/

		//-- Monde optimisé --
		glPushMatrix();
		glUseProgram(ShaderWorld);
		Renderer->sendTimeToShader(YEngine::getInstance()->DeltaTimeCumul, ShaderWorld);

		MainTexture->setAsShaderInput(ShaderWorld, GL_TEXTURE0, "myTexture");

		GLuint var3 = glGetUniformLocation(ShaderWorld, "sunPos");
		glUniform3f(var3, SunPosition.X, SunPosition.Y, SunPosition.Z);

		var3 = glGetUniformLocation(ShaderWorld, "sunDir");
		glUniform3f(var3, SunDirection.X, SunDirection.Y, SunDirection.Z);

		var3 = glGetUniformLocation(ShaderWorld, "sunColor");
		glUniform3f(var3, SunColor.R, SunColor.V, SunColor.B);

		var3 = glGetUniformLocation(ShaderWorld, "camPos");
		glUniform3f(var3, Renderer->Camera->Position.X, Renderer->Camera->Position.Y, Renderer->Camera->Position.Z);

		World->render_world_vbo(false, true);
		glPopMatrix();

		//-- Cube de terre --
		glPushMatrix();
		glUseProgram(ShaderCube);
		Renderer->updateMatricesFromOgl();
		Renderer->sendMatricesToShader(ShaderCube);
		GLuint var2 = glGetUniformLocation(ShaderCube, "cube_color");
		glUniform4f(var2, 40.0f / 255.0f, 25.0f / 255.0f, 0.0f, 1.0f);
		glPopMatrix();

		//-- Soleil --
		glPushMatrix();
		glUseProgram(ShaderSun); //Demande au GPU de charger ces shaders
		GLuint var = glGetUniformLocation(ShaderSun, "sun_color");
		glUniform3f(var, SunColor.R, SunColor.V, SunColor.B);
		glTranslatef(SunPosition.X, SunPosition.Y, SunPosition.Z);
		glScalef(10, 10, 10);
		Renderer->updateMatricesFromOgl(); //Calcule toute les matrices à partir des deux matrices OGL
		Renderer->sendMatricesToShader(ShaderSun); //Envoie les matrices au shader
		glPopMatrix();

		VboCube->render(); //Demande le rendu du VBO
		
		Fbo->setAsOutFBO(false);

		glUseProgram(PostProcessProg);

		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_TEST);

		Fbo->setColorAsShaderInput(0, GL_TEXTURE0, "TexColor");
		Fbo->setDepthAsShaderInput(GL_TEXTURE1, "TexDepth");

		var3 = glGetUniformLocation(PostProcessProg, "skyColor");
		glUniform3f(var3, SkyColor.R, SkyColor.V, SkyColor.B);

		GLuint var4 = glGetUniformLocation(PostProcessProg, "typeCubeCameraIn");
		glUniform1f(var4, World->getCube(Renderer->Camera->Position.X / MCube::CUBE_SIZE, Renderer->Camera->Position.Y /MCube::CUBE_SIZE, Renderer->Camera->Position.Z/ MCube::CUBE_SIZE )->getType());

		Renderer->sendScreenSizeToShader(PostProcessProg);
		Renderer->sendNearFarToShader(PostProcessProg);
		Renderer->drawFullScreenQuad();
	}

	bool getSunDirFromDayTime(YVec3f& sunDir, float mnLever, float mnCoucher, float boostTime)
	{
		bool nuit = false;

		SYSTEMTIME t;
		GetLocalTime(&t);

		//On borne le tweak time à une journée (cyclique)
		while (boostTime > 24 * 60)
			boostTime -= 24 * 60;

		//Temps écoulé depuis le début de la journée
		float fTime = (float)(t.wHour * 60 + t.wMinute);
		fTime += boostTime;
		while (fTime > 24 * 60)
			fTime -= 24 * 60;

		//Si c'est la nuit
		if (fTime < mnLever || fTime > mnCoucher)
		{
			nuit = true;
			if (fTime < mnLever)
				fTime += 24 * 60;
			fTime -= mnCoucher;
			fTime /= (mnLever + 24 * 60 - mnCoucher);
			fTime *= (float)M_PI;
		}
		else
		{
			//c'est le jour
			nuit = false;
			fTime -= mnLever;
			fTime /= (mnCoucher - mnLever);
			fTime *= (float)M_PI;
		}

		//Direction du soleil en fonction de l'heure
		sunDir.X = cos(fTime);
		sunDir.Y = 0.2f;
		sunDir.Z = sin(fTime);
		sunDir.normalize();

		return nuit;
	}

	void updateLights(float boostTime = 0)
	{
		//On recup la direciton du soleil
		bool nuit = getSunDirFromDayTime(SunDirection, 6.0f * 60.0f, 19.0f * 60.0f, boostTime);
		SunPosition = Renderer->Camera->Position + SunDirection * 500.0f;

		//Pendant la journée
		if (!nuit)
		{
			//On definit la couleur
			SunColor = YColor(1.0f, 1.0f, 0.8f, 1.0f);
			SkyColor = YColor(0.0f, 181.f / 255.f, 221.f / 255.f, 1.0f);
			YColor downColor(0.9f, 0.5f, 0.1f, 1);

			SunColor = SunColor.interpolate(downColor, (abs(SunDirection.X)));
			SkyColor = SkyColor.interpolate(downColor, (abs(SunDirection.X)));
		}
		else
		{
			//La nuit : lune blanche et ciel noir
			SunColor = YColor(1, 1, 1, 1);
			SkyColor = YColor(0, 0, 0, 1);
		}

		Renderer->setBackgroundColor(SkyColor);
	}

	void resize(int width, int height) {
		Fbo->resize(width, height);
	}

	/*INPUTS*/

	void keyPressed(int key, bool special, bool down, int p1, int p2) 
	{	
		if (key == 'g')
		{
			SunTimeOffset = down;
		}
		if (key == 'z')
		{
			Avatar->avance = down;
		}
		if (key == 'q')
		{
			Avatar->gauche = down;
		}
		if (key == 's')
		{
			Avatar->recule = down;
		}
		if (key == 'd')
		{
			Avatar->droite = down;
		}
		if (key == ' ')
		{
			int xCG, yCG, zCG;
			YVec3f inter;
			bool isGrounded = World->getRayCollisionGrounded(Renderer->Camera->Position, Renderer->Camera->Position - Renderer->Camera->UpRef * (Avatar->CurrentHeight + 0.5f), inter, xCG, yCG, zCG);
			if (isGrounded == true) {
				Avatar->Jump = down;
			}
		}
		if (key == 'e' && !down) {
			int xC, yC, zC;
			YVec3f inter;
			World->getRayCollision(Renderer->Camera->Position, Renderer->Camera->Position + Renderer->Camera->Direction * 30, inter, xC, yC, zC);
			World->deleteCube(xC, yC, zC);
		}
	}

	void mouseWheel(int wheel, int dir, int x, int y, bool inUi)
	{
		Renderer->Camera->move(Renderer->Camera->Direction * 10.0f * dir);
	}

	void mouseClick(int button, int state, int x, int y, bool inUi)
	{
		
	}

	void mouseMove(int x, int y, bool pressed, bool inUi)
	{
		static int lastx = -1;
		static int lasty = -1;

		if (!pressed)
		{
			lastx = x;
			lasty = y;
			showMouse(true);
		}
		else
		{
			if (lastx == -1 && lasty == -1)
			{
				lastx = x;
				lasty = y;
			}

			int dx = x - lastx;
			int dy = y - lasty;

			if (dx == 0 && dy == 0)
				return;

			lastx = x;
			lasty = y;

			if (MouseBtnState & GUI_MRBUTTON)
			{
				showMouse(false);
				if (GetKeyState(VK_LCONTROL) & 0x80)
				{
					Renderer->Camera->rotateAround((float)-dx / 300.0f);
					Renderer->Camera->rotateUpAround((float)-dy / 300.0f);
					glutWarpPointer(Renderer->ScreenWidth / 2, Renderer->ScreenHeight / 2);
					lastx = Renderer->ScreenWidth / 2;
					lasty = Renderer->ScreenHeight / 2;
				}
				else {
					showMouse(false);
					Renderer->Camera->rotate((float)-dx / 300.0f);
					Renderer->Camera->rotateUp((float)-dy / 300.0f);
					glutWarpPointer(Renderer->ScreenWidth / 2, Renderer->ScreenHeight / 2);
					lastx = Renderer->ScreenWidth / 2;
					lasty = Renderer->ScreenHeight / 2;
				}
			}

			if (MouseBtnState & GUI_MMBUTTON)
			{
				showMouse(false);
				if (GetKeyState(VK_LCONTROL) & 0x80)
				{
					YVec3f strafe = Renderer->Camera->RightVec;
					strafe.Z = 0;
					strafe.normalize();
					strafe *= (float)-dx / 2.0f;

					YVec3f avance = Renderer->Camera->Direction;
					avance.Z = 0;
					avance.normalize();
					avance *= (float)dy / 2.0f;

					Renderer->Camera->move(avance + strafe);
				}
				else {
					YVec3f strafe = Renderer->Camera->RightVec;
					strafe.Z = 0;
					strafe.normalize();
					strafe *= (float)-dx / 5.0f;

					Renderer->Camera->move(Renderer->Camera->UpRef * (float)dy / 5.0f);
					Renderer->Camera->move(strafe);
					glutWarpPointer(Renderer->ScreenWidth / 2, Renderer->ScreenHeight / 2);
					lastx = Renderer->ScreenWidth / 2;
					lasty = Renderer->ScreenHeight / 2;
				}
			}
		}
	}
	
};


#endif