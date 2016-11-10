// 
// 
// 

#include "Entrada.h"

void EntradaClass::init(int pin, bool estadoASetear){
	pinEntrada = pin;
	estadoAnterior = (estadoASetear == true) ? HIGH : LOW;
}

bool EntradaClass::entradaCambio(){
	bool result;
	int estadoActual = digitalRead(pinEntrada);
 
	result = ((estadoActual == estadoAnterior) ? true : false );
	estadoAnterior = estadoActual;
  	
	return !result;
}

int EntradaClass::getPin(){
	return pinEntrada;
}

EntradaClass Entrada;

