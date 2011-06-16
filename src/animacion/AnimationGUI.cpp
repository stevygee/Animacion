/*
	Animación!
	Copyright (c) 2011 Stefan Grassberger
	See license in Animacion.h
*/

#include "AnimationGUI.h"

AnimationGUI::AnimationGUI(GameRenderer* gameRenderer)
{
	this->gameRenderer = gameRenderer;

	this->animController = 0;
	this->m_pOverlay = 0;

	this->visible = false;
	this->initialized = false;

	this->graphs;
	this->text = 0;
}

AnimationGUI::~AnimationGUI()
{
	// Destroy graphs
	clearGraphs();

	if(text)
	{
		delete text;
		text = 0;
	}

	// Destroy direction lines
	for(std::map<std::string, ManualObject*>::iterator i = this->directionVectors.begin(); i != this->directionVectors.end();)
	{
		GameRenderer::destroyManualObject( (*i).second, this->gameRenderer );
		(*i).second = 0;
		i = this->directionVectors.erase(i);
	}

	if(m_pOverlay)
	{
		Ogre::OverlayManager::getSingletonPtr()->destroy(m_pOverlay);
		m_pOverlay = 0;
	}
}

void AnimationGUI::init(AnimationController* animController)
{
	// Create overlay
	m_pOverlay = Ogre::OverlayManager::getSingletonPtr()->create("animGUIoverlay");

	// Create text headline
	this->text = new ObjectTextDisplay(m_pOverlay, "header", -0.45f, 0.04f);
	this->text->init();
	this->text->enable(this->visible);

	// Set up graphs etc. for this character
	initChar(animController);

	// Node and Aim direction vectors for player character // TODO: All characters
	initDirectionVectors();

	// Done
	this->initialized = true;
}

void AnimationGUI::initChar(AnimationController* animController)
{
	this->animController = animController;
	this->entityName = animController->getEntityName();

	clearGraphs();

	// Create 1 graph per AnimationState
	// TODO: Arrange more than 9 animations on screen
	Ogre::AnimationStateIterator it = this->animController->getAnimStateIter();

	for(int i = 0; it.hasMoreElements(); i++)
	{
		Ogre::AnimationState* anim = it.getNext();

		// Graph & text positions
		float posX;
		float posY;
		float txtPosX;
		float txtPosY;

		// X
		if(i <= 2)
		{
			posX = -0.9f;
			txtPosX = -0.45f;
		}
		else if(i <= 5)
		{
			posX = -0.5f;
			txtPosX = -0.25f;
		}
		else
		{
			posX = -0.1f;
			txtPosX = -0.05f;
		}

		// Y
		if(i == 0 || i == 3 || i == 6)
		{
			posY = 0.5f;
			txtPosY = 0.13f;
		}
		else if(i == 1 || i == 4 || i == 7)
		{
			posY = 0.2f;
			txtPosY = 0.28f;
		}
		else
		{
			posY = -0.1f;
			txtPosY = 0.43f;
		}

		AnimationGraph* graph = new AnimationGraph(this->gameRenderer, m_pOverlay, anim->getAnimationName(), posX, posY, txtPosX, txtPosY);
		graph->init();
		graph->setVisible(this->visible);
		this->graphs.push_back(graph);
	}

	// Set headline
	std::stringstream ss;
	std::string manualObjName;
	ss << "CharacterAnimation:";
	ss << removeSpaces(getEntityName());
	ss >> manualObjName;
	this->text->setText(manualObjName);
}

void AnimationGUI::initDirectionVectors()
{
	// Create manual objects
	directionVectors["nodeDir"]				= this->gameRenderer->getSceneManager()->createManualObject( "gameGUI_manualObj_nodeDir" );
	directionVectors["aimDirHorizontal"]	= this->gameRenderer->getSceneManager()->createManualObject( "gameGUI_manualObj_aimDirHorizontal" );
	directionVectors["aimDirVertical"]		= this->gameRenderer->getSceneManager()->createManualObject( "gameGUI_manualObj_aimDirVertical" );

	for(std::map<std::string, ManualObject*>::iterator i = this->directionVectors.begin(); i != this->directionVectors.end(); i++)
	{
		std::string name = (*i).first;
		ManualObject* dirVector = (*i).second;

		// Use identity view/projection matrices
		dirVector->setUseIdentityProjection(false);
		dirVector->setUseIdentityView(false);
		dirVector->setDynamic(true);

		// Material
		dirVector->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_LIST);

		// Initial values
		dirVector->position(Vector3(0,0,0));
		dirVector->colour(1,1,1,0.5);	
		dirVector->end();

		// Construct sceneNode name
		std::stringstream ss;
		std::string sceneNodeName;
		ss << "gameGUI_manualObj_";
		ss << name;
		ss << "_node";
		ss >> sceneNodeName;

		// Add to scene
		SceneNode* sceneNode = this->gameRenderer->getSceneManager()->getRootSceneNode()->createChildSceneNode( sceneNodeName );
		sceneNode->attachObject( dirVector );
	}
}

void AnimationGUI::update(unsigned long timeSinceLastFrame)
{
	if(isInitialized() && this->visible)
	{
		// Set new values
		Ogre::AnimationStateIterator it = this->animController->getAnimStateIter();
		for(int i = 0; it.hasMoreElements(); i++)
		{
			Ogre::AnimationState* anim = it.getNext();
			this->graphs.at(i)->setNewValue( "first", this->animController->getAnimWeight( anim->getAnimationName(), "upperbody" ) );
			this->graphs.at(i)->setNewValue( "second", this->animController->getAnimWeight( anim->getAnimationName(), "lowerbody" ) );
		}

		// Update graphs
		for(unsigned int i = 0; i < this->graphs.size(); i++)
		{
			this->graphs.at(i)->update(timeSinceLastFrame);
		}

		// Update text
		this->text->update();

		// Update direction vectors
		updateDirectionVectors();
	}
}

void AnimationGUI::updateDirectionVectors()
{
	for(std::map<std::string, ManualObject*>::iterator i = this->directionVectors.begin(); i != this->directionVectors.end(); i++)
	{
		std::string name = (*i).first;
		ManualObject* dirVector = (*i).second;

		// Update Node Direction
		dirVector->beginUpdate(0);

		// Construct coordinates
		Ogre::Vector3 lineStart = this->animController->getNodePosition();
		lineStart.y += 1.5f;

		Ogre::Vector3 lineEnd = this->animController->getNodePosition();
		if( name.compare("nodeDir") == 0 )					{ lineEnd += this->animController->getDirectionVector("nodeDirection");		}
		else if( name.compare("aimDirHorizontal") == 0 )	{ lineEnd += this->animController->getDirectionVector("aimDirHorizontal");	}
		else if( name.compare("aimDirVertical") == 0 )		{ lineEnd += this->animController->getDirectionVector("aimDirVertical");	}
		lineEnd.y += 1.5f;

		// Apply coordinates
		dirVector->position(lineStart);
		dirVector->colour(1.0f, 1.0f, 1.0f, 1.0f);

		dirVector->position(lineEnd);
		dirVector->colour(1.0f, 1.0f, 1.0f, 1.0f);

		dirVector->end();
	}
}

// from http://www.daniweb.com/forums/thread98542.html
std::string AnimationGUI::removeSpaces( std::string stringIn )
{
	std::string::size_type pos = 0;
	bool spacesLeft = true;

	while( spacesLeft )
	{
		pos = stringIn.find(" ");
		if( pos != std::string::npos )
		stringIn.erase( pos, 1 );
		else
		spacesLeft = false;
	}

	return stringIn;
}

void AnimationGUI::clearGraphs()
{
	// Remove old graphs if there are any
	for(std::vector<AnimationGraph*>::iterator i = this->graphs.begin(); i != this->graphs.end();)
	{
		delete *i;
		i = this->graphs.erase(i); // Delete
	}

	this->graphs.clear();
}

void AnimationGUI::toggleVisible()
{
	this->visible = !this->visible;

	// Set all text visiblity (all use the same overlay lol)
	this->text->enable(this->visible);

	// Set graph's visibility
	for(unsigned int i = 0; i < this->graphs.size(); i++)
	{
		this->graphs.at(i)->setVisible(this->visible);
	}

	// Set direction vector's visibility
	for(std::map<std::string, ManualObject*>::iterator i = this->directionVectors.begin(); i != this->directionVectors.end(); i++)
	{
		(*i).second->setVisible(this->visible);
	}
}

bool AnimationGUI::isInitialized()
{
	return this->initialized;
}

Ogre::String AnimationGUI::getEntityName()
{
	return this->entityName;
}

//----------------------------------------------------------------------------//

AnimationGraph::AnimationGraph(GameRenderer* gameRenderer, Ogre::Overlay* m_pOverlay, std::string name, float posX, float posY, float txtPosX, float txtPosY, float width, float height)
{
	this->gameRenderer = gameRenderer;
	this->m_pOverlay = m_pOverlay;
	this->visible = false;

	this->manual_first = 0;
	this->manual_second = 0;

	this->text = 0;

	this->name = name;
	this->posX = posX;
	this->posY = posY;
	this->txtPosX = txtPosX;
	this->txtPosY = txtPosY;
	this->width = width;
	this->height = height;

	// settings
	this->sampleTime = 10.f;
	this->samples = 80;
	this->color_first = Ogre::ColourValue(	Ogre::Real(0.0f),	// R
											Ogre::Real(0.0f),	// G
											Ogre::Real(1.0f) );	// B
	this->color_second = Ogre::ColourValue(	Ogre::Real(1.0f),	// R
											Ogre::Real(0.5f),	// G
											Ogre::Real(0.0f) );	// B

	// do not change:
	this->timeElapsed = 0.f;
	this->nextValue_first = 0.f;
	this->nextValue_second = 0.f;
}

AnimationGraph::~AnimationGraph()
{
	if(text)
	{
		delete text;
		text = 0;
	}

	// Destroy lines
	GameRenderer::destroyManualObject( this->manual_first, this->gameRenderer );
	GameRenderer::destroyManualObject( this->manual_second, this->gameRenderer );

	manual_first = 0;
	manual_second = 0;
}

void AnimationGraph::init()
{
	for(int i = 0; i < this->samples; i++)
	{
		this->values_first.push_front(0.f);
		this->values_second.push_front(0.f);
	}

	// Create manual object name
	std::stringstream ss;
	std::string manual_first_name;
	ss << "gameGUI_manualObj_";
	ss << this->name;
	ss << "_";
	ss << "first";
	ss >> manual_first_name;

	std::stringstream ss2;
	std::string manual_second_name;
	ss2 << "gameGUI_manualObj_";
	ss2 << this->name;
	ss2 << "_";
	ss2 << "second";
	ss2 >> manual_second_name;

	this->manual_first = createManualObject( manual_first_name );
	this->manual_second = createManualObject( manual_second_name );

	this->text = new ObjectTextDisplay(this->m_pOverlay, this->name, this->txtPosX, this->txtPosY);
	this->text->init();
	this->text->enable(this->visible);
	this->text->setText(this->name);
}

Ogre::ManualObject* AnimationGraph::createManualObject(std::string name)
{
	// Create a manual object for 2D
	Ogre::ManualObject* manual = gameRenderer->getSceneManager()->createManualObject(name);

	// Use identity view/projection matrices
	manual->setUseIdentityProjection(true);
	manual->setUseIdentityView(true);
	manual->setDynamic(true);

	manual->begin("BaseWhiteNoLighting", Ogre::RenderOperation::OT_LINE_STRIP);

	// Set inital position and color
	for(int i = 0; i < this->samples; i++)
	{
		manual->position(this->posX + (this->width/this->samples*i), this->posY + this->height, 0.0);
		manual->colour(Ogre::Real(0.0f), Ogre::Real(0.0f), Ogre::Real(1.0f), Ogre::Real(1.0f));
	}

	// Indizes (leave open at the end; don't close)
	for(int i = 0; i < this->samples; i++)
	{
		manual->index(i);
	}

	manual->end();

	// Use infinite AAB to always stay visible
	Ogre::AxisAlignedBox aabInf;
	aabInf.setInfinite();
	manual->setBoundingBox(aabInf);

	// Render just before overlays
	manual->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY - 1);

	// Attach to scene
	gameRenderer->getSceneManager()->getRootSceneNode()->createChildSceneNode()->attachObject(manual);

	return manual;
}

void AnimationGraph::setVisible(bool visible)
{
	this->manual_first->setVisible(visible);
	this->manual_second->setVisible(visible);
	this->text->enable(visible);
}

void AnimationGraph::setNewValue(std::string name, float newValue)
{
	if( name.compare("first") == 0 )
	{
		this->nextValue_first = newValue;
	}
	else
	{
		this->nextValue_second = newValue;
	}
}

void AnimationGraph::update(unsigned long timeSinceLastFrame)
{
	// Update data
	timeElapsed += timeSinceLastFrame;
	if(this->timeElapsed >= this->sampleTime)
	{
		this->values_first.push_back( this->nextValue_first );
		this->values_first.pop_front();

		this->values_second.push_back( this->nextValue_second );
		this->values_second.pop_front();

		timeElapsed = 0.f;
	}

	// Update lines
	updateLine(this->manual_first, &this->values_first, this->color_first);
	updateLine(this->manual_second, &this->values_second, this->color_second);

	// Update text
	std::stringstream ss;
	std::string newTextValue;
	ss << this->name;
	ss << "_";
	ss << this->nextValue_first;
	ss << "_";
	ss << this->nextValue_second;
	ss >> newTextValue;

	this->text->setText(newTextValue);
	this->text->update();
}

void AnimationGraph::updateLine(Ogre::ManualObject* manual, std::deque<float>* values, Ogre::ColourValue color)
{
	manual->beginUpdate(0);
	
	for(long i = 0; i < (long)values->size(); ++i)
	{
		manual->position(	this->posX + ( this->width / this->samples * i ),
							this->posY + ( this->height * values->at(i) ),
							0.0 );

		manual->colour(color);
	}

	manual->end();
}