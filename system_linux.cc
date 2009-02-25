/***************************************************************************
                     system_linux.cc  -  Linux platform specific routines
                               -------------------
      copyright            : (C) 2003 by Morten Hustveit
      email                : morten@debian.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <vector>

#include <sched.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <unistd.h>

#include <espace/input.h>
#include <espace/output.h>
#include <espace/string.h>
#include <espace/system.h>

namespace
{
  struct JoystickEvent
  {
    enum
    {
      Button = 1,
      Axis = 2,
      Init = 128
    };

    uint32_t time;
    int16_t  value;
    uint8_t  type;
    uint8_t  number;
  };

  std::vector<int> joysticks;

  void uninitialize();
}

void System::initializeLinux()
{
  for(int i = 0; i < 4; ++i)
  {
    String device = "/dev/input/js" + String::number(i);

    int fd = open(device, O_RDONLY);

    if(fd < 0)
      continue;

    joysticks.push_back(fd);

    esInfo << "Added joystick #" << joysticks.size()
           << " on device \"" << device << "\"." << std::endl;
  }

  atexit(uninitialize);
}

void System::idleLinux()
{
  // In case the program is running several threads (e.g. for audio), let the
  // others run for a bit
  sched_yield();

  for(uint i = 0; i < joysticks.size(); ++i)
  {
    // Break when there are no more events
    for(;;)
    {
      pollfd pfd;

      pfd.fd = joysticks[i];
      pfd.events = POLLIN;
      pfd.revents = 0;

      if(0 >= poll(&pfd, 1, 0))
        break;

      JoystickEvent event;

      read(joysticks[i], &event, sizeof(event));

      if((event.type & JoystickEvent::Init)
      && (event.value == 0))
        continue;

      if(event.type & JoystickEvent::Button)
      {
        int key = ((i == 1)   ? Input::K_JOY2_1
                 : (i == 2)   ? Input::K_JOY3_1
                 : (i == 3)   ? Input::K_JOY4_1
                 : /* i == 0 */ Input::K_JOY1) + event.number;

        if(event.value)
        {
          Input::keyPressed(key);
        }
        else
        {
          Input::keyReleased(key);
        }
      }

      if(event.type & JoystickEvent::Axis)
      {
        Input::joystickMoved(i, event.number, event.value);
      }
    }
  }
}

namespace
{
  void uninitialize()
  {
    for(uint i = 0; i < joysticks.size(); ++i)
      close(joysticks[i]);
  }
}

