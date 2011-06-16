/*
	Animación!
	Copyright (c) 2011 Stefan Grassberger
	See license in StevysAnimationSystem.h
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

//----------------------------------------------------------------------------//

class AnimCtrlPlayerCharacter : public AnimationController
{
	// Functions
public:
	AnimCtrlPlayerCharacter(AnimationPipeline* animPipeline, AnimationXML* animXML, std::string name);
	~AnimCtrlPlayerCharacter();

	void updateAimRotation(Ogre::Radian angleHorizontal, Ogre::Radian angleVertical, unsigned long timeSinceLastFrame);
	float interpolateAiming(Ogre::Real angle, float maxDegrees, float prevValue, float maxValue, Ogre::Real maxSpeed, Ogre::Real secSinceLastFrame);

	// Get important bone positions/orientations
	bool hasFlashlight();

	Ogre::Vector3 getRightShoulderPosition();
	Ogre::Vector3 getWeaponBulletOrigin(Ogre::Entity* weaponEnt);
	//Ogre::Vector3 getFlashlightPosition(std::string boneName, Ogre::Entity* weaponEnt, Attachment* attachment);
	//Ogre::Quaternion getFlashlightOrientation(Ogre::Entity* weaponEnt, Attachment* attachment);

	// Attach Weapons to Character
	//void attachWeapon(Weapon* weapon);
	//void detachWeapon(Weapon* weapon);

	// Attach Attachments to Weapon
	//void attachToWeapon(Attachment* attachment, Weapon* weapon);
	//void detachFromWeapon(Attachment* attachment, Weapon* weapon);

	// Variables
private:
	bool flashlightAttached;
};

//----------------------------------------------------------------------------//

class AnimCtrlAiCharacter : public AnimationController
{
	// Functions
public:
	AnimCtrlAiCharacter(AnimationPipeline* animPipeline, AnimationXML* animXML, std::string name);
	~AnimCtrlAiCharacter();

	void updateIdleWalkWeights();

	// Variables
private:
	float idleAnimWeight;
	float walkAnimWeight;
};

#endif