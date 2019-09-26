#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QOpenGLWindow>
#include <QSurfaceFormat>
#include <QOpenGLFunctions>
#include <QtOpenGL>
#include <GL/glu.h>
#include <QMouseEvent>


#include "point.h"
#include "octree.h"

using namespace std;
class MainWindow : public QOpenGLWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();


    void drawSun();

    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
    void keyPressEvent(QKeyEvent *);

    bool pointIN(Point a);

    vector<Point> pointsW;


    vector<Point> linesA;
    vector<Point> linesB;
    vector<Point*> pSelec;

    virtual void paintGL();
    OctTree *qTree ;

    void printQT(OctTree **quad);

    float zoomScale;

protected:
    virtual void initializeGL();
    virtual void resizeGL(int w, int h);

    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);

    void findC(OctTree **p);
    bool circleInt(Point tl, Point br);
    bool circleInt2(Point tl, Point br);


    bool pointC(Point a,Point max, Point min);
    void pushPoint(OctTree **p);

    Point cCircle;
    float angle;
    bool pressLB;
    Point cSphere;
    float radio;
private:
    QOpenGLContext *context;
    QOpenGLFunctions *openGLFunctions;
};

#endif // MAINWINDOW_H
