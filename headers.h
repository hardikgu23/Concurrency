#ifndef _headers
#define  _headers

#include <assert.h>
#include <errno.h>
#include <math.h>
#include <memory.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <sched.h>

#define MAX_STUDENT 500
#define MAX_COURSE 50
#define MAX_LABS 50
#define MAX_TA_IN_LAB 50
#define MAX_LABS_IN_COURSE 50

// student
typedef struct student
{
    int id;   
    float calbre;
    int entry_time;
    int course_pref[3]; 
    int has_finalized_course;  // if 0 , it has not finalized else yes , it has finalalized
    int is_available;  // will tell if student has filled its choices or not?
    int current_pref_no;  // tell which priority is it at this moment
    int in_a_tut;         // will tell if a student is in a tut or not?
    int course;           // tells which course he is planning to take // -1 means no signal was not sent from any course 
}student;

// course
typedef struct course
{
    int course_id;
    char name[256];
    float interest;
    int max_slots;
    int no_of_labs;       
    int lab_list[100];    // assuming course do not take tas from more than 10 lasb
    int course_removed;   // if 1 that means course has been removed from the portal
    // int no_of_student_interested;

}course;

typedef struct ta
{
    int id;
    int course_id;                    // stores the id of the course it is taking 
    int no_of_taship_completed;       // stores how many taship it has completed
    int taship_ongoing;               // 0-> ta is idle // 1-> he is taking tut for course[course_id] 
}ta;

// labs -> max 50 ta's are allowed
typedef struct lab{
    int lab_id;
    char name[256];
    int taship_limit;
    int no_of_ta;            // stores how many ta lab has  // ids are 0 indexed
    ta tas[51];              // stores the info about the tas in a lab
    int is_available;        // if 1 -> means ta some ta is there who has not completed its quota else 0
    int no_of_ta_active;           // stores how many tas are there who has not completed their quota
}lab;

int no_of_students , no_of_courses , no_of_labs;

student Students[MAX_STUDENT];
pthread_t studentThread[MAX_STUDENT];
pthread_mutex_t mutexStudent[MAX_STUDENT];
pthread_cond_t condStudent[MAX_STUDENT];

course Courses[MAX_COURSE];
pthread_t courseThread[MAX_COURSE];
// pthread_mutex_t mutexCourse[MAX_COURSE];

lab Labs[MAX_LABS];
pthread_t labThread[MAX_LABS];
pthread_mutex_t mutexTas[MAX_LABS][MAX_TA_IN_LAB];
// pthread_cond_t condLabs[MAX_LABS];

// function definition
void* handleStudent(void* );
void* handleCourse(void* );

// functions to print in diff color
void red () ;
void yellow () ;
void reset ();
void green ();
void blue ();
void purple();

int rng(int , int);

#endif 