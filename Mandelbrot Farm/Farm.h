#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "Task.h"


class Farm
{
public:
	Farm(int);

	~Farm();

	void add_task(Task *task);
	void run();
	int getTasks();
	void exitThreads();
	void notifyWorkers();
	double getPercentDone();
	void resetPercentDone();

private:
	//array for all tasks
	std::queue<Task *> Tasks;
	//mutex used in worker threads to keep Tasks queue thread safe
	std::mutex Tasks_Mutex;
	//holds pointers to all threads (includes all worker threads and another thread that handles the loading bar)
	std::vector<std::thread *> threadList;
	//condition variable to make worker threads wait when not in use, so no need to keep making and joining them
	std::condition_variable conditionVar;
	//holds number of tasks currently in Tasks queue
	std::atomic<int> currentTasks = 0;
	//mutex for notification thread and noti thread condition var
	std::mutex Noti_Mutex;
	std::condition_variable notiConditionVar;
	//holds number of number of tasks that have finished execution
	std::atomic<int> taskNum = 0;
	//assinment happends in one instruction, and only one thread is assinging so should be safe to read in a seperate thread
	std::atomic<double> percentDone = 0.0;
};

