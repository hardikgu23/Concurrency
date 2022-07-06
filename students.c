#include "headers.h"

// one thread per student
void *handleStudent(void *args)
{
    student *student1 = (student *)args;
    int id = student1->id;
    int t = student1->entry_time;
    sleep(t);

    blue();
    printf("Students %d has filled its course preference\n", student1->id);
    reset();

    student1->is_available = 1;

    while (1)
    {
        pthread_mutex_lock(&mutexStudent[id]);
start:
        student1->course = -1;
        student1->in_a_tut =0;
        // wait for a signal from course to indicate that a tutorial has been completed
        if(Courses[student1->course_pref[student1->current_pref_no]].course_removed == -1)
        {
            pthread_cond_wait(&condStudent[id], &mutexStudent[id]);
        }

        student1->in_a_tut = 0;

        // check if signal was because course has been removed
        // course has been removed
        // move to the next course which is available
        if (Courses[student1->course].course_removed == 1)
        {
            int pref = student1->current_pref_no;

            // check which one is possible
            if ((pref + 1 < 3) && (Courses[student1->course_pref[pref + 1]].course_removed == -1))
            {
                student1->current_pref_no++;
                pref = student1->current_pref_no;

                red();
                printf("Student id %d has changed its preference from %s (priority %d) to %s (priority %d)\n", student1->id, Courses[student1->course].name, pref - 1, Courses[student1->course_pref[pref]].name, pref);
                reset();
                // pthread_mutex_unlock(&mutexStudent[id]);
                goto start;
            }
            else if ((pref + 2 < 3) && (Courses[student1->course_pref[pref + 2]].course_removed == -1))
            {
                student1->current_pref_no += 2;
                pref = student1->current_pref_no;

                red();
                printf("Student id %d has changed its preference from %s (priority %d) to %s (priority %d)\n", student1->id, Courses[student1->course].name, pref - 2, Courses[student1->course_pref[pref]].name, pref);
                reset();
                // pthread_mutex_unlock(&mutexStudent[id]);
                goto start;
            }
            else
            {
                red();
                printf("Student %d has exited the simulation\n", student1->id);
                reset();
                student1->is_available = -1;
                pthread_mutex_unlock(&mutexStudent[id]);
                return NULL;
            }
        }

        // student has completed the tut of some course
        student1->in_a_tut = 0;

        // Deciding whether student selects or not
        float value = ((float)rand() * 1.0) / RAND_MAX;

        int pref_no = student1->current_pref_no;

        int course_no = student1->course_pref[pref_no];
        course *course1 = &Courses[course_no];

        float probability = (student1->calbre) * (course1->interest) * 1.0;

        if (value <= probability) // select this course
        {
            purple();
            printf("Student %d has selected the %s permanently\n", id, course1->name);
            Students[id].has_finalized_course = 1;
            reset();
            student1->is_available = -1;
            pthread_mutex_unlock(&mutexStudent[id]);
            return NULL;
        }
        else // change priority if this is not the third course or else exit the simulation
        {
            purple();
            printf("Student %d has withdrawn from the %s course\n", id, course1->name);
            reset();
            
            int pref = student1->current_pref_no;
            
            if ( (pref + 1 < 3) && (Courses[student1->course_pref[pref + 1]].course_removed == -1))
            {
                student1->current_pref_no++;
                pref = student1->current_pref_no;

                red();
                printf("Student id %d has changed its preference from %s (priority %d) to %s (priority %d)\n", student1->id, Courses[student1->course].name, pref - 1, Courses[student1->course_pref[pref]].name, pref);
                reset();
                // pthread_mutex_unlock(&mutexStudent[id]);
                goto start;
            }
            else if ((pref + 2 < 3) && (Courses[student1->course_pref[pref + 2]].course_removed == -1))
            {
                student1->current_pref_no += 2;
                pref = student1->current_pref_no;

                red();
                printf("Student id %d has changed its preference from %s (priority %d) to %s (priority %d)\n", student1->id, Courses[student1->course].name, pref - 2, Courses[student1->course_pref[pref]].name, pref);
                reset();
                // pthread_mutex_unlock(&mutexStudent[id]);
                goto start;
            }
            else
            {
                red();
                student1->is_available = -1;
                printf("Student %d has exited the simulation\n", student1->id);
                reset();
                pthread_mutex_unlock(&mutexStudent[id]);
                return NULL;
            }
        }
    }
}