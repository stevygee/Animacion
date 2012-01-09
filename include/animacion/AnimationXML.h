/*
	Animación!
	Copyright (c) 2011 Stefan Grassberger
	See license in Animacion.h
*/

#ifndef ANIMATIONXML_H
#define ANIMATIONXML_H

#include <string>
#include <iostream>

#include "Ogre.h"
//#include "../OgreFramework.h"
#include "tinyxml.h"

#include "AnimationBlendNodes.h"

class AnimationXML
{
	// Functions
public:
	AnimationXML(std::string fileName);
	~AnimationXML();

	void init();
	
	bool isLoaded();
	std::string getFileName();

	float getCharacterSpeed();
	Ogre::Vector3 getWeaponPosition();
	Ogre::Vector3 getWeaponOrientation();
	std::string getWeaponBone();

	std::vector<AnimationActionState*> getStates(std::string stateLayerName);	// Return list of all states from the stateLayer
	std::vector<AnimationStateLayer*> getStateLayers();
private:
	AnimationStateLayer* getStateLayer(std::string stateLayerName);
	TiXmlHandle AnimationXML::getStateBlendTree(std::string stateName, std::string stateLayerName);
	AnimationBlendNode* getBlendNode(TiXmlHandle parent, int childIndex);

	bool compareAttributes(TiXmlElement* attribute1Node, const char* attribute1, std::string attribute2);
	std::string getAttributeSafely(TiXmlElement* node, const char* attribName);

	// Variables
private:
	bool loaded;

	std::string fileName;
	TiXmlHandle docHandle;
};

#endif