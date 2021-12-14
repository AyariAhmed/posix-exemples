#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <stdio.h>

#define NB_MR1 2
#define NB_MR2 1
#define NB_MR3 1

#define AB 0
#define BC 1
#define CD 2
#define EC 3
#define BF 4
pthread_mutex_t sc[5];

void acceder(int segment, int id)
{
    printf("Le metro %d attend pour acceder le segment %d .\n", id, segment);
    pthread_mutex_lock(&sc[segment]);
    printf("Le metro %d occupe le segment %d .\n", id, segment);
}

void quitter(int segment, int id)
{
    printf("Le metro %d vient de quitte le segment %d .\n", id, segment);
    pthread_mutex_unlock(&sc[segment]);
}

int tirage_aleatoire(double max)
{
    int j = (int)(max * rand() / (RAND_MAX + 1.0));
    if (j < 1)
        return 1;
    return j;
}

void attendre(int periode)
{
    struct timespec delai;
    delai.tv_sec = periode;
    delai.tv_nsec = 0;
    nanosleep(&delai, NULL);
}

void *metro_MR1(void *args)
{
    attendre(tirage_aleatoire(3.0));

    int pid = *((int *)args);

    acceder(AB, pid);
    attendre(6);
    quitter(AB, pid);

    acceder(BC, pid);
    attendre(6);
    quitter(BC, pid);

    acceder(CD, pid);
    attendre(6);
    quitter(CD, pid);

    pthread_exit(NULL);
}

void *metro_MR2(void *args)
{

    attendre(tirage_aleatoire(3.0));

    int pid = *((int *)args);

    acceder(BF, pid);
    attendre(6);
    quitter(BF, pid);

    acceder(AB, pid);
    attendre(6);
    quitter(AB, pid);

    pthread_exit(NULL);
}

void *metro_MR3(void *args)
{

    attendre(tirage_aleatoire(3.0));

    int pid = *((int *)args);

    acceder(EC, pid);
    attendre(6);
    quitter(EC, pid);

    acceder(BC, pid);
    attendre(6);
    quitter(BC, pid);

    pthread_exit(NULL);
}

int main()
{
    int id_metro = 0;
    pthread_t id;
    int i;

    for (i = 0; i < 5; i++)
    {
        pthread_mutex_init(&sc[i], 0);
    }

    for (i = 0; i < NB_MR1; i++)
    {

        int *j = (int *)malloc(sizeof(int));
        *j = id_metro;
        id_metro++;
        pthread_create(&id, NULL, metro_MR1, j);
    }
    for (i = 0; i < NB_MR2; i++)
    {

        int *j = (int *)malloc(sizeof(int));
        *j = id_metro;
        id_metro++;
        pthread_create(&id, NULL, metro_MR2, j);
    }
    for (i = 0; i < NB_MR3; i++)
    {

        int *j = (int *)malloc(sizeof(int));
        *j = id_metro;
        id_metro++;
        pthread_create(&id, NULL, metro_MR3, j);
    }

    pthread_exit(NULL);
    return 0;
}
