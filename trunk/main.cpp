#include "GLUT/glut.h"
#include <iostream>
#include <stdio.h>
#include <math.h>
#include "TextureLoader.h"
#include "irrKlang.h"
#include <sstream>

using namespace std;

/* ################################ HEADERS ######################## */
#define PI 3.14159265
#define GRAV 0.010
#define MAX_PITCH 15
#define MAX_ROLL 15
#define WINDOW_W 1024
#define WINDOW_H 768
#define ELASTICITY 0.2// max 1.0
#define FRICTION 0.7 // max 1.0

void anim(void);
void myKeyboard(unsigned char thekey,int mouseX,int mouseY);
void myMouse(int,int);
void display();
float sign(float);
GLuint platformTexture;
GLuint ballTexture;
GLuint holeTexture;
GLuint blockTexture;
GLuint finishTexture;
bool gameover;
bool win;
bool KR_mode;
time_t start;
time_t end;
irrklang::ISoundEngine* engine;



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


struct Hole{
    float x;
    float z;
    bool finish;
    
    void draw(){
        glColor3f(1, 1, 1);
        glPushMatrix();
        GLUquadricObj * qobj;
        qobj = gluNewQuadric();
        gluQuadricDrawStyle( qobj, GLU_FILL);
        gluQuadricNormals( qobj, GLU_SMOOTH);
        gluQuadricTexture(qobj,GL_TRUE);
        gluQuadricOrientation( qobj,GLU_OUTSIDE);
        glEnable(GL_TEXTURE_2D);
        if(!finish){
            glBindTexture(GL_TEXTURE_2D, holeTexture);
        }else{
            glBindTexture(GL_TEXTURE_2D, finishTexture);
        }
        glTranslated(x,0.1 ,z);
        glRotated(90, 1, 0, 0);
        glRotated(180, 0, 0, 1);
        gluDisk(qobj, 0, 1.5, 20,20);
        gluDeleteQuadric(qobj);
        glDisable(GL_TEXTURE_2D);
        glPopMatrix();
        
    }
};
Hole holes[18]= {
    {-9.5,-14,false},
    {-11,-8,false},
    {-14,7,false},
    {-14,13,false},
    {-7,13,false},
    {-4,-21,false},
    {-4,-13.5,false},
    {4,-11,false},
    {9,-17,false},
    {6.5,0,false},
    {6.5,9,false},
    {13.5,9,false},
    {10,13,false},
    {0,15.5,false},
    {0,22,false},
    {-3.5,2,false},
    {0,-4,false},
    {14,21,true}
};
struct Block{
    float x;
    float z;
    
    float width;
    float depth;
    
    bool horizontal;
    
    
    void draw(){
        glColor3f(1,1,1);
        glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, blockTexture);
        glTranslated(x,1.5,z);
        glScaled(width, 4,depth);
        if(horizontal){
            glBegin(GL_QUADS);
            glTexCoord2f(0,0);
            glVertex3f(0.5,0.5,0.5);
            glTexCoord2f(1,0);
            glVertex3f(0.5,0.5,-0.5);
            glTexCoord2f(0,1);
            glVertex3f(-0.5,0.5,-0.5);
            glTexCoord2f(1,1);
            glVertex3f(-0.5,0.5,0.5);
            glEnd();
            glBegin(GL_QUADS);
            glVertex3f(0.5,0.5,0.5);
            glVertex3f(0.5,0.5,-0.5);
            glVertex3f(0.5,-0.5,-0.5);
            glVertex3f(0.5,-0.5,0.5);
            glEnd();
            glBegin(GL_QUADS);
            glVertex3f(-0.5,-0.5,-0.5);
            glVertex3f(-0.5,-0.5,0.5);
            glVertex3f(-0.5,0.5,0.5);
            glVertex3f(-0.5,0.5,-0.5);
            glEnd();
            glBegin(GL_QUADS);
            glTexCoord2f(1,0);
            glVertex3f(0.5,-0.5,0.5);
            glTexCoord2f(0,1);
            glVertex3f(-0.5,-0.5,0.5);
            glTexCoord2f(1,1);
            glVertex3f(-0.5,0.5,0.5);
            glTexCoord2f(0,0);
            glVertex3f(0.5,0.5,0.5);
            glEnd();
            glBegin(GL_QUADS);
            glTexCoord2f(1,1);
            glVertex3f(-0.5,-0.5,-0.5);
            glTexCoord2f(0,0);
            glVertex3f(0.5,-0.5,-0.5);
            glTexCoord2f(1,0);
            glVertex3f(0.5,0.5,-0.5);
            glTexCoord2f(0,1);
            glVertex3f(-0.5,0.5,-0.5);
            glEnd();
            glBegin(GL_QUADS);
            glVertex3f(0.5,-0.5,0.5);
            glVertex3f(0.5,-0.5,-0.5);
            glVertex3f(-0.5,-0.5,-0.5);
            glVertex3f(-0.5,-0.5,0.5);
            glEnd();
            glDisable(GL_TEXTURE_2D);
        }else{
            glBegin(GL_QUADS);
            glTexCoord2f(1,1);
            glVertex3f(0.5,0.5,0.5);
            glTexCoord2f(0,0);
            glVertex3f(0.5,0.5,-0.5);
            glTexCoord2f(1,0);
            glVertex3f(-0.5,0.5,-0.5);
            glTexCoord2f(0,1);
            glVertex3f(-0.5,0.5,0.5);
            glEnd();
            glBegin(GL_QUADS);
            glTexCoord2f(1,1);
            glVertex3f(0.5,0.5,0.5);
            glTexCoord2f(0,0);
            glVertex3f(0.5,0.5,-0.5);
            glTexCoord2f(1,0);
            glVertex3f(0.5,-0.5,-0.5);
            glTexCoord2f(0,1);
            glVertex3f(0.5,-0.5,0.5);
            glEnd();
            glBegin(GL_QUADS);
            glTexCoord2f(1,0);
            glVertex3f(-0.5,-0.5,-0.5);
            glTexCoord2f(0,1);
            glVertex3f(-0.5,-0.5,0.5);
            glTexCoord2f(1,1);
            glVertex3f(-0.5,0.5,0.5);
            glTexCoord2f(0,0);
            glVertex3f(-0.5,0.5,-0.5);
            glEnd();
            glBegin(GL_QUADS);
            glVertex3f(0.5,-0.5,0.5);
            glVertex3f(-0.5,-0.5,0.5);
            glVertex3f(-0.5,0.5,0.5);
            glVertex3f(0.5,0.5,0.5);
            glEnd();
            glBegin(GL_QUADS);
            glVertex3f(-0.5,-0.5,-0.5);
            glVertex3f(0.5,-0.5,-0.5);
            glVertex3f(0.5,0.5,-0.5);
            glVertex3f(-0.5,0.5,-0.5);
            glEnd();
            glBegin(GL_QUADS);
            glVertex3f(0.5,-0.5,0.5);
            glVertex3f(0.5,-0.5,-0.5);
            glVertex3f(-0.5,-0.5,-0.5);
            glVertex3f(-0.5,-0.5,0.5);
            glEnd();
            glDisable(GL_TEXTURE_2D);
        }
        glPopMatrix();
    }
};
Block blocks[26] ={
    {17.5,0,3,48},
    {-17.5,0,3,48},
    {0,-25.5,38,3,true},
    {0,25.5,38,3,true},
    {-14,-19,4,2,true},
    {-7,-18,2,12,false},
    {-4,-17,6,2,true},
    {-9,-11,6,2,true},
    {-11,-4,2,4,false},
    {-10,-1,4,2,true},
    {-11,4,12,2,true},
    {-3,10,13,2,true},
    {-10,18,4,2,true},
    {-7,17,2,4,false},
    {2.5,6,2,6,false},
    {3,19.5,2,9,false},
    {-0.5,-1,8,2,true},
    {2.5,-5,2,6,false},
    {6.5,-18,2,6,false},
    {9.5,-20,4,2,true},
    {9,-14,7,2,true},
    {13,-8.5,6,2,true},
    {10,0,2,6,false},
    {10,9,2,4,false},
    {10,17,2,4,false},
    {13.5,17,5,2,true}
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
        //Maze
        
        for(int i = 0; i<26;i++){
            Block b = blocks[i];
            b.draw();
        }
        
        for(int i = 0; i<18;i++){
            Hole h = holes[i];
            h.draw();
        }
        //end MazeSides
        
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
    int lastblockcollision;
    void collide(Block *b,int index){
        
        float bx = b->x;
        float bz = b->z;
        float bw = b->width;
        float bd = b->depth;
        
        bool hitx=false, hitz= false;
        bool bhitx=false, bhitz=false;
        if (x  -rad + delta_x< bx + bw/2 && x + rad + delta_x> bx - bw/2){
            hitx = true;
        }
        
        if (z - rad + delta_z< bz +  bd/2 && z + rad + delta_z> bz - bd/2){
            hitz = true;
        }
        
        
        if (x - rad< bx + bw/2 && x + rad> bx - bw/2){
            bhitx = true;
        }
        
        if (z - rad< bz +  bd/2 && z + rad> bz - bd/2){
            bhitz = true;
        }
        
        if (hitx && hitz){
            if (lastblockcollision != index ||lastblockcollision ==-2){
                if(pow(delta_x,2)>pow(0.01,2) ||pow(delta_z,2)>pow(0.01,2)){
                    engine->setSoundVolume(sqrt(pow(delta_x,2)+pow(delta_z,2))*3);
                    engine->play2D("/sounds/hit.wav");
                    
                }
            }
            if (bhitz)
                delta_x = -1 * ELASTICITY * delta_x;
            if (bhitx)
                delta_z = -1 * ELASTICITY * delta_z;
            
            lastblockcollision = index;
            return;
        }
        if(pow(delta_x,2)>pow(0.01,2) && pow(delta_z,2)>pow(0.01,2)){
            lastblockcollision =-1;
        }
        
    }
    void update(Platform p){
        
        if (!gameover) {
            
            
            double acc_x = (GRAV-FRICTION*GRAV) * sin(p.roll * PI/180)/(weight);
            double acc_y;
            
            double acc_y_1 = -1 * GRAV * cos(p.roll * PI/180)/weight;
            double acc_y_2 = -1 * GRAV * cos(p.pitch * PI/180)/weight;
            acc_y = acc_y_1+acc_y_2;
            
            double acc_z = (GRAV-FRICTION*GRAV) * sin(p.pitch * PI/180)/(weight);
            
            
            
            delta_x += acc_x;
            //        delta_y = acc_y;
            delta_z += acc_z;
            
            if (fabs(delta_x) < 1e-4)
                delta_x = 0;
            if (fabs(delta_z) < 1e-4)
                delta_z = 0;
            
            
            
            // collision detection 
            for (int i=0;i  < /*blocks*/ 26; i++){
                collide(&blocks[i],i);
            }
            
            //            printf("%f, %f, %f, %f\n", delta_x,delta_z, acc_x, acc_z);
            x += delta_x;
            z += delta_z;
            
            y = 0.0;
            // end platform
            
            rot_x += delta_x*360/(2*PI*rad);
            rot_z += delta_z*360/(2*PI*rad);
            
            for(int i=0;i<18;i++){
                
                if((( x<holes[i].x+0.9)&&( x>holes[i].x-0.9))&&(( z<holes[i].z+0.9)&&(z>holes[i].z-0.9))){
                    
                    
                    if(holes[i].finish){
                        x=holes[i].x;
                        z=holes[i].z;
                        win = true;
                        engine->setSoundVolume(1);
                        printf(">>>>>>>> Win\n");
                        engine->play2D("/sounds/gameover.wav");
                        gameover=true;
                        end = time(NULL);
                    }
                    if(!KR_mode){
                        x=holes[i].x;
                        z=holes[i].z;
                        engine->setSoundVolume(1);
                        printf(">>>>>>>> Lost\n");
                        engine->play2D("/sounds/lost.wav");
                        gameover=true;
                        end = time(NULL);
                    }
                    
                }
            }
        }else {
            rad-=0.01;
        }
    }
    
    void draw(){
        float specReflection[] = { 0.8f, 0.8f, 0.8f, 1.0f };
        glMaterialfv(GL_FRONT, GL_SPECULAR, specReflection);
        glMateriali(GL_FRONT, GL_SHININESS, 1);
        
        glPushMatrix(); // start ball
        glColor3f(0.6, 0.6, 0.6);
        glTranslated(x,rad, z);
        
        
        
        // FIXME put both on only one rotatation
        //        double ang = atan(rot_z/rot_x);
        //        double rot = rot_x / cos(ang);
        
        //        printf("%.3f, %.3f\n", rot*cos(ang), rot*sin(ang) );
        //        glRotatef(rot, sin(ang), 0.0, -1 * cos(ang));
        glRotatef(rot_z,1,0,0);
        glRotatef(rot_x,0,0,-1);
        
        
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
Ball ball = {/*weight*/2.0, -14, 0, -22, /*rad*/1.0, 0.0, 0.0, 0.0, 0, 0,-2};


string convertInt(int number)
{
    stringstream ss; 
    ss<< number;//add number to the stream
    return ss.str();//return a string with the contents of the stream
}

void render(string s,int w_l_n) {
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 1204, 768, 0, -1000, 1000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    switch (w_l_n) {
        case 0:glColor3f(0.0f, 0.0f, 0.0f);
            break;
            
        case 1:glColor3f(0.0f, 1.0f, 0.0f);
            break;
            
        case 2:glColor3f(1.0f, 0.0f, 0.0f);
            break;
            
    }
    glRasterPos2f(25,25);
    for(int i =0;i<s.size();i++){
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, s[i]);
    }
    
    
}

void display(void)
{
    glutSetCursor(GLUT_CURSOR_NONE);
    glMatrixMode(GL_PROJECTION); // set the view volume shape
    glLoadIdentity();
    gluPerspective(50, //Field of view
                   (WINDOW_W)*1.0/(WINDOW_H)*1.0, //Aspect ratio
                   0.1, // Z near
                   1000.0);// Z far
    
    double factor = 1.0;
    glOrtho(10/factor, 10/factor, -10/factor, -10/factor, 0.1, 500);
    
    glMatrixMode(GL_MODELVIEW); // position and aim the camera
    glEnable( GL_LINE_SMOOTH );
    glEnable( GL_POLYGON_SMOOTH );
    glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
    glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
    glEnable(GL_DEPTH_TEST);
    glLoadIdentity();
    gluLookAt(0.0,85.0,0.1, // eye
              0.0,0.0,0.0,
              0.0, 1.0, 0.0); // normal
    
    //    gluLookAt(ball.x - ball.delta_x ,15,ball.z - ball.delta_z, // eye
    //              ball.x,ball.y,ball.z,
    //              0.0, 1.0, 0.0); // normal
    
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
    
    
    
    platform.draw();
    
    glRotatef(platform.roll, 0.0, 0.0, -1.0);
    glRotatef(platform.pitch, 1.0, 0.0, 0.0);
    ball.draw();
    
    glPopMatrix(); // end everything
    glDisable(GL_LIGHTING);
    if(!gameover){
        render("Time Elapsed :" +convertInt( time(NULL)-start),0);
    }
    else{
        if(win)
            render("Game Over @ :" +convertInt(end-start),1);
        else {
            render("Game Over @ :" +convertInt(end-start),2);
        }
    }
    glEnable(GL_LIGHTING);
    glFlush();
}
//<<<<<<<<<<<<<<<<<<<<<< main >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
int main(int argc, char **argv)
{
    //Sound Engine
    engine = irrklang::createIrrKlangDevice();
    
    if (!engine)
        return 0;
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize(WINDOW_W, WINDOW_H);
    
    glutInitWindowPosition(0, 0);
    glutCreateWindow("Transformation testbed - wireframes");
    glutDisplayFunc(display);
    platformTexture = loadTexture("/pngs/wood_texture.png",256,192);
    ballTexture = loadTexture("/pngs/chess_texture.png",1200,1200);
    holeTexture = loadTexture("/pngs/holes_texture.png",200,200);
    blockTexture= loadTexture("/pngs/block_texture.png",200,200);
    finishTexture= loadTexture("/pngs/finish_texture.png",200,200);
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
    start = time(NULL);
    glutMainLoop();
    engine->drop(); // delete engine
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
        case 'k':
            KR_mode = !KR_mode;
            
            break;
        case 'r':
            ball.x = -14;
            ball.z = -22;
            ball.delta_x = 0;
            ball.delta_z = 0;
            ball.rot_x = 0;
            ball.rot_z = 0;
            ball.rad = 1;
            gameover=false;
            start = time(NULL);
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
}

void anim(){
    glutPostRedisplay();
    ball.update(platform);
}

float sign(float x){
    return x >= 0? 1:-1;
}