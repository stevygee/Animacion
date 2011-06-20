/*
	Animación!
	Copyright (c) 2011 Stefan Grassberger
	See license in Animacion.h
*/

#ifndef ANIMCONTROLLERS_H
#define ANIMCONTROLLERS_H

#include <string>

#include "AnimationController.h"

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