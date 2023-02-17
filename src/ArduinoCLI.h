#ifndef _ARDUINO_CLI_H
#define _ARDUINO_CLI_H

#if ARDUINO >= 100
# include <Arduino.h>
#else
# include <WProgram.h>
#endif

#include <stdlib.h>

#define CONSOLE_MAX_ARGS_NUM    8
#define CONSOLE_MAX_ARG_BUFFER  16
#define CONSOLE_INPUT_BUFFER    128
#define CONSOLE_PROMPT_BUFFER   16
#define CONSOLE_WELCOME_BUFFER  128

#define CONSOLE_CMD_NAME_SIZE   CONSOLE_MAX_ARG_BUFFER
#define CONSOLE_CMD_DESCRIPTION_SIZE 32

class ArduinoCLI;

typedef struct _CLICommand {
    bool registered;
    char command[CONSOLE_CMD_NAME_SIZE];
    char description[CONSOLE_CMD_DESCRIPTION_SIZE];
    int (*function)(ArduinoCLI *,int argc, char **argv);    
} CLICommand;

#define CONSOLE_MAX_COMMAND_NUM 32

#define ARDUINO_CLI_COMMAND(X) int X(ArduinoCLI *cli,int argc, char **argv)

class ArduinoCLI
{
  public:
    ArduinoCLI();
    ~ArduinoCLI();
    void bind(Stream* port);
    void setDefaultWelcome(const char *p);
    void setDefaultPrompt(const char *p);
    void registerCommand(const char *command,const char *description, int (*function)(ArduinoCLI *,int, char **));
    void loop();

    void printf(char *format,...);
    void help();

  protected:
    void print(char* buffer,int len);
    void welcome();
    void prompt();
    void readCommand();
    void parseCommand();
    void executeCommand();
    void clearBuffer();
    
  private:
    Stream          *m_port;
    char            m_welcome[CONSOLE_WELCOME_BUFFER];
    char            m_prompt[CONSOLE_PROMPT_BUFFER];
    char            m_buffer[CONSOLE_INPUT_BUFFER];
    char            m_readByte;
    int             m_readBytes;    
    char            m_argv[CONSOLE_MAX_ARGS_NUM][CONSOLE_MAX_ARG_BUFFER];
    int             m_argc;

    CLICommand      m_command[CONSOLE_MAX_COMMAND_NUM];

    char            m_outputBuffer[2048];
};

extern ArduinoCLI CLI;

#endif /* _ARDUINO_CLI_H */