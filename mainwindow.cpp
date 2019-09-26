#include "mainwindow.h"
#include "point.h"
#include <QtMath>
#include <cmath>

void MainWindow::drawSun()
{
    glPushMatrix();
    glTranslatef(cSphere.x,cSphere.y,cSphere.z);
    glScalef(1,1,1);

    GLUquadricObj *quadric=gluNewQuadric();
    gluQuadricNormals(quadric, GLU_SMOOTH);
    glColor3f(1,1,0);

    gluQuadricDrawStyle(quadric,GLU_SILHOUETTE);

    gluSphere(quadric, 0.4, 60,60);

    gluDeleteQuadric(quadric);
    glPopMatrix();
}


bool MainWindow::pointIN(Point a){
    if(sqrt(pow((cSphere.x-a.x),2)+pow((cSphere.y-a.y),2)+pow((cSphere.z-a.z),2)) <= pow(radio,2)){

        // qDebug()<< "DENTRO "<< endl;
         return true;
    }
     //qDebug()<< sqrt(pow((cSphere.x-a.x),2)+pow((cSphere.y-a.y),2)+pow((cSphere.z-a.z),2)) << "AFUERA "<< endl;
    return false;
}

bool MainWindow::pointC(Point a,Point max, Point min){
    if(a.x >= min.x and a.y >= min.y and a.z >= min.z and
       a.x <= max.x and a.y <= max.y and a.z <= max.z){


         return true;
    }
    return false;
}


bool MainWindow::circleInt(Point max, Point min){
    if(pointIN(max) and pointIN(min)
      and pointIN(Point(max.x,max.y,min.z)) and pointIN(Point(min.x,max.y,max.z))
      and pointIN(Point(max.x,min.y,max.z)) and pointIN(Point(max.x,min.y,min.z))
      and pointIN(Point(min.x,max.y,min.z)) and pointIN(Point(min.x,min.y,max.z)) )
    {
        qDebug()<< "todos "<< endl;
      return true;
    }
    qDebug()<< "ninguno "<< endl;
    return false;

}

bool MainWindow::circleInt2(Point max, Point min){
    if(pointIN(max) or pointIN(min)
      or pointIN(Point(max.x,max.y,min.z)) or pointIN(Point(min.x,max.y,max.z))
      or pointIN(Point(max.x,min.y,max.z)) or pointIN(Point(max.x,min.y,min.z))
      or pointIN(Point(min.x,max.y,min.z)) or pointIN(Point(min.x,min.y,max.z)) )
    {
         qDebug()<< "alguno "<< endl;
      return true;
    }
    return false;

}



MainWindow::MainWindow(QWidget *parent)
{

    setSurfaceType(QWindow::OpenGLSurface);


    QSurfaceFormat format;
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    format.setVersion(2,1);

    setFormat(format);

    context = new QOpenGLContext;
    context->setFormat(format);
    context->create();

    context->makeCurrent(this);
    openGLFunctions = context->functions();

    //qDebug() << "lo creare" ;
    Point min(-1.0,-1.0,-1.0);
    Point max(1.0,1.0,1.0);

    qTree = nullptr;
    qTree = new OctTree(min,max);

    cCircle.x = 0.0;
    cCircle.y = 0.0;
    cCircle.z = 0.0;

    cSphere.x = 0.0;
    cSphere.y = 0.0;
    cSphere.z = 0.0;

    angle= 0.2;

    zoomScale = 1.0;

    pressLB = false;
    radio = 0.5;


}

MainWindow::~MainWindow()
{
    delete qTree;

}

void MainWindow::mouseMoveEvent(QMouseEvent *ev)
{
    //qDebug() << ev->x() << " - " << ev-> y();

    //if(ev->button() == Qt::RightButton){
        cCircle.x = float(ev->x())/float(this->width())-0.5;
        cCircle.y = float(ev->y())/float(this->height())-0.5;
         cCircle.x *= 2;
       cCircle.y *= -2;

       cSphere.x = cCircle.x ;
       cSphere.y = cCircle.y ;

    //}
    update();
}

void MainWindow::wheelEvent(QWheelEvent *ev){


    //angle += (ev->delta()/3000.0);
    //if(angle < 0) angle = 0.0001;
    //update();

    QPoint numDegrees = ev->angleDelta();
    if (numDegrees.y() < 0)  zoomScale = zoomScale/1.1;
    if (numDegrees.y() > 0)  zoomScale = zoomScale*1.1;
    update(); // call paintGL()



}

void MainWindow::pushPoint(OctTree **p)
{
    if((*p)->leaf){
        for(int i=0; i<(*p)->data.size();i++){
            pSelec.push_back((*p)->data[i]);
        }
    }else{
        pushPoint(&(*p)->northEastB);
        pushPoint(&(*p)->southEastB);
        pushPoint(&(*p)->northWestB);
        pushPoint(&(*p)->southWestB);
        pushPoint(&(*p)->northEastF);
        pushPoint(&(*p)->southEastF);
        pushPoint(&(*p)->northWestF);
        pushPoint(&(*p)->southWestF);
    }
}

void MainWindow::findC(OctTree **p){

    //qDebug()<< "comenzOOOOOOOp" <<endl;

    if(circleInt((*p)->dimMax,(*p)->dimMin)){
        pushPoint(p);

        return;
    }


    if(circleInt2((*p)->dimMax,(*p)->dimMin) or
            pointC(Point(cSphere.x,cSphere.y,cSphere.z),(*p)->dimMax,(*p)->dimMin) ){

        if((*p)->leaf){
            for(int i=0;i<(*p)->data.size();i++){
                if(pointIN(*(*p)->data[i]))
                    pSelec.push_back((*p)->data[i]);
            }
            return;
        }else{
            findC(&(*p)->northEastB);
            findC(&(*p)->southEastB);
            findC(&(*p)->northWestB);
            findC(&(*p)->southWestB);
            findC(&(*p)->northEastF);
            findC(&(*p)->southEastF);
            findC(&(*p)->northWestF);
            findC(&(*p)->southWestF);

        }
    }





}

void MainWindow::keyPressEvent(QKeyEvent *ev){
    //qDebug() << "key: " << ev->key();
    if(ev->key() == 70){

        pSelec.clear();

        //qDebug() << "buscar";
        findC(&qTree);

        /*
        for(int i=0;i<pSelec.size();i++){
            qDebug() << pSelec[i]->x << " - " << pSelec[i]->y ;
        }
        */
        //qDebug() << "----" <<pSelec.size()<< " puntos." ;
        update();
    }
}

void MainWindow::mousePressEvent(QMouseEvent *ev)
{

    float x_p = float(ev->x())/float(this->width())-0.5;
    float y_p = float(ev->y())/float(this->height())-0.5;




    pointsW.push_back(Point(x_p*2,y_p*-2));

    qTree->insert(new Point(x_p*2,y_p*-2));


    linesA.clear();
    linesB.clear();



    printQT(&qTree);
    update();

}

void MainWindow::initializeGL()
{
    resizeGL(this->width(),this->height());

}


void MainWindow::resizeGL(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glViewport(0,0,w,h);

   qreal aspectratio = qreal(w)/qreal(h);
   glOrtho(-1* aspectratio,1*aspectratio,-1,1,1,-1);


}



void MainWindow::paintGL()
{

    glClearColor(255, 255, 255, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();

    glScaled(zoomScale, zoomScale, zoomScale);
    //qDebug() << zoomScale ;
    glPointSize(3);


    gluPerspective(45.0,1.0,1.0,100.0);
    glTranslatef(0,0,-6);


    glRotatef(cCircle.x*100,0,1,0);
    glRotatef(cCircle.y*100,1,0,0);

    pressLB = false;

    drawSun();


    glBegin(GL_POINTS);
    glColor3d(0,0,255);
    for(int i=0;i<int(pointsW.size());i++){
        //qDebug() <<pointsW[i].first<< "paint " << pointsW[i].second<< "size" <<pointsW.size() ;
        glVertex3d(pointsW[i].x,pointsW[i].y,pointsW[i].z);
    }
    glEnd();

    qDebug() <<pSelec.size() << "puntos selec " << endl;

    glBegin(GL_POINTS);
    glColor3d(0,255,0);
    for(int i=0;i<int(pSelec.size());i++){

        glVertex3d(pSelec[i]->x,pSelec[i]->y,pSelec[i]->z);
    }
    glEnd();




    glBegin(GL_LINES);
    glColor3d(255,0,0);
    //qDebug() <<" gh "<<linesA.size() ;
    for(int i=0;i<int(linesA.size());i++){

        glVertex3d(linesA[i].x,linesA[i].y,linesA[i].z);
        glVertex3d(linesB[i].x,linesB[i].y,linesB[i].z);
    }

    glEnd();



    //qDebug() << x_m << " pa " << y_m ;line
    //drawHollowCircle(cCircle.x,cCircle.y,angle);

    glFlush();


}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    resizeGL(this->width(),this->height());
    this->update();
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    paintGL();

}

void MainWindow::printQT(OctTree **quad)
{


    if(!((*quad)->leaf)){

        float midX = ((*quad)->dimMax.x + (*quad)->dimMin.x)/2;
        float midY = ((*quad)->dimMax.y + (*quad)->dimMin.y)/2;
        float midZ = ((*quad)->dimMax.z + (*quad)->dimMin.z)/2;

        linesA.push_back(Point((*quad)->dimMin.x,midY,midZ));
        linesB.push_back(Point((*quad)->dimMax.x,midY,midZ));

        linesA.push_back(Point(midX,midY,(*quad)->dimMin.z));
        linesB.push_back(Point(midX,midY,(*quad)->dimMax.z));

        linesA.push_back(Point(midX,(*quad)->dimMin.y,midZ));
        linesB.push_back(Point(midX,(*quad)->dimMax.y,midZ));

        linesA.push_back(Point((*quad)->dimMin.x,midY,(*quad)->dimMax.z));
        linesB.push_back(Point((*quad)->dimMax.x,midY,(*quad)->dimMax.z));

        linesA.push_back(Point(midX,(*quad)->dimMin.y,(*quad)->dimMax.z));
        linesB.push_back(Point(midX,(*quad)->dimMax.y,(*quad)->dimMax.z));


        linesA.push_back(Point((*quad)->dimMin.x,midY,(*quad)->dimMin.z));
        linesB.push_back(Point((*quad)->dimMax.x,midY,(*quad)->dimMin.z));

        linesA.push_back(Point(midX,(*quad)->dimMin.y,(*quad)->dimMin.z));
        linesB.push_back(Point(midX,(*quad)->dimMax.y,(*quad)->dimMin.z));

        linesA.push_back(Point((*quad)->dimMin.x,midY,(*quad)->dimMin.z));
        linesB.push_back(Point((*quad)->dimMin.x,midY,(*quad)->dimMax.z));

        linesA.push_back(Point((*quad)->dimMax.x,midY,(*quad)->dimMin.z));
        linesB.push_back(Point((*quad)->dimMax.x,midY,(*quad)->dimMax.z));


        linesA.push_back(Point(midX,(*quad)->dimMax.y,(*quad)->dimMin.z));
        linesB.push_back(Point(midX,(*quad)->dimMax.y,(*quad)->dimMax.z));
        linesA.push_back(Point((*quad)->dimMin.x,(*quad)->dimMax.y,(*quad)->dimMin.z));
        linesB.push_back(Point((*quad)->dimMin.x,(*quad)->dimMax.y,(*quad)->dimMax.z));
        linesA.push_back(Point((*quad)->dimMax.x,(*quad)->dimMax.y,(*quad)->dimMin.z));
        linesB.push_back(Point((*quad)->dimMax.x,(*quad)->dimMax.y,(*quad)->dimMax.z));

        linesA.push_back(Point(midX,(*quad)->dimMin.y,(*quad)->dimMin.z));
        linesB.push_back(Point(midX,(*quad)->dimMin.y,(*quad)->dimMax.z));
        linesA.push_back(Point((*quad)->dimMin.x,(*quad)->dimMin.y,(*quad)->dimMin.z));
        linesB.push_back(Point((*quad)->dimMin.x,(*quad)->dimMin.y,(*quad)->dimMax.z));
        linesA.push_back(Point((*quad)->dimMax.x,(*quad)->dimMin.y,(*quad)->dimMin.z));
        linesB.push_back(Point((*quad)->dimMax.x,(*quad)->dimMin.y,(*quad)->dimMax.z));


        linesA.push_back(Point(midX,(*quad)->dimMax.y,(*quad)->dimMin.z));
        linesB.push_back(Point(midX,(*quad)->dimMax.y,(*quad)->dimMax.z));

        linesA.push_back((*quad)->dimMin);
        linesB.push_back(Point((*quad)->dimMax.x,(*quad)->dimMin.y,(*quad)->dimMin.z));

        linesA.push_back(Point((*quad)->dimMin.x,(*quad)->dimMax.y,(*quad)->dimMin.z));
        linesB.push_back(Point((*quad)->dimMax.x,(*quad)->dimMax.y,(*quad)->dimMin.z));

        linesA.push_back(Point((*quad)->dimMin.x,(*quad)->dimMax.y,midZ));
        linesB.push_back(Point((*quad)->dimMax.x,(*quad)->dimMax.y,midZ));

        linesA.push_back(Point((*quad)->dimMin.x,(*quad)->dimMax.y,(*quad)->dimMax.z));
        linesB.push_back(Point((*quad)->dimMax.x,(*quad)->dimMax.y,(*quad)->dimMax.z));

        linesA.push_back(Point((*quad)->dimMin.x,(*quad)->dimMin.y,midZ));
        linesB.push_back(Point((*quad)->dimMax.x,(*quad)->dimMin.y,midZ));

        linesA.push_back(Point((*quad)->dimMin.x,(*quad)->dimMin.y,(*quad)->dimMax.z));
        linesB.push_back(Point((*quad)->dimMax.x,(*quad)->dimMin.y,(*quad)->dimMax.z));

        linesA.push_back((*quad)->dimMin);
        linesB.push_back(Point((*quad)->dimMin.x,(*quad)->dimMax.y,(*quad)->dimMin.z));

        linesA.push_back(Point((*quad)->dimMin.x,(*quad)->dimMin.y,midZ));
        linesB.push_back(Point((*quad)->dimMin.x,(*quad)->dimMax.y,midZ));

        linesA.push_back(Point((*quad)->dimMin.x,(*quad)->dimMin.y,(*quad)->dimMax.z));
        linesB.push_back(Point((*quad)->dimMin.x,(*quad)->dimMax.y,(*quad)->dimMax.z));

        linesA.push_back((*quad)->dimMax);
        linesB.push_back(Point((*quad)->dimMax.x,(*quad)->dimMin.y,(*quad)->dimMax.z));


        linesA.push_back(Point((*quad)->dimMax.x,(*quad)->dimMax.y,midZ));
        linesB.push_back(Point((*quad)->dimMax.x,(*quad)->dimMin.y,midZ));
        linesA.push_back(Point((*quad)->dimMax.x,(*quad)->dimMax.y,(*quad)->dimMin.z));
        linesB.push_back(Point((*quad)->dimMax.x,(*quad)->dimMin.y,(*quad)->dimMin.z));


            printQT(&((*quad)->northEastB));
            printQT(&((*quad)->southEastB));
            printQT(&((*quad)->northWestB));
            printQT(&((*quad)->southWestB));
            printQT(&((*quad)->northEastF));
            printQT(&((*quad)->southEastF));
            printQT(&((*quad)->northWestF));
            printQT(&((*quad)->southWestF));

    }
}
