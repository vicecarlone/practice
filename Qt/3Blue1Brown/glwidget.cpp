#include "glwidget.h"
#include <cmath>
#include <GL/glut.h>
#include <GL/glu.h>

#define PI 3.1415

GLWidget::GLWidget(QWidget *parent) : QGLWidget(parent)
{
    /*connect(&timer,SIGNAL(timeout()),this,SLOT(updateGL()));
    timer.start(20)*/
    numlight = 50;

}

void GLWidget::initializeGL()
{
    glClearColor(1.0,1.0,1.0,1);
    glEnable(GL_BLEND);
    glEnable(GL_POLYGON_SMOOTH);

}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(0.0,0.0,1.0);

    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 99; i++){
        glVertex3f(cos(i/50.0*PI),sin(i/50.0*PI),0.0);
    }
    glEnd();

    draw(numlight);
}

void GLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.5, +1.5, -1.5, +1.5, -1.0, 15.0);
    glMatrixMode(GL_MODELVIEW);
}

void GLWidget::draw(int n){

    glColor4f(0.9,0.9,0.9,0.2);
    for (int i = 1; i < n+ 1; i++){
        double alpha = acos(2.0*i/(n+1)-1.0);
        glBegin(GL_LINES);
            glVertex3d(2.0,sin(alpha+PI/2.0),0.0);
            glVertex3d(cos(alpha+PI/2.0),sin(alpha+PI/2.0),0.0);

        glEnd();
    }

    glColor3f(0.2,0.2,0.2);
    for (int i = 1; i < n+ 1; i++){
        double alpha = acos(2.0*i/(n+1)-1.0);
        glBegin(GL_LINES);
            glVertex3d(cos(alpha+PI/2.0),sin(alpha+PI/2.0),0.0);
            glVertex3d(cos(alpha * 3.0 +PI/2.0),sin(alpha * 3.0 +PI/2.0),0.0);
        glEnd();
    }


}

void GLWidget::draw2(int n){

    glColor4f(0.9,0.9,0.9,0.2);
    for (int i = 1; i < n+ 1; i++){
        double alpha = acos(2.0*i/(n+1)-1.0);
        glBegin(GL_LINES);
            glVertex3d(1.0,0.0,0.0);
            glVertex3d(cos(alpha+PI/2.0),sin(alpha+PI/2.0),0.0);

        glEnd();
    }

    glColor3f(0.2,0.2,0.2);
    for (int i = 1; i < n+ 1; i++){
        double alpha = acos(2.0*i/(n+1)-1.0);
        glBegin(GL_LINES);
            glVertex3d(cos(alpha+PI/2.0),sin(alpha+PI/2.0),0.0);
            glVertex3d(cos(alpha * 2.0 +PI),sin(alpha * 2.0 +PI),0.0);
        glEnd();
    }

    glColor4f(0.9,0.9,0.9,0.2);
        for (int i = 1; i < n+ 1; i++){
            double alpha = acos(2.0*i/(n+1)-1.0);
            glBegin(GL_LINES);
                glVertex3d(1.0,0.0,0.0);
                glVertex3d(cos(alpha),sin(alpha),0.0);

            glEnd();
        }

        glColor3f(0.2,0.2,0.2);
        for (int i = 1; i < n+ 1; i++){
            double alpha = acos(2.0*i/(n+1)-1.0);
            glBegin(GL_LINES);
                glVertex3d(cos(alpha),sin(alpha),0.0);
                glVertex3d(cos(alpha * 2.0),sin(alpha * 2.0),0.0);
            glEnd();
        }


        glColor4f(0.9,0.9,0.9,0.2);
            for (int i = 1; i < n+ 1; i++){
                double alpha = -acos(2.0*i/(n+1)-1.0);
                glBegin(GL_LINES);
                    glVertex3d(1.0,0.0,0.0);
                    glVertex3d(cos(alpha),sin(alpha),0.0);

                glEnd();
            }

            glColor3f(0.2,0.2,0.2);
            for (int i = 1; i < n+ 1; i++){
                double alpha = -acos(2.0*i/(n+1)-1.0);
                glBegin(GL_LINES);
                    glVertex3d(cos(alpha),sin(alpha),0.0);
                    glVertex3d(cos(alpha * 2.0),sin(alpha * 2.0),0.0);
                glEnd();
            }
}


void GLWidget::setnumlight(int n)
{
    numlight = n;
    updateGL();
}
