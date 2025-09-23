#include <iostream>
#include <fstream>

using namespace std;

unsigned char rotarDerecha(unsigned char byte, int posiciones);
char* desencriptar(char* mensajeEncriptado, int longitud, int rotacion, unsigned char claveXor);
char* descomprimirRLE(const char* mensajeComprimido, int longitud, int& longitudSalida);

int main() {


    return 0;
}

//Función para rotar los bits a la derecha
unsigned char rotarDerecha(unsigned char byte, int posiciones) {
    return (byte >> posiciones) | (byte << (8 - posiciones));
}

// Función para desencriptar el mensaje
char* desencriptar(char* mensajeEncriptado, int longitud, int rotacion, unsigned char claveXor) {
    char* desencriptado = new char[longitud + 1];

    for (int i = 0; i < longitud; i++) {
        unsigned char temp = (unsigned char)mensajeEncriptado[i] ^ claveXor;
        desencriptado[i] = (char)rotarDerecha(temp, rotacion);
    }

    desencriptado[longitud] = '\0';
    return desencriptado;
}

//Función para descomprimir por RLE
char* descomprimirRLE(const char* mensajeComprimido, int longitud, int& longitudSalida) {
    longitudSalida = 0;
    for (int i = 0; i < longitud;) {
        int numero = 0;

        while (i < longitud && mensajeComprimido[i] >= '0' && mensajeComprimido[i] <= '9') {
            numero = numero * 10 + (mensajeComprimido[i] - '0');
            i++;
        }

        if (i < longitud) {
            longitudSalida += numero;
            i++;
        }
    }

    char *descomprimido = new char[longitudSalida + 1];

    int posicion = 0;
    for (int i = 0; i < longitud;) {
        int numero1 = 0;

        while (i < longitud && mensajeComprimido[i] >= '0' && mensajeComprimido[i] <= '9') {
            numero1 = numero1 * 10 + (mensajeComprimido[i] - '0');
            i++;
        }
        if (i < longitud) {
            char simbolo = mensajeComprimido[i];
            for (int j = 0; j < numero1; j++) {
                descomprimido[posicion++] = simbolo;
            }
            i++;
        }
    }

    descomprimido[longitudSalida] = '\0';

    return descomprimido;
}






