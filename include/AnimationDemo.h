//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef OGRE_DEMO_HPP
#define OGRE_DEMO_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "OgreFramework.h"
#include "GameRenderer.h"
#include "GameAnimation.h"
#include "PlayerCharacter.h"

//|||||||||||||||||||||||||||||||||||||||||||||||

class AnimationDemo : public OIS::KeyListener
{
public:
	AnimationDemo();
	~AnimationDemo();

	void startDemo();
	
	void processInput();
	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

private:
	void setupDemoScene();
	void runDemo();

	bool						m_bShutdown;

	//Variables
	bool loaded;
private:
	GameRenderer* gameRenderer;
	GameAnimation* gameAnimation;
public:
	PlayerCharacter* playerChar;
	Ogre::Vector3				playerInputVector;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif 

//|||||||||||||||||||||||||||||||||||||||||||||||