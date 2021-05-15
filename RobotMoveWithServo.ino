// WiFi үшін кітапхана жүктейміз:
#include <WiFi.h>
#include <Servo.h>

// төменде SSID және Wi-Fi желіміз үшін құпия сөзді енгіземіз: 
const char* ssid     = "spanch";
const char* password = "sanzhik2006";
// сервердің объектісін құраймыз және «80» портын меншіктейміз
WiFiServer server(80);
// HTTP-сұранысын сақтау үшін айнымалы :
String header;
// мотор 1:
int motor1Pin1 = 27; 
int motor1Pin2 = 26; 
int enable1Pin = 14; 

// мотор 2:
int motor2Pin1 = 33; 
int motor2Pin2 = 25; 
int enable2Pin = 32;

// кең импульсты модуляция (КИМ) қасиеттері үшін айнымалылар:
const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 0;
Servo servoUD;
Servo servoLR;
Servo servoClaw1;
Servo servoNeck;
Servo servoCamLR;
Servo servoCamUD;
int servoUDPin = 15;
int servoLRPin = 13;
int servoClaw2 = 12;
int servoNeckPin = 2;
int servoCamLRPin = 4;
int servoCamUDPin = 18;

String valueUD = String(5);
String valueLR = String(5);
String valueClaw = String(5);
String valueNeck = String(5);
String valueCamLR = String(5);
String valueCamUD = String(5);
int pos1UD = 0;
int pos2UD = 0;

int pos1LR = 0;
int pos2LR = 0;

int pos1Claw = 0;
int pos2Claw = 0;

int pos1Neck = 0;
int pos2Neck = 0;

int pos1CamLR = 0;
int pos2CamLR = 0;

int pos1CamUD = 0;
int pos2CamUD = 0;

String valueString = String(5);
int pos1 = 0;
int pos2 = 0;



void setup() {
  Serial.begin(115200);

  servoUD.attach(servoUDPin , 3);
  servoLR.attach(servoLRPin , 2);
  servoClaw1.attach(servoClaw2, 4);
  servoNeck.attach(servoNeckPin, 5);
  servoCamLR.attach(servoCamLRPin, 6);
  servoCamUD.attach(servoCamUDPin, 7);

  // «OUTPUT» режиміндегі мотор контактілерін қосамыз  
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);

  // КИМ – каналының баптауларын береміз: 
  ledcSetup(pwmChannel, freq, resolution);
  
  // КИМ-канал 0-ді ENA және  ENB контактілеріне қосамыз, 
  // яғни,   GPIO-контактісіне моторлардың айналу жылдамдығын басқару үшін :
  ledcAttachPin(enable1Pin, pwmChannel);
  ledcAttachPin(enable2Pin, pwmChannel);

  // ENA және ENB – ға контактілер береміз 
  // «0» толтыру коэффициентімен КИМ – сигнал:
  ledcWrite(pwmChannel, dutyCycle);

  
  // жоғарыда көрсетілген SSID және құпиясөз арқылы Wi-Fi желіге қосыламыз:
  Serial.print("Connecting to ");  //  "Қосыламыз "
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // монитор портында басып шығарамыз
  // локальды IP-мекенжай және веб-серверді жүктейміз: 
  Serial.println("");
  Serial.println("WiFi connected.");  //  "Wi-Fi желіге қосылдық"
  Serial.println("IP address: ");  //  "IP-мекенжай: "
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();  // Келіп түсіп жатқан клиенттерді 
                                           // қабылдау сатысы

  if (client) {                            // Егер жаңа клиент 
                                         // қосылса,
    Serial.println("New Client.");         // мына жайлы хабарламаны  
                                           // монитор портына шығарамыз.
    String currentLine = "";               // Клиенттен келіп түскен мәліметті 
                                           // сақтау үшін 
                                           // жол құрамыз.
    while (client.connected() ) {           // while() циклін іске қосамыз, 
                                         // клиент қосулы кезде
                                           // жұмыс жасайтын 
      if (client.available()) {            // Клиенттің байты болса,
                                           // оқуға келетін 
                                           
        char c = client.read();            // байтты санаймыз, 
        Serial.write(c);                   // және оны монитор  
                                           // портында басып шығарамыз.
        header += c;
        if (c == '\n') {                   // Егер алынған  байт – 
                                           // жаңа алынған жолдың символы болса.
          // Егер біз жаңа жолдың екі бірдей символын алар болсақ,
          // демек, бұл ағымдағы жол бос екендігін көрсетеді. 
          // Бұл клиенттің HTTP- сұранысының соңы, сондықтан оған жауап жібереміз:
          if (currentLine.length() == 0) {
            // HTTP- тақырыпшалар әрқашан жауап кодының жауабымен басталады 
            // (мысалы,  «HTTP/1.1 200 OK»),
            // контент типі жайлы ақпарат, 
            // себебі, клиент өзінің не алып жатқанын көруі керек 
            // Осыдан кейін бос жол жабамыз: 
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
                       //  "Байланыс: өшірулі"
            client.println();
            
            // Веб парақшада қандай батырмалар басылғанына байланысты, 
            //  бұл код мотор контактілерімен басқаруға арналады:
            if (header.indexOf("GET /forward") >= 0) {
              Serial.println("Forward");  //  "Алға"
              digitalWrite(motor1Pin1, LOW);
              digitalWrite(motor1Pin2, HIGH); 
              digitalWrite(motor2Pin1, LOW);
              digitalWrite(motor2Pin2, HIGH);
            }  else if (header.indexOf("GET /left") >= 0) {
              Serial.println("Left");  //  "Влево"
              digitalWrite(motor1Pin1, LOW); 
              digitalWrite(motor1Pin2, LOW); 
              digitalWrite(motor2Pin1, LOW);
              digitalWrite(motor2Pin2, HIGH);
            }  else if (header.indexOf("GET /stop") >= 0) {
              Serial.println("Stop");  //  "Стоп"
              digitalWrite(motor1Pin1, LOW); 
              digitalWrite(motor1Pin2, LOW); 
              digitalWrite(motor2Pin1, LOW);
              digitalWrite(motor2Pin2, LOW);             
            } else if (header.indexOf("GET /right") >= 0) {
              Serial.println("Right");  //  "Вправо"
              digitalWrite(motor1Pin1, LOW); 
              digitalWrite(motor1Pin2, HIGH); 
              digitalWrite(motor2Pin1, LOW);
              digitalWrite(motor2Pin2, LOW);    
            } else if (header.indexOf("GET /reverse") >= 0) {
              Serial.println("Reverse");  //  "Назад"
              digitalWrite(motor1Pin1, HIGH);
              digitalWrite(motor1Pin2, LOW); 
              digitalWrite(motor2Pin1, HIGH);
              digitalWrite(motor2Pin2, LOW);          
            }
            // Веб-парақшаны көрсетеміз:
            client.println("<!DOCTYPE HTML><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS  арқылы батырмалар стилін береміз
            // Өз бетіңізбен тәжірибе жасап көріңіз.
            // «background-color»  атрибуттарымен және «font-size», 
            // өз қалауыңыз бойынша батырмаларды әсемдеу үшін: 
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { -webkit-user-select: none; -moz-user-select: none; -ms-user-select: none; user-select: none; background-color: #4CAF50;");
            client.println("border: none; color: white; padding: 12px 28px; text-decoration: none; font-size: 26px; margin: 1px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style>");
            client.println("<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js\"></script></head>");
            
            // веб-парақша:
            client.println("<body>");
            client.println("<p><button class=\"button\" onclick=\"moveForward()\">FORWARD</button></p>");
            client.println("<div style=\"clear: both;\"><p><button class=\"button\" onclick=\"moveLeft()\">LEFT </button>");
            client.println("<button class=\"button\" onclick=\"moveRight()\">RIGHT</button>");
            client.println("<button class=\"button button2\" onclick=\"stopRobot()\">STOP</button></p></div>");
            client.println("<p><button class=\"button\" onclick=\"moveReverse()\">REVERSE</button></p>");
                    
            client.println("<p><center><iframe src=\"http://192.168.43.51/\" width=\"400\" height=\"300\"></iframe></cemter></p>");
            
            client.println("<p><input type=\"range\" min=\"0\" max=\"100\" step=\"2\" class=\"slider\" id=\"motorSlider\" onchange=\"motorSpeed(this.value)\" value=\"" + valueString + "\"/></p>");
            client.println("<p>Motor Speed: <span id=\"motorSpeed\"></span></p>");    
            client.println("<p><input type=\"range\" min=\"0\" max=\"90\" step=\"2\" class=\"slider\" id=\"servoUD\" onchange=\"servoUDRotation(this.value)\" valueUD=\"" + valueUD + "\"/></p>");
            client.println("<p>Up and Down servo angle degree: <span id=\"servoUDRotation\"></span></p>");  
            client.println("<input type=\"range\" min=\"0\" max=\"180\" step=\"2\" class=\"slider\" id=\"servoLR\" onchange=\"servoLRRotation(this.value)\" valueLR=\"" + valueLR + "\"/>");
            client.println("<p>Left and Right servo angle degree: <span id=\"servoLRRotation\"></span></p>");
            client.println("<p><input type=\"range\" min=\"0\" max=\"120\" step=\"2\" class=\"slider\" id=\"servoClaw\" onchange=\"servoClawRotation(this.value)\" valueClaw=\"" + valueClaw + "\"/></p>");
            client.println("<p>Claw angle degree: <span id=\"servoClawRotation\"></span></p>");
            client.println("<p><input type=\"range\" min=\"0\" max=\"150\" step=\"2\" class=\"slider\" id=\"servoNeck\" onchange=\"servoNeckRotation(this.value)\" valueNeck=\"" + valueNeck + "\"/></p>");
            client.println("<p>Servo Neck angle degree: <span id=\"servoNeckRotation\"></span></p>");
            client.println("<p><input type=\"range\" min=\"0\" max=\"180\" step=\"2\" class=\"slider\" id=\"servoCamLR\" onchange=\"servoCamLRRotation(this.value)\" valueCamLR=\"" + valueCamLR + "\"/></p>");
            client.println("<p>Cam left and right angle degree: <span id=\"servoCamLRRotation\"></span></p>");
            client.println("<p><input type=\"range\" min=\"0\" max=\"180\" step=\"2\" class=\"slider\" id=\"servoCamUD\" onchange=\"servoCamUDRotation(this.value)\" valueCamUD=\"" + valueCamUD + "\"/></p>");
            client.println("<p>Cam up and down angle degree: <span id=\"servoCamUDRotation\"></span></p>");
            
            client.println("<script>$.ajaxSetup({timeout:1000});");
             client.println("function moveForward() { $.get(\"/forward\"); {Connection: close};}");
            client.println("function moveLeft() { $.get(\"/left\"); {Connection: close};}");
            client.println("function stopRobot() {$.get(\"/stop\"); {Connection: close};}");
            client.println("function moveRight() { $.get(\"/right\"); {Connection: close};}");
            client.println("function moveReverse() { $.get(\"/reverse\"); {Connection: close};}");
            client.println("var slider = document.getElementById(\"motorSlider\");");
            client.println("var motorP = document.getElementById(\"motorSpeed\"); motorP.innerHTML = slider.value;");
            client.println("slider.oninput = function() { slider.value = this.value; motorP.innerHTML = this.value; }");
            client.println("function motorSpeed(pos) { $.get(\"/?value=\" + pos + \"&\"); {Connection: close};} ");
            
            client.println("var servoUDSlider = document.getElementById(\"servoUD\");");
            client.println("var servoUDPos = document.getElementById(\"servoUDRotation\"); servoUDPos.innerHTML = servoUDSlider.value;");
            client.println("servoUDSlider.oninput = function() { servoUDSlider.value = this.value; servoUDPos.innerHTML = this.value;}");
            client.println("function servoUDRotation(posUD) { $.get(\"/?UD=\" + posUD + \"&\"); {Connection: close};} ");

            client.println("var servoClawSlider = document.getElementById(\"servoClaw\");");
            client.println("var servoClawPos = document.getElementById(\"servoClawRotation\"); servoClawPos.innerHTML = servoClawSlider.value;");
            client.println("servoClawSlider.oninput = function() { servoClawSlider.value = this.value; servoClawPos.innerHTML = this.value;}");
            client.println("function servoClawRotation(posClaw) { $.get(\"/?CLAW=\" + posClaw + \"&\"); {Connection: close};} ");

            client.println("var servoNeckSlider = document.getElementById(\"servoNeck\");");
            client.println("var servoNeckPos = document.getElementById(\"servoNeckRotation\"); servoNeckPos.innerHTML = servoNeckSlider.value;");
            client.println("servoNeckSlider.oninput = function() { servoNeckSlider.value = this.value; servoNeckPos.innerHTML = this.value;}");
            client.println("function servoNeckRotation(posNeck) { $.get(\"/?NECK=\" + posNeck + \"&\"); {Connection: close};} ");

            client.println("var servoCamLRSlider = document.getElementById(\"servoCamLR\");");
            client.println("var servoCamLRPos = document.getElementById(\"servoCamLRRotation\"); servoCamLRPos.innerHTML = servoCamLRSlider.value;");
            client.println("servoCamLRSlider.oninput = function() { servoCamLRSlider.value = this.value; servoCamLRPos.innerHTML = this.value;}");
            client.println("function servoCamLRRotation(posCamLR) { $.get(\"/?CAMFG=\" + posCamLR + \"&\"); {Connection: close};} ");

            client.println("var servoCamUDSlider = document.getElementById(\"servoCamUD\");");
            client.println("var servoCamUDPos = document.getElementById(\"servoCamUDRotation\"); servoCamUDPos.innerHTML = servoCamUDSlider.value;");
            client.println("servoCamUDSlider.oninput = function() { servoCamUDSlider.value = this.value; servoCamUDPos.innerHTML = this.value;}");
            client.println("function servoCamUDRotation(posCamUD) { $.get(\"/?CAMNM=\" + posCamUD + \"&\"); {Connection: close};} ");

            client.println("var servoLRSlider = document.getElementById(\"servoLR\");");
            client.println("var servoLRPos = document.getElementById(\"servoLRRotation\"); servoLRPos.innerHTML = servoLRSlider.value;");
            client.println("servoLRSlider.oninput = function() { servoLRSlider.value = this.value; servoLRPos.innerHTML = this.value; }");
            client.println("function servoLRRotation(posLR) { $.get(\"/?LR=\" + posLR + \"$\");{Connection: close};} </script>"); 
            
           
            client.println("</body></html>");
            
            // HTTP-сұранысының мысалы: «GET /?value=100& HTTP/1.1»;
            // Бұл КИМ – ді 100% толтыруға коэффициент береді (255): 

             if(header.indexOf("GET /?value=")>=0) {
              pos1 = header.indexOf('=');
              pos2 = header.indexOf('&');
              valueString = header.substring(pos1+1, pos2);
              // Мотор жылдамдығын береміз: 
              if (valueString == "0") {
                ledcWrite(pwmChannel, 0);
                digitalWrite(motor1Pin1, LOW); 
                digitalWrite(motor1Pin2, LOW); 
                digitalWrite(motor2Pin1, LOW);
                digitalWrite(motor2Pin2, LOW);   
              }
              else { 
                dutyCycle = map(valueString.toInt(), 2, 100, 155, 255);
                ledcWrite(pwmChannel, dutyCycle);
                Serial.println(valueString);
              } 
            } else if(header.indexOf("GET /?UD=")>=0){
                pos1UD = header.indexOf('=');
                pos2UD = header.indexOf('&');
                valueUD = header.substring(pos1UD + 1, pos2UD);
                servoUD.write(valueUD.toInt());
                Serial.println(valueUD.toInt());
                delay(valueUD.toInt() * 15);
            } else if(header.indexOf("GET /?LR=")>=0){
                pos1LR = header.indexOf('=');
                pos2LR = header.indexOf('$');
                valueLR = header.substring(pos1LR + 1, pos2LR);
                servoLR.write(valueLR.toInt());
                Serial.println(valueLR.toInt());
                delay(valueLR.toInt() * 15);
            } else if(header.indexOf("GET /?CLAW=")>=0){
                pos1Claw = header.indexOf('=');
                pos2Claw = header.indexOf('&');
                valueClaw = header.substring(pos1Claw + 1, pos2Claw);
                servoClaw1.write(valueClaw.toInt());
                Serial.println(valueClaw.toInt());
                delay(valueClaw.toInt() * 15);
            } else if(header.indexOf("GET /?NECK=")>=0){
                pos1Neck = header.indexOf('=');
                pos2Neck = header.indexOf('&');
                valueNeck = header.substring(pos1Neck + 1, pos2Neck);
                servoNeck.write(valueNeck.toInt());
                Serial.println(valueNeck.toInt());
                delay(valueNeck.toInt() * 15);
            } else if(header.indexOf("GET /?CAMFG=")>=0){
                pos1CamLR = header.indexOf('=');
                pos2CamLR = header.indexOf('&');
                valueCamLR = header.substring(pos1CamLR + 1, pos2CamLR);
                servoCamLR.write(valueCamLR.toInt());
                Serial.println(valueCamLR.toInt());
                delay(valueCamLR.toInt() * 15);
            } else if(header.indexOf("GET /?CAMNM=")>=0){
                pos1CamUD = header.indexOf('=');
                pos2CamUD = header.indexOf('&');
                valueCamUD = header.substring(pos1CamUD+ 1, pos2CamUD);
                servoCamUD.write(valueCamUD.toInt());
                Serial.println(valueCamUD.toInt());
                delay(valueCamUD.toInt() * 15);
            }
            // HTTP- жауап тағы бір бос жолмен аяқталады 
            client.println();
            // while() циклынан шығамыз:
            break;
          } else {  // Егер жаңа жолдың символын алған болсаңыз, 
                    // «currentLine» айнымалысын тазалаймыз:
            currentLine = "";
          }
        } else if (c != '\r') {  // Егер тағы бірдеңе алған болсаңыз,  
                                 // каретка қайтару символынан бөлек... 
          currentLine += c;      // ... онда осы мәліметті енгіземіз 
                                 // «currentLine» айнымалысының соңына 
        }
      }
    }
    // «header» айнымалысын тазалаймыз:
    header = "";
    // Байланысты өшіреміз:
    client.stop();
    Serial.println("Client disconnected.");  // "Клиент өшірулі."
    Serial.println("");
  }
}
