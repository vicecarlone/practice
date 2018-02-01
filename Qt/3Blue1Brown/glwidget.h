#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QObject>
#include <QGLWidget>
#include <QTimer>

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = 0);

    void initializeGL();
    void paintGL();
    void resizeGL(int h, int w);

private:
    QTimer timer;

    void draw(int n);
    void draw2(int n);
    int numlight;
public slots:
    void setnumlight(int n);
};

#endif // GLWIDGET_H
