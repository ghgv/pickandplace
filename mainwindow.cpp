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
QString str,deltaf;
QCamera *mCamera;
QCameraViewfinder *mCameraViewfinder;
QVBoxLayout *mLayout;
int camera_angle=0;
int offsetX=-42.1;
int offsetY=-6;
int offsetZ=3;
float delta=0.1;


QCamera *_camera;
CameraFrameGrabber *_cameraFrameGrabber;

float X,Y,Z;
float W=0;//positions of the grip

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->PosX->setText("0");
    ui->PosY->setText("0");
    ui->PosZ->setText("0");
    ui->PosW->setText("0");

    serial= new QSerialPort(this);
    str=ui->port->text();
    deltaf=ui->delta->text();
    delta=deltaf.toFloat();
    serial->setPortName(str);

    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    serial->open(QIODevice::ReadWrite);




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
    connect(ui->slider, SIGNAL(valueChanged(int)), this, SLOT(Slider(int)));
    connect(ui->SetOrigin, SIGNAL(pressed()), this, SLOT(SetOrigin()));
    connect(ui->Goto, SIGNAL(pressed()), this, SLOT(Goto()));
    connect(ui->Pick, SIGNAL(pressed()), this, SLOT(Pick()));
    connect(ui->Place, SIGNAL(pressed()), this, SLOT(Place()));
    connect(ui->HomeX, SIGNAL(pressed()), this, SLOT(HomeX()));
    connect(ui->HomeY, SIGNAL(pressed()), this, SLOT(HomeY()));
    connect(ui->HomeZ, SIGNAL(pressed()), this, SLOT(HomeZ()));

    _camera = new QCamera();
    _cameraFrameGrabber = new CameraFrameGrabber();
    _camera->setViewfinder(_cameraFrameGrabber);
    connect(_cameraFrameGrabber, SIGNAL(frameAvailable(QImage)), this, SLOT(handleFrame(QImage)));
    _camera->start();


    serial->write("G92 X0 Y0 X0\r");


}

MainWindow::~MainWindow()
{
    delete ui;
       serial->close();
}

void MainWindow::Zup()
{
    deltaf=ui->delta->text();
    delta=deltaf.toFloat();
    Z=Z+delta;
    QString s= QString::number(Z);
    s="G1 Z"+s+"\r";
    serial->write(s.toStdString().c_str());
    qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;
    ui->PosZ->setText(s);
}

void MainWindow::Zdown()
{
    deltaf=ui->delta->text();
    delta=deltaf.toFloat();
    Z=Z-delta;
    QString s= QString::number(Z);
    ui->PosZ->setText(s);
    s="G1 Z"+s+"\r";
     serial->write(s.toStdString().c_str());
     qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;

}

void MainWindow::Yminus()
{
    deltaf=ui->delta->text();
    delta=deltaf.toFloat();
    Y=Y-delta;
    QString s= QString::number(Y);
    ui->PosY->setText(s);
    s="G1 Y"+s+"\r";
     serial->write(s.toStdString().c_str());
     qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;

}

void MainWindow::Yplus()
{
    deltaf=ui->delta->text();
    delta=deltaf.toFloat();
    Y=Y+delta;
    QString s= QString::number(Y);
    ui->PosY->setText(s);
    s="G1 Y"+s+"\r";
     serial->write(s.toStdString().c_str());
     qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;
}

void MainWindow::Xleft()
{
    deltaf=ui->delta->text();
    delta=deltaf.toFloat();
    X=X-delta;
    QString s= QString::number(X);
    ui->PosX->setText(s);
    s="G1 X"+s+"\r";
     serial->write(s.toStdString().c_str());
     qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;
}

void MainWindow::Xright()
{
    deltaf=ui->delta->text();
    delta=deltaf.toFloat();
    X=X+delta;
    QString s= QString::number(X);
    ui->PosX->setText(s);
    s="G1 X"+s+"\r";
     serial->write(s.toStdString().c_str());
    qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;
}

void MainWindow::Get()
{

     //serial->write("M106\r"); For Marlin
     serial->write("M3\r"); //Laser/Pump On

}
void MainWindow::Release()
{
    // serial->write("M107\r"); For Marlin
    serial->write("M4\r"); //Laser/Pump Off
}

void MainWindow::rotorright()
{
    /* serial->write("M302 S0\r");
     serial->write("M83 S0\r");
     serial->write("G1 E-1\r");*/ //For Marlin
     serial->write("G1 E-1\r");  //For GerCode
     W=W-delta;
     qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;
}
void MainWindow::rotorleft()
{
     /*serial->write("M302 S0\r");
     serial->write("M83\r");
     serial->write("G1 E1\r");*/
     serial->write("G1 E1\r");  //For GerCode
     W=W-delta;
     W=W+delta;
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
    ui->Messages->appendPlainText(data);

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
        //mCameraViewfinder->setGeometry(375,380,0,0);		// zoom out
    }
}

void MainWindow::handleFrame(QImage imageObject)
{
    int width=460;
    int height=460;
    QRgb value;
    QTransform matrix(-1,0,0,0,1,0,0,0,1); // m11=-1 : flipping horizontal. I do not know why
    value = qRgb(100, 0, 0); // 0xffbd9527
    imageObject=imageObject.transformed(matrix);
    imageObject=imageObject.copy();

    int y,x;
       for ( y=0;y<height;++y) {
                 imageObject.setPixel(640/2, 480/2-height/2+y, value);

           }
   for ( x=0;x<width;++x) {
                 imageObject.setPixel(640/2-width/2+x, 480/2, value);

           }

   QPainter p;
     if (!p.begin(&imageObject))
     {
     }
        p.setPen(QPen(Qt::white));
        p.setFont(QFont("Arial",  12, QFont::Bold));
        QString sx= QString::number((float)X);
        QString sy= QString::number((float)Y);
        QString sz= QString::number((float)Z);
        QString sw= QString::number((float)W);
        QString s;
        s="X:"+sx+" Y:"+sy+" Z:"+sz+" W:"+sw;
        p.drawText(imageObject.rect(), Qt::AlignTop,s);


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
    ui->pic->setPixmap(pixmap);
    this->update();

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

void MainWindow::SetOrigin()
{
X=Y=Z=W=0;
serial->write("G92 X0 Y0 Z0\r");
qDebug()<<"Setting origin at: X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;
ui->PosX->setText("0");
ui->PosY->setText("0");
ui->PosZ->setText("0");
ui->PosW->setText("0");

}

void MainWindow::Goto()
{
    QString s= QString::number(X);
    ui->PosX->setText(s);
    //s="G1 X"+ui->GotoX->text()+" Y"+ui->GotoY->text()+" Z"+ui->GotoZ->text()+" W"+ui->GotoW->text()+" \r";
    s="G1 X"+ui->GotoX->text()+" Y"+ui->GotoY->text()+" \r";
    X=ui->GotoX->text().toFloat();
    Y=ui->GotoY->text().toFloat();
    //Z=ui->GotoZ->text().toFloat();
    qDebug()<<"Going to "<<s.toStdString().c_str();
    //s="G1 X"+ui->GotoX->text()+" Y"+ui->GotoY->text()+" Z"+ui->GotoZ->text()+"\r";
    serial->write(s.toStdString().c_str());


}

void MainWindow::Pick()
{
    QString s;
    float x,y,z;
    x=offsetX+X;
    y=offsetY+Y;
    z=offsetZ;
    QString m = QString::number(x);
    QString n = QString::number(y);
    QString p=  QString::number(z);
    s="G1 X"+m+" Y"+n+" \r";
    serial->write(s.toStdString().c_str());
    qDebug()<<"Going to pick at:"<<s.toStdString().c_str();
    s="G1 Z"+p+" \r";
    serial->write(s.toStdString().c_str());
    qDebug()<<"Going down to pick on:"<<s.toStdString().c_str();
    serial->write("M3\r");//Pump On
    x=x-offsetX;
    y=y-offsetY;
    z=Z;
    m = QString::number(x);
    n = QString::number(y);
    p = QString::number(z);
    s="G1 Z"+p+" \r";
    serial->write(s.toStdString().c_str());
    qDebug()<<"Going up.."<<s.toStdString().c_str();

    s="G1 X"+m+" Y"+n+" \r";
    qDebug()<<"Returning "<<s.toStdString().c_str();
    serial->write(s.toStdString().c_str());



}

void MainWindow::Place()
{
    QString s;
    float x,y,z;
    x=offsetX+X;
    y=offsetY+Y;
    z=offsetZ;
    QString m = QString::number(x);
    QString n = QString::number(y);
    QString p=  QString::number(z);
    s="G1 X"+m+" Y"+n+" \r";
    serial->write(s.toStdString().c_str());
    qDebug()<<"Going to pick at:"<<s.toStdString().c_str();
    s="G1 Z"+p+" \r";
    serial->write(s.toStdString().c_str());
    qDebug()<<"Going down to pick on:"<<s.toStdString().c_str();
    serial->write("M4\r");//Pump Off
    x=x-offsetX;
    y=y-offsetY;
    z=Z;
    m = QString::number(x);
    n = QString::number(y);
    p = QString::number(z);
    s="G1 Z"+p+" \r";
    serial->write(s.toStdString().c_str());
    qDebug()<<"Going up.."<<s.toStdString().c_str();

    s="G1 X"+m+" Y"+n+" \r";
    qDebug()<<"Returning "<<s.toStdString().c_str();
    serial->write(s.toStdString().c_str());
}

void MainWindow::HomeX()
{

serial->write("G28 X0\r");
X=0;
ui->PosX->setText("0");
qDebug()<<"Homing X";
}

void MainWindow::HomeY()
{
Y=0;
serial->write("G28 Y0\r");
ui->PosY->setText("0");
qDebug()<<"Homing X";
}

void MainWindow::HomeZ()
{
Z=0;
serial->write("G28 Z0\r");
ui->PosZ->setText("0");
qDebug()<<"Homing Z";
Z=Z+11.2;
serial->write("G1 Z10\r");
}
