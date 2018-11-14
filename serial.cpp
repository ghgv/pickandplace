#include "serial.h"


#include <QtDebug>

//////////////////////////////////////////
// Set header file for documentation    /
////////////////////////////////////////

//////////////////////////////////////////////////////
// Windows Version of the serial port driver Code
/////////////////////////////////////////////////////


#include <windows.h>

QVSerialPort::QVSerialPort(QObject *parent) :
    QThread(parent)
{
    // make everything in this thread, run in this thread. (Including signals/slots)
    QObject::moveToThread(this);
    // make our data buffer
    dataBuffer = new QByteArray();
    hSerial = INVALID_HANDLE_VALUE;
    running = true;
    deviceName=NULL;
    bufferSem = new QSemaphore(1); // control access to buffer
}

QVSerialPort::~QVSerialPort() {
    running = false;
    CloseHandle(hSerial);
}

//write data to serial port
int QVSerialPort::writeBuffer(QByteArray *buffer) {
        int dwBytesRead = 0;
    if (hSerial != INVALID_HANDLE_VALUE) {
        // have a valid file discriptor
        WriteFile(hSerial, buffer->constData(), buffer->size(), (DWORD *)&dwBytesRead, NULL);
        return dwBytesRead;
    } else {
        return -1;
    }
}

// setup what device we should use
void QVSerialPort::usePort(QString *device_Name, int _buad, int _byteSize, int _stopBits, int _parity) {
    deviceName = new QString(device_Name->toLatin1());
    // serial port settings
    Buad = _buad;
    ByteSize = _byteSize;
    StopBits = _stopBits;
    Parity = _parity;
}

// data fetcher, get next byte from buffer
uint8_t QVSerialPort::getNextByte() {
    // mutex needed to make thread safe
    bufferSem->acquire(1); // lock access to resource, or wait untill lock is avaliable
    uint8_t byte = (uint8_t)dataBuffer->at(0); // get the top most byte
    dataBuffer->remove(0, 1); // remove top most byte
    bufferSem->release(1);
    return byte; // return top most byte
}

// return number of bytes in receive buffer
uint32_t QVSerialPort::bytesAvailable() {
    // this is thread safe, read only operation
    bufferSem->acquire(1); // lock access to resource, or wait untill lock is avaliable
    uint32_t res = (uint32_t)dataBuffer->size();
    bufferSem->release(1);
    return res;
}

// our main code thread
void QVSerialPort::run() {
//    bufferSem->release(1);      // not in a locked state

    // thread procedure
    //if (_SERIALTHREAD_DEBUG)
    {
        qDebug() << "QVSerialPort: QVSerialPort Started..";
        qDebug() << "QVSerialPort: Openning serial port " << deviceName->toLatin1();
    }

    // open selected device
    hSerial = CreateFile( (WCHAR *) deviceName->constData() , GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if ( hSerial == INVALID_HANDLE_VALUE ) {
        qDebug() << "QVSerialPort: Failed to open serial port " << deviceName->toLatin1();
        emit openPortFailed();
        return;  // exit thread
    }

    // Yay we are able to open device as read/write
    qDebug() << "QVSerialPort: Opened serial port " << deviceName->toLatin1() << " Sucessfully!";

    // now save current device/terminal settings
    dcbSerialParams.DCBlength=sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
                qDebug() << "QVSerialPort: Failed to get com port paramters";
                emit openPortFailed();
                return;
        }

    //if (_SERIALTHREAD_DEBUG)
    {
        qDebug() << "QVSerialPort: Serial port setup and ready for use";
        qDebug() << "QVSerialPort: Starting QVSerialPort main loop";
    }
    dcbSerialParams.BaudRate=Buad;
    dcbSerialParams.ByteSize=ByteSize;
    dcbSerialParams.Parity=Parity;
    dcbSerialParams.StopBits=StopBits;

    if(!SetCommState(hSerial, &dcbSerialParams)) {
                qDebug() << "QVSerialPort: Failed to set new com port paramters";
                emit openPortFailed();
                return;
    }
    COMMTIMEOUTS timeouts;

    timeouts.ReadIntervalTimeout = 0;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 0;
    timeouts.WriteTotalTimeoutMultiplier = 1;
    timeouts.WriteTotalTimeoutConstant = 1;
    if (!SetCommTimeouts(hSerial, &timeouts)){
        qDebug()<<" error setcommtimeout";
    }
    // signal we are opened and running
    emit openPortSuccess();

    static uint8_t byte123[1023]; // temp storage byte
    int dwBytesRead;
    int state=0;    // state machine state

    // start polling loop
    while(running) {
        int ret = ReadFile(hSerial, (void *)byte123, 128, (DWORD *)&dwBytesRead, NULL); // reading 1 byte at a time..  only 2400 baud.
        // print what we received
        if (ret != 0 && dwBytesRead > 0){
            //if (_SERIALTHREAD_DEBUG)
            {
                qDebug() << "QVSerialPort: Received byte with value: " << byte123[0];
            }
            if (dataBuffer->size() > 1023*1024) {
                if ( state == 0 ) {
                    qDebug() << "Local buffer overflow, dropping input serial port data";
                    state = 1;  // over-flowed state
                    emit bufferOverflow();
                }
            } else {
                if ( state == 1 ) {
                    qDebug() << "Local buffer no-longer overflowing, back to normal";
                    state = 0;;
                }
                // stick byte read from device into buffer
                // Mutex needed to make thread safe from buffer read operation
                bufferSem->acquire(1);
                for (int i=0;i<dwBytesRead;i++)
                    dataBuffer->append(byte123[i]);
                bufferSem->release(1);
                emit hasData(); // signal our user that there is data to receive
            }
        }
    }
    CloseHandle(hSerial);
}
