#ifndef ALIEN_H
#define ALIEN_H

struct _Alien
{
	char* icon; //what the alien will look like
	int xloc, yloc; //where the alien will be
	int numOfIterations; //subtract from here to determine when to move
};

typedef struct _Alien Alien;

Alien createAlien(char* icon, int xloc, int yloc, int numOfIterations);

#endif
