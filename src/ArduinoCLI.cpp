#include "ArduinoCLI.h"

ArduinoCLI::ArduinoCLI()
{
  this->m_port = NULL;
  this->m_readByte = 0;
  this->m_readBytes = 0;
  
  this->m_argc = 0;

  memset(this->m_welcome,0,CONSOLE_WELCOME_BUFFER);
  memset(this->m_prompt,0,CONSOLE_PROMPT_BUFFER);
  memset(this->m_buffer,0,CONSOLE_INPUT_BUFFER);
  memset(this->m_argv,  0,sizeof(this->m_argv[0][0]) * CONSOLE_MAX_ARGS_NUM * CONSOLE_MAX_ARG_BUFFER);

  strcpy(this->m_prompt,">");
  strcpy(this->m_welcome,"Welcome to this simple Arduino command line interface (CLI).\r\nType \"help\" to see a list of commands.\r\n");

  for (int i = 0 ; i < CONSOLE_MAX_COMMAND_NUM ; i++)
  {    
    this->m_command[i].registered = false;
    this->m_command[i].function = NULL;
    memset(this->m_command[i].command,0,CONSOLE_CMD_NAME_SIZE);
    memset(this->m_command[i].description,0,CONSOLE_CMD_DESCRIPTION_SIZE);
  }
}

ArduinoCLI::~ArduinoCLI()
{
  this->m_port = NULL;
}

void ArduinoCLI::print(char* buffer,int len)
{
  for (int i=0; i < len ; i++)
  {  
    if (( buffer[i] == '\n' ) && ( buffer[ i -1 ] != '\r' ))
      this->m_port->print('\r');

    this->m_port->print(buffer[i]);     
  }
}

void ArduinoCLI::printf(char *format,...)
{
  //char buff[128];

  memset(this->m_outputBuffer,0,2048);

  if (this->m_port == NULL)
    return;

	va_list args;
	va_start (args,format);
	vsnprintf(this->m_outputBuffer,sizeof(this->m_outputBuffer),format,args);
	va_end (args);
	this->m_outputBuffer[sizeof(this->m_outputBuffer)/sizeof(this->m_outputBuffer[0]) - 1] ='\0';
	
  this->print(this->m_outputBuffer,strlen(this->m_outputBuffer));
  //this->m_port->println();
}

void ArduinoCLI::bind(Stream *port)
{
  this->m_port = port;
 
  delay(100);

  this->welcome();
  this->prompt();
}

void ArduinoCLI::welcome()
{
  this->printf("%s",this->m_welcome);
}

void ArduinoCLI::prompt()
{  
  this->printf("%s",this->m_prompt);
}

void ArduinoCLI::setDefaultPrompt(const char *p)
{
  if (p == NULL)
  {
    return;
  }

  if (strlen(p) >= CONSOLE_PROMPT_BUFFER )
  {
    return;
  }

  strcpy(this->m_prompt,p);
}

void ArduinoCLI::setDefaultWelcome(const char *p)
{
  if (p == NULL)
  {
    return;
  }

  if (strlen(p) >= CONSOLE_WELCOME_BUFFER )
  {
    return;
  }

  strcpy(this->m_welcome,p);
}

void ArduinoCLI::clearBuffer()
{
  memset(this->m_buffer,0,CONSOLE_INPUT_BUFFER);
    
  this->m_readByte = 0;
  this->m_readBytes = 0;


  memset(this->m_argv,  0,sizeof(m_argv[0][0]) * CONSOLE_MAX_ARGS_NUM * CONSOLE_MAX_ARG_BUFFER);
  this->m_argc = 0;
}

void ArduinoCLI::readCommand()
{
  while (this->m_port->available())
  {    
	this->m_readByte = this->m_port->read();

    if ((this->m_readByte == '\n' ) || (this->m_readByte == '\r'))
    {
      this->printf("\r\n");
      this->parseCommand();    
      this->clearBuffer();
      this->prompt();  
    }
    else
    {
      if (this->m_readBytes >= ( CONSOLE_INPUT_BUFFER - 1 ))
      {
        /* buffer size exceeded */
        this->clearBuffer();
      }

      this->m_buffer[this->m_readBytes++] = this->m_readByte;
      this->printf("%c",this->m_readByte);
    }
  }
}

void ArduinoCLI::parseCommand()
{  
  char *arg;
  char *saveptr;

  for(arg=strtok_r(this->m_buffer," ",&saveptr);  arg != NULL ; arg = strtok_r(NULL," ",&saveptr)) 
  {
    strcpy(this->m_argv[this->m_argc],arg);
    this->m_argc++;
  } 
  
  this->executeCommand();
}

void ArduinoCLI::executeCommand()
{
  if ( this->m_argc <= 0 )
  {
    return;
  }

  bool valid_command = false;

  for (int i = 0 ; i < CONSOLE_MAX_COMMAND_NUM ; i++)
  {
    if ( this->m_command[i].registered == false)
      continue;
    
    if (strncasecmp( this->m_command[i].command, this->m_argv[0], strlen(this->m_command[i].command)) == 0)
    {
      char *argv[CONSOLE_MAX_ARGS_NUM];

      for (int j = 0 ; j < this->m_argc ; j++)
      {
        argv[j] = this->m_argv[j];
      }

      this->m_command[i].function(this,this->m_argc, argv);
      valid_command = true;
      break;
    }
  }

  if (valid_command != true)
    this->printf("    ERROR: Unknown command: \"%s\"\r\n",this->m_argv[0]);
}

void ArduinoCLI::loop()
{
  this->readCommand();
}

void ArduinoCLI::registerCommand(const char *command,const char *description, int (*function)(ArduinoCLI *,int, char **))
{
  if (command == NULL )
  {
    return;
  }

  for (int i = 0 ; i < CONSOLE_MAX_COMMAND_NUM ; i++)
  {
    if ( this->m_command[i].registered == true)
      continue;
    else
    {
      this->m_command[i].registered = true;
      this->m_command[i].function = function;

      if (strlen(command) < CONSOLE_CMD_NAME_SIZE)
        strcpy(this->m_command[i].command,command);
      if (strlen(description) < CONSOLE_CMD_DESCRIPTION_SIZE )
        strcpy(this->m_command[i].description,description);
      break;
    }
  }
}

void ArduinoCLI::help()
{
  this->printf("\r\n");
  
  for (int i = 0 ; i < CONSOLE_MAX_COMMAND_NUM ; i++)
  {
    if ( this->m_command[i].registered == false)
      continue;

    this->printf("    %16s\t --- \t%s\r\n",this->m_command[i].command,this->m_command[i].description);
  }

  this->printf("\r\n");
}

ArduinoCLI CLI;