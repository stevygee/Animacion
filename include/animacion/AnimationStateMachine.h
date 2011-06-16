/*
	Animación!
	Copyright (c) 2011 Stefan Grassberger
	See license in StevysAnimationSystem.h
*/

#ifndef ANIMATIONSTATEMACHINE_H
#define ANIMATIONSTATEMACHINE_H

#include <iostream>
#include <string>
#include <vector>

#include "AnimationPipeline.h"
#include "AnimationXML.h"

class AnimationStateMachine
{
	// Functions
public:
	AnimationStateMachine(AnimationPipeline* animPipeline, AnimationXML* animXML, AnimationStateLayer* stateLayer);
	~AnimationStateMachine();

	void init();								// Init states, current state, transition, can be used anytime (XML reload)
	void update(Ogre::Real secSinceLastFrame);	// Updates current state

	bool transition_state(Ogre::Real secSinceLastFrame);

	bool switchState(std::string actionName);	// Switch to another state using a transition
	void switchStateWithoutTransition(std::string targetStateName);		// For transition states that end themselves

	std::string getName();

	int getStateID(std::string stateName);
	AnimationActionState* getState(int stateID);
	AnimationActionState* getState(std::string stateName);
	AnimationActionState* getCurrentState();

	void setNextAction(std::string actionName);

	// Variables
private:
	AnimationPipeline* animPipeline;
	AnimationXML* animXML;
	AnimationStateLayer* stateLayer;
	
	std::string nextAction;
	std::vector<AnimationActionState*> actionStates;

	int currentState;
	AnimationTransition* currentTrans;
};

#endif