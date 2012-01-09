/*
	Animación!
	Copyright (c) 2011 Stefan Grassberger
	See license in Animacion.h
*/

#include "AnimationXML.h"

AnimationXML::AnimationXML(std::string fileName) : docHandle(0)
{
	this->fileName = fileName;
}

AnimationXML::~AnimationXML()
{
}

void AnimationXML::init()
{
	std::cout << "Loading animation XML file \"" << this->fileName << "\"... ";

	this->loaded = false;

	// Build stateName
	std::stringstream ss;
	ss << "../../../../media/animation/" << this->fileName;
	std::string xmlPath = ss.str();	

	TiXmlDocument* doc = new TiXmlDocument( xmlPath.c_str()/*OgreFramework::GetFullPath( this->fileName )*/ );
	if( doc->LoadFile() ) {
		this->loaded = true;

		std::cout << "loaded" << std::endl;

		docHandle = TiXmlHandle(doc);
		docHandle = docHandle.FirstChild("darkRealm");
	} else {
		this->loaded = false;

		std::cout << "failed! file not found or isn't valid XML" << std::endl;
	}
}

bool AnimationXML::isLoaded()
{
	return this->loaded;
}

std::string AnimationXML::getFileName()
{
	return this->fileName;
}

float AnimationXML::getCharacterSpeed()
{
	TiXmlHandle rootNode = docHandle;

	// Process the "character" block
	TiXmlElement* characterNode = rootNode.FirstChild("character").ToElement();
	if(characterNode)
	{
		// Save info from character
		float speed = 1.0f;
		characterNode->QueryFloatAttribute("speed", &speed);

		return speed;
	}

	return 1.0f; // Default value
}

Ogre::Vector3 AnimationXML::getWeaponPosition()
{
	TiXmlHandle rootNode = docHandle;

	// Process the "attachedObject" block
	TiXmlElement* weaponNode = rootNode.FirstChild("character").FirstChild("attachedObject").FirstChild("position").ToElement();
	if(weaponNode)
	{
		// Save info from character
		float posX = 0.0f;
		float posY = 0.0f;
		float posZ = 0.0f;
		weaponNode->QueryFloatAttribute("x", &posX);
		weaponNode->QueryFloatAttribute("y", &posY);
		weaponNode->QueryFloatAttribute("z", &posZ);

		return Ogre::Vector3(posX, posY, posZ);
	}

	return Ogre::Vector3(0.f, 0.f, 0.f); // Default value
}

Ogre::Vector3 AnimationXML::getWeaponOrientation()
{
	TiXmlHandle rootNode = docHandle;

	// Process the "attachedObject" block
	TiXmlElement* weaponNode = rootNode.FirstChild("character").FirstChild("attachedObject").FirstChild("rotation").ToElement();
	if(weaponNode)
	{
		// Save info from character
		float rotX = 0.0f;
		float rotY = 0.0f;
		float rotZ = 0.0f;
		weaponNode->QueryFloatAttribute("x", &rotX);
		weaponNode->QueryFloatAttribute("y", &rotY);
		weaponNode->QueryFloatAttribute("z", &rotZ);

		return Ogre::Vector3(rotX, rotY, rotZ);
	}

	return Ogre::Vector3(0.f, 0.f, 0.f); // Default value
}

std::string AnimationXML::getWeaponBone()
{
	TiXmlHandle rootNode = docHandle;

	// Process the "attachedObject" block
	TiXmlElement* weaponNode = rootNode.FirstChild("character").FirstChild("attachedObject").ToElement();
	if(weaponNode)
	{
		// Save info from character
		std::string boneName		= getAttributeSafely(weaponNode, "bone");

		return boneName;
	}

	return ""; // Default value
}

std::vector<AnimationActionState*> AnimationXML::getStates(std::string stateLayerName)
{
	TiXmlHandle rootNode = docHandle;

	std::vector<AnimationTransition*> transitions;
	AnimationBlendTree* blndTree;
	std::vector<AnimationActionState*> actionStates;

	// Process the "stateLayer" block
	TiXmlElement* stateLayerNode = rootNode.FirstChild("stateLayer").ToElement();
	for(stateLayerNode; stateLayerNode; stateLayerNode = stateLayerNode->NextSiblingElement("stateLayer"))
	{
		// Is this the right state layer?
		if(compareAttributes(stateLayerNode, "name", stateLayerName))
		{
			rootNode = TiXmlHandle(stateLayerNode);

			// Process the "stateLayer" block

			// Process states
			TiXmlElement* stateNode = rootNode.FirstChild("state").ToElement();
			for(stateNode; stateNode; stateNode = stateNode->NextSiblingElement("state"))
			{
				// Save info from this state
				std::string stateName		= getAttributeSafely(stateNode, "name");

				TiXmlHandle stateHandle = TiXmlHandle(stateNode);

				// Process the "end" node
				TiXmlElement* endNode = stateHandle.FirstChild("end").ToElement();
				// Save info from this "end" node
				std::string targetState = (endNode) ? getAttributeSafely(endNode, "targetState") : "";

				// Process the "blendTree" block
				blndTree = new AnimationBlendTree(
								getBlendNode(stateHandle.FirstChild("blendTree"), 0)
							);

				// Process the "transitions" block
				TiXmlElement* transitionNode = stateHandle.FirstChild("transitions").FirstChild("transition").ToElement();
				for(transitionNode; transitionNode; transitionNode = transitionNode->NextSiblingElement("transition"))
				{
					// Save info from this transition
					std::string action		= getAttributeSafely(transitionNode, "action");
					std::string targetState	= getAttributeSafely(transitionNode, "targetState");
					std::string type		= getAttributeSafely(transitionNode, "type");
					float duration = 0.0f;
					transitionNode->QueryFloatAttribute("duration", &duration);

					transitions.push_back(new AnimationTransition(action, targetState, type, duration));
				}

				// Add this state to the list
				actionStates.push_back(
					new AnimationActionState(stateName, blndTree, transitions, targetState)
				);

				transitions.clear();
			}

			// Process transitionStates (shortcut for a predefined state)
			TiXmlElement* transitionStateNode = rootNode.FirstChild("transitionState").ToElement();
			for(transitionStateNode; transitionStateNode; transitionStateNode = transitionStateNode->NextSiblingElement("transitionState"))
			{
				// Save info from this transitionState
				std::string from				= getAttributeSafely(transitionStateNode, "from");
				std::string to					= getAttributeSafely(transitionStateNode, "to");

				TiXmlHandle transitionStateHandle = TiXmlHandle(transitionStateNode);

				// Build stateName
				std::stringstream ss;
				ss << from << "_to_" << to;
				std::string stateName = ss.str();

				// Build blendTree
				blndTree = new AnimationBlendTree(
					new AnimationBlendNodeLERP(
						getBlendNode( getStateBlendTree( from, stateLayerName ), 0 ),
						getBlendNode( getStateBlendTree( to, stateLayerName ), 0 ),
						0.0f,
						"transition_progress")
				);

				// Process the "transition" block
				TiXmlElement* transitionNode = transitionStateHandle.FirstChild("transition").ToElement();
				if(transitionNode)
				{
					// Save info from this transition
					std::string action		= getAttributeSafely(transitionNode, "action");
					std::string type		= getAttributeSafely(transitionNode, "type");
					float duration = 0.0f;
					transitionNode->QueryFloatAttribute("duration", &duration);

					// Build transition targetState
					std::stringstream ss2;
					ss2 << to << "_to_" << from;
					std::string inverseStateName = ss2.str();

					transitions.push_back(new AnimationTransition(action, inverseStateName, type, duration));
				}

				// Add this state to the list
				actionStates.push_back(
					new AnimationActionState(stateName, blndTree, transitions, to)
				);

				transitions.clear();
			}

			break;
		}
	}

	return actionStates;

	// TODO: Exception if empty
}

std::vector<AnimationStateLayer*> AnimationXML::getStateLayers()
{
	TiXmlHandle rootNode = docHandle;
	std::vector<AnimationStateLayer*> stateLayers;

	// Process the "stateLayer" block
	TiXmlElement* stateLayerNode = rootNode.FirstChild("stateLayer").ToElement();
	for(stateLayerNode; stateLayerNode; stateLayerNode = stateLayerNode->NextSiblingElement("stateLayer"))
	{
		// Save info from this "stateLayer" entry
		std::string name		= getAttributeSafely(stateLayerNode, "name");

		stateLayers.push_back( getStateLayer( name ) );
	}

	return stateLayers;
}

AnimationStateLayer* AnimationXML::getStateLayer(std::string stateLayerName)
{
	TiXmlHandle rootNode = docHandle;
	std::vector<animationBlendMaskEntry> blendMask;

	// Process the "stateLayer" block
	TiXmlElement* stateLayerNode = rootNode.FirstChild("stateLayer").ToElement();
	for(stateLayerNode; stateLayerNode; stateLayerNode = stateLayerNode->NextSiblingElement("stateLayer"))
	{
		// Is this the right state layer?
		if(compareAttributes(stateLayerNode, "name", stateLayerName))
		{
			rootNode = TiXmlHandle(stateLayerNode);

			// Save info from this "stateLayer" entry
			std::string name		= getAttributeSafely(stateLayerNode, "name");
			std::string type		= getAttributeSafely(stateLayerNode, "type");
			std::string startState	= getAttributeSafely(stateLayerNode, "startState");

			TiXmlElement* blendMaskNode = rootNode.FirstChild("blendMask").ToElement();

			rootNode = TiXmlHandle(blendMaskNode);

			// Process the "blendMask" block
			TiXmlElement* boneNode = rootNode.FirstChild("bone").ToElement();
			for(boneNode; boneNode; boneNode = boneNode->NextSiblingElement("bone"))
			{
				// Save info from this "bone" entry
				std::string name		= getAttributeSafely(boneNode, "name");
				float weight = 0.0f;
				boneNode->QueryFloatAttribute("weight", &weight);

				animationBlendMaskEntry entry;
				entry.name = name;
				entry.weight = weight;

				blendMask.push_back(entry);
			}

			return new AnimationStateLayer(name, type, startState, blendMask);

			break;
		}
	}

	return 0; // TODO: Exception
}

TiXmlHandle AnimationXML::getStateBlendTree(std::string stateName, std::string stateLayerName)
{
	TiXmlHandle rootNode = docHandle;

	TiXmlHandle blndTree = NULL;

	// Process the "stateLayer" block
	TiXmlElement* stateLayerNode = rootNode.FirstChild("stateLayer").ToElement();
	for(stateLayerNode; stateLayerNode; stateLayerNode = stateLayerNode->NextSiblingElement("stateLayer"))
	{
		// Is this the right state layer?
		if(compareAttributes(stateLayerNode, "name", stateLayerName))
		{
			rootNode = TiXmlHandle(stateLayerNode);

			// Process the "stateLayer" block
			TiXmlElement* stateNode = rootNode.FirstChild("state").ToElement();
			for(stateNode; stateNode; stateNode = stateNode->NextSiblingElement("state"))
			{
				// Is this the right state layer?
				if(compareAttributes(stateNode, "name", stateName))
				{
					// Save info from this state
					std::string stateName		= getAttributeSafely(stateNode, "name");

					rootNode = TiXmlHandle(stateNode);

					// Process the "blendTree" block
					blndTree = rootNode.FirstChild("blendTree");

					break;
				}
			}

			break;
		}
	}

	return blndTree;

	// TODO: Exception if empty
}

AnimationBlendNode* AnimationXML::getBlendNode(TiXmlHandle parent, int childIndex)
{
	// TODO: Clean up this mess

	if(parent.ToElement() && parent.ToElement()->NoChildren() == false)
	{
		int indexCounter = 0;

		TiXmlNode* child = 0;
		for(int i = 0; child = parent.ToElement()->IterateChildren(child); i++ )
		{
			TiXmlElement* blendNode = parent.Child(i).ToElement();
			if(!blendNode || blendNode->Type() != TiXmlNode::TINYXML_ELEMENT)
			{
				// ignore comments etc.
			}
			else
			{
				// Save info from this blendNode
				std::string blendNodeName = blendNode->Value();
				if(blendNodeName.compare("clip") == 0)
				{
					// Animation clip
					std::string name				= getAttributeSafely(blendNode, "name");
					std::string loop				= getAttributeSafely(blendNode, "loop");
					std::string temporalPlayback	= getAttributeSafely(blendNode, "temporalPlayback");
					std::string flags				= getAttributeSafely(blendNode, "flags");
					float speed = 0.0f;
					blendNode->QueryFloatAttribute("speed", &speed);

					if(indexCounter == childIndex)
					{
						//std::cout << " | clip: " << name << " . " << flags << std::endl;
						return new AnimationBlendNodeClip(name, speed, temporalPlayback, loop, flags);
					}
					indexCounter++;
				}
				else if(blendNodeName.compare("lerp") == 0)
				{
					if(blendNode->NoChildren() == false)
					{
						if(indexCounter == childIndex)
						{
							//std::cout << " | lerp" << std::endl;

							// LERP Node
							std::string flags	= getAttributeSafely(blendNode, "flags");
							float blendFactor = 0.0f;
							blendNode->QueryFloatAttribute("blendFactor", &blendFactor);

							return new AnimationBlendNodeLERP(
											getBlendNode(parent.Child(i), 0),
											getBlendNode(parent.Child(i), 1),
											blendFactor,
											flags
										);
						}
						indexCounter++;
					}
				}
				else if(blendNodeName.compare("additive") == 0)
				{
					if(blendNode->NoChildren() == false)
					{
						if(indexCounter == childIndex)
						{
							//std::cout << " | additive" << std::endl;

							// Additive Node
							return new AnimationBlendNodeAdditive(
											getBlendNode(parent.Child(i), 0),
											getBlendNode(parent.Child(i), 1)
										);
						}
						indexCounter++;
					}
				}
				else if(blendNodeName.compare("shortcut") == 0)
				{
					if(indexCounter == childIndex)
					{
						//std::cout << " | shortcut: " << name << std::endl;

						// Shortcut Node
						std::string stateName				= getAttributeSafely(blendNode, "state");
						std::string stateLayerName			= getAttributeSafely(blendNode, "stateLayer");

						TiXmlHandle blendTreeNode = getStateBlendTree( stateName, stateLayerName );

						return getBlendNode( blendTreeNode, 0 );
					}
					indexCounter++;
				}
			}
		}
	}

	// TODO: Exception
}

bool AnimationXML::compareAttributes(TiXmlElement* attribute1Node, const char* attribute1, std::string attribute2)
{
	std::string attribute1XML = getAttributeSafely(attribute1Node, attribute1);
	return attribute2.compare(attribute1XML) == 0;
}

std::string AnimationXML::getAttributeSafely(TiXmlElement* node, const char* attribName)
{
	std::string output = node && node->Attribute(attribName) ? node->Attribute(attribName) : "";
	return output;
}