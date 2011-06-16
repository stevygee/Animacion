/*
	Animación!
	Copyright (c) 2011 Stefan Grassberger
	See license in Animacion.h
*/

#include "GameAnimation.h"

GameAnimation::GameAnimation(GameRenderer* gameRenderer)
{
	this->gameRenderer = gameRenderer;

	this->timeTaken = 1;
	this->startTime = 0;
	this->m_pTimer = 0;
}

GameAnimation::~GameAnimation()
{
	// Delete GUI
	delete this->animGUI;
	this->animGUI = 0;

	// Delete all controllers, pipelines...
	this->animCharacters.clear();

	// Delete XML
	this->xmlFiles.clear();
}

unsigned long GameAnimation::update(unsigned long timeSinceLastFrame)
{
	this->startTime = this->m_pTimer->getMillisecondsCPU();

	// This is freaking important! Must be in seconds and an Ogre::Real
	Ogre::Real secSinceLastFrame = ((Ogre::Real) timeSinceLastFrame) / ((Ogre::Real) 1000.0);

	// Update all character's animation
	for(unsigned int i = 0; i < this->animCharacters.size(); i++)
	{
		// Reset all AnimationStates
		this->animCharacters.at(i)->animPipeline->init(false);

		// Update all AnimationControllers
		this->animCharacters.at(i)->animController->update( secSinceLastFrame );

		// Update all AnimationPipelines
		this->animCharacters.at(i)->animPipeline->update(
			this->animCharacters.at(i)->animController->getActiveStates(),
			secSinceLastFrame
		);
	}

	// Update GUI
	this->animGUI->update( timeSinceLastFrame );

	this->timeTaken = this->m_pTimer->getMillisecondsCPU() - startTime;
	return this->timeTaken;
}

void GameAnimation::init()
{
	// GUI
	this->animGUI = new AnimationGUI(this->gameRenderer);

	// XML
	this->xmlFiles.clear();
	this->xmlFiles["eva.xml"]			= new AnimationXML("eva.xml");
	this->xmlFiles["malezombie.xml"]	= new AnimationXML("malezombie.xml");

	loadXML();

	// Timer
	this->m_pTimer = new Ogre::Timer();
	this->m_pTimer->reset();
}

void GameAnimation::loadXML()
{
	// Load all XML files and init AnimationXML classes
	for(std::map<std::string, AnimationXML*>::iterator i = this->xmlFiles.begin(); i != this->xmlFiles.end(); i++)
	{
		(*i).second->init();
	}
}

void GameAnimation::reloadXML()
{
	loadXML();

	// Reset all AnimationControllers & AnimationPipelines
	for(unsigned int i = 0; i < this->animCharacters.size(); i++)
	{
		this->animCharacters.at(i)->animController->reset();
		this->animCharacters.at(i)->animPipeline->init();
	}
}

AnimationXML* GameAnimation::getXML(std::string xmlFileName) throw(int)
{
	// Find AnimationXML
	std::map<std::string, AnimationXML*>::iterator i = this->xmlFiles.find( xmlFileName );
	if( i != this->xmlFiles.end() )
		return (*i).second;

	throw(1); // AnimationXML instance not found
}

AnimationController* GameAnimation::registerCharacter(int characterType, Ogre::SceneNode* node, Ogre::Entity* ent)
{
	animatedCharacter* animCharacter = new animatedCharacter();
	AnimationPipeline* animPipeline;
	AnimationController* animController;
	AnimationXML* animXML;

	// Initialize XML, pipeline and controller
	switch(characterType)
	{
		case PLAYER_CHARACTER:
			animXML = getXML( "eva.xml" );
			animPipeline = new AnimationPipeline( animXML, node, ent );
			animController = new AnimCtrlPlayerCharacter( animPipeline, animXML, ent->getName() );

			break;
		case AI_CHARACTER:
		default:
			animXML = getXML( "malezombie.xml" );
			animPipeline = new AnimationPipeline( animXML, node, ent );
			animController = new AnimCtrlAiCharacter( animPipeline, animXML, ent->getName() );

			break;
	}

	animPipeline->init();
	animController->init();

	// Store this animated character
	animCharacter->name = ent->getName();
	animCharacter->animController = animController;
	animCharacter->animPipeline = animPipeline;
	this->animCharacters.push_back( animCharacter );

	// Initialize GUI
	if( !this->animGUI->isInitialized() )
		this->animGUI->init( animController );

	return animController;
}

void GameAnimation::unregisterCharacter(Ogre::Entity* ent)
{
	for(std::vector<animatedCharacter*>::iterator i = this->animCharacters.begin(); i != this->animCharacters.end();)
	{
		if((*i)->name.compare(ent->getName()) != 0)
		{
			++i;
		}
		else
		{
			delete (*i)->animController;
			delete (*i)->animPipeline;

			delete *i;
			i = this->animCharacters.erase(i);
		}
	}

	if(ent->getName().compare(this->animGUI->getEntityName()) == 0) // Switch to next character in GUI
		cycleCharGUI();
}

void GameAnimation::cycleCharGUI()
{
	int nextCharIndex = 0;

	for(unsigned int i = 0; i < this->animCharacters.size(); i++)
	{
		if(this->animGUI->getEntityName().compare( this->animCharacters.at(i)->name ) == 0)
		{
			nextCharIndex = i + 1;
			if(nextCharIndex >= (int) this->animCharacters.size())
				nextCharIndex = 0;
			break;
		}
	}

	this->animGUI->initChar( this->animCharacters.at(nextCharIndex)->animController );
}