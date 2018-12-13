/* Csci4061 Fall 2018 Project 3
* Name: John Hedlund-Fay, Zaffer Hussein, Chris Pieper
* X500: hedlu131, husse147, piep072 */

Date: 12/12/2018

This program is a multi-threaded web server made to be able to use POSIX threads to handle files of a variety of types. Using dispatcher threads and worker threads we accept incoming connections from clients and place requests into a queue (implemented as an array). The worker threads retrieve requests from the queue and fulfill them. Our cache replacement policy is random replacement.

This program can be compiled directly using make as degined in the Makefile provided in the tar.gz. The program can be run using ./web_server port_number (an int from 1025 to 65535 excluding 9000) testing_directory num_dispatchers num_workers dynamic_flag request_queue_size cache_size. Requests are logged in web_server_log.

John Hedlund-Fay was responsible for creating and sharing the github repository used for the project, creating the readme, and working to fix compilation and runtime errors during the programming process as well as implementing and fixing the structs and some of the helper functions for queue and cache operations. Zaffer Hussein was responsible for reimplementing the queue as an array when the implementation using a doubly-linked list was causing segmentation faults as well as much of the initial function and struct implementations. Chris Pieper implemented all of the functions involving logging requests to web_server_log as well as helping to fix problems with the functions and struct implementation during the programming process.
