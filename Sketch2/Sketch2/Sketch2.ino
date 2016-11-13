/*
  Name:    Sketch2.ino
  Created: 9/12/2016 12:13:21 PM
  Author:  Neri Ortez
  
*/


#include "sim900.h"
#include "GPRS_Shield_Arduino.h"
#include "Entrada.h"
#include <SoftwareSerial.h>
#include <EEPROM.h>

#define PIN_TX    5
#define PIN_RX    6
#define BAUDRATE  9600

#define NADA "nada"

#define LONGITUD_MENSAJE 160
#define LONGITUD_NUMEROS 13

#define LON_ID_SITIO 7
#define LON_SITIO 15
#define LON_CONTRA 10
#define EE_SITIO 150
#define DIR_SITIO 7
#define EE_CONTRA 255
#define DIR_CONTRA 8

#define ESCRIBIR "escribir"
#define BORRAR "borrar"
#define ASK_FOR_NUMEROS "memoria?"
#define NULO "NULO"
#define CAMBIAR_SITIO "sitio"
#define CAMBIAR_IDSITIO "idsitio"
#define ASK_SITIO "sitio?"
#define CONTRA "contra"
#define TODOS "todos"


GPRS Sim(PIN_TX, PIN_RX, BAUDRATE);
bool resp = false;

EntradaClass entrada2;
EntradaClass entrada1;
EntradaClass entrada3;

int ALARMA1 = 2;//Entrada de ALARMA1
int ALARMA2 = 3;//Entrada de ALARMA2
int ALARMA3 = 4;

struct NumeroTelefono { //Estructura para guardar numeros en EEPROM
  char numero[LONGITUD_NUMEROS];		//Numero de celular
};

// Variables para numeros
NumeroTelefono numero1;
NumeroTelefono numero2;
NumeroTelefono numero3;
NumeroTelefono numero4;
NumeroTelefono numero5;
NumeroTelefono numero6;
NumeroTelefono numero7;
//Variables de EEPROM
const int direcciones[9] = { 0, 15, 30, 45, 60, 75, 90, EE_SITIO, EE_CONTRA }; //Array de direccion en EEPROM

struct Sitio  //Estructura para en futuro modificar el sitio en la EEPROM
{
  char IDSitio[LON_ID_SITIO];
  char NombreSitio[LON_SITIO];
};
Sitio sitio;

struct Contra
{
  char contra[LON_CONTRA];
};
Contra contra;


char sms[LONGITUD_MENSAJE];//Mensaje recibido
char sms_num[LONGITUD_NUMEROS];
char diahora[24];


char IDSitio[LON_ID_SITIO];
char NombreSitio[LON_SITIO];
char CONTRA_TEMP[LON_CONTRA] = { "CLARO" };

#define DEBUG false


//int entradaCambiada = 0;

// ============================================================= SETUP ====================================//
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(ALARMA1, INPUT);
  entrada1.init(ALARMA1, false);
  pinMode(ALARMA2, INPUT);
  entrada2.init(ALARMA2, false);
  pinMode(ALARMA3, INPUT);
  entrada3.init(ALARMA3, false);

  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);
  
  Serial.begin(9600);
  while (Serial.available() == -1) {
    ;
  }
  Sim.resetModule();
  setupe();
}

void setupe(){
  leerEEPROM();
  leerSITIO();
  leerContra();

  //String("1234").toCharArray(contra.contra, LON_CONTRA);

  Serial.println("Iniciando Modulo SIM");
  bool init = false;
  while (!init) {
    if(!resp){
      resp = Sim.begin();
    }
    else {
      init = Sim.init();
    }
      Serial.print("init error\r\n");
      delay(1000);
  }
  delay(3000);
  Sim.deleteAll();
}
void leerEEPROM() {
  //Serial.println("Obteniendo numeros de la EEPROM");
  EEPROM.get(direcciones[0], numero1);
  Serial.println(numero1.numero);
  EEPROM.get(direcciones[1], numero2);
  Serial.println(numero2.numero);
  EEPROM.get(direcciones[2], numero3);
  Serial.println(numero3.numero);
  EEPROM.get(direcciones[3], numero4);
  Serial.println(numero4.numero);
  EEPROM.get(direcciones[4], numero5);
  Serial.println(numero5.numero);
  EEPROM.get(direcciones[5], numero6);
  Serial.println(numero6.numero);
  EEPROM.get(direcciones[6], numero7);
  Serial.println(numero7.numero);
}

void leerSITIO() {
  EEPROM.get(direcciones[DIR_SITIO], sitio);
  Serial.println(sitio.NombreSitio);
  Serial.println(sitio.IDSitio);
}

void cambiarSITIO() {
  EEPROM.put(direcciones[DIR_SITIO], sitio);
}

void leerContra() {
  EEPROM.get(direcciones[DIR_CONTRA], contra);
}

void cambiarContra() {
  EEPROM.put(direcciones[DIR_CONTRA], contra);
}

void blinkLED(int tiempo, int veces){
	for (size_t i = 0; i < veces; i++)
	{
		digitalWrite(LED_BUILTIN, HIGH);
		delay(tiempo);
		digitalWrite(LED_BUILTIN, LOW);
		delay(tiempo);
	}
 Serial.write(("."));
}

void loop()
{
  blinkLED(100, 1);
  Serial.println("Iniciando Loop");
  
#if !DEBUG
  blinkLED(100, 1);
  if (entrada1.entradaCambio()) {
    //Serial.println("Entrada 1 cambio");
    enviarCambio(entrada1.getPin());
  }
  blinkLED(100, 1);
  if (entrada2.entradaCambio()) {
    //Serial.println("Entrada 2 cambio");
    enviarCambio(entrada2.getPin());
  }
  blinkLED(100, 1);
  if (entrada3.entradaCambio()) {
    enviarCambio(entrada3.getPin());
  }
#endif
  //Serial.println("buscando mensajes sms");
/*
  int sss;
  sss = Sim.isSMSunread();
  Serial.println(sss);*/
 
  blinkLED(100, 1);
  if (Sim.readSMS(1, sms, LONGITUD_MENSAJE, sms_num, diahora)) {
    Sim.deleteAll();
    if(strlen(sms_num) > 7){
      Serial.println(F("Mensaje recibido"));
      Serial.println(msjrecibido());
    }
  }
  
  blinkLED(100,1);
  if(!Sim.checkSIMStatus()){
    Sim.resetModule();
    setupe();
    #ifdef RES
      digitalWrite(RES, HIGH);
    #endif
  }
  blinkLED(100,1);
  delay(3000);

}



void enviarCambio(int entrada)
{
  blinkLED(400, 2);
  Serial.println(F("Cambio en la entrada:"));
  Serial.println(entrada);
  String str;
  if (entrada == 2) {
    str = String(sitio.IDSitio) + "\n" +
          String(sitio.NombreSitio) +
          "\nMG " + (digitalRead(entrada) ? String(F("OPERANDO")) : String(F("SE APAGO")));
  }
  else if (entrada == 3) {
	  str = String(F("***ALARMA***\n")) +
		  String(sitio.IDSitio) + "\n" +
		  String(sitio.NombreSitio) +
		  (digitalRead(entrada) ? String(F("\nALARMA GENERAL DE MG - OVERCRANK")) : String(F("\nLA ALARMA SE HA RESETEADO")));
  }
  else if(entrada == 4){
    str = String(F("***ADVERTENCIA***\n")) +
		String(sitio.IDSitio) + "\n" +
		String(sitio.NombreSitio) +
		(digitalRead(entrada) ? String(F("\nBAJO NIVEL DE COMBUSTIBLE")) : String(F("\nNIVEL DE COMBUSTIBLE OPTIMO")));
  }
  if (String(numero1.numero).indexOf("+504") != -1)
  {
    if(!Sim.sendSMS(numero1.numero, &str[0u] /*mensaje*/)){
      Serial.println(F("Error sending"));
      Sim.resetModule();
      resp = false;
      setupe();
    }
  }
  if (String(numero2.numero).indexOf("+504") != -1)
  {
    Sim.sendSMS(numero2.numero, &str[0u] /*mensaje*/);
  }
  if (String(numero3.numero).indexOf("+504") != -1)
  {
    //Serial.println("Enviando a numero 3");
    Sim.sendSMS(numero3.numero, &str[0u] /*mensaje*/);
  }
  if (String(numero4.numero).indexOf("+504") != -1)
  {
    Sim.sendSMS(numero4.numero, &str[0u] /*mensaje*/);
  }
  if (String(numero5.numero).indexOf("+504") != -1)
  {
    Sim.sendSMS(numero5.numero, &str[0u] /*mensaje*/);
  }
  if (String(numero6.numero).indexOf("+504") != -1)
  {
    Sim.sendSMS(numero6.numero, &str[0u] /*mensaje*/);
  }
  if (String(numero7.numero).indexOf("+504") != -1)
  {
    Sim.sendSMS(numero7.numero, &str[0u] /*mensaje*/);
  }
}


// Decoficar mensaje recibido
bool msjrecibido() {
	blinkLED(400, 4);
  String mensaje_codificado = String(sms);
  int index1 = mensaje_codificado.indexOf(' ');				//Indice de la contrase
  int index2 = mensaje_codificado.indexOf(' ', index1 + 1);	//Indice del comando
  int index3 = mensaje_codificado.indexOf(' ', index2 + 1);	//Indice de la posicion

  String cont = mensaje_codificado.substring(0, index1);
  String com = mensaje_codificado.substring(index1 + 1, index2);
  String poss = mensaje_codificado.substring(index2 + 1, index3);

  NumeroTelefono temp;
  //Serial.println(String(contra.contra));
  if (cont == String(contra.contra)) {
    // ESCOGER COMANDO
    if (com == String(ESCRIBIR)) {
      String numm = mensaje_codificado.substring(index3 + 1/*, index4*/);
      //Serial.println("Escribir");
      bool err = true;
      Serial.println(numm);
      if (/*(!(index4 == -1)) && */(numm.indexOf("+504") != -1) && (numm.length() == 12))
      {
        //Serial.println("Formato de numero correcto");
        //String num = sms.substring(index3 + 1, index4);		 // Obtener el numero
        int pos = poss.toInt(); //REVISAR POSICION
        numm.toCharArray(temp.numero, LONGITUD_NUMEROS); //Variable temporal
        switch (pos)
        {
          case 1:
            numm.toCharArray(numero1.numero, LONGITUD_NUMEROS);
            EEPROM.put(direcciones[pos - 1], temp);
            break;
          case 2:
            numm.toCharArray(numero2.numero, LONGITUD_NUMEROS);
            EEPROM.put(direcciones[pos - 1], temp);
            break;
          case 3:
            numm.toCharArray(numero3.numero, LONGITUD_NUMEROS);
            EEPROM.put(direcciones[pos - 1], temp);
            break;
          case 4:
            numm.toCharArray(numero4.numero, LONGITUD_NUMEROS);
            EEPROM.put(direcciones[pos - 1], temp);
            break;
          case 5:
            numm.toCharArray(numero5.numero, LONGITUD_NUMEROS);
            EEPROM.put(direcciones[pos - 1], temp);
            break;
          case 6:
            numm.toCharArray(numero6.numero, LONGITUD_NUMEROS);
            EEPROM.put(direcciones[pos - 1], temp);
            break;
          case 7:
            numm.toCharArray(numero7.numero, LONGITUD_NUMEROS);
            EEPROM.put(direcciones[pos - 1], temp);
            break;
          default:
            err = false;
            break;
        }
        if (err) {
          Sim.sendSMS(sms_num, &String(F("Numero guardado exitosamente"))[0u]);
          leerEEPROM();
          return true;
        }
        else {
          Sim.sendSMS(sms_num, "Posision no existente");
          return false;
        }
      }
      else {
        Sim.sendSMS(sms_num, "Error en formato de numero");  // ERROR: no contiene "+504" o "@"
        return false;
      }
    }
    // COMANDO DE BORRAR
    else if (com == BORRAR) {
      String numm = NULO;
      numm.toCharArray(temp.numero, LONGITUD_NUMEROS);
      if (poss == TODOS) {
        for (int i = 0; i < 7; i++)
        {
          EEPROM.put(direcciones[i], temp);
        }
        leerEEPROM();
        Sim.sendSMS(sms_num, "Memoria borrada completamente");
        return true;
      }
      int pos = poss.toInt();
      String borradoOK = "";
      if (0 < (pos < 8)) {
        EEPROM.put(direcciones[pos - 1], temp);
        switch (pos)
        {
          case 1:
            borradoOK = borradoOK + numero1.numero;
            break;
          case 2:
            borradoOK = borradoOK + numero2.numero;
            break;
          case 3:
            borradoOK = borradoOK + numero3.numero;
            break;
          case 4:
            borradoOK = borradoOK + numero4.numero;
            break;
          case 5:
            borradoOK = borradoOK + numero5.numero;
            break;
          case 6:
            borradoOK = borradoOK + numero6.numero;
            break;
          case 7:
            borradoOK = borradoOK + numero7.numero;
            break;
        }
        leerEEPROM();
        if ((borradoOK.indexOf(NULO) != -1) || (borradoOK.indexOf("+504") == -1)) {
          borradoOK = "La posicion " + String(pos) + ", es nula.\nEs innecesario borrar.";
          return true;
        }
        else {
          borradoOK = borradoOK + ", de la posicion: " + pos + ", fue establecido a NULO";
        }
        Sim.sendSMS(sms_num, &borradoOK[0u]);
        return true;
      }
      else {
        Sim.sendSMS(sms_num, "Posision Erronea");
        return false;
      }
    }
    //COMANDO DE SOLICITUD DE MEMORIA
    else if (com == String(ASK_FOR_NUMEROS)) {
      String memoria = "MEMORIA:\n" +
                       String(numero1.numero) + "\n" +
                       String(numero2.numero) + "\n" +
                       String(numero3.numero) + "\n" +
                       String(numero4.numero) + "\n" +
                       String(numero5.numero) + "\n" +
                       String(numero6.numero) + "\n" +
                       String(numero7.numero);
     // Serial.println(&memoria[0u]);
      Sim.sendSMS(sms_num, &memoria[0u]);
      return true;
    }
    //CAMBIO DE SITIO
    else if (com == String(CAMBIAR_SITIO)) {
      //.1234 CAMBIARSITIO sitio
      mensaje_codificado.substring(index2 + 1).toCharArray(sitio.NombreSitio, LON_SITIO);
      cambiarSITIO();
      Sim.sendSMS(sms_num, "Sitio cambiado correctamente");
      return true;
    }
    //CAMBIO DE ID SITIO
    else if (com == String(CAMBIAR_IDSITIO)) {
      //.1234 CAMBIARSITIO sitio
      mensaje_codificado.substring(index2 + 1).toCharArray(sitio.IDSitio, LON_ID_SITIO);
      cambiarSITIO();
      Sim.sendSMS(sms_num, "ID del sitio cambiado correctamente");
      return true;
    }
    //PIDE EL SITIO Y EL ID
    else if (com == String(ASK_SITIO)) {
      String memoria = "ID:\n" +
                       String(sitio.IDSitio) + "\n" +
                       "Sitio:\n" +
                       String(sitio.NombreSitio);
      Sim.sendSMS(sms_num, &memoria[0u]);
      return true;
    }
    //CAMBIO DE PASSWORD
    else if (com == String(CONTRA)) {
      //.1234 passw NEWPASS
      String NEWPASS = mensaje_codificado.substring(index2 + 1);
      if ((NEWPASS.length() > 3) && (NEWPASS.length() < 11))
      {
        NEWPASS.toCharArray(CONTRA_TEMP, LON_CONTRA);
        //cambiarContra();
        Sim.sendSMS(sms_num, "Esta seguro de cambiar la clave?");
        return true;
        //Serial.println(CONTRA_TEMP);
        //Serial.println(contra.contra);
      }
      else Sim.sendSMS(sms_num, "Error en la longitud de nueva clave.");
      return false;
    }
    else {
      //Serial.println("Comando erroneo");
      Sim.sendSMS(sms_num, "Comando erroneo");
      return false;
    }
  }
  else if (cont == String(CONTRA_TEMP)) {
    //NEWPASS OLDPASS
    if (com == String(contra.contra)) {
      cont.toCharArray(contra.contra, LON_CONTRA);
      cambiarContra();
      Sim.sendSMS(sms_num, "Cambio de palabra clave exitoso");
      return true;
    }
    else {
      Sim.sendSMS(sms_num, "Error en palabras clave.");
      String("cacahuate").toCharArray(CONTRA_TEMP, LON_CONTRA);
      return false;
    }
  }
  else {
    Sim.sendSMS(sms_num, "Mensaje erroneo");
    return false;
  }
}

