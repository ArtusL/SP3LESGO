#pragma once
#include "Assignment1.h"
#include <vector>

enum SCENES
{
	S_LEVEL1 = 0,
	S_LEVEL2,
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

#pragma once
