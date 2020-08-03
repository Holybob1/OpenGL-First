#include "Game.h"

//Private functions
void Game::initGLFW()
{
	//INIT glfw
	if (glfwInit() == GLFW_FALSE)
	{
		std::cout << "ERROR::GLFW_INIT_FAILED" << "\n";
		glfwTerminate();
	}
}

void Game::initWindow(
	const char* title,
	bool resizable)
{
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, this->GL_VERSION_MAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, this->GL_VIRSION_MINOR);
	glfwWindowHint(GLFW_RESIZABLE, resizable);

	this->window = glfwCreateWindow(this->WINDOW_WIDTH, this->WINDOW_HEIGHT, title, NULL, NULL);

	if (this->window == nullptr)
	{
		std::cout << "ERROR::GLFW_WINDOW_FAILED" << "\n";
		glfwTerminate();
	}

	//canvas size
	glfwGetFramebufferSize(this->window, &this->framebufferWidth, &this->framebufferHeight);
	glfwSetFramebufferSizeCallback(this->window,Game::framebuffer_resize_callback);

	glfwMakeContextCurrent(this->window); //important for glew
}

void Game::initGLEW()
{
	//INIT glew (needs window and gl context)
	glewExperimental = GL_TRUE;

	//error
	if (glewInit() != GLEW_OK)
	{
		std::cout << "ERROR::MAIN.CPP::GLEW_INIT_FAILED" << "\n";
		glfwTerminate();
	}
}

void Game::initOpenGLOptions()
{
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//Input
	glfwSetInputMode(this->window, GLFW_CURSOR,GLFW_CURSOR_DISABLED);
}

void Game::initMatrices()
{
	this->ViewMatrix = glm::mat4(1.f);
	this->ViewMatrix = glm::lookAt(this->camPosition, this->camPosition + this->camFront, this->worldUp);

	this->ProjectionMatrix = glm::mat4(1.f);
	this->ProjectionMatrix = glm::perspective(glm::radians(this->fov),
		static_cast<float>(this->framebufferWidth) / this->framebufferHeight,
		this->nearPlane,
		this->farPlane);
}

void Game::initShaders()
{
	this->shaders.push_back(new Shader (this->GL_VERSION_MAJOR, this->GL_VIRSION_MINOR,
		"vertex_core.glsl", "fragment_core.glsl"));
}

void Game::initTextures()
{
	//Texture 0 
	this->textures.push_back(new Texture("Images/Box.png", GL_TEXTURE_2D));

	this->textures.push_back(new Texture("Images/Box_specular.png", GL_TEXTURE_2D));
	//Texture 1
	this->textures.push_back(new Texture("Images/Ricardo_Kantov.png", GL_TEXTURE_2D));

	this->textures.push_back(new Texture("Images/Ricardo_Kantov_specular.png", GL_TEXTURE_2D));
}

void Game::initMaterials()
{
	this->materials.push_back(new Material(glm::vec3(0.1f), glm::vec3(1.f), glm::vec3(2.f),
	0, 1));
}

void Game::initOBJModels()
{

}

void Game::initModels()
{
	std::vector<Mesh*> meshes;

	meshes.push_back(new Mesh(
		&Pyramid(),
		glm::vec3(0.f),
		glm::vec3(0.f),
		glm::vec3(0.f),
		glm::vec3(1.f)
	)
	);

	this->models.push_back(new Model(
		glm::vec3(0.f),
		this->materials[0],
		this->textures[TEX_BOX],
		this->textures[TEX_BOX_SPECULAR],
		meshes
		)
	);

	this->models.push_back(new Model(
		glm::vec3(2.f,0.f,2.f),
		this->materials[0],
		this->textures[TEX_BOX],
		this->textures[TEX_BOX_SPECULAR],
		meshes
	)
	);

	this->models.push_back(new Model(
		glm::vec3(0.f, 1.f, 1.f),
		this->materials[0],
		this->textures[TEX_RICARDO_KANTOV],
		this->textures[TEX_RICARDO_KANTOV_SPECULAR],
		meshes
	)
	);

	this->models.push_back(new Model(
		glm::vec3(4.f, 0.f, 4.f),
		this->materials[0],
		this->textures[TEX_RICARDO_KANTOV],
		this->textures[TEX_RICARDO_KANTOV_SPECULAR],
		"OBJFiles/sphere.obj"
	)
	);

	for (auto*& i : meshes)
		delete i;
}

void Game::initLights()
{
	this->lights.push_back(new glm::vec3(0.f, 0.f, 1.f));
}

void Game::intiUniforms()
{
	this->shaders[SHADER_CORE_PROGRAM]->setMat4fv(ViewMatrix, "ViewMatrix");
	this->shaders[SHADER_CORE_PROGRAM]->setMat4fv(ProjectionMatrix, "ProjectionMatrix");

	this->shaders[SHADER_CORE_PROGRAM]->setVec3f(*this->lights[0], "lightPos0");
}

void Game::updateUniforms()
{
	//Update View Matrix(camera)
	this->ViewMatrix = this->camera.getViewMatrix();
	this->shaders[SHADER_CORE_PROGRAM]->setMat4fv(this->ViewMatrix, "ViewMatrix");
	this->shaders[SHADER_CORE_PROGRAM]->setVec3f(this->camera.getPosition(), "cameraPos");
	this->shaders[SHADER_CORE_PROGRAM]->setVec3f(*this->lights[0], "lightPos0");

	//Update Framebuffer Size and Projection matrix
	glfwGetFramebufferSize(this->window, &this->framebufferWidth, &this->framebufferHeight);

	this->ProjectionMatrix = glm::perspective(glm::radians(this->fov),
		static_cast<float>(this->framebufferWidth) / this->framebufferHeight,
		this->nearPlane,
		this->farPlane);
	this->shaders[SHADER_CORE_PROGRAM]->setMat4fv(this->ProjectionMatrix, "ProjectionMatrix");
}

//Constractor/Destractors
Game::Game(const char* title,
	const int WINDOW_WIDTH, const int WINDOW_HEIGHT,
	const int GL_VERSION_MAJOR, const int	GL_VIRSION_MINOR,
	bool resizable
) : WINDOW_WIDTH(WINDOW_WIDTH),
	WINDOW_HEIGHT(WINDOW_HEIGHT),
	GL_VERSION_MAJOR(GL_VERSION_MAJOR),
	GL_VIRSION_MINOR(GL_VIRSION_MINOR),
	camera(glm::vec3(0.f,0.f,1.f),glm::vec3(0.f,0.f,1.f), glm::vec3(0.f ,1.f, 0.f))
{
	//init variables
	this->window = nullptr;
	this->framebufferHeight = this->WINDOW_HEIGHT;
	this->framebufferWidth = this->WINDOW_WIDTH;

	this->camPosition = glm::vec3(0.f,0.f,1.f);
	this->worldUp = glm::vec3(0.f,1.f,0.f);
	this->camFront = glm::vec3(0.f, 0.f, -1.f);
	
	this->fov = 90.0;
	this->nearPlane = 0.1f;
	this->farPlane = 1000;

	this->dt = 0.f;
	this->curTime = 0.f;
	this->lastTime = 0.f;

	this->lastMouseX = 0.0;
	this->lastMouseY = 0.0;
	this->mouseX = 0.0;
	this->mouseY = 0.0;
	this->mouseOffsetX = 0.0;
	this->mouseOffsetY = 0.0;
	this->firstMouse = true;

	this->initGLFW();
	this->initWindow(title, resizable);
	this->initGLEW();
	this->initOpenGLOptions();
	this->initMatrices();
	this->initShaders();
	this->initTextures();
	this->initMaterials();
	this->initOBJModels();
	this->initModels();
	this->initLights();
	this->intiUniforms();
}

Game::~Game()
{
	glfwDestroyWindow(this->window);
	glfwTerminate();

	for (auto*& i : this->shaders)
		delete i;
	for (auto*& i : this->textures)
		delete i;
	for (auto*& i : this->materials)
		delete i;
	for (auto*& i : this->models)
		delete i;
	for (auto*& i : this->lights)
		delete i;
}

//Accessors
int Game::getWindowShouldClose()
{
	return glfwWindowShouldClose(this->window);
}

//Modifiers
void Game::setWindowShouldClose()
{
	glfwSetWindowShouldClose(this->window, GLFW_TRUE);
}

//Functions
void Game::updateDt()
{
	this->curTime = static_cast<float>(glfwGetTime());
	this->dt = this->curTime - this->lastTime;
	this->lastTime = this->curTime;
}

void Game::updateKeyboardInput()
{
	//Program
	if (glfwGetKey(this->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(this->window, GLFW_TRUE);
	}

	//Camera
	if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS)
	{
		this->camera.move(this->dt, FORWARD);
	}
	if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS)
	{
		this->camera.move(this->dt, BACKWARD);
	}
	if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS)
	{
		this->camera.move(this->dt, LEFT);
	}
	if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS)
	{
		this->camera.move(this->dt, RIGHT);
	}
	if (glfwGetKey(this->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		this->camera.move(this->dt, DOWN);
	}
	if (glfwGetKey(this->window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		this->camera.move(this->dt, UP);
	}
}

void Game::updateMouseInput()
{
	glfwGetCursorPos(this->window, &this->mouseX, &this->mouseY);

	if (this->firstMouse)
	{
		this->lastMouseX = this->mouseX;
		this->lastMouseY = this->mouseY;
		this->firstMouse = false;
	}

	//Calc offset
	this->mouseOffsetX = this->mouseX - this->lastMouseX;
	this->mouseOffsetY = this->mouseY - this->lastMouseY;

	//Set last X and Y
	this->lastMouseX = this->mouseX;
	this->lastMouseY = this->mouseY;

	//move light
	if (glfwGetMouseButton(this->window, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS)
	{
		*this->lights[0] = this->camera.getPosition();
	}

}	
void Game::updateInput()
{
	glfwPollEvents();
	this->updateKeyboardInput();
	this->updateMouseInput();
	this->camera.updateInput(dt,-1,this->mouseOffsetX,this->mouseOffsetY);
}

void Game::update()
{
	//UPDATE Input 
	this->updateDt();
	this->updateInput();

	/*this->models[0]->rotate(glm::vec3(0.f,1.f,0.f));
	this->models[1]->rotate(glm::vec3(0.f,-1.f,0.f));
	this->models[2]->rotate(glm::vec3(0.f,-1.f,1.f));*/
}

void Game::render()
{
	//uptade
	

	//clear
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//update the uniforms
	this->updateUniforms();

	//Render Uniforms
	for (auto& i :this->models)
	{
		i->render(this->shaders[SHADER_CORE_PROGRAM]);
	}
	

	//end draw 
	glfwSwapBuffers(window);
	glFlush();

	glBindVertexArray(0);
	glUseProgram(0);
	glActiveTexture(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

//Static functions
void Game::framebuffer_resize_callback(GLFWwindow* window, int fbW, int fbH)
{
	glViewport(0, 0, fbW, fbH);
}

