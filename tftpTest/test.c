/*
 * Test RTEMS TFTP Client Filesystem
 *
 * This program may be distributed and used for any purpose.
 * I ask only that you:
 *      1. Leave this author information intact.
 *      2. Document any changes you make.
 *
 * W. Eric Norum
 * Saskatchewan Accelerator Laboratory
 * University of Saskatchewan
 * Saskatoon, Saskatchewan, CANADA
 * eric@skatter.usask.ca
 *
 *  $Id: test.c,v 1.8 2007/06/21 18:26:28 joel Exp $
 */

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <rtems.h>
#include <rtems/error.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdlib.h>

static char cbuf[1024];
static char *fullname;
static rtems_interval then, now;

static void
showRate (unsigned long totalRead)
{
	int elapsed;

	printf ("Read %lu bytes", totalRead);
	elapsed = now - then;
	if (elapsed) {
		rtems_interval ticksPerSecond;
		rtems_clock_get (RTEMS_CLOCK_GET_TICKS_PER_SECOND, &ticksPerSecond);
		printf (" (%ld bytes/sec)",
				(long)(((long long)totalRead * ticksPerSecond)
								/ elapsed));
	}
	printf (".\n");
}

static void
testRawRead (void)
{
	int fd;
	int nread;
	unsigned long totalRead = 0;

	fd = open (fullname, O_RDONLY);
	if (fd < 0) {
		printf ("Open failed: %s\n", strerror (errno));
		return;
	}

	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &then);
	for (;;) {
		nread = read (fd, cbuf, sizeof cbuf);
		if (nread < 0) {
			printf ("Read failed: %s\n", strerror (errno));
			close (fd);
			return;
		}
		if (nread == 0)
			break;
		totalRead += nread;
	}
	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &now);
	close (fd);
	showRate (totalRead);
}

static void
testFread (void)
{
	FILE *fp;
	int nread;
	unsigned long totalRead = 0;

	fp = fopen (fullname, "r");
	if (fp == NULL) {
		printf ("Open failed: %s\n", strerror (errno));
		return;
	}

	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &then);
	for (;;) {
		nread = fread (cbuf, sizeof cbuf[0], sizeof cbuf, fp);
		if (nread < 0) {
			printf ("Read failed: %s\n", strerror (errno));
			fclose (fp);
			return;
		}
		if (nread == 0)
			break;
		totalRead += nread;
	}
	rtems_clock_get (RTEMS_CLOCK_GET_TICKS_SINCE_BOOT, &now);
	fclose (fp);
	showRate (totalRead);
}

static int
makeFullname (const char *hostname, const char *file)
{
	if (hostname == NULL)
		hostname = "";
	fullname = realloc (fullname, 8 + strlen (file) + strlen (hostname));
	sprintf (fullname, "/TFTP/%s/%s", hostname, file);
	printf ("Read `%s'.\n", fullname);
	return 1;
}

void
testTFTP (const char *hostname, const char *filename)
{
	/*
	 * Check that invalid file names are reported as such
	 */
	if (makeFullname (hostname, "")) {
		testRawRead ();
		testFread ();
	}

	/*
	 * Check that non-existent files are reported as such
	 */
	if (makeFullname (hostname, "BAD-FILE-NAME")) {
		testRawRead ();
		testFread ();
	}

	/*
	 * Check that read works
	 */
	if (makeFullname (hostname, filename)) {
		testRawRead ();
		testFread ();
	}
}
