#include <iostream>  // esta libreria se encarga de las funcionalidades de entrada y salida estándar en C++
#include <vector>    // esta libreria se encarga de la clase vector para contenedores dinámicos
#include <stack>     // esta libreria se encarga de la clase stack para la estructura de pila
#include <algorithm> // esta libreria se encarga de los algoritmos estándar como random_shuffle
#include <chrono>    // con esta libreria se manipula y mide el tiempo
#include <thread>    // esta libreria se encarga de los hilos
#include <ctime>     // esta libreria se encarga de proporcionar funciones para obtener y manipular el tiempo del sistema
#ifdef _WIN32
#include <windows.h> // esta libreria se usa para tener acceso a funciones específicas de Windows como system("cls") para limpiar la consola
#else
#include <unistd.h>  // esta libreria se usa para tener acceso a funciones específicas de UNIX como system("clear") para limpiar la consola.
#endif

using namespace std; // esto permite usar los "nombres estandares" de c++ sin el prefijo de "std::"

// funcion para limpiar la pantalla de la consola
void limpiarPantalla() {
#ifdef _WIN32
    system("cls");  // comando para limpiar la consola en Windows
#else
    system("clear"); // comando para limpiar la consola en sistemas como linux
#endif
}

// estructura que representa una celda del laberinto
struct Celda {
    int x, y; // coordenadas x,y de la celda
};

// funcion para imprimir el laberinto en la consola
void imprimirLaberinto(const vector<vector<int>>& laberinto) {
    limpiarPantalla();  // limpia la pantalla antes de imprimir el laberinto
    for (const auto& fila : laberinto) {  // recorre cada fila del laberinto
        for (int celda : fila) {  // recorre cada celda de la fila
            switch (celda) {
                case 1: cout << "#"; break;  // muro
                case 2: cout << "P"; break;  // recorrido
                case 3: cout << "S"; break;  // salida
                default: cout << "*"; break; // camino libre
            }
        }
        cout << endl;  // inserta una nueva linea al final de cada fila
    }
}

// funcion para recorrer el laberinto con backtracking
bool resolverLaberinto(vector<vector<int>>& laberinto, int x, int y) {
    if (laberinto[y][x] == 3) {  // si la celda actual es la salida
        laberinto[y][x] = 2;     // marca como parte de la solución
        return true;  // retorna True si se encuentra una solucion
    }

    // verifica si la posición es valida y si es un camino libre o la salida
    if (x >= 0 && x < laberinto[0].size() && y >= 0 && y < laberinto.size() && (laberinto[y][x] == 0 || laberinto[y][x] == 3)) {
        int temp = laberinto[y][x];  // guarda el estado actual de la celda
        laberinto[y][x] = 2;         // marca la celda como parte del camino explorado

        imprimirLaberinto(laberinto);  // imprime el estado actual del laberinto
        this_thread::sleep_for(chrono::milliseconds(100));  // pausa para poder visualizar

        // define posibles direcciones de movimiento
        vector<pair<int, int>> direcciones = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
        random_shuffle(direcciones.begin(), direcciones.end());  // mezcla direcciones para mezclar al azar

        // intenta buscar un camino en cada direccion
        for (auto dir : direcciones) {
            if (resolverLaberinto(laberinto, x + dir.first, y + dir.second))
                return true;  // retorna true si encuentra una solucion
        }

        // si no se encuentra una solucion se restaura la celda
        laberinto[y][x] = temp;

        imprimirLaberinto(laberinto);  // imprime el laberinto sin el camino fallido
        this_thread::sleep_for(chrono::milliseconds(100));  // pausa para poder visualizar

        return false;  // retorna false si no se encuentra una solucion en esa direccion
    }

    return false;  // retorna false si la posicion no es valida
}

// funcion para generar un laberinto usando retroceso (backtracking)
void generarLaberinto(int filas, int columnas) {
    vector<vector<int>> laberinto(filas, vector<int>(columnas, 1));  // Inicializar el laberinto lleno de muros

    srand(static_cast<unsigned int>(time(nullptr)));  // seed para la generación de numeros aleatorios
    stack<Celda> pila;
    int startX = 1, startY = 1;
    laberinto[startY][startX] = 0;  // marca el punto inicial como camino
    pila.push({startX, startY});

    // define posibles movimientos en el laberinto
    vector<pair<int, int>> direcciones = {{2, 0}, {0, 2}, {-2, 0}, {0, -2}};

    while (!pila.empty()) {
        Celda actual = pila.top();
        vector<Celda> vecinos;

        // busca celdas vecinas no visitadas
        for (auto dir : direcciones) {
            int nx = actual.x + dir.first;
            int ny = actual.y + dir.second;
            if (nx > 0 && nx < columnas - 1 && ny > 0 && ny < filas - 1 && laberinto[ny][nx] == 1) {
                vecinos.push_back({nx, ny});
            }
        }

        // si encuentra vecinos o neighbors no visitados, elige uno al azar y lo explora
        if (!vecinos.empty()) {
            Celda siguiente = vecinos[rand() % vecinos.size()];
            laberinto[siguiente.y][siguiente.x] = 0;  // Marcar como camino
            laberinto[(actual.y + siguiente.y) / 2][(actual.x + siguiente.x) / 2] = 0;  // Derribar el muro entre las celdas
            pila.push(siguiente);
        } else {
            pila.pop();  // retrocede si no hay vecinos no visitados
        }
    }

    // define las esquinas como posibles entradas o salidas
    vector<Celda> esquinas = {{1, 1}, {1, columnas - 2}, {filas - 2, 1}, {filas - 2, columnas - 2}};
    Celda entrada = esquinas[rand() % 4];  // elige una entrada
    Celda salida;
    do {
        salida = esquinas[rand() % 4];
    } while (salida.x == entrada.x && salida.y == entrada.y);  // se asegura que la entrada y salida no sean la misma

    laberinto[entrada.y][entrada.x] = 0;  // marca la entrada
    laberinto[salida.y][salida.x] = 3;    // marca la salida

    imprimirLaberinto(laberinto);  // imprime el laberinto generado
    this_thread::sleep_for(chrono::milliseconds(100));  // pausa para poder visualizar

    // intenta resolver el laberinto desde la entrada
    if (resolverLaberinto(laberinto, entrada.x, entrada.y)) {
        cout << "¡Felicidades! Se ha encontrado una solución." << endl;
    } else {
        cout << "No se ha encontrado una solución." << endl;
    }
}

int main() {
    int filas, columnas;  // variables para el tamaño del laberinto
    char reiniciar;  // variable para el reinicio del juego

    do {
        do {
            cout << "Ingrese el número de filas del laberinto (mínimo 10): ";
            cin >> filas;  // lee el número de filas del usuario
            cout << "Ingrese el número de columnas del laberinto (mínimo 10): ";
            cin >> columnas;  // lee el número de columnas del usuario

            // verifica que el tamaño
            if (filas < 10 || columnas < 10) {
                cout << "El tamaño mínimo del laberinto es 10x10. Por favor, intente nuevamente." << endl;
            }
        } while (filas < 10 || columnas < 10);

        // genera y resuelve el laberinto
        generarLaberinto(filas, columnas);

        // pregunta al usuario si desea reiniciar el juego
        cout << "¿Quieres reiniciar el juego? (s/n): ";
        cin >> reiniciar;

    } while (reiniciar == 's' || reiniciar == 'S');  // reinicia si el usuario lo desea

#ifdef _WIN32
    system("pause");  // pausa en windows para que la consola no se cierre automáticamente
#else
    cout << "Presiona Enter para continuar...";  // mensaje para sistemas que no sean windows
    cin.ignore();
    cin.get();
#endif
    return 0;  // retorna 0 para indicar que el programa finaliza correctamente
}