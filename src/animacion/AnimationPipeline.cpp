/*
	Animación!
	Copyright (c) 2011 Stefan Grassberger
	See license in Animacion.h
*/

// Features:
// - Is stateless! (no info on transitions kept here, just executes statements)
// - One pipeline exists per character
// - All its animationStates are managed here

// TODO:
// - Collect animation attributes from all animation layers and create a single blend tree from those layers
// - Rethink and/or move old functions that were copy-pasted in here

#include "AnimationPipeline.h"

AnimationPipeline::AnimationPipeline(AnimationXML* animXML, Ogre::SceneNode* node, Ogre::Entity *ent)
{
	this->animXML = animXML;

	this->node = node;
	this->ent = ent;
}

AnimationPipeline::~AnimationPipeline()
{
}

void AnimationPipeline::init(bool firstInit)
{
	if(firstInit)
		this->ent->getSkeleton()->setBlendMode(Ogre::ANIMBLEND_CUMULATIVE);		// Set blend mode for whole skeleton

	// Set initial animationState parameters

	Ogre::AnimationStateSet *set = this->ent->getAllAnimationStates();
	Ogre::AnimationStateIterator it = set->getAnimationStateIterator();

	while(it.hasMoreElements())
	{
		Ogre::AnimationState* anim = it.getNext();
		std::string animName = anim->getAnimationName();

		// Basic parameters
		anim->setEnabled(true);
		anim->setWeight(1);
		if(firstInit) anim->setTimePosition(0);

		// Advanced parameters
		if(firstInit)
		{
			this->animStatesAdvanced[animName] = new animStateAdvanced();
			setAnimSpeed( animName, 1.0f );				// All animations run at normal speed unless this is changed later
			setAnimTemporalPlayback( animName, true );	// Normal temporal playback vs. setting time position manually
			setAnimManualTimePos( animName, 0.0f );		// Default manual time position

			// TODO: Create duplicate of real blend mask as a fake blend mask
		}

		// Initialize all blend weights to zeros
		if( !anim->hasBlendMask() )
		{
			anim->createBlendMask( this->ent->getSkeleton()->getNumBones(), 0.0f );
		}
		else
		{
			applyBlendMaskHierarchy( anim, 0.0f );
		}
	}
}

void AnimationPipeline::update(std::vector<AnimationActionState*> activeStates, Ogre::Real secSinceLastFrame)
{
	// Apply each active blend tree from the respective state machines
	for(unsigned int i = 0; i < activeStates.size(); i++)
	{
		// TODO: Don't access XML every single time in order to get blend mask
		// TODO: Actually combine the blend trees, taking in account if layer is LERP or additive
		blend(
			activeStates.at(i)->getBlendTree(),
			this->animXML->getStateLayer( activeStates.at(i)->getStateLayerName() )->getBlendMask(),
			activeStates.at(i)->getActiveFlags()
		);
	}

	// Update all animationStates (time position)
	Ogre::AnimationStateSet *set = this->ent->getAllAnimationStates();
	Ogre::AnimationStateIterator it = set->getAnimationStateIterator();

	while(it.hasMoreElements())
	{
		updateAnimState( it.getNext(), secSinceLastFrame );
	}
}

void AnimationPipeline::blend(AnimationBlendTree* tree, std::vector<animationBlendMaskEntry> blendMask, std::map<std::string, float> flags)
{
	// Clamp values
	for(std::map<std::string, float>::iterator i = flags.begin(); i != flags.end();i++)
	{
		if( (*i).first.compare("transition_progress") == 0 ||
			(*i).first.compare("walk_weight") == 0 ||
			(*i).first.compare("idle_weight") == 0 )
		{
			(*i).second = std::min<float>( (*i).second, 1.0f );
			(*i).second = std::max<float>( (*i).second, 0.0f );
		}
	}

	applyBlendNode( tree->getChild(), blendMask, flags );
}

void AnimationPipeline::applyBlendNode(AnimationBlendNode* bn, std::vector<animationBlendMaskEntry> blendMask, std::map<std::string, float> flags, float weight)
{
	if( !bn ) return;

	AnimationBlendNodeClip* clip;
	AnimationBlendNodeLERP* lerp;
	AnimationBlendNodeAdditive* additive;

	switch( bn->getType() )
	{
		case BLEND_NODE_CLIP:
			// This really is a clip node
			clip = static_cast<AnimationBlendNodeClip*>(bn);

			applyClip( clip, blendMask, flags, weight );

			break;
		case BLEND_NODE_LERP:
			// This really is a LERP node
			lerp = static_cast<AnimationBlendNodeLERP*>(bn);

			applyLERP( lerp, blendMask, flags, weight );

			break;
		case BLEND_NODE_ADDITIVE:
			// This really is a Additive node
			additive = static_cast<AnimationBlendNodeAdditive*>(bn);

			applyAdditive( additive, blendMask, flags, weight );

			break;
	}
}

void AnimationPipeline::applyClip(AnimationBlendNodeClip* clip, std::vector<animationBlendMaskEntry> blendMask, std::map<std::string, float> flags, float weight)
{
	if(clip->getType() == BLEND_NODE_CLIP)
	{
		// Get AnimationState for this animation
		Ogre::AnimationState* animState = getAnimState( clip->getName() );

		// Set blend mask
		applyBlendMask( animState, blendMask, weight ); // TODO: Accumulate the right way

		// Set loop
		animState->setLoop( clip->getLoop() ); // TODO: reset to start at some point?

		// Set advanced clip attributes
		float speed = ( clip->getFlags().compare("character_speed") == 0 && flags.count("character_speed") == 1 ) ?
						flags["character_speed"] / (clip->getSpeed() + 0.001f) :
						clip->getSpeed();
		
		setAnimSpeed( clip->getName(), speed );
		setAnimTemporalPlayback( clip->getName(), clip->getTemporalPlayback() );
	}
}

void AnimationPipeline::applyLERP(AnimationBlendNodeLERP* lerp, std::vector<animationBlendMaskEntry> blendMask, std::map<std::string, float> flags, float weight)
{
	if(lerp->getType() == BLEND_NODE_LERP)
	{
		AnimationBlendNode* child1 = lerp->getFirstChild();
		AnimationBlendNode* child2 = lerp->getSecondChild();

		// Set weights
		// If the LERP has a "transition_progress" or similar flag set, use this special value as blend factor
		float weight2 = ( !lerp->getFlags().empty() && flags.count( lerp->getFlags() ) == 1 ) ?
						flags[lerp->getFlags()] :
						lerp->getBlendFactor();
		float weight1 = 1.0f - weight2;

		weight1 = weight1 * weight;
		weight2 = weight2 * weight;

		applyBlendNode(child1, blendMask, flags, weight1);
		applyBlendNode(child2, blendMask, flags, weight2);
	}
}

void AnimationPipeline::applyAdditive(AnimationBlendNodeAdditive* additive, std::vector<animationBlendMaskEntry> blendMask, std::map<std::string, float> flags, float weight)
{
	if(additive->getType() == BLEND_NODE_ADDITIVE)
	{
		AnimationBlendNode* child1 = additive->getFirstChild();
		AnimationBlendNode* child2 = additive->getSecondChild();

		// Weights are both the same because we use additive blending here

		applyBlendNode(child1, blendMask, flags, weight);
		applyBlendNode(child2, blendMask, flags, weight);
	}
}

void AnimationPipeline::updateAnimState(Ogre::AnimationState *animState, Ogre::Real secSinceLastFrame)
{
	if( !animState->getEnabled() ) return;

	if(getAnimTemporalPlayback(animState->getAnimationName()))
		// Negative speed is also supported (animation then goes backwards)
		animState->addTime( secSinceLastFrame * getAnimSpeed( animState->getAnimationName() ) );
	else
		animState->setTimePosition( getAnimManualTimePos( animState->getAnimationName() ) );
}

std::vector<animationBlendMaskEntry> AnimationPipeline::generateBlendMask(std::vector<animationBlendMaskEntry> blendMask)
{
	// TODO: Optimize more

	// Generate a real blend mask from XML values
	std::vector<animationBlendMaskEntry> blendMaskFull;
	float parentWeight = blendMask.at(0).weight; // TODO: Real Root

	Ogre::Skeleton::BoneIterator it = this->ent->getSkeleton()->getBoneIterator();
	while(it.hasMoreElements())
	{
		Ogre::Bone* bone = it.getNext();

		// Get parent value
		if(blendMaskFull.size() > 0/*bone->getParent() != NULL*/)
		{
			for(unsigned int i = 0; i < blendMaskFull.size(); i++)
			{
				if( blendMaskFull.at(i).name.compare( bone->getParent()->getName() ) == 0 )
				{
					parentWeight = blendMaskFull.at(i).weight;
					break;
				}
			}
		}
		float newWeight = parentWeight; // Default = Parent

		// Get current value
		for(unsigned int i = 0; i < blendMask.size(); i++)
		{
			if( blendMask.at(i).name.compare( bone->getName() ) == 0 )
			{
				newWeight = blendMask.at(i).weight; // If found = take value
				break;
			}
		}

		// Set
		animationBlendMaskEntry entry;
		entry.name = bone->getName();
		entry.weight = newWeight; // Parent or defined value

		blendMaskFull.push_back(entry);
	}

	return blendMaskFull;
}

void AnimationPipeline::applyBlendMask(Ogre::AnimationState* animState, std::vector<animationBlendMaskEntry> blendMask, float weight)
{
	if(blendMask.size() > 0)
	{
		// Generate real blend mask
		std::vector<animationBlendMaskEntry> blendMaskFull = generateBlendMask(blendMask);

		// Apply real blend mask
		Ogre::AnimationState::BoneBlendMask* bm = const_cast<Ogre::AnimationState::BoneBlendMask*>( animState->getBlendMask() );

		for(unsigned int i = 0; i < blendMaskFull.size(); i++)
		{
			unsigned short handle = getBone( this->ent, blendMaskFull.at(i).name )->getHandle();
			float newWeight = animState->getBlendMaskEntry( handle ) + ( blendMaskFull.at(i).weight * weight );

			animState->setBlendMaskEntry( handle, newWeight );
			(*bm)[handle] = newWeight;
		}
	}
	else
	{
		// No blend mask defined in XML - Use single value for all bone weights
		// TODO: Use setWeight instead? Performance?
		unsigned short handle = getBone( this->ent, "root" )->getHandle();
		float newWeight = animState->getBlendMaskEntry( handle ) + weight;

		applyBlendMaskHierarchy( animState, newWeight );
	}

	animState->getParent()->_notifyDirty();
}

void AnimationPipeline::applyBlendMaskHierarchy(Ogre::AnimationState* animState, float weight, std::string parentBoneName)
{
	std::vector<unsigned short> handles;
	Ogre::Bone* parentBone = this->getBone( this->ent, parentBoneName );
	this->getBoneHandles( parentBone, handles );

	// Apply real blend mask
	Ogre::AnimationState::BoneBlendMask* bm = const_cast<Ogre::AnimationState::BoneBlendMask*>(animState->getBlendMask());

	for(unsigned int i = 0; i < handles.size(); i++)
	{
		animState->setBlendMaskEntry( handles[i], weight );
		(*bm)[handles[i]] = weight;
	}
}

void AnimationPipeline::getBoneHandles(Ogre::Bone* in_bone, std::vector<unsigned short> &handles)
{
	handles.push_back( in_bone->getHandle() );
	
	Ogre::Node::ChildNodeIterator iter = in_bone->getChildIterator();
	while(iter.hasMoreElements())
	{
		getBoneHandles( dynamic_cast<Ogre::Bone*>( iter.getNext() ), handles );         
	}
}

Ogre::Bone* AnimationPipeline::getBone(Ogre::Entity* ent, std::string boneName) throw(int)
{
	if( ent && ent->hasSkeleton() )
	{
		Ogre::Skeleton* skeleton = ent->getSkeleton();

		// Get Bone from Skeleton
		if( skeleton->hasBone(boneName) )
		{
			return skeleton->getBone(boneName);
		}
		else
		{
			// If invalid, use the root bone
			Ogre::Skeleton::BoneIterator it = skeleton->getRootBoneIterator();
			if( it.hasMoreElements() ) return it.getNext();
		}
	}

	throw(1); // No skeleton found
}

Ogre::TagPoint* AnimationPipeline::attachEntityToBone(std::string boneName, Ogre::Entity* ent, Ogre::Quaternion q, Ogre::Vector3 p) throw(int)
{
	if( this->ent && this->ent->hasSkeleton() && this->ent->getSkeleton()->hasBone(boneName) )
		return this->ent->attachObjectToBone(boneName, ent, q, p);
	else
		throw(1); // No skeleton found
}

void AnimationPipeline::detachEntityFromBone(Ogre::Entity* ent) throw(int)
{
	if( this->ent && this->ent->hasSkeleton() )
		this->ent->detachObjectFromBone( ent );
	else
		throw(1); // No skeleton found
}

Ogre::Vector3 AnimationPipeline::getBoneWorldPosition(std::string boneName, std::string objectType, Ogre::Entity* weaponEnt, Ogre::Entity* attachmentEnt, std::string mountPoint)
{
	// TODO: Use something other than getFullTransform (read function comment)

	if( !this->node || !this->ent ) return Ogre::Vector3(0.f, 0.f, 0.f);	// Just in case...
	Ogre::Vector3 worldPos = this->node->getPosition();						// Default value

	if( objectType.compare("character") == 0 &&
		this->ent->getSkeleton()->hasBone( boneName ) )				// Bone on character
	{
		Ogre::Bone* bone = getBone( this->ent, boneName );

		worldPos = this->node->_getFullTransform() * bone->_getDerivedPosition();
	}
	else if( objectType.compare("weapon") == 0 &&
			 weaponEnt &&
			 weaponEnt->getSkeleton()->hasBone( boneName ) )		// Bone on weapon on character
	{
		Ogre::TagPoint* tp;
		Ogre::Bone* bone;

		try
		{
			tp = getTagPoint( "weapon_on_character" );
			bone = getBone( weaponEnt, boneName );
		}
		catch(...)
		{
			return worldPos;
		}

		if( tp )
			worldPos = this->node->_getFullTransform() * tp->_getFullLocalTransform() * bone->_getDerivedPosition();
	}
	else if( objectType.compare("attachment") == 0 &&
			 weaponEnt &&
			 attachmentEnt &&
			 attachmentEnt->getSkeleton()->hasBone( boneName ) &&
			 !mountPoint.empty() )									// Bone on attachment on weapon on character
	{
		Ogre::TagPoint* tp;
		Ogre::TagPoint* tp2;
		Ogre::Bone* bone;

		try
		{
			tp = getTagPoint( "weapon_on_character" );
			tp2 = getTagPoint( mountPoint );
			bone = getBone( attachmentEnt, boneName );
		}
		catch(...)
		{
			return worldPos;
		}

		if( tp && tp2 )
			worldPos = this->node->_getFullTransform() * tp->_getFullLocalTransform() * tp2->_getFullLocalTransform() * bone->_getDerivedPosition();
	}

	return worldPos;
}

Ogre::Quaternion AnimationPipeline::getBoneWorldOrientation(std::string boneName, std::string objectType, Ogre::Entity* attachmentEnt)
{
	// TODO: Other objectTypes... Testing... etc.

	if( !this->node || !this->ent ) return Ogre::Quaternion();		// Just in case...
	Ogre::Quaternion worldOrient = this->node->getOrientation();	// Default value
	Ogre::Bone* bone;

	if( objectType.compare("attachment") == 0 &&
			 attachmentEnt &&
			 getBone( attachmentEnt, boneName ) )	// Bone on attachment on weapon on character
	{
		Ogre::TagPoint* tp;

		try
		{
			tp = getTagPoint( "weapon_on_character" );
			bone = getBone( attachmentEnt, boneName );
		}
		catch(...)
		{
			return worldOrient;
		}

		worldOrient = tp->_getDerivedOrientation() * bone->_getDerivedOrientation();
	} else {										// Bone on character
		try
		{
			bone = getBone( this->ent, boneName );
		}
		catch(...)
		{
			return worldOrient;
		}

		worldOrient = bone->_getDerivedOrientation();
	}

	return worldOrient;
}

std::string AnimationPipeline::getEntityName() throw(int)
{
	if( this->ent )
		return this->ent->getName();
	else
		throw(1); // Null pointer
}

Ogre::Vector3 AnimationPipeline::getNodePosition() throw(int)
{
	if( this->ent )
		return this->ent->getParentNode()->getPosition();
	else
		throw(1); // Null pointer
}

Ogre::AnimationStateIterator AnimationPipeline::getAnimStateIter() throw(int)
{
	if( !this->ent ) throw(1); // Null pointer

	Ogre::AnimationStateSet *set = this->ent->getAllAnimationStates();
	Ogre::AnimationStateIterator it = set->getAnimationStateIterator();

	return it;
}

Ogre::TagPoint* AnimationPipeline::getTagPoint(std::string name)
{
	if( !this->tagPoints[name] ) throw(1); // Null pointer
	return this->tagPoints[name];
}

Ogre::AnimationState* AnimationPipeline::getAnimState(std::string animName) throw(int)
{
	if( this->ent )
	{
		Ogre::AnimationState* animState = this->ent->getAnimationState( animName );
		if( animState )	return animState;
	}

	throw(1); // Animation State not found
}

float AnimationPipeline::getAnimLength(std::string animName)
{
	return getAnimState(animName)->getLength();
}

float AnimationPipeline::getAnimWeight(std::string animName, std::string blendMaskName)
{
	// TODO: Make more dynamic

	if( blendMaskName.compare("lowerbody") == 0 )
	{
		return getAnimState(animName)->getBlendMaskEntry( this->getBone(this->ent, "leftFoot")->getHandle() );
	}
	else if( blendMaskName.compare("upperbody") == 0 )
	{
		return getAnimState(animName)->getBlendMaskEntry( this->getBone(this->ent, "neck")->getHandle() );
	}
	else
	{
		return 0.0f;
		//return getAnimState(animName)->getWeight();
	}
}

bool AnimationPipeline::getAnimLoop(std::string animName)
{
	return getAnimState(animName)->getLoop();
}

float AnimationPipeline::getAnimSpeed(std::string animName)
{
	return animStatesAdvanced[animName]->speed;
}

bool AnimationPipeline::getAnimTemporalPlayback(std::string animName)
{
	return animStatesAdvanced[animName]->temporalPlayback;
}

float AnimationPipeline::getAnimManualTimePos(std::string animName)
{
	return animStatesAdvanced[animName]->manualTimePos;
}

void AnimationPipeline::clearTagPoints()
{
	this->tagPoints.clear();
}

void AnimationPipeline::setTagPoint(std::string name, Ogre::TagPoint* tagPoint)
{
	this->tagPoints[name] = tagPoint;
}

void AnimationPipeline::setAnimSpeed(std::string animName, float speed)
{
	animStatesAdvanced[animName]->speed = speed;
}

void AnimationPipeline::setAnimTemporalPlayback(std::string animName, bool temporalPlayback)
{
	animStatesAdvanced[animName]->temporalPlayback = temporalPlayback;
}

void AnimationPipeline::setAnimManualTimePos(std::string animName, float newPos)
{
	// Clamp values
	newPos = std::min(newPos, getAnimLength(animName));
	newPos = std::max(newPos, 0.0f);

	animStatesAdvanced[animName]->manualTimePos = newPos;
}