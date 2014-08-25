#include "Arduino.h"
#include "RR10.h"
#include <SipHash_2_4.h>

RR10::RR10()
{
  card = 0;
  readcmd = false;
  comstatus = 0;
  rf_i = 0;
  pinset = false;
  incmd = false;
  readstatus = 0;
}

void RR10::setPins(int sensor, HardwareSerial* serialid)
{
    rfSerial = serialid;
    rfSerial->begin(115200); //the only possible baud rate for this module
    
    pinset=true;

}

void RR10::update()
{
  if(!pinset)
	return;
  if(!readcmd)
      return;

  // 16 chars + null
  char lcdline[17];
  char hex[2];
      
      
   switch(readstatus)
   {
     case 0:
     {
     //let's read ISO15693 first
     
     byte cmd[7] = {0x07,0x06,0x00,0x00,0x00}; //command 0x06 : ISO15693 Tag Inventory (params : normal mode, no AFI)
     sendCmd(cmd);
     
     if( cmdUpdate() )//when reading is finished
     {
       if(rfidp[2] != 0)//tag found
        {
	  //at least one tag is found, let's read the uid of the first tag (extra tags are ignored)
          for(int i=0;i<8;i++)
          {
             uid[i] = rfidp[11-i];
             if (i == 0) {
                sprintf(lcdline,"%02X",uid[i]);
             } else {
                sprintf(hex,"%02X",uid[i]);
                strcat(lcdline,hex);
             }
          }			

		
          if(uid[0] == 0xE0 && uid[1] == 0x04)  // if correct konami card, reading is done, don't every bother to check for Felica
          {
            card = 1;
            readcmd = false;
            readstatus = 0;

            if (lcd_enabled) {
               if (lcd_rows == 4) {
                 lcd->setCursor(0,1);
                 char line2[17];
                 sprintf(line2,"P%d ISO15693     ",readerNumber);
                 lcd->print(line2);
     
                 lcd->setCursor(-4,2);
                 lcd->print("e-AMUSEMENT PASS");
               }

             if (LCD_STATUSLINE > 1) {
               lcd->setCursor(-4,LCD_STATUSLINE);
             } else {
               lcd->setCursor(0,LCD_STATUSLINE);
             }
             lcd->print(lcdline);
            }
          }
          else
          {
            readstatus = 1;
          }
        
        }
        else
        {
          readstatus = 1;
        }
          
     }
     
     break;
     
     
     }
     
      
     case 1:
     {
     //no ISO15696 found, let's try to find some FeliCa instead

     byte cmd[4] = {0x04,0x0E,0x00,0x0A}; //command 0x0E : FeliCa Tag Inventory
     sendCmd(cmd);
     
     if( cmdUpdate() )//when reading is finished
     {
       
       if(rfidp[2] != 0)//tag found
        {
          
	  //at least one tag is found, let's read the uid of the first tag (extra tags are ignored)
          for(int i=0;i<8;i++)
          {
             uid[i] = rfidp[3+i];
             if (i == 0) {
                sprintf(lcdline,"%02X",uid[i]);
             } else {
                sprintf(hex,"%02X",uid[i]);
                strcat(lcdline,hex);
             }
          }			

          if (lcd_enabled) {
             if (lcd_rows == 4) {
               lcd->setCursor(-4,1);
               char line2[17];
               sprintf(line2,"P%d FeliCa       ",readerNumber);
               lcd->print(line2);
   
               lcd->setCursor(-4,2);
               lcd->print("e-AMUSEMENT PASS");
             }

             if (LCD_STATUSLINE > 1) {
               lcd->setCursor(-4,LCD_STATUSLINE);
             } else {
               lcd->setCursor(0,LCD_STATUSLINE);
             }

             lcd->print(lcdline);
          }

          card = 2;
          readcmd = false;
          readstatus = 0;

  
        }
        else
        {
          readstatus = 2;
        }
            
     } else {
        readstatus = 2;
     }
     
     break;
     
     }      

     case 2:
     {
     // Give ISO14443-A a go, e.g. mifare classic.
     // Hashes the ID it gets back

     while (cmdUpdate()) {
       delay(50);
     }

     byte cmd[5] = {0x05,0x09,0x00,0x00,0x00}; //command 0x09 : ISO14443 Tag Inventory 
     sendCmd(cmd);
     
     if( cmdUpdate() )//when reading is finished
     {
       if(rfidp[3] > 0 && rfidp[3] < 8)//tag found
        {
          int uidlen = rfidp[4];
          byte realuid[uidlen];
          char hexrealuid[uidlen+4];
          
          unsigned char key[] PROGMEM = {0xe8, 0x0b, 0x6e, 0x3a, 0x12, 0x11, 0x40, 0x57,
                                         0x7c, 0x7b, 0xea, 0x17, 0x64, 0x08, 0xe8, 0x6e};

          sipHash.initFromPROGMEM(key);

          // Read a UID!
          for(int i=0;i<uidlen;i++)
          {
             realuid[i] = rfidp[12+i];
             sipHash.updateHash((byte)realuid[i]);

             if (lcd_rows == 4) {
               if (i == 0) {
                  sprintf(hexrealuid,"%02X",realuid[i]);
               } else {
                  sprintf(hex,"%02X",realuid[i]);
                  strcat(hexrealuid,hex);
               }
            }
          }
		
          sipHash.finish();

          uid[0] = 0xE0;
          uid[1] = 0x04;
          sprintf(lcdline,"%s","E004");

          for(int i = 2; i < 8; i++) {
              uid[i] = (byte)sipHash.result[i];
              sprintf(hex,"%02X",uid[i]);
              strcat(lcdline,hex);
          }

          if (lcd_enabled) {
            if (lcd_rows == 4) {
              char line2[17];

              lcd->setCursor(0,1);
              sprintf(line2,"P%d ISO14443A ID:",readerNumber);
              lcd->print(line2);
  
              lcd->setCursor(-4,2);
              lcd->print("                ");
              lcd->setCursor(-4,2);
              strcat(hexrealuid," =");
              lcd->print(hexrealuid);
            }

            if (LCD_STATUSLINE > 1) {
              lcd->setCursor(-4,LCD_STATUSLINE);
            } else {
              lcd->setCursor(0,LCD_STATUSLINE);
            }
            lcd->print(lcdline);
          }

		
          card = 1;
          readcmd = false;
          readstatus = 0;
        }
        else
        {
          card = 0;
          readcmd = false;
          readstatus = 0;
        }
          
     }
     
     break;
     
     
     }
   }
}

byte RR10::isCardPresent()
{
	return card;
}

void RR10::read()
{
  readcmd = true;
}

void RR10::getUID(byte* uida)
{
	memcpy(uida,uid,8);
}


void RR10::sendCmd(byte* cmd)
{
  if(incmd)
    return;
    
  memcpy(command,cmd,cmd[0] - 2);//store command
  
  //compute checksum
  word chksm = 0;
  for(int i=0;i<command[0]-2;i++)
  chksm += command[i];
  command[command[0]-2]=chksm;
  command[command[0]-1]=chksm>>8;
  
  incmd = true;
  
}

//perform next step of the cmd, return true if cmd is finished
boolean RR10::cmdUpdate()
{
  if(!incmd)
    return false;
  
      switch(comstatus)
    {
	case 0://nothing sent yet, need to send handshake
		rf_i = 0;
		rfSerial->write(0x55);
                timesent = millis();
		comstatus = 1;
		break;
	case 1://handshake sent, waiting for ack back, then send request
		if(rfSerial->available() == 0 ) //nothing yet
			break;
		
        	 if(rfSerial->read()!=0xAA) // incorrect answer
        	 {
        	  comstatus = 0; //let's try again from the beginning
        	  break;
        	 }
		
		
		rfSerial->write(command,command[0]); // send command to serial port

		
		comstatus = 2;
		break;		
	case 2: // Command sent, waiting for handshake, then send hanshake back
		if(rfSerial->available() == 0 ) //nothing yet
			break;
		
	    if(rfSerial->read()!=0xA5) // incorrect answer
	    {
		    comstatus = 0; //let's try again from the beginning
			break;
		}
		
		rfSerial->write(0x5A); //send handshake back
		
		comstatus = 3;
		break;
		
	case 3: // Handshake sent back, waiting for answer	
		if(rfSerial->available() == 0 ) //nothing yet
			break;
			
		rfidp[rf_i] = rfSerial->read(); //add received data to the buffer
                rf_i++;
		
		if(rf_i < rfidp[0])//response not fully received yet
			break;
		
		//response has been fully received, let's check it
			
		{//compute checksum
			word chksm=0;
			for(int i=0;i<rfidp[0]-2;i++)
				chksm += rfidp[i];
	
			if(chksm != ((((word)rfidp[rfidp[0]-1])<<8) + rfidp[rfidp[0]-2]) ) //if checksum mismatch
			{
				comstatus = 0; //let's try again from the beginning
				break;
			}
		}
		
		//everythings went fine, cmd was sucessful

		comstatus = 0;
                incmd = false;
                return true;
    }
    
    if(millis() - timesent >1000) //timeout, let's try again
      comstatus = 0;
      
    return false;
    
}

void RR10::setReaderNumber(int reader)
{
    readerNumber = reader;
}

void RR10::setLcd(LiquidCrystal *passed_lcd, int passed_lcd_rows, int passed_lcd_statusline)
{
    lcd = passed_lcd;
    lcd_rows = passed_lcd_rows;
    lcd_statusline = passed_lcd_statusline;
    lcd_enabled = true;
}

