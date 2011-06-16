/*
	Animación!
	Copyright (c) 2011 Stefan Grassberger
	See license in StevysAnimationSystem.h
*/

#include "AnimationBlendNodes.h"

//----------------------------------------------------------------------------//
// State Layer
//----------------------------------------------------------------------------//

AnimationStateLayer::AnimationStateLayer(std::string name, std::string type, std::string startState, std::vector<animationBlendMaskEntry> blendMask)
{
	// TODO: needs good default values

	// Validate XML values, if invalid use default value
	this->name			= name.empty()			? ""		: name;
	this->type			= type.empty()			? "lerp"	: type;
	this->startState	= startState.empty()	? ""		: startState;
	//this->blendMask		= targetState.empty()	? ""		: targetState;
	this->blendMask		= blendMask;
}

AnimationStateLayer::~AnimationStateLayer()
{
}	

std::string AnimationStateLayer::getName()
{
	return this->name;
}

std::string AnimationStateLayer::getStartStateName()
{
	return this->startState;
}

std::vector<animationBlendMaskEntry> AnimationStateLayer::getBlendMask()
{
	return this->blendMask;
}

//----------------------------------------------------------------------------//
// State
//----------------------------------------------------------------------------//

AnimationActionState::AnimationActionState(std::string name, AnimationBlendTree* blendTree, std::vector<AnimationTransition*> transitions, std::string targetState, std::string stateLayerName)
{
	// Validate XML values, if invalid use default value
	this->name				= name.empty()			? ""		: name;
	this->blendTree			= blendTree;
	this->transitions		= transitions;
	this->targetState		= targetState.empty()	? ""		: targetState;

	this->stateLayerName	= stateLayerName.empty()	? ""	: stateLayerName;
}

AnimationActionState::~AnimationActionState()
{
}	

std::string AnimationActionState::getName()
{
	return this->name;
}

AnimationBlendTree* AnimationActionState::getBlendTree()
{
	return this->blendTree;
}

AnimationTransition* AnimationActionState::getTransition(std::string actionName)
{
	for(unsigned int i = 0; i < this->transitions.size(); i++)
	{
		// TODO: This only returns the first, what if there are more?
		if(this->transitions.at(i)->getAction().compare(actionName) == 0)
			return this->transitions.at(i);
	}

	return 0;
}

std::string AnimationActionState::getTargetState()
{
	return this->targetState;
}

std::string AnimationActionState::getStateLayerName()
{
	return this->stateLayerName;
}

std::map<std::string, float> AnimationActionState::getActiveFlags()
{
	return this->activeFlags;
}

float AnimationActionState::getFlagValue(std::string flagName)
{
	return this->activeFlags[flagName];
}

void AnimationActionState::setFlagValue(std::string flagName, float flagValue)
{
	this->activeFlags[flagName] = flagValue;
}

//----------------------------------------------------------------------------//
// Transition
//----------------------------------------------------------------------------//

AnimationTransition::AnimationTransition(std::string action, std::string targetState, std::string type, float duration)
{
	// Validate XML values, if invalid use default value
	this->action		= action.empty()		? ""		: action;
	this->targetState	= targetState.empty()	? ""		: targetState;
	this->type			= type.empty()			? "linear"	: type;
	this->duration		= (duration == 0.0f)	? 10.0f 	: duration;

	reset();
}

AnimationTransition::~AnimationTransition()
{
}

std::string AnimationTransition::getAction()
{
	return this->action;
}

std::string AnimationTransition::getTargetState()
{
	return this->targetState;
}

std::string AnimationTransition::getType()
{
	return this->type;
}

float AnimationTransition::getDuration()
{
	return this->duration;
}

float AnimationTransition::getProgress()
{
	return this->progress;
}

float AnimationTransition::getValue()
{
	return this->value;
}

void AnimationTransition::setTimeLeft(float value)
{
	this->timeLeft = value;
}

void AnimationTransition::setProgress(float value)
{
	this->progress = value;
}

bool AnimationTransition::update(Ogre::Real secSinceLastFrame)
{
	// Calculate transition progress
	if( this->type.compare("linear") == 0 || this->type.compare("smooth") == 0 )
	{
		this->progress += (1.0f - this->progress) / this->timeLeft * secSinceLastFrame;
	}

	// Calculate actual value resulting from a function curve
	if( this->type.compare("linear") == 0 )
	{													// Linear curve
		this->value = this->progress;
	}
	else if( this->type.compare("smooth") == 0 )
	{													// Bézier curve
		// Gregory, J.: Game Engine Architecture.
		// A K Peters, Nattick, Massachusetts, 1st Edition, 2009.
		// Page 529.

		float u = this->progress;	// normalized time
		float v = 1 - u;			// inverse normalized time

		this->value = 3*v*u*u + u*u*u;
	}	// Do nothing if type = "pop"

	// Continue transition
	this->timeLeft -= secSinceLastFrame;
	this->timeLeft = std::max<float>( this->timeLeft, 0.0f );

	return ( this->timeLeft <= 0.0f );
}

void AnimationTransition::reset()
{
	this->timeLeft = this->duration;
	this->progress = 0.0f;
	this->value = 0.0f;
}

//----------------------------------------------------------------------------//
// BlendTree
//----------------------------------------------------------------------------//

AnimationBlendTree::AnimationBlendTree(AnimationBlendNode* child)
{
	// Validate XML values, if invalid use default value
	this->child = child;
}

AnimationBlendTree::~AnimationBlendTree()
{
}

AnimationBlendNode* AnimationBlendTree::getChild()
{
	return this->child;
}

//----------------------------------------------------------------------------//
// BlendNode
//----------------------------------------------------------------------------//

AnimationBlendNode::AnimationBlendNode()
{
}

AnimationBlendNode::~AnimationBlendNode()
{
}

std::string AnimationBlendNode::getFlags()
{
	return this->flags;
}

int AnimationBlendNode::getType()
{
	return this->type;
}

//----------------------------------------------------------------------------//
// Clip
//----------------------------------------------------------------------------//

AnimationBlendNodeClip::AnimationBlendNodeClip(std::string name, float speed, std::string temporalPlayback, std::string loop, std::string flags)
{
	// Validate XML values, if invalid use default value
	this->type				= BLEND_NODE_CLIP;

	this->name				= name.empty()								? ""		: name;
	this->speed				= (speed == 0.0f)							? 1.0f		: speed;
	this->temporalPlayback	= (!temporalPlayback.empty() &&
								temporalPlayback.compare("false") == 0)	? false		: true;
	this->loop				= (!loop.empty() &&
								loop.compare("false") == 0)				? false		: true;
	this->flags				= flags.empty()								? ""		: flags;
}

AnimationBlendNodeClip::~AnimationBlendNodeClip()
{
}

std::string AnimationBlendNodeClip::getName()
{
	return this->name;
}

float AnimationBlendNodeClip::getSpeed()
{
	return this->speed;
}

bool AnimationBlendNodeClip::getTemporalPlayback()
{
	return this->temporalPlayback;
}

bool AnimationBlendNodeClip::getLoop()
{
	return this->loop;
}

//----------------------------------------------------------------------------//
// LERP
//----------------------------------------------------------------------------//

AnimationBlendNodeLERP::AnimationBlendNodeLERP(AnimationBlendNode* child1, AnimationBlendNode* child2, float blendFactor, std::string flags)
{
	// Validate XML values, if invalid use default value
	this->type			= BLEND_NODE_LERP;
	this->flags			= flags.empty()		? ""		: flags;

	this->child1		= child1;
	this->child2		= child2;
	this->blendFactor	= blendFactor; // 0.0f is default
}

AnimationBlendNodeLERP::~AnimationBlendNodeLERP()
{
}

float AnimationBlendNodeLERP::getBlendFactor()
{
	return this->blendFactor;
}

AnimationBlendNode* AnimationBlendNodeLERP::getFirstChild()
{
	return this->child1;
}

AnimationBlendNode* AnimationBlendNodeLERP::getSecondChild()
{
	return this->child2;
}

//----------------------------------------------------------------------------//
// Additive
//----------------------------------------------------------------------------//

AnimationBlendNodeAdditive::AnimationBlendNodeAdditive(AnimationBlendNode* child1, AnimationBlendNode* child2)
{
	// Validate XML values, if invalid use default value
	this->type			= BLEND_NODE_ADDITIVE;

	this->child1		= child1;
	this->child2		= child2;
}

AnimationBlendNodeAdditive::~AnimationBlendNodeAdditive()
{
}

AnimationBlendNode* AnimationBlendNodeAdditive::getFirstChild()
{
	return this->child1;
}

AnimationBlendNode* AnimationBlendNodeAdditive::getSecondChild()
{
	return this->child2;
}