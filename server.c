#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include "util.h"
#include <stdbool.h>
#include <unistd.h> //for read(int fd, void* content_buffer, int size)

#define MAX_THREADS 100
#define MAX_queue_len 100
#define MAX_CE 100
#define INVALID -1
#define BUFF_SIZE 1024

/*
  THE CODE STRUCTURE GIVEN BELOW IS JUST A SUGESSTION. FEEL FREE TO MODIFY AS NEEDED
*/

// structs:

typedef struct request_node
{
    int fd;
    char *filename;
    struct request_node *next;
} request_node_t;

typedef struct request_queue
{
    int count;
    request_node_t *head;
    request_node_t *tail;
} request_queue_t;

typedef struct cache_entry {
    char *filename;
    int fd;
    int bytes;
    char *content;
    char *content_type;
    int status;
} cache_entry_t;

typedef struct worker_args {
  request_queue_t *request_queue;
  cache_entry_t * cache;
  int cache_size;
} warg_t;

typedef struct dispatch_args {
  request_queue_t *request_queue;
  int queue_length;
} darg_t;

/*typedef struct request_queue {
   int fd;
   void *request-;
} request_t;
*/

pthread_mutex_t accept_con_mtx =  PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t cond_mtx = PTHREAD_MUTEX_INITIALIZER;


/* ************************ Dynamic Pool Code ***********************************/
// Extra Credit: This function implements the policy to change the worker thread pool dynamically
// depending on the number of requests
void * dynamic_pool_size_update(void *arg) {
  while(1) {
    // Run at regular intervals
    // Increase / decrease dynamically based on your policy
  }
}


/**********************************************************************************/

/* ************************************ Cache Code ********************************/

void enqueue(request_queue_t *q,char *name, int descriptor)
{
    request_node_t *tmp;
    tmp = malloc(sizeof(request_node_t));
    tmp->fd = descriptor;
    tmp->filename = name;
    tmp->next = NULL;
    if(q->tail != NULL)
    {
        q->tail->next = tmp;
        q->tail = tmp;
    }
    else
    {
        q->head = q->tail = tmp;
    }
    q->count++;
}

request_node_t *dequeue(request_queue_t *q)
{
    request_node_t *tmp;
    int n = q->head->fd;
    tmp = q->head;
    q->head = q->head->next;
    q->count--;
    return tmp;
}


// Function to check whether the given request is present in cache
int getCacheIndex(char *filename,cache_entry_t * cache, int cache_size){
  /// return the index if the request is present in the cache
  for(int i = 0; i< cache_size ;i++){
    if(strcmp(filename,cache[i].filename)){
      return i;
    }
  }
  return -1;
}

// Function to add the request and its file content into the cache
int addIntoCache(cache_entry_t * cache, int fdnum ,char *name,int bytesread,char *content_buffer,char *cont_type, int cache_size){

  for(int i = 0; i < cache_size; i++){
    if(cache[i].status == -1){
      cache[i].filename = name;
      cache[i].fd = fdnum;
      cache[i].bytes = bytesread;
      cache[i].content = content_buffer;
      cache[i].content_type = cont_type;
      cache[i].status = 1;
      return i;
    }
  }
  //replace random cache_entry
  int rand_index = rand() % cache_size;

  cache[rand_index].filename = name;
  cache[rand_index].bytes = bytesread;
  cache[rand_index].content = content_buffer;
  cache[rand_index].content_type = cont_type;

  return rand_index;

}

// clear the memory allocated to the cache
void deleteCache(cache_entry_t *cache, int cache_size){
  for(int i = 0; i < cache_size; i++) {
    free(cache[i].content);
    free(cache[i].content_type);
    free(cache[i].filename);
  }
  free(cache);
}

// Function to initialize the cache
void initQueue(request_queue_t *q)
{
    q->count = 0;
    q->head = NULL;
    q->tail = NULL;
}

void initCache(cache_entry_t *cache,int cache_size){

  // Allocating memory and initializing the cache array
  // memset(cache->filename, 0, cache_size * sizeof(cache_entry_t));

  for(int i = 0; i < cache_size; i++){
    cache[i].bytes = -1;
    memset(cache[i].filename, '\0', BUFF_SIZE);
    memset(cache[i].content, '\0', BUFF_SIZE);
    memset(cache[i].content_type, '\0', BUFF_SIZE);
    cache[i].status = -1;
  }

}


// Function to open and read the file from the disk into the memory
// Add necessary arguments as needed
int readFromDisk(char *content_buffer,int fd) {

  FILE *file = fdopen(fd, "r");
  int bytes_read;

  if(file == NULL){
    perror("Error opening file");
    return -1;
  }

  if((bytes_read = read(fd,content_buffer,BUFF_SIZE)) > 0){

    return bytes_read;
  }
  else{
    perror("Error reading file");
    return -1;
  }
  // Open and read the contents of file given the request
}

/**********************************************************************************/

/* ************************************ Utilities ********************************/
// Function to get the content type from the request
char* getContentType(char * mybuf) {
  // Should return the content type based on the file type in the request
  // (See Section 5 in Project description for more details)
	int i=0;
	char *unused_token= strtok(mybuf,".");
	char *wanted_token= strtok(NULL, ".");

	if ((strcmp("html",wanted_token)==0)||(strcmp("htm",wanted_token)==0)){
		return "text/html";
	}
	else if (strcmp("jpg",wanted_token)==0){
		return "image/jpeg";
	}
        else if (strcmp("gif",wanted_token)==0){
		return "image/gif";
	}  
	else{
		return "text/plain";
	}
}

// This function returns the current time in milliseconds
int getCurrentTimeInMills() {
  struct timeval curr_time;
  gettimeofday(&curr_time, NULL);
  return curr_time.tv_usec;
}

/**********************************************************************************/

// Function to receive the request from the client and add to the queue
// Dispatcher threads are expected to be blocked at accept_connection until a new request is fired.
void * dispatch(darg_t *dispatcher_args) {

  while (1) {

  int connection_fd;
  request_node_t *replace;
  char filename[BUFF_SIZE];

  if(pthread_mutex_lock(&accept_con_mtx) == -1){
    printf("Failed to lock thread before accept_connection() call");
    continue;
  }

	  connection_fd = accept_connection(); //added by C.P.

  if(pthread_mutex_unlock(&accept_con_mtx) == -1){
    printf("Failed to unlock thread after accept_connection() call");
    continue;
  }
    // Get request from the client
  if(connection_fd >= 0){

	 if(get_request(connection_fd,filename)!=0){
     printf("Error getting request from fd: %d",connection_fd);
     continue;
   }
   else{
        if(dispatcher_args->request_queue->count < dispatcher_args->queue_length){   //check if queue is full
            enqueue(dispatcher_args->request_queue,filename,connection_fd);
          }
        else{
            replace = dequeue(dispatcher_args->request_queue);
            free(replace);
            enqueue(dispatcher_args->request_queue,filename,connection_fd);
            }
       }
  }
  else{
    perror("Accept connection returned negative file descriptor (dispacther)");
    continue;
  }

}
   return NULL;
}


/**********************************************************************************/

// Function to retrieve the request from the queue, process it and then return a result to the client
void * worker(warg_t *worker_args){

  int counter = 0;

  while (1) {

    char content_type[BUFF_SIZE];
    char content_buffer[BUFF_SIZE];
    cache_entry_t cache_entry;
    request_node_t *request;

    int bytesread;
    int index;
    int threadID = pthread_self(); 
    int hit_or_miss;// 1 if hit, 0 if miss 
    char * log_string; 

    int start_time = getCurrentTimeInMills();

    // Get a request from the queue. Continue to next itertation if no requests are in the queue
    if((request = dequeue(worker_args->request_queue)) == NULL){
      continue;
    }

    if((index = getCacheIndex(request->filename,worker_args->cache,worker_args->cache_size)) == -1) {  //if request in not in cache then readfrom disk and add to cache

      if((bytesread = readFromDisk(content_buffer,request->fd))>0){
        index = addIntoCache(worker_args->cache,request->fd,request->filename,bytesread,content_buffer,content_type,worker_args->cache_size);
        cache_entry = worker_args->cache[index];
	hit_or_miss = 0;

      }
      else{
        cache_entry = worker_args->cache[index];
	hit_or_miss = 1;
      }
    }


    int end_time = getCurrentTimeInMills();

  	//total time taken to get request and data, added by C.P.
  	int time_taken= end_time - start_time;
	counter = counter+1;

      // Log the request into the file and terminal

  	if((return_result(cache_entry.fd, cache_entry.content, cache_entry.content_type,cache_entry.bytes))!=0){
  		return_error(cache_entry.fd, "error text");
  	}

	char time_taken_char = time_taken + '0';

	log_string = "[ ";
	strcpy(log_string, threadID);
	strcpy(log_string, "][");
	strcpy(log_string, counter);
	strcpy(log_string, "][");
	strcpy(log_string, cache_entry.fd);
	strcpy(log_string, "][");
	strcpy(log_string, cache_entry.filename);
	strcpy(log_string, "][");
	strcpy(log_string, bytesread);
	strcpy(log_string, "][");
	strcpy(log_string, time_taken_char);
	strcpy(log_string, "][");
	strcpy(log_string, hit_or_miss);
	strcpy(log_string, "]");

	if((write(1,log_string,sizeof(log_string)))!=0){
		perror("Test");
	}
	
  }
  return NULL;
}

/**********************************************************************************/

int main(int argc, char **argv) {

  // Error check on number of arguments
  if(argc != 8){
    printf("usage: %s port path num_dispatcher num_workers dynamic_flag queue_length cache_size\n", argv[0]);
    return -1;
  }

  int port, num_dispatcher, num_workers, dynamic_flag /* Is dynamic flag an int?*/, queue_length, cache_size;//added by C.P.
  char *path;//or of type void?


  // Get the input args (added by C.P.)
	port=atoi(argv[1]);
	path=argv[2];
	num_dispatcher=atoi(argv[3]);
	num_workers=atoi(argv[4]);
	dynamic_flag=atoi(argv[5]);
	queue_length=atoi(argv[6]);
	cache_size=atoi(argv[7]);







  // Perform error checks on the input arguments (added by C.P.)
	if((port<1024)||(port==9000)||(port>65536)){
		printf("Please pick a random port number other than 9000 from (1024 to 65536)");
		return -1;
	}
	//Here, check is path is a valid path / file

	if((num_dispatcher>MAX_THREADS)||(num_dispatcher<=0)){
		printf("Invalid dispatcher thread number. Please pick a number between 1 and 100");
		return -1;
	}
	if((num_workers>MAX_THREADS)||(num_workers<=0)){
		printf("Invalid worker thread number. Please pick a number between 1 and 100");
		return -1;
	}

  // Change the current working directory to server root directory-----------------

  if(chdir(path) == -1){
    printf("Invalid path: %s", path);
    exit(1);
  }

  // Start the server and initialize cache and queue-----------------------------------------

  //init server and cache
  init(port);
  cache_entry_t req_cache[cache_size];
  initCache(req_cache,cache_size);

  // initialize queue
  request_queue_t *req_queue;
  req_queue = malloc(sizeof(request_queue_t));
  initQueue(req_queue);


  // setting up the threads -------------------------------------------------------

  pthread_t worker_pool[num_workers];
  pthread_t dispatcher_pool[num_dispatcher];

  warg_t w_args;
  darg_t d_args;

  w_args.request_queue = req_queue;
  w_args.cache = req_cache;
  w_args.cache_size = cache_size;

  d_args.request_queue = req_queue;
  d_args.queue_length = queue_length;


  /* if(pthread_mutex_init(&lock,NULL)!=0){
     perror("Failed to mutex init")
     exit(1);
   } */


  // creating worker and dispatcher threads ---------------------------------------

  for(int i = 0 ; i < num_workers; i++ ){
      if(pthread_create(worker_pool[i],NULL,worker,NULL)){
        perror("Error creating worker thread");
        exit(1);
      }
  }


  for(int i = 0 ; i < num_dispatcher; i++ ){
      if(pthread_create(dispatcher_pool[i],NULL,dispatch,NULL)){
        perror("Error creating dispatcher thread");
        exit(1);
      }
  }

  // Clean up ---------------------------------------------------------------------


  return 0;
}
