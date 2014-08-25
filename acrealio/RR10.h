#ifndef RR10_H
#define RR10_H

#include "Arduino.h"
#include "RfidModule.h"
#include <LiquidCrystal.h>

class RR10 : public RfidModule
{
public:
    RR10();
    void setPins(int sensor, HardwareSerial* serialid);
    void read();
    void update();	
    byte isCardPresent();
    void getUID(byte* uid);	
    void setReaderNumber(int reader);
    void setLcd(LiquidCrystal *passed_lcd, int lcd_rows, int lcd_statusline);
private:
    void sendCmd(byte* cmd);
    boolean cmdUpdate();

private:
    byte card;               // 0 : no card 1:ISO15693 2:Felica
    byte uid[8];
    HardwareSerial* rfSerial;    // rfid Serial
    boolean pinset;  // pin init done flag
    boolean readcmd; // read request from host flag
    
    boolean incmd; // command is being processed
    byte command[256]; //command
    byte comstatus; // 0 : idle, 1 : handshake sent , 2 : command sent , 3 : handshake sent back, waiting for answer
    byte readstatus; // 0 : nothing read, 1 : ISO15693 read, reading Felica 
    unsigned long timesent;
    byte rfidp[256];            // rfid response buffer
    byte rf_i;         //response buffer size

    int readerNumber;           // P1 or P2?

    LiquidCrystal *lcd;
    int lcd_rows;
    int lcd_statusline;
    boolean lcd_enabled = false;

};

#endif
