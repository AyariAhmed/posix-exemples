#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <stdio.h>

#define NB_CAMIONS 5
#define NB_VOITURES 5
#define NB_VEHICULES (NB_CAMIONS + NB_VOITURES)


sem_t sem_compteur;
pthread_mutex_t camions_sc;



void quitter_pont(int tonnes);
int tirage_aleatoire(double max);
void acceder_au_pont(int tonnes);



// but de ce fonction est de faire le processus appelant attendre pendant use periode proche de max
void attendre(double max)
{
    struct timespec delai;
    delai.tv_sec = tirage_aleatoire(max);
    delai.tv_nsec = 0;
    nanosleep(&delai, NULL);
}

//return int between 1 and max*RAND_MAX/(RAND_MAX+1)

int tirage_aleatoire(double max)
{
    int j = (int)(max * rand() / (RAND_MAX + 1.0));
    if (j < 1)
        return 1;
    return j;
}

void *voiture(void *args)
{
    int pid = *((int *)args);
    attendre(5.0);
    acceder_au_pont(5);
    printf("Voiture %d traverse le pont \n", pid);
    attendre(5.0);
    printf("Voiture %d quitte le pont\n", pid);
    quitter_pont(5);
    pthread_exit(NULL);
}

void *camion(void *args)
{
    int pid = *((int *)args);
    attendre(5.0);
    acceder_au_pont(15);
    printf("Camion %d traverse le pont \n", pid);
    attendre(5.0);
    printf("Camion %d quitte le pont\n", pid);
    quitter_pont(15);
    pthread_exit(NULL);
}


void acceder_au_pont(int tonnes)
{
    if (tonnes == 15)
    {
        pthread_mutex_lock(&camions_sc);
        while (tonnes > 0)
        {
            sem_wait(&sem_compteur);
            tonnes -= 5;
        }
        pthread_mutex_unlock(&camions_sc);
    }
    else sem_wait(&sem_compteur);
}

void quitter_pont(int tonnes)
{
    while (tonnes > 0)
    {
        sem_post(&sem_compteur);
        tonnes -= 5;
    }
}

int main()
{
    int i;
    pthread_t id;
    sem_init(&sem_compteur, 0, 3);
    for (i = 0; i < NB_VEHICULES; i++)
    {
        int *j = (int *)malloc(sizeof(int));
        *j = i;
        if (i < NB_CAMIONS)
            pthread_create(&id, NULL, camion, j);
        else
            pthread_create(&id, NULL, voiture, (j - 4));
    }
    pthread_exit(NULL);
}