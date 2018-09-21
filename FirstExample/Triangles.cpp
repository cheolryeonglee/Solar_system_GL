
///////////////////////////////////////////////////////////////////////
//
// triangles.cpp
//
///////////////////////////////////////////////////////////////////////

using namespace std;

#include <cmath>
#include <iostream>
#include "vgl.h"
#include "LoadShaders.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"

enum VAO_IDs { Triangles, NumVAOs };
enum Buffer_IDs { ArrayBuffer, NumBuffers };
enum Attrib_IDs { vPosition = 0 };

GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
GLuint location;


// For circle
const GLuint circleVertices = 360;
// Triangle
const GLuint triangleVertices = 3;
// Square
const GLuint squareVertices = 4;
// Pentagon
const GLuint pentaVertices = 7;

// Total
const GLuint totalVertices = circleVertices + 
							 triangleVertices + 
							 squareVertices + 
							 pentaVertices;


float translate_value = 0;
float rotate_value = 0;
float alpha = 0;

const float radian = 3.14159f / 180.f;

//---------------------------------------------------------------------
//
// init
//

void
init(void)
{

	ShaderInfo shaders[] = {
		{ GL_VERTEX_SHADER, "triangles.vert" },
		{ GL_FRAGMENT_SHADER, "triangles.frag" },
		{ GL_NONE, NULL }
	};

	GLuint program = LoadShaders(shaders);
	glUseProgram(program);	//My Pipeline is set up
	
	/*
		v000 - v359: Circle
		v360 - v362: Triangle
		v363 - v366: Square
		v367 - v373: Pentagon

		NOTE: (0, 0) is specified for both Circle and Pentagon because GL_TRIANGLE_FAN is used
	*/
	GLfloat vertices[totalVertices][2];

	// Circle
	vertices[0][0] = 0.f;
	vertices[0][1] = 0.f;
	for (int i = 1; i < circleVertices; i++) {
		vertices[i][0] = cos(i * radian);
		vertices[i][1] = sin(i * radian);
	}
	// Triangle
	for (int i = circleVertices; i < circleVertices + triangleVertices; i++) {
		vertices[i][0] = cos(((i * 120) + 90) * radian);
		vertices[i][1] = sin(((i * 120) + 90) * radian);
	}
	// Square
	for (int i = circleVertices + triangleVertices; i < circleVertices + triangleVertices + squareVertices; i++) {
		vertices[i][0] = cos((i * 90) * radian);
		vertices[i][1] = sin((i * 90) * radian);
	}
	// Pentagon
	for (int i = circleVertices + triangleVertices + squareVertices; i < totalVertices - 1; i++) {
		vertices[i][0] = cos(((i * 72) + 18) * radian);
		vertices[i][1] = sin(((i * 72) + 18) * radian);
	}
	vertices[totalVertices - 1][0] = 0.f;
	vertices[totalVertices - 1][1] = 0.f;

	// Change color for each as well
	// Circle
	GLfloat colorData[totalVertices][3];
	for (int i = 0; i < circleVertices; i++) {
		colorData[i][0] = 1.0f;
		colorData[i][1] = 1.0f;
		colorData[i][2] = 1.0f;
	}
	// Triangle
	for (int i = circleVertices; i < circleVertices + triangleVertices; i++) {
		colorData[i][0] = 0.5f;
		colorData[i][1] = 0.4f;
		colorData[i][2] = 0.0f;
	}
	// Square
	for (int i = circleVertices + triangleVertices; i < circleVertices + triangleVertices + squareVertices; i++) {
		if (i % 2 == 0) {
			colorData[i][0] = 0.0f;
			colorData[i][1] = 1.0f;
			colorData[i][2] = 0.0f;
		}
		else {
			colorData[i][0] = 0.0f;
			colorData[i][1] = 0.0f;
			colorData[i][2] = 1.0f;
		}
	}
	// Pentagon
	for (int i = circleVertices + triangleVertices + squareVertices; i < totalVertices - 1; i++) {
		colorData[i][0] = 1.0f;
		colorData[i][1] = 0.0f;
		colorData[i][2] = 0.0f;
	}

	glGenBuffers(2, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[0]);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindAttribLocation(program, 0, "vPosition");
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_STATIC_DRAW);
	glBindAttribLocation(program, 1, "vertexColor");
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1);	

	location = glGetUniformLocation(program, "model_matrix");
}


//---------------------------------------------------------------------
//
// display
//

void
display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);

	// Sun: Pentagon - not that much is going on here
	glm::mat4 sun = glm::scale(glm::mat4(1.0), glm::vec3(0.125f, 0.125f, 1.0f));

	glUniformMatrix4fv(location, 1, GL_FALSE, &sun[0][0]);

	glDrawArrays(GL_TRIANGLE_FAN, circleVertices + triangleVertices + squareVertices, pentaVertices);

	// Earth: Square - as simple as the Sun
	glm::mat4 earth = glm::rotate(glm::mat4(1.0), rotate_value / 2, glm::vec3(0.0f, 0.0f, 1.0f));
	earth = glm::translate(earth, glm::vec3(0.25f, 0.0f, 0.0f));
	earth = glm::rotate(earth, rotate_value / 2, glm::vec3(0.0f, 0.0f, 1.0f));
	earth = glm::scale(earth, glm::vec3(0.03f, 0.03f, 1.0f));

	glUniformMatrix4fv(location, 1, GL_FALSE, &earth[0][0]);

	glDrawArrays(GL_QUADS, circleVertices + triangleVertices, squareVertices);

	// Moon: Circle - x distance of the first translate must match that of the Earth translate
	glm::mat4 moon = glm::rotate(glm::mat4(1.0), rotate_value / 2, glm::vec3(0.0f, 0.0f, 1.0f));
	moon = glm::translate(moon, glm::vec3(0.25f, 0.0f, 0.0f));
	moon = glm::rotate(moon, rotate_value, glm::vec3(0.0f, 0.0f, 1.0f));
	moon = glm::translate(moon, glm::vec3(0.05f, 0.0f, 0.0f));
	moon = glm::rotate(moon, rotate_value / 2, glm::vec3(0.0f, 0.0f, 1.0f));
	moon = glm::scale(moon, glm::vec3(0.01f, 0.01f, 1.0f));

	glUniformMatrix4fv(location, 1, GL_FALSE, &moon[0][0]); 

	glDrawArrays(GL_TRIANGLE_FAN, 0, circleVertices);

	// Jupiter: Triangle - exactly the same concept as the Earth, just difference of numeric values
	glm::mat4 jupiter = glm::rotate(glm::mat4(1.0f), rotate_value / 3, glm::vec3(0.0f, 0.0f, 1.0f));
	jupiter = glm::translate(jupiter, glm::vec3(0.7f, 0.0f, 0.0f));
	jupiter = glm::rotate(jupiter, rotate_value / 2, glm::vec3(0.0f, 0.0f, 1.0f));
	jupiter = glm::scale(jupiter, glm::vec3(0.05f, 0.05f, 1.0f));

	glUniformMatrix4fv(location, 1, GL_FALSE, &jupiter[0][0]);

	glDrawArrays(GL_TRIANGLES, circleVertices, triangleVertices);

	// Moons (Jupiter): Circle - exactly the same concpet as the Moon from the Earth, 
	// just difference of numeric values and loop
	const int numOfMoons = 4;
	glm::mat4 moons[numOfMoons];
	for (int i = 0; i < numOfMoons; i++) {
		moons[i] = glm::rotate(glm::mat4(1.0f), rotate_value / 3, glm::vec3(0.0f, 0.0f, 1.0f));
		moons[i] = glm::translate(moons[i], glm::vec3(0.7f, 0.0f, 0.0f));
		moons[i] = glm::rotate(moons[i], rotate_value / (i + 1), glm::vec3(0.0f, 0.0f, 1.0f));
		moons[i] = glm::translate(moons[i], glm::vec3(0.10f + (i + 1) * 0.04f, 0.0f, 0.0f));
		moons[i] = glm::rotate(moons[i], rotate_value / 2, glm::vec3(0.0f, 0.0f, 1.0f));
		moons[i] = glm::scale(moons[i], glm::vec3(0.01f, 0.01f, 1.0f));

		glUniformMatrix4fv(location, 1, GL_FALSE, &moons[i][0][0]);

		glDrawArrays(GL_TRIANGLE_FAN, 0, circleVertices);
	}

	glFlush();
}

void idle()
{
	rotate_value += 0.001;
	glutPostRedisplay();
}

//---------------------------------------------------------------------
//
// main
//

int
main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(640, 640);
	glutCreateWindow("Hello World");

	glewInit();	//Initializes the glew and prepares the drawing pipeline.

	init();

	glutDisplayFunc(display);

	glutIdleFunc(idle);

	glutMainLoop();
	
	

}
