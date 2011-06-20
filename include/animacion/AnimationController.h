/*
	Animación!
	Copyright (c) 2011 Stefan Grassberger
	See license in Animacion.h
*/

#ifndef ANIMATIONCONTROLLER_H
#define ANIMATIONCONTROLLER_H

#include <string>

#include "AnimationStateMachine.h"
#include "AnimationPipeline.h"
//#include "../Item.h"

class AnimationController
{
	// Functions
public:
	AnimationController(AnimationPipeline* animPipeline, AnimationXML* animXML, std::string name);
	~AnimationController();

	void init();
	void update(Ogre::Real secSinceLastFrame);
	void reset();

	// Getters
	std::string getName();
	std::string getEntityName() throw(int);
	Ogre::Vector3 getNodePosition() throw(int);
	Ogre::AnimationStateIterator getAnimStateIter() throw(int);

	Ogre::TagPoint* getTagPoint(std::string name);
	std::vector<AnimationActionState*> getActiveStates();
	bool isStateActive(std::string stateName, std::string stateLayerName);
	Ogre::Vector3 getDirectionVector(std::string name);

	float getAnimLength(std::string animName);
	float getAnimWeight(std::string animName, std::string blendMaskName);
	float getCharacterSpeed();
protected:
	AnimationStateMachine* getStateMachine(std::string name) throw(int);
	float getFlagValue(std::string flagName, int stateMachineID);

	// Setters
public:
	void clearTagPoints();
	void setTagPoint(std::string name, Ogre::TagPoint* tagPoint);
	void setNextAction(std::string actionName);
	void setFlagValue(std::string flagName, float flagValue);
	void setDirectionVector(std::string name, Ogre::Vector3 vector);

	// Variables
protected:
	AnimationPipeline* animPipeline;
	AnimationXML* animXML;

	std::string name;
	std::vector<AnimationStateMachine*> stateMachines;
	std::map<std::string, float> activeFlags;

	float characterSpeed;

	std::map<std::string, Ogre::Vector3> directionVectors;
};

#endif