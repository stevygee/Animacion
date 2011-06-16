//----------------------------------------------------------------------------//
// GameRenderer.cpp                                                           //
// Copyright (C) 2009 - 2010 Lukas Meindl											  //
//----------------------------------------------------------------------------//


//----------------------------------------------------------------------------//
// Includes                                                                   //
//----------------------------------------------------------------------------//

#include "GameRenderer.h"

//----------------------------------------------------------------------------//
// Constants                                                                  //
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// Functions                                                                  //
//----------------------------------------------------------------------------//


GameRenderer::GameRenderer()
{
}

GameRenderer::~GameRenderer()
{
}

GameRenderer::GameRenderer(Ogre::SceneManager* mSceneMgr)
{
	this->mSceneMgr = mSceneMgr;
}


//Must be called after Scenemanager setup
void GameRenderer::init()
{
}


void GameRenderer::update(unsigned long timeSinceLastFrame)
{
}

void GameRenderer::initCharacterMeshes()
{
	//PLAYERCHARACTER
	characterMeshes[0] = "eva";

	//ENEMIES
	characterMeshes[1] = "malezombie";

	//Preload all meshes (Not final)
	std::map<int, std::string>::iterator iter;
	for(iter = characterMeshes.begin(); iter != characterMeshes.end(); iter++)
	{
		Ogre::Entity* characterEntity = mSceneMgr->createEntity((*iter).second + ".mesh");
		characterEntity->_initialise();
		mSceneMgr->destroyMovableObject(characterEntity);
		Ogre::MeshManager::getSingletonPtr()->load((*iter).second + ".mesh", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	}

	//Preload all materials + textures
	Ogre::MaterialManager* matMgr = Ogre::MaterialManager::getSingletonPtr();
	for(unsigned int i = 1; i <= 8; i++)
	{
		std::stringstream ss;
		std::string zombieMaterialName;
		ss << "maleZombieMaterial";
		ss << i;
		ss >> zombieMaterialName;

		matMgr->load(zombieMaterialName, "MasterMaterial");
	}

	matMgr->load("eva_material", "MasterMaterial");
}

/*

void GameRenderer::getItemEntity(ItemIdentifier itemIdentifier, Entity*& ent, QueryFlags flagName)
{
	if(!ent)
	{
		// Entity doesn't exist yet
		std::string meshName = getItemMeshName(itemIdentifier);
		Ogre::Entity* itemEntity = mSceneMgr->createEntity(meshName + ".mesh");
		itemEntity->setQueryFlags(flagName);
		ent = itemEntity;
	}
}*/

void GameRenderer::deleteItemFromScene(SceneNode* itemNode)
{
	GameRenderer::destroyAllAttachedMovableObjects(itemNode);
	itemNode->removeAndDestroyAllChildren();
	mSceneMgr->destroySceneNode(itemNode);
}



//For the discussion regarding this function check: http://www.ogre3d.org/forums/viewtopic.php?f=2&t=53647
void GameRenderer::destroyAllAttachedMovableObjects( SceneNode* i_pSceneNode )
{
	if (i_pSceneNode )
	{
		// Destroy all the attached objects
		SceneNode::ObjectIterator itObject = i_pSceneNode->getAttachedObjectIterator();

		while ( itObject.hasMoreElements() )
		{
			MovableObject* pObject = static_cast<MovableObject*>(itObject.getNext());
			i_pSceneNode->getCreator()->destroyMovableObject( pObject );
		}

		// Recurse to child SceneNodes
		SceneNode::ChildNodeIterator itChild = i_pSceneNode->getChildIterator();

		while ( itChild.hasMoreElements() )
		{
			SceneNode* pChildNode = static_cast<SceneNode*>(itChild.getNext());
			destroyAllAttachedMovableObjects( pChildNode );
		}
	}
}

void GameRenderer::destroyManualObject(Ogre::ManualObject* manualObj, GameRenderer* gameRenderer)
{
	if( manualObj )
	{
		Ogre::SceneManager* sceneMgr = gameRenderer->getSceneManager();

		if( gameRenderer->getSceneManager()->hasManualObject( manualObj->getName() ) )
		{
			if( manualObj->getParentSceneNode() )
				manualObj->getParentSceneNode()->detachObject( manualObj );

			gameRenderer->getSceneManager()->destroyManualObject( manualObj );
		}
	}
}