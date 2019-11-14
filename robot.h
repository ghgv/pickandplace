#ifndef ROBOT_H
#define ROBOT_H

#include <QString>
#include <QObject>
#include <QSerialPort>
#include <QByteArray>
#include <windows.h>
#include <stdint.h>

class robot :  public QSerialPort
{

public:

    robot(QString Port);
    ~robot();
    int moveX(float X);
    int moveY(float Y);
    int moveZ(float Z);
    int moveW1(float W);
    int moveW2(float W);
    int review();
    int lightson();
    int lightsoff();
    int pump1on();
    int pump1off();
    int pump2on();
    int pump2off();
    int motors_enabled();
    int motors_disabled();
    int setOrigin();
    int command(QString s);
    int query_position();
    QByteArray Data;
    int theta1,theta2,theta3;
    float X,Y,Z,W1,W2,xMAX,xMIN;
    private:
    QString port;
    bool status=false;
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts;
    bool feedback_b=false;


private Q_SLOTS:

Q_SIGNALS:

    QByteArray readyRead();

};

#endif // ROBOT_H
