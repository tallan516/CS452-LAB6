#ifndef TIMSHADER_H_
#define TIMSHADER_H_

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "GL/gl.h"
#include "GL/glu.h"
#include "SDL2/SDL.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <iostream>
#include <string.h>
using namespace std;

//Declare functions
void compileShader(GLuint shader);
void attachShaders(GLuint shader1, GLuint shader2, GLuint program);
const GLchar* inputShader(const char* filename);

void initShaders(GLuint program)
{
	GLuint vshader = glCreateShader(GL_VERTEX_SHADER);	//Creates vertex shader
	const GLchar* vshaderSource = inputShader("vertexshader.glsl");	//Gets the location of the shader file
	glShaderSource(vshader, 1, &vshaderSource, NULL);	//Links shader to file
	compileShader(vshader);	//Calls function below to compile shader

	GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);	//Creates fragment shader
	const GLchar* fshaderSource = inputShader("fragmentshader.glsl");	//Gets the location of the shader file
	glShaderSource(fshader, 1, &fshaderSource, NULL);	//Links shader to file
	compileShader(fshader);	//Calls function below to compile shader

	attachShaders(vshader, fshader, program);	//Calls function below to attach shaders
	glUseProgram(program);	//Installs the program object to OpenGL's current rendering state

	glm::mat4 view;	//Matrix for the view of the camera
	view = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 50.0f),	//Camera position
		glm::vec3(0.0f, 0.0f, 0.0f),	//Camera target
		glm::vec3(0.0f, 1.0f, 0.0f) );	//Up-vector (0, 1, 0) is normal view, (0, -1, 0) is upside down

	GLint tempLoc = glGetUniformLocation(program, "viewMatrix");	//The view matrix handles the camera movement
	glUniformMatrix4fv(tempLoc, 1, GL_FALSE, &view[0][0]);
	  
	glm::mat4 mainProjMatrix;
	mainProjMatrix = glm::perspective(57.0, 1.0, .1, 500.0);	//Matrix that handles the orthographic or perspective viewing
	tempLoc = glGetUniformLocation(program, "Matrix");
	glUniformMatrix4fv(tempLoc, 1, GL_FALSE, &mainProjMatrix[0][0]);
}

void rotateCamera(float x, float y, GLuint program)
{
	glm::mat4 view;	//Matrix for the view of the camera
	view = glm::lookAt(
	glm::vec3(x, y, 50.0),	//Camera position
	glm::vec3(0.0f, 0.0f, 0.0f),	//Camera target
	glm::vec3(0.0f, 1.0f, 0.0f) );	//Up-vector (0, 1, 0) is normal view, (0, -1, 0) is upside down
	
	GLint tempLoc = glGetUniformLocation(program, "viewMatrix");	//The view matrix handles the camera movement
	glUniformMatrix4fv(tempLoc, 1, GL_FALSE, &view[0][0]);
}

void compileShader(GLuint shader)
{
	glCompileShader(shader);	//Compiles shader
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);	//Gets value for if shader compiled or not
	if(!compiled)	//If the shader did not compile, outputs an error message and exits
	{	
		GLint logSize;//variable for size of the debug info
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);//returns the size of the the source file into the variable
		
		GLchar* infoLog = new GLchar[logSize+1];//allocating memory for the debug info
		glGetShaderInfoLog(shader,logSize,&logSize,infoLog);//returns the error messages into the variable infoLog
		cerr << "The shader failed to compile." << infoLog << endl;
		exit(EXIT_FAILURE);
	}
}

void attachShaders(GLuint shader1, GLuint shader2, GLuint program)
{
	glAttachShader(program, shader1);	//Attaches shader1 to program
	glAttachShader(program, shader2);	//Attaches shader2 to program
	glBindAttribLocation(program, 0, "in_position");	//binds the location an attribute to a program
	glBindAttribLocation(program, 1, "in_color");	//binds the location an attribute to a program
	glBindAttribLocation(program, 2, "normal");

	glLinkProgram(program);//links program to your program //weird

	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);	//Gets value for if program/shader attached

	if(!linked)	//If the program/shader did not attach, outputs an error message and exits
	{
		GLint logSize;//variable for size of the debug info
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);//returns the linking status into the variable
		    
		GLchar *infoLog = new GLchar[logSize+1];//allocating memory for the debug info
		glGetProgramInfoLog(program,logSize,&logSize,infoLog);//returns the error messages into the variable infoLog
		cerr << "Shader program failed to link." << infoLog << endl;
		exit(EXIT_FAILURE);
	}
}

//Taken directly from Alex's example
//this funtion loads the shader from the vertex, fragments shaders
const GLchar* inputShader(const char* filename)
{
	FILE* fshade = fopen(filename, "rb");//opens file
	if(!fshade)	//check to see if file is opened
	{
		fprintf(stderr,"unable to open file '%s'\n",filename);
		return NULL;
	}

	//neat way to get the length of the file
	fseek(fshade, 0, SEEK_END);
	long filesize=ftell(fshade);
	fseek(fshade, 0, SEEK_SET);
	
	//allocates memory for the file and read in the file
	GLchar* shadingSource= new GLchar[filesize+1];//
	fread(shadingSource, 1, filesize, fshade);
	
	if(ftell(fshade) == 0)	//checks to see if the file is empty
	{
		fprintf(stderr, "File '%s' is empty.\n",filename);
		return NULL;
	}
	
	fclose(fshade);//closes file
	
	shadingSource[filesize] = 0;//neat way to set a '\0' at end of file
	
	return const_cast<const GLchar*>(shadingSource);	//overloads the const so the value with change per file
	//NOTE: if the file is unable to open or is empty this function will segmentation fault your program
}

#endif
