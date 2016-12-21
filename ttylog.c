/* ttylog - serial port logger
 Copyright (C) 1999-2002  Tibor Koleszar <oldw@debian.org>
 Copyright (C) 2008-2017  Robert James Clay <jame@rocasa.us>
 Copyright (C)      2016  Donald Gordon <donald@tawherotech.nz>
 Copyright (C)      2016  Alexander (MrMontag) Fust <MrMontagOpenDev@gmail.com>
 Copyright (C)      2016  Logan Rosen <loganrosen@gmail.com>

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
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

#include "config.h"

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
  int retval, i, j, baud, stamp = -1;
  timer_t timerid;
  struct sigevent sevp;
  sevp.sigev_notify = SIGEV_SIGNAL;
  sevp.sigev_signo = SIGINT;
  int fd;
  char line[1024], modem_device[512];
  struct termios oldtio, newtio;
  time_t rawtime;
  struct tm *timeinfo;
  char *timestr;

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
          printf ("ttylog version %s\n", TTYLOG_VERSION);
          printf ("Usage:  ttylog [-b|--baud] [-d|--device] [-f|--flush] [-t|--timeout] > /path/to/logfile\n");
          printf (" -h, --help	This help\n -v, --version	Version number\n -b, --baud	Baud rate\n");
          printf (" -d, --device	Serial device (eg. /dev/ttyS1)\n -f, --flush	Flush output\n");
          printf (" -s, --stamp\tPrefix each line with datestamp\n");
          printf (" -t, --timeout  How long to run, in seconds.\n");
          printf ("ttylog home page: <http://ttylog.sourceforge.net/>\n\n");
          exit (0);
        }

      if (!strcmp (argv[i], "-v") || !strcmp (argv[i], "--version"))
        {
          printf ("ttylog version %s\n", TTYLOG_VERSION);
          printf ("Copyright (C) 2016 Robert James Clay <jame@rocasa.us>\n");
          printf ("Copyright (C) 2016 Logan Rosen <loganrosen@gmail.com>\n");
          printf ("Copyright (C) 2016 Alexander (MrMontag) Fust <alexander.fust.info@gmail.com>\n");
          printf ("Copyright (C) 2002 Tibor Koleszar <oldw@debian.org>\n");
          printf ("License GPLv2+: <http://www.gnu.org/licenses/old-licenses/gpl-2.0.html>\n");
          printf ("This is free software: you are free to change and redistribute it.\n");
          printf ("There is NO WARRANTY, to the extent permitted by law.\n\n");
          exit (0);
        }

      if (!strcmp (argv[i], "-f") || !strcmp (argv[i], "--flush"))
        {
          flush = 1;
        }

      if (!strcmp (argv[i], "-s") || !strcmp (argv[i], "--stamp"))
        {
          stamp = 1;
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
            modem_device[(sizeof(modem_device)-1)] = '\0';
          }
        else
          {
          }
      }

    if (!strcmp (argv[i], "-t") || !strcmp (argv[i], "--timeout"))
      {
        if (argv[i + 1] == NULL)
          {
            printf ("%s: invalid time span %s\n", argv[0], argv[i + 1]);
            exit(0);
          }
        if (timer_create (CLOCK_REALTIME, &sevp, &timerid) == -1)
          {
            printf ("%s: unable to create timer: %s\n", argv[0], strerror(errno));
            exit (0);
          }
        struct itimerspec new_value;
        new_value.it_interval.tv_sec = 0;
        new_value.it_interval.tv_nsec = 0;
        int sec = atoi(argv[i + 1]);
        if (!sec)
          {
            printf ("%s: invalid time span %s\n", argv[0], argv[i + 1]);
            exit(0);
          }
        new_value.it_value.tv_sec = atoi(argv[i + 1]);
        new_value.it_value.tv_nsec = 0;
        if (timer_settime(timerid, 0, &new_value, NULL) == -1)
          {
            printf ("%s: unable to set timer time: %s\n", argv[0], strerror(errno));
            exit (0);
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

  newtio.c_cflag = CRTSCTS | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR | IGNCR;
  newtio.c_oflag = 0;
  newtio.c_lflag = ICANON;
  /* Only truly portable method of setting speed. */
  cfsetispeed (&newtio, BAUD_B[baud]);
  cfsetospeed (&newtio, BAUD_B[baud]);

  tcflush (fd, TCIFLUSH);
  tcsetattr (fd, TCSANOW, &newtio);

  /* Clear the device */
  {
    int flags = fcntl (fd, F_GETFL, 0);
    fcntl (fd, F_SETFL, flags | O_NONBLOCK);
    while (fgets (line, 1024, logfile) );
    fcntl (fd, F_SETFL, flags);
  }

  if (flush)
    setbuf(stdout, NULL);

  while (1)
    {
      FD_ZERO (&rfds);
      FD_SET (fd, &rfds);
      retval = select (fd + 1, &rfds, NULL, NULL, NULL);
      if (retval > 0)
        {
          if (!fgets (line, 1024, logfile))
            {
              if (ferror (logfile)) { break; }
            }
          if (stamp)
            {
              time(&rawtime);
              timeinfo = localtime(&rawtime);
              timestr = asctime(timeinfo);
              timestr[strlen(timestr) - 1] = 0;
              printf ("[%s] %s", timestr, line);
            } else {
              fputs (line, stdout);
            }

          if (flush) { fflush(stdout); }
        }
      else if (retval < 0) { break; }
    }

  fclose (logfile);
  tcsetattr (fd, TCSANOW, &oldtio);
  return 0;

}
