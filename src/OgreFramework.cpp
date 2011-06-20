#ifndef LAUNCH_WITH_CONFIG_DIALOG
#define LAUNCH_WITH_CONFIG_DIALOG 1
#endif

#include "OgreFramework.h"

using namespace Ogre;

template<> OgreFramework* Ogre::Singleton<OgreFramework>::ms_Singleton = 0;

OgreFramework::OgreFramework()
{
    m_MoveSpeed			= 1.f;
    m_RotateSpeed		= 0.3f;

    m_bShutDownOgre		= false;
    m_iNumScreenShots	= 0;

    m_pRoot				= 0;
    m_pSceneMgr			= 0;
    m_pRenderWnd		= 0;
    m_pCamera			= 0;
    m_pViewport			= 0;
    m_pLog				= 0;
    m_pTimer			= 0;

    m_pInputMgr			= 0;
    m_pKeyboard			= 0;
    m_pMouse			= 0;

    m_pTrayMgr          = 0;
	m_pDetailsPanel		= 0;
    m_FrameEvent        = Ogre::FrameEvent();

	cameraHorizAngle	= 0;
}

OgreFramework::~OgreFramework()
{
    if(m_pInputMgr)		OIS::InputManager::destroyInputSystem(m_pInputMgr);
    if(m_pTrayMgr)		delete m_pTrayMgr;
    if(m_pRoot)			delete m_pRoot;
}

bool OgreFramework::initOgre(Ogre::String wndTitle, OIS::KeyListener *pKeyListener, OIS::MouseListener *pMouseListener)
{
    Ogre::LogManager* logMgr = new Ogre::LogManager();

    m_pLog = Ogre::LogManager::getSingleton().createLog("Ogre.log", true, true, false);
    m_pLog->setDebugOutputEnabled(true);

    m_pRoot = new Ogre::Root();

#if LAUNCH_WITH_CONFIG_DIALOG<1
	manualInitialize("OpenGL");
#else
    if(!m_pRoot->showConfigDialog())
        return false;
#endif

    m_pRenderWnd = m_pRoot->initialise(true, wndTitle);

	createScene();
	createCamera();

    unsigned long hWnd = 0;
    OIS::ParamList paramList;
    m_pRenderWnd->getCustomAttribute("WINDOW", &hWnd);

    paramList.insert(OIS::ParamList::value_type("WINDOW", Ogre::StringConverter::toString(hWnd)));

    m_pInputMgr = OIS::InputManager::createInputSystem(paramList);

    m_pKeyboard = static_cast<OIS::Keyboard*>(m_pInputMgr->createInputObject(OIS::OISKeyboard, true));
    m_pMouse = static_cast<OIS::Mouse*>(m_pInputMgr->createInputObject(OIS::OISMouse, true));

    m_pMouse->getMouseState().height = m_pRenderWnd->getHeight();
    m_pMouse->getMouseState().width	 = m_pRenderWnd->getWidth();

    if(pKeyListener == 0)
        m_pKeyboard->setEventCallback(this);
    else
        m_pKeyboard->setEventCallback(pKeyListener);

    if(pMouseListener == 0)
        m_pMouse->setEventCallback(this);
    else
        m_pMouse->setEventCallback(pMouseListener);

    Ogre::String secName, typeName, archName;
    Ogre::ConfigFile cf;
    cf.load("resources.cfg");

    Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        Ogre::ConfigFile::SettingsMultiMap *settings = seci.getNext();
        Ogre::ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(archName, typeName, secName);
        }
    }
    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

	createGUI();

    m_pTimer = new Ogre::Timer();
    m_pTimer->reset();

    m_pRenderWnd->setActive(true);

    return true;
}

bool OgreFramework::manualInitialize(Ogre::String desiredRenderer)
{
	// From http://rnurminen.originator.fi/projects/initializing-ogre-without-using-the-config-dialog/

    RenderSystem *renderSystem;
    bool ok = false;
    Ogre::RenderSystemList renderers = m_pRoot->getAvailableRenderers();

    // See if the list is empty (no renderers available)
    if(renderers.empty())
        return false;

    for(RenderSystemList::iterator it = renderers.begin(); it != renderers.end(); it++)
    {
        renderSystem = (*it);
        if(strstr(renderSystem->getName().c_str(), desiredRenderer.c_str()))
        {
            ok = true;
            break;
        }
    }

    if(!ok) {
        // We still don't have a renderer; pick
        // up the first one from the list
        renderSystem = (*renderers.begin());
    }

    Root::getSingleton().setRenderSystem(renderSystem);

    // Manually set some configuration options (optional)
	renderSystem->setConfigOption("Full Screen", "No");
    //renderSystem->setConfigOption("Video Mode", "640 x 480");

    return true;
}

void OgreFramework::createScene()
{
    m_pSceneMgr = m_pRoot->createSceneManager(ST_GENERIC, "SceneManager");
}

void OgreFramework::createCamera()
{
    m_pCamera = m_pSceneMgr->createCamera("Camera");
    m_pCamera->setPosition(Ogre::Vector3(-300,200,0));
    m_pCamera->lookAt(Ogre::Vector3(100,100,0));
    m_pCamera->setNearClipDistance(5);

    m_pViewport = m_pRenderWnd->addViewport(m_pCamera);
    m_pViewport->setBackgroundColour(ColourValue(0.0f, 0.0f, 0.0f, 1.0f));

    m_pCamera->setAspectRatio(Real(m_pViewport->getActualWidth()) / Real(m_pViewport->getActualHeight()));

    m_pViewport->setCamera(m_pCamera);
}

void OgreFramework::createGUI()
{
	m_pTrayMgr = new OgreBites::SdkTrayManager("TrayMgr", m_pRenderWnd, m_pMouse, this);
    m_pTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
    m_pTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
    m_pTrayMgr->hideCursor();

    // create a params panel for displaying sample details
    Ogre::StringVector items;
    items.push_back("timeTakenAnimation");
    items.push_back("timeSinceLast");
    items.push_back("playerPos");
	items.push_back("aimAngleH");
	items.push_back("aimAngleV");

	m_pDetailsPanel = m_pTrayMgr->createParamsPanel(OgreBites::TL_NONE, "DetailsPanel", 300, items);
	
	m_pTrayMgr->toggleAdvancedFrameStats();
	m_pTrayMgr->moveWidgetToTray(m_pDetailsPanel, OgreBites::TL_TOPRIGHT, 0);
}

void OgreFramework::updateOgre(unsigned long timeSinceLastFrame)
{
    m_MoveScale = m_MoveSpeed   * (float)timeSinceLastFrame;
    m_RotScale  = m_RotateSpeed * (float)timeSinceLastFrame;

    m_TranslateVector = Vector3::ZERO;

    processInput();
    updateCamera();

    m_FrameEvent.timeSinceLastFrame = (Ogre::Real) timeSinceLastFrame;
    m_pTrayMgr->frameRenderingQueued(m_FrameEvent);
}

void OgreFramework::updateCamera()
{
    if(m_pKeyboard->isKeyDown(OIS::KC_LSHIFT))
        m_pCamera->moveRelative(m_TranslateVector / 5);
    else
        m_pCamera->moveRelative(m_TranslateVector);

	//int x = m_pMouse->getMouseState().X.rel;
	//this->cameraHorizAngle += Ogre::Radian(-2.3*x/m_pRenderWnd->getWidth());
}

void OgreFramework::processInput()
{
	if(m_pKeyboard->isKeyDown(OIS::KC_LEFT))
        m_TranslateVector.x = -m_MoveScale;

    if(m_pKeyboard->isKeyDown(OIS::KC_RIGHT))
        m_TranslateVector.x = m_MoveScale;

    if(m_pKeyboard->isKeyDown(OIS::KC_UP))
        m_TranslateVector.z = -m_MoveScale;

    if(m_pKeyboard->isKeyDown(OIS::KC_DOWN))
        m_TranslateVector.z = m_MoveScale;
}

bool OgreFramework::keyPressed(const OIS::KeyEvent &keyEventRef)
{
    if(m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
    {
        m_bShutDownOgre = true;
        return true;
    }

    if(m_pKeyboard->isKeyDown(OIS::KC_SYSRQ))
    {
        m_pRenderWnd->writeContentsToTimestampedFile("screenshot_", ".jpg");
        return true;
    }

	// Toggle visibility of entire GUI
    if(m_pKeyboard->isKeyDown(OIS::KC_O))
    {
        if(m_pTrayMgr->isLogoVisible())
        {
            m_pTrayMgr->hideLogo();
            m_pTrayMgr->hideFrameStats();

			m_pTrayMgr->removeWidgetFromTray(m_pDetailsPanel);
            m_pDetailsPanel->hide();
        }
        else
        {
            m_pTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
            m_pTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
			m_pTrayMgr->toggleAdvancedFrameStats();

			m_pTrayMgr->moveWidgetToTray(m_pDetailsPanel, OgreBites::TL_TOPRIGHT, 0);
            m_pDetailsPanel->show();
        }
    }

	// Toggle visibility of advanced frame stats
	if (m_pKeyboard->isKeyDown(OIS::KC_F))
        m_pTrayMgr->toggleAdvancedFrameStats();

    return true;
}

bool OgreFramework::keyReleased(const OIS::KeyEvent &keyEventRef)
{
    return true;
}

bool OgreFramework::mouseMoved(const OIS::MouseEvent &evt)
{
    m_pCamera->yaw(Degree(evt.state.X.rel * -0.1f));
    m_pCamera->pitch(Degree(evt.state.Y.rel * -0.1f));

	//if (mTrayMgr->injectMouseMove(arg)) return true;

    return true;
}

bool OgreFramework::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	//if (mTrayMgr->injectMouseDown(arg, id)) return true;
    return true;
}

bool OgreFramework::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	//if (mTrayMgr->injectMouseUp(arg, id)) return true;
    return true;
}