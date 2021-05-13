#include "IMU.h"
#include "Broker.h"
#include "Button.h"

void setup()
{
  //115200
  Serial.begin(9600);
  while (!Serial)
    ;

  // initialize the IMU
  status1 = imu.begin();
  if (status1 < 0)
  {
    Serial.println("Failed to initialize IMU!");
    while (1)
      ;
  }

  /* Set sample rate divider for 50 Hz */
  bool status = imu.setSrd(19);
  if (!status)
  {
    // ERROR
  }

  // get the TFL representation of the model byte array
  tflModel = tflite::GetModel(model);
  if (tflModel->version() != TFLITE_SCHEMA_VERSION)
  {
    Serial.println("Model schema mismatch!");
    while (1)
      ;
  }

  tflOpsResolver.AddBuiltin(
      tflite::BuiltinOperator_DEPTHWISE_CONV_2D,
      tflite::ops::micro::Register_DEPTHWISE_CONV_2D());
  tflOpsResolver.AddBuiltin(
      tflite::BuiltinOperator_MAX_POOL_2D,
      tflite::ops::micro::Register_MAX_POOL_2D());
  tflOpsResolver.AddBuiltin(tflite::BuiltinOperator_CONV_2D,
                            tflite::ops::micro::Register_CONV_2D());
  tflOpsResolver.AddBuiltin(
      tflite::BuiltinOperator_FULLY_CONNECTED,
      tflite::ops::micro::Register_FULLY_CONNECTED());
  tflOpsResolver.AddBuiltin(tflite::BuiltinOperator_SOFTMAX,
                            tflite::ops::micro::Register_SOFTMAX());

  // Create an interpreter to run the model
  tflInterpreter = new tflite::MicroInterpreter(tflModel, tflOpsResolver, tensorArena, tensorArenaSize, &tflErrorReporter);

  // Allocate memory for the model's input and output tensors
  tflInterpreter->AllocateTensors();

  // Get pointers for the model's input and output tensors
  tflInputTensor = tflInterpreter->input(0);
  tflOutputTensor = tflInterpreter->output(0);

  // output declareren
  pinMode(SpeakerPin, OUTPUT);
  pinMode(PushButton, INPUT);

  // Attach an interrupt to the ISR vector
  attachInterrupt(15, pin_ISR, CHANGE);

  // testen
  AantalSquat = 0;
  AantalKPunch = 0;
  AantalSwing = 0;
  geenFouten = true;

  // wifi setup
  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);
  oefReeks = esp_random() % 4 + 1;
  Serial.println("De setup is klaar.");
  Serial.print("Oefeningenreeks: ");
  Serial.println(oefReeks);
  Serial.println("");
}

void loop()
{
  if (!client.connected())
  {
    reconnect(oefReeks);
  }
  client.loop();

  if (buttonPressed == true)
  {
    digitalWrite(SpeakerPin, HIGH);
    delay(100);
    digitalWrite(SpeakerPin, LOW);
    delay(80);
    digitalWrite(SpeakerPin, HIGH);
    delay(100);
    digitalWrite(SpeakerPin, LOW);

    // put your main code here, to run repeatedly:
    geenFouten = true;
    client.publish("esp32/fitness/LCDmessage", "meten");
    switch (oefReeks)
    {
    case 1:
      while (AantalSwing < 7 && geenFouten == true)
      {
        fitnessTracken();
        if (motion != 1)
        {
          geenFouten = false;
        }
        else
        {
          client.publish("esp32/fitness/OKmessage", "oefOK");
        }
      }
      if (geenFouten == true)
      {
        digitalWrite(SpeakerPin, HIGH);
        delay(900);
        digitalWrite(SpeakerPin, LOW);
        client.publish("esp32/fitness/LCDmessage", "klaar");
        delay(5000);
        client.publish("esp32/fitness/LCDmessage", "meten");
      }

      while (AantalSquat < 7 && geenFouten == true)
      {
        fitnessTracken();
        if (motion != 0)
        {
          geenFouten = false;
        }
        else
        {
          client.publish("esp32/fitness/OKmessage", "oefOK");
        }
      }
      if (geenFouten == true)
      {
        digitalWrite(SpeakerPin, HIGH);
        delay(900);
        digitalWrite(SpeakerPin, LOW);
        client.publish("esp32/fitness/LCDmessage", "klaar");
        delay(5000);
        client.publish("esp32/fitness/LCDmessage", "meten");
      }

      while (AantalKPunch < 7 && geenFouten == true)
      {
        fitnessTracken();
        if (motion != 2)
        {
          geenFouten = false;
        }
        else
        {
          client.publish("esp32/fitness/OKmessage", "oefOK");
        }
      }
      break;
    case 2:
      while (AantalRotate < 7 && geenFouten == true)
      {
        fitnessTracken();
        if (motion != 3)
        {
          geenFouten = false;
        }
        else
        {
          client.publish("esp32/fitness/OKmessage", "oefOK");
        }
      }
      if (geenFouten)
      {
        digitalWrite(SpeakerPin, HIGH);
        delay(900);
        digitalWrite(SpeakerPin, LOW);
        client.publish("esp32/fitness/LCDmessage", "klaar");
        delay(5000);
        client.publish("esp32/fitness/LCDmessage", "meten");
      }

      while (AantalKPunch < 7 && geenFouten == true)
      {
        fitnessTracken();
        if (motion != 2)
        {
          geenFouten = false;
        }
        else
        {
          client.publish("esp32/fitness/OKmessage", "oefOK");
        }
      }
      if (geenFouten == true)
      {
        digitalWrite(SpeakerPin, HIGH);
        delay(900);
        digitalWrite(SpeakerPin, LOW);
        client.publish("esp32/fitness/LCDmessage", "klaar");
        delay(5000);
        client.publish("esp32/fitness/LCDmessage", "meten");
      }

      while (AantalSquat < 7 && geenFouten == true)
      {
        fitnessTracken();
        if (motion != 0)
        {
          geenFouten = false;
        }
        else
        {
          client.publish("esp32/fitness/OKmessage", "oefOK");
        }
      }
      break;
    case 3:
      while (AantalKPunch < 7 && geenFouten == true)
      {
        fitnessTracken();
        if (motion != 2)
        {
          geenFouten = false;
        }
        else
        {
          client.publish("esp32/fitness/OKmessage", "oefOK");
        }
      }
      if (geenFouten)
      {
        digitalWrite(SpeakerPin, HIGH);
        delay(900);
        digitalWrite(SpeakerPin, LOW);
        client.publish("esp32/fitness/LCDmessage", "klaar");
        delay(5000);
        client.publish("esp32/fitness/LCDmessage", "meten");
      }

      while (AantalRotate < 7 && geenFouten == true)
      {
        fitnessTracken();
        if (motion != 3)
        {
          geenFouten = false;
        }
        else
        {
          client.publish("esp32/fitness/OKmessage", "oefOK");
        }
      }
      if (geenFouten == true)
      {
        digitalWrite(SpeakerPin, HIGH);
        delay(900);
        digitalWrite(SpeakerPin, LOW);
        client.publish("esp32/fitness/LCDmessage", "klaar");
        delay(5000);
        client.publish("esp32/fitness/LCDmessage", "meten");
      }

      while (AantalSwing < 7 && geenFouten == true)
      {
        fitnessTracken();
        if (motion != 1)
        {
          geenFouten = false;
        }
        else
        {
          client.publish("esp32/fitness/OKmessage", "oefOK");
        }
      }
      break;
    case 4:
      while (AantalSquat < 7 && geenFouten == true)
      {
        fitnessTracken();
        if (motion != 0)
        {
          geenFouten = false;
        }
        else
        {
          client.publish("esp32/fitness/OKmessage", "oefOK");
        }
      }
      if (geenFouten)
      {
        digitalWrite(SpeakerPin, HIGH);
        delay(900);
        digitalWrite(SpeakerPin, LOW);
        client.publish("esp32/fitness/LCDmessage", "klaar");
        delay(5000);
        client.publish("esp32/fitness/LCDmessage", "meten");
      }

      while (AantalSwing < 7 && geenFouten == true)
      {
        fitnessTracken();
        if (motion != 1)
        {
          geenFouten = false;
        }
        else
        {
          client.publish("esp32/fitness/OKmessage", "oefOK");
        }
      }
      if (geenFouten == true)
      {
        digitalWrite(SpeakerPin, HIGH);
        delay(900);
        digitalWrite(SpeakerPin, LOW);
        client.publish("esp32/fitness/LCDmessage", "klaar");
        delay(5000);
        client.publish("esp32/fitness/LCDmessage", "meten");
      }

      while (AantalRotate < 7 && geenFouten == true)
      {
        fitnessTracken();
        if (motion != 3)
        {
          geenFouten = false;
        }
        else
        {
          client.publish("esp32/fitness/OKmessage", "oefOK");
        }
      }
      break;
    default:
      break;
    }
    delay(100);
    if (geenFouten == true)
    {
      Serial.println("De reeks juist uitgevoerd.");
      client.publish("esp32/fitness/LCDmessage", "juist");
    }
    else
    {
      Serial.println("De reeks was FOUT!!");
      client.publish("esp32/fitness/LCDmessage", "fout");
    }

    digitalWrite(SpeakerPin, HIGH);
    delay(100);
    digitalWrite(SpeakerPin, LOW);
    delay(80);
    digitalWrite(SpeakerPin, HIGH);
    delay(100);
    digitalWrite(SpeakerPin, LOW);

    delay(100);
    // Serial.print("Aantal Squats: ");
    // Serial.println(AantalSquat);
    // Serial.print("Aantal KPunches: ");
    // Serial.println(AantalKPunch);
    // Serial.print("Aantal Swings: ");
    // Serial.println(AantalSwing);
    // Serial.println("");

    AantalSquat = 0;
    AantalSwing = 0;
    AantalKPunch = 0;
    AantalRotate = 0;
    buttonPressed = false;
  }
}