#include "pwm-toradex.hpp"

void PWMDriver::openDevice(){
  f=open(device.c_str(),O_RDWR);
  if (f<0){
    cerr << "Device " << device << " could not be opened!" << endl;
    exit(EXIT_FAILURE);
  }

  if(ioctl(f, I2C_SLAVE, address) < 0){
    cerr << "Binding I2C slave device failed!" << endl;
    exit(EXIT_FAILURE);
  }

  this->setAllPWM(0, 0);
 
  this->i2cWrite8(__MODE2, __OUTDRV);
  this->i2cWrite8(__MODE1, __ALLCALL);

  usleep(5000); // wait for oscillator

  myByte mode1 = this->i2cRead8(__MODE1);
  mode1 = mode1 & (~__SLEEP); // wake up, reset sleep
  this->i2cWrite8(__MODE1, mode1);

  usleep(5000); // wait for oscillator
    
}

void PWMDriver::closeDevice(){
  
  if(close(f)!=0){
    cerr << "Device was not closed successfully!" << endl;
  }
}

myByte PWMDriver::i2cRead8(myByte reg){
  myByte buf[2];
  memset(buf,0,2);
  
  if( read(f,buf,1) != 1){
    cerr << "Reading from I2C device failed!" << endl;
    return 0;
  }
  return buf[0];
}

myByte PWMDriver::i2cWrite8(myByte reg, myByte buf){
  myByte b[2];
  b[0]=reg;
  b[1]=buf;

  if(write(f,b,2) != 2){
    cerr << "Error writing to I2C device" << endl;
    return 0;
  }
  
  return 1;
}

void PWMDriver::setPWMFrequency(double freq){
  double prescaleVal=25000000.0f; // 25MHz
  prescaleVal /= 4096; // 12-bit
  prescaleVal /= freq;
  prescaleVal -= 1.0;

  #if DEBUG
  cout << "Setting PWM frequency to " << freq << " Hz" << endl;
  cout << "Estimated prescale: " << prescaleVal << endl;
  #endif

  prescaleVal = floor(prescaleVal + 0.5);

  #if DEBUG
  cout << "Final prescale: " << prescaleVal << endl;
  #endif

  myByte oldMode = this->i2cRead8(__MODE1);
  myByte newMode = (oldMode & 0x7F) | 0x10;

  this->i2cWrite8(__MODE1, newMode);
  this->i2cWrite8(__PRESCALE, (myByte)prescaleVal);
  this->i2cWrite8(__MODE1, oldMode);

  usleep(5000);
  this->i2cWrite8(__MODE1,oldMode | 0x80);
  
}

void PWMDriver::setPWM(myByte servoNumber, myShort on, myShort off){
  this->i2cWrite8(__LED0_ON_L + 4*servoNumber,on & 0xff);
  this->i2cWrite8(__LED0_ON_H + 4*servoNumber, on >> 8);
  this->i2cWrite8(__LED0_OFF_L + 4*servoNumber,off & 0xff);
  this->i2cWrite8(__LED0_OFF_H + 4*servoNumber, off >> 8);
}
void PWMDriver::setAllPWM(myShort on, myShort off){
  this->i2cWrite8(__ALL_LED_ON_L, on & 0xff);
  this->i2cWrite8(__ALL_LED_ON_H, on >> 8);
  this->i2cWrite8(__ALL_LED_OFF_L, off & 0xff);
  this->i2cWrite8(__ALL_LED_OFF_H, off >> 8);
}

int main(){
  PWMDriver pwm;
  
  pwm.openDevice();  
  pwm.setPWMFrequency(60);
  pwm.setPWM(0, 0, 500);
  pwm.closeDevice();
}
