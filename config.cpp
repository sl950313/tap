#include "config.h"
#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

using namespace std;

void loadConfigureFile(char *config_filename, Configure &config) {
   cout << "configure file name : " << config_filename << endl;
   GKeyFile *keyfile;
   GError *err = NULL;
   keyfile = g_key_file_new();
   g_key_file_load_from_file(keyfile, (gchar *)config_filename, G_KEY_FILE_NONE, &err);
   char *username_md = g_key_file_get_string(keyfile, "Account", "username_md", &err);
   strcpy(config.username_md, username_md);
   char *passwd_md = g_key_file_get_string(keyfile, "Account", "password_md", &err);
   strcpy(config.passwd_td, passwd_md);
   char *username_td = g_key_file_get_string(keyfile, "Account", "username_td", &err);
   strcpy(config.username_td, username_td);
   char *passwd_td = g_key_file_get_string(keyfile, "Account", "password_td", &err);
   strcpy(config.passwd_td, passwd_td);
   char *auth_code = g_key_file_get_string(keyfile, "Account", "authCode", &err);
   strcpy(config.auth_code, auth_code);

   char *ip_md = g_key_file_get_string(keyfile, "FrontAddress", "MDAddress", &err);
   strcpy(config.ip_md, ip_md);
   config.port_md = atoi(g_key_file_get_string(keyfile, "FrontAddress", "MDPort", &err));
   char *ip_td = g_key_file_get_string(keyfile, "FrontAddress", "TDAddress", &err);
   strcpy(config.ip_td, ip_td);
   config.port_td = atoi(g_key_file_get_string(keyfile, "FrontAddress", "TDPort", &err));

   char *log_path = g_key_file_get_string(keyfile, "Configure", "logPath", &err);
   strcpy(config.log_path, log_path);
}
