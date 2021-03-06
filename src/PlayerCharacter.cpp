//----------------------------------------------------------------------------//
// PlayerCharacter.cpp                                                        //
// Copyright (C) 2009 - 2010 Lukas Meindl									  //
//----------------------------------------------------------------------------//

#include "PlayerCharacter.h"

const Ogre::Real PlayerCharacter::MOVEROTATION_SMOOTH_TARGETTIME = Ogre::Real(300.f);

PlayerCharacter::PlayerCharacter():
m_moveRotation(MOVEROTATION_SMOOTH_TARGETTIME)
{
	std::stringstream identifierStream;
	identifierStream << "Player";
	this->identifier =  identifierStream.str();

	maxMoveSpeed = 0.25f;
	curMoveSpeed = 0.f;
}

PlayerCharacter::~PlayerCharacter() {}

void PlayerCharacter::init(std::string meshName, GameAnimation* gameAnimation)
{
	// Entity / Node
	this->ent = OgreFramework::getSingletonPtr()->m_pSceneMgr->createEntity("Eva", meshName);
    this->node = OgreFramework::getSingletonPtr()->m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
    this->node->attachObject(this->ent);

	// Animation
	AnimationController* animController = gameAnimation->registerCharacter(GameAnimation::PLAYER_CHARACTER, this->node, this->ent);
	this->animation = static_cast<AnimCtrlPlayerCharacter*>( animController );
}

void PlayerCharacter::updateCharDir(unsigned long timeSinceLastFrame, Ogre::Entity* pointedEntity, Ogre::Vector3* aimPoint, Ogre::Vector3* moveDir, Ogre::Radian &moveDirRotAngle)
{
	if(pointedEntity)
		updateCharDirLooking(aimPoint, moveDir, timeSinceLastFrame);

	updateCharDirMoving(moveDir, timeSinceLastFrame, moveDirRotAngle);
}

void PlayerCharacter::updateCharDir(unsigned long timeSinceLastFrame, Ogre::Radian angleHorizontal, Ogre::Radian angleVertical, Ogre::Vector3* moveDir, Ogre::Radian &moveDirRotAngle)
{
	// Use theses angles
	this->animation->updateAimRotation(angleHorizontal, angleVertical, timeSinceLastFrame);	// For Animation
	/*this->animation->setDirectionVector("nodeDirection",	nodeDirection);					// For AnimationGUI...
	this->animation->setDirectionVector("aimDirHorizontal",	aimDirection);
	this->animation->setDirectionVector("aimDirVertical",	aimDirectionY);*/

	updateCharDirMoving(moveDir, timeSinceLastFrame, moveDirRotAngle);
}

//Moves the Character
Ogre::Vector3 PlayerCharacter::updateMovement(unsigned long timeSinceLastFrame, Vector3 inputMoveDir, Ogre::Radian* moveDirRotAngle)
{
	//static const float maxUpdateDelay = 0.1f;
	//Ogre::Vector3 moveVector  = Quaternion(*moveDirRotAngle, Vector3::UNIT_Y) * inputMoveDir;

	Ogre::Vector3 moveVector  = this->node->getOrientation() * inputMoveDir;
	moveVector.normalise();

	// Acceleration
	if( inputMoveDir.isZeroLength() )
	{
		curMoveSpeed -= 0.001f * timeSinceLastFrame;
		curMoveSpeed = (curMoveSpeed < 0.001f) ? 0.f : curMoveSpeed;
	}
	else
	{
		curMoveSpeed += 0.001f * timeSinceLastFrame;
		curMoveSpeed = (curMoveSpeed > maxMoveSpeed) ? maxMoveSpeed : curMoveSpeed;
	}

	// Standing or running?
	if( curMoveSpeed == 0.f )
	{
		this->animation->setNextAction("stop");

		return node->getPosition();
	}
	else
	{
		this->animation->setNextAction("run");
		
		Ogre::Vector3 newPos = node->getPosition() + moveVector * curMoveSpeed * (Ogre::Real)timeSinceLastFrame;
		this->node->setPosition(newPos);

		// Pass actual character speed on to animation system
		this->animation->setFlagValue( "character_speed", curMoveSpeed );

		return newPos;
	}
}

void PlayerCharacter::updateCharDirLooking(Ogre::Vector3* aimPoint, Ogre::Vector3* moveDir, unsigned long timeSinceLastFrame)
{
	// Get directions
	Ogre::Vector3 nodeDirection = this->node->getOrientation() * Vector3::UNIT_X;
	Ogre::Vector3 aimDirection = *aimPoint - this->node->getPosition();
	Ogre::Vector3 aimDirectionY = *aimPoint - this->animation->getRightShoulderPosition();

	nodeDirection.y = 0;
	aimDirection.y = 0;
	nodeDirection.normalise();
	aimDirection.normalise();
	aimDirectionY.normalise();

	// Horizontal angle
	Ogre::Radian angleHorizontal = nodeDirection.angleBetween(aimDirection);

	// Is the target on the left or the right side of the direction vector?
	// see http://www.c-plusplus.de/forum/viewtopic-var-t-is-266934.html and http://www.mikrocontroller.net/topic/105993 (German)
	Vector3 A = node->getPosition();					// Stützvektor
	Vector3 B = nodeDirection + node->getPosition();	// Zielpunkt Richtungsvektor (Node Richtung)
	Vector3 C = (*aimPoint);							// Gefragter Punkt
	Vector3 R = B-A;									// Richtungsvektor
	
	if((R.z * (C.x-A.x) - R.x * (C.z - A.z)) > 0)
		angleHorizontal = -angleHorizontal; // Make angle negative if on left side
	angleHorizontal += Ogre::Radian(Ogre::Degree(180)); // From -180° to 180° becomes 0° to 360° (where 180° is forwards)

	// Vertical angle
	Ogre::Radian angleVertical = aimDirection.angleBetween(aimDirectionY);

	if(aimDirectionY.y > 0.f)
		angleVertical = -angleVertical; // Angle negative when looking up
	angleVertical += Ogre::Radian(Ogre::Degree(90)); // From -90° to 90° becomes 0° to 180° (where 0° is up and 180° is down)

	// Use theses angles
	this->animation->updateAimRotation(angleHorizontal, angleVertical, timeSinceLastFrame);	// For Animation
	this->animation->setDirectionVector("nodeDirection",	nodeDirection);					// For AnimationGUI...
	this->animation->setDirectionVector("aimDirHorizontal",	aimDirection);
	this->animation->setDirectionVector("aimDirVertical",	aimDirectionY);
}

void PlayerCharacter::updateCharDirMoving(Ogre::Vector3* moveDir, unsigned long timeSinceLastFrame, Ogre::Radian &moveDirRotAngle)
{
	 // TODO: Running backwards goes crazy, turn too fast

	// http://www.ogre3d.org/tikiwiki/Intermediate+Tutorial+1&structure=Tutorials
	// http://www.ogre3d.org/tikiwiki/Quaternion+and+Rotation+Primer#Q._How_can_I_make_my_objects_rotate_smoothly_You_mentioned_slerp_etc_

	if(!moveDir->isZeroLength())
	{
		Ogre::Vector3 src = this->node->getOrientation() * Ogre::Vector3::UNIT_Z;

		if ((1.0f + src.dotProduct(*moveDir)) < 0.0001f) 
		{
			this->node->yaw(Ogre::Degree(180));
		}
		else
		{
			Ogre::Quaternion quat = src.getRotationTo(*moveDir);
			this->node->rotate(quat);
		}
	}

	/*if(!moveDir->isZeroLength())
	{
		//Ogre::Vector3 targetDirection = Quaternion(moveDirRotAngle, Vector3::UNIT_Y) * (*moveDir);
		Ogre::Vector3 targetDirection = this->node->getOrientation() * *moveDir;

		Ogre::Radian rotationDifferenceToLastValue = m_lastTargetDirection.angleBetween(targetDirection);

		Radian targetRotation;

		if(rotationDifferenceToLastValue > Ogre::Radian(Ogre::Real(0.001)))
		{
			Quaternion rotationQuat = (Vector3::UNIT_X).getRotationTo(targetDirection, Vector3::UNIT_Y);

			Radian targetRotation = rotationQuat.getYaw();

			//m_moveRotation.setTargetValueWithAdjustedSmoothtime(targetRotation, true);
			m_moveRotation = targetRotation;
		}


		//m_moveRotation.smoothValue(timeSinceLastFrame);
		this->node->setDirection(0,0,-1, Node::TS_WORLD);
		this->node->yaw(m_moveRotation);

		m_lastTargetDirection = targetDirection;
	}*/
}