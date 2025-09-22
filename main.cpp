#include <iostream>
#include <fstream>

using namespace std;

unsigned char rotarDerecha(unsigned char byte, int posiciones);

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
