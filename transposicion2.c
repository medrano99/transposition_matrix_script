/*
	Este programa realiza la inicializacion de una matriz
	empleando un hilo POSIX.
	Para compilar el programa, emplee el comando 
	
	$ gcc -pthread transposicion.c -o transposicion

	Para ejecutarlo y recolectar los datos, emplee el comando

	$ time ./transposicion

*/

/* NOTA: Para efectos de portabilidad, deliberadamente se han omitido las 
 * tildes en los comentarios 
 * 
 */

/* Libreria que contiene la funcion printf */
#include <stdio.h>

/* Libreria que contiene las funciones rand y srand */
#include <stdlib.h>

/* Libreria de Hilos POSIX */
#include <pthread.h>

/* Librerias para el manejo de tiempos */
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <math.h> //libreria para operaciones matematicas
 
/* Las dimensiones de las matrices se especifican como constantes */
#define FILAS 10000//32000 fddd
#define COLS 5000//16000
 
/* El numero de hilos con que se ejecutar la operacion. En este caso, se
usa un hilo de inicializacion */
#define HILOSINIT 3//1
#define HILOSTRANS 3//1 //# hilos para la matriz traspuesta

/* Las matrices. Como la matriz original es de orden (FILAS X COLS), la transpuesta
sera de orden (COLS x FILAS) */
float matOriginal[FILAS][COLS];
float matTranspuesta[COLS][FILAS];

/* Variable global que se usa para controlar el ciclo de ejecucion del programa
   Ver mas adelante la funcion init()
*/
int step_i = 0;
int step_j = 0;


/* Funcion que devuelve el tiempo actual del sistema medido en milisegundos
   transcurridos desde las 0 horas del 1 de enero de 1970, UTC.
   Para mas informacion, consulte la documentacion de la funcion gettimeofday().
   Esta funcion se usa para calcular el tiempo que toma la ejecucion de una operacion
   dentro del programa. La logica es calcular tiempoFinal - tiempoInicial
*/
static size_t get_milliseconds()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return time.tv_sec * 1000 + time.tv_usec / 1000;
}


/* init() es la operacion que sera ejecutada por el hilo de inicializacion */ 
void* init(void* arg)
{

    /* La variable core, que es local a cada hilo (esta definida dentro de 
       cada hilo se inicializa con una copia de la variable global step_i, y 
       permite dividir el trabajo entre el numero de hilos que se desee 
       asignar. Asi­, cada hilo realizara¡ 1/HILOSINIT del trabajo. En este caso, 
       cada hilo de inicializacion asignara valores a 1/HILOSINIT de las filas 
       de la matriz original.
    */
    int core = step_i; // A core se le asigna el valor de step_i
    step_i++;        // Luego esta se incrementa

    // TODO: Como se puede combinar lo anterior en una sola instruccion?
   
    // Variables locales que se usaran como iteradores en el for() 
    int i, j;

    // Variables que se usaran para inicializar la posicion de la matriz
    float x, y;

    /* La funcion srand() reinicializa la semilla del generador de numeros 
       aleatorios por cada vez que el programa se ejecute. Si no se invoca, la
       matriz tendra siempre los mismos valores numericos. Para mas 
       informacion, revise la documentacion de la funcion srand() 
       Si desea verificar que la operacion de transposicion se ejecuta 
       correctamente, puede comentar esta funcion para que la matriz de 
       salida sea siempre la misma.
    */
    srand((unsigned)time(NULL));

    size_t t_start=0, t_end = 0;

    t_start = get_milliseconds();

    for(int i = core * FILAS / HILOSINIT; i < ((core + 1) * FILAS) / HILOSINIT ; i++){
	for(int j = 0; j < COLS; j++){

		x = rand();
		y = rand();

		/* La siguiente operacion puede generar una condicion de error 
		   de ejecucion. TODO: Agregar un control que evite dicho error
		   y explique cual era el posible error que se podia presentar.
                */
		y++;
      if (y==0){y=1;}
		matOriginal[i][j] = x/y;

	}

    }



    t_end = get_milliseconds();

    printf("Tiempo total en el hilo inicializacion %d: %d\n", core, t_end-t_start);
}

void* transponer(void* arg)
{
 
    int core = step_j; // A core se le asigna el valor de step_i
    step_j++;        // Luego esta se incrementa

    // TODO: Como se puede combinar lo anterior en una sola instruccion?
   
    int i, j;

    size_t t_start=0, t_end = 0;

    t_start = get_milliseconds();

   for(int i = core * FILAS / HILOSINIT; i < ((core + 1) * FILAS) / HILOSINIT ; i++){
	for(int j = 0; j < COLS; j++){
      		matTranspuesta[j][i]=matOriginal[j][i];

	}

    }



    t_end = get_milliseconds();

    printf("Tiempo total en el hilo transpuesta %d: %d\n", core, t_end-t_start);
}
void* determinante(void* arg){
   float matAux[3][3];
   float det_matriz;
   size_t t_start=0, t_end = 0;//referencia de inicio y fin
   t_start = get_milliseconds();//iniciamos el tiempo
   for (int i = 0; i < 3; i++)
   {
      for (int j= 0; i < 3; i++)
      {
         matAux[i][j]=matTranspuesta[i][j];
      }
      
   }
   //det_matriz= determ(matAux,3);
   //printf("El determinante es:%f",det_matriz);
   t_end = get_milliseconds();//capturamos tiempo
   printf("Tiempo total en el hilo determiante %d: %d\n",1, t_end-t_start);

}
float determ(float matrix[][3],int orden);
float cofactor(float matrix[][3],int orden,int fila ,int  columna);
/* El programa principal. Este programa crea el hilo de inicializacion
   y calcula el tiempo que el programa se tarda en inicializar la matriz */


int main(void)
{

    /* Variables para medir el tiempo de ejecucion */	
    /* size_t es un tipo abstracto del sistema. Es simplemente un
       unsigned int 
    */ 
    size_t start=0, end=0;
   
    // Se toma el tiempo al momento de iniciar la operacion 
    start = get_milliseconds();


    /* Para simplificar el manejo de multiples hilos, es mas facil crear un 
       array de hilos 
    */
    pthread_t threads[HILOSINIT];
    
	
    /* En este ciclo se crean los hilos que comienzan a ejecutar la operacion
       init que inicializa la matriz. El cuarto argumento, es un puntero
       que no hace nada en este caso
    */
    for (int i = 0; i < HILOSINIT; i++) {
        int* p;
        pthread_create(&threads[i], NULL, init, (void*)(p));
    }
    
 
    /*	La funcion pthread_join() junta todos los hilos y espera a que se 
	completen. Para mas informacion, revise la documentacion.
    */
    for (int i = 0; i < HILOSINIT; i++) 
        pthread_join(threads[i], NULL);  
  
   //aplicando transpuesta la matriz original 
   pthread_t threads_trans[HILOSTRANS];//aray de hilos para la transpuesta

   //inicio de los hilos
    for (int i = 0; i < HILOSTRANS; i++) {
        int* p;
        pthread_create(&threads_trans[i], NULL, transponer, (void*)(p));
    } 
   //aplicando join a los hilos
   for (int i = 0; i < HILOSTRANS; i++) 
        pthread_join(threads_trans[i], NULL);

   //hilo para el determinante
   
   pthread_t thread_determinante;
   int* p;
   pthread_create(&thread_determinante,NULL,determinante,(void*)(p));
   pthread_join(thread_determinante,NULL);



    /* Finalmente, se toma el tiempo de finalizacion para calcular
       la duracion total de la operacion. 
    */
    end = get_milliseconds();


    printf("\nTiempo de Inicializacion %d\n", end-start);

    return 0;
}
/*
float determ(float matrix[][3],int orden){
    
    float detM = 0.0;
    if (orden <= 1){
        detM = matrix[0][0];
    }else
    {
        for (int i= 0; i<orden;i++){
        detM = detM + matrix[0][i]*(cofactor(matrix,orden,0,i));
        }
    }
    return detM;
}
float cofactor(float matrix[][3],int orden,int fila ,int  columna){
    int n = orden-1;
    float auxMatrix [3][3];
    int x = 0;
    int y = 0;
    
    for (int i = 0; i < orden; i++) {
      for (int j = 0; j < orden; j++) {
         if (i != fila && j != columna) {
            auxMatrix[x][y] = matrix[i][j];
            y++;
            if (y >= n ) {
               x++;
               y = 0;
            }
         }
      }
   }
    
    return pow(-1.0 , fila + columna)*(determ(auxMatrix,n));
}
*/
