#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<semaphore.h>

sem_t sisanje, posao, frizerZavrsio, musterijaZavrsila, mutex; //deklaracija promenljivih

#define THREAD_NUM 8

void balk(){
    printf("Otiso sam bilo je puno sve \n");
}

void getHaircut(){
printf("sisa se musterija \n");
}

void Haircut(){
printf("frizer sisa \n");
}

int musterija = 0;
int maks;

void* customer(){
while(1){
    if(musterija == maks){ //proveravamo da li je vec maksimalni broj musterija u salonu
        balk(); //ukoliko jeste musterija salje poruku da je bilo puno i nista se od daljeg dela funkcije ne izvrsava
    continue;
}
    sem_wait(&mutex); //mutex se smanjuje na nulu i obezbedjuje da prilikom menjanje broja musterija niko drugi ne utice na njenu vrednost
    musterija++;
    sem_post(&mutex); //mutex se oslobadja

    sem_post(&posao); //obavestava frizera da ima posao
    sem_wait(&sisanje); //ceka frizera da prione na posao

    getHaircut();

    sem_wait(&frizerZavrsio); //ceka se da frizer obavesti o zavrsetku
    sem_post(&musterijaZavrsila); //musterija signalizira da odlazi

    sem_wait(&mutex); //identicno kao malo pre radi sigurnosti prilikom menjanja vrednosti koristimo mutex
    musterija--;
    sem_post(&mutex);


}
    

}

void* barber(){
while(1){
    sleep(3);

    sem_wait(&posao); //frizer ceka na posao
    sem_post(&sisanje); //signalizira da je spreman da sisa

    Haircut(); //funkcija salje poruku da je sisanje u toku

    sem_post(&frizerZavrsio); //signal da je frizer zavrsio
    sem_wait(&musterijaZavrsila); //ceka se da se musterija pomeri
}

}

int main(int argc, char const *argv[])
{

    /* code */
    pthread_t th[THREAD_NUM];
    sem_init(&sisanje, 0, 0);
    sem_init(&posao, 0, 0);
    sem_init(&frizerZavrsio, 0, 0);
    sem_init(&musterijaZavrsila, 0, 0);
    sem_init(&mutex, 0, 1);

    printf("Unesi maksimalni broj musterija: ");
    scanf("%d", &maks);

    for(int i = 0; i < THREAD_NUM; i++){
        if(i == 0){
        if(pthread_create(&th[i], NULL, &barber, NULL) != 0){
            perror("greska prilikom pravljenja niti");
        }
        }
        else {
             if(pthread_create(&th[i], NULL, &customer, NULL) != 0){
            perror("greska prilikom pravljenja niti");
        }
        }
    }

    for(int i = 0; i < THREAD_NUM; i++){
        if(pthread_join(th[i], NULL) != 0){
            perror("greska prilikom cekanja niti da zavrsi");
        }
    }


    return 0;
}
