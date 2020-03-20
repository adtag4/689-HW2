#include "Client.h"

int main(int argc, char **argv)
{
	Client c("127.0.0.1", 10000);
	c.main();
	return 0;
}
