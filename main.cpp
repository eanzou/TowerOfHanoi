#include <windows.h>
#include <GL/glut.h>

void display(); //display function
void init(); //self initialisation
void reshape(GLsizei width, GLsizei height); //reshape function
void printInstruction();
void keyboard(unsigned char key, int x, int y); //rotate whole model
void numberRelease (unsigned char key, int x, int y); //detect key release
void arrow (int key, int x,int y); //move disks
void movability (bool movable[]);
void movingDown (bool moveDown[]);
void mouseClick (int button, int state, int x, int y);
void timerFunction (int value); //zooming in and out
void confetti();
void resetConfetti();
GLUquadricObj *quad;

float zoom = 1.0;

//control rotation of the whole diagram
static int rotateX = 0;
static int rotateY = 0;

//torus
static float diskY[3] = {-0.5, -0.6, -0.7};
static float diskX[3] = {-0.7, -0.7, -0.7};

//confetti positions for resetting
const float V1[21][3] = {{0.1, 0.0, 1.0}, {0.3, 0.7, 1.0}, {-0.5, 0.6, 1.0}, {0.9, 0.8, 1.0}, {0.8, -0.8, 1.0}, {-0.4, 0.4, 1.0},
                        {-0.8, 0.7, 1.0}, {1.0, 0.4, 1.0}, {-1.4, -1.3, 1.0}, {1.4, 0.0, 1.0}, {2.4, -0.4, 1.0},
                        {-2.0, -0.8, 1.0}, {-0.4, -0.5, 1.0}, {-1.5, 0.0, 1.0}, {1.7, 0.9, 1.0}, {-2.4, 0.8, 1.0},
                        {2.0, 0.5, 1.0}, {-0.9, 0.1, 1.0}, {-1.0, -0.4, 1.0}, {-1.8, -0.8, 1.0}, {2.0, -0.8, 1.0}};
const float V2[21][3] = {{0.0, -0.1, 1.0}, {0.2, 0.6, 1.0}, {-0.6, 0.5, 1.0}, {1.0, 1.0, 1.0}, {0.9, -0.9, 1.0}, {-0.5, 0.5, 1.0},
                        {-0.9, 0.7, 1.0}, {1.1, 0.4, 1.0}, {-1.6, -1.2, 1.0}, {1.3, 0.0, 1.0}, {2.5, -0.4, 1.0},
                        {-2.1, -0.7, 1.0}, {-0.3, -0.6, 1.0}, {-1.5, 0.1, 1.0}, {1.8, 0.8, 1.0}, {-2.4, 0.9, 1.0},
                        {2.1, 0.7, 1.0}, {-0.8, 0.0, 1.0}, {-0.9, -0.3, 1.0}, {-1.7, -0.8, 1.0}, {2.2, -0.9, 1.0}};
class Colours
{
public:
    //Colours of the pole
    float poleColours[3][3] = {{0.25,0.34,0.8},
                                {0.2,0.5,0.8},
                                {0.35,0.25,0.67}};

    //Colour of the base
    float baseColour[3] = {0.36, 0.25, 0.2};

    //Colour of the disks
    float diskColours[3][3] = {{1.0,0.5,0.3},
                                {1.0, 0.6, 0.8},
                                {1.0, 0.0, 0.2}};

    float cloudColour[3] = {1.0, 1.0, 1.0};
};

class Cylinders : public Colours
{
public:
    void drawPole();
    void drawPShadow();
private:
    float x[3] = {-0.7, 0.0, 0.7}; //x-axis location poles
};

class Cuboid: public Colours
{
public:
    void drawBase();
private:
    float x = 1.16; //length of base
    float y = 0.05; //height of base
    float z = 0.46; //size of base
    //normals for 6 faces to know direction it is facing (for lighting purpose)
    float normal[6][3] = {{0.0, 1.0, 0.0}, {0.0, -1.0, 0.0}, {0.0, 0.0, 1.0},
                        {0.0, 0.0, -1.0}, {-1.0, 0.0, 0.0}, {1.0, 0.0, 0.0}};
};

class Disks: public Colours
{
public:
    void drawDisks (float, float, float, int); //torus shape
    void drawDShadow(float, float, int);
};

class Cloud: public Colours
{
public:
    void drawCloud();
private:
    float cloudY[15] = {-0.9, -0.86, -0.96, -0.86, -0.86, -0.86, -1.01, -1.01, -0.86, -0.86, -0.91, -0.96, -0.86, -0.91, -0.88};
    float cloudX[15] = {-1.1, -0.8, -0.75, -0.6, -0.3, -0.3, 0.0, 0.3, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 1.1};
    float cloudZ[15] = {0.0, 0.3, 0.25, -0.2, -0.3, 0.2, 0.0, 0.15, 0.3, 0.2, -0.3, -0.2, -0.1, 0.1, 0.0};
};

void Cylinders::drawPole()
{
    for (int i=0; i<3; i++)
    {
        glColor3fv(poleColours[i]); //cylinder body
        glPushMatrix();
        glTranslatef(x[i],-0.76,0);
        glRotatef(-90, 1.0, 0.0, 0.0); //rotate vertically
        gluCylinder(quad, 0.05f, 0.05f, 0.85f, 50, 50);
        glPopMatrix();

        glPushMatrix(); //cylinder bottom cover
        glTranslatef(x[i],-0.76,0);
        glRotatef(-90, 1.0, 0.0, 0.0); //rotate horizontally to cover cylinder bottom
        gluDisk(quad,0,0.05,50,50);
        glPopMatrix();

        glPushMatrix(); //cylinder top cover
        glTranslatef(x[i],0.09,0);
        glRotatef(-90, 1.0, 0.0, 0.0); //rotate horizontally to cover cylinder top
        gluDisk(quad,0,0.05,50,50);
        glPopMatrix();
    }
}

void Cylinders::drawPShadow()
{
    for(int i=0; i<3; i++)
    {
        glColor4f(0.1,0.1,0.1,0.0); //black
        glPushMatrix();
        glTranslatef(x[i],-.756,0);
        glRotatef(30, 0.0, 1.0, 0.0);

        glBegin(GL_QUADS);
            glVertex3f(-0.05, 0, 0);
            glVertex3f(0.05, 0, 0);
            glVertex3f(0.05, 0, -0.5);
            glVertex3f(-0.05, 0, -0.55);
        glEnd();

        glPopMatrix();
    }
}

void Cuboid::drawBase()
{
    glPushMatrix();
    glTranslatef(0.0,-0.81,0); //lower the base

    glColor3fv(baseColour);
    glBegin(GL_QUADS); //draw quads to form a cuboid base
    glNormal3fv(&normal[0][0]);
    glVertex3f(-x, y, z); //top quad
    glVertex3f(-x, y, -z);
    glVertex3f(x, y, -z);
    glVertex3f(x, y, z);

    glNormal3fv(&normal[1][0]);
    glVertex3f(-x, -y, z); //bottom quad
    glVertex3f(-x, -y, -z);
    glVertex3f(x, -y, -z);
    glVertex3f(x, -y, z);

    glNormal3fv(&normal[2][0]);
    glVertex3f(-x, -y, z); //front quad
    glVertex3f(-x, y, z);
    glVertex3f(x, y, z);
    glVertex3f(x, -y, z);

    glNormal3fv(&normal[3][0]);
    glVertex3f(-x, -y, -z); //back quad
    glVertex3f(-x, y, -z);
    glVertex3f(x, y, -z);
    glVertex3f(x, -y, -z);

    glNormal3fv(&normal[4][0]);
    glVertex3f(-x, -y, -z); //left quad
    glVertex3f(-x, -y, z);
    glVertex3f(-x, y, z);
    glVertex3f(-x, y, -z);

    glNormal3fv(&normal[5][0]);
    glVertex3f(x, -y, z);
    glVertex3f(x, -y, -z); //right quad
    glVertex3f(x, y, -z);
    glVertex3f(x, y, z);

    glEnd();
    glPopMatrix();
}

void Disks::drawDisks(float DiskX, float DiskY, float DiskW, int i)
{
    glColor3fv(diskColours[i]);
    glPushMatrix();
    glTranslatef(DiskX,DiskY,0);
    glRotatef(90, 1.0, 0.0, 0.0);
    glutSolidTorus(0.06,DiskW,25,25);
    glPopMatrix();
}

float width[3] = {0.0, 0.0, 0.0};

void Disks:: drawDShadow(float DiskX, float DiskW, int i)
{
    glColor4f(0.1,0.1,0.1,0.0); //black
    glPushMatrix();
    glTranslatef(DiskX-0.09, -0.8, -0.09); //reposition the shadow according to light
    glRotatef(90, 1.0, 0.0, 0.0);
    glutSolidTorus(0.06, DiskW, 25, 25);
    glPopMatrix();
}

void Cloud::drawCloud()
{
    for (int i=0; i<15; i++)
    {
        glColor3fv(cloudColour);
        glPushMatrix();
        glTranslatef(cloudX[i],cloudY[i],cloudZ[i]);
        glRotatef(90, 1.0, 0.0, 0.0);
        gluCylinder(quad, 0.5f, 0.5f, 0.2f, 50, 50);
        glPopMatrix();

        glPushMatrix(); //cloud top cover
        glTranslatef(cloudX[i],cloudY[i],cloudZ[i]);
        glRotatef(-90, 1.0, 0.0, 0.0);
        gluDisk(quad,0,0.5,50,50);
        glPopMatrix();

        glPushMatrix(); //cloud bottom cover
        glTranslatef(cloudX[i],cloudY[i]-0.2,cloudZ[i]);
        glRotatef(-90, 1.0, 0.0, 0.0);
        gluDisk(quad,0,0.5,50,50);
        glPopMatrix();
    }
}

Cylinders pole;
Cuboid base;
Disks disk_1;
Disks disk_2;
Disks disk_3;
Cloud cloud;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers

    glPushMatrix();

    printInstruction(); //display instructions

    //confetti conditions
    if (diskY[0] <= -0.5 && diskY[1] <= -0.6 && diskY[2] <= -0.65) //if they are ordered correctly
        if ((diskX[0] == diskX[1] && diskX[0] == diskX[2] && diskX[0] > 0.6) || //all on third pole
            (diskX[0] == diskX[1] && diskX[0] == diskX[2] && diskX[0] == 0.0)) //all on middle pole
            confetti();

    glRotatef((GLfloat)rotateX, 1.0, 0.0, 0.0 ); //rotate x direction
    glRotatef((GLfloat)rotateY, 0.0, 1.0, 0.0 ); //rotate y direction

    glScalef(zoom, zoom, zoom); //scaling TOH to make it bigger or smaller

    pole.drawPole(); //draw the three poles
    pole.drawPShadow();

    disk_1.drawDisks(diskX[0], diskY[0], 0.1, 0); //draw the smallest torus
    disk_2.drawDisks(diskX[1], diskY[1], 0.2, 1); //draw the second biggest torus
    disk_3.drawDisks(diskX[2], diskY[2], 0.3, 2); //draw the biggest torus
    disk_1.drawDShadow(diskX[0], 0.1+width[0], 0);
    disk_2.drawDShadow(diskX[1], 0.2+width[1], 1);
    disk_3.drawDShadow(diskX[2], 0.3+width[2], 2);

    base.drawBase(); //draw the base
    cloud.drawCloud(); //draw cloud

    glPopMatrix(); //in state before all transformation

    glutSwapBuffers();
}

//confetti
static float confetV1[21][3];
static float confetV2[21][3];
static float Ccolour1[2][3] = {{1.0, 0.0, 0.0}, {1.0, 0.0, 0.5}};
static float Ccolour2[2][3] = {{1.0, 1.0, 0.0}, {1.0, 0.5, 0.3}};

void confetti()
{
    glLineWidth(10.0); //width of line
    for (int i=0 ;i<10; i++)
    {
        glBegin(GL_LINES);
        glColor3fv(Ccolour1[0]); //first set of colour
        glVertex3fv(confetV1[i]);
        glColor3fv(Ccolour2[0]);
        glVertex3fv(confetV2[i]);
        glEnd();
    }

    for (int i=10 ;i<21; i++)
    {
        glBegin(GL_LINES);
        glColor3fv(Ccolour1[1]); //second set of colour
        glVertex3fv(confetV1[i]);
        glColor3fv(Ccolour2[1]);
        glVertex3fv(confetV2[i]);
        glEnd();
    }
}

void resetConfetti()
{
    //initialize and reset confetti position
    for (int i=0; i<21; i++)
        for (int j=0; j<3; j++)
        {
            confetV1[i][j] = V1[i][j];
            confetV2[i][j] = V2[i][j];
        }
}

void printInstruction()
{
    //set y coordinate of each set of instruction
    float zoomY = 0.25;
    float rotateY = -0.15;
    float moveY = 0.75;
    float etc = -1.3;

    glColor3f(0.0, 0.0, 0.0); //black
    char titleStr[15] = "Tower Of Hanoi";
    char str[14][50] = {"Zooming In/Out", "Left click mouse - zoom in", "Right click mouse - zoom out",
                        "Rotate 3D Model", "w - rotate upwards", "a - rotate to the left", "s - rotate downwards",
                        "d - rotate to the left", "Moving Disks", "Long press '1' with arrow keys - smallest disk",
                        "Long press '2' with arrow keys - medium disk", "Long press '3' with arrow keys - largest disk",
                        "Spacebar - reset view and disks", "Esc - exit"};
    int titleLength;
    titleLength = strlen(titleStr); //obtain length of string

    //title
    glRasterPos3f(-0.33, 0.9, 0.0); //position of words
    for (int i=0; i<titleLength; i++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, titleStr[i]); //print each character
    }

    int length[14];
    for (int i=0; i<14; i++)
        length[i] = strlen(str[i]);

    for (int i = 0; i < 14; i++)
    {
        if (i < 3)
        {
            glRasterPos3f(-2.4, zoomY, 0.0);
            for (int j=0; j<length[i]; j++)
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i][j]);
            zoomY -= 0.1;
        }
        if (i>2 && i<8)
        {
            glRasterPos3f(-2.4, rotateY, 0.0);
            for (int j=0; j<length[i]; j++)
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i][j]);
            rotateY -= 0.1;
        }
        if (i>7 && i<12)
        {
            glRasterPos3f(-2.4, moveY, 0.0);
            for (int j=0; j<length[i]; j++)
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i][j]);
            moveY -= 0.1;
        }
        if (i > 11)
        {
            glRasterPos3f(1.3, etc, 0.0);
            for (int j=0; j<length[i]; j++)
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, str[i][j]);
            etc -= 0.1;
        }
    }
}

void init()
{
    glClearColor(0.9 ,1, 1, 0.0); //light blue background
    glClearDepth(1.0f); //background depth furthest

    quad = gluNewQuadric();

    glEnable(GL_DEPTH_TEST); //enable depth testing

    GLfloat position[] = {4.0, 2.0, 5.0, 0.0}; //light position
    glLightfv(GL_LIGHT0, GL_POSITION, position); //change position of light

    glEnable(GL_LIGHTING); //enable lighting
    glEnable(GL_LIGHT0); //enable light 0 (white light)
    glEnable(GL_NORMALIZE); //normalize models
    glShadeModel(GL_SMOOTH);   //Enable smooth shading
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  //Nice perspective corrections
    glEnable(GL_COLOR_MATERIAL); //trace back original colour

    glShadeModel(GL_SMOOTH);
    glEnable(GL_LINE_SMOOTH); //enable smooth line
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST); //nice smooth lines
}

void reshape(GLsizei width, GLsizei height)
{
    glViewport(0,0,width,height); //to cover the new window

    glMatrixMode(GL_PROJECTION); //operate 3D projection view
    glLoadIdentity();
    glOrtho(-2.5, 2.5, -1.5, 1.0, 1, 10); //2D orthographic view

    glMatrixMode(GL_MODELVIEW); // Model-view matrix
    glLoadIdentity();
    gluLookAt(0.0,1.0,6.0,0.0,0.0,0.0,0.0,1.0,0.0); //look at front, up and far position
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
    //glutInitWindowPosition(300, 100);
    //glutInitWindowSize(700, 550);
    glutCreateWindow("Tower of Hanoi");
    init(); //initialization
    glutReshapeFunc(reshape); //reshape function
    resetConfetti(); //set confetti position
    glutDisplayFunc(display); //display function
    glutFullScreen(); //set full screen
    glutKeyboardUpFunc(numberRelease);
    glutSpecialFunc(arrow); //rotate whole diagram
    glutKeyboardFunc(keyboard); //keyboard function
    glutMouseFunc(mouseClick); //zoom in and out of the diagram
    glutTimerFunc(100, timerFunction, 0);
    glutMainLoop();

    return 0;
}

bool buffer [256]; //buffer to keep keyboard letters (specifically 1,2 and 3)

void numberRelease (unsigned char key, int x, int y)
{
    buffer[key] = false; //when key is released set to false
}

void keyboard (unsigned char key, int x, int y)
{
    buffer[key] = true; //when key is pressed, set to true

     //rotate the model 360 degrees
	switch(key)
	{
        case 'w':
            rotateX = (rotateX - 5) % 360; //go back to zero if hit 360 degree
            glutPostRedisplay();
            break;

        case 's':
            rotateX = (rotateX + 5) % 360;
            glutPostRedisplay();
            break;

        case 'a':
            rotateY = (rotateY - 5) % 360;
            glutPostRedisplay();
            break;

        case 'd':
            rotateY = (rotateY + 5) % 360;
            glutPostRedisplay();
            break;
        case ' ': //reset view back to normal
            //reset zoom
            zoom = 1.0;
            //reset rotation
            rotateY = 0;
            rotateX = 0;
            //reset disk position
            diskY[0] = -0.5; diskX[0] = -0.7;
            diskY[1] = -0.6; diskX[1] = -0.7;
            diskY[2] = -0.7; diskX[2] = -0.7;
            //reset confetti position
            resetConfetti();
            glutPostRedisplay();
            break;
        case 27:
            exit(0);
        default:
            break;
	}

}

void arrow(int key, int x, int y)
{
    bool moveable[] = {true, false, false};
    float upper_limit = 0.2;
    float lower_limit = -0.7;
    float left_limit = -0.7;
    float right_limit = 0.7;
    float pole_height = 0.1;
    float moveX = 0.7;
    float moveY = 0.1;
    bool moveDown[] = {false, false, true};
    int i=-1; //disk i

    if (buffer['1']) i=0;
    else if (buffer['2']) i=1;
    else if (buffer['3']) i=2;

    movability(moveable);  //smaller disk on top of bigger disk and they are in the same pole
    movingDown(moveDown); //if there is a disk below, the disk on top cannot go down

    if (buffer['1'] || buffer['2'] || buffer['3'] ) //if user press either 1,2 or 3
    {
        if (moveable[i])
        {
            if (key == GLUT_KEY_UP && diskY[i] < upper_limit) //arrow up
            {
                diskY[i] += moveY;
                if (i==0)
                    width[0] += 0.002; //increase width of shadow when go up
                if (i==1)
                    width[1] += 0.002;
                if (i==2)
                    width[2] += 0.001;
            }

            if (key == GLUT_KEY_DOWN && diskY[i] > lower_limit) //arrow down
            {
                if (i==0)
                    if (moveDown[0])
                    {
                        diskY[i] -= moveY;
                        width[0] -= 0.002; //decrease width of shadow when go down
                    }

                if (i==1)
                    if (moveDown[1])
                    {
                        diskY[i] -= moveY;
                        width[1] -= 0.002;
                    }

                if (i==2)
                    if (moveDown[2])
                    {
                        diskY[i] -= moveY;
                        width[2] -= 0.001;
                    }
            }

            if (key == GLUT_KEY_LEFT && diskX[i] > left_limit && diskY[i] > pole_height) //arrow left
            diskX[i] -= moveX;

            if (key == GLUT_KEY_RIGHT && diskX[i] < right_limit && diskY[i] > pole_height) //arrow right
            diskX[i] += moveX;
    }
        glutPostRedisplay();
    }
}

void movability (bool moveable[])
{
    //bigger disk below smaller disk and on the same pole, make the bigger disk unmovable
    if (diskY[0] >= diskY[1] && diskX[0] == diskX[1])
        moveable[1] = false;
    else
        moveable[1] = true;

    if ((diskY[0] >= diskY[2] && diskX[0] == diskX[2]) ||
        (diskY[1] >= diskY[2] && diskX[1] == diskX[2]))
        moveable[2] = false;
    else
        moveable[2] = true;
}

void movingDown (bool moveDown[])
{
    //if there is a disk below, the disk on top cannot go down
    float rightBelow_0 = 0.2;
    float rightBelow_1 = 0.1;

    //prevent small disk sinking down
    if (((diskY[0]-diskY[1] <= rightBelow_0) && diskX[1] == diskX[0]) || //small exactly on top of medium
        ((diskY[0]-diskY[2] <= rightBelow_1) && diskX[2] == diskX[0])) //small exactly on top of large
        moveDown[0] = false;
    else
        moveDown[0] = true;

    //prevent medium disk sinking down
    if ((diskY[1]-diskY[2] <= rightBelow_1) && diskX[2] == diskX[1]) //medium exactly on top of large
        moveDown[1] = false;
    else if ((diskY[0] < diskY[1]) && (diskX[0] == diskX[1])) //if the pole already have a smaller disk inside
        moveDown[1] = false; //don't let medium disk move down
    else
        moveDown[1] = true;

    if ((diskY[0] < diskY[2] && diskX[0] == diskX[2]) ||
        (diskY[1] < diskY[2] && diskX[1] == diskX[2])) //if the pole already have a smaller disk inside
        moveDown[2] = false; //don't let largest disk move down
    else
        moveDown[2] = true;
}

bool zoomIn = false;
bool zoomOut = false;

void mouseClick (int button, int state, int x, int y)
{
    if (state == GLUT_DOWN) //when button pressed set as true
    {
        switch (button)
        {
            case GLUT_LEFT_BUTTON: //mouse left button
                zoomIn = true;
                break;
            case GLUT_RIGHT_BUTTON: //mouse right button
                zoomOut = true;
                break;
            default:
                break;
        }
    }

    if (state == GLUT_UP) //when button is not pressed set as false
    {
        switch (button)
        {
            case GLUT_LEFT_BUTTON:
                zoomIn = false;
                break;
            case GLUT_RIGHT_BUTTON:
                zoomOut = false;
                break;
            default:
                break;
        }
    }
}

void timerFunction(int value)
{
    //zooming
    if (zoomIn) //TOH bigger whenever left button pressed and not released
        zoom+=0.05;
    if (zoomOut) //TOH smaller whenever right button pressed and not released
    {
        if (zoom > 0.05) //prevent it from going out of screen
            zoom-=0.05;
    }

    //confetti
    if (diskY[0] <= -0.5 && diskY[1] <= -0.6 && diskY[2] <= -0.65) //if the disks are ordered correctly
        if ((diskX[0] == diskX[1] && diskX[0] == diskX[2] && diskX[0] > 0.6) || //all disks on third pole
            (diskX[0] == diskX[1] && diskX[0] == diskX[2] && diskX[0] == 0.0)) //all disks on second pole
            for (int i=0; i<21; i++) //make all the confetti fall
            {
                if (confetV1[i][1] >= -1.3) //limit the y of the confetti (prevent it from going off screen)
                    confetV1[i][1] -= 0.015; //decrease y coordinate of one vertex

                if (confetV2[i][1] >= -1.3)
                    confetV2[i][1] -= 0.015; //decrease y coordinate of another vertex
            }

    glutPostRedisplay();
    glutTimerFunc(100, timerFunction, 0);
}
