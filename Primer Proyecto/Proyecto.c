//#################################
#include <stdio.h>              //#
#include <ctype.h>              //#
//Inlcudes for file management  //#
#include <fcntl.h>              //#
#include <sys/types.h>          //#
#include <sys/stat.h>           //#
#include <unistd.h>             //#
//memory mapped include         //#                
#include <sys/mman.h> 		//#
#include <stdlib.h>		//#
#include <math.h> 		//#
#include <string.h>		//#
#include <stdbool.h>		//#
#include <pthread.h>            //#
#include <ncurses.h>            //#
//Timer                         //#
#include <time.h>               //#
//#################################


//Variables
FILE *ArchivoEntrada; //variable de tipo FILE 

//Thread Synchronization Problems --> Mutexes
//funcion mutex para cerrar (lock) una region de codigo
pthread_mutex_t lock;
int Cantidad_Threads = 1; //Numero de threads a realizar iniciar
pthread_t threads[1]; //objeto pthread en el cual contendra el arreglo de threads creados
bool Finalizados[1]; //banderas para finalizar threads
double Bytes_Leer; //lee los bytes del ArchivoEntrada
double Linea_Inicio; //Variable en la cual se le asignara el valor en que comience a leer el archivo.
double Inicio_Leer = 0;
int conteo[255]; //un arreglo de todos los caracteres del ascii, en el cual se incrementara ++ cuando encuentre este caracter
int conteo2;
//variables para el timer
clock_t start_t, end_t, total_t, total_t2;


//Metodo que contara cuantos caracteres tiene la primera fila y en base al resultado comenzara a analizar el archivo desde esa posicion.
int charcount( FILE *const fin,int cas)
{
    int c, count;
    char compare;
    count = 0;
     switch (cas)
      {
    
    case 1:
    for( ;; )
    {
        c = fgetc( fin );    
        if( c == EOF || c == '\n' )
            break;	
        ++count;
    }
    case 2:
    for(;;)
    {
        c = fgetc( fin );  
	       
        if(c == 44) {++count;}
	//EOF equivale a fin de linea      
        if( c == EOF || c == '\n' )
            break;	       
    }
        ++count;
       }
   
    return count;
}

//Metodo que sumara las minusculas con las mayusculas
void *Sumatoria()
{

int j =97;
for(int i =65;i<=90;i++)
{

conteo[i] = conteo[j]+ conteo[i];
j++;
}

}

//Declaracion de variables para iniciar en archivo
//Se define un estructura (agrupacion de diferentes tipos de datos para relacionarlos a un objetivo)
struct LeerBytes 
{
    double Inicio_Bytes;
    double Fin_Bytes;
    int posicion;
};


//Metodo que sirve para escribir el archivo de salida
void *Archivo_Salida()
{
FILE * fptr;
fptr = fopen("Salida.csv", "w");

for (int i = 65; i <= 90; i++) 
{		

        //if(i == 67) {int res = conteo[i]-conteo2; if(res < 0) {conteo[i] = 0;}else { conteo[i] = res;} }
	char fn[3];	
	fn[0] = (char) i;
	fn[1] = ',';	
	fn[2] = '\n';        

 	for (int b = 0; b <3; b++)
	{	

	if(b == 2)
	{ fprintf(fptr,"%d",conteo[i]);}
	fputc(fn[b], fptr); 
	}
	     
	}    
fclose(fptr);
}


//Metodo que imprime resultado en consola
void *imprimir()
{
    char ch;
    scanf("%c",&ch);

//ocultar el cursor
//curs_set (0);
//system("clear");
//sleep(1);

   Sumatoria();
for (int i = 0; i <= 255; i++) 
    {	if(i == 67) { conteo[i] = conteo[i]-conteo2; } //para C
	//if(i == 88) { conteo[i] = conteo[i]-conteo2; } //para X
	//Solo se imprime del caracter ascii 65 (A) hasta el caracter ascii 90 (Z)
	if(i >= 65 && i <= 90)
	{
        printf("Letra: %c | Total: (%i)\n", (char) i, conteo[i]);
	}
    }
}

//Metodo de threads que lee el archivo
void *Analisis_Threads(void *entrada) 
{
    //Obtiene el valor de entrada del argumento del struct 
    //almacena el valor de las variables globales de inicio de byte, fin byte y la posicion. 
    double Posicion_Byte_Inicio = ((struct LeerBytes*)entrada)->Inicio_Bytes;
    double Posicion_Byte_Fin = ((struct LeerBytes*)entrada)->Fin_Bytes;
    int threadPosicion = ((struct LeerBytes*)entrada)->posicion;

    char Varptr[1024];
    char c;
    double conteoInterno[255]; //ASCII
    int Varnmemb = 1024;
    //bloquea el archivo para que solo un thread lo lea a la vez
    //Se crearan n threads, por lo que cada thread leera cierta porcion del archivo, se le asignara en que posicion de bytes comience y en que posicion de bytes finalice.

   /*Se utiliza la funcion pthread_mutex_lock para que unicamente un thread pueda realizar una accion a la vez y no realicen los n threads la misma tarea, sino que cada uno realice una tarea cuando 
   el thread n termine el n+1 comience y asi, esto se hace para que no cuente los mismos datos */
    for (double i = Posicion_Byte_Inicio; i <= Posicion_Byte_Fin; i += 1024) 
	{
	if((i+ 1024)> Posicion_Byte_Fin)
	{
	Varnmemb = (Posicion_Byte_Fin - i);
	}
	else {
	Varnmemb = 1024;
	}
    
	//////*CRITICAL SECTION*/////////
	//hacemos lock con mutex para que ningun otro thread acceda a esta región de codigo 
        pthread_mutex_lock(&lock);

	/*
	fseek() reposiciona un puntero de tipo FILE 
	void fseeko(FILE * steam, long offset, int whence)

	//PARAMETROS
	stream: Objeto de tipo FILE que identifica la secuencia
	offset: Es la nueva posicion del archivo
	whence: Es la posicion desde donde se agrega el desplazamiento --> Utilizamos SEEK_SET: Posición respecto al inicio del archivo
	*/
            if (fseek(ArchivoEntrada, i, SEEK_SET) == 0) 
		{   
			/*
			fread() lee los datos del stream dado al array apuntado por el ptr(Varptr)
			size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream)

			//PARAMETROS	
   			ptr − es un puntero a un bloque de memoria con un tamaño minimo de *nmemb bytes
    			size − es el tamaño de bytes que cada elemeto va a leer
    			nmemb − es el numero de elementos, cada uno con un tamaño de size bytes
    			stream − puntero a un objeto FILE 
			*/
                	 fread(&Varptr, 1, Varnmemb, ArchivoEntrada);

			 //se desbloquea, el thread le pasa la llave a otro thread para acceder a esta region de codigo
        	         pthread_mutex_unlock(&lock);           
          	 } else 
			{
				//ERROR
			}

        // Realiza la cuenta
        for (int i = 0; i <= Varnmemb; i++) 
	{	    
            conteoInterno[(int) ((char) Varptr[i])]++;
        }
    }
        for (int i = 0; i <= 255; i++) 
	{
            conteo[i] += conteoInterno[i];
        }

	//Almacena en el arreglo de finalizados las banderas en que los threads acabaron su proceso.
        Finalizados[threadPosicion] = true;
        bool flagTerminados = true;
        for (int i = 0; i < Cantidad_Threads; i++) 
	{           
            //double free or corruption (!prev) -error
            if (!Finalizados[i]) flagTerminados = false;
        }

	//Valida si todos los threads ya fueron finalizadas sus tareas.
	//solamente entra si todos los threads ya han finalizado sus operaciones
        if (flagTerminados) 
	{
	    //detener el timer
	    end_t = clock();
            //printf("End of timer = %ld\n", end_t);
            total_t = (float)(end_t - start_t) / CLOCKS_PER_SEC;  
	    total_t2 = (float)(end_t - start_t) / 60;  
	    //cerramos el FILE* de entrada
            fclose(ArchivoEntrada);
 	    printf("\n");
            printf("*************************************\n");
            printf("PROCESO DE CONTEO FINALIZADO\n");
	    if(total_t != 0)
	    {
            printf("Tiempo total por el CPU: %ld Segundos (%ld milisegundos)\n", total_t , total_t*1000);
	    }
	     else {
		  printf("Tiempo total por el CPU: %ld Segundos (%ld milisegundos)\n", total_t2 , total_t2*1000);
		  }
            printf("*************************************\n");
            printf("\n");
            printf("Presione ENTER para ver los resultados\n");
        }
}


//////MAIN///////
int main() {
//inicia la libreria de ncurses
//initscr ();d

    //archivo 1.0 kb
    char ubicacion[100] = "/home/ivan/Escritorio/prueba/File.csv";   
    
    //archivo 1.0 mb
    //char ubicacion[100] = "/home/charlie/File.csv"; 

    //archivo 2.8 gb
   // char ubicacion[100] = "/home/ivan/Escritorio/File3.csv";  

    FILE *fin;
    fin = fopen(ubicacion, "r" );
    Bytes_Leer = charcount(fin,1)-1;    
    conteo2 = charcount(fin,2); 
    system("clear");

    // Abre el archivo en modo read only ("r)
    ArchivoEntrada = fopen(ubicacion, "r");

    //fseek() reposiciona un puntero de tipo FILE 
    //void fseeko(FILE * steam, long offset, int whence)
    //whence - SEEK_END: Fin del archivo
    fseek(ArchivoEntrada, 0LL, SEEK_END);

    //Obtiene el peso del archivo
    //ftell() returna el la posicion del archivo actual dado el stream proporcionado (ArchivoEntrada)
      double fileSize = ftell(ArchivoEntrada);
      printf("El archivo con ruta %s pesa: %lf GB (%f Bytes)\n",ubicacion, fileSize / 1e+9,fileSize);
      printf("\n");
      printf("Presione ENTER para empezar el conteo\n");
      printf("-----------------------------------------------------------------------\n");
      char sc;
      scanf("%c",&sc);
      //empieza el timer
      start_t = clock();
      printf("¡Espere a que el conteo haya finalizado!\n");
      Bytes_Leer = fileSize;

    //Creamos una variable de tipo struct y un contador para el ciclo while para validar con la cantida de threads
    int i = 0;
    struct LeerBytes *Struct_Main;    
    double lineaInicio = Linea_Inicio;
    Inicio_Leer = Bytes_Leer;
   
    //double ceil(double x)
    //ceil() retorna el entero mas pequeño que sea mayor o igual al parametro dado (Bytes_Leer / (Cantidad_Threads))
    double bytesLeidos_Thread = ceil(Bytes_Leer / (Cantidad_Threads));
    //Variable que indicara cuando se encuentre el tope del archivo
    double lineaFin = lineaInicio + bytesLeidos_Thread;
    
    while (i < Cantidad_Threads) 
	{
	//int contador = 0;
	// Crea el struct con la cantidad de lineas

	//void *malloc(size_t size) 
        //Malloc() asigna la memoria solicitada y retorna un puntero
        Struct_Main = (struct LeerBytes *)malloc(sizeof(struct LeerBytes));
        Struct_Main->Inicio_Bytes = lineaInicio;
        Struct_Main->Fin_Bytes = lineaFin;
        Struct_Main->posicion = i;
        //se crean threads, parametros(arreglo de instancia de  threads, NULL, proceso de thread, parametros del proceso de thread)
        pthread_create(&threads[i], NULL, Analisis_Threads, Struct_Main);
        
	
	lineaInicio = lineaFin + 1;
        lineaFin = lineaInicio + bytesLeidos_Thread; 
        
	if (lineaFin >= Bytes_Leer) 
	{
            lineaFin = Bytes_Leer;

        }
	/*
	while (contador<3)
	{		
		imprimir();
		contador++;
		refresh ();
	}
	*/
        i++;
    }


   imprimir();
   Archivo_Salida();
   //cierra la ventana (ncurses)
   //endwin();
    return 0;
}
