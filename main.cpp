/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */
/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"


// Blinking rate in milliseconds
#define BLINKING_RATE     500ms
#define TIEMPO_LECTURA    1000ms

#define add_aht10  0x38

BufferedSerial pc(USBTX, USBRX);
I2C Aht10 (D14,D15); // pines I2C

// hilos 
Thread hilo_leer;

void leer(void);
void configurar_arranque(void);
void comenzar(void);
void resetear(void);



static int humedad_int     = 0;
static int temperatura_int = 0; 

static float humedad     = 0.0;
static float temperatura = 0.0;

const char calibrar[3]  = {0xE1, 0x08, 0x00};
const char normal[3]    = {0xA8, 0x00, 0x00};
const char medir[3]     = {0xAC, 0x33, 0x00};
const char reinicio[1]     = {0xBA};

char men[30];

int main()
{
    // Initialise the digital pin LED1 as an output
    DigitalOut led(LED1);
    
    configurar_arranque();
    ThisThread::sleep_for(500ms); // Espera para arrancar el sensor
    // Verificar si requiere arranque...
    comenzar();
    //
    hilo_leer.start(leer);
    while (true) {
        led = !led;
        ThisThread::sleep_for(BLINKING_RATE);
    }
}


void configurar_arranque (void)
{
    Aht10.write(add_aht10, calibrar , 3);
}   


void leer (void)
{
    static char temp[6];
    while(true)
    {
        Aht10.write(add_aht10, medir, 3);
        ThisThread::sleep_for(75ms); // esperar 75 ms         
        Aht10.read(add_aht10, temp, 6);    
        humedad_int  =    ((temp[1] << 16) | (temp[2] << 8) | temp[3]) >>  4;
        temperatura_int = ((temp[3] & 0x0F) << 16) | (temp[4] << 8) | temp[5];
        humedad     = (float(humedad_int)*100)/ 1048576;
        temperatura = ((float(temperatura_int)*200)/ 1048576)-50;
        // Falta imprimir los datos 

        //
        ThisThread::sleep_for(TIEMPO_LECTURA);
    }

}

void comenzar (void)
{
     Aht10.write(add_aht10, normal, 3);

}

void resetear(void)
{

    Aht10.write(add_aht10, reinicio, 1);
    ThisThread::sleep_for(20ms);
}