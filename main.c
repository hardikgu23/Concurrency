#include "headers.h"

// rng(a,b) gives a random integer between a and b
int rng(int a, int b)
{ // [a, b]
    int dif = b - a + 1;
    int rnd = rand() % dif;
    return (a + rnd);
}

// color code
void red()
{
    printf("\033[1;31m");
}

void yellow()
{
    printf("\033[1;33m");
}

void reset()
{
    printf("\033[0m");
}

void green()
{
    printf("\033[1;32m");
}

void blue()
{
    printf("\033[1;34m");
}

void purple()
{
    printf("\033[1;35m");
}

void handleInput()
{
    scanf("%d %d %d", &no_of_students, &no_of_labs, &no_of_courses);

    // course data  // name , interest , max_slot , no of labs , list of labs
    for (int i = 0; i < no_of_courses; i++)
    {
        scanf("%s %f %d %d", Courses[i].name, &Courses[i].interest, &Courses[i].max_slots, &Courses[i].no_of_labs);

        for (int j = 0; j < Courses[i].no_of_labs; j++)
        {
            scanf("%d", &Courses[i].lab_list[j]);
        }

        Courses[i].course_id = i;
        Courses[i].course_removed = -1;
    }

    // input students dara
    // calibre(float) , pref1 , pref2 , pref3 , entry_time
    for (int i = 0; i < no_of_students; i++)
    {
        scanf("%f %d %d %d %d", &Students[i].calbre, &Students[i].course_pref[0], &Students[i].course_pref[1], &Students[i].course_pref[2], &Students[i].entry_time);
        Students[i].id = i;
        Students[i].has_finalized_course = 0;
        Students[i].current_pref_no = 0;
        Students[i].is_available = -1;
        Students[i].in_a_tut = 0;
        Students[i].course = -1;
    }

    // input labs data
    // name , no_of_ta , max_taship_allowed
    for (int i = 0; i < no_of_labs; i++)
    {
        scanf("%s %d %d", Labs[i].name, &Labs[i].no_of_ta, &Labs[i].taship_limit);

        // initialize the info about the tas in a lab
        for (int j = 0; j < Labs[i].no_of_ta; j++)
        {
            Labs[i].tas[j].id = j;
            Labs[i].tas[j].course_id = -1;
            Labs[i].tas[j].no_of_taship_completed = 0;
            Labs[i].tas[j].taship_ongoing = 0;
            Labs[i].is_available = 1;
            Labs[i].no_of_ta_active = Labs[i].no_of_ta;
        }

        Labs[i].lab_id = i;
    }
}


int main()
{
    srand(time(0));
    handleInput();

    // set locks
    for (int i = 0; i < no_of_students; i++)
    {
        pthread_mutex_init(&mutexStudent[i], NULL);
        pthread_cond_init(&condStudent[i], NULL);
    }

    for (int i = 0; i < no_of_labs; i++)
    {
        for (int j = 0; j < 50; j++)
        {
            pthread_mutex_init(&mutexTas[i][j], NULL);
        }
    }

    // setup threads
    for (int i = 0; i < no_of_students; i++)
        pthread_create(&studentThread[i], NULL, handleStudent, &Students[i]);

    for (int i = 0; i < no_of_courses; i++)
        pthread_create(&courseThread[i], NULL, handleCourse, &Courses[i]);

    // join student thread
    for (int i = 0; i < no_of_students; i++)
        pthread_join(studentThread[i], NULL);

    printf("Simulation Over\n");
    return 0;
}