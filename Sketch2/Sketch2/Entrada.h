// Entrada.h

#ifndef _ENTRADA_h
#define _ENTRADA_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class EntradaClass
{
 protected:
	 int pinEntrada;
	 int estadoAnterior;

 public:
	void init(int pin, bool estadoASetear);
	bool entradaCambio();
	int getPin();
};

extern EntradaClass Entrada;

#endif


