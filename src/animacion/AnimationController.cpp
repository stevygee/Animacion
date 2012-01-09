/*
	Animación!
	Copyright (c) 2011 Stefan Grassberger
	See license in Animacion.h
*/

#include "AnimationController.h"

AnimationController::AnimationController(AnimationPipeline* animPipeline, AnimationXML* animXML, std::string name)
{
	this->animPipeline = animPipeline;
	this->animXML = animXML;

	this->name = name;

	this->characterSpeed = 1.f;
}

void AnimationController::init()
{
	this->characterSpeed = this->animXML->getCharacterSpeed();

	// Set up AnimationStateMachines according to stateLayers defined in XML
	std::vector<AnimationStateLayer*> stateLayers = this->animXML->getStateLayers();
	for(unsigned int i = 0; i < stateLayers.size(); i++)
	{
		AnimationStateMachine* stateMachine = new AnimationStateMachine( this->animPipeline, animXML, stateLayers.at(i) );
		stateMachine->init();
		this->stateMachines.push_back( stateMachine );
	}
}

AnimationController::~AnimationController()
{
	// Delete AnimationStateMachines
	for(std::vector<AnimationStateMachine*>::iterator i = this->stateMachines.begin(); i != this->stateMachines.end();)
	{
		delete *i;
		i = this->stateMachines.erase(i);
	}
}

void AnimationController::update(Ogre::Real secSinceLastFrame)
{
	// Update the AnimationStateMachines themselves
	for(unsigned int i = 0; i < this->stateMachines.size(); i++)
	{
		this->stateMachines.at(i)->update( secSinceLastFrame );
	}
}

void AnimationController::reset()
{
	for(unsigned int i = 0; i < this->stateMachines.size(); i++)
	{
		this->stateMachines.at(i)->init();
	}
}

std::string AnimationController::getName()
{
	return this->name;
}

std::string AnimationController::getEntityName() throw(int)
{
	return this->animPipeline->getEntityName();
}

Ogre::Vector3 AnimationController::getNodePosition() throw(int)
{
	return this->animPipeline->getNodePosition();
}

Ogre::AnimationStateIterator AnimationController::getAnimStateIter() throw(int)
{
	return this->animPipeline->getAnimStateIter();
}

Ogre::TagPoint* AnimationController::getTagPoint(std::string name)
{
	return this->animPipeline->getTagPoint(name);
}

std::vector<AnimationActionState*> AnimationController::getActiveStates()
{
	std::vector<AnimationActionState*> activeStates;

	for(unsigned int i = 0; i < this->stateMachines.size(); i++)
	{
		activeStates.push_back(	this->stateMachines.at(i)->getCurrentState() );
	}

	return activeStates;
}

bool AnimationController::isStateActive(std::string stateName, std::string stateLayerName)
{
	// TODO: Optimize

	for(unsigned int i = 0; i < this->stateMachines.size(); i++)
	{
		AnimationActionState* state = this->stateMachines.at(i)->getCurrentState();

		if(	state->getStateLayer()->getName().compare( stateLayerName ) == 0 )
		{
			return state->getName().compare( stateName ) == 0;
		}
	}

	// TODO: Exception
}

Ogre::Vector3 AnimationController::getDirectionVector(std::string name)
{
	return this->directionVectors[name];
}

float AnimationController::getAnimLength(std::string animName)
{
	return this->animPipeline->getAnimLength( animName );
}

float AnimationController::getAnimWeight(std::string animName, std::string blendMaskName)
{
	return this->animPipeline->getAnimWeight( animName, blendMaskName );
}

float AnimationController::getCharacterSpeed()
{
	return this->characterSpeed;
}

AnimationStateMachine* AnimationController::getStateMachine(std::string name) throw(int)
{
	for(unsigned int i = 0; i < this->stateMachines.size(); i++)
	{
		if( this->stateMachines.at(i)->getName().compare( name ) == 0 )
			return this->stateMachines.at(i);
	}

	throw(1); // StateMachine not found
}

float AnimationController::getFlagValue(std::string flagName, int stateMachineID)
{
	return this->stateMachines.at(stateMachineID)->getCurrentState()->getFlagValue( flagName );
}

void AnimationController::clearTagPoints()
{
	this->animPipeline->clearTagPoints();
}

void AnimationController::setTagPoint(std::string name, Ogre::TagPoint* tagPoint)
{
	this->animPipeline->setTagPoint(name, tagPoint);
}

void AnimationController::setNextAction(std::string actionName)
{
	// Just apply for all stateMachines / layers
	// If one action is supposed to affect one layer, then only define it in one layer in the XML file
	for(unsigned int i = 0; i < this->stateMachines.size(); i++)
	{
		this->stateMachines.at(i)->setNextAction( actionName );
	}
}

void AnimationController::setFlagValue(std::string flagName, float flagValue)
{
	// Just set for all active states
	for(unsigned int i = 0; i < this->stateMachines.size(); i++)
	{
		this->stateMachines.at(i)->getCurrentState()->setFlagValue( flagName, flagValue );
	}
}

void AnimationController::setDirectionVector(std::string name, Ogre::Vector3 vector)
{
	this->directionVectors[name] = vector;
}