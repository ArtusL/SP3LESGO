#include "SceneBase.h"
#include "GL\glew.h"

#include "shader.hpp"
#include "MeshBuilder.h"
#include "Application.h"
#include "Utility.h"
#include "LoadTGA.h"
#include "LoadTexture.h"
#include <sstream>

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

	m_programID = LoadShaders( "Shader//comg.vertexshader", "Shader//comg.fragmentshader" );
	
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

	for(int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = NULL;
	}
	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("reference", 1000, 1000, 1000);
	meshList[GEO_BALL] = MeshBuilder::GenerateSphere("ball", Color(1, 1, 1), 10, 10, 1.f);
	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("cube", Color(1, 1, 1), 2.f);

	meshList[GEO_BACKGROUND] = MeshBuilder::GenerateQuad("background", Color(1, 1, 1),1.f);
	meshList[GEO_BACKGROUND]->textureID = LoadTGA("Image//background.tga");

	meshList[GEO_BOSS] = MeshBuilder::GenerateQuad("boss", Color(1, 1, 1), 1.f);
	meshList[GEO_BOSS]->textureID = LoadTGA("Image//NightBorne.tga");

	meshList[GEO_SHIP] = MeshBuilder::GenerateQuad("ship", Color(1, 1, 1), 1.f);
	meshList[GEO_SHIP]->textureID = LoadTGA("Image//playership2.tga");

	// Health Bar
	meshList[GEO_GREENHEALTH] = MeshBuilder::GenerateQuad("green health", Color(0, 1, 0), 1.f);
	meshList[GEO_REDHEALTH] = MeshBuilder::GenerateQuad("red health", Color(1, 0, 0), 1.f);
	meshList[GEO_PLAYERHEALTH] = MeshBuilder::GenerateQuad("player health", Color(0, 0, 1), 1.f);

	meshList[GEO_HEAL] = MeshBuilder::GenerateQuad("healing item", Color(1, 0, 0), 1.f);
	meshList[GEO_HEAL]->textureID = LoadTGA("Image//heal.tga");

	// Enemy types
	meshList[GEO_ASTEROID] = MeshBuilder::GenerateQuad("asteroid", Color(1, 1, 1), 1.f);
	meshList[GEO_ASTEROID]->textureID = LoadTGA("Image//Goblin.tga");

	meshList[GEO_BIGASTEROID] = MeshBuilder::GenerateQuad("big asteroid", Color(1, 1, 1), 1.f);
	meshList[GEO_BIGASTEROID]->textureID = LoadTGA("Image//Nightmare.tga");

	meshList[GEO_ENEMYSHIP] = MeshBuilder::GenerateQuad("enemy ship", Color(1, 1, 1), 1.f);
	meshList[GEO_ENEMYSHIP]->textureID = LoadTGA("Image//BDemon.tga");

	meshList[GEO_FLAMEDEMON] = MeshBuilder::GenerateQuad("flame demon", Color(1, 1, 1), 1.f);
	meshList[GEO_FLAMEDEMON]->textureID = LoadTGA("Image//FlameDemon.tga");



	// Bullet types
	meshList[GEO_CLUB] = MeshBuilder::GenerateQuad("basic bullet", Color(1, 1, 1), 1.f);
	meshList[GEO_CLUB]->textureID = LoadTGA("Image//Dagger.tga");

	meshList[GEO_ENEMYBULLET] = MeshBuilder::GenerateQuad("enemy bullet", Color(1, 1, 1), 1.f);
	meshList[GEO_ENEMYBULLET]->textureID = LoadTGA("Image//enemybullet.tga");

	meshList[GEO_BOW]= MeshBuilder::GenerateQuad("missle", Color(1, 1, 1), 1.f);
	meshList[GEO_BOW]->textureID = LoadTGA("Image//Gauntlet.tga");

	meshList[GEO_BOMB] = MeshBuilder::GenerateQuad("Bomb", Color(1, 1, 1), 1.f);
	meshList[GEO_BOMB]->textureID = LoadTGA("Image//Bomb.tga");

	meshList[GEO_ARROW] = MeshBuilder::GenerateQuad("Arrow1", Color(1, 1, 1), 1.f);
	meshList[GEO_ARROW]->textureID = LoadTGA("Image/arrow1.tga");

	meshList[GEO_FLAMINGARROW] = MeshBuilder::GenerateQuad("Arrow2", Color(1, 1, 1), 1.f);
	meshList[GEO_FLAMINGARROW]->textureID = LoadTGA("Image//arrow2.tga");

	meshList[GEO_RING] = MeshBuilder::GenerateQuad("ring", Color(1, 1, 1), 1.f);
	meshList[GEO_RING]->textureID = LoadTGA("Image//Perk_Ring.tga");

	meshList[GEO_EXPLOSION] = MeshBuilder::GenerateSphere("explosion", Color(1, 0, 0), 10.f, 20);

	meshList[GEO_RINGAURA] = MeshBuilder::GenerateSphere("Protection", Color(0, 1, 0), 10.f, 20);

	meshList[GEO_MOLOTOV] = MeshBuilder::GenerateQuad("molotov", Color(1, 1, 1), 1.f);
	meshList[GEO_MOLOTOV]->textureID = LoadTGA("Image//Molotov.tga");

	meshList[GEO_MOLOTOV] = MeshBuilder::GenerateQuad("molotov", Color(1, 1, 1), 1.f);
	meshList[GEO_MOLOTOV]->textureID = LoadTGA("Image//Molotov.tga");

	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Image//font.tga");
	meshList[GEO_TEXT]->material.kAmbient.Set(1, 0, 0);

	meshList[GEO_UPGRADESCREEN] = MeshBuilder::GenerateQuad("upgrade screen", Color(1, 1, 1), 1.f);
	meshList[GEO_UPGRADESCREEN]->textureID = LoadTGA("Image//upgradescreen.tga");

	meshList[GEO_TRIPLESHOT] = MeshBuilder::GenerateQuad("triple shot", Color(1, 1, 1), 1.f);
	meshList[GEO_TRIPLESHOT]->textureID = LoadTGA("Image//tripleshot.tga");

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

	meshList[GEO_FDEMON] = MeshBuilder::GenerateSpriteAnimation("Fdemon", 1, 6);
	meshList[GEO_FDEMON]->textureID = LoadTexture("Image//FlameDemon.png", true);
	meshList[GEO_FDEMON]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* Fdemonsprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_FDEMON]);
	Fdemonsprite->AddAnimation("IDLE", 0, 6);

	meshList[GEO_BDEMON] = MeshBuilder::GenerateSpriteAnimation("Bdemon", 1, 6);
	meshList[GEO_BDEMON]->textureID = LoadTexture("Image//Bdemon.png", true);
	meshList[GEO_BDEMON]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* Bdemonsprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_BDEMON]);
	Bdemonsprite->AddAnimation("IDLE", 0, 6);

	meshList[GEO_NIGHTMARE] = MeshBuilder::GenerateSpriteAnimation("Nightmare", 1, 4);
	meshList[GEO_NIGHTMARE]->textureID = LoadTexture("Image//Nightmare.png", true);
	meshList[GEO_NIGHTMARE]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* Nightmaresprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_NIGHTMARE]);
	Nightmaresprite->AddAnimation("IDLE", 0, 4);

	meshList[GEO_GHOST] = MeshBuilder::GenerateSpriteAnimation("Ghost", 1, 7);
	meshList[GEO_GHOST]->textureID = LoadTexture("Image//ghost-idle.png", true);
	meshList[GEO_GHOST]->material.kAmbient.Set(1, 1, 1);
	SpriteAnimation* Ghostsprite = dynamic_cast<SpriteAnimation*>(meshList[GEO_GHOST]);
	Ghostsprite->AddAnimation("IDLE", 0, 7);

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

	bLightEnabled = false;
}

void SceneBase::Update(double dt)
{
	//Keyboard Section
	if(Application::IsKeyPressed('1'))
		glEnable(GL_CULL_FACE);
	if(Application::IsKeyPressed('2'))
		glDisable(GL_CULL_FACE);
	if(Application::IsKeyPressed('3'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if(Application::IsKeyPressed('4'))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	fps = (float)(1.f / dt);
}

void SceneBase::RenderText(Mesh* mesh, std::string text, Color color)
{
	if(!mesh || mesh->textureID <= 0)
		return;
	
	glDisable(GL_DEPTH_TEST);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for(unsigned i = 0; i < text.length(); ++i)
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

void SceneBase::RenderTextOnScreen(Mesh* mesh, std::string text, Color color, float size, float x, float y)
{
	if(!mesh || mesh->textureID <= 0)
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
	modelStack.Scale(size, size, size);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for(unsigned i = 0; i < text.length(); ++i)
	{
		Mtx44 characterSpacing;
		characterSpacing.SetToTranslation(i * 1.0f + 0.5f, 0.5f, 0); //1.0f is the spacing of each character, you may change this value
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

void SceneBase::RenderMesh(Mesh *mesh, bool enableLight)
{
	Mtx44 MVP, modelView, modelView_inverse_transpose;
	
	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, &MVP.a[0]);
	if(enableLight && bLightEnabled)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView = viewStack.Top() * modelStack.Top();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, &modelView.a[0]);
		modelView_inverse_transpose = modelView.GetInverse().GetTranspose();
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, &modelView.a[0]);
		
		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{	
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}
	if(mesh->textureID > 0)
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
	if(mesh->textureID > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void SceneBase::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SceneBase::Exit()
{
	// Cleanup VBO
	for(int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if(meshList[i])
			delete meshList[i];
	}
	glDeleteProgram(m_programID);
	glDeleteVertexArrays(1, &m_vertexArrayID);
}
