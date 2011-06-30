#include <OgreLight.h>
#include <OgreWindowEventUtilities.h>

#include "AnimationDemo.h"

AnimationDemo::AnimationDemo()
{
	this->playerInputVector = Ogre::Vector3(Ogre::Vector3::ZERO);
	this->aimAngleH = Ogre::Radian(Ogre::Degree(180.f));
	this->aimAngleV = Ogre::Radian(Ogre::Degree(90.f));
}

AnimationDemo::~AnimationDemo()
{
	delete OgreFramework::getSingletonPtr();
}

void AnimationDemo::startDemo()
{
	new OgreFramework();
	if(!OgreFramework::getSingletonPtr()->initOgre("AnimationDemo", this, 0))
		return;
	
	m_bShutdown = false;

	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Demo initialized!");

	setupDemoScene();
	runDemo();
}

void AnimationDemo::setupDemoScene()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Init main loop...");

	this->loaded = false;

	// Init game systems
	this->gameAnimation = new GameAnimation();
	this->gameAnimation->init();
	// ... other systems ...

	// Setup scene
    OgreFramework::getSingletonPtr()->m_pSceneMgr->setAmbientLight(Ogre::ColourValue(0.5f, 0.5f, 0.5f));
	OgreFramework::getSingletonPtr()->m_pSceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
	
	Ogre::Light* directionalLight = OgreFramework::getSingletonPtr()->m_pSceneMgr->createLight("directionalLight");
    directionalLight->setType(Ogre::Light::LT_DIRECTIONAL);
    directionalLight->setDiffuseColour(Ogre::ColourValue(.5, .5, .5));
    directionalLight->setSpecularColour(Ogre::ColourValue(.5, .5, .5));
    directionalLight->setDirection(Ogre::Vector3( 0, -1, 1 )); 

	// Init characters
	this->playerChar = new PlayerCharacter();
	this->playerChar->init("eva.mesh", this->gameAnimation);
	this->playerChar->ent->setCastShadows(true);

	// Setup level
	Ogre::Plane plane(Ogre::Vector3::UNIT_Y, 0);
	Ogre::MeshManager::getSingleton().createPlane("ground", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
        plane, 3500, 3500, 20, 20, true, 1, 5, 5, Ogre::Vector3::UNIT_Z);
	Ogre::Entity* entGround = OgreFramework::getSingletonPtr()->m_pSceneMgr->createEntity("GroundEntity", "ground");
    OgreFramework::getSingletonPtr()->m_pSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(entGround);
	entGround->setMaterialName("Examples/Rockwall");
    entGround->setCastShadows(false);

	// Init AI characters (temp)
	Ogre::Entity* zombieEnt1 = OgreFramework::getSingletonPtr()->m_pSceneMgr->createEntity("Zombie1", "malezombie.mesh");
	zombieEnt1->setCastShadows(true);
    Ogre::SceneNode* zombieNode1 = OgreFramework::getSingletonPtr()->m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
    zombieNode1->attachObject(zombieEnt1);
	zombieNode1->setPosition(-200.f, 0.f, 270.f);

	Ogre::Entity* zombieEnt2 = OgreFramework::getSingletonPtr()->m_pSceneMgr->createEntity("Zombie2", "malezombie.mesh");
	zombieEnt2->setCastShadows(true);
    Ogre::SceneNode* zombieNode2 = OgreFramework::getSingletonPtr()->m_pSceneMgr->getRootSceneNode()->createChildSceneNode();
    zombieNode2->attachObject(zombieEnt2);
	zombieNode2->setPosition(250.f, 0.f, -300.f);

	this->loaded = true;
}

void AnimationDemo::runDemo()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Start main loop...");
	
	unsigned long timeSinceLastFrame = 1;
	unsigned long startTime = 0;
	unsigned long timeTakenAnimation = 1;

	OgreFramework::getSingletonPtr()->m_pRenderWnd->resetStatistics();
	
	while(!m_bShutdown && !OgreFramework::getSingletonPtr()->isOgreToBeShutDown()) 
	{
		if(OgreFramework::getSingletonPtr()->m_pRenderWnd->isClosed())m_bShutdown = true;

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_LINUX
		Ogre::WindowEventUtilities::messagePump();
#endif	
		if(OgreFramework::getSingletonPtr()->m_pRenderWnd->isActive())
		{
			startTime = OgreFramework::getSingletonPtr()->m_pTimer->getMillisecondsCPU();
			
			OgreFramework::getSingletonPtr()->m_pKeyboard->capture();
			OgreFramework::getSingletonPtr()->m_pMouse->capture();

			// Update game loop
			processInput();

			this->playerChar->updateMovement(timeSinceLastFrame, playerInputVector, &(OgreFramework::getSingletonPtr()->cameraHorizAngle));
			this->playerChar->updateCharDir(timeSinceLastFrame, this->aimAngleH, this->aimAngleV, &playerInputVector, OgreFramework::getSingletonPtr()->cameraHorizAngle);
			
			playerInputVector = playerInputVector / 3.f;
			timeTakenAnimation = this->gameAnimation->update(timeSinceLastFrame);

			// GUI
			if (OgreFramework::getSingletonPtr()->m_pDetailsPanel->isVisible())   // if details panel is visible, then update its contents
			{
				OgreFramework::getSingletonPtr()->m_pDetailsPanel->setParamValue(0, Ogre::StringConverter::toString( timeTakenAnimation ));
				OgreFramework::getSingletonPtr()->m_pDetailsPanel->setParamValue(1, Ogre::StringConverter::toString( timeSinceLastFrame ));
				OgreFramework::getSingletonPtr()->m_pDetailsPanel->setParamValue(2, Ogre::StringConverter::toString( this->playerChar->curMoveSpeed ));
				OgreFramework::getSingletonPtr()->m_pDetailsPanel->setParamValue(3, Ogre::StringConverter::toString( this->playerChar->node->getPosition() ));
				OgreFramework::getSingletonPtr()->m_pDetailsPanel->setParamValue(4, Ogre::StringConverter::toString( this->aimAngleH.valueDegrees() ));
				OgreFramework::getSingletonPtr()->m_pDetailsPanel->setParamValue(5, Ogre::StringConverter::toString( this->aimAngleV.valueDegrees() ));
			}

			OgreFramework::getSingletonPtr()->updateOgre(timeSinceLastFrame);
			OgreFramework::getSingletonPtr()->m_pRoot->renderOneFrame();
		
			timeSinceLastFrame = OgreFramework::getSingletonPtr()->m_pTimer->getMillisecondsCPU() - startTime;
		}
		else
		{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
		}
	}

	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Main loop quit");
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Shutdown OGRE...");
}

bool AnimationDemo::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);

	if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_V))
	{
		// TODO: Cycle characters
	}
	else if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_C))
		this->gameAnimation->animGUI->toggleVisible();
	else if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_X))
		this->gameAnimation->reloadXML();

	return true;
}

void AnimationDemo::processInput()
{
	// Player navigation
	if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_S))
        playerInputVector.x = -1;
    if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_W))
        playerInputVector.x = 1;
    if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_A))
        playerInputVector.z = -1;
    if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_D))
        playerInputVector.z = 1;

	// Temp aiming
	if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_4))
	{
		aimAngleH += Ogre::Radian(Ogre::Degree(3.f));
		aimAngleH = (aimAngleH <= Ogre::Radian(Ogre::Degree(360.f))) ? aimAngleH : aimAngleH - Ogre::Radian(Ogre::Degree(360.f));
	}
	else if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_5))
	{
		aimAngleH -= Ogre::Radian(Ogre::Degree(3.f));
		aimAngleH = (aimAngleH >= Ogre::Radian(Ogre::Degree(0.f))) ? aimAngleH : aimAngleH + Ogre::Radian(Ogre::Degree(360.f));
	}
	if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_6))
	{
		aimAngleV += Ogre::Radian(Ogre::Degree(3.f));
		aimAngleV = Ogre::Radian(Ogre::Degree(std::min<float>(aimAngleV.valueDegrees(), 135.f)));
	}
	else if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_7))
	{
		aimAngleV -= Ogre::Radian(Ogre::Degree(3.f));
		aimAngleV = Ogre::Radian(Ogre::Degree(std::max<float>(aimAngleV.valueDegrees(), 45.f)));
	}

	 // TODO: setNextAction() only has one action and ignores these...

	if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_1))
		this->playerChar->animation->setNextAction("aim");		// Switch to "aiming" animation
	else if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_2))
	{
		// Switch animation
		std::string nextAction = this->playerChar->animation->isStateActive( "running", "lowerbody" ) ? "drop_weapon_run" : "drop_weapon_idle";
		this->playerChar->animation->setNextAction( nextAction );
	}
	else if (OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_3))
		this->playerChar->animation->setNextAction("reload");	// Switch to "reloading" animation
}

bool AnimationDemo::keyReleased(const OIS::KeyEvent &keyEventRef)
{
	OgreFramework::getSingletonPtr()->keyReleased(keyEventRef);
	
	return true;
}