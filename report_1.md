# Question 1

## To Run
-> make
-> ./a.out

Each course and student have a separate thread

## Student Thread
First the execution of thread is halted (using sleep) for its registration time.Then after this the students is made available to handleCourse program (student->is_available = 1) which is responsible to allocate course and conduct tut.

studnet receives signal in two cases. If the course is removed , and the student has this course as its current priority , then in that case student is asked to lower its priority or 
the other case in which signal is received is when the student has completed any tut. In that case it is randomly decided that if the student takes that course or not. If yes , then student exits the simulation else it changes the priority to next course.

## Course thread
Course thread first checks if the course is available or not (by checking if there is any ta in the lab list who has not completed its ). Next it first allocate a ta to the course and then the random no of seats is allocated.
Then it loops throught the students list and select students whp have this course as their current priority (less than max slot). It conducts the tut for 2 sec and then signal the students that tut has been completed. It update the ta info.

