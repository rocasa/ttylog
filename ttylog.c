/* ttylog - serial port logger
 Copyright (C) 1999-2002  Tibor Koleszar
 Copyright (C) 2008-2013  Robert James Clay

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#include <sys/stat.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#define VERSION "0.21"
#define BAUDN 9

char flush = 0;

char *BAUD_T[] =
{"300", "1200", "2400", "4800", "9600", "19200", "38400", "57600", "115200"};

int BAUD_B[] =
{B300, B1200, B2400, B4800, B9600, B19200, B38400, B57600, B115200};

int
main (int argc, char *argv[])
{
  FILE *logfile;
  fd_set rfds;
  int retval, i, j, baud = -1;
  int fd;
  char line[1024], modem_device[512];
  struct termios oldtio, newtio;


  modem_device[0] = 0;

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
	  printf (" -b, --baud	Baud rate\n -d, --device	Serial device (eg. /dev/ttyS1)\n -f, --flush	Flush output\n");
	  exit (0);
	}

      if (!strcmp (argv[i], "-v") || !strcmp (argv[i], "--version"))
	{
	  printf ("ttylog version %s.\n", VERSION);
	  exit (0);
	}

      if (!strcmp (argv[i], "-f") || !strcmp (argv[i], "--flush"))
	{
	  flush = 1;
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
	      memset (modem_device, '\0', sizeof(modem_device));
	      strncpy (modem_device, argv[i + 1], sizeof(modem_device)-1);
	    }
	  else
	    {
	    }
	}


    }

  if (!strlen(modem_device)) {
    printf ("%s: no device is set. Use %s -h for more information.\n", argv[0], argv[0]);
    exit (0);
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
  newtio.c_iflag = IGNPAR | IGNCR;
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
	  printf ("%s\n", line);
	  if (flush) { fflush(stdout); }
	}
    }

  fclose (logfile);
  tcsetattr (fd, TCSANOW, &oldtio);
  return 0;
}
