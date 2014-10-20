#ifndef SL015M_H
#define SL015M_H

#include "Arduino.h"
#include "RfidModule.h"
#include <LiquidCrystal.h>

class SL015M : public RfidModule
{
public:
    SL015M();
    void setPins(int sensor, HardwareSerial* serialid);
    void read();
	void update();	
    byte isCardPresent();
    void getUID(byte* uid);	
    void setReaderNumber(int reader);
    void setLcd(LiquidCrystal *passed_lcd, int lcd_rows, int lcd_statusline);

private:
    byte card;               // card presence
    byte uid[8];
    boolean rfcmdsent;
    unsigned long timesent;
    byte rfidp[256];            // rfid response buffer
    byte rf_i;         //response buffer size
    short rfSENSOR;             // rfid OUT pin
    HardwareSerial* rfSerial;    // rfid Serial
    
    boolean pinset;
    boolean readcmd; // read request from host flag

    int readerNumber;           // P1 or P2?
    LiquidCrystal *lcd;
    int lcd_rows;
    int lcd_statusline;
    boolean lcd_enabled;

};

#endif
