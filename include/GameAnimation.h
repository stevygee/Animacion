/*
	Animación!
	Copyright (c) 2011 Stefan Grassberger
	See license in Animacion.h
*/

#ifndef GAMEANIMATION_H
#define GAMEANIMATION_H

#include <string>
#include <iostream>
#include <vector>

#include "Ogre.h"

#include "AnimControllers.h"
#include "animacion/AnimationPipeline.h"
#include "animacion/AnimationXML.h"
#include "animacion/AnimationGUI.h"

class GameAnimation
{
	// Types
public:
	typedef enum {
		PLAYER_CHARACTER,
		AI_CHARACTER,
		TOTAL_CHARACTER_TYPES
	} CHARACTER_TYPES;

	struct animatedCharacter {
		std::string name;
		AnimationController* animController;
		AnimationPipeline* animPipeline;
	};

	// Functions
public:
	GameAnimation();
	~GameAnimation();

	void init();
	unsigned long update(unsigned long timeSinceLastFrame);
	void reloadXML();

	AnimationController* registerCharacter(int characterType, Ogre::SceneNode* node, Ogre::Entity* ent);
	void unregisterCharacter(Ogre::Entity* ent);	// Clean up animation stuff, when character entity is removed from scene
	void cycleCharGUI();
private:
	void loadXML();
	AnimationXML* getXML(std::string xmlFileName) throw(int);

	// Variables
public:
	AnimationGUI* animGUI;
private:
	std::map<std::string, AnimationXML*> xmlFiles;
	std::vector<animatedCharacter*> animCharacters;

	unsigned long timeTaken;
	unsigned long startTime;
	Ogre::Timer* m_pTimer;
};

#endif