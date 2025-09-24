#include <iostream>
#include <fstream>

using namespace std;

unsigned char rotarDerecha(unsigned char byte, int posiciones);
char* desencriptar(char* mensajeEncriptado, int longitud, int rotacion, unsigned char claveXor);
char* descomprimirRLE(const char* mensajeComprimido, int longitud, int& longitudSalida);
char* descomprimirLZ78(const unsigned char* mensajeComprimido, int longitud, int& longitudSalida);
bool contieneFragmento(const char* texto, int longitudTexto, const char* fragmento, int longitudFragmento);

int main() {

    const char* texto = "ABAABABA";
    int longitudTexto = 8;

    const char* fragmento = "AABA";
    int longitudFragmento = 4;

    if (contieneFragmento(texto, longitudTexto, fragmento, longitudFragmento)) {
        cout << "El fragmento SI se encuentra en el texto." << endl;
    } else {
        cout << "El fragmento NO se encuentra en el texto." << endl;
    }

    cin.get();

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

//Función para descomprimir por LZ78
char* descomprimirLZ78(const unsigned char* mensajeComprimido, int longitud, int& longitudSalida) {
    int capacidadDiccionario = 1000;
    char** diccionario = new char*[capacidadDiccionario];
    int* longitudes = new int[capacidadDiccionario];
    int entradas = 0;

    char* salida = new char[longitud * 10];
    int posSalida = 0;

    for (int i = 0; i < longitud; i += 3) {

        int indice = (mensajeComprimido[i] << 8) | mensajeComprimido[i + 1];

        unsigned char c = mensajeComprimido[i + 2];

        int lenPrefijo = 0;
        if (indice > 0 && indice <= entradas) {
            lenPrefijo = longitudes[indice - 1];
        }

        char* nuevo = new char[lenPrefijo + 1 + 1];

        if (indice > 0 && indice <= entradas) {
            for (int j = 0; j < lenPrefijo; j++) {
                nuevo[j] = diccionario[indice - 1][j];
            }
        }

        nuevo[lenPrefijo] = c;
        nuevo[lenPrefijo + 1] = '\0';

        diccionario[entradas] = nuevo;
        longitudes[entradas] = lenPrefijo + 1;
        entradas++;

        for (int j = 0; j < lenPrefijo + 1; j++) {
            salida[posSalida++] = nuevo[j];
        }
    }

    salida[posSalida] = '\0';
    longitudSalida = posSalida;

    for (int i = 0; i < entradas; i++) {
        delete[] diccionario[i];
    }
    delete[] diccionario;
    delete[] longitudes;

    return salida;
}

//Función para verificar si el texto contiene ese fragmento
bool contieneFragmento(const char* texto, int longitudTexto, const char* fragmento, int longitudFragmento) {
    if (longitudFragmento > longitudTexto) {
        return false;
    }

    for (int i = 0; i <= longitudTexto - longitudFragmento; i++) {
        bool coincide = true;

        for (int j = 0; j < longitudFragmento; j++) {
            if (texto[i + j] != fragmento[j]) {
                coincide = false;
                break;
            }
        }

        if (coincide) {
            return true;
        }
    }

    return false;
}
