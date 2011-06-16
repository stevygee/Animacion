/*
	Animación!
	Copyright (c) 2011 Stefan Grassberger
	See license in StevysAnimationSystem.h
*/

#ifndef ANIMATIONPIPELINE_H
#define ANIMATIONPIPELINE_H

#include <iostream>

#include "Ogre.h"
#include <OgreTagPoint.h>

#include "AnimationBlendNodes.h"
#include "AnimationXML.h"

struct animStateAdvanced {
	float speed;
	bool temporalPlayback;
	float manualTimePos;
};

class AnimationPipeline
{
	// Functions
public:
	AnimationPipeline(AnimationXML* animXML, Ogre::SceneNode* node, Ogre::Entity *ent);
	~AnimationPipeline();

	void init(bool firstInit = true);
	void update(std::vector<AnimationActionState*> activeStates, Ogre::Real secSinceLastFrame);

private:
	// Blending
	void blend(AnimationBlendTree* blendTree, std::vector<animationBlendMaskEntry> blendMask, std::map<std::string, float> flags);
	void applyBlendNode(AnimationBlendNode* bn, std::vector<animationBlendMaskEntry> blendMask, std::map<std::string, float> flags, float weight = 1.0f);
	void applyClip(AnimationBlendNodeClip* clip, std::vector<animationBlendMaskEntry> blendMask, std::map<std::string, float> flags, float weight = 1.0f);
	void applyLERP(AnimationBlendNodeLERP* lerp, std::vector<animationBlendMaskEntry> blendMask, std::map<std::string, float> flags, float weight = 1.0f);
	void applyAdditive(AnimationBlendNodeAdditive* additive, std::vector<animationBlendMaskEntry> blendMask, std::map<std::string, float> flags, float weight = 1.0f);
	void updateAnimState(Ogre::AnimationState *animState, Ogre::Real secSinceLastFrame);			// Updates one animationState

	// Related to blend mask
	std::vector<animationBlendMaskEntry> AnimationPipeline::generateBlendMask(std::vector<animationBlendMaskEntry> blendMask);
	void applyBlendMask(Ogre::AnimationState* animState, std::vector<animationBlendMaskEntry> blendMask, float weight);
	void applyBlendMaskHierarchy(Ogre::AnimationState* animState, float weight, std::string parentBoneName = "root");
	void getBoneHandles(Ogre::Bone* in_bone, std::vector<unsigned short> &handles);

public:
	// Getters
	static Ogre::Bone* getBone(Ogre::Entity* ent, std::string boneName) throw(int);	// Static function that gets a Bone in a secure way
	Ogre::TagPoint* attachEntityToBone(std::string boneName, Ogre::Entity* ent, Ogre::Quaternion q, Ogre::Vector3 p) throw(int);
	void detachEntityFromBone(Ogre::Entity* ent) throw(int);

	Ogre::Vector3 getBoneWorldPosition(std::string boneName, std::string objectType = "character", Ogre::Entity* weaponEnt = 0, Ogre::Entity* attachmentEnt = 0, std::string mountPoint = "");
	Ogre::Quaternion getBoneWorldOrientation(std::string boneName, std::string objectType = "character", Ogre::Entity* attachmentEnt = 0);

	std::string getEntityName() throw(int);
	Ogre::Vector3 getNodePosition() throw(int);
	Ogre::AnimationStateIterator getAnimStateIter() throw(int);
	Ogre::TagPoint* getTagPoint(std::string name);
	
private:
	Ogre::AnimationState* getAnimState(std::string animName) throw(int);
public:
	float getAnimLength(std::string animName);										// Get Animation Length (Time)
	float getAnimWeight(std::string animName, std::string blendMaskName);			// Get Animation Weight
	bool getAnimLoop(std::string animName);											// Get Animation Loop

	float getAnimSpeed(std::string animName);
	bool getAnimTemporalPlayback(std::string animName);
	float getAnimManualTimePos(std::string animName);								// Get Animation Position (Time)

	// Setters
	void clearTagPoints();
	void setTagPoint(std::string name, Ogre::TagPoint* tagPoint);

	void setAnimSpeed(std::string animName, float speed);							// Set Animation speed
	void setAnimTemporalPlayback(std::string animName, bool temporalPlayback);		// Set playback type
	void setAnimManualTimePos(std::string animName, float newPos);					// Set Animation Position only, nothing is being "updated" yet

	// Variables
private:
	AnimationXML* animXML;

	Ogre::SceneNode* node;
	Ogre::Entity* ent;												// Easy access to Entity (Bones, Skeleton...)

	std::map<std::string, animStateAdvanced*> animStatesAdvanced;	// Additional Animation infos
	std::map<std::string, Ogre::TagPoint*> tagPoints;				// TagPoints (Bones with Objects attached)
};

#endif