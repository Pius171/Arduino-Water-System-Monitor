#include "FS.h"
#include "SD.h"
#include "SPI.h"

#define NTP_SERVER     "pool.ntp.org"
#define UTC_OFFSET     0
#define UTC_OFFSET_DST 0

const char* time_zone = "UTC-1";

struct DateTime {
  char timeString[30];
  char dateString[30];
};

// Declare the function
DateTime getLocalTime();

DateTime printLocalTime() {
  struct tm timeinfo;
  DateTime dateTime;
  
  if(!getLocalTime(&timeinfo)){
    strcpy(dateTime.timeString, "No time available (yet)");
    strcpy(dateTime.dateString, "");
    return dateTime;
  }

  strftime(dateTime.timeString, sizeof(dateTime.timeString), "%H:%M:%S", &timeinfo); // Format the time
  strftime(dateTime.dateString, sizeof(dateTime.dateString), "%d/%m/%Y", &timeinfo); // Format the date
  
  return dateTime;
}

void logger_init(){
      //time
      configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
  configTzTime(time_zone,NTP_SERVER);
}
void listDir(const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = SD.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(SD.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(SD.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = SD.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = SD.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = SD.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void renameFile(const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (SD.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(SD.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}


void Log(String pump_1, String pump_2, float volume){
DateTime dateTime=printLocalTime();
char buffer[500];
snprintf(buffer,sizeof(buffer),"%s,%s,%s,%s,%f\n",dateTime.dateString,dateTime.timeString,pump_1,pump_2,volume);
Serial.println(buffer);    

    appendFile("/log.csv", buffer);
    //Serial.println("about to read log");
    //readFile("/log.csv");
}

