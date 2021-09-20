
// restrict помогает программисту показать компилятору, что по адресу, 
// на который указывает переменная с данным ключевым словом не будет 
// никаких других указателей, тогда компилятор сможет скомпилировать 
// программу эфективнее 

#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>

int sum = 0;
pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

struct vec
{
    int id;
    int* vals;
    int threads_num;
    int vec_size;
};

void* vec_sum(void* arg) {
    struct vec* parg = (struct vec*)(arg);
    int i;
    long long local_sum = 0;
    for (i = parg->id; i < parg->vec_size; i += parg->threads_num){
        local_sum += parg->vals[i];
    }
    int err = pthread_mutex_trylock(&g_mutex);
    sum += local_sum;
    if(err != EBUSY){
        pthread_mutex_unlock(&g_mutex);
    }
    return NULL;
}

int
vector_sum (int *v, int size, int nthreads)
{
    pthread_t threads[nthreads];
    struct vec vector[nthreads];
    int i;
    for (i = 0; i < nthreads; ++i){
        vector[i].id = i;
        vector[i].vals = v;
        vector[i].threads_num = nthreads;
        vector[i].vec_size = size;
    }
    for (i = 0; i < nthreads; ++i){
        int res = pthread_create(threads + i, NULL, vec_sum, vector + i);
        accert(res == 0);
    }
    void* status;
    for (int i = 0; i < nthreads; ++i) {
        status = NULL;
        pthread_join(threads[i], &status);
    }
    return sum;
}
/*
int main(int argc, char const *argv[]) {
    int v[10];
    v[0] = 1;
    v[1] = 2;
    v[2] = 3;
    v[3] = 4;
    v[4] = 5;
    v[5] = 6;
    v[6] = 7;
    v[7] = 8;
    v[8] = 9;
    v[9] = 5;
    for (int j = 1; j < 10; ++j){
        long int start = clock();
        int res = vector_sum(v, 10, j);
        long int end = clock();
        sum = 0;
        printf("Sum = %d\n", res);
        printf("Time: %lf s\n", (double) (end - start) / CLOCKS_PER_SEC);
    }
    return 0;
}
*/
