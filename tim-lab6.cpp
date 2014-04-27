/**************************************************************************************

PSUEDOCODE FOR SHADOW MAPPING:

Currently, this program is just a cube on a platform with a light on the top right near
corner of the cube that rotates with I, J, K, and L. Shadow mapping is not implemented.

To implement shadow mapping, we need to add two more glsl files, a fragment shader for
shadow mapping, and a vertex shader for shadow mapping. We then need to save the shadow
being mapped onto the plane as a texture, and apply it similar to texture mapping. Then
we need to compare depths of the normal camera and the shadow map. The regular vertex
shader and fragment shader render the shapes, and the shadow vertex shader and the
shadow fragment shader render the actual shadow map.

In the regular vertex shader, we need to make another out variable called 
shadow_coordinate which will be the position of the shadow from the shadow camera.

In the regular fragment shader, we need to make another variable for visibility, and
use it when computing out_color.

In the shadow fragment shader, we need to calculate the depth.

In the shadow vertex shader, we need to use the depth to compute the position.

Once all this is done, the shadow map should be working, but there will still be
"shadow acne" and "peter panning" present in the shadow.

***************************************************************************************
*/

#include "timShader.h"

#include "SDL2/SDL.h"

using namespace std;

GLuint abuffer;
GLuint buffer[3];
GLuint ebuffer;
GLuint program;

GLfloat pit = 1;
GLfloat yaw = 1;

GLfloat norm = 1.0f / sqrt(3.0f);

GLfloat vertices[] = {	-5.0f,-5.0f,-5.0f,	//0 Left, Bottom, Far
				5.0f,-5.0f,-5.0f,		//1 Right, Bottom, Far
				5.0f,-5.0f,5.0f,		//2 Right, Bottom, Near
				-5.0f,-5.0f,5.0f,		//3 Left, Bottom, Near
				-5.0f,5.0f,-5.0f,		//4 Left, Top, Far
				5.0f,5.0f,-5.0f,		//5 Right, Top, Far
				5.0f,5.0f,5.0f,		//6 Right, Top, Near
				-5.0f,5.0f,5.0f,		//7 Left, Top, Near
				-20.0f,-5.0f,-20.0f,		//8  Platform: Back, Left
				-20.0f,-5.0f,20.0f,		//9  Platform: Front, Left
				20.0f,-5.0f,20.0f,		//10 Platform: Front, Right
				20.0f,-5.0f,-20.0f	};	//11 Platform: Back, Right

				//R, G, B, A (transparency)
GLfloat colors[] = {	0.0f,0.0f,1.0f,1.0f,		//0 Blue
				0.0f,0.0f,1.0f,1.0f,		//1 Blue
				0.0f,0.0f,1.0f,1.0f,		//2 Blue
				0.0f,0.0f,1.0f,1.0f,		//3 Blue
				0.0f,0.0f,1.0f,1.0f,		//4 Blue
				0.0f,0.0f,1.0f,1.0f,		//5 Blue
				0.0f,0.0f,1.0f,1.0f,		//6 Blue
				0.0f,0.0f,1.0f,1.0f,		//7 Blue
				1.0f,1.0f,1.0f,1.0f,		//8 Platform: White
				1.0f,1.0f,1.0f,1.0f,		//9 Platform: White
				1.0f,1.0f,1.0f,1.0f,		//10 Platform: White
				1.0f,1.0f,1.0f,1.0f	};	//11 Platform: White
				

GLfloat normals[] = {	-norm, -norm, -norm,
				norm, -norm, -norm,
				norm, -norm, norm,
				-norm, -norm, norm,
				-norm, norm, -norm,
				norm, norm, -norm,
				norm, norm, norm,
				-norm, norm, norm,
				-4*norm, -norm, -4*norm,
				-4*norm, -norm, 4*norm,
				4*norm, -norm, 4*norm,
				4*norm, -norm, -4*norm	};

GLubyte elems[] = {	2,3,0,1,2,0,		//Bottom
				5,4,7,6,5,7,		//Top
				6,7,3,2,6,3,		//Front
				4,5,1,0,4,1,		//Back
				7,4,0,3,7,0,		//Left
				5,6,2,1,5,2,		//Right
				8,9,10,8,11,10	};	//Platform


//Declare functions
void init();
void display(SDL_Window* window);
void input(SDL_Window* window);


int main(int argc, char **argv)
{
	//Main method stuff
	SDL_Window *window;	//SDL window
	if(SDL_Init(SDL_INIT_VIDEO) < 0)	//Tries to intitiate SDL
	{
		cerr << "Error, cannot initialize SDL." << endl;
		SDL_Quit();	//Close out of SDL
		exit(0);	//End program
	}

	//Create window
	window = SDL_CreateWindow("CS452-LAB4", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 500, 500, SDL_WINDOW_OPENGL);

	//Check window creation
	if(window == NULL)
	{
		cerr << "Error, cannot create window." << endl;
	}

	SDL_GLContext glcontext = SDL_GL_CreateContext(window);	//Creates opengl context associated with the window
	glewInit();	//Initializes glew

	init();	//Calls function to initialize the shaders and set up buffers

	//Keep looping through to make sure
	while(true)
	{
		input(window);	//Check keyboard input
		display(window);	//Render
	}

	//Close out of SDL stuff
	SDL_GL_DeleteContext(glcontext);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

void display(SDL_Window* window)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);	//Clears the frame buffer

	glm::mat4 trans;	//Matrix for transformations 
	
	trans = glm::rotate(trans, pit, glm::vec3(1, 0, 0));	//rotate the cube around the x axis
	trans = glm::rotate(trans, yaw, glm::vec3(0, 1, 0));	//rotate the cube arround the y axis
	
	GLint tempLoc = glGetUniformLocation(program, "modelMatrix");	//Matrix that handles the transformations
	glUniformMatrix4fv(tempLoc, 1 ,GL_FALSE,&trans[0][0]);
	
	GLfloat ambient[] = {0.5f, 0.5f, 0.5f, 1.0f};
	GLfloat light1_dir[] = {20.0f, 20.0f, 20.0f};
	GLfloat light1_color[] = {1.0f, 1.0f, 0.0f};
	
	tempLoc = glGetUniformLocation(program,"Ambient");
	glUniform4fv(tempLoc,1,ambient);
	tempLoc = glGetUniformLocation(program,"LightColor1");
	glUniform3fv(tempLoc,1,light1_color);
	tempLoc = glGetUniformLocation(program,"LightDirection1");
	glUniform3fv(tempLoc,1,light1_dir);
	tempLoc = glGetUniformLocation(program,"HalfVector1");
	glUniform3fv(tempLoc,1,light1_dir);
	
	glDrawElements(GL_TRIANGLES, 42, GL_UNSIGNED_BYTE, NULL);
	glFlush();	//Makes sure all data is rendered as soon as possible
	SDL_GL_SwapWindow(window);	//Updates the window
}

void input(SDL_Window* window)
{
	SDL_Event event;
	while (SDL_PollEvent(&event))	//Handling the keyboard
	{
		if(event.type == SDL_QUIT)
		{
			exit(0);
		}
		else if(event.type == SDL_KEYDOWN)
		{
			switch(event.key.keysym.sym)
			{
				case SDLK_ESCAPE: exit(0);
				case SDLK_i: pit+=2; break;
				case SDLK_k: pit-=2; break;
				case SDLK_j: yaw+=2; break;
				case SDLK_l: yaw-=2; break;
			}
		}
	}
}

void init()
{
      glEnable(GL_DEPTH_TEST);
      glEnable(GL_COLOR_MATERIAL);
      glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHT1);
	glEnable(GL_NORMALIZE);
      
	program = glCreateProgram();	//Creates program
	initShaders(program);	//Calls the initialize shader function in the header file

	glGenVertexArrays(1, &abuffer);
	glBindVertexArray(abuffer);

	glGenBuffers(3, buffer);

	//Sets up pointers and stuff
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	
	glBindBuffer(GL_ARRAY_BUFFER, buffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	
	glGenBuffers(1, &ebuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elems), elems, GL_STATIC_DRAW);

	//Enables vertex arrays to draw stuff
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
}
