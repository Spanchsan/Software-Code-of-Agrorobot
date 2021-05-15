#include <Arduino.h>
#include <ESP32QRCodeReader.h>

ESP32QRCodeReader reader(CAMERA_MODEL_AI_THINKER);
int pastSeconds = 0;



/*void onQrCodeTask(void *pvParameters)
{


  struct QRCodeData qrCodeData;

  while (true)
  {
    if (reader.receiveQrCode(&qrCodeData, 100))
    {
      Serial.println("Found QRCode");
      if (qrCodeData.valid)
      {
        Serial.print("Payload: ");
        Serial.println((const char *)qrCodeData.payload);
      }
      else
      {
        Serial.print("Invalid: ");
        Serial.println((const char *)qrCodeData.payload);
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
*/

String onQrCodeText()
{
  struct QRCodeData qrCodeData;
  Serial.println("Entered");

  while (true)
  {
    if (reader.receiveQrCode(&qrCodeData, 100))
    {
      Serial.println("Found QRCode");
      if (qrCodeData.valid)
      {
        String code = (const char*)qrCodeData.payload;
        return code;
      }
      else
      {
        Serial.println("Failrure");
      }
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}
void setup()
{
  Serial.begin(115200);
  Serial.println();
  pinMode(12,OUTPUT);
  reader.setup();
  
  Serial.println("Setup QRCode Reader");

  reader.beginOnCore(1);

  Serial.println("Begin on Core 1");

}

void loop()
{
  if(millis() / 1000 - pastSeconds > 5){
    Serial.println("Check");
  String code = onQrCodeText();
  int litres = code.substring(0, code.indexOf('$')).toInt();
  String name = code.substring(code.indexOf('$') + 1);
  Serial.print("Litres:");
  Serial.println(litres);
  Serial.print("Name:");
  Serial.println(name);
  digitalWrite(12, HIGH);
  pastSeconds = millis() / 1000;
  delay(litres * 1000);
  digitalWrite(12, LOW);
  code = "";
  }
  delay(100);
}
