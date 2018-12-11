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

#define MAX_THREADS 100
#define MAX_queue_len 100
#define MAX_CE 100
#define INVALID -1
#define BUFF_SIZE 1024

/*
  THE CODE STRUCTURE GIVEN BELOW IS JUST A SUGESSTION. FEEL FREE TO MODIFY AS NEEDED
*/

// structs:
struct request_node
{
    int fd;
    char *filename;
    struct node *next;
};
typedef struct node node;

struct queue
{
    int count;
    node *front;
    node *rear;
};
typedef struct queue queue;

/*typedef struct request_queue {
   int fd;
   void *request-;
} request_t;
*/
typedef struct cache_entry {
    int len;
    char *request;
    char *content;
    int status;
} cache_entry_t;


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

// Function to check whether the given request is present in cache
int getCacheIndex(char *filename,cache_entry_t * cache){
  /// return the index if the request is present in the cache
  for(int i = 0; i< cache_size ;i++){
    if(strcmp(filename,cache[i].filename)){
      return i;
    }
  }

  return -1

}

// Function to add the request and its file content into the cache
void addIntoCache(char *mybuf, char *memory , int memory_size){
  // It should add the request at an index according to the cache replacement policy
  // Make sure to allocate/free memeory when adding or replacing cache entries
}

// clear the memory allocated to the cache
void deleteCache(char *cache){
  free(cache);
}

// Function to initialize the cache
void initQueue(queue *q)
{
    q->count = 0;
    q->front = NULL;
    q->rear = NULL;
}

void initCache(char *cache){

  // Allocating memory and initializing the cache array
  cache_entry_t cache[cache_size];
  memset(cache,0,cache_size * sizeof(cache_entry_t));

}

void enqueue(queue *q,char *name, int descriptor, void *req)
{
    node *tmp;
    tmp = malloc(sizeof(node));
    tmp->fd = descriptor;
    tmp->filename = filename;
    tmp->next = NULL;
    if(!isempty(q))
    {
        q->rear->next = tmp;
        q->rear = tmp;
    }
    else
    {
        q->front = q->rear = tmp;
    }
    q->count++;
}

node* dequeue(queue *q)
{
    node *tmp;
    int n = q->front->data;
    tmp = q->front;
    q->front = q->front->next;
    q->count--;
    //free(tmp);
    return tmp;
}
// Function to open and read the file from the disk into the memory
// Add necessary arguments as needed
int readFromDisk(/*necessary arguments*/) {
  // Open and read the contents of file given the request
}

/**********************************************************************************/

/* ************************************ Utilities ********************************/
// Function to get the content type from the request
char* getContentType(char * mybuf) {
  // Should return the content type based on the file type in the request
  // (See Section 5 in Project description for more details)
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
void * dispatch(queue *request_queue,int queue_length) {

  while (1) {

    //Modifying parameters later - C.P.

    // Accept client connection
  int connection_fd;
  char filename[BUFF_SIZE];

  if(pthread_mutex_lock(&lock) == -1){
    printf("Failed to lock thread before accept_connection() call");
    exit(1);
  }

	  connection_fd = accept_connection(void); //added by C.P.

  if(pthread_mutex_unlock(&lock) == -1){
    printf("Failed to unlock thread after accept_connection() call");
    exit(1);
  }
    // Get request from the client
  if(connection_fd >= 0){

	 if(get_request(connection_fd,filename)!=0){
     printf("Error getting request from fd: %d",connection_fd);
   }
   else{
        //check if queue is full
        if(request_queue.count < queue_length)

            enqueue(request_queue,connection_fd,filename)
        else{
                 // QUEUE REPLACEMENT POLICY HANDLED HERE
        }
     //filename now contains name of file
     // Add the request into the queue

   }



  }


   }
   return NULL;
}


/**********************************************************************************/

// Function to retrieve the request from the queue, process it and then return a result to the client
void * worker(queue *request_queue,cache_entry_t * cache, int cache_size){

  while (1) {

	time_t start_time, end_time;
	double time_taken;

  char content_type[BUFF_SIZE];
  char content_buffer[BUFF_SIZE];
  cache_entry_t cache_entry;
  node request;
  int bytesread;
  int index;

    // Start recording time, added by C.P.
	if((start_time=time(NULL))==((time_t)-1)){
		start_time=-1;
	}

    // Get the request from the queue


    request = dequeue(request_queue)
    index = getCacheIndex(request.filename,cache,cache_size);


    if(index == -1){
        while(bytesread = read(request.fd,content_buffer,BUFF_SIZE) > 0){

        }

        if //use a pointer to keep track where we are in the cache. if pointer is at last index. go to front and replace LIFO
          //use cache[i].status to check if spot is occupied

      //after read from file, make cache entry. Use replacement policy if cache is full
    }
    else{

      cache_entry = cache[index]
      //request is in the cache
      //get contents from cache index and return
    }



    // Stop recording the time, added by C.P.
	if((end_time=time(NULL))==((time_t)-1)){
		end_time=-1;
	}

	//total time taken to get request and data, added by C.P.
	time_taken=difftime(start_time,end_time);

    // Log the request into the file and terminal

    // return the result (modify parameters later)
	if(return_result(int fd, char *content_type, char *buf, int numbytes))!=0){
		return_error(int fd, char *buf);
	} // added by C.P.
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
	port=argv[1];
	path=argv[2];
	num_dispatcher=argv[3];
	num_workers=argv[4];
	dynamic_flag=argv[5];
	queue_length=argv[6];
	cache_size=argv[7];







  // Perform error checks on the input arguments (added by C.P.)
	if((port<1024)||(port==9000)||(port>65536)){
		printf("Please pick a random port number other than 9000 from (1024 to 65536)");
		return -1;
	}
	//Here, check is path is a valid path / file

	if((num_dispatcher>MAX_THREADS)||(num_dispatcher<=0)){
		printf("Invalid dispatcher thread number. Please pick a number between 1 and 100")
		return -1;
	}
	if((num_workers>MAX_THREADS)||(num_workers<=0)){
		printf("Invalid worker thread number. Please pick a number between 1 and 100")
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
  cache_entry_t cache[cache_size];
  initCache(cache);

  // initialize queue
  queue *request_queue;
  request_queue = malloc(sizeof(queue));
  initQueue(request_queue);


  // setting up the threads -------------------------------------------------------

  pthread_t worker_pool[num_workers];
  pthread_t dispatcher_pool[num_dispatcher];



  if(pthread_mutex_init(&lock,NULL)!=0){
     perror("Failed to mutex init")
     exit(1);
   }


  // creating worker and dispatcher threads ---------------------------------------

  for(i = 0 ; i < num_workers; i++ ){
      if(pthread_create(worker_pool[i],NULL,worker,NULL)){
        perror("Error creating worker thread");
        exit(1);
      }


  for(i = 0 ; i < num_dispatcher; i++ ){
      if(pthread_create(dispatcher_pool[i],NULL,dispatch,NULL)){
        perror("Error creating dispatcher thread");
        exit(1);
      }

  // Clean up ---------------------------------------------------------------------


  return 0;
}
