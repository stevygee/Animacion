

// TODO: Re-add functionality from
// http://www.ogre3d.org/tikiwiki/ObjectTextDisplay

#include "ObjectTextDisplay.h"

ObjectTextDisplay::ObjectTextDisplay(Ogre::Overlay* m_pOverlay, std::string ID, float posX, float posY) {
	this->m_pOverlay = m_pOverlay;
	this->ID = ID;
	this->posX = posX;
	this->posY = posY;

	this->m_enabled = false;
	this->m_text = "";

	this->m_pText = 0;
	this->m_pContainer = 0;
}

ObjectTextDisplay::~ObjectTextDisplay() {
	// overlay cleanup -- Ogre would clean this up at app exit but if your app 
	// tends to create and delete these objects often it's a good idea to do it here.

	//m_pOverlay->hide();
	Ogre::OverlayManager *overlayManager = Ogre::OverlayManager::getSingletonPtr();
	m_pContainer->removeChild(this->textAreaName);
	m_pOverlay->remove2D(m_pContainer);
	overlayManager->destroyOverlayElement(m_pText);
	overlayManager->destroyOverlayElement(m_pContainer);
	//overlayManager->destroy(m_pOverlay); // this is done later

	Ogre::OverlayElement* m_pText = 0;
    Ogre::OverlayContainer* m_pContainer = 0;
}

void ObjectTextDisplay::init() {
	std::stringstream ss;
	this->containerName;
	ss << "animGUI_";
	ss << this->ID;
	ss << "_container";
	ss >> this->containerName;

	std::stringstream ss2;
	this->textAreaName;
	ss2 << "animGUI_";
	ss2 << this->ID;
	ss2 << "_text";
	ss2 >> this->textAreaName;

	m_pContainer = static_cast<Ogre::OverlayContainer*>(Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", this->containerName));

	m_pOverlay->add2D(m_pContainer);

	m_pText = Ogre::OverlayManager::getSingleton().createOverlayElement("TextArea", this->textAreaName);
	m_pText->setDimensions(1.0, 1.0);
	m_pText->setMetricsMode(Ogre::GMM_PIXELS);
	m_pText->setPosition(0.0, 0.0);

	m_pText->setParameter("font_name", "BlueHighway");
	m_pText->setParameter("char_height", "14");
	m_pText->setParameter("horz_align", "center");
	m_pText->setColour(Ogre::ColourValue(1.0, 1.0, 1.0));

	m_pContainer->addChild(m_pText);
	m_pOverlay->show();
}
 
void ObjectTextDisplay::enable(bool enable) {
	m_enabled = enable;
	if (enable)
		m_pOverlay->show();
	else
		m_pOverlay->hide();
}
 
void ObjectTextDisplay::setText(const Ogre::String& text) {
	m_text = text;
	m_pText->setCaption(m_text);
}

void ObjectTextDisplay::update()  {
	if (!m_enabled)
		return;

	m_pContainer->setPosition(this->posX, this->posY);
}