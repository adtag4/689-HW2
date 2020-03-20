/* Logger.h
 * Author: Albert Taglieri
 * Purpose: a logger that runs on its own thread and is available to log events to a text file
 */

#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <chrono>

#include <queue>

class Logger
{
	public:
		Logger(); // always must be given a filename
		Logger(std::string filename); // also starts running own thread on construction
		~Logger() {};
		
		void	shutdown(); // tells log to shutdown its thread
		void	log(std::string entry); // allows entries to be added to the queue to log
	protected:
	private:
		void		main(); // performs actual loggin
		
		std::ofstream	logFile_;
		bool		run_;

		// holds entries and their times
		std::queue<std::pair<std::string, std::chrono::system_clock::time_point>>	toWrite_;
};
