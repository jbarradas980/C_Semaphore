#include "progconcutils.h"
#include <semaphore.h>

/*
 * Fecha de creación: 18 de marzo del 2019
 * Descripción: Implementación de solución general al problema de los lectores y escritores usando hilos en Linux.
 */

#define TRUE 1
#define FALSE 0
#define NUM_LEC 4
#define NUM_ESC 4
// Se definen los semaforos:
sem_t sem_lec;		//Para bloquear lectores
sem_t sem_esc;		//Para bloquear escritores
sem_t sem_mutex;	//Para exclusión mutua

int cont_lec; 		//Contador de lectores accediendo a la bd
int cont_esc; 		//Contador de escritores accediendo a la bd
int lec_acce;		//Lector que esta accediendo
int BD; 		//Contenido de la base de datos.
int escribiendo;	//Bandera para saber si se esta escribiendo en la bd

void lector(int i) {
	for (int k=0;k<10;k++) {
		sleep(5);
		sem_post(&sem_mutex);
			if (escribiendo == TRUE || cont_esc != 0){
				cont_lec=cont_lec+1;
				sem_wait(&sem_mutex);
				sem_post(&sem_lec);
				cont_lec=cont_lec-1;
			}
		lec_acce = lec_acce +1;
		if (cont_lec != 0){
			sem_wait(&sem_lec);
		}else{
			sem_wait(&sem_mutex);
		}
		/* Accediendo a la BD */
		printf("Lector %d leyendo BD = %d\n", i, BD);
		sem_post(sem_mutex);
			lec_acce = lec_acce-1;
			if (lec_acce == 0 && cont_esc !=  0)
				sem_wait(&sem_esc);
			else
				sem_wait(&sem_mutex);
		
	}//Termina for
}// Fin de la función lector.


void escritor (int j){
	for (int m=0; m<10; m++){
		sleep(5);
		sem_post(sem_mutex);
			if (lec_acce != 0 || escribiendo == TRUE){
				cont_esc = cont_esc +1;
				sem_wait(&sem_mutex);
				sem_post(&sem_esc);
				cont_esc = cont_esc - 1;
			}
		escribiendo = TRUE;
		sem_wait(&sem_mutex);
			/* Actualizando BD */
			BD = BD + 1;
			printf("Escritor %d actualizando BD = %d\n", j, BD);
		sem_post(&sem_mutex);
		escribiendo = FALSE;
		if (cont_lec != 0)
			sem_wait(&sem_lec);
		else{
			if(cont_esc != 0)
				sem_wait(&sem_esc);
			else
				sem_wait(&sem_mutex);
		}
	}
}//Termina función escritor

void main (void){
	int i;
	/*Inicializamos el generador de números aleatorio*/
	srand((unsigned) getpid());
	/*Cantidad de lectores y escritores*/
	int nl=NUM_LEC; int ne=NUM_ESC;
	/*Arreglos para guardar los identificadores de los hilos*/
	pthread_t tidsl[NUM_LEC];
	pthread_t tidse[NUM_ESC];
	long lids[NUM_LEC];
	long eids[NUM_ESC];
	//sem_mutex	= 1;
	sem_init(&sem_mutex,0,1);
	//sem_esc	= 0;
	sem_init(&sem_esc,0,1);
	//sem_lec	= 0;
	sem_init(&sem_lec,0,1);
	/* Valires iniciales */
	cont_lec	= 0;
	cont_esc	= 0;
	lec_acce	= 0;
	escribiendo 	= FALSE;
	BD=0;
	/* Creamos los hilos para los lectores */
	for (i=0; i<nl; i++){
		lids[i] =(long)i;
		if (pthread_create(&tidsl [i], NULL, (void *)&lector, (void *)lids[i]) != 0 ){
			perror("Ocurrio un error (lector): pthread_create con proc(0)");
			exit (EXIT_FAILURE);
		}
	}
	/* Creamos los hilos para los escritores */
	for (i=0; i<ne; i++){
		eids[i] = (long)i;
		if (pthread_create(&tidse[i], NULL, (void *)&escritor, (void *)eids[i]) != 0){
			perror("Ocurrio un error (escritor): pthread_create con proc(0)");
			exit (EXIT_FAILURE);

		}
	}
	/* Espera la terminación de los hilos lectores */
	for (i=0;1<ne;i++){
		pthread_join(tidsl[i], NULL);
	}
	/* Espera la terminación de los hilos escritores */
	for (i=0;1<nl;i++){
		pthread_join(tidse[i], NULL);
	}
	/*
	//cobegin{
	lector (0);
	lector (1);
	lector (2);
	lector (3);
	escritor (0);
	escritor (1);
	escritor (2);
	escritor (3);
	//}
	*/
}
