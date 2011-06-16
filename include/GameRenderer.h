//----------------------------------------------------------------------------//
// GameRenderer.h                                                             //
// Copyright (C) 2009 Lukas Meindl						                      //
//----------------------------------------------------------------------------//

#ifndef GAMERENDERER_H
#define GAMERENDERER_H


//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "Ogre.h"
#include "OgreSceneManager.h"


using namespace Ogre;

//----------------------------------------------------------------------------//
// Class declaration                                                          //
//----------------------------------------------------------------------------//

class GameRenderer
{
public:
	//Constructors
	GameRenderer();
	GameRenderer(Ogre::SceneManager* mSceneMgr);
	~GameRenderer();

	void init();

	//Functions
	void update(unsigned long timeSinceLastFrame);

	//void addEntity(GameEntity* gameEntity, string* entityName, string* meshName, string* nodeName, QueryFlags flagName);

	void deleteItemFromScene(SceneNode* itemNode);
	static void destroyAllAttachedMovableObjects( SceneNode* i_pSceneNode );
	void destroyEntity(Entity* ent);
	static void destroyManualObject(Ogre::ManualObject* manualObj, GameRenderer* gameRenderer);

	Ogre::SceneManager* getSceneManager(){return mSceneMgr;};

private:
	GameRenderer&  operator = (const GameRenderer& original) {}
	GameRenderer(const GameRenderer& original) {}

	//variables
	Ogre::SceneManager* mSceneMgr;

	//Itembezeichner/Meshname Liste für alle Characters
	std::map<int, std::string> characterMeshes;

	//Functions
	void initCharacterMeshes();
	void initItemMeshes();
	//std::string getItemMeshName(ItemIdentifier itemIdentifier);
};

#endif