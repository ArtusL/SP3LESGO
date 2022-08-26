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
	M_CHOOSE,
	M_BOMB,
	M_ARROW,
	M_CARD,
	M_CONTROL,
	M_UPGRADE,
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
		GEO_MAIN_MENU,
		GEO_PAUSE,
		GEO_SELECTOR,
		GEO_UIBG,
		GEO_GAMEOVER,
		GEO_DEADBG,
		GEO_SHOPBG,


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
		GEO_INFOBORDERRED,

		// Enemy types
		GEO_BDEMON,
		GEO_FDEMON,
		GEO_GHOST,
		GEO_NIGHTMARE,
		GEO_BDEMON_LEFT,
		GEO_FDEMON_LEFT,
		GEO_GHOST_LEFT,
		GEO_NIGHTMARE_LEFT,
		GEO_FLAMEDEMON,
		GEO_BOSS,
		GEO_BOSS_LEFT,
		GEO_BOSSATTACK,
		GEO_BOSSATTACK_LEFT,
		GEO_EXPLODER,
		GEO_EXPLODER_LEFT,
		GEO_ENEMYEXPLOSION,
		// Unique Enemy type
		GEO_WORMHEAD,
		GEO_WORMBODY1,
		GEO_WORMBODY2,
		GEO_WORMTAIL,
		// Player and All Weapons (including enemy)
		GEO_SHIP,
		GEO_PRIMARY,
		GEO_ENEMYBULLET,
		GEO_LASER,
		GEO_CARDS,
		GEO_RING,
		GEO_RINGAURA,
		GEO_BOMB,
		GEO_MOLOTOV,
		GEO_FIRE,
		GEO_EXPLOSION,
		GEO_ARROW,
		GEO_FLAMINGARROW,
		GEO_TRIPLESHOT,
		// Obstacles on map
		GEO_CHEST,
		GEO_CHEST_PARTICLE,
		GEO_TREE,
		GEO_SWAMP,
		// Players animations
		GEO_HERO,
		GEO_HERO_LEFT,
		GEO_HEROATTACK,
		GEO_HEROATTACK_LEFT,
		GEO_HERORUN,
		GEO_HERORUN_LEFT,
		//Shrek and animations
		GEO_SHREK,
		GEO_SHREK_LEFT,
		// story photos
		GEO_MAFIASHREK,
		GEO_EVIL,

		NUM_GEOMETRY,
	};
public:
	static MENU_TYPE menuType;
	static bool restartGame;
	static bool resetGame;
	static bool bombChoose;
	static bool arrowChoose;
	static bool cardChoose;
	static bool ringUpgrade;
	static bool healthUpgrade;
	static bool molotovUpgrade;
	static bool firerateUpgrade;
	static bool damageUpgrade;
	static bool upgradescreen;

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
	void RenderChoose();
	void RenderCBomb();
	void RenderCArrow();
	void RenderCCard();
	void RenderControl();
	void RenderUpgrade();
	void UpdateUpgrade(float& m_speed);
	void UpdateControl(float& m_speed);
	void UpdateCBomb(float& m_speed);
	void UpdateCArrow(float& m_speed);
	void UpdateCCard(float& m_speed);
	void UpdateChoose(float& m_speed);
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