#include "GLUT/glut.h"
#include <iostream>

/* ################################ HEADERS ######################## */
#define PI 3.14159265
void anim(void);
void myKeyboard(unsigned char thekey,int mouseX,int mouseY);
void display();


/* ############################## HEADER END ######################## */


float camera_x = 0.0;
float camera_y = 0.0;
float camera_z = 0.0;

void display(void)
{
    glMatrixMode(GL_PROJECTION); // set the view volume shape
    glLoadIdentity();
    gluPerspective(140, //Field of view
                   800.0/600.0, //Aspect ratio
                   0.9, // Z near
                   100.0);// Z far
    
    double factor = 1;
    glOrtho(-10/factor, 10/factor, -10/factor, 10/factor, 0.1, 500);
    glMatrixMode(GL_MODELVIEW); // position and aim the camera
    
    glEnable(GL_DEPTH_TEST);
    glLoadIdentity();
    gluLookAt(0.0,0.0,5.0, // eye
              0.0,0.0,0.0,
              0.0, 1.0, 0.0); // normal
    
    glRotatef(camera_x, 1.0, 0.0, .0);
    glRotatef(camera_y, 0.0, 1.0, .0);
    glRotatef(camera_z, 0.0, 0.0, 1.0);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix(); // everything
    
    glutWireCube(30);
    glutSolidCube(5);
    
    
    glPopMatrix(); // end everything
    glFlush();
}


//<<<<<<<<<<<<<<<<<<<<<< main >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH );
    
    glutInitWindowSize(800,600);
    
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Transformation testbed - wireframes");
    glutDisplayFunc(display);
    
    GLfloat light_diffuse[] = {1.0, 1.0, 1.0};
    float light_position[] = {10.0, 10.5, 10.5, 0.0};
    
    
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
    
    glEnable(GL_DEPTH_TEST);
    //    glDepthMask(GL_TRUE);
    //    glDepthFunc(GL_LEQUAL);
    //    glDepthRange(0.0f, 1.0f);
    
    glutKeyboardFunc(myKeyboard);
    glClearColor(1.0f, 1.0f, 1.0f,0.0f); // background is white
	glutIdleFunc(anim);
    glViewport(0, 0, 800, 600);
    
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
            
    }
}

void anim(){
    glutPostRedisplay();
}
