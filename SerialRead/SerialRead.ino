String info;

void loop()
{
    if (Serial.available())
    {    
        info = Serial.readString();
       
    }
 if (info.charAt(1) == 'B') {
    Serial.println("You might have found the Benjamins.");
 }

//if (info.startsWith("!a")) {
//     Serial.println("Got an OK from the server"); 
//}

}

