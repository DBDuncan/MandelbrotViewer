#include <thread>
#include <queue>
#include <mutex>
#include <iostream>
#include <condition_variable>

#include "Farm.h"
#include "Common.h"



Farm::Farm(int setNumThreads)
{
	auto worker = [&](int num)
	{
		while (true) {
			Task *task;
			{
				std::unique_lock<std::mutex> lock(Tasks_Mutex);

				//while loop here to prevent spurious wakeups by checking if the Tasks vector is empty, and if it is
				//the program should be waiting
				while (Tasks.empty())
				{
					conditionVar.wait(lock);
				}
				//gets first task from the vector
				task = Tasks.front();

				//exits the thread if the pointer gotten from the task vector is a nullptr which acts as a signal to stop execution
				if (task == nullptr)
				{
					std::cout << "thread exit" << std::endl;
					break;
				}

				taskNum++;
				Tasks.pop();
				notiConditionVar.notify_one();
				currentTasks--;
			}
			//std::cout << "thread num " << num << " task num "<< Tasks.size() << std::endl;
			task->run();
		}
	};


	auto notififer = [&]()
	{
		//try adding "tasks" to array and norifier works on it
		//intead use atomic int type and have worker add and notififer take away and work when greater than 0
		while (true) {

			std::unique_lock<std::mutex> lock(Noti_Mutex);

			
			//spurious wakeups are not an issue, becasue if wakeup does happen while program is not making a mandelbrot, 
			//the loading bar will not change size because while the image is not loading, the loading bar is being set to
			//max size
			notiConditionVar.wait(lock);
			

			if (taskNum == -1)
			{
				std::cout << "noti thread exit" << std::endl;
				break;
			}


			percentDone = (static_cast<double>(taskNum) / static_cast<double>(HEIGHT)) * 100.0;
			//reset taskNum to 0 
			if (taskNum >= HEIGHT)
			{
				taskNum = 0;
			}
		}
	};

	threadList.push_back(new std::thread(notififer));

	int CPUs = std::thread::hardware_concurrency();

	if (setNumThreads > 0)
	{
		CPUs = setNumThreads;
	}

	std::cout << "mandelbrot calc threads: " << CPUs << std::endl;

	if (CPUs < 1)
	{
		threadList.push_back(new std::thread(worker, 0));
	}
	else
	{
		for (int i = 0; i < CPUs; i++)
		{
			threadList.push_back(new std::thread(worker, i));
		}
	}



}


Farm::~Farm()
{
}

void Farm::add_task(Task *task)
{
	std::unique_lock<std::mutex> lock(Tasks_Mutex);
	Tasks.push(task);
	currentTasks++;
}

void Farm::run()
{







}

int Farm::getTasks()
{

	return currentTasks;
}

void Farm::exitThreads()
{
	//scoped in so the mutex is unlocked and the threads can process the exit
	{
		//locks task mutex, pushes nullptr that the threads look for as a noti to exit, and notify all of them
		//then, set task to -1 which acts as notification for the thread to handel the loading bar to exit
		//then notify the one loading bar thread
		std::unique_lock<std::mutex> lock(Tasks_Mutex);
		Tasks.push(nullptr);
		conditionVar.notify_all();
		taskNum = -1;
		notiConditionVar.notify_one();
	}

	for (auto *t : threadList)
	{
		t->join();
	}
	std::cout << "thread exit done" << std::endl;
}

void Farm::notifyWorkers()
{
	conditionVar.notify_all();
}

double Farm::getPercentDone()
{
	return percentDone;
}

//to fix rare bug of loading bar being offset
void Farm::resetPercentDone()
{
	taskNum = 0;
}





