// Falling Boxes.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "FreeGLUTCallbacks.h"
#include "RagDollApplication.h"



int main(int argc, char **argv)
{

	RagDollApplication ragDoll(ORTHOGRAPHIC);
	return glutmain(argc, argv, 1000, 1000, "Rag Doll", &ragDoll);
}
