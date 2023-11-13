#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MLX90614.h>

#define MAX_SAMPLE_COUNT 30
 
 // For the SSD1306 I2C supported Oled Display Module
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 32    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
 
 
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
 
int temp_amb;
int temp_obj;
 
void initI2CPeripherals()
{
  mlx.begin();         //Initialize MLX90614
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); //initialize with the I2C addr 0x3C (128x64)
  display.clearDisplay();
  delay(500);
}

 
void runI2C()
{
 
  temp_amb = mlx.readAmbientTempC();
  temp_obj = mlx.readObjectTempC();
 
 
  display.clearDisplay();
  display.setCursor(25,5);  
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println("Temp:");
  display.setCursor(25,15);
  display.setTextSize(2);
  display.print(temp_obj);
  display.print((char)247);
  display.print("C");
  display.display();
 
  delay(100);
}

int getTempAvg(){
  int temp_avg = 0;
  for (int i=0; MAX_SAMPLE_COUNT > i; i++){
    temp_avg += mlx.readObjectTempC();
  }

  temp_avg /= MAX_SAMPLE_COUNT;
  return temp_avg;
}
