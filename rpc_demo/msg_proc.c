/*
 * Server-side implementation of remote procedure calls
 */
#include <stdio.h>
#include "msg.h"

int *
printmessage_1_svc(char **argp, struct svc_req *rqstp)
{
	static int result;

	result = printf ("WOW -- RPC WORKS: `%s'\n", *argp);
	return &result;
}

int *
printnum_1_svc(int *argp, struct svc_req *rqstp)
{
	static int  result;

	printf ("The number you sent is %d.\n", *argp);
	result = *argp * *argp;
	return &result;
}
