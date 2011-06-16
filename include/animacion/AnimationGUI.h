/*
	Animación!
	Copyright (c) 2011 Stefan Grassberger
	See license in Animacion.h
*/

#ifndef ANIMATIONGUI_H
#define ANIMATIONGUI_H

#include <string>
#include <iostream>
#include <deque>

#include "Ogre.h"

#include "../ObjectTextDisplay.h"
#include "../GameRenderer.h"

#include "AnimationController.h"

class AnimationGraph
{
	// Functions
public:
	AnimationGraph(GameRenderer* gameRenderer, Ogre::Overlay* m_pOverlay, std::string name, float posX, float posY, float txtPosX, float txtPosY, float width = 0.3f, float height = 0.2f);
	~AnimationGraph();

	void init();
	void update(unsigned long timeSinceLastFrame);

	void setNewValue(std::string name, float newValue);
	void setVisible(bool visible);

private:
	Ogre::ManualObject* createManualObject(std::string name);
	void updateLine(Ogre::ManualObject* manual, std::deque<float>* values, Ogre::ColourValue color);

	// Variables
private:
	GameRenderer* gameRenderer;

	std::string name;
	Ogre::ManualObject* manual_first;
	Ogre::ManualObject* manual_second;

	Ogre::Overlay* m_pOverlay;
	ObjectTextDisplay* text;

	bool visible;

	std::deque<float> values_first;
	std::deque<float> values_second;
	float nextValue_first;
	float nextValue_second;
	Ogre::ColourValue color_first;
	Ogre::ColourValue color_second;

	float timeElapsed;	// time since last sample
	float sampleTime;	// time between samples
	int samples;		// no. of points on the line (resolution)

	float width;		// width * Screenwidth = Width in pixels
	float height;		// see above
	float posX;			// 0.0f is left side of the screen
	float posY;			// 0.0f is top side of the screen
	float txtPosX;
	float txtPosY;
};

//----------------------------------------------------------------------------//

class AnimationGUI
{
	// Functions
public:
	AnimationGUI(GameRenderer* gameRenderer);
	~AnimationGUI();

	void init(AnimationController* animPipeline);
	void initChar(AnimationController* animPipeline);
	void initDirectionVectors();

	void update(unsigned long timeSinceLastFrame);
	void updateDirectionVectors();

	static std::string removeSpaces( std::string stringIn );
	void clearGraphs();

	void toggleVisible();
	bool isInitialized();

	std::string getEntityName();

	// Variables
private:
	AnimationController* animController;
	std::string entityName;

	GameRenderer* gameRenderer;
	Ogre::Overlay* m_pOverlay;

	bool visible;
	bool initialized;

	std::vector<AnimationGraph*> graphs;
	ObjectTextDisplay* text;

	std::map<std::string, Ogre::ManualObject*> directionVectors;		// Direction vectors for debugging
};

#endif