#include <stdio.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <string>
#include <limits.h>
#include <iomanip>
#include <unistd.h>

using namespace std;

struct car
{
	int carNo;
	char destination;
	int time;
};

void* tunnelThread(void* arg);
void* carThread(void* arg);

static bool done = false;
static char bound;
static pthread_mutex_t carMutex;
static int MAX;
static int Whit = 0;
static int Bear = 0;
static int delayed = 0;
static int inTunnel = 0;
static pthread_cond_t wake = PTHREAD_COND_INITIALIZER;

int main()
{
	int numCars = 0;
	pthread_t threadIDs[128];
	pthread_mutex_init(&carMutex, NULL);

	string readIn;
	cin >> readIn;
	MAX = stoi(readIn);

	pthread_t theTunnel;
	pthread_create(&theTunnel, NULL, tunnelThread, NULL);
	while (cin >> readIn)
	{
		sleep(stoi(readIn));
		cin >> readIn;
		car* temp = new car;
		temp->destination = readIn[0];
		cin >> readIn;
		temp->time = stoi(readIn);
		temp->carNo = numCars + 1;
		pthread_t tid;
		pthread_create(&tid, NULL, carThread, (void*)temp);
		threadIDs[numCars] = tid;
		numCars++;
	}
	for (int i = 0; i < numCars; i++)
		pthread_join(threadIDs[i], NULL);
	done = true;

	cout << Whit << " car(s) going to Whittier arrived at the tunnel." << endl;
	cout << Bear << " car(s) going to Bear Valley arrived at the tunnel." << endl;
	cout << delayed << " car(s) were delayed." << endl;

	return 0;
}

void* tunnelThread(void* arg)
{
	while (!done)
	{
		pthread_mutex_lock(&carMutex);
		bound = 'W';
		cout << "The tunnel is now open for Whittier-bound traffic." << endl;
		pthread_cond_broadcast(&wake);
		pthread_mutex_unlock(&carMutex);
		sleep(5);

		pthread_mutex_lock(&carMutex);
		bound = 'N';
		cout << "The tunnel is now closed to ALL traffic." << endl;
		pthread_mutex_unlock(&carMutex);
		sleep(5);

		pthread_mutex_lock(&carMutex);
		bound = 'B';
		cout << "The tunnel is now open for Bear Valley-bound traffic." << endl;
		pthread_cond_broadcast(&wake);
		pthread_mutex_unlock(&carMutex);
		sleep(5);

		pthread_mutex_lock(&carMutex);
		bound = 'N';
		cout << "The tunnel is now closed to ALL traffic." << endl;
		pthread_mutex_unlock(&carMutex);
		sleep(5);
	}
}

void* carThread(void* arg)
{
	bool wasDelayed = false;
	car* tempCar;
	tempCar = (struct car*)arg;
	string direction;
	int carNo = tempCar->carNo;
	int travel = tempCar->time;
	char headed = tempCar->destination;
	if (headed == 'W')
	{
		direction = "Whittier";
		Whit++;
	}
	else
	{
		direction = "Bear Valley";
		Bear++;
	}
	cout << "Car # " << carNo << " going to " << direction << " arrives at the tunnel." << endl;
	pthread_mutex_lock(&carMutex);
	while ((headed != bound) || (inTunnel >= MAX))
	{
		if (inTunnel >= MAX)
			wasDelayed = true;
		pthread_cond_wait(&wake, &carMutex);
	}
	inTunnel++;
	cout << "Car # " << carNo << " going to " << direction << " enters the tunnel." << endl;
	pthread_mutex_unlock(&carMutex);
	sleep(travel);
	pthread_mutex_lock(&carMutex);
	cout << "Car # " << carNo << " going to " << direction << " exits the tunnel." << endl;
	if (wasDelayed)
		delayed++;
	inTunnel--;
	pthread_cond_broadcast(&wake);
	pthread_mutex_unlock(&carMutex);
}