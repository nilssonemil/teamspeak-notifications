#include "config.h"
#include "inih-r42/ini.h"
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>

/* Default configuration location */
const char *DEFAULT_CONFIG = 
  "/.ts3client/plugins/teamspeak-notifications/config.ini";

static int handler(void *user, const char *section, const char *name,
    const char *value)
{
  struct Conf *pconf = (struct Conf*) user;

  #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
  if (MATCH("plugin", "show_welcome"))
  {
    pconf->show_welcome = strcmp(value, "false") ? 1 : 0;
  }
  else {
    return 0; // unknown section/name
  }
  return 1;
}

/* Set the default values of the config struct. */
void set_default_values(struct Conf *conf)
{
  conf->show_welcome = 1;
}

/**
 * Load the configuration file.
 * Return -1 if the config cannot be loaded, otherwise 0.
 */
int init_config(struct Conf *conf)
{
  set_default_values(conf);

  // use $HOME / DEFAULT_CONFIG
  char *fname;
  const char *home = getenv("HOME");
  fname = malloc(strlen(home) + strlen(DEFAULT_CONFIG) + 1);
  strcpy(fname, home);
  strcat(fname, DEFAULT_CONFIG);

  if (ini_parse(fname, handler, conf) < 0
      && access(fname, F_OK) != -1)
    return -1; // configuration exists and cannot be loaded

  return 0;
}

