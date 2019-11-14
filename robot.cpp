#include <QSerialPort>
#include <QDebug>
#include <QIODevice>
#include "robot.h"

robot::robot(QString Port)
{

    X=Y=Z=W1=W2=0;
    //serial= new QSerialPort();
    this->setPortName(Port);
    this->setBaudRate(QSerialPort::Baud115200);
    this->setDataBits(QSerialPort::Data8);
    this->setParity(QSerialPort::NoParity);
    this->setStopBits(QSerialPort::OneStop);
    this->setFlowControl(QSerialPort::NoFlowControl);
    this->open(QIODevice::ReadWrite);
    this->dataTerminalReadyChanged(true);

    dcbSerialParams.DCBlength=sizeof(DCB);
    dcbSerialParams.fBinary=TRUE;
    dcbSerialParams.ByteSize=8;
    dcbSerialParams.BaudRate=CBR_115200;
    dcbSerialParams.Parity=NOPARITY;
    dcbSerialParams.StopBits=ONESTOPBIT;
    dcbSerialParams.EofChar=0x1A;
    dcbSerialParams.ErrorChar=0x00;
    dcbSerialParams.EvtChar=0x00;
    dcbSerialParams.XonChar=0x11;
    dcbSerialParams.XoffChar=0x13;
    dcbSerialParams.fRtsControl=RTS_CONTROL_DISABLE;//RTS_CONTROL_HANDSHAKE;//RTS_CONTROL_DISABLE;
    //Setting the DTR to Control_Enable ensures that the Arduino is properly
    //reset upon establishing a connection
    dcbSerialParams.fDtrControl = DTR_CONTROL_ENABLE;
    dcbSerialParams.XonLim=4096;
    dcbSerialParams.XoffLim=4096;

    timeouts.ReadIntervalTimeout = -1;
    timeouts.ReadTotalTimeoutMultiplier = -1;
    timeouts.ReadTotalTimeoutConstant = 500;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 500;
    if (!SetCommTimeouts(this->handle(), &timeouts))
       {
           qDebug()<<" error setcommtimeout";
       };

    if(!SetCommState(this->handle(),&dcbSerialParams))
        {
            qDebug()<<" error SetCommState";
        }


    if(this->isOpen()){
        qDebug()<<"Port"<<Port<<"opened\n";
        status=true;
        }
    else {
        qDebug()<<"Port"<<Port <<"error at opening\n";
        status=false;
    }
    this->write("G92 X0 Y0 X0\r");

}

int robot::moveX(float X1){
    QString s= QString::number(X1);
    X=X1;
    s="G1 X"+s+"\r";
    this->write(s.toStdString().c_str());
    qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W1: "<<W1;
    return 1;
}

int robot::moveY(float Y1){
    QString s= QString::number(Y1);
    Y=Y1;
    s="G1 Y"+s+"\r";
    this->write(s.toStdString().c_str());
    qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W1: "<<W1;
    return 1;
}

int robot::moveZ(float Z1){
    QString s= QString::number(Z1);
    Z=Z1;
    s="G1 Z"+s+"\r";
    this->write(s.toStdString().c_str());
    qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W1: "<<W1;
    return 1;
}
int robot::moveW1(float W){
    QString s= QString::number(W);
    W1=W;
    s="G1 W"+s+"\r";
    this->write(s.toStdString().c_str());
    qDebug()<<"X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W1: "<<W1;
    return 1;
}
int robot::pump1on(){
    this->write("M4\r");
    return 1;
}
int robot::pump1off(){
    this->write("M3\r");
    return 1;
}
int robot::pump2on(){
    this->write("M32\r");; //Pump Small On
    return 1;
}
int robot::pump2off(){
    this->write("M42\r"); //Pump Small Off
    return 1;
}

int robot::lightson(){
    this->write("Lights ON\r");
    return 1;
}
int robot::lightsoff(){
    this->write("Lights OFF\r");
    return 1;
}
int robot::motors_enabled(){
    this->write("M17\r");
    return 1;
}
int robot::motors_disabled(){
    this->write("M18\r");
    return 1;
}

int robot::setOrigin(){
    this->write("G92 X0 Y0 Z0 W0\r");
    qDebug()<<"Setting origin at: X: "<<X<<"Y: "<<Y<<"Z: "<<Z<<"W1: "<<W1;
    return 1;
}
int robot::command(QString s){
    s=s+"\r";
    this->write(s.toStdString().c_str());
    return 1;
}
int robot::query_position(){
    this->write("Counters\r");//Query position
    return 1;
}
QByteArray robot::readyRead(){

    int n=this->bytesAvailable();
    int i=0;
    Data = this->readLine(n);
    QString myString(Data);
    if(myString.contains("Counter", Qt::CaseInsensitive))
        qDebug()<<"Stopped";
    QStringList query = myString.split(',',QString::SkipEmptyParts);
    QString item;

    Q_FOREACH( item , query)
        qDebug()<<i++<<" "<<item;
    if(i>3){
        if(query[0]=="ang"){
            theta1=query[1].toInt();
            theta2=query[2].toInt();
            theta3=query[3].toInt();
        }
        if(query[0]=="os"){
            theta1=query[1].toInt();
            theta2=query[2].toInt();
            theta3=query[3].toInt();
        }
        if(query[0]=="X"){
            theta1=query[1].toInt();
        }
        if(query[0]=="Y"){
            theta2=query[1].toInt();
        }
        if(query[0]=="theta1"){
            theta1=query[1].toInt();
            theta2=query[3].toInt();

        }
    }
    if(i>7){
        if(query[0]=="PZ"){
            Z=query[1].toFloat();
            X=query[3].toFloat();
            theta1=query[5].toFloat();
            theta2=query[7].toFloat();
            qDebug()<<theta1<<" "<<theta2;
        }
    }

    return Data;
}

robot::~robot(){
    this->close();
    if(!this->isOpen()){
        qDebug()<<"Port"<<port <<" closed\n";
        status=false;
        }
    else {
        qDebug()<<"Port error\n";

    }
};
