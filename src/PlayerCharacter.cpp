//----------------------------------------------------------------------------//
// PlayerCharacter.cpp                                                        //
// Copyright (C) 2009 - 2010 Lukas Meindl									  //
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "PlayerCharacter.h"

//----------------------------------------------------------------------------//
// Functions                                                                  //
//----------------------------------------------------------------------------//


unsigned int PlayerCharacter::playerNum = 0;

const Ogre::Real PlayerCharacter::MOVEROTATION_SMOOTH_TARGETTIME = Ogre::Real(300.f);

PlayerCharacter::PlayerCharacter(GameRenderer* gameRenderer):
m_moveRotation(MOVEROTATION_SMOOTH_TARGETTIME)
{
	//this->collisionTool = NULL;

	this->gameRenderer = gameRenderer;

	std::stringstream identifierStream;
	identifierStream << "Player " << playerNum;
	this->identifier =  identifierStream.str();
	playerNum++;

	moveSpeed = 0.25f;
}

PlayerCharacter::~PlayerCharacter()
{
	
}

void PlayerCharacter::init(std::string meshName, GameAnimation* gameAnimation)
{
	// Entity / Node
	this->ent = this->gameRenderer->getSceneManager()->createEntity("Eva", meshName);
    this->node = this->gameRenderer->getSceneManager()->getRootSceneNode()->createChildSceneNode();
    this->node->attachObject(this->ent);

	// Animation
	AnimationController* animController = gameAnimation->registerCharacter(GameAnimation::PLAYER_CHARACTER, this->node, this->ent);
	this->animation = static_cast<AnimCtrlPlayerCharacter*>( animController );
}

//Updating function
void PlayerCharacter::update(unsigned long timeSinceLastFrame, Ogre::Entity* pointedEntity, Ogre::Vector3* aimPoint)
{
}

void PlayerCharacter::updateCharDir(unsigned long timeSinceLastFrame, Ogre::Entity* pointedEntity, Ogre::Vector3* aimPoint, Ogre::Vector3* moveDir, Ogre::Radian &moveDirRotAngle)
{
	if(pointedEntity)
		updateCharDirLooking(aimPoint, moveDir, timeSinceLastFrame);

	updateCharDirMoving(moveDir, timeSinceLastFrame, moveDirRotAngle);
}

//Moves the Character
Ogre::Vector3 PlayerCharacter::updateMovement(unsigned long timeSinceLastFrame, Vector3 inputMoveDir, Ogre::Radian* moveDirRotAngle)
{
	//static const float maxUpdateDelay = 0.1f;

	if(inputMoveDir.isZeroLength())
	{
		this->animation->setNextAction("stop");
		return node->getPosition();
	}
	else
	{
		//Ogre::Vector3 moveVector  = Quaternion(*moveDirRotAngle, Vector3::UNIT_Y) * inputMoveDir;
		Ogre::Vector3 moveVector  = this->node->getOrientation() * inputMoveDir; // TODO: Running backwards goes crazy, turn too fast
		//Ogre::Vector3 moveVector = inputMoveDir;
		moveVector.normalise();

		this->animation->setNextAction("run");
		
		Ogre::Vector3 newPos = node->getPosition() + moveVector * moveSpeed * (Ogre::Real)timeSinceLastFrame;
		this->node->setPosition(newPos);

		// Get Agent speed for animation
		this->animation->setFlagValue( "character_speed", moveSpeed * moveVector.length() );

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

	// Ist das Ziel links oder rechts vom Richtungsvektor?
	// siehe http://www.c-plusplus.de/forum/viewtopic-var-t-is-266934.html und http://www.mikrocontroller.net/topic/105993
	Vector3 A = node->getPosition();					// Stützvektor
	Vector3 B = nodeDirection + node->getPosition();	// Zielpunkt Richtungsvektor (Node Richtung)
	Vector3 C = (*aimPoint);							// Gefragter Punkt
	Vector3 R = B-A;									// Richtungsvektor;
	
	if((R.z * (C.x-A.x) - R.x * (C.z - A.z)) > 0)
		angleHorizontal = -angleHorizontal; // Winkel auf der linken Seite negativ
	angleHorizontal += Ogre::Radian(Ogre::Degree(180)); // -180 bis 180 wird zu 0 bis 360 (180 = vorn)

	// Vertical angle
	Ogre::Radian angleVertical = aimDirection.angleBetween(aimDirectionY);

	if(aimDirectionY.y > 0.f)
		angleVertical = -angleVertical; // Winkel negativ, wenn man "hinauf schaut"
	angleVertical += Ogre::Radian(Ogre::Degree(90)); // -90 bis 90 wird zu 0 bis 180 (0 = oben, 180 = unten)

	// Use theses angles
	this->animation->updateAimRotation(angleHorizontal, angleVertical, timeSinceLastFrame);	// For Animation
	this->animation->setDirectionVector("nodeDirection",	nodeDirection);			// For AnimationGUI
	this->animation->setDirectionVector("aimDirHorizontal",	aimDirection);
	this->animation->setDirectionVector("aimDirVertical",	aimDirectionY);
}

void PlayerCharacter::updateCharDirMoving(Ogre::Vector3* moveDir, unsigned long timeSinceLastFrame, Ogre::Radian &moveDirRotAngle)
{
	if(!moveDir->isZeroLength())
	{
		//Ogre::Vector3 targetDirection = Quaternion(moveDirRotAngle, Vector3::UNIT_Y) * (*moveDir);
		Ogre::Vector3 targetDirection = this->node->getOrientation() * *moveDir;

		Ogre::Radian rotationDifferenceToLastValue = m_lastTargetDirection.angleBetween(targetDirection);

		if(rotationDifferenceToLastValue > Radian(0.001))
		{
			Quaternion rotationQuat = (Vector3::UNIT_X).getRotationTo(targetDirection, Vector3::UNIT_Y);

			Radian targetRotation = rotationQuat.getYaw();

			m_moveRotation.setTargetValueWithAdjustedSmoothtime(targetRotation, true);
		}


		m_moveRotation.smoothValue(timeSinceLastFrame);
		this->node->setDirection(0,0,-1, Node::TS_WORLD);
		this->node->yaw(m_moveRotation.getCurrentValue());

		m_lastTargetDirection = targetDirection;
	}
}