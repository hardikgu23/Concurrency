#include "headers.h"

// will initialize the course , then will search for an ta to be assigned // allocate seats // get student // tutorial begin // tut end // ta free and
// student makes choices // repeat
void *handleCourse(void *args)
{
    course *course1 = (course *)args;

    // will store the id's of the student selected for a tutorial
    int students_selected[100];

    red();
    printf("Course %s has been initialized\n", course1->name);
    reset();

    int iter_count = 0;
    int flag = 0;

    while (1)
    {

    check_lab_removal_start:
        iter_count = 0; // check for lab removal after every 10 iterations
        flag = 0;

        // check if course is available
        for (int i = 0; i < course1->no_of_labs; i++)
        {
            lab *lab1 = &Labs[course1->lab_list[i]];
            int max_taship = lab1->taship_limit;

            for (int j = 0; j < lab1->no_of_ta; j++)
            {
                if (lab1->tas[j].no_of_taship_completed < max_taship)
                {
                    flag = 1;
                    break;
                }
            }

            if (flag == 1)
                break;
        }

        if (flag == 0)
        {
            // course as been removed
            red();
            printf("Course %s has been removed from the portal\n", course1->name);

            course1->course_removed = 1;
            reset();

            for (int i = 0; i < no_of_students; i++)
            {
                // pthread_mutex_lock(&mutexStudent[i]);
                if (Students[i].course_pref[Students[i].current_pref_no] == course1->course_id)
                {
                    Students[i].course = course1->course_id;
                    pthread_cond_signal(&condStudent[Students[i].id]);
                }
            }
            return NULL;
        }

        // find a ta for this course from the ta list
        int ta_no = -1;
        int lab_no = -1;
        int course_ta_found = 0;
        ta *ta_selected;

        while (course_ta_found == 0)
        {
            iter_count++;
            if (iter_count > 10)
            {
                goto check_lab_removal_start;
            }

            // iterate through the list of tas
            for (int i = 0; i < course1->no_of_labs; i++)
            {

                lab *lab1 = &Labs[course1->lab_list[i]];

                {

                    red();
                    // printf("Start data\n\n");

                    for (int j = 0; j < lab1->no_of_ta; j++)
                    {

                        ta *ta1 = &lab1->tas[j];
                        if ((ta1->taship_ongoing == 0) && (ta1->no_of_taship_completed < lab1->taship_limit))
                        {
                            // printf("Course %s lab_name: %s ta_id: %d before\n", course1->name, lab1->name, ta1->id);

                            if (pthread_mutex_trylock(&mutexTas[lab1->lab_id][j]) != 0)
                            {
                                continue;
                            }

                            course_ta_found = 1;
                            ta1->taship_ongoing = 1;
                            ta1->no_of_taship_completed++;
                            ta1->course_id = course1->course_id;
                            ta_selected = ta1;
                            ta_no = ta1->id;
                            lab_no = lab1->lab_id;
                            break;
                        }
                    }

                    red();
                }

                if (course_ta_found == 1)
                    break;
            }
        }

        if (course_ta_found == 1)
        {
            // assign the ta to this course
            green();
            printf("Course %s has been allocated TA %d from lab %s\n", course1->name, ta_no, Labs[lab_no].name);
            yellow();
            printf("TA %d from lab %s has been allocated to course %s for his %d taship\n", ta_no, Labs[lab_no].name, course1->name, ta_selected->no_of_taship_completed);
            reset();

            // allocate random number of seats between 1 and max_slot for this course
            int slots = rng(1, course1->max_slots);

            green();
            printf("Course %s has been allocated %d seats\n", course1->name, slots);
            reset();

            int num_student_selected = 0;

            while (num_student_selected == 0)
            {

                // select students
                for (int i = 0; i < no_of_students; i++)
                {

                    if ((Students[i].is_available != -1) && (Students[i].course_pref[Students[i].current_pref_no] == course1->course_id) && (Students[i].in_a_tut == 0))
                    {
                        pthread_mutex_lock(&mutexStudent[i]);
                        Students[i].in_a_tut = 1;
                        students_selected[num_student_selected] = i;
                        Students[i].course = course1->course_id;
                        num_student_selected++;

                        green();
                        printf("Student %d has been allocated course %s\n", i, course1->name);
                        reset();
                    }

                    if (num_student_selected >= slots)
                        break;

                }

                if (num_student_selected == 0)
                {
                    // make this thread go for sleep for one sec before checking again
                    sleep(1);
                }
            }

            // start the tutorial // go to sleep for 2 sec
            purple();
            printf("Tutorial has started for course %s with %d seats fulled out of %d\n", course1->name, num_student_selected, slots);
            sleep(2);

            // tutorial has ended
            purple();
            printf("ta %d from lab %s has completed tut for course %s\n", ta_no, Labs[lab_no].name, course1->name);
            reset();

            // signal all the selected students . that tut has ended and they can update their choices
            for (int i = 0; i < num_student_selected; i++)
            {
                pthread_mutex_unlock(&mutexStudent[students_selected[i]]);
                pthread_cond_signal(&condStudent[students_selected[i]]);
            }

            ta_selected->taship_ongoing = 0;

            // checking for ta removal
            int max_taship = Labs[lab_no].taship_limit;
            int lab_flag = 0;

            for (int i = 0; i < Labs[lab_no].no_of_ta; i++)
            {
                if (Labs[lab_no].tas[i].no_of_taship_completed < max_taship)
                {
                    lab_flag = 1;
                }
            }

            if (lab_flag == 0 && Labs[lab_no].is_available != -1)
            {
                green();
                printf("Lab %s has been removed\n", Labs[lab_no].name);
                reset();
                Labs[lab_no].is_available = -1;
            }

            pthread_mutex_unlock(&mutexTas[lab_no][ta_selected->id]);
        }
    }
}