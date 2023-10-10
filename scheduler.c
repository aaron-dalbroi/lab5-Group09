#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include "job.h"

//This is the start of our linked list of jobs, i.e., the job list
struct job *head = NULL;

/*Function to print out the list of jobs and currently available information.*/
const void printJobs(){
  struct job * someJob = head;
  printf("\nJob List:\n");
  while(someJob != NULL){
    printf("Job %d: arr %d, len %d, %d >>> %d, tick.: %d, ETR: %d.\n",
    someJob->id, someJob->arrival, someJob->length, someJob->executionStarted, someJob->executionEnded, someJob->tickets, someJob->executionTimeRemaining);
    someJob = someJob->next;
  }
  printf("\n");
}

/*Returns smaller of two ints.*/
int min(int a, int b){
  if(a < b)
    return a;
  else
    return b;
}

/*Returns larger of two ints.*/
int max(int a, int b){
  if(a > b)
    return a;
  else
    return b;
}

/*Function to append a new job to the list*/
void append(int id, int arrival, int length, int tickets){
  // create a new struct and initialize it with the input data
  struct job *tmp = (struct job*) malloc(sizeof(struct job));

  //tmp->id = numofjobs++;
  tmp->id = id;
  tmp->length = length;
  tmp->arrival = arrival;
  tmp->tickets = tickets;

  tmp->executionStarted = -1;
  tmp->executionEnded = -1;
  tmp->executionTimeRemaining = length;

  // the new job is the last job
  tmp->next = NULL;

  // Case: job is first to be added, linked list is empty 
  if (head == NULL){
    head = tmp;
    return;
  }

  struct job *prev = head;

  //Find end of list 
  while (prev->next != NULL){
    prev = prev->next;
  }

  //Add job to end of list 
  prev->next = tmp;
  return;
}

/*Function to read in the workload file and create job list*/
void read_workload_file(char* filename) {
  int id = 0;
  FILE *fp;
  size_t len = 0;
  ssize_t read;
  char *line = NULL,
       *arrival = NULL, 
       *length = NULL;
  int tickets = 0;

  struct job **head_ptr = malloc(sizeof(struct job*));

  if( (fp = fopen(filename, "r")) == NULL)
    exit(EXIT_FAILURE);

  while ((read = getline(&line, &len, fp)) > 1) {
    arrival = strtok(line, ",\n");
    length = strtok(NULL, ",\n");
    tickets += 100;
       
    // Make sure neither arrival nor length are null. 
    assert(arrival != NULL && length != NULL);
        
    append(id++, atoi(arrival), atoi(length), tickets);
  }

  fclose(fp);

  // Make sure we read in at least one job
  assert(id > 0);

  return;
}

/*To be used when no jobs are available at current time. Finds the next job's arrival.*/
int findNextJobsTime(){
  struct job * jobIterator = head;
  int nextJobsTime = -1;

  // Scan through the list and find the the next job's arrival time.
  while(jobIterator != NULL){
    if(jobIterator->executionStarted == -1){ //I.e., job hasn't been scheduled yet.
      nextJobsTime = jobIterator->arrival;
      break;
    }
    jobIterator = jobIterator->next;
  }

  return nextJobsTime;
}

/*Finds the job that has shortest time to completion from available, incomplete jobs. */
struct job * findShortestJobToCompletion(int timeLimit){
  struct job * jobIterator = head;
  struct job * STTCJob = NULL;

  while(jobIterator != NULL){


    jobIterator = jobIterator->next;
  }

  return STTCJob;
}

void policy_STCF(struct job *head, int slice) {
  

  return;
}

void policy_RR(struct job * head, int slice){
  // TODO: Fill this in as well.
}

void policy_LT(struct job * head, int slice){
  // TODO: Fill this in as well.
}

void analyze(struct job * head){
  float averageResponseTime = 0;
  float averageTurnaroundTime = 0;
  int numJobs = 0;
  
  struct job * jobIterator = head;

  // Scan through the list and print statistics for each job.
  while(jobIterator != NULL){
    numJobs++;
    int responseTime = jobIterator->executionStarted - jobIterator->arrival;
    int turnaroundTime = jobIterator->executionEnded - jobIterator->arrival;

    averageResponseTime += responseTime;
    averageTurnaroundTime += turnaroundTime;

    printf("Job %d -- Response time: %d Turnaround: %d Wait: %d\n",jobIterator->id , responseTime, turnaroundTime, responseTime);

    jobIterator = jobIterator->next;
  }

  averageResponseTime /= numJobs;
  averageTurnaroundTime /= numJobs;

  printf("Average -- Response: %.2f Turnaround %.2f Wait %.2f\n", averageResponseTime, averageTurnaroundTime, averageResponseTime);
  
  return;
}

void freeJobList(){
  struct job * jobIterator = head;

  while(jobIterator != NULL){
    struct job * doomedJob = jobIterator;
    jobIterator = jobIterator->next;
    free(doomedJob);
  }
}

int main(int argc, char **argv) {

 if (argc < 5) {
    fprintf(stderr, "missing variables\n");
    fprintf(stderr, "usage: %s analysis-flag policy workload-file slice-length\n", argv[0]);
		exit(EXIT_FAILURE);
  }

  int analysis = atoi(argv[1]);
  char *policy = argv[2],
       *workload = argv[3];
  int slice = atoi(argv[4]);

  // Note: we use a global variable to point to 
  // the start of a linked-list of jobs, i.e., the job list 
  read_workload_file(workload);

  if (strcmp(policy, "STCF") == 0  || strcmp(policy, "stct") == 0) {
    policy_STCF(head, slice);
    if (analysis) {
      printf("Begin analyzing STCF:\n");
      analyze(head);
      printf("End analyzing STCF.\n");
    }
    exit(EXIT_SUCCESS);
  }

  if (strcmp(policy, "RR") == 0  || strcmp(policy, "rr") == 0) {
    policy_RR(head, slice);
    if (analysis) {
      printf("Begin analyzing RR:\n");
      analyze(head);
      printf("End analyzing RR.\n");
    }
    exit(EXIT_SUCCESS);
  }

  if (strcmp(policy, "LT") == 0  || strcmp(policy, "lt") == 0) {
    policy_RR(head, slice);
    if (analysis) {
      printf("Begin analyzing LT:\n");
      analyze(head);
      printf("End analyzing LT.\n");
    }
    exit(EXIT_SUCCESS);
  }

  freeJobList();

	exit(EXIT_SUCCESS);
}
