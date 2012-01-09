/*
	Animación!
	Copyright (c) 2011 Stefan Grassberger
	See license in Animacion.h
*/

#include "AnimationStateMachine.h"

AnimationStateMachine::AnimationStateMachine(AnimationPipeline* animPipeline, AnimationXML* animXML, AnimationStateLayer* stateLayer)
{
	this->animPipeline = animPipeline;
	this->animXML = animXML;
	this->stateLayer = stateLayer;

	this->nextAction = "";
	this->currentTrans = 0;
}

AnimationStateMachine::~AnimationStateMachine()
{
}

void AnimationStateMachine::init()
{
	// Fill actionStates with all states for the current stateLayer
	this->actionStates.clear();
	this->actionStates = this->animXML->getStates( this->stateLayer->getName() );

	// Set stateLayer shortcut
	for(unsigned int i = 0; i < this->actionStates.size(); i++)
	{
		getState(i)->setStateLayer( this->stateLayer );
	}

	// If a transition is currently active, reset it
	if(this->currentTrans) this->currentTrans->reset();

	// Set current state back to start state defined within stateLayer
	this->currentState = getStateID( this->stateLayer->getStartStateName() );
	this->currentTrans = 0;
}

void AnimationStateMachine::update(Ogre::Real secSinceLastFrame)
{
	// Execute action
	if( !this->nextAction.empty() )
	{
		// Switch to new state if a transition matching this action exists
		switchState( this->nextAction );

		// Reset
		setNextAction( "" );
	}

	// Is this state a transition state?
	if( getCurrentState()->getTargetState().empty() )
	{
		// It is a regular state; nothing to do
	}
	else
	{
		// It is a transition state

		if( transition_state( secSinceLastFrame ) && !getCurrentState()->getTargetState().empty() ) {
			// If transition is finished
			switchStateWithoutTransition( getCurrentState()->getTargetState() );
		}
	}
}

bool AnimationStateMachine::transition_state(Ogre::Real secSinceLastFrame)
{
	// Continue transition
	bool done = this->currentTrans->update( secSinceLastFrame );

	// Blend animations
	if(this->currentTrans->getType().compare("pop") == 0) {
	} else {
		getCurrentState()->setFlagValue( "transition_progress", this->currentTrans->getValue() );
	}

	if(done) {
		// Finished transition, reset
		this->currentTrans->reset();

		return true;
	}

	return false;
}

bool AnimationStateMachine::switchState(std::string actionName)
{
	AnimationTransition* trans = getCurrentState()->getTransition(actionName);
	
	// If there is a transition for this action, use it
	if(trans)
	{
		// If a transition is currently running, we need to adjust the new transition...
		if(this->currentTrans)
		{
			trans->setProgress( 1.0f - this->currentTrans->getProgress() );		// Establish consistent blend values between the two transitions
			trans->setTimeLeft( trans->getDuration() );							// Remaining duration of new transition is independent of previous transition's timeLeft
																				// TODO: Make this shorter if the other transition only ran for a short time
		
			// Reset the current transition before switching to the new one
			this->currentTrans->reset();
		}

		// Switch to new state and transition
		this->currentState = getStateID( trans->getTargetState() );
		this->currentTrans = trans;

		return true;
	}

	// Switch not allowed
	return false;
}

void AnimationStateMachine::switchStateWithoutTransition(std::string targetStateName)
{
	// Switch to another state without a transition (end of transition)
	if(this->currentTrans) this->currentTrans->reset();

	this->currentState = getStateID(targetStateName);
	this->currentTrans = 0;
}

int AnimationStateMachine::getStateID(std::string stateName)
{
	// Find stateID (int) using stateName (string)
	for(unsigned int i = 0; i < this->actionStates.size(); i++)
	{
		if(getState(i)->getName().compare(stateName) == 0)
			return i;
	}

	// TODO: Exception
}

std::string AnimationStateMachine::getName()
{
	return this->stateLayer->getName();
}

AnimationActionState* AnimationStateMachine::getState(int stateID)
{
	return this->actionStates.at(stateID);
}

AnimationActionState* AnimationStateMachine::getState(std::string stateName)
{
	return getState(getStateID(stateName));
}

AnimationActionState* AnimationStateMachine::getCurrentState()
{
	return getState(this->currentState);
}

void AnimationStateMachine::setNextAction(std::string actionName)
{
	this->nextAction = actionName;
}