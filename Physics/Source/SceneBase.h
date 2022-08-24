#ifndef SCENE_BASE_H
#define SCENE_BASE_H

#include "Scene.h"
#include "Mtx44.h"
#include "Camera.h"
#include "Mesh.h"
#include "MatrixStack.h"
#include "Light.h"
#include "GameObject.h"
#include <vector>


enum MENU_TYPE
{
	M_NONE = 0,
	M_MAIN,
	M_PAUSE,
	M_GAMEOVER,
	M_TOTAL
};
class SceneBase : public Scene
{
	enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
		U_LIGHTENABLED,
		U_NUMLIGHTS,
		U_LIGHT0_TYPE,
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_TOTAL,
	};
public:
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_TEXT,
		GEO_BACKGROUND,
		GEO_UPGRADESCREEN,
		GEO_EVIL,
		GEO_MAIN_MENU,
		GEO_PAUSE,
		GEO_SELECTOR,
		GEO_UIBG,
		GEO_GAMEOVER,


		GEO_BALL,
		GEO_CUBE,

		// Health bar / UI
		GEO_GREENHEALTH,
		GEO_REDHEALTH,
		GEO_HEAL,
		GEO_PLAYERHEALTH,

		GEO_BOSSHEALTH,
		GEO_HEALTHBACK,
		GEO_HEALTHBORDER,
		GEO_HEROICON,
		GEO_INFOBORDER,

		// Enemy types
		GEO_ASTEROID,
		GEO_ENEMYSHIP,
		GEO_BIGASTEROID,
		GEO_FLAMEDEMON,
		GEO_BOSS,
		GEO_BOSS_LEFT,
		GEO_BOSSATTACK,
		GEO_BOSSATTACK_LEFT,
		GEO_EXPLODER,
		GEO_EXPLODER_LEFT,
		GEO_ENEMYEXPLOSION,

		GEO_WORMHEAD,
		GEO_WORMBODY1,
		GEO_WORMBODY2,
		GEO_WORMTAIL,

		GEO_SHIP,
		GEO_CLUB,
		GEO_ENEMYBULLET,
		GEO_LASER,
		GEO_BOW,
		GEO_RING,
		GEO_RINGAURA,
		GEO_BOMB,
		GEO_MOLOTOV,
		GEO_FIRE,
		GEO_EXPLOSION,
		GEO_ARROW,
		GEO_FLAMINGARROW,
		GEO_TRIPLESHOT,

		GEO_CHEST,

		GEO_HERO,
		GEO_HERO_LEFT,
		GEO_HEROATTACK,
		GEO_HEROATTACK_LEFT,
		GEO_HERORUN,
		GEO_HERORUN_LEFT,

		GEO_SHREK,
		GEO_SHREK_LEFT,

		GEO_BDEMON,
		GEO_FDEMON,
		GEO_EYE,
		GEO_SKULL,
		GEO_GHOST,
		GEO_NIGHTMARE,

		GEO_BDEMON_LEFT,
		GEO_FDEMON_LEFT,
		GEO_GHOST_LEFT,
		GEO_NIGHTMARE_LEFT,


		NUM_GEOMETRY,
	};
public:
	static MENU_TYPE menuType;
	static bool restartGame;
	static bool resetGame;

	SceneBase();
	~SceneBase();

	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	void RenderText(Mesh* mesh, std::string text, Color color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y, bool smallerSpacing);
	void RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey);
	void RenderMesh(Mesh *mesh, bool enableLight);
	void RenderGO(GameObject *go);
	void RenderMainMenu();
	void RenderPauseMenu();
	void RenderGameOver();
	void UpdateMainMenu(float& m_speed);
	void UpdatePauseMenu(float& m_speed);
	void UpdateGameOver(float& m_speed);
	GameObject* FetchGO();
protected:
	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];

	Camera camera;

	MS modelStack;
	MS viewStack;
	MS projectionStack;

	Light lights[1];

	bool bLightEnabled;

	float fps;


	int selectorIndex;
	int colourIndex;
};

#endif