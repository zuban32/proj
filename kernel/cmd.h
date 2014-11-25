#ifndef CMD_H_INCLUDED
#define CMD_H_INCLUDED

#define CMD_NUM 6
#define CMD_NAME_LEN 80
#define MAX_PARAMS 2

#define TESTBASE 0x1000000
#define CHECKADDR 0x27ff010

void cmd(void);
void parse_cmd(void);

void cmd_isr(void);
void cmd_listisr(void);
void cmd_int(void);
void cmd_clear(void);
void cmd_help(void);
void cmd_check(void);

#endif