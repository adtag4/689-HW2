// Logger.cpp

#include "Logger.h"
#include <queue>
#include <thread>
#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>

Logger::Logger() : Logger("log.log")
{
}

Logger::Logger(std::string filename)
	: run_(true)
{
	logFile_.open(filename);
	auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	logFile_ << "STARTING LOG AT: " << ctime(&time) << std::endl << std::endl;

	std::thread logThread(&Logger::main, this); // start logging thread
	logThread.detach(); // don't need to keep track - can shutdown with shutdown() on Logger object
}

void Logger::shutdown()
{
	run_ = false;
}

void Logger::main()
{
	while(run_)
	{
		while(!toWrite_.empty()) // stuff to write - and ensured to write (b/c check for run_ is outside)
		{
			auto item = toWrite_.front();
			toWrite_.pop();

			// write "TIME: MSG"
			auto time = std::chrono::system_clock::to_time_t(item.second);
			logFile_ << ctime(&time) << ": " << item.first << std::endl;
		}

		// don't need to burn all the cycles
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	// clean up
	logFile_.close();
}

void Logger::log(std::string entry)
{
	// add time
	auto x = std::make_pair(entry, std::chrono::system_clock::now());
	toWrite_.push(x); // add to queue
}
