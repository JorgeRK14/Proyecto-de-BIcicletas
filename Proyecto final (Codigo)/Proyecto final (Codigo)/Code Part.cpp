#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <map>

using namespace std;

// Función para limpiar la consola
void limpiarConsola() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
// Función para mostrar la guía de uso (instrucciones del programa)
void mostrarGuia() {
    cout << "=====================================\n";
    cout << "   BIENVENIDO AL SISTEMA DE MONITOREO DE BICICLETAS\n";
    cout << "=====================================\n";
    cout << "\nEste programa permite visualizar y analizar la distribucion de bicicletas\n";
    cout << "en diferentes estaciones durante un periodo de 30 dias.\n";
    cout << "\nA continuacion se explica cada opcion del menu principal:\n";
    cout << "1. Ver el registro de bicicletas por estacion y dia: Te permite ver en que\n";
    cout << "   estacion estuvo cada bicicleta durante el periodo de 30 dias.\n";
    cout << "2. Ver la distribucion de bicicletas el Dia 1: Muestra cuantas bicicletas\n";
    cout << "   hubo en cada estacion en el primer dia.\n";
    cout << "3. Ver las probabilidades de transicion: Muestra con que frecuencia\n";
    cout << "   las bicicletas se mueven entre estaciones.\n";
    cout << "4. Ver la distribucion de bicicletas en un dia especifico: Te permite ingresar\n";
    cout << "   un dia y ver cuantas bicicletas hubo en cada estacion ese dia.\n";
    cout << "5. Ver la distribucion a largo plazo: Muestra como se distribuyen las\n";
    cout << "   bicicletas despues de muchos dias.\n";
    cout << "6. Salir del sistema: Finaliza el programa.\n";
    cout << "\n=====================================\n";
    cout << "Por favor, siga las instrucciones del menu para utilizar el programa.\n";
    cout << "=====================================\n";
}

// Generar datos aleatorios para 30 días
vector<vector<int>> generarDatosAleatorios(int numBicicletas, int numEstaciones, int numDias) {
    vector<vector<int>> datos(numBicicletas, vector<int>(numDias));
    srand(time(0));  // Inicializa la semilla para la generación aleatoria

    for (int i = 0; i < numBicicletas; ++i) {
        for (int j = 0; j < numDias; ++j) {
            datos[i][j] = rand() % numEstaciones + 1;
        }
    }

    return datos;
}

string intToString(int num) {
    stringstream ss;
    ss << num;
    return ss.str();
}

// Función para imprimir los datos generados con un formato ordenado
void imprimirDatos(const vector<vector<int>>& datos) {
    // Encabezado de los días
    cout << setw(12) << "Bicicleta/Dia";
    for (size_t j = 0; j < datos[0].size(); ++j) {
        cout << setw(5) << j + 1;
    }
    cout << endl;

    // Datos de cada bicicleta
    for (size_t i = 0; i < datos.size(); ++i) {
        cout << setw(12) << "Bicicleta " + intToString(i + 1);
        for (size_t j = 0; j < datos[i].size(); ++j) {
            cout << setw(5) << "E" + intToString(datos[i][j]);
        }
        cout << endl;
    }
}

// Función para generar el vector inicial basado en los datos del primer día
vector<double> generarVectorInicial(const vector<vector<int>>& datos, int numEstaciones) {
    vector<double> vectorInicial(numEstaciones, 0.0);
    int numBicicletas = datos.size();

    // Contar cuántas bicicletas están en cada estación en el primer día
    for (int i = 0; i < numBicicletas; ++i) {
        int estacion = datos[i][0] - 1;
        vectorInicial[estacion] += 1.0;
    }

    // Normalizar el vector inicial dividiendo por el total de bicicletas
    for (int i = 0; i < numEstaciones; ++i) {
        vectorInicial[i] /= numBicicletas;
    }

    return vectorInicial;
}

// Función para imprimir el vector inicial
void imprimirVectorInicial(const vector<double>& vectorInicial, int precision) {
    cout << "\nVector Inicial (Distribucion de bicicletas el Dia 1):\n";
    for (size_t i = 0; i < vectorInicial.size(); ++i) {
        cout << "Estacion E" << (i + 1) << ": " << fixed << setprecision(precision) << vectorInicial[i] << endl;
    }
}

// Construir la matriz de transición (normalizando columnas)
vector<vector<double>> construirMatrizTransicion(const vector<vector<int>>& datos, int numEstaciones, int precision) {
    vector<vector<double>> transiciones(numEstaciones, vector<double>(numEstaciones, 0.0));

    for (const auto& bicicleta : datos) {
        for (size_t dia = 0; dia < bicicleta.size() - 1; ++dia) {
            int origen = bicicleta[dia] - 1;
            int destino = bicicleta[dia + 1] - 1;
            transiciones[origen][destino] += 1;
        }
    }

    for (int i = 0; i < numEstaciones; ++i) {
        double total = 0;
        for (int j = 0; j < numEstaciones; ++j) {
            total += transiciones[i][j];
        }
        if (total > 0) {
            for (int j = 0; j < numEstaciones; ++j) {
                transiciones[i][j] /= total;
            }
        }
    }

    return transiciones;
}

// Función para elevar la matriz a la potencia n
vector<vector<double>> elevarMatriz(const vector<vector<double>>& matriz, int n) {
    int size = matriz.size();
    vector<vector<double>> resultado = matriz;

    // Multiplicamos la matriz por si misma n veces
    for (int exp = 1; exp < n; ++exp) {
        vector<vector<double>> temp(size, vector<double>(size, 0.0));

        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                for (int k = 0; k < size; ++k) {
                    temp[i][j] += resultado[i][k] * matriz[k][j];
                }
            }
        }
        resultado = temp;
    }

    return resultado;
}

// Multiplicar matriz por vector
vector<double> multiplicarMatrizVector(const vector<vector<double>>& matriz, const vector<double>& vec) {
    int size = matriz.size();
    vector<double> resultado(size, 0.0);

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            resultado[i] += matriz[i][j] * vec[j];
        }
    }

    return resultado;
}

// Función para contar los cambios de estación fila por fila
map<string, int> contarCambiosEstacion(const vector<vector<int>>& datos, int numEstaciones) {
    map<string, int> cambiosEstacion;

    for (size_t i = 0; i < datos.size(); ++i) {
        for (size_t j = 0; j < datos[i].size() - 1; ++j) {
            int origen = datos[i][j];
            int destino = datos[i][j + 1];
            string cambio = "E" + intToString(origen) + "->E" + intToString(destino);
            cambiosEstacion[cambio]++;
        }
    }

    return cambiosEstacion;
}

// Función para mostrar la tabla de transición con las probabilidades
void mostrarMatrizTransicionConProbabilidades(const vector<vector<double>>& matriz, int precision) {
    cout << "\nMatriz de Transicion con Probabilidades:\n";
    cout << setw(10) << "Desde/ Hacia";
    for (size_t j = 0; j < matriz.size(); ++j) {
        cout << setw(8) << "E" + intToString(j + 1);
    }
    cout << endl;

    for (size_t i = 0; i < matriz.size(); ++i) {
        cout << setw(10) << "E" + intToString(i + 1);
        for (size_t j = 0; j < matriz[i].size(); ++j) {
            cout << setw(8) << fixed << setprecision(precision) << matriz[i][j] << " ";
        }
        cout << endl;
    }
}

// Función para mostrar los cambios de estación ordenados en filas de 12
void mostrarCambiosEstacionOrdenados(const map<string, int>& cambiosEstacion, int numEstaciones) {
    cout << "\nCambios de estacion ordenados:\n";

    for (int origen = 1; origen <= numEstaciones; ++origen) {
        int sumaFila = 0;  // Suma de las transiciones por fila (origen)
        for (int destino = 1; destino <= numEstaciones; ++destino) {
            string cambio = "E" + intToString(origen) + "->E" + intToString(destino);
            int valor = cambiosEstacion.at(cambio);
            sumaFila += valor;
            cout << cambio << "=" << setw(4) << valor << "   ";
        }
        // Mostrar la suma de las variaciones por fila
        cout << "| Suma: " << sumaFila << endl;
    }
}

// Función para calcular la probabilidad de que una bicicleta esté en la estación Y en el día n
void calcularProbabilidadEstacion(const vector<vector<double>>& matrizTransicion, const vector<double>& vectorInicial, int n, int precision) {
    vector<vector<double>> matrizElevada = elevarMatriz(matrizTransicion, n);

    // Imprimir la matriz elevada
    cout << "\nMatriz de Transicion elevada a la potencia " << n << ":\n";
    mostrarMatrizTransicionConProbabilidades(matrizElevada, precision);

    // Multiplicación de la matriz elevada por el vector inicial
    vector<double> resultado = multiplicarMatrizVector(matrizElevada, vectorInicial);

    cout << "\nProbabilidad de que una bicicleta este en cada estacion el dia " << n << ":\n";
    for (int i = 0; i < resultado.size(); ++i) {
        cout << "Estacion E" << (i + 1) << ": " << fixed << setprecision(precision) << resultado[i] << endl;
    }
}

// Función para calcular el vector estacionario
vector<double> calcularVectorEstacionario(const vector<vector<double>>& matrizTransicion) {
    int size = matrizTransicion.size();
    vector<vector<double>> matrizElevada = matrizTransicion;
    const int iteraciones = 1000;  // Número de iteraciones para aproximar el equilibrio
    int potenciaElevada = 0;  // Variable para almacenar la potencia a la que se eleva la matriz

    // Elevar la matriz a una potencia suficientemente alta para aproximar el vector estacionario
    for (int exp = 1; exp <= iteraciones; ++exp) {
        vector<vector<double>> temp(size, vector<double>(size, 0.0));

        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                for (int k = 0; k < size; ++k) {
                    temp[i][j] += matrizElevada[i][k] * matrizTransicion[k][j];
                }
            }
        }
        matrizElevada = temp;

        // Imprimir la matriz elevada y la potencia en la última iteración
        if (exp == iteraciones) {
            potenciaElevada = exp;
            cout << "\nMatriz elevada a la potencia " << potenciaElevada << ":\n";
            for (const auto& fila : matrizElevada) {
                for (double valor : fila) {
                    cout << fixed << setprecision(4) << valor << "\t";
                }
                cout << endl;
            }
        }
    }

    // Tomamos cualquier fila de la matriz elevada como aproximación del vector estacionario
    vector<double> vectorEstacionario = matrizElevada[0];
    return vectorEstacionario;
}

// Función para imprimir el vector estacionario
void imprimirVectorEstacionario(const vector<double>& vectorEstacionario, int precision) {
    cout << "\nVector Estacionario (Distribucion estable de bicicletas):\n";
    for (size_t i = 0; i < vectorEstacionario.size(); ++i) {
        cout << "Estacion E" << (i + 1) << ": " << fixed << setprecision(precision) << vectorEstacionario[i] << endl;
    }
}

// Menú del programa con explicaciones más amigables
void menuPrincipal(vector<vector<int>>& datos, int numEstaciones, int precision) {
    int opcion;
    vector<vector<double>> matrizTransicion;  // Mover la declaración fuera del switch
    vector<double> vectorEstacionario;
    do {
        limpiarConsola();
        cout << "=====================================\n";
        cout << "   SISTEMA DE MONITOREO DE BICICLETAS\n";
        cout << "=====================================\n";
        cout << "\nPor favor, seleccione una opcion del menu para continuar:\n";
        cout << "1. Ver datos de las bicicletas durante los 30 dias\n";
        cout << "2. Ver distribucion de bicicletas el primer dia (Vector Inicial)\n";
        cout << "3. Ver matriz de transicion con probabilidades\n";
        cout << "4. Calcular la probabilidad de que una bicicleta esté en una estacion el dia n\n";
        cout << "5. Mostrar tabla de transiciones ordenadas por fila\n";
        cout << "6. Calcular la distribución de las bicicletas a largo plazo (Vector estacionario)\n";  // Nueva opción agregada
        cout << "7. Salir\n";
        cout << "Elige una opcion: ";
        cin >> opcion;

        switch (opcion) {
        case 1:
            limpiarConsola();
            imprimirDatos(datos);
            break;

        case 2: {
            limpiarConsola();
            vector<double> vectorInicial = generarVectorInicial(datos, numEstaciones);
            imprimirVectorInicial(vectorInicial, precision);
            break;
        }

        case 3: {
            limpiarConsola();
            vector<vector<double>> matrizTransicion = construirMatrizTransicion(datos, numEstaciones, precision);
            mostrarMatrizTransicionConProbabilidades(matrizTransicion, precision);
            break;
        }

        case 4: {
            limpiarConsola();
            int n;
            cout << "Ingrese el dia (n) para calcular las probabilidades: ";
            cin >> n;

            vector<vector<double>> matrizTransicion = construirMatrizTransicion(datos, numEstaciones, precision);
            vector<double> vectorInicial = generarVectorInicial(datos, numEstaciones);
            calcularProbabilidadEstacion(matrizTransicion, vectorInicial, n, precision);
            break;
        }

        case 5: {
            limpiarConsola();
            map<string, int> cambiosEstacion = contarCambiosEstacion(datos, numEstaciones);
            mostrarCambiosEstacionOrdenados(cambiosEstacion, numEstaciones);
            break;
        }

        case 6: {  // Nueva opción para calcular el vector estacionario
            limpiarConsola();
            vector<vector<double>> matrizTransicion = construirMatrizTransicion(datos, numEstaciones, precision);
            vector<double> vectorEstacionario = calcularVectorEstacionario(matrizTransicion);
            imprimirVectorEstacionario(vectorEstacionario, precision);
            break;
        }

        case 7:
            cout << "Saliendo del programa...\n";
            break;

        default:
            cout << "Opcion no valida. Intente nuevamente.\n";
            break;
        }

        if (opcion != 7) {
            cout << "\nPresione Enter para continuar...";
            cin.ignore();
            cin.get();
        }
    } while (opcion != 7);

}
int main() {
    int numEstaciones = 12;
    int numBicicletas = 140;
    int numDias = 30;
    int precision = 5;
    int opcion;
    vector<vector<int>> datos = generarDatosAleatorios(numBicicletas, numEstaciones, numDias);

    int opcionInicial;
    do {
        limpiarConsola();
        cout << "=====================================\n";
        cout << "   MENU PRINCIPAL\n";
        cout << "=====================================\n";
        cout << "1. Ver instrucciones del programa\n";
        cout << "2. Entrar al programa\n";
        cout << "Seleccione una opcion: ";
        cin >> opcionInicial;

        switch (opcionInicial) {
        case 1:
            limpiarConsola();
            mostrarGuia();
            cout << "\nPresione Enter para volver al menu principal...";
            cin.ignore();
            cin.get();
            break;
        case 2:
            limpiarConsola();
            menuPrincipal(datos, numEstaciones, precision);
            return 0;
        default:
            limpiarConsola();
            cout << "Opcion no valida. Por favor, intente nuevamente.\n";
        }
    } while (opcionInicial != 2);

    return 0;
}