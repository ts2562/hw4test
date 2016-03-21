#ifdef _WINDOWS
	#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "Matrix.h"
#include "ShaderProgram.h"
#include "SDL_opengles2.h"
#include "vector"
#include <SDL_mixer.h>
using namespace std;

#ifdef _WINDOWS
	#define RESOURCE_FOLDER ""
#else
	#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;




GLuint LoadTexture(const char *image_path) {
	SDL_Surface *surface = IMG_Load(image_path);
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA,
		GL_UNSIGNED_BYTE, surface->pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	SDL_FreeSurface(surface);
	return textureID;
}	

class SheetSprite {
public:
	SheetSprite();
	SheetSprite(unsigned int textureID1, float u1, float v1, float width1, float height1, float
		size1) : textureID(textureID1), u(u1), v(v1), width(width1), height(height1), size(size1){
	};
	void shoot();
	void Draw(ShaderProgram *p);
	float size;
	unsigned int textureID;
	float u;
	float v;
	float width;
	float height;
	Matrix matrix;
};

class Entity {
public:
	Entity();
	Entity(unsigned int textureID1, float u1, float v1, float width1, float height1, float
		size1, float initP, float mov) : textureID(textureID1), u(u1), v(v1),
		width(width1), height(height1), size(size1), initPos(initP), movement(mov){};
	void Draw(ShaderProgram *p);
	float GetX1();
	float GetX2();
	float GetY();
	float size;
	unsigned int textureID;
	float u;
	float v;
	float width;
	float height;
	float initPos;
	float movement;
	bool isAlive = true;
};

class Bullet {
public:
	Bullet();
	Bullet(unsigned int textureID1, float u1, float v1, float width1, float height1, float
		size1) : textureID(textureID1), u(u1), v(v1), width(width1), height(height1), size(size1){};
	void Draw(ShaderProgram *p);
	float size;
	unsigned int textureID;
	float u;
	float v;
	float width;
	float height;
	bool isShot = false;
	Matrix matrix;
	float x = 0.0;
	float y = 0.0;
};

void SheetSprite::Draw(ShaderProgram *program) {
	glBindTexture(GL_TEXTURE_2D, textureID);
	GLfloat texCoords[] = {
		u, v + height,
		u + width, v,
		u, v,
		u + width, v,
		u, v + height,
		u + width, v + height
	};
	float aspect = width / height;
	float vertices[] = {
		-0.5f * size * aspect, -0.5f * size,
		0.5f * size * aspect, 0.5f * size,
		-0.5f * size * aspect, 0.5f * size,
		0.5f * size * aspect, 0.5f * size,
		-0.5f * size * aspect, -0.5f * size,
		0.5f * size * aspect, -0.5f * size };
	// draw our arrays
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::Draw(ShaderProgram *program) {
	glBindTexture(GL_TEXTURE_2D, textureID);
	GLfloat texCoords[] = {
		u, v + height,
		u + width, v,
		u, v,
		u + width, v,
		u, v + height,
		u + width, v + height
	};
	float aspect = width / height;
	float vertices[] = {
		-0.5f * size * aspect - 2.0f + initPos + movement, -0.5f * size + 1.75f,
		0.5f * size * aspect - 2.0f + initPos + movement, 0.5f * size + 1.75f,
		-0.5f * size * aspect - 2.0f + initPos + movement, 0.5f * size + 1.75f,
		0.5f * size * aspect - 2.0f + initPos + movement, 0.5f * size + 1.75f,
		-0.5f * size * aspect - 2.0f + initPos + movement, -0.5f * size + 1.75f,
		0.5f * size * aspect - 2.0f + initPos + movement, -0.5f * size + 1.75f };
	// draw our arrays
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

float Entity::GetX1(){
	float aspect = width / height;
	return -0.5f * size * aspect - 2.0f + initPos + movement;
}

float Entity::GetX2(){
	float aspect = width / height;
	return 0.5f * size * aspect - 2.0f + initPos + movement;
}

float Entity::GetY(){
	return -0.5f * size + 1.75f;
}

void Bullet::Draw(ShaderProgram *program) {
	glBindTexture(GL_TEXTURE_2D, textureID);
	GLfloat texCoords[] = {
		u, v + height,
		u + width, v,
		u, v,
		u + width, v,
		u, v + height,
		u + width, v + height
	};
	float aspect = width / height;
	float vertices[] = {
		-0.5f * size * aspect, -0.5f * size,
		0.5f * size * aspect, 0.5f * size,
		-0.5f * size * aspect, 0.5f * size,
		0.5f * size * aspect, 0.5f * size,
		-0.5f * size * aspect, -0.5f * size,
		0.5f * size * aspect, -0.5f * size };
	// draw our arrays
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void DrawText(ShaderProgram *program, int fontTexture, std::string text, float size, float spacing) {
	float texture_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (int i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		vertexData.insert(vertexData.end(), {
			((size + spacing) * i) + (-0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
		});
		texCoordData.insert(texCoordData.end(), {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size,
		});
	}
	glUseProgram(program->programID);
	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glDrawArrays(GL_TRIANGLES, 0, text.size() * 6);
	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

enum GameState {STATE_MAIN_MENU, STATE_GAME_LEVEL};
int state;
void renderMenu(){
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	SDL_Event event;
	bool done = false;
#ifdef _WINDOWS
	glewInit();
#endif

	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;
	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	const Uint8 *key = SDL_GetKeyboardState(NULL);
	if (key[SDL_SCANCODE_SPACE]){
		state = STATE_GAME_LEVEL;
	}
	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	glUseProgram(program.programID);

	GLuint font = LoadTexture("font2.png");
	string start = "SPACE INVADERS-CLOSE TO START";
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE){
				done = true;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT);
		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
		SDL_GL_SwapWindow(displayWindow);

		DrawText(&program, font, start, 0.3, 1.0);
	}

	SDL_Quit();
}

void renderGame(){
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 360, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	SDL_Event event;
	bool done = false;
#ifdef _WINDOWS
	glewInit();
#endif

	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;
	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	const Uint8 *keys = SDL_GetKeyboardState(NULL);
	GLuint spriteSheetTexture = LoadTexture("sheet.png");
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	Mix_Chunk *player_shoot;
	player_shoot = Mix_LoadWAV("player_shoot.wav");
	Mix_Chunk *enemy_destroy;
	enemy_destroy = Mix_LoadWAV("enemy_destroy.wav");
	Mix_Music *music;
	music = Mix_LoadMUS("NAFL.mp3");
	Entity enemySprite = Entity(spriteSheetTexture, (425.0f / 1024.0f), (384.0f / 1024.0f), (93.0f / 1024.0f), (84.0f / 1024.0f), .3, 0, 0);
	Entity enemySprite2 = Entity(spriteSheetTexture, (425.0f / 1024.0f), (384.0f / 1024.0f), (93.0f / 1024.0f), (84.0f / 1024.0f), .3, 0, 0);
	Entity enemySprite3 = Entity(spriteSheetTexture, (425.0f / 1024.0f), (384.0f / 1024.0f), (93.0f / 1024.0f), (84.0f / 1024.0f), .3, 0, 0);
	Entity enemySprite4 = Entity(spriteSheetTexture, (425.0f / 1024.0f), (384.0f / 1024.0f), (93.0f / 1024.0f), (84.0f / 1024.0f), .3, 0, 0);
	Entity enemySprite5 = Entity(spriteSheetTexture, (425.0f / 1024.0f), (384.0f / 1024.0f), (93.0f / 1024.0f), (84.0f / 1024.0f), .3, 0, 0);

	SheetSprite mySprite = SheetSprite(spriteSheetTexture, (224.0f / 1024.0f), (832.0f / 1024.0f),
		(99.0f / 1024.0f), (75.0f / 1024.0f), .3);

	Bullet myBullet = Bullet(spriteSheetTexture, (856.0f / 1024.0f), (421.0f / 1024.0f),
		(9.0f / 1024.0f), (51.0f / 1024.0f), .3);
	vector<Entity> enemies;
	enemies.push_back(enemySprite);
	enemies.push_back(enemySprite2);
	enemies.push_back(enemySprite3);
	enemies.push_back(enemySprite4);
	enemies.push_back(enemySprite5);

	projectionMatrix.setOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	glUseProgram(program.programID);
	float lastFrameTicks = 0.0f;
	double movement = 0.0;
	double move = 0.0;
	double enemyMovement = 0.0;

	bool canShoot = true;
	bool turn = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT);
		program.setModelMatrix(modelMatrix);
		program.setProjectionMatrix(projectionMatrix);
		program.setViewMatrix(viewMatrix);
		glBindTexture(GL_TEXTURE_2D, spriteSheetTexture);
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		Mix_PlayMusic(music, 0);
		if ((enemyMovement * 0.1) > 1){
			turn = true;
		}
		if ((enemyMovement * 0.1) < -0.75){
			turn = false;
		}
		if (turn == false){
			enemyMovement = enemyMovement + elapsed;
		}
		else if (turn == true){
			enemyMovement = enemyMovement - elapsed;
		}

		if (keys[SDL_SCANCODE_A] && myBullet.isShot == false) {
			move -= elapsed;
		}
		else if (keys[SDL_SCANCODE_D] && myBullet.isShot == false) {
			move += elapsed;
		}

		if (keys[SDL_SCANCODE_S]){
			myBullet.isShot = true;
			if (myBullet.isShot == true)
			{
				Mix_PlayChannel(-1, player_shoot, 0);
			}
		}

		myBullet.x = move;
		myBullet.y = movement;
		for (int i = 0; i < enemies.size(); i++)
		{
			if (enemies[i].isAlive == true){
				enemies[i].initPos = (float)i;
				enemies[i].movement = (float)(enemyMovement * 0.1);
				enemies[i].Draw(&program);
			}
			if (myBullet.x >= enemies[i].GetX1() && myBullet.x <= enemies[i].GetX2()){
				if (myBullet.y >= enemies[i].GetY()){
					enemies[i].isAlive = false;
						Mix_PlayChannel(-1, enemy_destroy, 0);
				}
			}
		}

		if (myBullet.isShot == false){
			program.setModelMatrix(mySprite.matrix);
			movement = 0.0;
		}
		modelMatrix.identity();
		mySprite.matrix.identity();
		mySprite.matrix.Translate(move, 0.0, 0.0);
		mySprite.Draw(&program);
		if (myBullet.isShot == true){
			movement = movement + elapsed;
			program.setModelMatrix(myBullet.matrix);
			myBullet.matrix.identity();
			myBullet.matrix.Translate(move, movement, 0.0);
			myBullet.Draw(&program);
			if (movement >= 2.5){
				myBullet.isShot = false;
			}
		}
		SDL_GL_SwapWindow(displayWindow);
	}
	Mix_FreeChunk(player_shoot);
	Mix_FreeChunk(enemy_destroy);
	Mix_FreeMusic(music);
	SDL_Quit();
}

void render(){
	const Uint8 *key = SDL_GetKeyboardState(NULL);
	if (key[SDL_SCANCODE_SPACE]){
		state = STATE_GAME_LEVEL;
	}
	switch (state){
	case STATE_MAIN_MENU:
		renderMenu();
	case STATE_GAME_LEVEL:
		renderGame();
	break;
	}
}
int main(int argc, char *argv[])
{
	render(); 
	return 0;
}
