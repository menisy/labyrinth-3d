#include "GLUT/glut.h"
#include <iostream>
#include <stdio.h>
#include <math.h>
#include "TextureLoader.h"

using namespace std;

/* ################################ HEADERS ######################## */
#define PI 3.14159265
#define GRAV 0.010
#define MAX_PITCH 15
#define MAX_ROLL 15
#define WINDOW_W 800
#define WINDOW_H 600 
#define ELASTICITY 0.2// max 1.0
#define FRICTION 0.8 // max 1.0

void anim(void);
void myKeyboard(unsigned char thekey,int mouseX,int mouseY);
void myMouse(int,int);
void display();
float sign(float);
GLuint platformTexture;
GLuint ballTexture;

struct Ball;
struct Block;
struct Platform;
struct Point;
/* ############################## HEADER END ######################## */
/**
 
 Notes on the world:
 The platform surface (when horizontal) is at y=0. So anything you want to draw
 on the surface should be drawn without translation. 
 
 in platform draw, there is a matrix with a comment "World", this means
 platoform and blocks on the platform. Not sure if it's the best design though,
 but it will work fine for platforms + array of blocks.
 
 The ball is indepedent. Gravity, Friction, Elasticity affects it in the update 
 method.
 */
float camera_x = 0.0;
float camera_y = 0.0;
float camera_z = 0.0;

struct Point{
    float x,y,z;
};

struct Block{
    float size;
    float *texture;
    
    void draw(){
        glutSolidCube(size);
    }
};
struct Platform{
    float width;
    float roll;
    float pitch;
    
    void draw(){
        
        glPushMatrix(); // world
        glRotatef(roll, 0.0, 0.0, -1.0);
        glRotatef(pitch, 1.0, 0.0, 0.0);
        glPushMatrix(); // platform
        glScaled(width, 1, width);
        glTranslated(0, -0.5, 0);
        glColor3f(1,1,1);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, platformTexture);
        //  glutSolidCube(1);
        glBegin(GL_QUADS);
        glTexCoord2f(0,0);
        glVertex3f(1.0,0.5,1.0);
        glTexCoord2f(0,1);
        glVertex3f(1.0,0.5,-1.0);
        glTexCoord2f(1,0);
        glVertex3f(-1.0,0.5,-1.0);
        glTexCoord2f(1,1);
        glVertex3f(-1.0,0.5,1.0);
        glEnd();
        glBegin(GL_QUADS);
        glVertex3f(1.0,-0.5,1.0);
        glVertex3f(1.0,-0.5,-1.0);
        glVertex3f(1.0,0.5,-1.0);
        glVertex3f(1.0,0.5,1.0);
        glEnd();
        glBegin(GL_QUADS);
        glVertex3f(-1.0,-0.5,-1.0);
        glVertex3f(-1.0,-0.5,1.0);
        glVertex3f(-1.0,0.5,1.0);
        glVertex3f(-1.0,0.5,-1.0);
        glEnd();
        glBegin(GL_QUADS);
        glVertex3f(1.0,-0.5,1.0);
        glVertex3f(-1.0,-0.5,1.0);
        glVertex3f(-1.0,0.5,1.0);
        glVertex3f(1.0,0.5,1.0);
        glEnd();
        glBegin(GL_QUADS);
        glVertex3f(-1.0,-0.5,-1.0);
        glVertex3f(1.0,-0.5,-1.0);
        glVertex3f(1.0,0.5,-1.0);
        glVertex3f(-1.0,0.5,-1.0);
        glEnd();
        glBegin(GL_QUADS);
        glTexCoord2f(0,0);
        glVertex3f(1.0,-0.5,1.0);
        glTexCoord2f(0,1);
        glVertex3f(1.0,-0.5,-1.0);
        glTexCoord2f(1,0);
        glVertex3f(-1.0,-0.5,-1.0);
        glTexCoord2f(1,1);
        glVertex3f(-1.0,-0.5,1.0);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        
        
        glPopMatrix(); // end platform
        
        glPopMatrix(); // end world
    }
    
    float get_y(float x, float z){
        float y = 0.0;
        
        // diff due to roll
        y -= sin(roll * PI / 180.0) * x;
        y -= sin(pitch * PI / 180.0) * z;
        return y;
    }
    
}platform = {30, 0.0, 0.0};


struct Ball{
    double weight;
    double x,y,z;
    double rad;
    double delta_x, delta_y, delta_z;
    double rot_x;
    double rot_z;
    
    void update(Platform p){
        
        printf("%f, %f, %f\n", delta_x, delta_y, delta_z);
        // f = m*a; a = f/m;
        double acc_x = GRAV * sin(p.roll * PI/180)/weight;
        double acc_y;
        
        double acc_y_1 = -1 * GRAV * cos(p.roll * PI/180)/weight;
        double acc_y_2 = -1 * GRAV * cos(p.pitch * PI/180)/weight;
        acc_y = acc_y_1+acc_y_2;
        
        double acc_z = GRAV * sin(p.pitch * PI/180)/weight;
        
        acc_x = acc_x > 0? acc_x - FRICTION*acc_x : acc_x + FRICTION*acc_x;
        acc_z = acc_z > 0? acc_z - FRICTION*acc_z : acc_z + FRICTION*acc_z;
        
        delta_x += acc_x;
        //        delta_y = acc_y;
        delta_z += acc_z;
        
        
        // Collision detection
        // platform
        
        if (x + delta_x < platform.width && x + delta_x > -1 * platform.width){
            x += delta_x;
        }else{
            delta_x = -1 * ELASTICITY * delta_x;
        }
        
        
        y = 0.0;
        if (z + delta_z < platform.width && z+delta_z > -1 * platform.width){
            z += delta_z;
        }else{
            delta_z = -1 * ELASTICITY * delta_z;
        }
        // end platform
        
        // circum = 2 pi r
        
        rot_x += delta_x  *30;// XXX hardcoded, can get it physically
        if (rot_x > 360)
            rot_x -=360;
        
        if (rot_x < -360)
            rot_x +=360;
        
        rot_z += delta_z *30; // XXX hardcoded, can get it physically
        if (rot_z > 360)
            rot_z -=360;
        
        if (rot_z < -360)
            rot_z +=360;
        
        printf("\t%.3f, %.3f\n", rot_x, rot_z);
        
    }
    
    void draw(){
        float specReflection[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        glMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);
        glMateriali(GL_FRONT, GL_SHININESS, 1);
        
        glPushMatrix(); // start ball
        glColor3f(0.6, 0.6, 0.6);
        glTranslated(x,rad, z);
        
        
        glRotatef(rot_x,0,0,-1);
        glRotatef(rot_z,1,0,0);
        
        
        GLUquadricObj * qobj;
        qobj = gluNewQuadric();
        gluQuadricDrawStyle(qobj,GLU_FILL);
        gluQuadricTexture(qobj,GL_TRUE); 
        gluQuadricNormals(qobj, GLU_SMOOTH);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, ballTexture);
        gluSphere(qobj, rad, 20, 10);
        gluDeleteQuadric(qobj);
        glDisable(GL_TEXTURE_2D);
        glPopMatrix(); // end ball
        
    }
};
Ball ball = {2.0, 0.0, 5.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0, 0};

void display(void)
{
    glMatrixMode(GL_PROJECTION); // set the view volume shape
    glLoadIdentity();
    gluPerspective(50, //Field of view
                   (WINDOW_W)*1.0/(WINDOW_H)*1.0, //Aspect ratio
                   0.1, // Z near
                   1000.0);// Z far
//    
    double factor = 1.0;
    glOrtho(10/factor, 10/factor, -10/factor, -10/factor, 0.1, 500);
    glMatrixMode(GL_MODELVIEW); // position and aim the camera

    glEnable(GL_DEPTH_TEST);
    glLoadIdentity();
    gluLookAt(0.0,85.0,0.1, // eye
              0.0,0.0,0.0,
              0.0, 1.0, 0.0); // normal
    
    glRotatef(camera_x, 1.0, 0.0, .0);
    glRotatef(camera_y, 0.0, 1.0, .0);
    glRotatef(camera_z, 0.0, 0.0, 1.0);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float pos[] = {0,200,0};
    float dif[] = {.3,.3,.3,3};
    float spec[] = {0.7,0.7,0.7,1};
    float amb[] = {1,1,1,1};
    glLightfv(GL_LIGHT0, GL_AMBIENT,amb );
    glLightfv(GL_LIGHT0,GL_DIFFUSE,dif);
    glLightfv(GL_LIGHT0,GL_POSITION,pos);
    glLightfv(GL_LIGHT0,GL_SPECULAR, spec);
    
    glPushMatrix(); // everything
    
    glColor3f(0.0,0,0);
    glutWireCube(50);
    
    platform.draw();
    
    glRotatef(platform.roll, 0.0, 0.0, -1.0);
    glRotatef(platform.pitch, 1.0, 0.0, 0.0);
    ball.draw();
    
    glPopMatrix(); // end everything
    glFlush();
}
//<<<<<<<<<<<<<<<<<<<<<< main >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
int main(int argc, char **argv)
{
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH );
    
    glutInitWindowSize(WINDOW_W, WINDOW_H);
    
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Transformation testbed - wireframes");
    glutDisplayFunc(display);
    platformTexture = loadTexture("/pngs/wood_texture.png",256,192);
    ballTexture = loadTexture("/pngs/chess_texture.png",1200,1200);
    //    GLfloat light_diffuse[] = {0.5, 0.5, 0.5};
    //    float light_position[] = {10.0, 10.5, 10.5, 0.0};
    
    
    //    glLightfv(GL_LIGHT0, GL_AMBIENT, light_diffuse);
    //    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    //    glShadeModel(GL_SMOOTH);
    //    
    //    glEnable(GL_LIGHT0);
    //    glEnable(GL_LIGHTING);
    
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    
    glutKeyboardFunc(myKeyboard);
    glutPassiveMotionFunc(myMouse);
    glClearColor(1.0f, 1.0f, 1.0f,0.0f); // background is white
	glutIdleFunc(anim);
    glViewport(0, 0, WINDOW_H, WINDOW_W);
    
    glutMainLoop();
}

void myKeyboard(unsigned char thekey,int mouseX,int mouseY){
    switch(thekey){
            // camera
        case 'x':
            camera_x += 5;
            camera_x= (int)camera_x %360;
            break;
        case 'y':
            camera_y += 5;
            camera_y= (int)camera_y %360;
            break;
        case 'z':
            camera_z += 5;
            camera_z= (int)camera_z %360;
            break;
            
        case 'X':
            camera_x -= 5;
            camera_x= (int)camera_x %360;
            break;
        case 'Y':
            camera_y -= 5;
            camera_y= (int)camera_y %360;
            break;
        case 'Z':
            camera_z -= 5;
            camera_z= (int)camera_z %360;
            break;
        case 'r':
            ball.x = 0;
            ball.z = 0;
            ball.delta_x = 0;
            ball.delta_z = 0;
            break;
    }
}

void myMouse(int x, int y){
    float temp_w = WINDOW_W/2;
    float temp_h = WINDOW_H/2;
    if (x > WINDOW_W)
        x = WINDOW_W;
    
    if (x < 0)
        x = 0;
    
    if (y > WINDOW_H)
        y = WINDOW_H;
    
    if (y < 0)
        y = 0;
    platform.roll = (x-temp_w)/temp_w * MAX_ROLL;
    platform.pitch = (y-temp_h)/temp_h * MAX_PITCH;
    //    printf("%.3f, %.3f\n", platform.roll, platform.pitch);
}

void anim(){
    glutPostRedisplay();
    ball.update(platform);
}

float sign(float x){
    return x >= 0? 1:-1;
}
