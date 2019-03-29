#ifndef _SYSCFG_H_
#define _SYSCFG_H_

float read_conf_value(const char *key,  FILE *fp);
int write_conf_value(const char *key, float value, FILE *fp);

FILE * read_conf_pre(const char *file);
int read_conf_ok(FILE *fp);

int write_conf_ok(FILE *fp);
FILE * write_conf_pre(const char *file);


#endif //end of FILE_H_

