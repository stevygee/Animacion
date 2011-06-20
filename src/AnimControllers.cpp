/*
	Animación!
	Copyright (c) 2011 Stefan Grassberger
	See license in Animacion.h
*/

#include "AnimControllers.h"

AnimCtrlPlayerCharacter::AnimCtrlPlayerCharacter(AnimationPipeline* animPipeline, AnimationXML* animXML, std::string name) : AnimationController(animPipeline, animXML, name)
{
	this->flashlightAttached = false;
}

void AnimCtrlPlayerCharacter::updateAimRotation(Ogre::Radian angleHorizontal, Ogre::Radian angleVertical, unsigned long timeSinceLastFrame)
{
	// This is freaking important! Must be in seconds and an Ogre::Real
	Ogre::Real secSinceLastFrame = (Ogre::Real) timeSinceLastFrame / 1000.f;

	// Set manual frame position
	// ---------------------------------------
	
	float prevValue = animPipeline->getAnimManualTimePos(	"aim" );
	float maxValue = animPipeline->getAnimLength(			"aim" );

	float aimFrame = interpolateAiming( angleHorizontal.valueDegrees(), 360.f, prevValue, maxValue, 3500.f, secSinceLastFrame );

	animPipeline->setAnimManualTimePos( "aim",		aimFrame );
	animPipeline->setAnimManualTimePos( "aimUp",	aimFrame );
	animPipeline->setAnimManualTimePos( "aimDown",	aimFrame );

	// Set blend value
	// ---------------------------------------

	// Modify Angle; animated only from 45° (up) to -45° (down)
	float aimBlend = angleVertical.valueDegrees() - 45.f;
	aimBlend = std::max( 0.f, aimBlend );
	aimBlend = std::min( 90.f, aimBlend );

	prevValue = getFlagValue( "aim_vertical2", 0 ) +
				getFlagValue( "aim_vertical3", 0 );

	aimBlend = interpolateAiming( aimBlend, 90.f, prevValue, 2.f, 2000.f, secSinceLastFrame );

	// Split up blend value from 0 to 1 into three blend values for the LERP tree
	float aimBlendTrees	= (aimBlend > 1.f) ? 1.f : 0.f;			setFlagValue( "aim_vertical1", aimBlendTrees );
	float aimBlend2		= std::min( 1.f, aimBlend );			setFlagValue( "aim_vertical2", aimBlend2 );
	float aimBlend3		= std::max( 1.f, aimBlend ) - 1.f;		setFlagValue( "aim_vertical3", aimBlend3 );
}

float AnimCtrlPlayerCharacter::interpolateAiming(Ogre::Real angle, float maxDegrees, float prevValue, float maxValue, Ogre::Real maxSpeed, Ogre::Real secSinceLastFrame)
{
	// TODO:
	// - Use new value interpolation classes!

	// Get time/value it takes to turn 1 degree
	Ogre::Real oneDegree = maxValue / maxDegrees;
	
	// Get previous and new rotation in degrees
	Ogre::Real oldRot = prevValue / oneDegree;
	Ogre::Real newRot = angle;

	// Smooth aim turnaround interpolation!!11
	// Move from current aim direction to new direction using max speed
	// Ease in/out when the two directions are getting close to each other
	// Does not interpolate over 0->360 degree barrier :)

	// Settings
	//Ogre::Real maxSpeed = 3500.f;
	Ogre::Real smoothDistance = 36.f; // ease (slower than max speed) begins within this distance

	// Do not change these!
	Ogre::Real speed = 0.f;
	Ogre::Real wayToGo = 0.f;
	Ogre::Real minSpeedCutoffAt = 0.2f;

	// Calculate distance from one direction to the other
	wayToGo = (std::max(newRot, oldRot) - std::min(newRot, oldRot));

	// Calculate speed
	if(wayToGo > smoothDistance) {
		speed = maxSpeed; // Move at max velocity
	} else {
		speed = maxSpeed * (wayToGo / smoothDistance); // Ease movement
		if(speed < minSpeedCutoffAt) speed = 0.f; // Stop movement when very slow
	}
	if(newRot < oldRot) { speed = -speed; } // Go backwards

	// Set new turnaround animation position (time)
	oldRot += speed * secSinceLastFrame / 10.f;

	return ( oneDegree * oldRot );
}

/*void AnimCtrlPlayerCharacter::attachWeapon(Weapon* weapon)
{
	if( weapon && weapon->ent ) {
		// Attaching the weapon to the hand bone
		Ogre::Vector3 p = animXML->getWeaponPosition();
		Ogre::Vector3 r = animXML->getWeaponOrientation();
		Ogre::Quaternion q(Ogre::Degree(r.x), Ogre::Vector3::UNIT_X);
		Ogre::Quaternion q2(Ogre::Degree(r.y), Ogre::Vector3::UNIT_Y);
		Ogre::Quaternion q3(Ogre::Degree(r.z), Ogre::Vector3::UNIT_Z);
		q = q * q2 * q3;

		try
		{
			// todo: vorsicht! entity darf nicht schon an node kleben
			Ogre::TagPoint* tagPoint = this->animPipeline->attachEntityToBone( animXML->getWeaponBone(), weapon->ent, q, p );
			setTagPoint( "weapon_on_character", tagPoint );
		}
		catch (...)
		{
			std::cout << "Failed to attach weapon; target bone not found!" << std::endl;
		}

        try
        {
	        weapon->getAttachmentWithType(AttachmentType::ATTACHMENT_TACTICAL_LIGHT);
			this->flashlightAttached = true;
			// TOOD: Somehow set tagPoint again
        }
        catch (...)
        {
			this->flashlightAttached = false;
		}
	}
}

void AnimCtrlPlayerCharacter::detachWeapon(Weapon* weapon)
{
	// Detaching possible only if something is currently attached
	if( weapon->ent && weapon->ent->isAttached() )
	{
		if( weapon->ent->getParentNode() != weapon->ent->getParentSceneNode()) // TODO: What does this do, exactly?
		{
			this->animPipeline->detachEntityFromBone( weapon->ent );
			this->clearTagPoints();

			this->flashlightAttached = false;
		}
	}
}

void AnimCtrlPlayerCharacter::attachToWeapon(Attachment* attachment, Weapon* weapon)
{
	if( weapon && weapon->ent && weapon->ent->hasSkeleton() && attachment && attachment->ent )
	{
		// TODO: Check if slot (bone) is already taken

		// TODO: Get rid of this code block everywhere
		Ogre::String mountBone = "";
		switch(attachment->mountPoint)
		{
			case ATTACHMENT_MOUNT_POINT_TOP:
				mountBone = "topMount";
				break;
			case ATTACHMENT_MOUNT_POINT_BARREL:
			default:
				mountBone = "barrelMount";
				break;
		}

		Ogre::TagPoint* tagPoint = weapon->ent->attachObjectToBone( mountBone, attachment->ent );
		setTagPoint( mountBone, tagPoint );

		if( attachment->attachmentType == AttachmentType::ATTACHMENT_TACTICAL_LIGHT )
			this->flashlightAttached = true;
	}
}

void AnimCtrlPlayerCharacter::detachFromWeapon(Attachment* attachment, Weapon* weapon)
{
	if( weapon && weapon->ent && weapon->ent->hasSkeleton() && attachment && attachment->ent )
	{
		weapon->ent->detachObjectFromBone( attachment->ent );

		// TODO: Get rid of this code block everywhere
		Ogre::String mountBone = "";
		switch(attachment->mountPoint)
		{
			case ATTACHMENT_MOUNT_POINT_TOP:
				mountBone = "topMount";
				break;
			case ATTACHMENT_MOUNT_POINT_BARREL:
			default:
				mountBone = "barrelMount";
				break;
		}

		setTagPoint( mountBone, 0 );

		if( attachment->attachmentType == AttachmentType::ATTACHMENT_TACTICAL_LIGHT )
			this->flashlightAttached = false;
	}
}*/

Ogre::Vector3 AnimCtrlPlayerCharacter::getRightShoulderPosition()
{
	return this->animPipeline->getBoneWorldPosition( "rightShoulder" );
}

/*Ogre::Vector3 AnimCtrlPlayerCharacter::getWeaponBulletOrigin(Ogre::Entity* weaponEnt)
{
	return this->animPipeline->getBoneWorldPosition( "bulletOrigin", "weapon", weaponEnt );
}

bool AnimCtrlPlayerCharacter::hasFlashlight()
{
	return this->flashlightAttached;
}

Ogre::Vector3 AnimCtrlPlayerCharacter::getFlashlightPosition(std::string boneName, Ogre::Entity* weaponEnt, Attachment* attachment)
{
	// TODO: Get rid of this code block everywhere
	Ogre::String mountBone = "";
	switch(attachment->mountPoint)
	{
		case ATTACHMENT_MOUNT_POINT_TOP:
			mountBone = "topMount";
			break;
		case ATTACHMENT_MOUNT_POINT_BARREL:
		default:
			mountBone = "barrelMount";
			break;
	}

	return this->animPipeline->getBoneWorldPosition( boneName, "attachment", weaponEnt, attachment->ent, mountBone );
}

Ogre::Quaternion AnimCtrlPlayerCharacter::getFlashlightOrientation(Ogre::Entity* weaponEnt, Attachment* attachment)
{
	// TODO: Get rid of this code block everywhere
	Ogre::String mountBone = "";
	switch(attachment->mountPoint)
	{
		case ATTACHMENT_MOUNT_POINT_TOP:
			mountBone = "topMount";
			break;
		case ATTACHMENT_MOUNT_POINT_BARREL:
		default:
			mountBone = "barrelMount";
			break;
	}

	return this->animPipeline->getBoneWorldOrientation( "lightConeSource", "attachment", attachment->ent );
}*/

//----------------------------------------------------------------------------//

AnimCtrlAiCharacter::AnimCtrlAiCharacter(AnimationPipeline* animPipeline, AnimationXML* animXML, std::string name) : AnimationController(animPipeline, animXML, name)
{
	srand ( time(NULL) + 123 );
	int random1_raw = rand() % 10 + 1;
	this->idleAnimWeight = random1_raw > 5 ? 0.f : 1.f;

	srand ( time(NULL) + 321 );
	int random2_raw = rand() % 100 + 1;
	float random2 = random2_raw / 100.f;
	this->walkAnimWeight = random2;
}

void AnimCtrlAiCharacter::updateIdleWalkWeights()
{
	this->setFlagValue( "idle_weight", this->idleAnimWeight );
	this->setFlagValue( "walk_weight", this->walkAnimWeight );
}