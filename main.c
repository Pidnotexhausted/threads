#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#include <fcntl.h>
#include <sys/stat.h>

//declare lock and var-cond
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER; //mutex for data
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER; //mutex for file thongtinsinhvien.txt
pthread_mutex_t lock3 = PTHREAD_MUTEX_INITIALIZER; //mutex for total


pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond2 = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond3 = PTHREAD_COND_INITIALIZER;

typedef struct
{
    char ho_ten[50];
    char ngay_sinh[50];
    char que_quan[50];
} sinhvien_t;

//declare global variable
sinhvien_t data = {0};
const char* pathname = "thongtinsinhvien.txt";
int total = 0; //tong so SV
int firsttime = 0; 

//function suport for handler1: change \n to \0
void trim_newline(char *str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n')
        str[len - 1] = '\0';
}

static void *handler1(void *args)
{
    if (total == 0 | firsttime == 0){
        pthread_mutex_lock(&lock1);

        //mission
        printf("This is thread1\nPlease add name:\n");
        fgets(data.ho_ten, sizeof(data.ho_ten), stdin);
        trim_newline(data.ho_ten);
        sleep(1);
        printf("Please add date of birth:\n");
        fgets(data.ngay_sinh, sizeof(data.ngay_sinh), stdin);
        trim_newline(data.ngay_sinh);
        sleep(1);
        printf("Please add native place:\n");
        fgets(data.que_quan, sizeof(data.que_quan), stdin);
        trim_newline(data.que_quan);
        sleep(1);
        printf("Information is completed: %s (%s) was born at %s\n\n", data.ho_ten, data.ngay_sinh, data.que_quan);
        sleep(2);
        total++;
        firsttime = 0;
        pthread_cond_signal(&cond1);
        pthread_mutex_unlock(&lock1);
    }
    while(total != 0){
        pthread_mutex_lock(&lock1);
        // printf("Thread1 is waiting cond3\n");

        pthread_cond_wait(&cond3, &lock1);

        //mission
        printf("This is thread1\nPlease add name:\n");
        fgets(data.ho_ten, sizeof(data.ho_ten), stdin);
        trim_newline(data.ho_ten);
        sleep(1);
        printf("Please add date of birth:\n");
        fgets(data.ngay_sinh, sizeof(data.ngay_sinh), stdin);
        trim_newline(data.ngay_sinh);
        sleep(1);
        printf("Please add native place:\n");
        fgets(data.que_quan, sizeof(data.que_quan), stdin);
        trim_newline(data.que_quan);
        sleep(1);
        printf("Information is completed: %s (%s) was born at %s\n\n", data.ho_ten, data.ngay_sinh, data.que_quan);
        sleep(2);
        total++;

        pthread_cond_signal(&cond1);

        pthread_mutex_unlock(&lock1);
    }
    

    return NULL;
}
static void *handler2(void *args)
{
    while(1){
        pthread_mutex_lock(&lock1);
        // printf("Thread2 is waiting for cond1\n");

        //wait for thread1 complete and send cond_signal
        pthread_cond_wait(&cond1, &lock1);
        printf("This is thread2\n");

        pthread_mutex_lock(&lock2);

        int fd, num_write;
        //create or open if it available
        char* newline = "\n";
        fd = open(pathname, O_RDWR | O_CREAT | O_APPEND, 0777);
        if (-1 == fd) { 
            printf("open() hello.txt failed\n");
        }  

        //write infor of student to file txt
        num_write = write(fd, data.ho_ten, strlen(data.ho_ten));
        num_write = write(fd, newline, strlen(newline));
        num_write = write(fd, data.ngay_sinh, strlen(data.ngay_sinh));
        num_write = write(fd, newline, strlen(newline));
        num_write = write(fd, data.que_quan, strlen(data.que_quan));
        num_write = write(fd, newline, strlen(newline));
        sleep(1);
        printf("Thread2 write to 'thongtinsinhvien.txt' completed\n\n");
        
        close(fd);
        pthread_cond_signal(&cond2);
        pthread_mutex_unlock(&lock2);
        pthread_mutex_unlock(&lock1);
    }

    return NULL;
}

static void* handler3(void *args){
    while(1){
        pthread_mutex_lock(&lock2);
        // printf("Thread3 is waiting for cond2\n");

        pthread_cond_wait(&cond2, &lock2);
        printf("This is thread3\n");
        //get file size
        struct stat st;
        if (stat(pathname, &st) != 0) {
            perror("stat failed");
            exit(1);
        }
        //check size is not error?
        if (st.st_size < 0 || (uintmax_t)st.st_size > SIZE_MAX) {
            fprintf(stderr, "File quá lớn hoặc bị lỗi\n");
            exit(1);
        }
        size_t size = (size_t)st.st_size;
        char* buffer = (char*)malloc(size+1);


        int fd, numwrite;
        fd = open(pathname, O_RDONLY, 0777);


        ssize_t n = read(fd, buffer, size);
        buffer[size] = '\0';
        printf("Thread3 showes information that have read\n");
        sleep(1);
        printf("Total of students is %d\n", total);
        sleep(2);
        printf("%s\n", buffer);
        sleep(2);
        free(buffer);
        close(fd);

        // //ask customer want to add information anymore?
        // printf("Do you want to ")

        pthread_cond_signal(&cond3);
        pthread_mutex_unlock(&lock2);
    }
    
    return NULL;
}


int main(int argc, char **argv)
{
    pthread_t tid1, tid2, tid3;
    int ret;

    int fd = open(pathname, O_RDWR | O_CREAT, 0777);

    //check xem file co rong khong?
    struct stat st;
    stat(pathname, &st);
    if (st.st_size == 0){
        total = 0;
    }else{
        int line_count = 0;
        char buffer[1024];
        ssize_t bytes;
        
        while ((bytes = read(fd, buffer, sizeof(buffer))) > 0) {
            for (ssize_t i = 0; i < bytes; i++) {
                if (buffer[i] == '\n') {
                    line_count++;
                }
            }
        }
        total = line_count/3;
    }
    close(fd);

    //crete thread2 to write infor to file
    ret = pthread_create(&tid2, NULL, &handler2, NULL);

    //create thread1 to add infor
    ret = pthread_create(&tid1, NULL, &handler1, NULL);

    //create thread3 to read thongtinsinhvien.txt and printf it
    ret = pthread_create(&tid3, NULL, &handler3, NULL);

    // pthread_mutex_lock(&lock);
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    
    return 0;
}