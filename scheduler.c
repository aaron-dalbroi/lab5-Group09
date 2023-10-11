#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <limits.h>
#include "job.h"
#include <stdio.h>
#include <time.h>

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

/*To be used when no jobs are available at current time. Finds the next job's arrival.
  Returns the arrival time of the next arrivin job and -1 if no jobs are left.*/
int findNextJobsTime(){
  struct job * jobIterator = head;
  int nextJobsTime = -1;

  // Scan through the list and find the the next job's arrival time.
  while(jobIterator != NULL){
    if(jobIterator->executionTimeRemaining > 0){ //I.e., job hasn't been completed yet.
      nextJobsTime = jobIterator->arrival;
      break;
    }
    jobIterator = jobIterator->next;
  }

  return nextJobsTime;
}

/*Finds the job that has shortest time to completion from available, incomplete jobs.
  Returns the pointer to the shortest job or NULL, if no jobs are available.*/
struct job * findShortestJobToCompletion(int timeLimit){
  struct job * jobIterator = head;
  struct job * STTCJob = NULL;

  while(jobIterator != NULL){
    // Pay attention only to jobs that are not completed yet, and have already arrived.
    if(jobIterator->executionTimeRemaining > 0 && jobIterator->arrival <= timeLimit){
      if(STTCJob == NULL) // Any valid job will do to begin with.
        STTCJob = jobIterator;
      else if(jobIterator->executionTimeRemaining < STTCJob->executionTimeRemaining){
        STTCJob = jobIterator;
      }
    }

    jobIterator = jobIterator->next;
  }

  return STTCJob;
}

void policy_STCF(struct job *head, int slice) {
  printf("---Execution trace with STCF:\n");
  int currentTime = 0; // Initialize current time.

  while(1){
    struct job * nextJob = findShortestJobToCompletion(currentTime);

    if(nextJob == NULL){ // No job was found, means there are no jobs available.
      // Find when the next job arrives and skip time to there.
      printf("t=%d processor is idling.\n", currentTime);
      currentTime = findNextJobsTime();

      // If no valid time is found, all jobs are complete.
      if(currentTime == -1)
        break;
    }
    else{
      // If the job was found, run it for slice-time or till completion, whichever is shorter.
      int timeToRun = min(nextJob->executionTimeRemaining, slice);

      // If this job is running for the first time, set execution start time.
      if(nextJob->executionStarted == -1)
        nextJob->executionStarted = currentTime;

      printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n"
              , currentTime, nextJob->id, nextJob->arrival, timeToRun);

      currentTime += timeToRun;
      nextJob->executionTimeRemaining -= timeToRun;

      // If this job is fully complete, set execution end time.
      if(nextJob->executionTimeRemaining == 0)
        nextJob->executionEnded = currentTime;
    }
  }

  printf("---End of execution widht STCF.\n");

  return;
}

void policy_RR(struct job * head, int slice){
  // TODO: Fill this in as well.
}

/*Goes through the job list and adds up the ticket values of all active jobs.*/
int getActiveTickets(struct job * head, int timeLimit){
  struct job * jobIterator = head;
  int totalTickets = 0;

  while(jobIterator != NULL){
    if(jobIterator->arrival <= timeLimit && jobIterator->executionTimeRemaining > 0){
      totalTickets += jobIterator->tickets;
    }

    jobIterator = jobIterator->next;
  }

  return totalTickets;
}

void policy_LT(struct job * head, int slice){
  printf("---Execution trace with LT:\n");
  int currentTime = 0; // Initialize current time.
  srand(time(NULL)); // Seed randomizer.

  while(1){
    printJobs;

    // Find total number of tickets.
    int totalTickets = getActiveTickets(head, currentTime);

    if(totalTickets == 0){ // Total tickets == 0 means that no jobs are available.
      // Find when the next job arrives and skip time to there.
      printf("t=%d processor is idling.\n", currentTime);
      currentTime = findNextJobsTime();

      // If no valid time is found, all jobs are complete.
      if(currentTime == -1)
        break;
    }
    else{
      // Spin the lottery wheel.
      int winningTicket = rand() % (totalTickets + 1);

      // Find which job won the lottery.
      struct job * winner = NULL;
      struct job * jobIterator = head;
      int ticketRange  = 0;
      while(jobIterator != NULL){ // Go through the jobs.
        if(jobIterator->arrival <= currentTime && jobIterator->executionTimeRemaining > 0){ // If the job is active.
          ticketRange += jobIterator->tickets; // Find upper limit of this job's tickets.
          if(winningTicket < ticketRange){ // If ticket belongs to this job.
            winner = jobIterator;
            break;
          }
        }
        jobIterator = jobIterator->next;
      }

      // If the job was found, run it for slice-time or till completion, whichever is shorter.
      int timeToRun = min(winner->executionTimeRemaining, slice);

      // If this job is running for the first time, set execution start time.
      if(winner->executionStarted == -1)
        winner->executionStarted = currentTime;

      printf("t=%d: [Job %d] arrived at [%d], ran for: [%d]\n"
              , currentTime, winner->id, winner->arrival, timeToRun);

      currentTime += timeToRun;
      winner->executionTimeRemaining -= timeToRun;

      // If this job is fully complete, set execution end time.
      if(winner->executionTimeRemaining == 0)
        winner->executionEnded = currentTime;
    }
  }

  printf("---End of execution widht LT.\n");

  return;
}

void analyze(struct job * head){
  float averageResponseTime = 0;
  float averageTurnaroundTime = 0;
  float averageWaitTime = 0;
  int numJobs = 0;
  
  struct job * jobIterator = head;

  // Scan through the list and print statistics for each job.
  while(jobIterator != NULL){
    numJobs++;
    int responseTime = jobIterator->executionStarted - jobIterator->arrival;
    int turnaroundTime = jobIterator->executionEnded - jobIterator->arrival;
    int waitTime = jobIterator->executionEnded - jobIterator->arrival - jobIterator->length;

    averageResponseTime += responseTime;
    averageTurnaroundTime += turnaroundTime;
    averageWaitTime += waitTime;

    printf("Job %d -- Response time: %d Turnaround: %d Wait: %d\n",jobIterator->id , responseTime, turnaroundTime, waitTime);

    jobIterator = jobIterator->next;
  }

  averageResponseTime /= numJobs;
  averageTurnaroundTime /= numJobs;
  averageWaitTime /= numJobs;

  printf("Average -- Response: %.2f Turnaround %.2f Wait %.2f\n", averageResponseTime, averageTurnaroundTime, averageWaitTime);
  
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

  if (strcmp(policy, "STCF") == 0  || strcmp(policy, "stcf") == 0) {
    policy_STCF(head, slice);
    if (analysis) {
      printf("\n---Begin analyzing STCF:\n");
      analyze(head);
      printf("---End analyzing STCF.\n");
    }
    exit(EXIT_SUCCESS);
  }

  if (strcmp(policy, "RR") == 0  || strcmp(policy, "rr") == 0) {
    policy_RR(head, slice);
    if (analysis) {
      printf("\n---Begin analyzing RR:\n");
      analyze(head);
      printf("---End analyzing RR.\n");
    }
    exit(EXIT_SUCCESS);
  }

  if (strcmp(policy, "LT") == 0  || strcmp(policy, "lt") == 0) {
    policy_LT(head, slice);
    if (analysis) {
      printf("\n---Begin analyzing LT:\n");
      analyze(head);
      printf("---End analyzing LT.\n");
    }
    exit(EXIT_SUCCESS);
  }

  freeJobList();

	exit(EXIT_SUCCESS);
}
