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
#include <QtMath>
#include "cameraframegrabber.h"
#include "rectangle.h"


QSerialPort *serial;
QString str,deltaf;
QCamera *mCamera;
QCameraViewfinder *mCameraViewfinder;
QVBoxLayout *mLayout;
float camera_angle=0;
float offsetX=-42.1;
float offsetY=-6;
float offsetZ=3;
float package_width=12.573;
float package_large=12.573;
float delta=0.1;


QCamera *_camera;
CameraFrameGrabber *_cameraFrameGrabber;

float X,Y,Z;
float X1,Y1,X2,Y2,X3,Y3;
float catOp=0,hypo=1,desfase=0;
float W=0;//positions of the grip

int   r1, g1, b1;
int   ra, ga, ba;
int   rb, gb, bb;
int   rc, gc, bc;
int   rd, gd, bd;
int   re, ge, be;
int   rf, gf, bf;
int   rg, gg, bg;
int   rh, gh, bh;
int   ri, gi, bi;
float zr,zg,zb;

struct blur
{
    float a=2,b=2,c=2;
    float d=2,e=4,f=2;
    float g=2,h=2,i=2;
    float div=(a+b+c+d+e+f+g+h+i);
};

struct sobelx
{
    float a=-1,b=0,c=1;
    float d=-2,e=0,f=2;
    float g=-1,h=0,i=1;
};

struct sobely
{
    float a=-1,b=-2,c=-1;
    float d=0,e=0,f=0;
    float g=1,h=2,i=1;
};

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
    deltaf=ui->delta->text();
    delta=deltaf.toFloat();
    W=W+delta;
    camera_angle=W;
    QString s= QString::number(W);
    ui->PosW->setText(s);
    s="G1 E"+s+"\r";
    serial->write(s.toStdString().c_str());
    qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;
}
void MainWindow::rotorleft()
{
    deltaf=ui->delta->text();
    delta=deltaf.toFloat();
    W=W-delta;
    camera_angle=W;
    QString s= QString::number(W);
    ui->PosW->setText(s);
    s="G1 E"+s+"\r";
    serial->write(s.toStdString().c_str());
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
    int width=640;
    int height=480;
    int r1,g1,b1,a1;
    struct blur Blur;
    struct sobelx Sobelx;
    struct sobely Sobely;
    int n,m;
    rectangle *Rect = new rectangle(package_width,package_large,camera_angle,width,height);

    QRgb value;
    QColor colors;
    QPoint position;
    QTransform matrix(-1,0,0,0,1,0,0,0,1); // m11=-1 : flipping horizontal. I do not know why
    value = qRgb(100, 0, 0); // 0xffbd9527
    imageObject=imageObject.transformed(matrix);
    imageObject=imageObject.copy();




    int y,x;
    float as;
    for ( y=0;y<height;++y) {

                 as=(float)y/(float)(height);
                 imageObject.setPixel(width/2, 480/2-height/2+y, value);
                 Rect->segmentb(as,&m,&n);
                 if(ui->BnW->isChecked())
                     value=qRgb(255,255,255);
                 imageObject.setPixel(m,n, value);
                 Rect->segmentd(as,&m,&n);
                 imageObject.setPixel(m,n, value);


           }
    for ( x=0;x<width;++x) {
                as=(float)x/(float)(width);
                imageObject.setPixel(width/2-width/2+x, height/2, value);
                Rect->segmenta(as,&m,&n);
                if(ui->BnW->isChecked())
                    value=qRgb(255,255,255);
                imageObject.setPixel(m,n, value);
                Rect->segmentc(as,&m,&n);
                imageObject.setPixel(m,n, value);

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

    QImage Image2=imageObject.copy();

        //Blur? Corregir el uso de la matriz en el calculo
        if(ui->BLUR->isChecked()){
          for (y=1;y<(height-1);++y )
          {
              for (x=1;x<(width-1);++x)
              {
                  /* Get pixel's RGB values */
                  position.setX(x-1); position.setY(y-1); colors=Image2.pixelColor(position); colors.getRgb(&ra,&ga,&ba,&a1);
                  position.setX(x); position.setY(y-1); colors=Image2.pixelColor(position); colors.getRgb(&rb,&gb,&bb,&a1);
                  position.setX(x+1); position.setY(y-1); colors=Image2.pixelColor(position); colors.getRgb(&rc,&gc,&bc,&a1);
                  position.setX(x-1); position.setY(y); colors=Image2.pixelColor(position); colors.getRgb(&rd,&gd,&bd,&a1);
                  position.setX(x); position.setY(y); colors=Image2.pixelColor(position); colors.getRgb(&re,&ge,&be,&a1);
                  position.setX(x+1); position.setY(y); colors=Image2.pixelColor(position); colors.getRgb(&rf,&gf,&bf,&a1);
                  position.setX(x-1); position.setY(y+1); colors=Image2.pixelColor(position); colors.getRgb(&rg,&gg,&bg,&a1);
                  position.setX(x); position.setY(y+1); colors=Image2.pixelColor(position); colors.getRgb(&rh,&gh,&bh,&a1);
                  position.setX(x+1); position.setY(y+1); colors=Image2.pixelColor(position); colors.getRgb(&ri,&gi,&bi,&a1);

                  zr=(Blur.a*ra+Blur.b*rb+Blur.c*rc+Blur.d*rd+Blur.e*re+Blur.f*rf+Blur.g*rg+Blur.h*rh+Blur.i*ri)/Blur.div;
                  zg=(Blur.a*ga+Blur.b*gb+Blur.c*gc+Blur.d*gd+Blur.e*ge+Blur.f*gf+Blur.g*gg+Blur.h*gh+Blur.i*gi)/Blur.div;
                  zb=(Blur.a*ba+Blur.b*bb+Blur.c*bc+Blur.d*bd+Blur.e*be+Blur.f*bf+Blur.g*bg+Blur.h*bh+Blur.i*bi)/Blur.div;
                  /* Invert RGB values */
                  value = qRgb((int)zr,(int)zg ,(int)zb);
                  imageObject.setPixel(x,y, value);
              }

              }
            }


        if(ui->BnW->isChecked()){

           for (y=0;y<480;++y) // setting to gray
               for(x=0;x<640;++x)
               {
                   position.setX(x);
                   position.setY(y);
                   colors=imageObject.pixelColor(position);
                   colors.getRgb(&r1,&g1,&b1,&a1);
                   value = qRgb((r1+g1+b1)/3, (r1+g1+b1)/3,(r1+g1+b1)/3);
                   imageObject.setPixel(x,y, value);

               }
        }

        QImage Image3=imageObject.copy();

        float val;
        if(ui->SOBEL->isChecked()){
          for (y=1;y<(480-1);++y )
          {
              for (x=1;x<(640-1);++x)
              {
                  /* Get pixel's RGB values */
                  position.setX(x-1); position.setY(y-1); colors=Image3.pixelColor(position); colors.getRgb(&ra,&ga,&ba,&a1);
                  position.setX(x); position.setY(y-1); colors=Image3.pixelColor(position); colors.getRgb(&rb,&gb,&bb,&a1);
                  position.setX(x+1); position.setY(y-1); colors=Image3.pixelColor(position); colors.getRgb(&rc,&gc,&bc,&a1);
                  position.setX(x-1); position.setY(y); colors=Image3.pixelColor(position); colors.getRgb(&rd,&gd,&bd,&a1);
                  position.setX(x); position.setY(y); colors=Image3.pixelColor(position); colors.getRgb(&re,&ge,&be,&a1);
                  position.setX(x+1); position.setY(y); colors=Image3.pixelColor(position); colors.getRgb(&rf,&gf,&bf,&a1);
                  position.setX(x-1); position.setY(y+1); colors=Image3.pixelColor(position); colors.getRgb(&rg,&gg,&bg,&a1);
                  position.setX(x); position.setY(y+1); colors=Image3.pixelColor(position); colors.getRgb(&rh,&gh,&bh,&a1);
                  position.setX(x+1); position.setY(y+1); colors=Image3.pixelColor(position); colors.getRgb(&ri,&gi,&bi,&a1);
                  zr=(Sobely.a*ra+Sobely.b*rb+Sobely.c*rc+
                      Sobely.d*rd+Sobely.e*re+Sobely.f*rf+
                      Sobely.g*rg+Sobely.h*rh+Sobely.i*ri);
                  zg=(Sobelx.a*ra+Sobelx.b*rb+Sobelx.c*rc+
                      Sobelx.d*rd+Sobelx.e*re+Sobelx.f*rf+
                      Sobelx.g*rg+Sobelx.h*rh+Sobelx.i*ri);
                  val=sqrt(zr*zr+zg*zg)/8;
                  val=(val<60)?255:0;
                  value = qRgb(val,val ,val);
                  imageObject.setPixel(x,y, value);
              }

              }
            }


    QPixmap pixmap = QPixmap::fromImage(imageObject);
    ui->pic->setPixmap(pixmap);
    this->update();

}

void MainWindow::BMP_GetPixelRGB(QImage imageObject,int x,int y,int *r1,int *g1,int *b1){
    QPoint position;
    int a1;
    QColor colors;
    position.setX(x);
    position.setY(y);
    colors=imageObject.pixelColor(position);
    colors.getRgb(r1,g1,b1,&a1);
}

void MainWindow::BMP_SetPixelRGB(QImage imageObject,int x,int y,int  r1,int g1,int  b1){
    QPoint position;
    QRgb value;
    QColor colors;
    position.setX(x);
    position.setY(y);
    value = qRgb((r1+g1+b1)/3, (r1+g1+b1)/3,(r1+g1+b1)/3);
    imageObject.setPixel(x,y, value);
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
    camera_angle=(float)a;
    W=camera_angle;
    QString s= QString::number(W);
    ui->PosW->setText(s);
    s="G1 E"+s+"\r";
    serial->write(s.toStdString().c_str());
    qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W: "<<W;
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
    s="G1 X"+ui->GotoX->text()+" Y"+ui->GotoY->text()+" \r";
    X=ui->GotoX->text().toFloat();
    Y=ui->GotoY->text().toFloat();
    qDebug()<<"Going to "<<s.toStdString().c_str();
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

void MainWindow::on_PLCC28_clicked()
{
    offsetZ=4.572;
    package_width=12.573;
    package_large=12.573;
}

void MainWindow::on_TQFP44_clicked()
{
    offsetZ=1;
    package_width=10;
    package_large=10;
}

void MainWindow::on_A1_clicked()
{
    X1=X;
    Y1=Y;
}

void MainWindow::on_B_clicked()
{
    X2=X;
    Y2=Y;
    hypo=sqrt((X2-X1)*(X2-X1)+(Y2-Y1)*(Y2-Y1));

}

void MainWindow::on_C_clicked()
{
    X3=X;
    Y3=Y;

    catOp=sqrt((X3-X2)*(X3-X2)+(Y3-Y2)*(Y3-Y2));
    desfase=qAsin(catOp/hypo);
    //QString fase();
    ui->FASE->setText(std::to_string(desfase).c_str());
}

void MainWindow::on_Leds_ON_clicked()
{
    serial->write("Lights ON\r");

}

void MainWindow::on_Leds_OFF_clicked()
{
    serial->write("Lights OFF\r");
}

void MainWindow::on_Motors17_clicked()
{
    serial->write("M17\r");
}

void MainWindow::on_Motors18_clicked()
{
    serial->write("M18\r");
}
