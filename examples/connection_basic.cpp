#include "../include/gdbplz/connection.hpp"
#include <sstream>
#include <iostream>
#include <chrono>
#include <thread>

int main()
{
	gdbplz::connection c(gdbplz::guess_gdb_path().value());
	while(auto optres = c.wait())
	{
		//auto& res = *optres;
		//std::cout << res;
	}
}