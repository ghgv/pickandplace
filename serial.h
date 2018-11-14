#ifndef SERIAL_H
#define SERIAL_H

// library linking info


// Common Stuff
#include <QThread>
#include <QMutex>
#include <QSemaphore>


///////////////////////////////////////////////////////
//  IF BUILDING ON WINDOWS, IMPLEMENT WINDOWS VERSION
//  THE SERIAL PORT CLASS.
///////////////////////////////////////////////////////

#include <windows.h>
#include <stdint.h>

// default defined baud rates
// custom ones could be set.  These are just clock dividers from some base serial clock.
#define Baud300        CBR_300
#define Baud600        CBR_600
#define Baud1200       CBR_1200
#define Baud2400       CBR_2400
#define Baud4800       CBR_4800
#define Baud9600       CBR_9600
#define Baud19200      CBR_19200
#define Baud38400      CBR_38400
#define Baud57600      CBR_57600



// bytes sizes


class QVSerialPort : public QThread
{
    Q_OBJECT
public:
    explicit QVSerialPort(QObject *parent = 0);
    ~QVSerialPort();
    void usePort(QString *device_Name, int _buad, int _byteSize, int _stopBits, int _parity);
    void closePort();

    // data fetcher, get next byte from buffer
    uint8_t getNextByte();

    // return number of bytes in receive buffer
    uint32_t bytesAvailable();

    // write buffer
    int writeBuffer(QByteArray *buffer);

protected:
    // thread process, called with a start() defined in the base class type
    // This is our hardware receive buffer polling thread
    // when data is received, the hasData() signal is emitted.
    virtual void run();

signals:
    // asynchronous signal to notify there is receive data to process
    void hasData();
    // signal that we couldn't open the serial port
    void openPortFailed();
    // signal that we openned the port correct and are running
    void openPortSuccess();
    // RX buffer overflow signal
    void bufferOverflow();

public slots:
    // we don't need no sinking slots

private:
    // serial port settings
    int Buad;
    int ByteSize;
    int StopBits;
    int Parity;

    HANDLE hSerial;
    bool running;
    QByteArray *dataBuffer;
    QSemaphore *bufferSem;
    QString *deviceName;
    // windows uses a struct called DCB to hold serial port configuration information
    DCB dcbSerialParams;
};


#endif

