

#ifndef OBJECTTEXTDISPLAY_H
#define OBJECTTEXTDISPLAY_H

// TODO: Re-add functionality from
// http://www.ogre3d.org/tikiwiki/ObjectTextDisplay

#include <string>

#include "Ogre.h"

class ObjectTextDisplay
{
	// Functions
public:
	ObjectTextDisplay(Ogre::Overlay* m_pOverlay, std::string ID, float posX, float posY);
	virtual ~ObjectTextDisplay();

	void init();
	void enable(bool enable);
	void setText(const Ogre::String& text);
    void update();

	// Variables
protected:
    bool m_enabled;
    Ogre::Overlay* m_pOverlay;
    Ogre::OverlayElement* m_pText;
    Ogre::OverlayContainer* m_pContainer;
    Ogre::String m_text;

	float posX;
	float posY;
	std::string ID;

	std::string containerName;
	std::string textAreaName;
};

#endif