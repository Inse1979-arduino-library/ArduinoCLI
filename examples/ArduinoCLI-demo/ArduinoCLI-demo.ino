#include <ArduinoCLI.h>

ARDUINO_CLI_COMMAND(help) {
  cli->help();
  return 0; 
}

ARDUINO_CLI_COMMAND(demo) {
  for (int i = 0 ; i < argc; i++)
    cli->printf("    argc %d: %s\r\n",i,argv[i]);
  return 0; 
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  CLI.setDefaultWelcome("Welcome to this simple Arduino command line interface (CLI).\r\nType \"help\" to see a list of commands.\r\n");
  CLI.setDefaultPrompt("ARDUINO > ");
  CLI.bind(&Serial);
  CLI.registerCommand("help"      ,"print this screen"             ,help);  
  CLI.registerCommand("demo"      ,"print command line info"       ,demo); 
}

void loop() {
  // put your main code here, to run repeatedly:
  CLI.loop();

}
