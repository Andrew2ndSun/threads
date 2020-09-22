#include <stdio.h>
#include <pthread.h>
#include <iostream>
#include <fstream>
#include <string>
#include <limits.h>
#include <iomanip>
#include <unistd.h>
//#include <time.h>

using namespace std;

struct carVal
{
	int carNum;
	char dir;
	int time;
};

void* tunnel(void* arg);
void* car(void* arg);

static bool completed = false;
static char traffic;
static pthread_mutex_t trafLock;
static int maxCars;
static int Whittier_bound = 0;
static int BearValley_bound = 0;
static int delayed = 0;
static int currentCars = 0;
static pthread_cond_t wake = PTHREAD_COND_INITIALIZER;
//static pthread_cond_t wb = PTHREAD_COND_INITIALIZER;
//static pthread_cond_t bb = PTHREAD_COND_INITIALIZER;
//static pthread_cond_t notFull = PTHREAD_COND_INITIALIZER;
//static pthread_mutex_t carLock;

ifstream file;
ofstream output;

int main()
{
	int totalCars = 0;
	pthread_t cartid[128];
	pthread_mutex_init(&trafLock, NULL);
	//pthread_mutex_init(&carLock, NULL);
	
	/*string fileName;
	cin >> fileName;

	file.open(fileName);
	if (file.fail())
	{
		cout << "Could not open file.\n";
		exit(1);
	}*/
	
	string word;
	cin >> word;
	cout << word << endl;
	maxCars = stoi(word);

	//pthread_t tunnelID;
	//pthread_create(&tunnelID, NULL, tunnel, NULL);
	cout << "#";///
	while (cin >> word)
	{
		cout << " ! ";///
		cout << word;///
		sleep(stoi(word)*1000);
		cin >> word;
		cout << word;///
		carVal* temp = new carVal;
		temp->dir = word[0];
		cin >> word;
		cout << word << endl;///
		cout << " ? ";
		temp->time = stoi(word);
		temp->carNum = totalCars + 1;
		///pthread_t tid;
		///pthread_create(&tid, NULL, car, (void*)temp);//BEWARE!!!
		///cartid[totalCars] = tid;
		///totalCars++;
	}
	
	for (int i = 0; i < totalCars; i++)
		pthread_join(cartid[i], NULL);
	completed = true;

	cout << BearValley_bound << " car(s) going to Bear Valley arrived at the tunnel." << endl;
	cout << Whittier_bound << " car(s) going to Whittier arrived at the tunnel." << endl;
	cout << delayed << " car(s) were delayed." << endl;

	return 0;
}

void* tunnel(void* arg)
{
	while (1)
	{
		if (completed)
			pthread_exit((void*)0);//NULL?
		pthread_mutex_lock(&trafLock);
		traffic = 'W';
		cout << "The tunnel is now open for Whittier-bound traffic." << endl;
		pthread_cond_broadcast(&wake);
		//pthread_cond_broadcast(&wb);
		pthread_mutex_unlock(&trafLock);
		sleep(5000);

		if (completed)
			pthread_exit((void*)0);//NULL?
		pthread_mutex_lock(&trafLock);
		traffic = 'N';
		cout << "The tunnel is now closed to ALL traffic." << endl;
		pthread_mutex_unlock(&trafLock);
		sleep(5000);

		if (completed)
			pthread_exit((void*)0);//NULL?
		pthread_mutex_lock(&trafLock);
		traffic = 'B';
		cout << "The tunnel is now open for Bear Valley-bound traffic." << endl;
		pthread_cond_broadcast(&wake);
		//pthread_cond_broadcast(&bb);
		pthread_mutex_unlock(&trafLock);
		sleep(5000);

		if (completed)
			pthread_exit((void*)0);//NULL?
		pthread_mutex_lock(&trafLock);
		traffic = 'N';
		cout << "The tunnel is now closed to ALL traffic." << endl;
		pthread_mutex_unlock(&trafLock);
		sleep(5000);
	}
}

void* car(void* arg)
{
	carVal* acar;
	acar = (struct carVal*)arg;
	int carNo = acar->carNum;
	char bound = acar->dir;
	int travelTime = acar->time;
	bool wasDelayed = false;
	string direction;
	if (bound == 'W')
	{
		direction = "Whittier";
		Whittier_bound++;
	}
	else
	{
		direction = "Bear Valley";
		BearValley_bound++;
	}
	cout << "Car # "<<carNo<<" going to "<<direction<<" arrives at the tunnel." << endl;
	pthread_mutex_lock(&trafLock);
	while ((bound != traffic) || (currentCars >= maxCars))
	{
		if (currentCars >= maxCars)
			wasDelayed = true;
		pthread_cond_wait(&wake, &trafLock);
	}
	currentCars++;
	cout << "Car # " << carNo << " going to " << direction << " enters the tunnel." << endl;
	pthread_mutex_unlock(&trafLock);
	sleep(travelTime*1000);
	pthread_mutex_lock(&trafLock);
	currentCars--;
	pthread_cond_broadcast(&wake);
	cout << "Car # " << carNo << " going to " << direction << " exits the tunnel." << endl;
	if (wasDelayed)
		delayed++;
	pthread_mutex_unlock(&trafLock);




	/*
	if (bound == 'W')
	{
		Whittier_bound++;
		pthread_mutex_lock(&trafLock);
		cout << "Car # " << carNo << " going to Whittier arrives at the tunnel." << endl;
		while (traffic != 'W')
		{
			pthread_cond_wait(&wb, &trafLock);
			if (currentCars >= maxCars)
			{
				wasDelayed = true;
				pthread_cond_wait(&notFull, &trafLock);
			}
		}
		while (currentCars >= maxCars)
		{
			wasDelayed = true;
			pthread_cond_wait(&notFull, &trafLock);
			if (traffic != 'W')
				pthread_cond_wait(&wb, &trafLock);
		}
		currentCars++;
		cout << "Car # " << carNo << " going to Whittier enters the tunnel." << endl;
		pthread_mutex_unlock(&trafLock);
		sleep(travelTime);
		pthread_mutex_lock(&trafLock);
		currentCars--;
		pthread_cond_broadcast(&notFull);
		cout << "Car # " << carNo << " going to Whittier exits the tunnel." << endl;
		if (wasDelayed)
			delayed++;
		pthread_mutex_unlock(&trafLock);
	}
	else
	{
		BearValley_bound++;
		pthread_mutex_lock(&trafLock);
		cout << "Car # " << carNo << " going to Bear Valley arrives at the tunnel." << endl;
		while (traffic != 'B')
		{
			pthread_cond_wait(&bb, &trafLock);
			if (currentCars >= maxCars)
			{
				wasDelayed = true;
				pthread_cond_wait(&notFull, &trafLock);
			}
		}
		while (currentCars >= maxCars)
		{
			wasDelayed = true;
			pthread_cond_wait(&notFull, &trafLock);
			if (traffic != 'B')
				pthread_cond_wait(&bb, &trafLock);
		}
		currentCars++;
		cout << "Car # " << carNo << " going to Bear Valley enters the tunnel." << endl;
		pthread_mutex_unlock(&trafLock);
		sleep(travelTime);
		pthread_mutex_lock(&trafLock);
		currentCars--;
		pthread_cond_broadcast(&notFull);
		cout << "Car # " << carNo << " going to Bear Valley exits the tunnel." << endl;
		if (wasDelayed)
			delayed++;
		pthread_mutex_unlock(&trafLock);
	}*/
}