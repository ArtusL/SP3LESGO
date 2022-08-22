#pragma once
#include "Assignment1.h"
#include "Application.h"

#include <vector>

enum SCENES
{
	S_ASSIGNMENT1,
	S_COUNT
};

class SceneManager : public SceneBase
{
private:
	int sceneIndex;
public:
	std::vector<Scene*> SceneList;
	static SCENES activeScene;
	static SCENES prevScene;

	SceneManager();
	~SceneManager();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void initScenes();
	void ReInitScenes();
	Scene* getScene();
};


