/*
	Project Assignment6
*/
#define _CRT_SECURE_NO_WARNINGS

#include "glut.h"
#include <iostream>
#include <stdio.h>
#include <vector>
#include "glui.h"
#include <string>


using namespace std;
# define PI		3.14159265358979323846



//GLUI VARS
GLUI_Spinner* segment_spinner;
GLUI_Listbox* listbox;

GLUI_Checkbox* controlPolyCheckbox;
int controlPolyCheckboxBool;

GLUI_Checkbox* DeCastelijauCheckbox;
int DeCastelijauCheckboxBool;

GLUI_Checkbox* CurveCheckbox;
int CurveCheckboxBool;

GLUI_Checkbox* ftCheckbox;
int ftCheckboxBool;

GLUI_Checkbox* gridCheckbox;
int gridCheckboxBool;

GLUI_Checkbox* wirespherePolyCheckbox;
int wirespherePolyCheckboxBool;

GLUI_Checkbox* ctrlPointsToggleCheckbox;
int ctrlPointsToggleCheckboxBool;

GLUI_Checkbox* ticksCheckbox;
int ticksCheckboxBool;

GLUI_Checkbox* multBezierCheckbox;
int multBezierCheckboxBool;

GLUI_Spinner* s_spinner;
GLUI_Spinner* t_extend;


class Point {
public:
	GLfloat xcord = 0;
	GLfloat ycord = 0;
	GLfloat zcord = 0;
	float rcolor = 1.0;
	float bcolor = 0.0;
	float gcolor = 0.0;

	GLfloat tangentxcord = -1.0;
	GLfloat tangentycord = 0;
	GLfloat tangentzcord = 0;


};

int  main_window;; //variable that hold main window
GLfloat xRotated, yRotated, zRotated;
GLdouble radius = 1.0;
double xcord = 0;
double ycord = 0;

float xpoint = 0;
float ypoint = 0;

float m[16];

BOOLEAN addPoint = false;
BOOLEAN addtangent = true;
BOOLEAN lmbd = false;

BOOLEAN clipLeft = false;
BOOLEAN clipRight = false;

BOOLEAN extendLeft = false;
BOOLEAN extendRight= false;


float timevar= 0.26f;
float tVar = 0.26;
float hermitTime = 0.56;
float sval = 0.5;
float tExtend = 0.0;

int wireframe;
int segments;

vector<Point> vec;
vector<Point> tempSlerp;
vector<float>hermitPoints;


void drawCubeLocation(GLfloat xcenter, GLfloat ycenter, GLfloat size, GLfloat zpos, float rcolor, float gcolor, float bcolor) {

	glColor3f(rcolor, gcolor, bcolor);


	glBegin(GL_POLYGON); //front face


	glVertex3f(xcenter - size, ycenter + size, zpos - size); //top left

	glVertex3f(xcenter - size, ycenter - size, zpos - size); //bottom left

	glVertex3f(xcenter + size, ycenter - size, zpos - size); //bottom right

	glVertex3f(xcenter + size, ycenter + size, zpos - size); //top right

	glEnd();

	glBegin(GL_POLYGON);//back face
	glVertex3f(xcenter + size, ycenter + size, zpos + size); //top right
	glVertex3f(xcenter + size, ycenter - size, zpos + size); //bottom right
	glVertex3f(xcenter - size, ycenter - size, zpos + size); //bottom left
	glVertex3f(xcenter - size, ycenter + size, zpos + size); //top left
	glEnd();



	glBegin(GL_POLYGON); //left face
	glVertex3f(xcenter - size, ycenter - size, zpos - size); //bottom left
	glVertex3f(xcenter - size, ycenter + size, zpos - size); //top left
	glVertex3f(xcenter - size, ycenter + size, zpos + size); //top left
	glVertex3f(xcenter - size, ycenter - size, zpos + size); //bottom left
	glEnd();

	glBegin(GL_POLYGON);//right face
	glVertex3f(xcenter + size, ycenter + size, zpos - size); //top right
	glVertex3f(xcenter + size, ycenter - size, zpos - size); //bottom right
	glVertex3f(xcenter + size, ycenter - size, zpos + size); //bottom right
	glVertex3f(xcenter + size, ycenter + size, zpos + size); //top right
	glEnd();

	glBegin(GL_POLYGON);//top face
	glVertex3f(xcenter + size, ycenter + size, zpos - size); //top right
	glVertex3f(xcenter + size, ycenter + size, zpos + size); //top right
	glVertex3f(xcenter - size, ycenter + size, zpos + size); //top left
	glVertex3f(xcenter - size, ycenter + size, zpos - size); //top left
	glEnd();

	glBegin(GL_POLYGON);//bottom face
	glVertex3f(xcenter + size, ycenter - size, zpos - size); //bottom right
	glVertex3f(xcenter - size, ycenter - size, zpos - size); //bottom left
	glVertex3f(xcenter - size, ycenter - size, zpos + size); //bottom left
	glVertex3f(xcenter + size, ycenter - size, zpos + size); //bottom right
	glEnd();

	//glFlush();
}

void select(int index) {
	for (int i = 0; i < vec.size(); i++) {
		if (i == index) {
			vec.at(i).gcolor = 1.0;
			vec.at(i).rcolor = 0.0;
		}
		else {
			vec.at(i).rcolor = 1.0;
			vec.at(i).gcolor = 0.0;
		}
	}
}

void slerp(vector<Point> vec) {
	if (vec.size() == 1) {
		tempSlerp.clear();
		return;
	}

	float t = timevar;
	float cubex;
	float cubey;
	float cubez;
	BOOLEAN drawCube = false;
	BOOLEAN specialColor = false;

	vector<Point> tempVec;
	for (int q = 0; q < vec.size() - 1; q++) { //use all points in this stage to create the points in the next stage
		Point p0 = vec.at(q);
		Point p1 = vec.at(q + 1);

		float len0 = sqrt((pow(p0.xcord, 2)) + (pow(p0.ycord, 2)) + (pow(p0.zcord, 2))); //magnitude of V1
		float len1 = sqrt((pow(p1.xcord, 2)) + (pow(p1.ycord, 2)) + (pow(p1.zcord, 2))); //magnitude of V2

		p0.xcord = p0.xcord / len0; //normalize V1 and V2
		p0.ycord = p0.ycord / len0;
		p0.zcord = p0.zcord / len0;

		p1.xcord = p1.xcord / len1;
		p1.ycord = p1.ycord / len1;
		p1.zcord = p1.zcord / len1;



		float dot = (p0.xcord * p1.xcord) + (p0.ycord * p1.ycord) + (p0.zcord * p1.zcord); //dot product of Normalized vectors

		float angle = acos(dot / (1*1)); //angle between two Normalized vectors IE len of both = 1

		Point nextPoint;
		float alpha = (sin(1 - t) * angle) / sin(angle); //alpha equation
		float beta = sin(t * angle) / sin(angle); //bet equation
		//calculates based on current t value from loop in display func

		nextPoint.xcord = alpha * p0.xcord + beta * p1.xcord; //calculate coords of next point using current P0 and P1
		nextPoint.ycord = alpha * p0.ycord + beta * p1.ycord;
		nextPoint.zcord = alpha * p0.zcord + beta * p1.zcord;


		tempVec.push_back(nextPoint); //store next point for next stage of computation

		glColor3f(0.0, 0.5, 0.0); //set to green for lines

		if (DeCastelijauCheckboxBool && (abs(t-tVar)<0.005)) { //only draw if check box is marked AND current iteration matches the current spinner t value
			glBegin(GL_LINE_STRIP);
		}
		float counter = 0.0;
		float stop = 1.0;
		if (extendLeft) {
			counter = tExtend;
			stop = 1.0;
		}
		else if (extendRight) {
			counter = 0.0;
			stop = tExtend;
		}
		else {
			counter = 0.0;
			stop = 1.0;
		}


		for (counter; counter <= stop+0.05; counter += 0.05f) {
			//this represents T steps
			float alpha = sin((1 - counter) * angle) / sin(angle); //calculate alpha for current t
			float beta = sin(counter * angle) / sin(angle); //calculate beta for current t
			float xcord = alpha * p0.xcord + beta * p1.xcord;
			float ycord = alpha * p0.ycord + beta * p1.ycord;
			float zcord = alpha * p0.zcord + beta * p1.zcord;

			/*glGetFloatv(GL_MODELVIEW_MATRIX, m);
			 xcord = (xcord * m[0] + ycord * m[1] + zcord * m[2]);
			 ycord = xcord * m[4] + ycord * m[5] + zcord * m[6];
			 zcord = xcord * m[8] + ycord * m[9] + zcord * m[10];
			 */

			if (DeCastelijauCheckboxBool && (abs(t - tVar) < 0.005)) {
				glVertex3f(xcord, ycord, zcord);

			}
			if (CurveCheckboxBool) {
				//draw curve
				if (vec.size() == 2) {
					if (abs(counter - timevar) <= 0.05) {
						//point to "save"

						float tempxcord = xcord;
						float tempycord = ycord;
						float tempzcord = zcord;


						cubex = xcord;
						cubey = ycord;
						cubez = zcord;


						Point tempPoint;


						if (ftCheckboxBool && (abs(t-tVar)<0.01)) {
							//draw in other color!
							specialColor = true;
						}

						//cubex = xcordDraw;
						//cubey = ycordDraw;
						//cubez = zcordDraw;


						if (!clipLeft && !clipRight) {
							//no clip enabled
							drawCube = true;
							
						}
						else if (clipLeft && clipRight) {
							//should never be reached
							printf("both clips activated Err\n");
							drawCube = true;
							
						}

						else if (clipLeft && counter > sval) {
							drawCube = true;
							
						}

						else if (clipRight && counter <= sval) {
							drawCube = true;
							
						}
						if (drawCube) {
							tempPoint.xcord = cubex;
							tempPoint.ycord = cubey;
							tempPoint.zcord = cubez;
						}
						tempSlerp.push_back(tempPoint);
						
					}
				}
			}

		}//end for


		if (DeCastelijauCheckboxBool && (abs(t - tVar) < 0.01)) {
			glEnd();
		}
		glBegin(GL_LINE_STRIP);
		for (int p = 0; p < tempSlerp.size(); p++) {
			glVertex3f(tempSlerp.at(p).xcord, tempSlerp.at(p).ycord, tempSlerp.at(p).zcord);
		}
		glEnd();

		//store xyz for cube draw
		if (drawCube) {
			if (specialColor) {
				printf("inside special color\n");
				drawCubeLocation(cubex, cubey, 0.02, cubez, 255.0, 105.0, 180.0);
				specialColor = false;
			}
			else {
				//curve points
				drawCubeLocation(cubex, cubey, 0.01, cubez, 0.0,0.0,1.0);
			}
			drawCube = false;
		}


	}

	slerp(tempVec);  //redo slerp with vec loaded with next stage points
}//end slerp
void init() {
	// Set initial OpenGL states
	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shiniess[] = { 20.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

}



void reshape(int x, int y)
{
	if (y == 0 || x == 0) return;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-1.0, 1.0, -1.0,1.0, -10.0, 10.0);

	//gluPerspective(39.0, (GLdouble)x / (GLdouble)y, 0.1, 21.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glViewport(0, 0, x, y);  //Use the whole window for rendering
}

void hermit(vector<Point>vec,int i) {
	if (vec.at(i).tangentxcord == -1 || vec.at(i+1).tangentxcord == -1) {
		//tangents are not defined
		return;
	}
	for (float time = 0; time < 1.0;time +=0.05) {
		hermitTime = time;
		float p0x = vec.at(i).xcord;
		float p0y = vec.at(i).ycord;
		float p0z = vec.at(i).zcord;


		float p0tanx = vec.at(i).tangentxcord;
		float p0tany = vec.at(i).tangentycord;
		float p0tanz = vec.at(i).tangentzcord;


		float p1x = vec.at(i+1).xcord;
		float p1y = vec.at(i+1).ycord;
		float p1z = vec.at(i+1).zcord;


		float p1tanx = vec.at(1).tangentxcord;
		float p1tany = vec.at(1).tangentycord;
		float p1tanz = vec.at(1).tangentzcord;

		//curve equation 
		//y(t) = (2t^3 -3t^2 +1)P0 + (-2t^3 +3t^2)P1 + (t^3 -2t^2 +t)p0Tangent + (t^3-t^2) P1Tangent

		float alpha = (2 * pow(hermitTime, 3)) - (3 * pow(hermitTime, 2)) + 1;
		float beta = (-2 * pow(hermitTime, 3) + 3 * pow(hermitTime, 2));
		float charlie = (pow(hermitTime, 3) - 2 * pow(hermitTime, 2) + hermitTime);
		float delta = pow(hermitTime, 3) - pow(hermitTime, 2);

		float xcord = (alpha * p0x) + (beta * p1x) + (charlie * p0tanx) + (delta * p1tanx);
		float ycord = (alpha * p0y) + (beta * p1y) + (charlie * p0tany) + (delta * p1tany);
		float zcord = (alpha * p0z) + (beta * p1z) + (charlie * p0tanz) + (delta * p1tanz);


		float tempxcord = xcord;
		float tempycord = ycord;
		float tempzcord = zcord;

		//xcord = (tempxcord * m[0] + tempycord * m[1] + tempzcord * m[2]); //fix points to align with rotation
		//ycord = tempxcord * m[4] + tempycord * m[5] + tempzcord * m[6];

		//zcord = tempxcord * m[8] + tempycord * m[9] + tempzcord * m[10];

		hermitPoints.push_back(xcord);
		hermitPoints.push_back(ycord);
		hermitPoints.push_back(zcord);

	}
}


// display function
void display() {

	//glMatrixMode(GL_MODELVIEW);
	// clear the drawing buffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// clear the identity matrix.
	glLoadIdentity();
	// traslate the draw by z = -4.0
	// Note this when you decrease z like -8.0 the drawing will looks far , or smaller.
	glTranslatef(0.0, 0.0, -4.0);
	// Red color used to draw.
	//glColor3f(0.9, 0.3, 0.2);
	// changing in transformation matrix.
	// rotation about X axis


	glRotatef(xRotated, 1.0, 0.0, 0.0);
	// rotation about Y axis


	glRotatef(yRotated, 0.0, 1.0, 0.0);
	// rotation about Z axis


	glRotatef(zRotated, 0.0, 0.0, 1.0);
	// scaling transfomation 

	glScalef(1.0, 1.0, 1.0);
	// built-in (glut library) function , draw you a sphere.
	glColor3f(1.0, 1.0, 0.0); ///yellow sphere


	if (controlPolyCheckboxBool) {
		//draw sphere
		if (wirespherePolyCheckboxBool && gridCheckboxBool) {
			//wire and grid = only wire
			glutWireSphere(radius, 20, 20);
		}
		else if (!wirespherePolyCheckboxBool && gridCheckboxBool) {
			//solid sphere + grid
			glutSolidSphere(radius, 20, 20);

			glColor3f(0.0, 0.0, 1.0);//blue grid
			glutWireSphere(radius + 0.01, 20, 20);
		}
		else if (!gridCheckboxBool && wirespherePolyCheckboxBool) {
			glutWireSphere(radius, 20, 20);
		}
		else {
			glutSolidSphere(radius, 20, 20);
			//solid sphere no grid
		}
	}
	//glFlush();
	//use combo of solid + slightly bigger wire sphere to map "grid" to sphere

	// Flush buffers to screen
	if (addPoint) {
		//derive from x^2 + y^2 + z^2 = r^2
		double zval = sqrt(abs(pow(xpoint, 2) + pow(ypoint, 2) - pow(radius, 2)));
		glGetFloatv(GL_MODELVIEW_MATRIX, m);
		float xval = (xpoint * m[0] + ypoint * m[1] + zval * m[2]); //fix points to align with rotation
		float yval = xpoint * m[4] + ypoint * m[5] + zval * m[6];

		float zvalFinal = xpoint * m[8] + ypoint * m[9] + zval * m[10];
		if (addtangent) {
			if (vec.size() != 0) {
				vec.back().tangentxcord = xval;
				vec.back().tangentycord = yval;
				vec.back().tangentzcord = zvalFinal;
			}
		}

		else {
			Point point;
			point.xcord = xval;
			point.ycord = yval;
			point.zcord = zvalFinal;
			vec.push_back(point);
		}
		addPoint = false;

	}//end add point if


	for (int i = 0; i < vec.size(); i++) {
		glBegin(GL_POLYGON);
		double tempxcord = vec.at(i).xcord;
		double tempycord = vec.at(i).ycord;
		double tempzcord = vec.at(i).zcord;

		if (!ctrlPointsToggleCheckboxBool) {
			//only draw if checkbox is NOT checked
			drawCubeLocation(tempxcord, tempycord, 0.02, tempzcord, vec.at(i).rcolor, vec.at(i).bcolor, vec.at(i).gcolor);
		}

	}
	
	for (int j = 0; j < vec.size(); j++) {
		//draw tangent lines
		if (vec.at(j).tangentxcord == -1.0) {
			break;
		}

		float tanx = vec.at(j).tangentxcord;
		float tany = vec.at(j).tangentycord;
		float tanz = vec.at(j).tangentzcord;
		Point p0 = vec.at(j);



		drawCubeLocation(vec.at(j).tangentxcord, vec.at(j).tangentycord, 0.02, vec.at(j).tangentzcord, 1.0,0.41,0.705);
		
		glBegin(GL_LINE_STRIP);
		glColor3f(1.0, 0.41, 0.705);
		glVertex3f(vec.at(j).xcord, vec.at(j).ycord, vec.at(j).zcord);


		float len0 = sqrt((pow(p0.xcord, 2)) + (pow(p0.ycord, 2)) + (pow(p0.zcord, 2))); //magnitude of V1
		float len1 = sqrt((pow(tanx, 2)) + (pow(tany, 2)) + (pow(tanz, 2))); //magnitude of V2

		p0.xcord = p0.xcord / len0; //normalize V1 and V2
		p0.ycord = p0.ycord / len0;
		p0.zcord = p0.zcord / len0;

		tanx = tanx / len1;
		tany = tany / len1;
		tanz = tanz / len1;



		float dot = (p0.xcord * tanx) + (p0.ycord * tany) + (p0.zcord * tanz); //dot product of Normalized vectors

		float angle = acos(dot / (1 * 1)); //angle between two Normalized vectors IE len of both = 1

		for (float i = 0; i < 1.0; i += 0.05) {
			float alpha = sin((1 - i) * angle) / sin(angle); //calculate alpha for current t
			float beta = sin(i * angle) / sin(angle); //calculate beta for current t
			float xcord = alpha * p0.xcord + beta * tanx;
			float ycord = alpha * p0.ycord + beta * tany;
			float zcord = alpha * p0.zcord + beta * tanz;
			glVertex3f(xcord, ycord, zcord);
		}


		
		glVertex3f(vec.at(j).tangentxcord, vec.at(j).tangentycord, vec.at(j).tangentzcord);

		glEnd();
		//glFlush();


	}
	if (vec.size() > 1) {

		for (int i = 0; i < vec.size() - 1; i++) {
			hermit(vec,i);
		}
		if (hermitPoints.size() > 0) {
			glBegin(GL_LINE_STRIP);

			for (int index = 0; index < hermitPoints.size(); index += 3) {
				glVertex3f(hermitPoints.at(index), hermitPoints.at(index + 1), hermitPoints.at(index+2));
			}
			glEnd();
			hermitPoints.clear();
		}
	}
	if (vec.size() > 3){
		for (int vecIndex = 1; vecIndex < vec.size()-2; vecIndex++) {
			//feed slerp 4 points at a time
			vector<Point>cubicSlerp;
			
			if (multBezierCheckboxBool) {
				cubicSlerp.push_back(vec.at(vec.size() - vecIndex)); //get last point in vec which was first drawn
				cubicSlerp.push_back(vec.at(vec.size() - vecIndex - 1));
				cubicSlerp.push_back(vec.at(vec.size() - vecIndex - 2));
				cubicSlerp.push_back(vec.at(vec.size() - vecIndex - 3));
			}
			else {
				cubicSlerp = vec;
			}
			if (extendRight) {
				if (tExtend < 1.0) {
					//no longer valid to extend right
					extendRight = false;
				}
			}

			if (extendLeft) {
				if (tExtend > 0.0) {
					extendLeft = false;
				}
			}
			if (extendLeft) {
				for (float i = tExtend; i <= 1.0; i += 0.01) { // t =[textend,1]
					timevar = i; //TOGGLE FOR INTERMEDIATE INTERPOLATIONS
					//slerp(vec);
					slerp(cubicSlerp);
				}
			}
			else if (extendRight) {

				for (float i = 0.0; i <= tExtend; i += 0.01) { // t =[0,textend]
					timevar = i; //TOGGLE FOR INTERMEDIATE INTERPOLATIONS
					//slerp(vec);
					slerp(cubicSlerp);

				}
			}
			else {

				for (float i = 0.0; i <= 1.0; i += 0.01) { // t = [0,1] no extension
					timevar = i; //TOGGLE FOR INTERMEDIATE INTERPOLATIONS
					//slerp(vec);
					slerp(cubicSlerp);

				}
			}
			cubicSlerp.clear();
		}//end for
	}//end vec size if
	

	//glFlush();
	// sawp buffers called because we are using double buffering 
   // glutSwapBuffers();

}
/*
Function invoked when an event on a regular keys occur.
*/
void keyboard(unsigned char k, int x, int y)
{
	// Close application if ESC is pressed 
	if (k == 27)
	{
		exit(0);
	};
	
}




/*
Mouse Handler
*/
void mouseClick(int button, int mode, int x, int y) {
	// event happens once on pushing the button and once more when releasing it
	// button identifies what you clicked, mode if its down or up
	int z = glutGetModifiers();

	if (button == 0 && mode == 0 && z == 0) {
		//left mouse down no modifiers
		xcord = x;
		ycord = y;
		lmbd = true;

	}

	if (button == 0 && mode == 1) {//release lmb
		lmbd = false;
	}

	if (button == 2 && mode == 0) { //rmb down
		addPoint = true;
		addtangent = !addtangent;

		xpoint = 2.0f * ((GLfloat)x + 0.5f) / (GLfloat)(glutGet(GLUT_WINDOW_WIDTH)) - 1.0f;
		 ypoint = -1 * (2.0f * ((GLfloat)y + 0.5f) / (GLfloat)(glutGet(GLUT_WINDOW_HEIGHT)) - 1.0f);

		glutPostRedisplay();
	}


	if (z == 1 && button == 0 && mode == 0) { //shift Lclick down
		//select point
		lmbd = true;
		float localx = 2.0f * ((GLfloat)x + 0.5f) / (GLfloat)(glutGet(GLUT_WINDOW_WIDTH)) - 1.0f; //convert to same scale as points
		float localy = -1 * (2.0f * ((GLfloat)y + 0.5f) / (GLfloat)(glutGet(GLUT_WINDOW_HEIGHT)) - 1.0f);
		float templocalx = localx;


		double zval = sqrt(abs(pow(localx, 2) + pow(localy, 2) - pow(radius, 2)));
		glGetFloatv(GL_MODELVIEW_MATRIX, m);
		localx = (localx * m[0] + localy * m[1] + zval * m[2]); //fix points to align with rotation/align with x,y, coords as points on sphere

		localy = templocalx * m[4] + localy * m[5] + zval * m[6];

		//float zvalFinal = xpoint * m[8] + ypoint * m[9] + zval * m[10];




		for (int i = 0; i < vec.size(); i++) {
			if ((abs(vec.at(i).xcord - localx) < 0.05) && (abs(vec.at(i).ycord - localy) < 0.05)) { //find first point that is within the tolerance. change its color
				select(i);
				break;
			}
		}
	}

	return;
}
void mouseMotion(int x, int y) {
	// called when the mouse moves
	// active motion means a button is down
	int z = glutGetModifiers();

	if (lmbd && z == 0) {
		GLfloat xdiff = xcord - (GLfloat)x;
		GLfloat ydiff = ycord - (GLfloat)y;
		xcord = x;
		ycord = y;
		if (xdiff >= 0 && ydiff >= 0) {
			yRotated += xdiff;
			xRotated -= ydiff;
		}
		else if (xdiff >= 0 && ydiff <= 0) {
			yRotated += xdiff;
			xRotated -= ydiff;
		}
		else if (xdiff <= 0 && ydiff >= 0) {
			yRotated += xdiff;
			xRotated -= ydiff;
		}
		else if (xdiff <= 0 && ydiff <= 0) {
			yRotated += xdiff;
			xRotated += ydiff;
		}	
	} //end if

	if (z == 1 && lmbd) { //shift being held with left mouse
		//start shifting the selected point;
		int index = -1;
		for (int i = 0; i < vec.size(); i++) {
			if (vec.at(i).gcolor == 1.0) { //selected point
				index = i;
			}
		}//end for
		if (index == -1) { //no point found
			printf("no selected point!\n");
			return;
		}
		float localx = 2.0f * ((GLfloat)x + 0.5f) / (GLfloat)(glutGet(GLUT_WINDOW_WIDTH)) - 1.0f; //convert to same scale as points
		float localy = -1 * (2.0f * ((GLfloat)y + 0.5f) / (GLfloat)(glutGet(GLUT_WINDOW_HEIGHT)) - 1.0f);

		float templocalx = localx;
		float templocaly = localy;



		double zval = sqrt(abs(pow(localx, 2) + pow(localy, 2) - pow(radius, 2)));
		glGetFloatv(GL_MODELVIEW_MATRIX, m);
		localx = (templocalx * m[0] + templocaly * m[1] + zval * m[2]); //fix points to align with rotation/align with x,y, coords as points on sphere

		localy = templocalx * m[4] + templocaly * m[5] + zval * m[6];

		float zvalFinal = templocalx * m[8] + templocaly * m[9] + zval * m[10];


		vec.at(index).xcord = localx;
		vec.at(index).ycord = localy;
		vec.at(index).zcord = zvalFinal;
		

	}
	glutPostRedisplay();


}//end mouse move
void resize(int width, int height) {
	//this is called on window resize
	//params are window width and window height
}


void myGlutIdle(void)
{
	 /*According to the GLUT specification, the current window is
	   undefined during an idle callback.  So we need to explicitly change
	   it if necessary */
	if (glutGetWindow() != main_window)
		glutSetWindow(main_window);

	//glutPostRedisplay();
}

void resetPoints(int val) {
	//called upon reset button click
	//keep all checkboxes same clear Points
	vec.clear();
	glutPostRedisplay();
}

void clip(int val) {
	if (val == 1) {
		//lcip left
		clipLeft = true;
		clipRight = false;
	}
	else if (val == 2) {
		//clip right
		clipLeft = false;
		clipRight = true;
	}
	else if (val == 3) {
		//extend left
		if (tExtend >= 0.0) {
			extendLeft = false;
			extendRight = false;
			return;
		}
		extendLeft = true;
		extendRight = false;
	}
	else if (val == 4) {
		if (tExtend <= 1.0) {
			extendLeft = false;
			extendRight = false;
			return;
		}
		extendLeft = false;
		extendRight = true;
	}
	else if (val == 5) {
		//reset
		clipLeft = false;
		clipRight = false;
		extendLeft = false;
		extendRight = false;
	}
	glutPostRedisplay();
}

/*
The main function.
*/
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(640, 640);
	main_window = glutCreateWindow("Solid Sphere");
	zRotated = 30.0;
	xRotated = 0;
	yRotated = 0;

	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMouseFunc(mouseClick);
	glutMotionFunc(mouseMotion);
	glutKeyboardFunc(keyboard);
	init();

	

	GLUI* glui = GLUI_Master.create_glui("GLUI", 0, 640, 640);
	glui->set_main_gfx_window(main_window);


	glui->add_separator();
	glui->add_statictext("Basic SLERP Bezier");
	glui->add_separator();

	GLUI_Panel* displayPanel = glui->add_panel("Display");
	controlPolyCheckbox = glui->add_checkbox("Control Polygon", &controlPolyCheckboxBool);
	DeCastelijauCheckbox = glui->add_checkbox("DeCastelijau", &DeCastelijauCheckboxBool);
	CurveCheckbox = glui->add_checkbox("Curve", &CurveCheckboxBool);
	ftCheckbox = glui->add_checkbox("show F(t)", &ftCheckboxBool);
	gridCheckbox = glui->add_checkbox("show Grid", &gridCheckboxBool);
	wirespherePolyCheckbox = glui->add_checkbox("Wire Sphere", &wirespherePolyCheckboxBool);
	ctrlPointsToggleCheckbox = glui->add_checkbox("show no Control Points", &ctrlPointsToggleCheckboxBool);
	ticksCheckbox = glui->add_checkbox("show Ticks", &ticksCheckboxBool);
	multBezierCheckbox = glui->add_checkbox("Multiple Bezier", &multBezierCheckboxBool);

	glui->add_separator();
	glui->add_column();
	GLUI_Panel* functionPanel = glui->add_panel("Function Panel");
	segment_spinner = glui->add_spinner("T Val",GLUI_SPINNER_FLOAT, &tVar);
	segment_spinner->set_float_limits(0.0, 1.0);

	s_spinner = glui->add_spinner("S val", GLUI_SPINNER_FLOAT, &sval);
	s_spinner->set_float_limits(0.0, 1.0);

	t_extend = glui->add_spinner("T Extension", GLUI_SPINNER_FLOAT, &tExtend);
	t_extend->set_float_limits(-3.0, 3.0);

	GLUI_Button* reset = glui->add_button("Clear Control Points", 1, resetPoints);

	glui->add_separator();
	glui->add_button("Clip Left", 1, clip);
	glui->add_button("Clip Right", 2, clip);

	glui->add_button("Extend Left", 3, clip);
	glui->add_button("Extend Right", 4, clip);

	glui->add_button("Reset clips & extensions", 5, clip);



	GLUI_Button* quit = glui->add_button("Exit", 0, (GLUI_Update_CB)exit);

	GLUI_Master.set_glutIdleFunc(myGlutIdle);

	glutMainLoop();
	return 0;
}