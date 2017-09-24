#include <QApplication>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>
#include <QCamera>
#include <qtDebug>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QVBoxLayout>
#include <QTimer>
#include <string.h>
#include "cameraframegrabber.h"


QSerialPort *serial;
QString str;
QCamera *mCamera;
QCameraViewfinder *mCameraViewfinder;
QVBoxLayout *mLayout;
int camera_angle=0;


QCamera *_camera;
//MyVideoSurface *mVideo;
CameraFrameGrabber *_cameraFrameGrabber;

int X,Y,Z;
float W=0;//positions of the grip

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


  /*
    mCamera=new QCamera(this);
    mCameraViewfinder = new QCameraViewfinder(this);
    mCamera->setViewfinder(mCameraViewfinder);
    mLayout = new QVBoxLayout;

     mCameraViewfinder->show();

    mCamera->start(); // to start the viewfinder

    mLayout->addWidget(mCameraViewfinder);
    mLayout->setMargin(0);
    ui->scroll->setLayout(mLayout);
*/

    /*_camera = new QCamera(this);
    _cameraFrameGrabber = new CameraFrameGrabber();
    _camera->setViewfinder(_cameraFrameGrabber);
    connect(_cameraFrameGrabber, SIGNAL(frameAvailable(QImage)), this, SLOT(handleFrame(QImage)));
    _camera->start();*/


    serial= new QSerialPort(this);
    str=ui->port->text();
    serial->setPortName(str);

    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    serial->open(QIODevice::ReadWrite);


    connect(ui->slider, SIGNAL(valueChanged(int)), this, SLOT(Slider(int)));

    connect(ui->Zup, SIGNAL(pressed()),this, SLOT(Zup( )));
    connect(ui->Zdown, SIGNAL(pressed()),this, SLOT(Zdown( )));
    connect(ui->Yplus, SIGNAL(pressed()),this, SLOT(Yplus( )));
    connect(ui->Yminus, SIGNAL(pressed()),this, SLOT(Yminus( )));
    connect(ui->Xright, SIGNAL(pressed()),this, SLOT(Xright( )));
    connect(ui->Xleft, SIGNAL(pressed()),this, SLOT(Xleft( )));
    connect(ui->Get, SIGNAL(pressed()),this, SLOT(Get( )));
    connect(ui->Release, SIGNAL(pressed()),this, SLOT(Release( )));
    connect(ui->rotorright, SIGNAL(pressed()),this, SLOT(rotorright( )));
    connect(ui->rotorleft, SIGNAL(pressed()),this, SLOT(rotorleft( )));
    connect(serial,SIGNAL(readyRead()),this,SLOT(serialreceived()));
    connect(ui->zoom,SIGNAL(pressed()),this,SLOT(zoom()));
     connect(ui->sendButton, SIGNAL(pressed()),this, SLOT(command( )));

    _camera = new QCamera();
    _cameraFrameGrabber = new CameraFrameGrabber();
    _camera->setViewfinder(_cameraFrameGrabber);
    connect(_cameraFrameGrabber, SIGNAL(frameAvailable(QImage)), this, SLOT(handleFrame(QImage)));
    _camera->start();


    serial->write("G92 xo y0 z0\r");


}

MainWindow::~MainWindow()
{
    delete ui;
       serial->close();
}

void MainWindow::Zup()
{
    QString s= QString::number((float)++Z);
    s="G0 Z"+s+"\r";
    serial->write(s.toStdString().c_str());
    qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;
}

void MainWindow::Zdown()
{
    QString s= QString::number((float)--Z);
    s="G0 Z"+s+"\r";
     serial->write(s.toStdString().c_str());
     qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;
}

void MainWindow::Yminus()
{
    QString s= QString::number((float)--Y);
    s="G0 Y"+s+"\r";
     serial->write(s.toStdString().c_str());
     qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;
}

void MainWindow::Yplus()
{
    QString s= QString::number((float)++Y);
    s="G0 Y"+s+"\r";
     serial->write(s.toStdString().c_str());
     qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;
}

void MainWindow::Xleft()
{
    QString s= QString::number((float)++X);
    s="G0 X"+s+"\r";
     serial->write(s.toStdString().c_str());
     qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;
}

void MainWindow::Xright()
{
    QString s= QString::number((float)--X);
    s="G0 X"+s+"\r";
     serial->write(s.toStdString().c_str());
    qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;
}

void MainWindow::Get()
{

     serial->write("M106\r");
}
void MainWindow::Release()
{
     serial->write("M107\r");
}

void MainWindow::rotorright()
{
     serial->write("M302 S0\r");
     serial->write("M83 S0\r");
     serial->write("G1 E-0.1\r");
     W=W-0.1;
     qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;
}
void MainWindow::rotorleft()
{
     serial->write("M302 S0\r");
     serial->write("M83\r");
     serial->write("G1 E0.1\r");
     W=W+0.1;
     qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;
}

void MainWindow::portconnect()
{
    str=ui->port->text();
    serial->setPortName(str);
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    serial->open(QIODevice::ReadWrite);


}


int MainWindow::serialreceived()
{

    QByteArray data = serial->readAll();

   // parse((char *)data.constData());

// printf("command %s %s %s %s %s %s\n",argv[0],argv[1],argv[2],argv[3],argv[4],argv[5]);
    printf("%s",(char *)data.constData());

 }

void MainWindow::zoom(){
    if(mCameraViewfinder->width() != 1600)
    {
        mCameraViewfinder->setGeometry(-590,-490,1600,1440);  // zoom in
    }
    else
    {
        mCameraViewfinder->setGeometry(0,0,375,380);		// zoom out
    }
}

void MainWindow::handleFrame(QImage imageObject)
{
   // QImage imageObject(256,256,QImage::Format_ARGB32);
   // imageObject.load("C:/audio.png");
   // imageObject.setColorCount(22);
    //QGraphicsScene *scene = new QGraphicsScene ();
    int width=60;
    int height=60;
    QRgb value;
    QMatrix matrix;
    matrix.rotate(camera_angle);
    value = qRgb(0, 0, 0); // 0xffbd9527
    imageObject=imageObject.transformed(matrix);
    imageObject.setPixel(1, 1, value);
    int y,x;
       for ( y=0;y<height;++y) {
                 imageObject.setPixel(640/2, 480/2-height/2+y, value);

           }
   for ( x=0;x<width;++x) {
                 imageObject.setPixel(640/2-width/2+x, 480/2, value);

           }
   QPoint p;
   int r,g,b,a;
   QColor colors;
/*
   for (y=0;y<480;++y) // setting to gray
       for(x=0;x<640;++x)
       {
           p.setX(x);
           p.setY(y);
           colors=imageObject.pixelColor(p);
           colors.getRgb(&r,&g,&b,&a);
           value = qRgb((r+g+b)/3, (r+g+b)/3,(r+g+b)/3);
           imageObject.setPixel(x,y, value);

       }
*/


    QPixmap pixmap = QPixmap::fromImage(imageObject);
    scene->addPixmap(pixmap);

    ui->graphicsView->setScene(scene);

    //delete scene;
    //ui->mylabel->setPixmap(QPixmap::fromImage(imageObject));
    /*OR use the other way by setting the Pixmap directly
    QPixmap pixmapObject(imagePath");
    ui->myLabel2->setPixmap(pixmapObject);*/

}


void MainWindow::command()
{
    QString s ;
    s=ui->command->text();
    s=s+"\r";
    serial->write(s.toStdString().c_str());
}

void MainWindow::Slider(int a)
{
camera_angle=a;
    }
