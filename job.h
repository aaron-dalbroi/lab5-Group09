struct job {
    int id;
    int arrival;
    int length;

    int executionStarted;
    int executionEnded;

    int tickets;

    int executionTimeRemaining;
    struct job *next;
};