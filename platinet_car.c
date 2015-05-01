/*
 * platinet_car: Platinet car controller.
 *
 * Copyright (C) 2015 Yarda <zbox AT atlas.cz>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "platinet_car.h"
#include <stdio.h>
#include <stdarg.h>
#include <getopt.h>
#include <SDL/SDL.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#define E_ARG   1
#define E_SDL   2
#define E_JOY   3
#define E_SOCK  4
#define E_BTINQ 5
#define E_NODEV 6
#define E_CON   7
#define E_UNK   8

struct option long_options[] = {
  {"btname", required_argument, 0, 'n'},
  {"btaddr", required_argument, 0, 'b'},
  {"btchan", required_argument, 0, 'c'},
  {"speed-limit", required_argument, 0, 'l'},
  {"debug", no_argument, 0, 'd'},
  {"simulate", no_argument, 0, 's'},
  {"version", no_argument, 0, 'v'},
  {"help", no_argument, 0, 'h' },
  {0, 0, 0, 0}
};

char btname[256] = {0};
char btaddr[256] = {0};
int btchan = BTCHAN;
int speedlim = SPEED_LIMIT;
int debug = 0;
int simulate = 0;
int s;
SDL_Joystick *joy = NULL;

void version()
{
  printf("%s %s\n", xstr(NAME), xstr(VERSION));
  printf("Copyright (C) %s %s\n", xstr(YEAR), xstr(AUTHOR));
  printf("%s\n", "This is free software: you are free to change and redistribute it.");
  printf("%s\n", "This program comes with ABSOLUTELY NO WARRANTY.");
}

void help()
{
  version();
  printf("\nUsage: %s [OPTIONS]\n", xstr(NAME));
  printf("%s\n", "OPTIONS:");
  printf("%s\n", "        -n, --btname BTNAME      Connect to device with BTNAME.");
  printf("%s\n", "        -a, --btaddr BTADDR      Connect to device with BTADDR (it has");
  printf("%s\n", "                                 preference over BTNAME).");
  printf("%s\n", "        -c, --btchan BTCHAN      Use bluetooth channel BTCHAN.");
  printf("%s\n", "        -l, --speed-limit SPEED  Speed limiter (0-255), default no limit.");
  printf("%s\n", "        -d, --debug              Show debug info.");
  printf("%s\n", "        -s, --simulate           Simulation mode, no car is required.");
  printf("%s\n", "        -v, --version            Show program version.");
  printf("%s\n", "        -h, --help               Show this help.");
}

void err(code)
{
  switch (code)
  {
    case E_ARG:
      fprintf(stderr, "ERROR: %s\n", "Invalid argument.");
      printf("%s", "\n");
      help();
      exit(E_ARG);

    case E_SDL:
      fprintf(stderr, SDL_GetError());
      break;

    case E_JOY:
      fprintf(stderr, "%s\n", "ERROR: Opening joystick.");
      exit(E_JOY);

    case E_SOCK:
      fprintf(stderr, "%s\n", "ERROR: Opening socket.");
      exit(E_SOCK);

    case E_BTINQ:
      fprintf(stderr, "%s\n", "ERROR: HCI inquiry.");
      exit (E_BTINQ);

    case E_NODEV:
      fprintf(stderr, "%s\n", "ERROR: Unable to find bluetooth device with required name.");
      exit (E_NODEV);

    case E_CON:
      fprintf(stderr, "%s\n", "ERROR: Connecting to bluetooth device.");
      exit (E_CON);

    default:
      fprintf(stderr, "%s\n", "ERROR: Unknown error.");
      printf("%s", "\n");
      exit(E_UNK);
  }
}

void printf_cond(int cond, const char *fmt, ...)
{
  va_list ap;

  if (cond)
  {
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
  }
}

void parse_args(int argc, char *argv[])
{
  int c;
  int option_index = 0;

  while ((c = getopt_long(argc, argv, "n:a:c:l:dsvh", long_options, \
                            &option_index)) != -1)
  {
    if (c == -1)
      break;
    switch (c)
    {
      case 'n':
        if (optarg)
          strncpy(btname, optarg, sizeof(btname) - 1);
        else
          err(E_ARG);
        break;

      case 'a':
        if (optarg)
          strncpy(btaddr, optarg, sizeof(btaddr) - 1);
        else
          err(E_ARG);
        break;

      case 'c':
        if (optarg)
        {
          btchan = atoi(optarg);
          if (btchan < 0)
            err(E_ARG);
        }
        else
          err(E_ARG);
        break;

      case 'l':
        if (optarg)
        {
          speedlim = atoi(optarg);
          if (speedlim < 0 || speedlim > 255)
            err(E_ARG);
        }
        else
          err(E_ARG);
        break;

      case 'd':
        debug = 1;
        break;

      case 's':
        simulate = 1;
        break;

      case 'v':
        version();
        exit(0);

      case 'h':
        help();
        exit(0);

      default:
        err(E_ARG);
    }
  }
  if (optind < argc)
    err(E_ARG);
}

int mkck(int a, int b)
{
  int c = a + b + 0x33;
  return (c > 255) ? 255 : c;
}

char n2hex(unsigned char n)
{
  n &= 0xf;
  if (n < 10)
    return n + '0';
  else
    return n - 10 + 'a';
}

char *b2str(int b, char *c)
{
  int h = b / 16;
  int l = b % 16;
  snprintf(c, 3, "%c%c", n2hex(h), n2hex(l));
  return c;
}

void send_cmd(int sock, int forward, int speed, int wheel, int light)
{
  static unsigned char ctrlb;
  static unsigned char whb;
  static char ctrl[3] = {0};
  static char spd[3] = {0};
  static char wh[3] = {0};
  static char ck[3] = {0};
  static char cmd[13] = {0};

  ctrlb = 0;
  whb = 0;
  if (speed)
  {
    if (forward)
      ctrlb = 4;
    else
      ctrlb = 8;
  }
  else
  {
    if (light > 0)
      ctrlb |= 4;
    else if (light < 0)
      ctrlb |= 8;
    else
      ctrlb &= (unsigned char)0xf3;
  }

  if (wheel > 0)
    ctrlb |= 2;
  else if (wheel < 0)
    ctrlb |= 1;

  whb = abs(wheel);

  snprintf(cmd, 13, "0p%s%s%s20%s", b2str(ctrlb, ctrl), b2str(whb, wh), \
    b2str(speed, spd), b2str(mkck(speed, whb), ck));

  if (!simulate)
    send(s, cmd, 12, 0);
  printf_cond(debug, "%s\n", cmd);
}

int bt_scan(char btname[], char btaddr[])
{
  inquiry_info *ii = NULL;
  int max_rsp, num_rsp;
  int dev_id, sock, len, flags;
  int i;
  int found = 0;
  char addr[19] = {0};
  char name[248] = {0};

  printf("%s\n", "Scanning for bluetooth devices...");
  dev_id = hci_get_route(NULL);
  sock = hci_open_dev(dev_id);
  if (dev_id < 0 || sock < 0)
    err(E_SOCK);

  len  = 8;
  max_rsp = 255;
  flags = IREQ_CACHE_FLUSH;
  ii = (inquiry_info*) malloc(max_rsp * sizeof(inquiry_info));

  num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
  if (num_rsp < 0)
    err(E_BTINQ);

  for (i = 0; i < num_rsp; i++)
  {
    ba2str(&(ii+i)->bdaddr, addr);
    memset(name, 0, sizeof(name));
    if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name),
        name, 0) < 0)
      strcpy(name, "[unknown]");
    if (strncmp(name, btname, 248) == 0)
    {
      found = 1;
      strncpy(btaddr, addr, 19);
    }
    printf("%s  %s\n", addr, name);
  }

  free(ii);
  close(sock);
  return found;
}


void closejoy()
{
  if (joy)
    SDL_JoystickClose(joy);
}

void btcleanup()
{
  close(s);
}

int main(int argc, char *argv[])
{
  SDL_Surface* screen = NULL;
  SDL_Event event;
  Uint8* keys = NULL;
  int joynum = 0;
  int quit = 0;
  int speed = 0;
  int wheel = 0;
  int light = 0;
  int lk = 0;
  int forward = 1;
  struct sockaddr_rc addr= { 0 };

  parse_args(argc, argv);

  if (simulate)
    printf("%s\n", "Simulation mode.");
  else
  {
    if (!btaddr[0] && !btname[0])
      strncpy(btname, BTNAME, sizeof(btname) - 1);

    if (!btaddr[0])
    {
      printf("Bluetooth device name: %s\n", btname);
      if (!bt_scan(btname, btaddr))
        err(E_NODEV);
    }

    printf("Bluetooth device address: %s\n", btaddr);
    printf("Bluetooth device channel: %d\n", btchan);

    if ((s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM)) < 0)
      err(E_SOCK);

    atexit(btcleanup);

    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = btchan;
    str2ba(btaddr, &addr.rc_bdaddr);

    printf("Connecting to bluetooth device '%s'...\n", btaddr);
    if (!connect(s, (struct sockaddr *)&addr, sizeof(addr)))
      printf("%s\n", "Successfully connected.");
    else
      err(E_CON);
  }

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0)
    err(E_SDL);

  atexit(SDL_Quit);

  joynum = SDL_NumJoysticks();

  if (joynum)
  {
    SDL_JoystickEventState(SDL_ENABLE);
    if (!(joy = SDL_JoystickOpen(0)))
      err(E_JOY);
    atexit(closejoy);
  }

  screen = SDL_SetVideoMode(320, 240, 16, 0);

  if (!screen)
    err(E_SDL);

  while (!quit)
  {
    while(SDL_PollEvent(&event))
    {
      if (event.type == SDL_QUIT)
      {
        quit = 1;
      }
    }
    SDL_Delay(EVENT_LOOP_DELAY);
    keys = SDL_GetKeyState(NULL);

    if (keys[SDLK_ESCAPE] || keys[SDLK_q])
      quit = 1;

    if (keys[SDLK_UP] || keys[SDLK_DOWN])
      light = 0;

    // lights
    if (keys[SDLK_l])
    {
      if (lk == 0)
      {
        lk = 1;
        if (light == 0)
          light = 1;
        else
          light = 0;
      }
    }
    else
      lk = 0;

    // brakes
    if (keys[SDLK_SPACE])
    {
      speed -= 80;
      if (speed < 0)
        speed = 0;
        light = -1;
    }
    else
    {
      if (light < 0)
      light = 0;
    }

    if (keys[SDLK_UP])
      speed += (int)((float)SPEED_FACTOR * (256 - speed) / 256 + 1);
    else if (keys[SDLK_DOWN])
      speed -= (int)((float)SPEED_FACTOR * (256 - speed) / 256 + 1);

    // forward gear
    if (keys[SDLK_a])
      if (!speed)
        forward = 1;

    // rear gear
    if (keys[SDLK_z])
      if (!speed)
        forward = 0;

    // wheel
    if (keys[SDLK_LEFT])
      wheel -= WHEEL_STEP;
    else if (keys[SDLK_RIGHT])
      wheel += WHEEL_STEP;
    else
      wheel = 0;

    // joystick
    if (event.type == SDL_JOYAXISMOTION)
    {
      // filter fluctuations
      if ((event.jaxis.value < -3200) || (event.jaxis.value > 3200))
      {
        // left-right movement
        if (event.jaxis.axis == 2)
          wheel = (int)((float)event.jaxis.value / 32767 * 255);
        // up-down movement
        else if (event.jaxis.axis == 1)
        {
          light = 0;
          speed = (int)((float)event.jaxis.value / 32767 * 255);
          forward = speed <= 0;
          speed = abs(speed);
        }
      }
      else
      {
        if (event.jaxis.axis == 0)
          wheel = 0;
        else if (event.jaxis.axis == 1)
          speed = 0;
      }
    }

    // limits
    if (speed < 0)
      speed = 0;
    else if (speed > speedlim)
      speed = speedlim;
    if (wheel > 255)
      wheel = 255;
    if (wheel < -255)
      wheel = -255;

    printf_cond(debug | simulate, "gear: %c, speed: %d, course: %d\n", forward ? 'f' : 'r', speed, wheel);
    send_cmd(s, forward, speed, wheel, light);
  }
}
