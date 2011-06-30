//----------------------------------------------------------------------------//
// PlayerCharacter.h                                                          //
// Copyright (C) 2009 Lukas Meindl											  //
//----------------------------------------------------------------------------//

#ifndef PLAYERCHARACTER_H
#define PLAYERCHARACTER_H

//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "OgreFramework.h"

#include "SmoothableRadian.h"

#include "GameAnimation.h"
#include "AnimationController.h"

using namespace Ogre;
using namespace Mathematics;

class PlayerCharacter
{
public:
	PlayerCharacter();
	~PlayerCharacter();

	//Functions
	void init(std::string meshName, GameAnimation* gameAnimation);		// Materials, animations

	Ogre::Vector3 updateMovement(unsigned long timeSinceLastFrame, Vector3 inputMoveDir, Ogre::Radian* moveDirRotAngle);
	void updateCharDir(unsigned long timeSinceLastFrame, Ogre::Entity* pointedEntity, Ogre::Vector3* aimPoint, Ogre::Vector3* moveDir, Ogre::Radian &moveDirRotAngle);
	void updateCharDir(unsigned long timeSinceLastFrame, Ogre::Radian angleHorizontal, Ogre::Radian angleVertical, Ogre::Vector3* moveDir, Ogre::Radian &moveDirRotAngle);

private:
	void updateCharDirMoving(Ogre::Vector3* moveDir, unsigned long timeSinceLastFrame, Ogre::Radian &moveDirRotAngle);
	void updateCharDirLooking(Ogre::Vector3* aimPoint, Ogre::Vector3* moveDir, unsigned long timeSinceLastFrame);
	
	// Variables
public:
	std::string identifier;

	Ogre::Entity* ent;
	Ogre::SceneNode* node;
	float maxMoveSpeed;
	float curMoveSpeed;

	AnimCtrlPlayerCharacter* animation;

private:
	Ogre::Vector3				m_lastTargetDirection;
	SmoothableRadian			m_moveRotation;
	static const Ogre::Real		MOVEROTATION_SMOOTH_TARGETTIME;
};

#endif