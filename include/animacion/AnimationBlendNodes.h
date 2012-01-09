/*
	Animación!
	Copyright (c) 2011 Stefan Grassberger
	See license in Animacion.h
*/

#ifndef ANIMATIONBLENDNODES_H
#define ANIMATIONBLENDNODES_H

#include <string>
#include <vector>
#include <map>

#include "Ogre.h"

typedef enum {
	BLEND_NODE_CLIP,
	BLEND_NODE_LERP,
	BLEND_NODE_ADDITIVE,
	TOTAL_BLEND_NODE_TYPES
} BLEND_NODE_TYPES;

//----------------------------------------------------------------------------//

// TODO: Abstract?
class AnimationBlendNode
{
	// Functions
public:
	AnimationBlendNode();
	~AnimationBlendNode();

	// Getter
	std::string getFlags();
	int getType();

	// Variables
protected:
	std::string flags;
	int type;
};

//----------------------------------------------------------------------------//

class AnimationBlendNodeClip : public AnimationBlendNode
{
	// Functions
public:
	AnimationBlendNodeClip(std::string name, float speed, std::string temporalPlayback, std::string loop, std::string flags);
	~AnimationBlendNodeClip();

	// Getter
	std::string getName();
	float getSpeed();
	bool getTemporalPlayback();
	bool getLoop();

	// Variables
private:
	std::string name;
	float speed;
	bool temporalPlayback;
	bool loop;
};

//----------------------------------------------------------------------------//

class AnimationBlendNodeLERP : public AnimationBlendNode
{
	// Functions
public:
	AnimationBlendNodeLERP(AnimationBlendNode* child1, AnimationBlendNode* child2, float blendFactor, std::string flags);
	~AnimationBlendNodeLERP();

	// Getter
	float getBlendFactor();
	AnimationBlendNode* getFirstChild();
	AnimationBlendNode* getSecondChild();

	// Variables
private:
	float blendFactor;
	AnimationBlendNode* child1;
	AnimationBlendNode* child2;
};

//----------------------------------------------------------------------------//

class AnimationBlendNodeAdditive : public AnimationBlendNode
{
	// Functions
public:
	AnimationBlendNodeAdditive(AnimationBlendNode* child1, AnimationBlendNode* child2);
	~AnimationBlendNodeAdditive();

	// Getter
	AnimationBlendNode* getFirstChild();
	AnimationBlendNode* getSecondChild();

	// Variables
private:
	AnimationBlendNode* child1;
	AnimationBlendNode* child2;
};

//----------------------------------------------------------------------------//

class AnimationBlendTree
{
	// Functions
public:
	AnimationBlendTree(AnimationBlendNode* child);
	~AnimationBlendTree();

	// Getter
	AnimationBlendNode* getChild();

	// Variables
private:
	AnimationBlendNode* child;
};

//----------------------------------------------------------------------------//

class AnimationTransition
{
	// Functions
public:
	AnimationTransition(std::string action, std::string targetState, std::string type, float duration);
	~AnimationTransition();

	bool update(Ogre::Real secSinceLastFrame);	// Continue transition
	void reset();								// Reset timeLeft, progress to original values

	// Getter
	std::string getAction();
	std::string getTargetState();
	std::string getType();
	float getDuration();
	float getProgress();
	float getValue();

	// Setter
	void setTimeLeft(float value);
	void setProgress(float value);

	// Variables
private:
	std::string action;
	std::string targetState;
	std::string type;
	float duration;

	float timeLeft;
	float progress;					// Normalized progress
	float value;					// Actual function curve value
};

//----------------------------------------------------------------------------//

struct animationBlendMaskEntry {
	std::string name;
	float weight;
};

//----------------------------------------------------------------------------//

class AnimationStateLayer
{
	// Functions
public:
	AnimationStateLayer(std::string name, std::string type, std::string startState, std::vector<animationBlendMaskEntry> blendMask);
	~AnimationStateLayer();

	// Getter
	std::string getName();
	std::string getStartStateName();
	std::vector<animationBlendMaskEntry> getBlendMask();

	// Variables
private:
	std::string name;
	std::string type;
	std::string startState;

	std::vector<animationBlendMaskEntry> blendMask;
};

//----------------------------------------------------------------------------//

class AnimationActionState
{
	// Functions
public:
	AnimationActionState(std::string name, AnimationBlendTree* blendTree, std::vector<AnimationTransition*> transitions, std::string targetState);
	~AnimationActionState();

	// Getter
	std::string getName();
	AnimationBlendTree* getBlendTree();
	AnimationTransition* getTransition(std::string actionName);		// Get transition via action
	std::string getTargetState();

	AnimationStateLayer* getStateLayer();
	std::map<std::string, float> getActiveFlags();
	float getFlagValue(std::string flagName);

	// Setter
	void setFlagValue(std::string flagName, float flagValue);
	void setStateLayer(AnimationStateLayer* stateLayer);

	// Variables
private:
	std::string name;
	AnimationBlendTree* blendTree;
	std::vector<AnimationTransition*> transitions;
	std::string targetState;

	AnimationStateLayer* stateLayer;
	std::map<std::string, float> activeFlags;
};

//----------------------------------------------------------------------------//

#endif