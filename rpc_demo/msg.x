#ifdef RPC_HDR
%#define TCP_PORT	15376
#endif

program MESSAGEPROC
{
	version MESSAGEVERS
	{
		int printmessage(string) = 1;
		int printnum(int) = 2;
	} = 1;
} = 99;
