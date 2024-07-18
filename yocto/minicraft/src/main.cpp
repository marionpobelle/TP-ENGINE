//Includes application
#include <conio.h>
#include <vector>
#include <string>
#include <windows.h>

#include "engine_minicraft.h"

#ifdef __cplusplus
extern "C" {
#endif

	__declspec(dllexport) DWORD NvOptimusEnablement = 1;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#ifdef __cplusplus
}
#endif

/**
  * POINT D'ENTREE PRINCIPAL
  **/
int main(int argc, char* argv[])
{ 
	YEngine * engine = MEngineMinicraft::getInstance();

	engine->initBase(argc,argv);

	glutMainLoop(); 

	return 0;
}

