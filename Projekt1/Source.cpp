#include <iostream>
#include <array>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <fstream>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>

using namespace std;
using namespace glm;

vector<vec3> circlePoints;
vector<vec3> linePoints;
#define		numVAOs			2
#define		numVBOs			2
GLuint		VAO[numVAOs];
GLuint		VBO[numVBOs];

GLint		circleCenter;
GLint		Xoffset;
GLint		Yoffset;
GLuint		vonalYoffset;
GLboolean	Vonal;
float		vonalPozicioY = 0.0f;

float		x = 0.00f;
float		y = 0.00f;
float		szog = 25.0f * M_PI / 180.0f;
float		sebesseg = 0.01f;
float		xAlapSebesseg = sebesseg;
float		yAlapSebesseg = 0.0f;
float		xPattogoSebesseg = sebesseg * cos(szog);
float		yPattogoSebesseg = sebesseg * sin(szog);

int			window_width = 600;
int			window_height = 600;
char		window_title[] = "Szamitogepes grafika beadando 1.";

GLfloat		korAtlo = 50.0;
vec2		korKozepe = { 0.0f, 0.0f };

GLboolean	keyboard[512] = { GL_FALSE };
GLFWwindow* window = nullptr;
GLuint		renderingProgram;
bool		elinditva = false;

static bool checkOpenGLError() 
{
	bool	hiba = false;
	int		hibaSzamlalo = glGetError();

	while (hibaSzamlalo != GL_NO_ERROR) 
	{
		cerr << "glError: " << hibaSzamlalo << endl;

		hiba = true;
		hibaSzamlalo = glGetError();
	}

	return hiba;
}

void printShaderLog(GLuint shader) 
{
	int		length = 0;
	int		charsWritten = 0;
	char* log = nullptr;

	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

	if (length > 0) 
	{
		log = (char*)malloc(length);

		glGetShaderInfoLog(shader, length, &charsWritten, log);
		cout << "Shader Info: " << log << endl;
		free(log);
	}
}

void printProgramLog(int prog) 
{
	int		length = 0;
	int		charsWritten = 0;
	char* log = nullptr;

	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &length);

	if (length > 0) {
		log = (char*)malloc(length);

		glGetProgramInfoLog(prog, length, &charsWritten, log);
		cout << "Program Info: " << log << endl;
		free(log);
	}
}

string readShaderSource(const char* filePath) 
{
	ifstream	fileStream(filePath, ios::in);
	string		content;
	string		line;

	while (!fileStream.eof()) {
		getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return content;
}

GLuint createShaderProgram() 
{
	GLint		vertCompiled;
	GLint		fragCompiled;
	GLint		linked;
	string		vertShaderStr = readShaderSource("vertexShader.glsl");
	string		fragShaderStr = readShaderSource("fragmentShader.glsl");
	GLuint		vShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint		fShader = glCreateShader(GL_FRAGMENT_SHADER);

	const char* vertShaderSrc = vertShaderStr.c_str();
	const char* fragShaderSrc = fragShaderStr.c_str();

	glShaderSource(vShader, 1, &vertShaderSrc, NULL);
	glShaderSource(fShader, 1, &fragShaderSrc, NULL);

	glCompileShader(vShader);
	checkOpenGLError();
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &vertCompiled);
	if (vertCompiled != 1) 
	{
		cerr << "Vertex compilation failed." << endl;
		printShaderLog(vShader);
	}

	glCompileShader(fShader);
	
	checkOpenGLError();
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &fragCompiled);
	if (fragCompiled != 1) 
	{
		cerr << "Fragment compilation failed." << endl;
		printShaderLog(fShader);
	}

	GLuint vfProgram = glCreateProgram();
	glAttachShader(vfProgram, vShader);
	glAttachShader(vfProgram, fShader);
	glLinkProgram(vfProgram);
	glValidateProgram(vfProgram);
	checkOpenGLError();
	glGetProgramiv(vfProgram, GL_LINK_STATUS, &linked);
	if (linked != 1) 
	{
		cerr << "Shader linking failed." << endl;
		printProgramLog(vfProgram);
	}
	glDeleteShader(vShader);
	glDeleteShader(fShader);
	return vfProgram;
}

void generateCirclePoints(vec2 center_point, GLfloat r, GLint num_segments) 
{
	GLfloat		x, y;
	GLfloat		full_circle = 2.0f * M_PI;
	GLfloat		alpha = full_circle / (GLfloat)num_segments;
	for (int i = 0; i <= num_segments; i++) {
		x = center_point.x + 2 * r * cos(i * alpha);
		y = center_point.y + 2 * r * sin(i * alpha);
		circlePoints.push_back(vec3(x / 600, y / 600, 0));
	}
}


void init(GLFWwindow* window) 
{
	renderingProgram = createShaderProgram();
	generateCirclePoints(vec2(0.0f, 0.0f), 50.0f, 300);
	linePoints.push_back(vec3(-1.0f / 3, 0, 0));
	linePoints.push_back(vec3(1.0f / 3, 0, 0));
	glGenBuffers(numVBOs, VBO);
	glGenVertexArrays(numVAOs, VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, circlePoints.size() * sizeof(vec3), circlePoints.data(), GL_STATIC_DRAW);
	glBindVertexArray(VAO[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	circleCenter = glGetUniformLocation(renderingProgram, "circleCenter");
	Xoffset = glGetUniformLocation(renderingProgram, "offsetX");
	Yoffset = glGetUniformLocation(renderingProgram, "offsetY");

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, linePoints.size() * sizeof(vec3), linePoints.data(), GL_STATIC_DRAW);
	glBindVertexArray(VAO[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	Vonal = glGetUniformLocation(renderingProgram, "Vonal");
	vonalYoffset = glGetUniformLocation(renderingProgram, "lineOffsetY");
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(renderingProgram);
	glClearColor(0.7, 0.7, 0.0, 0.7);
}

void display(GLFWwindow* window, double currentTime) 
{
	glClear(GL_COLOR_BUFFER_BIT);
	glBindVertexArray(VAO[0]);
	glUniform1i(Vonal, false);
	if (elinditva) 
	{
		x += xPattogoSebesseg;
		y += yPattogoSebesseg;
		korKozepe.x = x;
		korKozepe.y = y;

		float thresholdX = korAtlo / (window_width / static_cast<float>(2));
		float thresholdY = korAtlo / (window_height / static_cast<float>(2));

		if (x > (1.0f - thresholdX) || x < (-1.0f + thresholdX))
			xPattogoSebesseg = -xPattogoSebesseg;
		if (y > (1.0f - thresholdY) || y < (-1.0f + thresholdY))
			yPattogoSebesseg = -yPattogoSebesseg;
	}
	else
	{
		x += xAlapSebesseg;
		y += yAlapSebesseg;
		korKozepe.x = x;
		korKozepe.y = y;

		float thresholdX = korAtlo / (window_width / static_cast<float>(2));
		float thresholdY = korAtlo / (window_height / static_cast<float>(2));

		if (x > (1.0f - thresholdX) || x < (-1.0f + thresholdX))
			xAlapSebesseg = -xAlapSebesseg;
		if (y > (1.0f - thresholdY) || y < (-1.0f + thresholdY))
			yAlapSebesseg = -yAlapSebesseg;
	}
	float offsets[] = { korKozepe.x, korKozepe.y };
	GLint locations[] = { Xoffset, Yoffset };
	for (int i = 0; i < 2; ++i) 
		glProgramUniform1f(renderingProgram, locations[i], offsets[i]);
	glProgramUniform3f(renderingProgram, circleCenter, korKozepe.x, korKozepe.y, 0);
	glDrawArrays(GL_TRIANGLE_FAN, 0, circlePoints.size());
	glBindVertexArray(VAO[1]);
	glLineWidth(3.0);
	glUniform1i(Vonal, true);
	glProgramUniform1f(renderingProgram, vonalYoffset, vonalPozicioY);
	glDrawArrays(GL_LINES, 0, linePoints.size());
	glBindVertexArray(0);
}

void cleanUpScene() 
{
	glDeleteVertexArrays(numVAOs, VAO);
	glDeleteBuffers(numVBOs, VBO);
	glDeleteProgram(renderingProgram);
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) 
{
	window_width = width;
	window_height = height;
	glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) 
{
	if (action == GLFW_PRESS) 
	{
		if (key == GLFW_KEY_UP) vonalPozicioY += 0.02f;
		if (key == GLFW_KEY_DOWN) vonalPozicioY -= 0.02f;
		if (key == GLFW_KEY_S) elinditva = true;
	}
}
int main(void) 
{
	if (!glfwInit())
		exit(EXIT_FAILURE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	window = glfwCreateWindow(window_width, window_height, window_title, nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	if (glewInit() != GLEW_OK)
		exit(EXIT_FAILURE);
	glfwSwapInterval(1);
	glfwSetWindowSizeLimits(window, 600, 600, 600, 600);
	glfwSetWindowAspectRatio(window, 1, 1);
	init(window);
	while (!glfwWindowShouldClose(window)) 
	{
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	cleanUpScene();
	return EXIT_SUCCESS;
}