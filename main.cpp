#include <iostream>
#include <fstream>

using namespace std;

unsigned char rotarDerecha(unsigned char byte, int posiciones);
char* desencriptar(char* mensajeEncriptado, int longitud, int rotacion, unsigned char claveXor);
char* descomprimirRLE(const char* mensajeComprimido, int longitud, int& longitudSalida);
char* descomprimirLZ78(const unsigned char* mensajeComprimido, int longitud, int& longitudSalida);
bool contieneFragmento(const char* texto, int longitudTexto, const char* fragmento, int longitudFragmento);
char* leerArchivo(const char* nombreArchivo, int &longitud);
bool encontrarParametrosYMetodo(char* encriptado, int longitudEncriptada, const char* pista, int longitudPista,
                                int& rotacionEncontrada, unsigned char& claveEncontrada, int &metodoEncontrado);

int main() {
    const char* archivoEncriptado = "Encriptado4.txt";
    const char* archivoPista = "pista4.txt";

    int longitudEncriptada = 0;
    char* encriptado = leerArchivo(archivoEncriptado, longitudEncriptada);

    int longitudPista = 0;
    char* pista = leerArchivo(archivoPista, longitudPista);

    if (encriptado == nullptr || pista == nullptr) {
        if (encriptado) delete[] encriptado;
        if (pista) delete[] pista;
        cin.get();
        return 1;
    }

    int rotacionEncontrada;
    unsigned char claveEncontrada;
    int metodoEncontrado = 0;

    if (encontrarParametrosYMetodo(encriptado, longitudEncriptada, pista, longitudPista,
                                   rotacionEncontrada, claveEncontrada, metodoEncontrado)) {

        cout << "¡Parametros encontrados!" << endl;
        cout << "Metodo de compresion: " << metodoEncontrado << endl;
        cout << "Rotacion (n): " << rotacionEncontrada << endl;
        cout << "Clave XOR (K): " << (int)claveEncontrada << endl;

        char* desencriptado = desencriptar(encriptado, longitudEncriptada, rotacionEncontrada, claveEncontrada);
        int longitudOriginal;
        char* textoOriginal;

        if (metodoEncontrado == 1) {
            textoOriginal = descomprimirRLE(desencriptado, longitudEncriptada, longitudOriginal);
        } else {
            textoOriginal = descomprimirLZ78((const unsigned char*)desencriptado, longitudEncriptada, longitudOriginal);
        }

        cout << "\n--- Mensaje Original Reconstruido ---" << endl;
        cout << textoOriginal << endl;

        delete[] desencriptado;
        delete[] textoOriginal;

    } else {
        cout << "No se pudo encontrar una combinacion de parametros valida." << endl;
    }

    delete[] encriptado;
    delete[] pista;
    cin.get();

    return 0;
}

//Función para rotar los bits a la derecha
unsigned char rotarDerecha(unsigned char byte, int posiciones) {
    return (byte >> posiciones) | (byte << (8 - posiciones));
}

//Función para desencriptar
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
    const int capacidadDiccionario = 65536;
    char** diccionario = new char*[capacidadDiccionario];
    int* longitudes = new int[capacidadDiccionario];
    int entradas = 0;

    char* salida = new char[longitud * 4];
    int posSalida = 0;
    int i = 0;

    while (i + 2 < longitud) {
        int indice = (mensajeComprimido[i] << 8) | mensajeComprimido[i + 1];
        unsigned char c = mensajeComprimido[i + 2];
        i += 3;

        int lenPrefijo = 0;
        char* prefijo = 0;

        if (indice == 0) {
            lenPrefijo = 0;
            prefijo = 0;
        } else if (indice > 0 && indice <= entradas) {
            prefijo = diccionario[indice - 1];
            lenPrefijo = longitudes[indice - 1];
        } else {
            for (int k = 0; k < entradas; k++) delete[] diccionario[k];
            delete[] diccionario;
            delete[] longitudes;
            delete[] salida;
            return 0;
        }

        char* nuevaCadena = new char[lenPrefijo + 2];
        for (int j = 0; j < lenPrefijo; j++)
            nuevaCadena[j] = prefijo[j];
        nuevaCadena[lenPrefijo] = c;
        nuevaCadena[lenPrefijo + 1] = '\0';

        for (int j = 0; j < lenPrefijo + 1; j++)
            salida[posSalida++] = nuevaCadena[j];

        if (entradas < capacidadDiccionario) {
            diccionario[entradas] = nuevaCadena;
            longitudes[entradas] = lenPrefijo + 1;
            entradas++;
        } else {
            delete[] nuevaCadena;
        }
    }

    salida[posSalida] = '\0';
    longitudSalida = posSalida;

    for (int k = 0; k < entradas; k++) delete[] diccionario[k];
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

//Funcion para leer el archivo
char* leerArchivo(const char* nombreArchivo, int &longitud) {
    ifstream archivo(nombreArchivo, ios::binary);
    if (!archivo.is_open()) {
        cout << "Error: No se pudo abrir el archivo." << endl;
        longitud = 0;
        return nullptr;
    }

    archivo.seekg(0, ios::end);
    longitud = archivo.tellg();
    archivo.seekg(0, ios::beg);

    char* texto = new char[longitud + 1];
    archivo.read(texto, longitud);
    texto[longitud] = '\0';

    archivo.close();
    return texto;
}

//Función para encontrar parámetros
bool encontrarParametrosYMetodo(char* encriptado, int longitudEncriptada, const char* pista, int longitudPista,
                                int& rotacionEncontrada, unsigned char& claveEncontrada, int &metodoEncontrado) {

    for (int rotacion = 1; rotacion < 8; rotacion++) {
        for (int clave = 0; clave < 256; clave++) {

            char* desencriptado = desencriptar(encriptado, longitudEncriptada, rotacion, (unsigned char)clave);

            int longitudDescomprimida = 0;
            char* textoOriginal = nullptr;
            bool encontrado = false;

            textoOriginal = descomprimirRLE(desencriptado, longitudEncriptada, longitudDescomprimida);
            if (textoOriginal && longitudDescomprimida > 0 &&
                contieneFragmento(textoOriginal, longitudDescomprimida, pista, longitudPista)) {
                rotacionEncontrada = rotacion;
                claveEncontrada = (unsigned char)clave;
                metodoEncontrado = 1;
                encontrado = true;
            }

            if (!encontrado) {
                if (textoOriginal) {
                    delete[] textoOriginal;
                    textoOriginal = nullptr;
                }

                textoOriginal = descomprimirLZ78((const unsigned char*)desencriptado, longitudEncriptada, longitudDescomprimida);
                if (textoOriginal && longitudDescomprimida > 0 &&
                    contieneFragmento(textoOriginal, longitudDescomprimida, pista, longitudPista)) {
                    rotacionEncontrada = rotacion;
                    claveEncontrada = (unsigned char)clave;
                    metodoEncontrado = 2;
                    encontrado = true;
                }
            }

            if (textoOriginal) {
                delete[] textoOriginal;
            }

            delete[] desencriptado;

            if (encontrado) {
                return true;
            }
        }
    }
    return false;
}
