#include <sys/stat.h>
#include <termios.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>

#define VERSION "0.1.a"
#define BAUDN 8

char *BAUD_T[] =
{"300", "1200", "2400", "9600", "19200", "38400", "57600", "115200"};

int BAUD_B[] =
{B300, B1200, B2400, B9600, B19200, B38400, B57600, B115200};


char *
del1013 (char *str)
{
  short i;
  char *dummy;

  dummy = (char *) malloc (1024);
  dummy[0] = 0;
  for (i = 0; i < strlen (str); i++)
    if (str[i] != 10 && str[i] != 13)
      sprintf (dummy, "%s%c", dummy, str[i]);

  return dummy;
}

int
main (int argc, char *argv[])
{
  FILE *logfile;
  fd_set rfds;
  int retval, i, j, baud = -1;
  int fd;
  char line[1024], modem_device[512];
  time_t logdate;
  struct tm *logdate_s;
  struct termios oldtio, newtio;



  if (argc < 2)
    {
      printf ("%s: no params. try %s -h\n", argv[0], argv[0]);
      exit (0);
    }

  for (i = 1; i < argc; i++)
    {

      if (!strcmp (argv[i], "-h") || !strcmp (argv[i], "--help"))
	{
	  printf ("ttylog version %s. Programmed by Tibor Koleszar <oldw@debian.org>\n", VERSION);
	  printf ("Usage: \n -h, --help	This help\n -v, --version	Version number\n");
	  printf (" -b, --baud	Baud rate\n -d, --device	Serial device (eg. /dev/ttyS1)\n");
	  exit (0);
	}

      if (!strcmp (argv[i], "-v") || !strcmp (argv[i], "--version"))
	{
	  printf ("ttylog verison %s.\n", VERSION);
	  exit (0);
	}

      if (!strcmp (argv[i], "-b") || !strcmp (argv[i], "--baud"))
	{
	  if (argv[i + 1] != NULL)
	    {
	      for (j = 0; j < BAUDN; j++)
		if (!strcmp (argv[i + 1], BAUD_T[j]))
		  baud = j;
	    }
	  if (baud == -1)
	    {
	      printf ("%s: invalid baud rate %s\n", argv[0], argv[i + 1]);
	      exit (0);
	    }
	}

      if (!strcmp (argv[i], "-d") || !strcmp (argv[i], "--device"))
	{
	  if (argv[i + 1] != NULL)
	    {
	      strcpy (modem_device, argv[i + 1]);
	    }
	  else
	    {
	    }
	}


    }


  logfile = fopen (modem_device, "rb");
  if (logfile == NULL)
    {
      printf ("%s: invalid device %s\n", argv[0], modem_device);
      exit (0);

    }
  fd = fileno (logfile);

  tcgetattr (fd, &oldtio);	/* save current serial port settings */
  bzero (&newtio, sizeof (newtio));	/* clear struct for new port settings */

  newtio.c_cflag = BAUD_B[baud] | CRTSCTS | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR | ICRNL;
  newtio.c_oflag = 0;
  newtio.c_lflag = ICANON;

  tcflush (fd, TCIFLUSH);
  tcsetattr (fd, TCSANOW, &newtio);

  /* Clear the device */
  FD_ZERO (&rfds);
  FD_SET (fd, &rfds);
  fgets (line, 1024, logfile);

  while (1)
    {
      FD_ZERO (&rfds);
      FD_SET (fd, &rfds);
      retval = select (fd + 1, &rfds, NULL, NULL, NULL);
      if (retval)
	{
	  fgets (line, 1024, logfile);
	  strcpy (line, del1013 (line));
	  logdate = time (NULL);
	  logdate_s = localtime (&logdate);
	  printf ("%s\n", line);
	  fflush(stdout);
	}
    }

  fclose (logfile);
  tcsetattr (fd, TCSANOW, &oldtio);
  return 0;
}
