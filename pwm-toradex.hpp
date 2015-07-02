#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>


// Registers/etc.
#define __MODE1                0x00
#define __MODE2                0x01
#define __SUBADR1              0x02
#define __SUBADR2              0x03
#define __SUBADR3              0x04
#define __PRESCALE             0xFE
#define __LED0_ON_L            0x06
#define __LED0_ON_H            0x07
#define __LED0_OFF_L           0x08
#define __LED0_OFF_H           0x09
#define __ALL_LED_ON_L         0xFA
#define __ALL_LED_ON_H         0xFB
#define __ALL_LED_OFF_L        0xFC
#define __ALL_LED_OFF_H        0xFD

// Bits
#define __RESTART              0x80
#define __SLEEP                0x10
#define __ALLCALL              0x01
#define __INVRT                0x10
#define __OUTDRV               0x04

#define DEBUG 1

using namespace std;

typedef uint16_t myShort;
typedef uint8_t myByte;

class PWMDriver{
  int f;
  // writes 8 bits (1 byte) data to register reg
  myByte i2cWrite8 (myByte reg, myByte data);
  // reads 1 byte from register reg
  myByte i2cRead8 (myByte reg);
  // path do device file
  string device;
  // address of a device on the i2c bus
  myByte address;
public:
  PWMDriver(){
    device = "/dev/i2c-0";
    address = 0x40;
  };
  PWMDriver(string dev, myByte addr){
    device = dev;
    address = addr;
  };
  // opens the I2C device and register it as a slave
  void openDevice();
  // closes the device
  void closeDevice();

  void setPWMFrequency(double freq);
  void setPWM(myByte servoNumber, myShort on, myShort off);
  void setAllPWM(myShort on, myShort off);
};


