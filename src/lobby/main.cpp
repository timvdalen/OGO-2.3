/*
 * Lobby main file
 *
 * Authors: Ogo 2.3 Group 3 2012
 * 
 * Date: 26-04-12 12:11
 *
 * Description: Player accumulation and social interaction prior to a game.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "net.h"

//------------------------------------------------------------------------------

int main(int argc, char *argv[])
{
	
	Net::Initialize();
	
	Net::Address test("www.example.com");
	
	printf("%x\n", test.debug());
	
	puts("Press any key...");
	getchar();
	return (EXIT_SUCCESS);
}

//------------------------------------------------------------------------------
