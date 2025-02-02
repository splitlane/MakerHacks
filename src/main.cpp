// TODO: power connection, mounting, test webui

#include "motor.cpp"
#include <Arduino.h>
#include <WiFi.h>
#include <WebSocketsServer.h>
#include <ESPmDNS.h>
#include <ESP32Servo.h>
#include <WebServer.h>

Motor frontLeftMotor(26, 27, 25);
Motor frontRightMotor(14, 12, 13);
Motor backLeftMotor(32, 35, 33);
Motor backRightMotor(5, 34, 18);
static const int servoPin1 = 23;
Servo servo1;
static const int servoPin2 = 21;
Servo servo2;
volatile bool isMoving = false;
volatile bool isForward = false;
volatile bool isBackward = false;

const char* ssid = "THR-RC";
const char* password = "12345678";

WebSocketsServer webSocket = WebSocketsServer(81);
WebServer server(80);

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

void moveForward() {
    isForward = true;
    frontLeftMotor.setPower(255);
    frontRightMotor.setPower(255);
    backLeftMotor.setPower(255);
    backRightMotor.setPower(255);
}

void moveBackward() {
    isBackward = true;
    frontLeftMotor.setPower(-255);
    frontRightMotor.setPower(-255);
    backLeftMotor.setPower(-255);
    backRightMotor.setPower(-255);
}

void rotateLeft() {
    if(isBackward) {
        backLeftMotor.setPower(255);
    }
    frontRightMotor.setPower(255);
}

void rotateRight() {
    if(isForward) {
        backRightMotor.setPower(255);
    }
    frontLeftMotor.setPower(255);
}

void stopMovement() {
    isForward = false;
    isBackward = false;
    frontLeftMotor.setPower(0);
    frontRightMotor.setPower(0);
    backLeftMotor.setPower(0);
    backRightMotor.setPower(0);
}

void startAuton() {
    // TODO: change time intervals
    int start = millis();
    servo1.write(10);
    servo2.write(10);

    moveForward();
    delay(3000);
    stopMovement();

    rotateRight();
    delay(1000);
    stopMovement();

    moveForward();
    delay(1000);
    stopMovement();

    rotateLeft();
    delay(2000);
    stopMovement();

    moveForward();
    delay(2000);
    stopMovement();

    rotateRight();
    delay(5000);
    stopMovement();

    while (millis()-start < 19000) {}
    moveForward();
}

void scoreCubes() {
    // TODO: modify angles
    isMoving = true;
    for(int posDegrees = 10; posDegrees <= 100; posDegrees++) {
        if (!isMoving) {
            break;
        }
        servo1.write(posDegrees);
        servo2.write(posDegrees);
        int start = millis();
        while (millis()-start < 20) {}
    }
      int start = millis();
      while (millis()-start < 1000) {}
    for(int posDegrees = 100; posDegrees >= 10; posDegrees--) {
        if (!isMoving) {
            break;
        }
        servo1.write(posDegrees);
        servo2.write(posDegrees);
        int start = millis();
        while (millis()-start < 20) {}
    }
    isMoving = false;
}

void pick() {
    // TODO: modify angles
    servo1.write(0);
    servo2.write(10);
    moveForward();
    int start = millis();
    while (millis()-start < 500) {}
    stopMovement();
    servo1.write(10);
    servo2.write(10);
    moveBackward();
    int start = millis();
    while (millis()-start < 500) {}
    stopMovement();
}

void armUp() {
    servo1.write(servo1.read()+1);
    servo2.write(servo2.read()+1);
}

void armDown() {
    servo1.write(servo1.read()-1);
    servo2.write(servo2.read()-1);
}

void stopArm() {
    isMoving = false;
    servo1.write(servo1.read());
    servo2.write(servo2.read());
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            stopMovement();
            break;
            
        case WStype_CONNECTED:
            Serial.printf("[%u] Connected!\n", num);
            break;
            
        case WStype_TEXT:
            String command = String((char*)payload);
            
            if(command == "W") moveForward();
            else if(command == "S") moveBackward();
            else if(command == "A") rotateLeft();
            else if(command == "D") rotateRight();
            else if(command == "STOP") stopMovement();
            else if(command == "SETUP") startAuton();
            else if(command == "SCORE") scoreCubes();
            else if(command == "ARROWUP") armUp();
            else if(command == "ARROWDOWN") armDown();
            else if(command == "STOPARM") stopArm();
            else if(command == "PICK") pick();
            break;
    }
}

void handleRoot() {
    server.sendContent("<!DOCTYPE html>\n<html>\n<head>\n    <title>Robot Control</title>\n    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n    <style>\n        .control-btn {\n            width: 60px;\n            height: 60px;\n            margin: 5px;\n            font-size: 24px;\n        }\n        .controls {\n            text-align: center;\n            margin-top: 50px;\n        }\n        body {\n            font-family: Arial, sans-serif;\n            background-color: #f0f0f0;\n        }\n        button {\n            background-color: #4CAF50;\n            color: white;\n            border: none;\n            border-radius: 5px;\n            cursor: pointer;\n        }\n        button:active {\n            background-color: #45a049;\n        }\n    </style>\n</head>\n<body>\n    <div class=\"controls\">\n        <div>\n            <button class=\"control-btn\" style=\"width: 100px\" onClick=\"sendCommand('SETUP')\">Auton</button>\n        </div>\n        <div>\n            <button class=\"control-btn\" style=\"width: 100px\" onClick=\"sendCommand('SCORE')\">Score</button>\n        </div>\n        <div>\n            <button class=\"control-btn\" style=\"width: 100px\" onClick=\"sendCommand('PICK')\">Pick</button>\n        </div>\n        <div><p></p></div>\n        <div>\n            <button class=\"control-btn\" onmousedown=\"sendCommand('W')\" \n                    onmouseup=\"sendCommand('STOP')\" \n                    ontouchstart=\"sendCommand('W')\" \n                    ontouchend=\"sendCommand('STOP')\">W</button>\n        </div>\n        <div>\n            <button class=\"control-btn\" onmousedown=\"sendCommand('A')\" \n                    onmouseup=\"sendCommand('STOP')\"\n                    ontouchstart=\"sendCommand('A')\" \n                    ontouchend=\"sendCommand('STOP')\">A</button>\n            <button class=\"control-btn\" onmousedown=\"sendCommand('S')\" \n                    onmouseup=\"sendCommand('STOP')\"\n                    ontouchstart=\"sendCommand('S')\" \n                    ontouchend=\"sendCommand('STOP')\">S</button>\n            <button class=\"control-btn\" onmousedown=\"sendCommand('D')\" \n                    onmouseup=\"sendCommand('STOP')\"\n                    ontouchstart=\"sendCommand('D')\" \n                    ontouchend=\"sendCommand('STOP')\">D</button>\n        </div>\n        <div><p></p></div>\n        <div>\n            <button class=\"control-btn\" onmousedown=\"sendCommand('ARROWUP')\" \n                    onmouseup=\"sendCommand('STOPARM')\" \n                    ontouchstart=\"sendCommand('ARROWUP')\" \n                    ontouchend=\"sendCommand('STOPARM')\">Up</button>\n        </div>\n        <div>\n            <button class=\"control-btn\" style=\"width: 75px\" onmousedown=\"sendCommand('ARROWDOWN')\" \n                    onmouseup=\"sendCommand('STOPARM')\" \n                    ontouchstart=\"sendCommand('ARROWDOWN')\" \n                    ontouchend=\"sendCommand('STOPARM')\">Down</button>\n        </div>\n    </div>\n\n    <script>\n        var socket = new WebSocket('ws://192.168.1.1:81/');\n\n        var mousedown = [];\n        \n        function sendCommand(command) {\n            socket.send(command);\n        }\n        \n        socket.onopen = function(e) {\n            console.log('Connected to robot');\n        };\n\n        socket.onerror = function(e) {\n            console.log('WebSocket error:', e);\n        };\n\n        socket.onclose = function(e) {\n            console.log('WebSocket connection closed');\n        };\n\n        document.addEventListener(\"keydown\", (event) => {\n            const keyName = event.key;\n            switch (keyName) {\n                case 'w':\n                    if (!mousedown.includes(\"W\")) {\n                        Array.from(document.getElementsByTagName('button')).find(button => button.innerText === 'W').dispatchEvent(new MouseEvent('mousedown'));\n                        mousedown.push(\"W\");\n                    }\n                    break;\n                case 'a':\n                    if (!mousedown.includes(\"A\")) {\n                        Array.from(document.getElementsByTagName('button')).find(button => button.innerText === 'A').dispatchEvent(new MouseEvent('mousedown'));\n                        mousedown.push(\"A\");\n                    }\n                    break;\n                case 's':\n                    if (!mousedown.includes(\"S\")) {\n                        Array.from(document.getElementsByTagName('button')).find(button => button.innerText === 'S').dispatchEvent(new MouseEvent('mousedown'));\n                        mousedown.push(\"S\");\n                    }\n                    break;\n                case 'd':\n                    if (!mousedown.includes(\"D\")) {\n                        Array.from(document.getElementsByTagName('button')).find(button => button.innerText === 'D').dispatchEvent(new MouseEvent('mousedown'));\n                        mousedown.push(\"D\");\n                    }\n                    break;\n                case 'ArrowUp':\n                    // if (!mousedown.includes(\"ARROWUP\")) {\n                    Array.from(document.getElementsByTagName('button')).find(button => button.innerText === 'Up').dispatchEvent(new MouseEvent('mousedown'));\n                    //     mousedown.push(\"ARROWUP\");\n                    // }\n                    break;\n                case 'ArrowDown':\n                    // if (!mousedown.includes(\"ARROWDOWN\")) {\n                    Array.from(document.getElementsByTagName('button')).find(button => button.innerText === 'Down').dispatchEvent(new MouseEvent('mousedown'));\n                    //     mousedown.push(\"ARROWDOWN\");\n                    // }\n                    break;\n                default:\n                    break;\n            }\n        });\n\n        document.addEventListener(\"keyup\", (event) => {\n            const keyName = event.key;\n            switch (keyName) {\n                case 'w':\n                    if (mousedown.includes(\"W\")) {\n                        Array.from(document.getElementsByTagName('button')).find(button => button.innerText === 'W').dispatchEvent(new MouseEvent('mouseup'));\n                        delete mousedown[mousedown.indexOf(\"W\")];\n                    }\n                    break;\n                case 'a':\n                    if (mousedown.includes(\"A\")) {\n                        Array.from(document.getElementsByTagName('button')).find(button => button.innerText === 'A').dispatchEvent(new MouseEvent('mouseup'));\n                        delete mousedown[mousedown.indexOf(\"A\")];\n                    }\n                    break;\n                case 's':\n                    if (mousedown.includes(\"S\")) {\n                        Array.from(document.getElementsByTagName('button')).find(button => button.innerText === 'S').dispatchEvent(new MouseEvent('mouseup'));\n                        delete mousedown[mousedown.indexOf(\"S\")];\n                    }\n                    break;\n                case 'd':\n                    if (mousedown.includes(\"D\")) {\n                        Array.from(document.getElementsByTagName('button')).find(button => button.innerText === 'D').dispatchEvent(new MouseEvent('mouseup'));\n                        delete mousedown[mousedown.indexOf(\"D\")];\n                    }\n                    break;\n                // case 'ArrowUp':\n                //     if (mousedown.includes(\"ARROWUP\")) {\n                //         Array.from(document.getElementsByTagName('button')).find(button => button.innerText === 'Up').dispatchEvent(new MouseEvent('mouseup'));\n                //         delete mousedown[mousedown.indexOf(\"ARROWUP\")];\n                //     }\n                //     break;\n                // case 'ArrowDown':\n                //     if (mousedown.includes(\"ARROWDOWN\")) {\n                //         Array.from(document.getElementsByTagName('button')).find(button => button.innerText === 'Down').dispatchEvent(new MouseEvent('mouseup'));\n                //         delete mousedown[mousedown.indexOf(\"ARROWDOWN\")];\n                //     }\n                //     break;\n                default:\n                    break;\n            }\n        });\n    </script>\n</body>\n</html>");
}

void setup() {
    Serial.begin(9600);
    WiFi.softAP(ssid, password);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    Serial.println("WiFi network created");
    Serial.print("SSID: ");
    Serial.println(ssid);
    Serial.print("Password: ");
    Serial.println(password);
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());
    if (MDNS.begin("robot")) {
        Serial.println("MDNS responder started");
    }
    server.on("/", handleRoot);
    server.begin();
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);
    servo1.attach(servoPin1);
    servo2.attach(servoPin2);
}

void loop() {
    webSocket.loop();
    server.handleClient();
}