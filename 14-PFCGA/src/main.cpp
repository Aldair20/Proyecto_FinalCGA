#define _USE_MATH_DEFINES
#include <cmath>
//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

// contains new std::shuffle definition
#include <algorithm>
#include <random>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"

// Shader include
#include "Headers/Shader.h"

// Model geometric includes
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"
#include "Headers/ThirdPersonCamera.h"

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Headers/Texture.h"

// Include loader Model class
#include "Headers/Model.h"

// Include Terrain
#include "Headers/Terrain.h"

#include "Headers/AnimationUtils.h"

// Include Colision headers functions
#include "Headers/Colisiones.h"

// OpenAL include
#include <AL/alut.h>

// Renderizado de letras
#include <Headers/FontTypeRendering.h>

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

int screenWidth;
int screenHeight;

double lastDamageTime = 0.0;
double damageCooldown = 1.0; // Tiempo entre reduccion de vida

const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

GLFWwindow *window;

Shader shader;
//Shader con skybox
Shader shaderSkybox;
//Shader con multiples luces
Shader shaderMulLighting;
//Shader para el terreno
Shader shaderTerrain;
//Shader para las particulas de fuego
Shader shaderParticlesFire;
//Shader para visualizar el buffer de profundidad
Shader shaderViewDepth;
//Shader para dibujar el buffer de profunidad
Shader shaderDepth;
//Shader texto
Shader shaderTexture;

std::shared_ptr<Camera> camera(new ThirdPersonCamera());
float distanceFromTarget = 7.0;

Sphere skyboxSphere(20, 20);
Box boxCollider;
Sphere sphereCollider(10, 10);
Box boxViewDepth;
Box boxLightViewBox;

//Modelos de proyecto
Model modelRavennaAnimate;

Model modelWolfBody;
Model modelWolfTail;

Model modelBatBody;
Model modelBatHead;
Model modelBatLwing;
Model modelBatRwing;

Model modelGrave1;
Model modelGrave2;
Model modelGrave3;
Model modelGrave4;
Model modelGrave5;
Model modelTree;

Model modelKael;

Model modelMonsterBody;
Model modelMonsterLarm;
Model modelMonsterRarm;

Box boxIntro;

// Variables para la rotacion oscilante
float wolfRotationAngle = 0.0f;     // Angulo de rotacion del lobo en el eje X
float wolfRotationSpeed = 140.0f;    // Velocidad de rotacion en grados por segundo

float monsterRotationAngle = 0.0f;  // Angulo de rotacion del monstruo en el eje Z
float monsterRotationSpeed = 100.0f; // Velocidad de rotacion en grados por segundo

Terrain terrain(-1, -1, 130, 13, "../Textures/MapaAlturasProyecto.png");

GLuint textureTerrainBackgroundID, textureTerrainRID, textureTerrainGID, textureTerrainBID, textureTerrainBlendMapID, texId;
GLuint skyboxTextureID;

// Modelo renderizado texto
FontTypeRendering::FontTypeRendering *modelText;

//Box de textura
GLuint textureInit1ID, textureInit2ID, textureActivaID, textureScreen3ID, textureScreen2ID, textureScreen1ID,textureScreen0ID, textureEndGameID;

bool iniciaPartida = false, presionarOpcion = false;

GLenum types[6] = {
GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

std::string fileNames[6] = { "../Textures/ulukai/corona_ft.png",
		"../Textures/ulukai/corona_bk.png",
		"../Textures/ulukai/corona_up.png",
		"../Textures/ulukai/corona_dn.png",
		"../Textures/ulukai/corona_rt.png",
		"../Textures/ulukai/corona_lf.png" };

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;

// Model matrix definitions
glm::mat4 modelMatrixRavenna = glm::mat4(1.0f);
glm::mat4 modelMatrixWolf = glm::mat4(1.0f);
glm::mat4 modelMatrixBat = glm::mat4(1.0f);
glm::mat4 modelMatrixGrave1 = glm::mat4(1.0f);
glm::mat4 modelMatrixGrave2 = glm::mat4(1.0f);
glm::mat4 modelMatrixGrave3 = glm::mat4(1.0f);
glm::mat4 modelMatrixGrave4 = glm::mat4(1.0f);
glm::mat4 modelMatrixGrave5 = glm::mat4(1.0f);
glm::mat4 modelMatrixTree = glm::mat4(1.0f);
glm::mat4 modelMatrixMonster = glm::mat4(1.0f);

int animationIndex = 0;
int modelSelected = 0;
bool enableCountSelected = true;
float timerCountdown = 60.0f;

// Variables to animations keyframes
bool saveFrame = false, availableSave = true;
std::ofstream myfile;
std::string fileName = "";
bool record = false;

// Graves y arbol positions

int randomInRange(int min, int max) {
    return min + rand() % (max - min + 1);
}

std::vector<glm::vec3> grave1Position = {
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)), //min x, -25, max x = 90
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),	// min z, -27, max z = 75
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75))
};
std::vector<float> grave1Orientation = {
	-10.0, -75.0, 20.0
};
std::vector<glm::vec3> grave2Position = {
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)), //min x, -25, max x = 90
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),	// min z, -27, max z = 75
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75))
};
std::vector<float> grave2Orientation = {
	-10.0, -75.0, 20.0
};
std::vector<glm::vec3> grave3Position = {
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)), //min x, -25, max x = 90
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),	// min z, -27, max z = 75
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75))
};
std::vector<float> grave3Orientation = {
	-10.0, -75.0, 20.0
};
std::vector<glm::vec3> grave4Position = {
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)), //min x, -25, max x = 90
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),	// min z, -27, max z = 75
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75))
};
std::vector<float> grave4Orientation = {
	-10.0, -75.0, 20.0
};
std::vector<glm::vec3> grave5Position = {
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)), //min x, -25, max x = 90
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),	// min z, -27, max z = 75
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75))
};
std::vector<float> grave5Orientation = {
	-10.0, -75.0, 20.0
};

std::vector<glm::vec3> treePosition = {
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)), //min x, -25, max x = 90
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),	// min z, -27, max z = 75
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75)),
	glm::vec3(randomInRange(-25, 90), 0, randomInRange(-27, 75))
};
std::vector<float> treeOrientation = {
	-10.0, -75.0, 20.0
};

double deltaTime;
double currTime, lastTime;

// Jump variables
bool isJump = false;
float GRAVITY = 2.81;
double tmv = 0;
double startTimeJump = 0;
//Proyecto Variables
float timeMonster = 0.0f;
//MonsterAnimation
float scaleY = 0.0f;
//BatANIMATION
float timeBat = 0.0f;
float floatOffset = 0.0f;
float wingAngle = 0.0f;
//Wolf animation
float timeWolf = 0.0f;
float scaleYMonster = 0.0f;
float tailAngle = 0.0f;

// Variables de velocidad para cada monstruo
float speedMonster = 0.0f;  // Velocidad del monstruo principal
float speedWolf = 0.0f;     // Velocidad del lobo
float speedBat = 0.0f;      // Velocidad del murciélago

float acelMonster = 0.0f;
float acelWolf = 0.0f;
float acelBat = 0.0f;

// Definition for the particle system
GLuint initVel, startTime;
GLuint VAOParticles;
GLuint nParticles = 8000;
double currTimeParticlesAnimation, lastTimeParticlesAnimation;

// Definition for the particle system fire
GLuint initVelFire, startTimeFire;
GLuint VAOParticlesFire;
GLuint nParticlesFire = 2000;
GLuint posBuf[2], velBuf[2], age[2];
GLuint particleArray[2];
GLuint feedback[2];
GLuint drawBuf = 1;
float particleSize = 0.5, particleLifetime = 3.0;
double currTimeParticlesAnimationFire, lastTimeParticlesAnimationFire;

// Colliders
std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > collidersOBB;
std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> > collidersSBB;

// Framesbuffers
GLuint depthMap, depthMapFBO;

/**********************
 * OpenAL config
 */

// OpenAL Defines
#define NUM_BUFFERS 3
#define NUM_SOURCES 3
#define NUM_ENVIRONMENTS 1
// Listener
ALfloat listenerPos[] = { 0.0, 0.0, 4.0 };
ALfloat listenerVel[] = { 0.0, 0.0, 0.0 };
ALfloat listenerOri[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };
// Source 0
ALfloat source0Pos[] = { -2.0, 0.0, 0.0 };
ALfloat source0Vel[] = { 0.0, 0.0, 0.0 };
// Source 1
ALfloat source1Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source1Vel[] = { 0.0, 0.0, 0.0 };
// Source 2
ALfloat source2Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source2Vel[] = { 0.0, 0.0, 0.0 };
// Buffers
ALuint buffer[NUM_BUFFERS];
ALuint source[NUM_SOURCES];
ALuint environment[NUM_ENVIRONMENTS];
// Configs
ALsizei size, freq;
ALenum format;
ALvoid *data;
int ch;
ALboolean loop;
std::vector<bool> sourcesPlay = {true, true, true};

void moveTowards(glm::mat4 &monsterMatrix, glm::vec3 targetPosition, float speed) {
    glm::vec3 monsterPosition = glm::vec3(monsterMatrix[3]);
    glm::vec3 direction = glm::normalize(targetPosition - monsterPosition);

    // Evitar divisiones por cero al normalizar el vector
    if (glm::length(direction) > 0.01f) {
        glm::vec3 newPosition = monsterPosition + direction * speed * (float)deltaTime;
        monsterMatrix = glm::translate(glm::mat4(1.0f), newPosition);
    }
}

void checkCollisionWithMonsters(glm::vec3 ravennaPosition, glm::vec3 monsterPosition, int &vidaRavenna, double currTime, double &lastDamageTime) {
    float distance = glm::length(monsterPosition - ravennaPosition);
    if (distance <= 1.0f && vidaRavenna > 0 && (currTime - lastDamageTime) >= damageCooldown) {
        vidaRavenna = std::max(vidaRavenna - 1, 0);
        lastDamageTime = currTime; // Actualizar el tiempo del ultimo daño
		std::cout << "Ravenna ha sido alcanzada. Vida restante: " << vidaRavenna << std::endl;
    }
}

void rotateTowards(glm::mat4 &monsterMatrix, glm::vec3 targetPosition) {
    glm::vec3 monsterPosition = glm::vec3(monsterMatrix[3]);
    glm::vec3 direction = glm::normalize(targetPosition - monsterPosition);

    if (glm::length(direction) > 0.01f) {
        float angle = atan2(direction.x, direction.z);
        monsterMatrix = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
        monsterMatrix[3] = glm::vec4(monsterPosition, 1.0f); // Mantener posicion original
    }
}

void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow *window, int key, int scancode, int action,
		int mode);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void initParticleBuffers();
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroy();
bool processInput(bool continueApplication = true);
void prepareScene();
void prepareDepthScene();
void renderScene(bool renderParticles = true);

void initParticleBuffers() {
	// Generate the buffers
	glGenBuffers(1, &initVel);   // Initial velocity buffer
	glGenBuffers(1, &startTime); // Start time buffer

	// Allocate space for all buffers
	int size = nParticles * 3 * sizeof(float);
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), NULL, GL_STATIC_DRAW);

	// Fill the first velocity buffer with random velocities
	glm::vec3 v(0.0f);
	float velocity, theta, phi;
	GLfloat *data = new GLfloat[nParticles * 3];
	for (unsigned int i = 0; i < nParticles; i++) {

		theta = glm::mix(0.0f, glm::pi<float>() / 6.0f, ((float)rand() / RAND_MAX));
		phi = glm::mix(0.0f, glm::two_pi<float>(), ((float)rand() / RAND_MAX));

		v.x = sinf(theta) * cosf(phi);
		v.y = cosf(theta);
		v.z = sinf(theta) * sinf(phi);

		velocity = glm::mix(0.6f, 0.8f, ((float)rand() / RAND_MAX));
		v = glm::normalize(v) * velocity;

		data[3 * i] = v.x;
		data[3 * i + 1] = v.y;
		data[3 * i + 2] = v.z;
	}
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

	// Fill the start time buffer
	delete[] data;
	data = new GLfloat[nParticles];
	float time = 0.0f;
	float rate = 0.00075f;
	for (unsigned int i = 0; i < nParticles; i++) {
		data[i] = time;
		time += rate;
	}
	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] data;

	glGenVertexArrays(1, &VAOParticles);
	glBindVertexArray(VAOParticles);
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void initParticleBuffersFire() {
	// Generate the buffers
	glGenBuffers(2, posBuf);    // position buffers
	glGenBuffers(2, velBuf);    // velocity buffers
	glGenBuffers(2, age);       // age buffers

	// Allocate space for all buffers
	int size = nParticlesFire * sizeof(GLfloat);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, age[1]);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);

	// Fill the first age buffer
	std::vector<GLfloat> initialAge(nParticlesFire);
	float rate = particleLifetime / nParticlesFire;
	for(unsigned int i = 0; i < nParticlesFire; i++ ) {
		int index = i - nParticlesFire;
		float result = rate * index;
		initialAge[i] = result;
	}
	// Shuffle them for better looking results
	//Random::shuffle(initialAge);
	auto rng = std::default_random_engine {};
	std::shuffle(initialAge.begin(), initialAge.end(), rng);
	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, initialAge.data());

	glBindBuffer(GL_ARRAY_BUFFER,0);

	// Create vertex arrays for each set of buffers
	glGenVertexArrays(2, particleArray);

	// Set up particle array 0
	glBindVertexArray(particleArray[0]);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// Set up particle array 1
	glBindVertexArray(particleArray[1]);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, age[1]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	// Setup the feedback objects
	glGenTransformFeedbacks(2, feedback);

	// Transform feedback 0
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[0]);

	// Transform feedback 1
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[1]);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
				glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
				nullptr);

	if (window == nullptr) {
		std::cerr
				<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
				<< std::endl;
		destroy();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Inicialización de los shaders
	shader.initialize("../Shaders/colorShader.vs", "../Shaders/colorShader.fs");
	shaderSkybox.initialize("../Shaders/skyBox.vs", "../Shaders/skyBox_fog.fs");
	shaderMulLighting.initialize("../Shaders/iluminacion_textura_animation_shadow.vs", "../Shaders/multipleLights_shadow.fs");
	shaderTerrain.initialize("../Shaders/terrain_shadow.vs", "../Shaders/terrain_shadow.fs");
	//shaderParticlesFountain.initialize("../Shaders/particlesFountain.vs", "../Shaders/particlesFountain.fs");
	shaderParticlesFire.initialize("../Shaders/particlesFire.vs", "../Shaders/particlesFire.fs", {"Position", "Velocity", "Age"});
	shaderViewDepth.initialize("../Shaders/texturizado.vs", "../Shaders/texturizado_depth_view.fs");
	shaderDepth.initialize("../Shaders/shadow_mapping_depth.vs", "../Shaders/shadow_mapping_depth.fs");
	shaderTexture.initialize("../Shaders/texturizado.vs", "../Shaders/texturizado.fs");

	// Inicializacion de los objetos.
	skyboxSphere.init();
	skyboxSphere.setShader(&shaderSkybox);
	skyboxSphere.setScale(glm::vec3(100.0f, 100.0f, 100.0f));

	boxCollider.init();
	boxCollider.setShader(&shader);
	boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	sphereCollider.init();
	sphereCollider.setShader(&shader);
	sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	boxViewDepth.init();
	boxViewDepth.setShader(&shaderViewDepth);

	boxLightViewBox.init();
	boxLightViewBox.setShader(&shaderViewDepth);

	terrain.init();
	terrain.setShader(&shaderTerrain);
	terrain.setPosition(glm::vec3(100, 0, 100));

	//Modelo de render text
	modelText = new FontTypeRendering::FontTypeRendering(screenWidth, screenHeight);
	modelText->Initialize();

	//textura de caja
	boxIntro.init();
	boxIntro.setShader(&shaderTexture);
	boxIntro.setScale(glm::vec3(2.0, 2.0, 1.0));

	modelRavennaAnimate.loadModel("../models/ModelosProyecto/Ravenna/ravennaV1.fbx");
	modelRavennaAnimate.setShader(&shaderMulLighting);
	
	modelWolfBody.loadModel("../models/ModelosProyecto/Wolf/WolfBody.obj");
	modelWolfBody.setShader(&shaderMulLighting);
	modelWolfTail.loadModel("../models/ModelosProyecto/Wolf/WolfTail.obj");
	modelWolfTail.setShader(&shaderMulLighting);

	modelBatBody.loadModel("../models/ModelosProyecto/Bat/BatBody.obj");
	modelBatBody.setShader(&shaderMulLighting);
	modelBatHead.loadModel("../models/ModelosProyecto/Bat/BatHead.obj");
	modelBatHead.setShader(&shaderMulLighting);
	modelBatLwing.loadModel("../models/ModelosProyecto/Bat/BatLwing.obj");
	modelBatLwing.setShader(&shaderMulLighting);
	modelBatRwing.loadModel("../models/ModelosProyecto/Bat/BatRwing.obj");
	modelBatRwing.setShader(&shaderMulLighting);

	modelGrave1.loadModel("../models/ModelosProyecto/Grave/Grave1.obj");
	modelGrave1.setShader(&shaderMulLighting);
	modelGrave2.loadModel("../models/ModelosProyecto/Grave/Grave2.obj");
	modelGrave2.setShader(&shaderMulLighting);
	modelGrave3.loadModel("../models/ModelosProyecto/Grave/Grave3.obj");
	modelGrave3.setShader(&shaderMulLighting);
	modelGrave4.loadModel("../models/ModelosProyecto/Grave/Grave4.obj");
	modelGrave4.setShader(&shaderMulLighting);
	modelGrave5.loadModel("../models/ModelosProyecto/Grave/Grave5.obj");
	modelGrave5.setShader(&shaderMulLighting);
	modelTree.loadModel("../models/ModelosProyecto/Grave/Tree.obj");
	modelTree.setShader(&shaderMulLighting);

	modelMonsterBody.loadModel("../models/ModelosProyecto/Monster/MonsterBody.obj");
	modelMonsterBody.setShader(&shaderMulLighting);
	modelMonsterLarm.loadModel("../models/ModelosProyecto/Monster/MonsterARMLEFT.obj");
	modelMonsterLarm.setShader(&shaderMulLighting);
	modelMonsterRarm.loadModel("../models/ModelosProyecto/Monster/MonsterARMRIGTH.obj");
	modelMonsterRarm.setShader(&shaderMulLighting);
	
	camera->setPosition(glm::vec3(0.0, 0.0, 10.0));
	camera->setDistanceFromTarget(distanceFromTarget);
	camera->setSensitivity(1.0);

	// Carga de texturas para el skybox
	Texture skyboxTexture = Texture("");
	glGenTextures(1, &skyboxTextureID);
	// Tipo de textura CUBE MAP
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		skyboxTexture = Texture(fileNames[i]);
		skyboxTexture.loadImage(true);
		if (skyboxTexture.getData()) {
			glTexImage2D(types[i], 0, GL_RGB, skyboxTexture.getWidth(), skyboxTexture.getHeight(), 0,
			GL_RGB, GL_UNSIGNED_BYTE, skyboxTexture.getData());
		} else
			std::cout << "Failed to load texture" << std::endl;
		skyboxTexture.freeImage();
	}

	// Definiendo la textura a utilizar
	Texture textureTerrainBackground("../Textures/PROY_darkgrass.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	textureTerrainBackground.loadImage();
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBackgroundID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBackgroundID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (textureTerrainBackground.getData()) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureTerrainBackground.getWidth(), textureTerrainBackground.getHeight(), 0,
		GL_RGB, GL_UNSIGNED_BYTE, textureTerrainBackground.getData());
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainBackground.freeImage();

	// Definiendo la textura a utilizar
	Texture textureTerrainR("../Textures/PROY_spooky.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	textureTerrainR.loadImage();
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainRID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainRID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (textureTerrainR.getData()) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos 
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureTerrainR.getWidth(), textureTerrainR.getHeight(), 0,
		GL_RGBA, GL_UNSIGNED_BYTE, textureTerrainR.getData());
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainR.freeImage();

	// Definiendo la textura a utilizar
	Texture textureTerrainG("../Textures/PROY_sueloroca.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	textureTerrainG.loadImage();
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainGID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainGID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (textureTerrainG.getData()) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, textureTerrainG.getWidth(), textureTerrainG.getHeight(), 0,
		GL_RGB, GL_UNSIGNED_BYTE, textureTerrainG.getData());
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainG.freeImage();

	// Definiendo la textura a utilizar
	Texture textureTerrainB("../Textures/PROY_pantano.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	textureTerrainB.loadImage();
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (textureTerrainB.getData()) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureTerrainB.getWidth(), textureTerrainB.getHeight(), 0,
		GL_RGBA, GL_UNSIGNED_BYTE, textureTerrainB.getData());
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainB.freeImage();

	// Definiendo la textura a utilizar
	Texture textureTerrainBlendMap("../Textures/blendMapProyecto.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	textureTerrainBlendMap.loadImage(true);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBlendMapID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (textureTerrainBlendMap.getData()) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureTerrainBlendMap.getWidth(), textureTerrainBlendMap.getHeight(), 0,
		GL_RGBA, GL_UNSIGNED_BYTE, textureTerrainBlendMap.getData());
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainBlendMap.freeImage();

	// Definiendo la textura
	Texture textureIntro1("../Textures/proy_brick1.png");
	textureIntro1.loadImage(); // Cargar la textura
	glGenTextures(1, &textureInit1ID); // Creando el id de la textura del landingpad
	glBindTexture(GL_TEXTURE_2D, textureInit1ID); // Se enlaza la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Wrapping en el eje u
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Wrapping en el eje v
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtering de minimización
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Filtering de maximimizacion
	if(textureIntro1.getData()){
		// Transferir los datos de la imagen a la tarjeta
		glTexImage2D(GL_TEXTURE_2D, 0, textureIntro1.getChannels() == 3 ? GL_RGB : GL_RGBA, textureIntro1.getWidth(), textureIntro1.getHeight(), 0,
		textureIntro1.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureIntro1.getData());
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else 
		std::cout << "Fallo la carga de textura" << std::endl;
	textureIntro1.freeImage(); // Liberamos memoria

	// Definiendo la textura
	Texture textureIntro2("../Textures/proy_brick2.png");
	textureIntro2.loadImage(); // Cargar la textura
	glGenTextures(1, &textureInit2ID); // Creando el id de la textura del landingpad
	glBindTexture(GL_TEXTURE_2D, textureInit2ID); // Se enlaza la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Wrapping en el eje u
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Wrapping en el eje v
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtering de minimización
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Filtering de maximimizacion
	if(textureIntro2.getData()){
		// Transferir los datos de la imagen a la tarjeta
		glTexImage2D(GL_TEXTURE_2D, 0, textureIntro2.getChannels() == 3 ? GL_RGB : GL_RGBA, textureIntro2.getWidth(), textureIntro2.getHeight(), 0,
		textureIntro2.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureIntro2.getData());
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else 
		std::cout << "Fallo la carga de textura" << std::endl;
	textureIntro2.freeImage(); // Liberamos memoria

	// Definiendo la textura
	Texture textureScreen("../Textures/screen_vida3.png");
	textureScreen.loadImage(); // Cargar la textura
	glGenTextures(1, &textureScreen3ID); // Creando el id de la textura del landingpad
	glBindTexture(GL_TEXTURE_2D, textureScreen3ID); // Se enlaza la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Wrapping en el eje u
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Wrapping en el eje v
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtering de minimización
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Filtering de maximimizacion
	if(textureScreen.getData()){
		// Transferir los datos de la imagen a la tarjeta
		glTexImage2D(GL_TEXTURE_2D, 0, textureScreen.getChannels() == 3 ? GL_RGB : GL_RGBA, textureScreen.getWidth(), textureScreen.getHeight(), 0,
		textureScreen.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureScreen.getData());
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else 
		std::cout << "Fallo la carga de textura" << std::endl;
	textureScreen.freeImage(); // Liberamos memoria
	
	// Definiendo la textura para "screen_vida2"
	Texture textureScreen2("../Textures/screen_vida2.png");
	textureScreen2.loadImage(); // Cargar la textura
	glGenTextures(1, &textureScreen2ID); // Creando el ID de la textura
	glBindTexture(GL_TEXTURE_2D, textureScreen2ID); // Enlazar la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (textureScreen2.getData()) {
		glTexImage2D(GL_TEXTURE_2D, 0, textureScreen2.getChannels() == 3 ? GL_RGB : GL_RGBA, textureScreen2.getWidth(), textureScreen2.getHeight(), 0,
					textureScreen2.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureScreen2.getData());
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Fallo la carga de textura para screen_vida2" << std::endl;
	}
	textureScreen2.freeImage();

	// Definiendo la textura para "screen_vida1"
	Texture textureScreen1("../Textures/screen_vida1.png");
	textureScreen1.loadImage(); // Cargar la textura
	glGenTextures(1, &textureScreen1ID); // Creando el ID de la textura
	glBindTexture(GL_TEXTURE_2D, textureScreen1ID); // Enlazar la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (textureScreen1.getData()) {
		glTexImage2D(GL_TEXTURE_2D, 0, textureScreen1.getChannels() == 3 ? GL_RGB : GL_RGBA, textureScreen1.getWidth(), textureScreen1.getHeight(), 0,
					textureScreen1.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureScreen1.getData());
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Fallo la carga de textura para screen_vida1" << std::endl;
	}
	textureScreen1.freeImage();

	// Definiendo la textura para "screen_vida1"
	Texture textureScreen0("../Textures/proy_brickFin.png");
	textureScreen0.loadImage(); // Cargar la textura
	glGenTextures(1, &textureScreen0ID); // Creando el ID de la textura
	glBindTexture(GL_TEXTURE_2D, textureScreen0ID); // Enlazar la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (textureScreen1.getData()) {
		glTexImage2D(GL_TEXTURE_2D, 0, textureScreen0.getChannels() == 3 ? GL_RGB : GL_RGBA, textureScreen0.getWidth(), textureScreen0.getHeight(), 0,
					textureScreen0.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, textureScreen0.getData());
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Fallo la carga de textura para screen" << std::endl;
	}
	textureScreen0.freeImage();
	
	// Definiendo la textura
	Texture textureEndGame("../Textures/proy_brickFin.png");
	textureEndGame.loadImage(); // Cargar la textura
	GLuint textureEndGameID;    // Crear el ID de la textura
	glGenTextures(1, &textureEndGameID); // Creando el id de la textura del fin del juego
	glBindTexture(GL_TEXTURE_2D, textureEndGameID); // Se enlaza la textura
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // Wrapping en el eje u
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // Wrapping en el eje v
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Filtering de minimización
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Filtering de maximización
	if (textureEndGame.getData()) {
		glTexImage2D(GL_TEXTURE_2D, 0, textureEndGame.getChannels() == 3 ? GL_RGB : GL_RGBA, 
					textureEndGame.getWidth(), textureEndGame.getHeight(), 0,
					textureEndGame.getChannels() == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, 
					textureEndGame.getData());
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Fallo la carga de textura para proy_brickFin.png" << std::endl;
	}
	textureEndGame.freeImage(); // Liberamos memoria

	std::uniform_real_distribution<float> distr01 = std::uniform_real_distribution<float>(0.0f, 1.0f);
	std::mt19937 generator;
	std::random_device rd;
	generator.seed(rd());
	int size = nParticlesFire * 2;
	std::vector<GLfloat> randData(size);
	for( int i = 0; i < randData.size(); i++ ) {
		randData[i] = distr01(generator);
	}

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_1D, texId);
	glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, size);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, size, GL_RED, GL_FLOAT, randData.data());
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	shaderParticlesFire.setInt("Pass", 1);
	shaderParticlesFire.setInt("ParticleTex", 0);
	shaderParticlesFire.setInt("RandomTex", 1);
	shaderParticlesFire.setFloat("ParticleLifetime", particleLifetime);
	shaderParticlesFire.setFloat("ParticleSize", particleSize);
	shaderParticlesFire.setVectorFloat3("Accel", glm::value_ptr(glm::vec3(0.0f,0.1f,0.0f)));
	shaderParticlesFire.setVectorFloat3("Emitter", glm::value_ptr(glm::vec3(0.0f)));

	glm::mat3 basis;
	glm::vec3 u, v, n;
	v = glm::vec3(0,1,0);
	n = glm::cross(glm::vec3(1,0,0), v);
	if( glm::length(n) < 0.00001f ) {
		n = glm::cross(glm::vec3(0,1,0), v);
	}
	u = glm::cross(v,n);
	basis[0] = glm::normalize(u);
	basis[1] = glm::normalize(v);
	basis[2] = glm::normalize(n);
	shaderParticlesFire.setMatrix3("EmitterBasis", 1, false, glm::value_ptr(basis));

	initParticleBuffers();
	initParticleBuffersFire();
	/*******************************************
	 * Inicializacion del framebuffer para
	 * almacenar el buffer de profunidadad
	 *******************************************/
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
				 SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/*******************************************
	 * OpenAL init
	 *******************************************/
	alutInit(0, nullptr);
	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);
	alGetError(); // clear any error messages
	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating buffers !!\n");
		exit(1);
	}
	else {
		printf("init() - No errors yet.");
	}
	// Config source 0
	// Generate buffers, or else no sound will happen!
	alGenBuffers(NUM_BUFFERS, buffer);
	buffer[0] = alutCreateBufferFromFile("../sounds/GloomyGrove.wav");
	//buffer[1] = alutCreateBufferFromFile("../sounds/fire.wav");
	buffer[2] = alutCreateBufferFromFile("../sounds/PunchEfect.wav");
	int errorAlut = alutGetError();
	if (errorAlut != ALUT_ERROR_NO_ERROR){
		printf("- Error open files with alut %d !!\n", errorAlut);
		exit(2);
	}

	alGetError(); /* clear error */
	alGenSources(NUM_SOURCES, source);

	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating sources !!\n");
		exit(2);
	}
	else {
		printf("init - no errors after alGenSources\n");
	}
	alSourcef(source[0], AL_PITCH, 1.0f);
	alSourcef(source[0], AL_GAIN, 3.0f);
	alSourcefv(source[0], AL_POSITION, source0Pos);
	alSourcefv(source[0], AL_VELOCITY, source0Vel);
	alSourcei(source[0], AL_BUFFER, buffer[0]);
	alSourcei(source[0], AL_LOOPING, AL_TRUE);
	alSourcef(source[0], AL_MAX_DISTANCE, 2000);

	alSourcef(source[1], AL_PITCH, 1.0f);
	alSourcef(source[1], AL_GAIN, 3.0f);
	alSourcefv(source[1], AL_POSITION, source1Pos);
	alSourcefv(source[1], AL_VELOCITY, source1Vel);
	alSourcei(source[1], AL_BUFFER, buffer[1]);
	alSourcei(source[1], AL_LOOPING, AL_TRUE);
	alSourcef(source[1], AL_MAX_DISTANCE, 2000);

	alSourcef(source[2], AL_PITCH, 1.0f);
	alSourcef(source[2], AL_GAIN, 0.3f);
	alSourcefv(source[2], AL_POSITION, source2Pos);
	alSourcefv(source[2], AL_VELOCITY, source2Vel);
	alSourcei(source[2], AL_BUFFER, buffer[2]);
	alSourcei(source[2], AL_LOOPING, AL_FALSE);
	alSourcef(source[2], AL_MAX_DISTANCE, 500);
}

void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
	// --------- IMPORTANTE ----------
	// Eliminar los shader y buffers creados.

	// Shaders Delete
	shader.destroy();
	shaderMulLighting.destroy();
	shaderSkybox.destroy();
	shaderTerrain.destroy();
	//shaderParticlesFountain.destroy();
	shaderParticlesFire.destroy();

	// Basic objects Delete
	skyboxSphere.destroy();
	boxCollider.destroy();
	sphereCollider.destroy();
	boxViewDepth.destroy();
	boxLightViewBox.destroy();

	// Terrains objects Delete
	terrain.destroy();

	//Destroy proyectos
	modelRavennaAnimate.destroy();

	modelWolfBody.destroy();
	modelWolfTail.destroy();

	modelBatBody.destroy();
	modelBatHead.destroy();
	modelBatLwing.destroy();
	modelBatRwing.destroy();

	modelGrave1.destroy();
	modelGrave2.destroy();
	modelGrave3.destroy();
	modelGrave4.destroy();
	modelGrave5.destroy();
	modelTree.destroy();

	modelMonsterBody.destroy();
	modelMonsterLarm.destroy();
	modelMonsterRarm.destroy();

	boxIntro.destroy();

	// Textures Delete
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &textureTerrainBackgroundID);
	glDeleteTextures(1, &textureTerrainRID);
	glDeleteTextures(1, &textureTerrainGID);
	glDeleteTextures(1, &textureTerrainBID);

	glDeleteTextures(1, &textureTerrainBlendMapID);
	glDeleteTextures(1, &textureInit1ID);
	glDeleteTextures(1, &textureInit2ID);
	glDeleteTextures(1, &textureScreen3ID);

	glDeleteTextures(1, &textureEndGameID);


	// Cube Maps Delete
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDeleteTextures(1, &skyboxTextureID);

	// Remove the buffer of the fountain particles
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &initVel);
	glDeleteBuffers(1, &startTime);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAOParticles);

	// Remove the buffer of the fire particles
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(2, posBuf);
	glDeleteBuffers(2, velBuf);
	glDeleteBuffers(2, age);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	glDeleteTransformFeedbacks(2, feedback);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAOParticlesFire);
}

void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action,
		int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
	distanceFromTarget -= yoffset;
	camera->setDistanceFromTarget(distanceFromTarget);
}

void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		}
	}
}

bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}

	if (!iniciaPartida) {
		bool presionarEnter = glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS;
		if (textureActivaID == textureInit1ID && presionarEnter) {
			iniciaPartida = true;
			textureActivaID = textureScreen3ID;
			speedBat = 3.0f;
			speedMonster = 2.0f;
			speedWolf = 1.0f;
		} else if (textureActivaID == textureInit2ID && presionarEnter) {
			// Termina el programa si se presiona ENTER y la textura activa es "proy_brick2"
			exitApp = true; // Señala que el programa debe terminar
		} else if (!presionarOpcion && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
			presionarOpcion = true;
			if (textureActivaID == textureInit1ID)
				textureActivaID = textureInit2ID;
			else if (textureActivaID == textureInit2ID)
				textureActivaID = textureInit1ID;
		} else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)
			presionarOpcion = false;
	}

	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, 0.0, deltaTime);
	if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		camera->mouseMoveCamera(0.0, offsetY, deltaTime);
	offsetX = 0;
	offsetY = 0;
	bool sonidoGolpe = false;

	glm::vec3 ravennaPosition = glm::vec3(modelMatrixRavenna[3]);
	if (ravennaPosition.x > 90) {
	    modelMatrixRavenna[3][0] = 89.0f; // Corrige a 9.9
	} else if (ravennaPosition.x < -25) {
	    modelMatrixRavenna[3][0] = -24.0f; // Corrige a -9.9
	}

	if (ravennaPosition.z > 75) {
	    modelMatrixRavenna[3][2] = 74.0f; // Corrige a 9.9
	} else if (ravennaPosition.z < -27) {
	    modelMatrixRavenna[3][2] = -26.0f; // Corrige a -9.9
	}
		// Ravinna model movements
	if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
		modelMatrixRavenna = glm::rotate(modelMatrixRavenna, glm::radians(1.0f), glm::vec3(0, 1, 0));
		animationIndex = 1;
	}else if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
		modelMatrixRavenna = glm::rotate(modelMatrixRavenna, glm::radians(-1.0f), glm::vec3(0, 1, 0));
		animationIndex = 1;
	}if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
		modelMatrixRavenna = glm::translate(modelMatrixRavenna, glm::vec3(0, 0, 0.1));
		animationIndex = 1;
	}else if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
		modelMatrixRavenna = glm::translate(modelMatrixRavenna, glm::vec3(0, 0, -0.1));
		animationIndex = 1;

	}if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS){
		animationIndex = 2;
		ALint state;
		alGetSourcei(source[2], AL_SOURCE_STATE, &state);
		
		if (state != AL_PLAYING) {
			alSourcePlay(source[2]);
		}
	}
	

	bool keySpaceStatus = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
	if(!isJump && keySpaceStatus){
		isJump = true;
		startTimeJump = currTime;
		tmv = 0;
	}

	glfwPollEvents();
	return continueApplication;
}
	int vidaRavenna = 3;


void applicationLoop() {
	bool psi = true;

	glm::mat4 view;
	glm::vec3 axis;
	glm::vec3 target;
	float angleTarget;

	//Proyecto Posicion de Ravenna
	modelMatrixRavenna = glm::translate(modelMatrixRavenna, glm::vec3(randomInRange(-25, 90), 5.0f, randomInRange(-27, 75)));

	modelMatrixWolf = glm::translate(modelMatrixWolf, glm::vec3(randomInRange(-25, 90), 5.0f, randomInRange(-27, 75)));

	modelMatrixBat = glm::translate(modelMatrixBat, glm::vec3(randomInRange(-25, 90), 5.0f, randomInRange(-27, 75)));

	modelMatrixGrave2 = glm::translate(modelMatrixGrave2, glm::vec3(13.0f, 5.0f, 2.0f));
	modelMatrixGrave3 = glm::translate(modelMatrixGrave3, glm::vec3(15.0f, 5.0f, 0.0f));
	modelMatrixGrave4 = glm::translate(modelMatrixGrave4, glm::vec3(13.0f, 5.0f, 0.0f));
	modelMatrixGrave5 = glm::translate(modelMatrixGrave5, glm::vec3(15.0f, 5.0f, -2.0f));
	modelMatrixTree = glm::translate(modelMatrixTree, glm::vec3(11.0f, 5.0f, -4.0f));

	modelMatrixMonster = glm::translate(modelMatrixMonster, glm::vec3(randomInRange(-25, 90), 5.0f, randomInRange(-27, 75)));

	lastTime = TimeManager::Instance().GetTime();

	// Limpia el framebuffer antes de renderizar
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Fondo negro
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	textureActivaID = textureInit1ID;

	// Time for the particles animation
	currTimeParticlesAnimation = lastTime;
	lastTimeParticlesAnimation = lastTime;

	currTimeParticlesAnimationFire = lastTime;
	lastTimeParticlesAnimationFire = lastTime;

	glm::vec3 lightPos = glm::vec3(10.0, 10.0, 0.0);

	while (psi) {
		currTime = TimeManager::Instance().GetTime();
		if(currTime - lastTime < 0.016666667){
			glfwPollEvents();
			continue;
		}
		lastTime = currTime;
		TimeManager::Instance().CalculateFrameRate(true);
		deltaTime = TimeManager::Instance().DeltaTime;
		psi = processInput(true);

		std::map<std::string, bool> collisionDetection;
		timeMonster += 5*deltaTime;
		timeBat += 5*deltaTime;
		timeWolf += 3*deltaTime;

		if (timerCountdown > 0.0f) {
    		timerCountdown -= deltaTime; // Resta el tiempo transcurrido.
    		if (timerCountdown < 0.0f) {
       			timerCountdown = 0.0f; // Asegúrate de que no sea negativo.
    		}
		}

		int seconds = static_cast<int>(timerCountdown); // Parte entera (segundos).
		int tenths = static_cast<int>((timerCountdown - seconds) * 10); // Décimas.

		std::string timerText = "Tiempo restante: " + std::to_string(seconds) + "." + std::to_string(tenths) + "s";
		
		// Obtén la posición actual del personaje principal (Ravenna)
		glm::vec3 ravennaPosition = glm::vec3(modelMatrixRavenna[3]);

		checkCollisionWithMonsters(ravennaPosition, glm::vec3(modelMatrixMonster[3]), vidaRavenna, currTime, lastDamageTime);
		checkCollisionWithMonsters(ravennaPosition, glm::vec3(modelMatrixWolf[3]), vidaRavenna, currTime, lastDamageTime);
		checkCollisionWithMonsters(ravennaPosition, glm::vec3(modelMatrixBat[3]), vidaRavenna, currTime, lastDamageTime);

		// Mueve los monstruos hacia Ravenna
		moveTowards(modelMatrixMonster, ravennaPosition, speedMonster);       // Monstruo
		moveTowards(modelMatrixWolf, ravennaPosition, speedWolf);       // Lobo
		moveTowards(modelMatrixBat, ravennaPosition, speedBat);       // Murciélago

		// (Opcional) Haz que los monstruos roten hacia Ravenna
		rotateTowards(modelMatrixMonster, ravennaPosition);
		rotateTowards(modelMatrixWolf, ravennaPosition);
		rotateTowards(modelMatrixBat, ravennaPosition);
		 // Mover y rotar a los monstruos hacia Ravenna
    	ravennaPosition = glm::vec3(modelMatrixRavenna[3]);

		// Actualizar los ángulos de rotación usando deltaTime para hacerlo independiente del framerate
		wolfRotationAngle += wolfRotationSpeed * deltaTime;
		if ((wolfRotationAngle*wolfRotationAngle) > 225.0f) {
			wolfRotationSpeed *= -1.0f;  // Invertir la dirección para oscilar
			wolfRotationAngle += wolfRotationSpeed * deltaTime;
		}

		monsterRotationAngle += monsterRotationSpeed * deltaTime;
		if ((monsterRotationAngle*monsterRotationAngle) > 100.0f) {
		    monsterRotationSpeed *= -1.0f;  // Invertir la dirección para oscilar
			monsterRotationAngle += monsterRotationSpeed * deltaTime;
		}

		// Variables donde se guardan las matrices de cada articulacion por 1 frame
		std::vector<float> matrixDartJoints;
		std::vector<glm::mat4> matrixDart;

		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
				(float) screenWidth / (float) screenHeight, 0.1f, 100.0f);

		if(modelSelected == 0){
			axis = glm::axis(glm::quat_cast(modelMatrixRavenna));
			angleTarget = glm::angle(glm::quat_cast(modelMatrixRavenna));
			target = modelMatrixRavenna[3];
		}

		if(std::isnan(angleTarget))
			angleTarget = 0.0;
		if(axis.y < 0)
			angleTarget = -angleTarget;
		if(modelSelected == 1)
			angleTarget -= glm::radians(90.0f);
		camera->setCameraTarget(target);
		camera->setAngleTarget(angleTarget);
		camera->updateCamera();
		view = camera->getViewMatrix();

		// Matriz de proyección del shadow mapping
		glm::mat4 lightProjection, lightView;
		glm::mat4 lightSpaceMatrix;
		float near_plane = 0.1f, far_plane = 20.0f;
		lightProjection = glm::ortho(-25.0f, 25.0f, -25.0f, 25.0f, near_plane, far_plane);
		lightView = glm::lookAt(lightPos, glm::vec3(0.0), glm::vec3(0.0, 1.0, 0.0));
		lightSpaceMatrix = lightProjection * lightView;
		shaderDepth.setMatrix4("lightSpaceMatrix", 1, false, glm::value_ptr(lightSpaceMatrix));

		// Settea la matriz de vista y projection al shader con solo color
		shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shader.setMatrix4("view", 1, false, glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con skybox
		shaderSkybox.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
		shaderSkybox.setMatrix4("view", 1, false,
				glm::value_ptr(glm::mat4(glm::mat3(view))));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderMulLighting.setMatrix4("projection", 1, false,
					glm::value_ptr(projection));
		shaderMulLighting.setMatrix4("view", 1, false,
				glm::value_ptr(view));
		shaderMulLighting.setMatrix4("lightSpaceMatrix", 1, false,
				glm::value_ptr(lightSpaceMatrix));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderTerrain.setMatrix4("projection", 1, false,
					glm::value_ptr(projection));
		shaderTerrain.setMatrix4("view", 1, false,
				glm::value_ptr(view));
		shaderTerrain.setMatrix4("lightSpaceMatrix", 1, false,
				glm::value_ptr(lightSpaceMatrix));

		/*******************************************
		 * Propiedades de neblina
		 *******************************************/
		shaderMulLighting.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.5, 0.5, 0.4)));
		shaderTerrain.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.5, 0.5, 0.4)));
		shaderSkybox.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.5, 0.5, 0.4)));

		/*******************************************
		 * Propiedades Luz direccional
		 *******************************************/
		shaderMulLighting.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderMulLighting.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderMulLighting.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-0.707106781, -0.707106781, 0.0)));

		/*******************************************
		 * Propiedades Luz direccional Terrain
		 *******************************************/
		shaderTerrain.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderTerrain.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderTerrain.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		shaderTerrain.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderTerrain.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-0.707106781, -0.707106781, 0.0)));

		/*******************************************
		 * Propiedades SpotLights
		 *******************************************/
		if (!iniciaPartida) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Limpia los buffers antes de dibujar
			shaderTexture.setMatrix4("projection", 1, false, glm::value_ptr(glm::mat4(1.0)));
			shaderTexture.setMatrix4("view", 1, false, glm::value_ptr(glm::mat4(1.0)));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureActivaID); // Usa la textura activa actual
			shaderTexture.setInt("outTexture", 0);
			boxIntro.render();
			glBindTexture(GL_TEXTURE_2D, 0); // Desvincula la textura después de renderizar
			glfwSwapBuffers(window); // Intercambia los buffers para mostrar la textura actual
			continue; // Salta al siguiente ciclo del bucle
		}

		/*******************************************
		 * 1.- We render the depth buffer
		 *******************************************/
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// render scene from light's point of view
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		//glCullFace(GL_FRONT);
		prepareDepthScene();
		renderScene(false);
		//glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		/*******************************************
		 * 2.- We render the normal objects
		 *******************************************/
		glViewport(0, 0, screenWidth, screenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		prepareScene();
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		shaderMulLighting.setInt("shadowMap", 10);
		shaderTerrain.setInt("shadowMap", 10);
		/*******************************************
		 * Skybox
		 *******************************************/
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;
		// deshabilita el modo del recorte de caras ocultas para ver las esfera desde adentro
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
		shaderSkybox.setFloat("skybox", 0);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		glActiveTexture(GL_TEXTURE0);
		skyboxSphere.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);
		renderScene();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		
		glEnable(GL_CULL_FACE);

		shaderTexture.setMatrix4("projection", 1, false, glm::value_ptr(glm::mat4(1.0)));
		shaderTexture.setMatrix4("view", 1, false, glm::value_ptr(glm::mat4(1.0)));

		// Desvincula cualquier textura previamente activa
		glBindTexture(GL_TEXTURE_2D, 0);
		if (vidaRavenna == 3) {
    		textureActivaID = textureScreen3ID; // screen_vida3
		} else if (vidaRavenna == 2) {
    		textureActivaID = textureScreen2ID; // screen_vida2
		} else if (vidaRavenna == 1) {
			textureActivaID = textureScreen1ID; // screen_vida1
		} else if (vidaRavenna == 0) {
			textureActivaID = textureScreen0ID; // screen_vida0
			timerText.clear();
		}

		if (timerCountdown <= 45.0f){
			speedBat = 3.5f;
			speedMonster = 3.0f;
			speedWolf = 2.5f;
		}
		if (timerCountdown <= 30.0f){
			speedBat = 3.7f;
			speedMonster = 4.0f;
			speedWolf = 4.5f;
		}
		if (timerCountdown <= 15.0f){
			speedBat = 4.0f;
			speedMonster = 5.3f;
			speedWolf = 4.8f;
		}

		if (timerCountdown <= 0.1f){
			textureActivaID = textureScreen0ID;
			timerText.clear();
		}

		// Enlaza la textura activa basada en vidaRavenna
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureActivaID);

		shaderTexture.setInt("outTexture", 0);
		boxIntro.render();
		
		glBindTexture(GL_TEXTURE_2D, 0);

		//MonsterAnimation
		scaleY = 0.3f + 0.02f * sin(timeMonster * 2.0f);  // Escala entre 0.3 y 0.5
		floatOffset = 0.2f * sin(timeBat * 5.0f);  // Oscila entre -0.2 y 0.2
		wingAngle = 60.0f * sin(timeBat * 6.0f);  // Oscila entre -15° y 15°
		scaleYMonster = 0.3f + 0.02f * sin(timeMonster * 2.0f);  // Misma escala que el monstruo
		tailAngle = 60.0f * sin(timeWolf * 2.0f);  // Oscila entre -10° y 10° con diferente velocidad

		// Constantes de animaciones
		animationIndex = 1;

		modelText->render(timerText, -1, 0, 1, 0, 0, 24);

		glfwSwapBuffers(window);

		/****************************
		 * Open AL sound data
		 */
		source0Pos[0] = modelMatrixRavenna[3].x;
		source0Pos[1] = modelMatrixRavenna[3].y;
		source0Pos[2] = modelMatrixRavenna[3].z;
		alSourcefv(source[0], AL_POSITION, source0Pos);

		source2Pos[0] = modelMatrixRavenna[3].x;
		source2Pos[1] = modelMatrixRavenna[3].y;
		source2Pos[2] = modelMatrixRavenna[3].z;
		alSourcefv(source[2], AL_POSITION, source2Pos);

		// Listener for the Thris person camera
		listenerPos[0] = modelMatrixRavenna[3].x;
		listenerPos[1] = modelMatrixRavenna[3].y;
		listenerPos[2] = modelMatrixRavenna[3].z;
		alListenerfv(AL_POSITION, listenerPos);

		glm::vec3 upModel = glm::normalize(modelMatrixRavenna[1]);
		glm::vec3 frontModel = glm::normalize(modelMatrixRavenna[2]);

		listenerOri[0] = frontModel.x;
		listenerOri[1] = frontModel.y;
		listenerOri[2] = frontModel.z;
		listenerOri[3] = upModel.x;
		listenerOri[4] = upModel.y;
		listenerOri[5] = upModel.z;
		alListenerfv(AL_ORIENTATION, listenerOri);

		for(unsigned int i = 0; i < sourcesPlay.size(); i++){
			if(sourcesPlay[i]){
				sourcesPlay[i] = false;
				alSourcePlay(source[i]);
			}
		}
	}
}

void prepareScene(){

	skyboxSphere.setShader(&shaderSkybox);
	terrain.setShader(&shaderTerrain);

	//Ravenna
	modelRavennaAnimate.setShader(&shaderMulLighting);

	modelWolfBody.setShader(&shaderMulLighting);
	modelWolfTail.setShader(&shaderMulLighting);

	modelGrave1.setShader(&shaderMulLighting);
	modelGrave2.setShader(&shaderMulLighting);
	modelGrave3.setShader(&shaderMulLighting);
	modelGrave4.setShader(&shaderMulLighting);
	modelGrave5.setShader(&shaderMulLighting);
	modelTree.setShader(&shaderMulLighting);

	modelMonsterBody.setShader(&shaderMulLighting);
	modelMonsterLarm.setShader(&shaderMulLighting);
	modelMonsterRarm.setShader(&shaderMulLighting);
}

void prepareDepthScene(){

	skyboxSphere.setShader(&shaderDepth);

	terrain.setShader(&shaderDepth);

	//Ravenna
	modelRavennaAnimate.setShader(&shaderDepth);

	modelWolfBody.setShader(&shaderDepth);
	modelWolfTail.setShader(&shaderDepth);

	modelGrave1.setShader(&shaderDepth);
	modelGrave2.setShader(&shaderDepth);
	modelGrave3.setShader(&shaderDepth);
	modelGrave4.setShader(&shaderDepth);
	modelGrave5.setShader(&shaderDepth);
	modelTree.setShader(&shaderDepth);

	modelMonsterBody.setShader(&shaderDepth);
	modelMonsterLarm.setShader(&shaderDepth);
	modelMonsterRarm.setShader(&shaderDepth);

}

void renderScene(bool renderParticles){
	/*******************************************
	 * Terrain Cesped
	 *******************************************/
	glm::mat4 modelCesped = glm::mat4(1.0);
	modelCesped = glm::translate(modelCesped, glm::vec3(0.0, 0.0, 0.0));
	modelCesped = glm::scale(modelCesped, glm::vec3(200.0, 0.001, 200.0));
	// Se activa la textura del background
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBackgroundID);
	shaderTerrain.setInt("backgroundTexture", 0);
	// Se activa la textura de tierra
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureTerrainRID);
	shaderTerrain.setInt("rTexture", 1);
	// Se activa la textura de hierba
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureTerrainGID);
	shaderTerrain.setInt("gTexture", 2);
	// Se activa la textura del camino
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBID);
	shaderTerrain.setInt("bTexture", 3);
	// Se activa la textura del blend map
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID);
	shaderTerrain.setInt("blendMapTexture", 4);
	shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(40, 40)));
	terrain.render();
	shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(0, 0)));
	glBindTexture(GL_TEXTURE_2D, 0);

	/*******************************************
	 * Custom objects obj
	 *******************************************/
	// Forze to enable the unit texture to 0 always ----------------- IMPORTANT
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_CULL_FACE);

	// Volver a habilitar el depth test para la escena 3D
	glEnable(GL_DEPTH_TEST);

	glm::mat4 projection = glm::perspective(glm::radians(45.0f),
		(float)screenWidth / (float)screenHeight, 0.1f, 100.0f);
	glm::mat4 view = camera->getViewMatrix();

	shaderMulLighting.setMatrix4("projection", 1, false, glm::value_ptr(projection));
	shaderMulLighting.setMatrix4("view", 1, false, glm::value_ptr(view));

	glm::mat4 modelMatrixRavennaBody = glm::mat4(modelMatrixRavenna);
	modelMatrixRavennaBody = glm::scale(modelMatrixRavennaBody, glm::vec3(0.015, 0.015, 0.015));
	modelRavennaAnimate.setAnimationIndex(animationIndex);
	modelRavennaAnimate.render(modelMatrixRavennaBody);

	modelMatrixRavenna[3][1] = -GRAVITY * tmv * tmv + 3.5 * tmv + terrain.getHeightTerrain(modelMatrixRavenna[3][0], modelMatrixRavenna[3][2]);
	tmv = currTime - startTimeJump;
	if(modelMatrixRavenna[3][1] < terrain.getHeightTerrain(modelMatrixRavenna[3][0], modelMatrixRavenna[3][2])){
		isJump = false;
		modelMatrixRavenna[3][1] = terrain.getHeightTerrain(modelMatrixRavenna[3][0], modelMatrixRavenna[3][2]);
	}

	//Proyecto Ravenna	
	glDisable(GL_CULL_FACE);
	modelMatrixWolf[3][1] = terrain.getHeightTerrain(modelMatrixWolf[3][0], modelMatrixWolf[3][2]);
	glm::mat4 modelMatrixWolfBody = glm::mat4(modelMatrixWolf);
	// Aplicar rotación oscilante en el eje X
	modelMatrixWolfBody = glm::rotate(modelMatrixWolfBody, glm::radians(wolfRotationAngle), glm::vec3(1.0f, 0.0f, 0.0f));
	modelMatrixWolfBody = glm::scale(modelMatrixWolfBody, glm::vec3(0.3f, scaleYMonster, 0.3f));

	modelWolfBody.render(modelMatrixWolfBody);

	glm::mat4 modelMatrixWolfTail = glm::mat4(modelMatrixWolfBody);
	modelMatrixWolfTail = glm::translate(modelMatrixWolfTail, glm::vec3(0.068688, 2.86288, -2.2089));
	modelMatrixWolfTail = glm::rotate(modelMatrixWolfTail, glm::radians(tailAngle), glm::vec3(0.0f, 1.0f, 0.0f));  // Rotación en Y
	
	modelWolfTail.render(modelMatrixWolfTail);
	///////////////////////////////////////////////////////////////////////////////////
	// Aplicar la transformación de flotación al cuerpo
	glm::mat4 modelMatrixBatBody = glm::mat4(modelMatrixBat);
	modelMatrixBatBody = glm::translate(modelMatrixBatBody, glm::vec3(0.0f, floatOffset, 0.0f));
	modelMatrixBatBody = glm::scale(modelMatrixBatBody, glm::vec3(0.5, 0.5, 0.5));
	modelBatBody.render(modelMatrixBatBody);
	// Renderizar la cabeza (hija del cuerpo)
	glm::mat4 modelMatrixBatHead = glm::mat4(modelMatrixBatBody);
	modelMatrixBatHead = glm::translate(modelMatrixBatHead, glm::vec3(0.0f, 2.21819, 0.01379));
	modelBatHead.render(modelMatrixBatHead);

	// Renderizar el ala izquierda (rotación hacia adentro y hacia afuera)
	glm::mat4 modelMatrixBatLwing = glm::mat4(modelMatrixBatBody);
	modelMatrixBatLwing = glm::translate(modelMatrixBatLwing, glm::vec3(-0.010513, 1.21819, 0.01379));
	modelMatrixBatLwing = glm::rotate(modelMatrixBatLwing, glm::radians(wingAngle), glm::vec3(0.0f, 1.0f, 0.0f));  // Rotación en Z
	modelBatLwing.render(modelMatrixBatLwing);

	// Renderizar el ala derecha (rotación hacia adentro y hacia afuera)
	glm::mat4 modelMatrixBatRwing = glm::mat4(modelMatrixBatBody);
	modelMatrixBatRwing = glm::translate(modelMatrixBatRwing, glm::vec3(0.010513, 1.21819, 0.01379));
	modelMatrixBatRwing = glm::rotate(modelMatrixBatRwing, glm::radians(-wingAngle), glm::vec3(0.0f, 1.0f, 0.0f));  // Rotación en Z inversa
	modelBatRwing.render(modelMatrixBatRwing);
	///////////////////////////////////////////////////////////////////////////////////
	modelMatrixMonster[3][1] = terrain.getHeightTerrain(modelMatrixMonster[3][0], modelMatrixMonster[3][2]);
	glm::mat4 modelMatrixMonsterBody = glm::mat4(modelMatrixMonster);
	
	// Aplicar rotación oscilante en el eje Z
	modelMatrixMonsterBody = glm::rotate(modelMatrixMonsterBody, glm::radians(monsterRotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
	modelMatrixMonsterBody = glm::scale(modelMatrixMonsterBody, glm::vec3(0.3f, scaleY, 0.3f));
	
	modelMonsterBody.render(modelMatrixMonsterBody);

	glm::mat4 modelMatrixMonsterLarm = glm::mat4(modelMatrixMonsterBody);
	modelMatrixMonsterLarm = glm::translate(modelMatrixMonsterLarm, glm::vec3(-0.9, 6.3, 0));
	modelMonsterLarm.render(modelMatrixMonsterLarm);
	glm::mat4 modelMatrixMonsterRarm = glm::mat4(modelMatrixMonsterBody);
	modelMatrixMonsterRarm = glm::translate(modelMatrixMonsterRarm, glm::vec3(0.9, 6.3, 0));
	modelMonsterRarm.render(modelMatrixMonsterRarm);

	modelMatrixGrave1[3][1] = terrain.getHeightTerrain(modelMatrixGrave1[3][0], modelMatrixGrave1[3][2]);
	glm::mat4 modelMatrixGrave1Body = glm::mat4(modelMatrixGrave1);
	modelMatrixGrave1Body = glm::scale(modelMatrixGrave1Body, glm::vec3(0.3, 0.3, 0.3));
	modelGrave1.render(modelMatrixGrave1Body);
	modelMatrixGrave2[3][1] = terrain.getHeightTerrain(modelMatrixGrave2[3][0], modelMatrixGrave2[3][2]);
	glm::mat4 modelMatrixGrave2Body = glm::mat4(modelMatrixGrave2);
	modelMatrixGrave2Body = glm::scale(modelMatrixGrave2Body, glm::vec3(0.3, 0.3, 0.3));
	modelGrave2.render(modelMatrixGrave2Body);
	modelMatrixGrave3[3][1] = terrain.getHeightTerrain(modelMatrixGrave3[3][0], modelMatrixGrave3[3][2]);
	glm::mat4 modelMatrixGrave3Body = glm::mat4(modelMatrixGrave3);
	modelMatrixGrave3Body = glm::scale(modelMatrixGrave3Body, glm::vec3(0.3, 0.3, 0.3));
	modelGrave3.render(modelMatrixGrave3Body);
	modelMatrixGrave4[3][1] = terrain.getHeightTerrain(modelMatrixGrave4[3][0], modelMatrixGrave4[3][2]);
	glm::mat4 modelMatrixGrave4Body = glm::mat4(modelMatrixGrave4);
	modelMatrixGrave4Body = glm::scale(modelMatrixGrave4Body, glm::vec3(0.3, 0.3, 0.3));
	modelGrave4.render(modelMatrixGrave4Body);
	modelMatrixGrave5[3][1] = terrain.getHeightTerrain(modelMatrixGrave5[3][0], modelMatrixGrave5[3][2]);
	glm::mat4 modelMatrixGrave5Body = glm::mat4(modelMatrixGrave5);
	modelMatrixGrave5Body = glm::scale(modelMatrixGrave5Body, glm::vec3(0.3, 0.3, 0.3));
	modelGrave5.render(modelMatrixGrave5Body);
	modelMatrixTree[3][1] = terrain.getHeightTerrain(modelMatrixGrave1[3][0], modelMatrixGrave1[3][2]);
	glm::mat4 modelMatrixTreeBody = glm::mat4(modelMatrixTree);
	modelMatrixTreeBody = glm::scale(modelMatrixTreeBody, glm::vec3(0.3, 0.3, 0.3));
	modelTree.render(modelMatrixTreeBody);

	//Lapidas
	for(int i = 0; i < grave1Position.size(); i++){
		grave1Position[i].y = terrain.getHeightTerrain(grave1Position[i].x, grave1Position[i].z);
		modelGrave1.setPosition(grave1Position[i]);
		modelGrave1.setScale(glm::vec3(0.7));
		modelGrave1.setOrientation(glm::vec3(0, grave1Orientation[i], 0));
		modelGrave1.render();
	}

	for(int i = 0; i < grave2Position.size(); i++){
		grave2Position[i].y = terrain.getHeightTerrain(grave2Position[i].x, grave2Position[i].z);
		modelGrave2.setPosition(grave2Position[i]);
		modelGrave2.setScale(glm::vec3(0.7));
		modelGrave2.setOrientation(glm::vec3(0, grave2Orientation[i], 0));
		modelGrave2.render();
	}

	for(int i = 0; i < grave3Position.size(); i++){
		grave3Position[i].y = terrain.getHeightTerrain(grave3Position[i].x, grave3Position[i].z);
		modelGrave3.setPosition(grave3Position[i]);
		modelGrave3.setScale(glm::vec3(0.7));
		modelGrave3.setOrientation(glm::vec3(0, grave3Orientation[i], 0));
		modelGrave3.render();
	}

	for(int i = 0; i < grave4Position.size(); i++){
		grave4Position[i].y = terrain.getHeightTerrain(grave4Position[i].x, grave4Position[i].z);
		modelGrave4.setPosition(grave4Position[i]);
		modelGrave4.setScale(glm::vec3(0.7));
		modelGrave4.setOrientation(glm::vec3(0, grave4Orientation[i], 0));
		modelGrave4.render();
	}

	for(int i = 0; i < grave5Position.size(); i++){
		grave5Position[i].y = terrain.getHeightTerrain(grave5Position[i].x, grave5Position[i].z);
		modelGrave5.setPosition(grave5Position[i]);
		modelGrave5.setScale(glm::vec3(0.7));
		modelGrave5.setOrientation(glm::vec3(0, grave5Orientation[i], 0));
		modelGrave5.render();
	}

		for(int i = 0; i < treePosition.size(); i++){
		treePosition[i].y = terrain.getHeightTerrain(treePosition[i].x, treePosition[i].z);
		modelTree.setPosition(treePosition[i]);
		modelTree.setScale(glm::vec3(0.4));
		modelTree.setOrientation(glm::vec3(0, treeOrientation[i], 0));
		modelTree.render();
	}
}

int main(int argc, char **argv) {
	std::cout << "Iniciando ---- " << std::endl;
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}