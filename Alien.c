#include <stdlib.h>
#include <stdio.h>
#include <Alien.h>

Alien createAlien(char* icon, int xloc, int yloc, int numOfIterations)
{
	Alien alien;
	alien.icon = icon;
	alien.xloc = xloc;
	alien.yloc = yloc;
	alien.numOfIterations = numOfIterations;

	return alien;
}
