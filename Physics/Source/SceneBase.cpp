#include "SceneBase.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"
#include "Application.h"
#include "Assignment1.h"
#include "Utility.h"
#include "LoadTGA.h"
#include "LoadTexture.h"
#include <sstream>
bool SceneBase::restartGame = false;
bool SceneBase::resetGame = false;
bool SceneBase::bombChoose = false;
bool SceneBase::arrowChoose = false;
bool SceneBase::cardChoose = false;
MENU_TYPE SceneBase::menuType = M_MAIN;
SceneBase::SceneBase()
{
}

SceneBase::~SceneBase()
{
}

void SceneBase::Init()
{
	// Black background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	m_programID = LoadShaders("Shader//comg.vertexshader", "Shader//comg.fragmentshader");

	// Get a handle for our uniform
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	//m_parameters[U_MODEL] = glGetUniformLocation(m_programID, "M");
	//m_parameters[U_VIEW] = glGetUniformLocation(m_programID, "V");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");
	// Get a handle for our "colorTexture" uniform
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
	// Get a handle for our "textColor" uniform
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");

	// Use our shader
	glUseProgram(m_programID);

	lights[0].type = Light::LIGHT_DIRECTIONAL;
	lights[0].position.Set(0, 20, 0);
	lights[0].color.Set(1, 1, 1);
	lights[0].power = 1;
	lights[0].kC = 1.f;
	lights[0].kL = 0.01f;
	lights[0].kQ = 0.001f;
	lights[0].cosCutoff = cos(Math::DegreeToRadian(45));
	lights[0].cosInner = cos(Math::DegreeToRadian(30));
	lights[0].exponent = 3.f;
	lights[0].spotDirection.Set(0.f, 1.f, 0.f);

	glUniform1i(m_parameters[U_NUMLIGHTS], 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);

	glUniform1i(m_parameters[U_LIGHT0_TYPE], lights[0].type);
	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &lights[0].color.r);
	glUniform1f(m_parameters[U_LIGHT0_POWER], lights[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], lights[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], lights[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], lights[0].kQ);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], lights[0].cosCutoff);
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], lights[0].cosInner);
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], lights[0].exponent);

	camera.Init(Vector3(0, 0, 1), Vector3(0, 0, 0), Vector3(0, 1, 0));

	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = NULL;
	}
	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("reference", 1000, 1000, 1000);
	meshList[GEO_BALL] = MeshBuilder::GenerateSphere("ball", Color(1, 1, 1), 10, 10, 1.f);
	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("cube", Color(0, 0, 0), 2.f);

	meshList[GEO_BACKGROUND] = MeshBuilder::GenerateQuad("background", Color(1, 1, 1), 1.f);
	meshList[GEO_BACKGROUND]->textureID = LoadTGA("Image//background.tga");

	meshList[GEO_MAFIASHREK] = MeshBuilder::GenerateQuad("background", Color(1, 1, 1), 1.f);
	meshList[GEO_MAFIASHREK]->textureID = LoadTGA("Image//mafiashrek.tga");

	meshList[GEO_BOSS] = MeshBuilder::GenerateQuad("boss", Color(1, 1, 1), 1.f);
	meshList[GEO_BOSS]->textureID = LoadTGA("Image//NightBorne.tga");

	meshList[GEO_SHIP] = MeshBuilder::GenerateQuad("ship", Color(1, 1, 1), 1.f);
	meshList[GEO_SHIP]->textureID = LoadTGA("Image//playership2.tga");

	meshList[GEO_EVIL] = MeshBuilder::GenerateQuad("evil", Color(1, 1, 1), 1.f);
	meshList[GEO_EVIL]->textureID = LoadTGA("Image//Forces_of_evil.tga");

	// Health Bar
	meshList[GEO_GREENHEALTH] = MeshBuilder::GenerateQuad("green health", Color(0, 1, 0), 1.f);
	meshList[GEO_REDHEALTH] = MeshBuilder::GenerateQuad("red health", Color(1, 0, 0), 1.f);
	meshList[GEO_PLAYERHEALTH] = MeshBuilder::GenerateQuad("player health", Color(0, 0, 1), 1.f);

	meshList[GEO_HEAL] = MeshBuilder::GenerateQuad("healing item", Color(1, 0, 0), 1.f);
	meshList[GEO_HEAL]->textureID = LoadTGA("Image//heal.tga");

	//Hero
	meshList[GEO_HERO] = MeshBuilder::GenerateSpriteAnimation("Hero", 1, 4);
	meshList[GEO_HERO]->textureID = LoadTexture("Image//HeroIdle.png", true);
	meshList[GEO_HERO]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* sa = dynamic_cast<SpriteAnimation*>(meshList[GEO_HERO]);
	sa->AddAnimation("IDLE", 0, 4);

	meshList[GEO_HERO_LEFT] = MeshBuilder::GenerateSpriteAnimation("Hero Left", 1, 4);
	meshList[GEO_HERO_LEFT]->textureID = LoadTexture("Image//HeroIdle.png", false);
	meshList[GEO_HERO_LEFT]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* saLeft = dynamic_cast<SpriteAnimation*>(meshList[GEO_HERO_LEFT]);
	saLeft->AddAnimation("IDLE", 0, 4);

	meshList[GEO_HEROATTACK] = MeshBuilder::GenerateSpriteAnimation("Hero", 1, 6);
	meshList[GEO_HEROATTACK]->textureID = LoadTexture("Image//HeroAttack.png", true);
	meshList[GEO_HEROATTACK]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* saAttack = dynamic_cast<SpriteAnimation*>(meshList[GEO_HEROATTACK]);
	saAttack->AddAnimation("Attack", 0, 6);

	meshList[GEO_HEROATTACK_LEFT] = MeshBuilder::GenerateSpriteAnimation("Hero Left", 1, 6);
	meshList[GEO_HEROATTACK_LEFT]->textureID = LoadTexture("Image//HeroAttack.png", false);
	meshList[GEO_HEROATTACK_LEFT]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* saAttackLeft = dynamic_cast<SpriteAnimation*>(meshList[GEO_HEROATTACK_LEFT]);
	saAttackLeft->AddAnimation("Attack", 0, 6);

	meshList[GEO_HERORUN] = MeshBuilder::GenerateSpriteAnimation("Hero", 1, 12);
	meshList[GEO_HERORUN]->textureID = LoadTexture("Image//HeroRun.png", true);
	meshList[GEO_HERORUN]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* saRun = dynamic_cast<SpriteAnimation*>(meshList[GEO_HERORUN]);
	saRun->AddAnimation("Run", 0, 12);

	meshList[GEO_HERORUN_LEFT] = MeshBuilder::GenerateSpriteAnimation("Hero Left", 1, 12);
	meshList[GEO_HERORUN_LEFT]->textureID = LoadTexture("Image//HeroRun.png", false);
	meshList[GEO_HERORUN_LEFT]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* saRunLeft = dynamic_cast<SpriteAnimation*>(meshList[GEO_HERORUN_LEFT]);
	saRunLeft->AddAnimation("Run", 0, 12);

	//Special Health
	meshList[GEO_PLAYERHEALTH] = MeshBuilder::GenerateQuad("health", Color(1, 1, 1), 1.f);
	meshList[GEO_PLAYERHEALTH]->textureID = LoadTGA("Image//HpBar.tga");

	meshList[GEO_BOSSHEALTH] = MeshBuilder::GenerateQuad("health", Color(1, 1, 1), 1.f);
	meshList[GEO_BOSSHEALTH]->textureID = LoadTGA("Image//HpBarBoss.tga");

	meshList[GEO_HEALTHBACK] = MeshBuilder::GenerateQuad("hpback", Color(1, 1, 1), 1.f);
	meshList[GEO_HEALTHBACK]->textureID = LoadTGA("Image//HpBack.tga");

	meshList[GEO_HEALTHBORDER] = MeshBuilder::GenerateQuad("hpborder", Color(1, 1, 1), 1.f);
	meshList[GEO_HEALTHBORDER]->textureID = LoadTGA("Image//HpBorder.tga");

	meshList[GEO_HEROICON] = MeshBuilder::GenerateQuad("Icon", Color(1, 1, 1), 1.f);
	meshList[GEO_HEROICON]->textureID = LoadTGA("Image//HeroIcon.tga");

	meshList[GEO_INFOBORDER] = MeshBuilder::GenerateQuad("Border", Color(1, 1, 1), 1.f);
	meshList[GEO_INFOBORDER]->textureID = LoadTGA("Image//UiBorder.tga");

	meshList[GEO_INFOBORDERRED] = MeshBuilder::GenerateQuad("Border Red", Color(1, 1, 1), 1.f);
	meshList[GEO_INFOBORDERRED]->textureID = LoadTGA("Image//UiBorderRed.tga");

	// Shopkeeper 
	meshList[GEO_SHREK] = MeshBuilder::GenerateSpriteAnimation("Shrek", 1, 16);
	meshList[GEO_SHREK]->textureID = LoadTexture("Image//shopkeepershrek.png", false);
	meshList[GEO_SHREK]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* ShrekSprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_SHREK]);
	ShrekSprite->AddAnimation("IDLE", 0, 16);

	meshList[GEO_SHREK_LEFT] = MeshBuilder::GenerateSpriteAnimation("Shrek Left", 1, 16);
	meshList[GEO_SHREK_LEFT]->textureID = LoadTexture("Image//shopkeepershrek.png", true);
	meshList[GEO_SHREK_LEFT]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* ShrekSpriteLeft = dynamic_cast<SpriteAnimation*>(meshList[GEO_SHREK_LEFT]);
	ShrekSpriteLeft->AddAnimation("IDLE", 0, 16);

	// Enemy types
	meshList[GEO_FDEMON] = MeshBuilder::GenerateSpriteAnimation("Fdemon", 1, 6);
	meshList[GEO_FDEMON]->textureID = LoadTexture("Image//FlameDemon.png", false);
	meshList[GEO_FDEMON]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* Fdemonsprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_FDEMON]);
	Fdemonsprite->AddAnimation("IDLE", 0, 6);

	meshList[GEO_FDEMON_LEFT] = MeshBuilder::GenerateSpriteAnimation("Fdemon Left", 1, 6);
	meshList[GEO_FDEMON_LEFT]->textureID = LoadTexture("Image//FlameDemon.png", true);
	meshList[GEO_FDEMON_LEFT]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* FdemonspriteLeft = dynamic_cast<SpriteAnimation*>(meshList[GEO_FDEMON_LEFT]);
	FdemonspriteLeft->AddAnimation("IDLE", 0, 6);

	meshList[GEO_BDEMON] = MeshBuilder::GenerateSpriteAnimation("Bdemon", 1, 6);
	meshList[GEO_BDEMON]->textureID = LoadTexture("Image//Bdemon.png", false);
	meshList[GEO_BDEMON]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* Bdemonsprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_BDEMON]);
	Bdemonsprite->AddAnimation("IDLE", 0, 6);

	meshList[GEO_BDEMON_LEFT] = MeshBuilder::GenerateSpriteAnimation("Bdemon Left", 1, 6);
	meshList[GEO_BDEMON_LEFT]->textureID = LoadTexture("Image//Bdemon.png", true);
	meshList[GEO_BDEMON_LEFT]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* BdemonspriteLeft = dynamic_cast<SpriteAnimation*>(meshList[GEO_BDEMON_LEFT]);
	BdemonspriteLeft->AddAnimation("IDLE", 0, 6);

	meshList[GEO_NIGHTMARE] = MeshBuilder::GenerateSpriteAnimation("Nightmare", 1, 4);
	meshList[GEO_NIGHTMARE]->textureID = LoadTexture("Image//Nightmare.png", false);
	meshList[GEO_NIGHTMARE]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* Nightmaresprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_NIGHTMARE]);
	Nightmaresprite->AddAnimation("IDLE", 0, 4);

	meshList[GEO_NIGHTMARE_LEFT] = MeshBuilder::GenerateSpriteAnimation("Nightmare Left", 1, 4);
	meshList[GEO_NIGHTMARE_LEFT]->textureID = LoadTexture("Image//Nightmare.png", true);
	meshList[GEO_NIGHTMARE_LEFT]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* NightmarespriteLeft = dynamic_cast<SpriteAnimation*>(meshList[GEO_NIGHTMARE_LEFT]);
	NightmarespriteLeft->AddAnimation("IDLE", 0, 4);

	meshList[GEO_GHOST] = MeshBuilder::GenerateSpriteAnimation("Ghost", 1, 7);
	meshList[GEO_GHOST]->textureID = LoadTexture("Image//ghost-idle.png", false);
	meshList[GEO_GHOST]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* Ghostsprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_GHOST]);
	Ghostsprite->AddAnimation("IDLE", 0, 7);

	meshList[GEO_GHOST_LEFT] = MeshBuilder::GenerateSpriteAnimation("Ghost Left", 1, 7);
	meshList[GEO_GHOST_LEFT]->textureID = LoadTexture("Image//ghost-idle.png", true);
	meshList[GEO_GHOST_LEFT]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* GhostspriteLeft = dynamic_cast<SpriteAnimation*>(meshList[GEO_GHOST_LEFT]);
	GhostspriteLeft->AddAnimation("IDLE", 0, 7);

	// Exploder
	meshList[GEO_EXPLODER] = MeshBuilder::GenerateSpriteAnimation("Skull", 1, 8);
	meshList[GEO_EXPLODER]->textureID = LoadTexture("Image//FlameSkull.png", false);
	meshList[GEO_EXPLODER]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* Explodersprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_EXPLODER]);
	Explodersprite->AddAnimation("IDLE", 0, 7);

	meshList[GEO_EXPLODER_LEFT] = MeshBuilder::GenerateSpriteAnimation("Skull Left", 1, 8);
	meshList[GEO_EXPLODER_LEFT]->textureID = LoadTexture("Image//FlameSkull.png", true);
	meshList[GEO_EXPLODER_LEFT]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* ExploderspriteLeft = dynamic_cast<SpriteAnimation*>(meshList[GEO_EXPLODER_LEFT]);
	ExploderspriteLeft->AddAnimation("IDLE", 0, 7);

	meshList[GEO_ENEMYEXPLOSION] = MeshBuilder::GenerateSpriteAnimation("Explosion", 1, 11);
	meshList[GEO_ENEMYEXPLOSION]->textureID = LoadTexture("Image//EnemyExplosion.png", true);
	meshList[GEO_ENEMYEXPLOSION]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* EnemyExplosionsprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_ENEMYEXPLOSION]);
	EnemyExplosionsprite->AddAnimation("Explode", 0, 11);

	// Worm enemy (Boss 2?)

	meshList[GEO_WORMHEAD] = MeshBuilder::GenerateQuad("Worm Head", Color(1, 0, 0), 1.f);
	meshList[GEO_WORMHEAD]->textureID = LoadTGA("Image//worm_head.tga");

	meshList[GEO_WORMBODY1] = MeshBuilder::GenerateQuad("Worm Body 1", Color(1, 0, 0), 1.f);
	meshList[GEO_WORMBODY1]->textureID = LoadTGA("Image//worm_body1.tga");

	meshList[GEO_WORMBODY2] = MeshBuilder::GenerateQuad("Worm Body 2", Color(1, 0, 0), 1.f);
	meshList[GEO_WORMBODY2]->textureID = LoadTGA("Image//worm_body2.tga");


	meshList[GEO_WORMTAIL] = MeshBuilder::GenerateQuad("Worm Tail", Color(1, 0, 0), 1.f);
	meshList[GEO_WORMTAIL]->textureID = LoadTGA("Image//worm_tail.tga");

	// Bullet types
	meshList[GEO_PRIMARY] = MeshBuilder::GenerateQuad("basic bullet", Color(1, 1, 1), 1.f);
	meshList[GEO_PRIMARY]->textureID = LoadTGA("Image//Dagger.tga");

	meshList[GEO_ENEMYBULLET] = MeshBuilder::GenerateQuad("enemy bullet", Color(1, 1, 1), 1.f);
	meshList[GEO_ENEMYBULLET]->textureID = LoadTGA("Image//enemybullet.tga");

	meshList[GEO_CARDS] = MeshBuilder::GenerateQuad("card", Color(1, 1, 1), 1.f);
	meshList[GEO_CARDS]->textureID = LoadTGA("Image//Cards.tga");

	meshList[GEO_BOMB] = MeshBuilder::GenerateQuad("Bomb", Color(1, 1, 1), 1.f);
	meshList[GEO_BOMB]->textureID = LoadTGA("Image//Bomb.tga");

	meshList[GEO_RING] = MeshBuilder::GenerateQuad("ring", Color(1, 1, 1), 1.f);
	meshList[GEO_RING]->textureID = LoadTGA("Image//Perk_Ring.tga");

	meshList[GEO_EXPLOSION] = MeshBuilder::GenerateSphere("explosion", Color(1, 0, 0), 10.f, 20);

	meshList[GEO_RINGAURA] = MeshBuilder::GenerateSphere("Protection", Color(0, 1, 0), 10.f, 20);

	meshList[GEO_MOLOTOV] = MeshBuilder::GenerateQuad("molotov", Color(1, 1, 1), 1.f);
	meshList[GEO_MOLOTOV]->textureID = LoadTGA("Image//Molotov.tga");

	meshList[GEO_MOLOTOV] = MeshBuilder::GenerateQuad("molotov", Color(1, 1, 1), 1.f);
	meshList[GEO_MOLOTOV]->textureID = LoadTGA("Image//Molotov.tga");


	meshList[GEO_ARROW] = MeshBuilder::GenerateQuad("Arrow1", Color(1, 1, 1), 1.f);
	meshList[GEO_ARROW]->textureID = LoadTGA("Image/arrow1.tga");

	meshList[GEO_FLAMINGARROW] = MeshBuilder::GenerateQuad("Arrow2", Color(1, 1, 1), 1.f);
	meshList[GEO_FLAMINGARROW]->textureID = LoadTGA("Image//arrow2.tga");

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//font.tga");
	meshList[GEO_TEXT]->material.kAmbient.Set(1, 0, 0);

	meshList[GEO_UPGRADESCREEN] = MeshBuilder::GenerateQuad("upgrade screen", Color(1, 1, 1), 1.f);
	meshList[GEO_UPGRADESCREEN]->textureID = LoadTGA("Image//upgradescreen.tga");

	meshList[GEO_TRIPLESHOT] = MeshBuilder::GenerateQuad("triple shot", Color(1, 1, 1), 1.f);
	meshList[GEO_TRIPLESHOT]->textureID = LoadTGA("Image//tripleshot.tga");

	//ui menus
	meshList[GEO_MAIN_MENU] = MeshBuilder::GenerateQuad("main menu", Color(1, 1, 1), 1.f);
	meshList[GEO_MAIN_MENU]->textureID = LoadTGA("Image//UI//menuButtons.tga");

	meshList[GEO_GAMEOVER] = MeshBuilder::GenerateQuad("main menu", Color(1, 1, 1), 1.f);
	meshList[GEO_GAMEOVER]->textureID = LoadTGA("Image//UI//gameoverButton.tga");

	meshList[GEO_SELECTOR] = MeshBuilder::GenerateQuad("main menu", Color(1, 1, 1), 1.f);
	meshList[GEO_SELECTOR]->textureID = LoadTGA("Image//UI//selector.tga");

	meshList[GEO_PAUSE] = MeshBuilder::GenerateQuad("main menu", Color(1, 1, 1), 1.f);
	meshList[GEO_PAUSE]->textureID = LoadTGA("Image//UI//resumeButton.tga");

	meshList[GEO_UIBG] = MeshBuilder::GenerateQuad("main menu", Color(1, 1, 1), 1.f);
	meshList[GEO_UIBG]->textureID = LoadTGA("Image//background.tga");

	// Boss Animation
	meshList[GEO_BOSSATTACK] = MeshBuilder::GenerateSpriteAnimation("Boss Attack", 1, 12);
	meshList[GEO_BOSSATTACK]->textureID = LoadTexture("Image//Nightborne_Attack.png", true);
	meshList[GEO_BOSSATTACK]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* NightborneSpriteAttack = dynamic_cast<SpriteAnimation*>(meshList[GEO_BOSSATTACK]);
	NightborneSpriteAttack->AddAnimation("Attack Right", 0, 12);

	meshList[GEO_BOSSATTACK_LEFT] = MeshBuilder::GenerateSpriteAnimation("Boss Attack Left", 1, 12);
	meshList[GEO_BOSSATTACK_LEFT]->textureID = LoadTexture("Image//Nightborne_Attack.png", false);
	meshList[GEO_BOSSATTACK_LEFT]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* NightborneSpriteAttackLeft = dynamic_cast<SpriteAnimation*>(meshList[GEO_BOSSATTACK_LEFT]);
	NightborneSpriteAttackLeft->AddAnimation("Attack Left", 0, 12);

	meshList[GEO_BOSS] = MeshBuilder::GenerateSpriteAnimation("Boss Attack Left", 1, 6);
	meshList[GEO_BOSS]->textureID = LoadTexture("Image//NightBorneMove.png", true);
	meshList[GEO_BOSS]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* NightborneSprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_BOSS]);
	NightborneSprite->AddAnimation("Move Right", 0, 6);


	meshList[GEO_BOSS_LEFT] = MeshBuilder::GenerateSpriteAnimation("Boss Attack Left", 1, 6);
	meshList[GEO_BOSS_LEFT]->textureID = LoadTexture("Image//NightBorneMove.png", false);
	meshList[GEO_BOSS_LEFT]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* NightborneSpriteLeft = dynamic_cast<SpriteAnimation*>(meshList[GEO_BOSS_LEFT]);
	NightborneSpriteLeft->AddAnimation("Move Left", 0, 6);

	// Projectile Animation
	meshList[GEO_LASER] = MeshBuilder::GenerateSpriteAnimation("purple_projectile", 8, 8);
	meshList[GEO_LASER]->textureID = LoadTexture("Image//purple_projectile.png", true);
	meshList[GEO_LASER]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* PurpleShot = dynamic_cast<SpriteAnimation*>(meshList[GEO_LASER]);
	PurpleShot->AddAnimation("Purple Projectile", 0, 60);

	meshList[GEO_EXPLOSION] = MeshBuilder::GenerateSpriteAnimation("Explosion", 1, 11);
	meshList[GEO_EXPLOSION]->textureID = LoadTexture("Image//Explosion.png", true);
	meshList[GEO_EXPLOSION]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* Explosionsprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_EXPLOSION]);
	Explosionsprite->AddAnimation("Explode", 0, 11);

	meshList[GEO_FIRE] = MeshBuilder::GenerateSpriteAnimation("Molotovflame", 1, 5);
	meshList[GEO_FIRE]->textureID = LoadTexture("Image//MolotovFire.png", true);
	meshList[GEO_FIRE]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* Firesprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_FIRE]);
	Firesprite->AddAnimation("Fire", 0, 4);

	meshList[GEO_RINGAURA] = MeshBuilder::GenerateSpriteAnimation("Aura", 2, 5);
	meshList[GEO_RINGAURA]->textureID = LoadTexture("Image//Shield.png", true);
	meshList[GEO_RINGAURA]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* Barriersprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_RINGAURA]);
	Barriersprite->AddAnimation("Aura", 0, 10);


	// Obstacles
	meshList[GEO_TREE] = MeshBuilder::GenerateQuad("Tree", Color(1, 1, 1), 1.f);
	meshList[GEO_TREE]->textureID = LoadTGA("Image//tree.tga");

	meshList[GEO_SWAMP] = MeshBuilder::GenerateQuad("Tree", Color(1, 1, 1), 1.f);
	meshList[GEO_SWAMP]->textureID = LoadTGA("Image//swamp.tga");

	//Extras
	meshList[GEO_CHEST] = MeshBuilder::GenerateSpriteAnimation("Chest", 8, 5);
	meshList[GEO_CHEST]->textureID = LoadTexture("Image//Chests.png", true);
	meshList[GEO_CHEST]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* Chestsprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_CHEST]);
	Chestsprite->AddAnimation("IDLE", 20, 24);
	Chestsprite->AddAnimation("OPEN", 25, 30);

	meshList[GEO_CHEST_PARTICLE] = MeshBuilder::GenerateSpriteAnimation("Chest Particle", 1, 6);
	meshList[GEO_CHEST_PARTICLE]->textureID = LoadTexture("Image//Chest_Particle.png", true);
	meshList[GEO_CHEST_PARTICLE]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* Chestparticlesprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_CHEST_PARTICLE]);
	Chestparticlesprite->AddAnimation("OPEN", 0, 6);



	bLightEnabled = true;
}

void SceneBase::Update(double dt)
{
	//Keyboard Section
	if (Application::IsKeyPressed('1'))
		glEnable(GL_CULL_FACE);
	if (Application::IsKeyPressed('2'))
		glDisable(GL_CULL_FACE);
	if (Application::IsKeyPressed('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (Application::IsKeyPressed('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	fps = (float)(1.f / dt);
}

void SceneBase::RenderText(Mesh* mesh, std::string text, Color color)
{
	if (!mesh || mesh->textureID <= 0)
		return;

	glDisable(GL_DEPTH_TEST);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f, 0, 0); //1.0f is the spacing of each character, you may change this value
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glEnable(GL_DEPTH_TEST);
}

void SceneBase::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y, bool smallerSpacing)
{
	if (!mesh || mesh->textureID <= 0)
		return;

	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, 80, 0, 60, -10, 10);
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity();
	modelStack.PushMatrix();
	modelStack.LoadIdentity();
	modelStack.Translate(x, y, 0);
	modelStack.Scale(size / 1.443, size, size);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		if (smallerSpacing == true)
		{
			if (stoi(text) >= 10)
			{
				characterSpacing.SetToTranslation(i * 0.7f + 0.5f, 0.5f, 0);
			}
			else
			{
				characterSpacing.SetToTranslation(i * 1.0f + 0.5f, 0.5f, 0);
			}
		}
		else
		{
			characterSpacing.SetToTranslation(i * 1.0f + 0.5f, 0.5f, 0); //1.0f is the spacing of each character, you may change this value
		}
		Mtx44 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);

		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	modelStack.PopMatrix();
	viewStack.PopMatrix();
	projectionStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void SceneBase::RenderMesh(Mesh* mesh, bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	if (enableLight && bLightEnabled && mesh->material.kAmbient.r >1)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView = viewStack.Top() * modelStack.Top();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView.a[0]);

		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 2, &mesh->material.kAmbient.g);
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 3, &mesh->material.kAmbient.b);
		//glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		//glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}
	if (mesh->textureID > 0)
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}
	mesh->Render();
	if (mesh->textureID > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void SceneBase::RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey)
{
	glDisable(GL_DEPTH_TEST);
	Mtx44 ortho;
	ortho.SetToOrtho(0, Application::GetWindowWidth() / 10, 0, Application::GetWindowHeight() / 10, -10, 10); //size of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity();

	modelStack.Translate((float)x, (float)y, 0);
	modelStack.Scale((float)sizex, (float)sizey, 1);

	RenderMesh(mesh, false); //UI should not have light
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}
void SceneBase::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SceneBase::RenderMainMenu()
{
	RenderMeshOnScreen(meshList[GEO_UIBG], 96, 55, 200, 110);

	RenderMeshOnScreen(meshList[GEO_MAIN_MENU], 96, 25, 45, 45);
	switch (selectorIndex)
	{
	case 0:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 40, 45, 45);
		break;
	case 1:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 25, 45, 45);
		break;
	}
	RenderTextOnScreen(meshList[GEO_TEXT], "GerryManDering's", Color(1, 1, 1), 7, 1, 45, false);
	RenderTextOnScreen(meshList[GEO_TEXT], "Dungeon", Color(1, 1, 1), 7, 23, 35, false);
	selectorIndex = Math::Clamp(selectorIndex, 0, 1);

}

void SceneBase::RenderCBomb()
{
	RenderMeshOnScreen(meshList[GEO_UIBG], 96, 55, 200, 110);
	RenderMeshOnScreen(meshList[GEO_BOMB], 96, 60, 15, 15);
	RenderMeshOnScreen(meshList[GEO_MAIN_MENU], 96, 25, 45, 45);
	switch (selectorIndex)
	{
	case 0:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 40, 45, 45);
		break;
	case 1:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 25, 45, 45);
		break;
	}
	RenderTextOnScreen(meshList[GEO_TEXT], "BOMBS", Color(1, 1, 1), 7, 25, 45, false);
	selectorIndex = Math::Clamp(selectorIndex, 0, 1);

}
void SceneBase::RenderCArrow()
{
	RenderMeshOnScreen(meshList[GEO_UIBG], 96, 55, 200, 110);
	RenderMeshOnScreen(meshList[GEO_ARROW], 96, 60, 15, 15);
	RenderMeshOnScreen(meshList[GEO_MAIN_MENU], 96, 25, 45, 45);
	switch (selectorIndex)
	{
	case 0:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 40, 45, 45);
		break;
	case 1:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 25, 45, 45);
		break;
	}
	RenderTextOnScreen(meshList[GEO_TEXT], "ARROW", Color(1, 1, 1), 7, 28, 45, false);
	selectorIndex = Math::Clamp(selectorIndex, 0, 1);

}
void SceneBase::RenderCCard()
{
	RenderMeshOnScreen(meshList[GEO_UIBG], 96, 55, 200, 110);
	RenderMeshOnScreen(meshList[GEO_CARDS], 96, 60, 15, 15);
	RenderMeshOnScreen(meshList[GEO_MAIN_MENU], 96, 25, 45, 45);
	switch (selectorIndex)
	{
	case 0:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 40, 45, 45);
		break;
	case 1:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 25, 45, 45);
		break;
	}
	RenderTextOnScreen(meshList[GEO_TEXT], "CARD", Color(1, 1, 1), 7, 31, 45, false);
	selectorIndex = Math::Clamp(selectorIndex, 0, 1);

}
void SceneBase::RenderChoose()
{
	RenderMeshOnScreen(meshList[GEO_UIBG], 96, 55, 200, 110);

	/*RenderMeshOnScreen(meshList[GEO_MAIN_MENU], 96, 25, 45, 45);*/
	switch (selectorIndex)
	{
	case 0:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 70, 45, 45);
		break;
	case 1:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 55, 45, 45);
		break;
	case 2:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 40, 45, 45);
		break;
	case 3:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 25, 45, 45);
		break;
	}
	RenderTextOnScreen(meshList[GEO_TEXT], "Choose Weapon", Color(1, 1, 1), 7, 1, 50, false);
	RenderTextOnScreen(meshList[GEO_TEXT], "BOMB", Color(1, 1, 1), 4, 35, 39.5, false);
	RenderTextOnScreen(meshList[GEO_TEXT], "ARROW", Color(1, 1, 1), 4, 32.5, 30.5, false);
	RenderTextOnScreen(meshList[GEO_TEXT], "CARD", Color(1, 1, 1), 4, 35, 21.5, false);
	RenderTextOnScreen(meshList[GEO_TEXT], "MENU", Color(1, 1, 1), 4, 35, 12.5, false);
	selectorIndex = Math::Clamp(selectorIndex, 0, 3);

}

void SceneBase::RenderGameOver()
{
	RenderMeshOnScreen(meshList[GEO_UIBG], 96, 55, 200, 110);

	RenderMeshOnScreen(meshList[GEO_GAMEOVER], 96, 25, 45, 45);
	switch (selectorIndex)
	{
	case 0:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 41, 45, 45);
		break;
	case 1:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 26, 45, 45);
		break;
	case 2:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 12, 45, 45);
		break;
	}
	RenderTextOnScreen(meshList[GEO_TEXT], "Gameover", Color(1, 1, 1), 7, 23, 35, false);
	selectorIndex = Math::Clamp(selectorIndex, 0, 2);

}
void SceneBase::RenderPauseMenu()
{
	RenderMeshOnScreen(meshList[GEO_UIBG], 96, 55, 200, 110);
	RenderMeshOnScreen(meshList[GEO_PAUSE], 96, 25, 45, 45);

	switch (selectorIndex)
	{
	case 0:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 26, 45, 45);
		break;
	case 1:
		RenderMeshOnScreen(meshList[GEO_SELECTOR], 96, 10, 45, 45);
		break;
	}
	RenderTextOnScreen(meshList[GEO_TEXT], "PAUSE", Color(1, 1, 1), 7, 29, 35, false);
	selectorIndex = Math::Clamp(selectorIndex, 0, 1);
}

void SceneBase::UpdateMainMenu(float& m_speed)
{
	switch (selectorIndex)
	{
	case 0:
		menuType = M_CHOOSE;
		m_speed = 1;
		break;
	case 1:
		Application::gameExit = true;
		break;
	}
}

void SceneBase::UpdateChoose(float& m_speed)
{
	switch (selectorIndex)
	{
	case 0:
		menuType = M_BOMB;
		break;
	case 1:
		menuType = M_ARROW;
		break;
	case 2:
		menuType = M_CARD;
		break;
	case 3:
		menuType = M_MAIN;
		break;
	}
}

void SceneBase::UpdateCBomb(float& m_speed)
{
	switch (selectorIndex)
	{
	case 0:
		menuType = M_NONE;
		cardChoose = false;
		arrowChoose = false;
		bombChoose = true;
		m_speed = 1;
		break;
	case 1:
		selectorIndex = 0;
		menuType = M_CHOOSE;
		break;
	}
}
void SceneBase::UpdateCArrow(float& m_speed)
{
	switch (selectorIndex)
	{
	case 0:
		menuType = M_NONE;
		arrowChoose = true;
		cardChoose = false;
		bombChoose = false;
		m_speed = 1;
		break;
	case 1:
		selectorIndex = 0;
		menuType = M_CHOOSE;
		break;
	}
}
void SceneBase::UpdateCCard(float& m_speed)
{
	switch (selectorIndex)
	{
	case 0:
		menuType = M_NONE;
		cardChoose = true;
		arrowChoose = false;
		bombChoose = false;
		m_speed = 1;
		break;
	case 1:
		selectorIndex = 0;
		menuType = M_CHOOSE;
		break;
	}
}
void SceneBase::UpdateGameOver(float& m_speed)
{
	restartGame = true;
	switch (selectorIndex)
	{
	case 0:
		menuType = M_NONE;
		m_speed = 1;
		break;
	case 1:
		selectorIndex = 0;
		menuType = M_CHOOSE;
		resetGame = true;
		break;
	case 2:
		Application::gameExit = true;
		break;
	}
}

void SceneBase::UpdatePauseMenu(float& m_speed)
{
	switch (selectorIndex)
	{
	case 0:
		menuType = M_NONE;
		m_speed = 1;
		break;
	case 1:
		selectorIndex = 0;
		resetGame = true;
		menuType = M_MAIN;
		break;
	}
}


void SceneBase::Exit()
{
	// Cleanup VBO
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if (meshList[i])
			delete meshList[i];
	}
	glDeleteProgram(m_programID);
	glDeleteVertexArrays(1, &m_vertexArrayID);
}
