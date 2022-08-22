#include "SceneManager.h"
SCENES SceneManager::activeScene;
SCENES SceneManager::prevScene;
#include "GL\glew.h"

SceneManager::SceneManager()
{
	Scene* scene1 = new SceneLevel1;
	SceneList.push_back(scene1);

	Scene* scene2 = new SceneLevel2;
	SceneList.push_back(scene2);
}

SceneManager::~SceneManager()
{
}

void SceneManager::Init()
{
}

void SceneManager::Update(double dt)
{
}

void SceneManager::Render()
{
}

void SceneManager::Exit()
{
}

void SceneManager::initScenes()
{
	for (int i = 0; i < SceneList.size(); ++i)
		SceneList[i]->Init();
}

void SceneManager::ReInitScenes()
{
	for (int i = 0; i < SceneList.size(); ++i)
		SceneList[i]->Exit();

	for (int i = 0; i < SceneList.size(); ++i)
		SceneList[i]->Init();
}

Scene* SceneManager::getScene()
{
	return SceneList[activeScene];
}
