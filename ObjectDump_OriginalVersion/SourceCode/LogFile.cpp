// GNU General Public License Agreement
// Copyright (C) 2015-2016 Daniele Berto daniele.fratello@gmail.com
//
// objectDump is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software Foundation. 
// You must retain a copy of this licence in all copies. 
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public License for more details.
// ---------------------------------------------------------------------------------

/**
* @file LogFile.cpp 
*/

#include "ConfObject.h"
#include "ApplicationSetup.h"
#include "DigitizerErrorObject.h"
#include "LogFile.h"
#include "DefineGeneral.h"
#include "OutputModule.h"
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


LogFile *
  LogFile::log_file_pInstance = NULL;


LogFile *
LogFile::Instance ()
{
  if (!log_file_pInstance)	// Only allow one instance of class to be generated.
    log_file_pInstance = new LogFile ();

  return log_file_pInstance;
}


void
LogFile::LogFileMessageOn ()
{

flag = 1;

}


void
LogFile::LogFileMessageOff ()
{

flag = 1;

}

void
LogFile::LogFileSet (const char *log_file_arg)
{

  flag = 0;

  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  log_file_path = (const char *) malloc (strlen (log_file_arg) + 1);
  strcpy ((char *) log_file_path, (char *) log_file_arg);
  log_file_punt = fopen (log_file_path, "w");

  if (log_file_punt == NULL)
    {
      output_module->Output("Warning, I can't access log file.\n");
      can_write = 0;

    }
  else
    {
      output_module->Output("Accessing log file...\n");
      can_write = 1;
    }
}				//void LogFile::LogFileSet(const char * log_file_arg)


void
LogFile::LogFileWriteString (const char *string)
{
  if (can_write)
    {
      fprintf (log_file_punt, string);
    }
}


void
LogFile::LogFileWrite (CAEN_DGTZ_ErrorCode ret_arg, const char *file,
		       const char *func, int line)
{
  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  if (can_write)
    ret_debug =
      my_error.DigitizerErrorObjectDebuggingLog (ret_arg, file, func, line,
						 log_file_punt);

  if (ret_debug && flag == 1)
	{
	output_module->Output("Some error occurred, digit \"more\" for informations\n");
        flag = 0;
	}

}  //void LogFile::LogFileWrite(CAEN_DGTZ_ErrorCode ret_arg, const char *file, const char *func, int line)


void
LogFile::LogFileRead ()
{
  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  char buffer[999];
  bzero (buffer, 999);
  char ch;

  int i = 0;
  int flag = 0;

  FILE *read_log;
  read_log = fopen (log_file_path, "r");


  if (read_log != NULL)
    {
      fseek (read_log, 0, SEEK_SET);
      while ((ch = fgetc (read_log)) != EOF)
	{
	  if (i < 997)
	    {
	      buffer[i] = ch;
	      i++;
	      flag = 1;
	    }
	  else
	    {
	      buffer[i] = ch;
	      buffer[998] = '\0';
	      output_module->Output((const char *) buffer);
	      bzero (buffer, 999);
	      flag = 0;
	      i = 0;
	    }			// } else {
	}			//while( ( ch = fgetc(read_log) ) != EOF )

      if (flag == 1)
	{
	  buffer[i] = '\0';
	  output_module->Output((const char *) buffer);
	  bzero (buffer, 999);
	}
      fclose (read_log);	// N.B. la fclose deve stare dentro senno' capita una segmentation fault!!!
    }   // if (read_log != NULL)

}   //void LogFile::LogFileRead()
