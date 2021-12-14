#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <stdio.h>

#define NB_CAMIONS 5
#define NB_VOITURES 5
#define NB_VEHICULES (NB_CAMIONS + NB_VOITURES)

#define ATTENDRE 1
#define RIEN 2
#define TRAVERSER 3

pthread_mutex_t sc;
sem_t sempriv[NB_VEHICULES];
int etat[NB_VEHICULES];
int nb_camion_bloques = 0;
int seuil = 0;

void attendre(double max);
int tirage_aleatoire(double max);
void *voiture(void *args);
void *camion(void *args);
void acceder_au_pont(int tonnes, int id);
void quitter_pont(int tonnes, int pid);

void acceder_au_pont(int tonnes, int id)
{
    pthread_mutex_lock(&sc);
    if (seuil + tonnes <= 15)
    {
        seuil += tonnes;
        etat[id] = TRAVERSER;

        pthread_mutex_unlock(&sc);
    }
    else
    {
        etat[id] = ATTENDRE;
        if (tonnes == 15)
            nb_camion_bloques++;
        pthread_mutex_unlock(&sc);
        sem_wait(&sempriv[id]);
    }
}

void quitter_pont(int tonnes, int pid)
{

    
    int i;
    pthread_mutex_lock(&sc);

    etat[pid] = RIEN;
    seuil -= tonnes;

    for (i = 0; i < NB_CAMIONS; i++)
    {
        if ((etat[i] == ATTENDRE) && (seuil == 0))
        {
            seuil = 15;
            nb_camion_bloques--;
            sem_post(&sempriv[i]);
        }
    }
    for (i = NB_CAMIONS; i < NB_VEHICULES; i++)
    {

        if ((seuil < 15) && (nb_camion_bloques == 0) && (etat[i] == ATTENDRE))
        {

            seuil += 5;
            sem_post(&sempriv[i]);
        }
    }
    pthread_mutex_unlock(&sc);

}

void attendre(double max)
{
    struct timespec delai;
    delai.tv_sec = tirage_aleatoire(max);
    delai.tv_nsec = 0;
    nanosleep(&delai, NULL);
}

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
    acceder_au_pont(5, pid);
    printf("Voiture %d traverse le pont \n", pid - 4);
    attendre(5.0);
    printf("Voiture %d quitte le pont\n", pid - 4);
    quitter_pont(5, pid);
    pthread_exit(NULL);
}

void *camion(void *args)
{
    int pid = *((int *)args);
    attendre(5.0);
    acceder_au_pont(15, pid);
    printf("Camion %d traverse le pont \n", pid);
    attendre(5.0);
    printf("Camion %d quitte le pont\n", pid);
    quitter_pont(15, pid);

    pthread_exit(NULL);
}

int main()
{
    int i;
    pthread_t id;

    pthread_mutex_init(&sc, 0);

    for (i = 0; i < NB_VEHICULES; i++)
    {

        etat[i] = RIEN;
        sem_init(&sempriv[i], 0, 0);

        int *j = (int *)malloc(sizeof(int));
        *j = i;
        if (i < NB_CAMIONS)
            pthread_create(&id, NULL, camion, j);
        else
            pthread_create(&id, NULL, voiture, j);
    }
    pthread_exit(NULL);
    return 0;
}