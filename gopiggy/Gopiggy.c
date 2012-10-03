/*
 ============================================================================
 Name        : gopiggy.c
 Author      : Murphy Meng
 Version     :
 Copyright   : Free
 Description : Pigs racing, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <ncurses.h>
#include <semaphore.h>
#ifdef __unix__
# include <unistd.h>
#elif defined _WIN32
# include <windows.h>
#define sleep(x) Sleep(1000 * x)
#endif

#include "Gopiggy.h"

int f_start_time = -1;
piggy_t f_pigs[MAX_NUM_PIGS];
int f_ranked_pigs[MAX_NUM_PIGS];
int f_num_pigs;
bool f_apple_released = false;
bool f_game_started = false;
pthread_mutex_t   apple_locker = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t    apple_release = PTHREAD_COND_INITIALIZER;
sem_t sem_game_start;
pthread_mutex_t  window_refresh_lock = PTHREAD_MUTEX_INITIALIZER;

char * pig_names[MAX_NUM_PIGS] =
{
		"TOMY",
		"JERY",
		"JECK",
		"CRIS",
		"DAVE",
		"ALEX",
		"PAUL",
		"JOHN",
		"RICK",
		"HENS"
};

int f_apple_locations[] = {100, 200, 300, 400, 500};
int f_apple_index_available = 0;
int diag_line_count = 0;

void * feeder (void *args);
void * user_interface(void *q);
void * pig (void *arg);

void fgets_convert(char * str, char * str1);
void piggy_assign_names(char * player_name);
void rank_piggy();
int try_catch_apple();
void rank_piggy_final();

int main()
{
    int status = EXIT_FAILURE;
    pthread_t interface_id, feeder_id, pig_id;

    status = pthread_create (&interface_id, NULL, user_interface, NULL);
    if (EXIT_SUCCESS != status)
    {
        printf("Error - Fail to create thread.\n");
    }

    status = pthread_create (&feeder_id, NULL, feeder, NULL);
    if (EXIT_SUCCESS != status)
    {
        printf("Error - Fail to create thread.\n");
    }

	status = pthread_create (&pig_id, NULL, pig, NULL);
	if (EXIT_SUCCESS != status)
	{
		printf("Error - Fail to create thread.\n");
	}

    pthread_exit(NULL);

    return EXIT_SUCCESS;
}

void * user_interface(void *q)
{
    char line[100];
    int i, index;
	int height = 6;
	int width = 40;
	int starty;
	int startx;
	int int_value;
	WINDOW *login_window, *info_window, *help_window;

	initscr();
	raw();
	cbreak();
	keypad(stdscr, TRUE);

    box(stdscr, 0, 0);

    attron(A_BOLD);
    mvaddstr(1, COLS/2-6, "Gopiggy! 0.2");
    refresh();
    attroff(A_BOLD);

    sleep(1);
	starty = (LINES - height) / 2;	/* Calculating for a center placement */
	startx = (COLS - width) / 2;	/* of the window		*/
    login_window = newwin(height, width, starty, startx);
    box(login_window, 0, 0);
    wrefresh(login_window);

    mvwprintw(login_window, 1, 2, "Num of piggy in a race (max: 10): ");
    f_num_pigs = wgetch(login_window) - 48;
    wrefresh(login_window);
    mvwprintw(login_window, 2, 2, "Name of your piggy: ");
    wgetstr(login_window, line);

	piggy_assign_names(line);

    sleep(1);

    for (i = 0; i < height; i++)
    {
    	for (index = 0; index < width; index++)
    	{
    		mvwprintw(login_window, i, index, "	");/*It's not a space but a tab. */
    	}
    }
    wrefresh(login_window);
    delwin(login_window);

    info_window = newwin(f_num_pigs+2, 50, LINES-(f_num_pigs+2), 2);
    wprintw(info_window, "Rank - Name - Position - Speed - Status");
	for (i = 0; i < f_num_pigs; ++i)
	{
		mvwprintw(info_window, i+1, 0, "%c      %s      %d         %d     %s", ' ', f_pigs[i].name, f_pigs[i].current_position, f_pigs[i].current_speed, "Ready");
	}
    wrefresh(info_window);

    help_window = newwin(3, 36, LINES-5, COLS-37);
    wprintw(help_window, "What's happening?");
    mvwprintw(help_window, 0, 0, "GoPiggy! Copyright@Murphy Meng 2012");
    mvwprintw(help_window, 1, 0, "    'g' - go piggy!");
    mvwprintw(help_window, 2, 0, "    'q' - quit the game.");
    wrefresh(help_window);

    noecho();

    while(1)
    {
    	if (true != f_game_started)
    	{
			int_value = getch();
			if ('q' == int_value)
			{
				break;
			}
			else if ('g' == int_value)
			{
				(void)sem_post(&sem_game_start);
				f_game_started = true;
			}
    		else
    		{
				/* Do nothing. */
    		}
    	}
    	else
    	{
    		rank_piggy();

    		pthread_mutex_lock(&window_refresh_lock);
    		for (i = 0; i < f_num_pigs; i++)
    		{
    			index = f_ranked_pigs[i];
    			mvwprintw(info_window, i+1, 0, " %d     %s       %d      %d     %s", i, f_pigs[index].name, f_pigs[index].current_position, f_pigs[index].current_speed, f_pigs[index].status);
    		}
    		wrefresh(info_window);

    		pthread_mutex_unlock(&window_refresh_lock);
    	}

    	sleep(1);
    }

    getch();
    endwin();
    pthread_exit(NULL);
}

void * pig (void *arg)
{
    int current_position = 0, speed;
    int i = 0;
    int time_interval;
    int num_piggy_finished = 0;
    int piggy_apple_catcher, index_apple_catcher = -1;
    int speed_cap[MAX_NUM_PIGS];
    int apple_bonus = 0;
    int index;

    srand(time(NULL));

    (void)sem_wait(&sem_game_start);

    for (i = 0; i < f_num_pigs; i++)
    {
    	/* Make the speed cap 4-6*/
    	speed_cap[i] = 4 + rand()%3;
    }
	f_start_time = time(NULL);

	for (i = 0; i < f_num_pigs; i++)
	{
		f_pigs[i].status = "Running";
	}

    do
    {
    	time_interval = time(NULL) - f_start_time;
    	for (i = 0; i < f_num_pigs; i++)
    	{
    		if (TRACK_LENGTH != f_pigs[i].current_position)
    		{
    			if ((index_apple_catcher != i) || (0 == apple_bonus))
    			{
    				speed = MIN_SPEED + rand()%(speed_cap[i]);
    			}
    			else
    			{
    				/* The current piggy just got the apple. It should be awarded! This lucky piggy shall run at max speed for 3sec.*/
    				if (apple_bonus > 0)
    				{
    					apple_bonus--;
    				}
    				else
    				{
    					printf("Error - Shound never happen.\n");
    				}
					speed = MAX_SPEED;
					pthread_mutex_lock(&window_refresh_lock);
					mvprintw((diag_line_count++)%(LINES -7)+2, (COLS/2 - 15), "%s runs as max speed %dyard/s for 3sec", f_pigs[i].name, speed);
					refresh();
					pthread_mutex_unlock(&window_refresh_lock);

    			}
				/* The position is where the piggy should be after 1 sec. */
				current_position = f_pigs[i].current_position;
				current_position += speed;

				if (current_position >= TRACK_LENGTH)
				{
					f_pigs[i].current_position = TRACK_LENGTH;
					f_pigs[i].time_period = time(NULL) - f_start_time + 1;
					f_pigs[i].status = "Finished";
					f_pigs[i].average_speed = TRACK_LENGTH/f_pigs[i].time_period;
					num_piggy_finished++;
				}
				else
				{
					f_pigs[i].current_speed = speed;
					f_pigs[i].current_position = current_position;
					/*f_pigs[i].average_speed = current_position/time_interval;*/
				}
    		}
    		else if (0 == f_pigs[i].average_speed)
    		{
    			f_pigs[i].average_speed = TRACK_LENGTH/time_interval;
    		}
    		else
    		{
    			/* Do nothing. */
    		}
    	}

		rank_piggy();

    	if (f_apple_index_available < 5)
    	{
    		piggy_apple_catcher = try_catch_apple();
    	}

    	if (-1 != piggy_apple_catcher)
    	{
    		index_apple_catcher = piggy_apple_catcher;
    		apple_bonus = 3;
    	}

    	sleep(1);
    }while(num_piggy_finished < f_num_pigs);

    rank_piggy_final();
	pthread_mutex_lock(&window_refresh_lock);
	mvprintw((diag_line_count++)%(LINES -7)+2, (COLS/2 - 15), "Rank - Name - average speed - Apples caught");
	for (i = 0; i < f_num_pigs; ++i)
	{
		index = f_ranked_pigs[i];
		mvprintw((diag_line_count++)%(LINES -7)+2, (COLS/2 - 15), "  %d    %s       %d             %d", i+1, f_pigs[index].name, f_pigs[index].average_speed, f_pigs[index].num_apples_caught);
	}

	f_game_started = false;

    pthread_exit(NULL);
}

void * feeder (void *q)
{
	struct timespec target_timeout;
	int status = EXIT_FAILURE;
	bool apple_dropped = false;

	while (f_apple_index_available < 5)
	{
		/* Don't release apple untill it's time. */
		pthread_mutex_lock(&apple_locker);
		f_apple_released = false;
		while (false == f_apple_released)
		{
			status = pthread_cond_wait(&apple_release, &apple_locker);
			if (0 != status)
			{
				printf("Error - pthread_cond_timewait failed.\n");
			}
			else
			{
				/* Do nothing.  */
			}
		}
		pthread_mutex_unlock(&apple_locker);

		/* An apple is released at this moment, i.e. time(NULL). */
		target_timeout.tv_sec = time(NULL) + APPLE_DROP_TIMEOUT;
		target_timeout.tv_nsec = 0;

		pthread_mutex_lock(&apple_locker);
		while (true == f_apple_released)
		{
			status = pthread_cond_timedwait(&apple_release, &apple_locker, &target_timeout);
			if (ETIMEDOUT == status)
			{
				apple_dropped = true;
				break;
			}
			else if (0 != status)
			{
				printf("Error - pthread_cond_timewait failed.\n");
			}
			else
			{
				/* Do nothing.  */
			}
		}
		pthread_mutex_unlock(&apple_locker);

		if (true == apple_dropped)
		{
			pthread_mutex_lock(&window_refresh_lock);
			mvprintw((diag_line_count++)%(LINES -7)+2, (COLS/2 - 15), "Shame! No piggy got the apple %d", f_apple_index_available+1);
			refresh();
			pthread_mutex_unlock(&window_refresh_lock);

/*			sleep(APPLE_SHOWUP_INTERVAL);*/
			f_apple_released = false;
		}
		else if (false == f_apple_released)
		{
#if 0
			printf("Info - Let's get a new apple ready after %ld sec.\n", target_timeout.tv_sec - time(NULL) + APPLE_SHOWUP_INTERVAL);
			sleep(target_timeout.tv_sec - time(NULL) + APPLE_SHOWUP_INTERVAL);
#endif
		}
		else
		{
			/* Only gets into this branch when pthread_cond_timedwait() failed. */
			printf("Error - should not gets here.\n");
		}

		f_apple_index_available++;
		apple_dropped = false;
	}

	f_apple_released = false;

	pthread_exit(NULL);
}

int try_catch_apple()
{
	static int apple_fall_count;
	int distance_to_apple = 0;
	int i;
	int apple_catcher = -1;

	if (true != f_apple_released)
	{
		distance_to_apple = f_apple_locations[f_apple_index_available] - f_pigs[f_ranked_pigs[0]].current_position;
		if ((distance_to_apple < 25) && (distance_to_apple > 0))
		{
			(void)pthread_mutex_lock(&apple_locker);
			f_apple_released = true;
			(void)pthread_cond_signal(&apple_release);
			(void)pthread_mutex_unlock(&apple_locker);

			pthread_mutex_lock(&window_refresh_lock);
			mvprintw((diag_line_count++)%(LINES -7)+2, (COLS/2 - 15), "Apple %d released!", f_apple_index_available+1);
			refresh();
			pthread_mutex_unlock(&window_refresh_lock);

			apple_fall_count = 0;
		}
	}
	else
	{
		apple_fall_count++;

		/* At this moment the apple fall on the ground. Check to see if any piggy can catch it.*/
		if (apple_fall_count == 3)
		{
			for (i = 0; i < f_num_pigs; i++)
			{
				distance_to_apple = f_apple_locations[f_apple_index_available] - f_pigs[f_ranked_pigs[i]].current_position;

				if ((distance_to_apple >= 0) && (distance_to_apple < 4) &&
						(true == f_apple_released))
				{
					(void)pthread_mutex_lock(&apple_locker);
					f_apple_released = false;
					pthread_cond_signal(&apple_release);
					(void)pthread_mutex_unlock(&apple_locker);
					apple_catcher = f_ranked_pigs[i];

					pthread_mutex_lock(&window_refresh_lock);
					mvprintw((diag_line_count++)%(LINES -7)+2, (COLS/2 - 15), "Piggy %s got the apple %d", f_pigs[f_ranked_pigs[i]].name, f_apple_index_available+1);
					refresh();
					pthread_mutex_unlock(&window_refresh_lock);

					f_pigs[f_ranked_pigs[i]].num_apples_caught++;
					break;
				}
			}
		}

	}

	return apple_catcher;
}

void piggy_assign_names(char * player_name)
{
	int i;
	int name_offset;

	srand(time(NULL));

	f_pigs[0].name = player_name;

	for (i = 1; i < f_num_pigs; i++)
	{
		name_offset = rand()%MAX_NUM_PIGS;
		f_pigs[i].name = pig_names[name_offset];
	}
}

void rank_piggy()
{
	int i, j, index;


	for (i = 0; i < f_num_pigs; ++i)
	{
		for (j = i; j > 0; --j)
		{
			index = f_ranked_pigs[j - 1];
			if (f_pigs[index].current_position < f_pigs[i].current_position)
			{
				f_ranked_pigs[j] =  f_ranked_pigs[j-1];
			}
			else
			{
				break;

			}
		}
		f_ranked_pigs[j] = i;

	}
}

void rank_piggy_final()
{
	int i, j, index;


	for (i = 0; i < f_num_pigs; ++i)
	{
		for (j = i; j > 0; --j)
		{
			index = f_ranked_pigs[j - 1];
			if (f_pigs[index].time_period > f_pigs[i].time_period)
			{
				f_ranked_pigs[j] =  f_ranked_pigs[j-1];
			}
			else
			{
				break;
			}
		}
		f_ranked_pigs[j] = i;

	}
}
