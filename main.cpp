#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <assert.h>
#include "objloader.hpp"

#define BUFFER_OFFSET( offset )		((GLvoid*) (offset))

using namespace std;

GLuint InitShader(const char* vShaderFile, const char* fShaderFile);

const float DivideByZeroTolerance = 1e-7f;
typedef glm::vec4  color4;
typedef glm::vec4  point4;
typedef glm::mat4  mat4;
typedef glm::vec4  vec4;
typedef glm::vec3  vec3;


std::vector<glm::vec3> vertices;
std::vector<glm::vec3> normals;

GLfloat RIGHT, LEFT, TOP, BOTTOM, zNear, zFar;

typedef struct {
	int width;
	int height;
	const char* title;
} glutWindow;

glutWindow win;

GLuint  ModelView, Projection;
mat4 model_view, projection;
int name = 1;

void Initialize() {
	RIGHT = 0.07;
	LEFT = -0.07;
	TOP = 0.1;
	BOTTOM = -0.1;
	zNear = 0.0;
	zFar = 10.0;
}


void init()
{
	loadOBJ("test1.obj", vertices, normals);

	std::cout << "LoadOBJ done" << std::endl;

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float) * 3 + normals.size() * sizeof(float) * 3,
		NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float) * 3, &vertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float) * 3,
		normals.size() * sizeof(float) * 3, &normals[0]);

	GLuint program = InitShader("vshader.glsl", "fshader.glsl");
	glUseProgram(program);

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0,
		BUFFER_OFFSET(vertices.size() * sizeof(float) * 3));

	
	point4 light_position(200.0, 200.0, 0.0, 0.0);
	color4 light_ambient(0.1, 0.1, 0.1, 1.0);
	color4 light_diffuse(0.3, 0.3, 0.3, 1.0);
	color4 light_specular(1.0, 1.0, 1.0, 1.0);

	color4 material_ambient(1.0, 1.0, 1.0, 1.0);
	color4 material_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 material_specular(1.0, 1.0, 1.0, 1.0);
	float  material_shininess = 5.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"),
		1, (const GLfloat*)&ambient_product[0]);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"),
		1, (const GLfloat*)&diffuse_product[0]);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"),
		1, (const GLfloat*)&specular_product[0]);

	glUniform4fv(glGetUniformLocation(program, "LightPosition"),
		1, (const GLfloat*)&light_position[0]);

	glUniform1f(glGetUniformLocation(program, "Shininess"),
		material_shininess);

	ModelView = glGetUniformLocation(program, "ModelView");
	Projection = glGetUniformLocation(program, "Projection");
	
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.0, 0.0, 0.0, 1.0); 
}


void snapshot(int windowWidth, int windowHeight, const char* filename)
{
	unsigned char* bmpBuffer = (unsigned char*)malloc(windowWidth*windowHeight * 3);
	if (!bmpBuffer)
		return;

	glReadPixels((GLint)0, (GLint)0,
		(GLint)windowWidth - 1, (GLint)windowHeight - 1,
		GL_BGR, GL_UNSIGNED_BYTE, bmpBuffer);

	cout << bmpBuffer[0] << endl;

	errno_t err;
	FILE *filePtr;
	err = fopen_s(&filePtr, filename, "wb");
	
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;

	bitmapFileHeader.bfType = 0x4D42; 
	bitmapFileHeader.bfSize = windowWidth * windowHeight * 3;
	bitmapFileHeader.bfReserved1 = 0;
	bitmapFileHeader.bfReserved2 = 0;
	bitmapFileHeader.bfOffBits =
		sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfoHeader.biWidth = windowWidth - 1;
	bitmapInfoHeader.biHeight = windowHeight - 1;
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biBitCount = 24;
	bitmapInfoHeader.biCompression = BI_RGB;
	bitmapInfoHeader.biSizeImage = 0;
	bitmapInfoHeader.biXPelsPerMeter = 0; 
	bitmapInfoHeader.biYPelsPerMeter = 0; 
	bitmapInfoHeader.biClrUsed = 0;
	bitmapInfoHeader.biClrImportant = 0;

	fwrite(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	fwrite(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	fwrite(bmpBuffer, windowWidth*windowHeight * 3, 1, filePtr);
	fclose(filePtr);

	free(bmpBuffer);

}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	vec3 at(0.0, 0.0, 0.0);
	vec3 eye(0.0, -1.0, 1.0);
	vec3 up(0.0, 1.0, 0.0);

	model_view = glm::lookAt(eye, at, up);
	
	glUniformMatrix4fv(ModelView, 1, GL_FALSE, (const GLfloat*)&model_view[0][0]);
	glUniformMatrix4fv(Projection, 1, GL_FALSE, (const GLfloat*)&projection[0][0]);

	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
	
	glutSwapBuffers();
}


void TimerCallBack(int)
{
	glutTimerFunc(33, TimerCallBack, 0);
	glutPostRedisplay();
}


void InitializeGlutCallbacks()
{
	glutDisplayFunc(display);
	glutTimerFunc(33, TimerCallBack, 0);
}


void reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	GLfloat aspect = GLfloat(width) / height;

	if (aspect > 1.0) {
		LEFT *= aspect;
		RIGHT *= aspect;
	}
	else {
		TOP /= aspect;
		BOTTOM /= aspect;
	}
	projection = glm::ortho(LEFT, RIGHT, BOTTOM, TOP, zNear, zFar);
}


void keyboard(unsigned char key, int x, int y)
{
	char filename[100];
	switch (key) {
	case 033: // Escape Key
	case 'q': case 'Q':
		exit(EXIT_SUCCESS);
		break;
	case 'w':
		TOP -= 0.01;
		BOTTOM -= 0.01;
		projection = glm::ortho(LEFT, RIGHT, BOTTOM, TOP, zNear, zFar);
		break;
	case 'a':
		RIGHT += 0.01;
		LEFT += 0.01;
		projection = glm::ortho(LEFT, RIGHT, BOTTOM, TOP, zNear, zFar);
		break;
	case 's':
		TOP += 0.01;
		BOTTOM += 0.01;
		projection = glm::ortho(LEFT, RIGHT, BOTTOM, TOP, zNear, zFar);
		break;
	case 'd':
		RIGHT -= 0.01;
		LEFT -= 0.01;
		projection = glm::ortho(LEFT, RIGHT, BOTTOM, TOP, zNear, zFar);
		break;
	}
	sprintf_s(filename, "result_%d.bmp", name);
	snapshot(win.width, win.height, filename);
	name++;
}


int main(int argc, char **argv)
{
	win.width = 640;
	win.height = 480;
	win.title = "OpenGL/GLUT OBJ Loader.";

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(win.width, win.height);
	//glutInitContextVersion(3, 2);
	//glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Test");

	glewInit();

	InitializeGlutCallbacks();
	
	Initialize();

	init();

	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMainLoop();
	return 0;
}
