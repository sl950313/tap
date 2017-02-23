#include "config.h"
#include "utiltool.h"
#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <string>

using namespace std;

void loadConfigureFile(char *config_filename, Configure &config) {
   cout << "configure file name : " << config_filename << endl;
   GKeyFile *keyfile;
   GError *err = NULL;
   keyfile = g_key_file_new();
   g_key_file_load_from_file(keyfile, (gchar *)config_filename, G_KEY_FILE_NONE, &err);
   char *username_md = g_key_file_get_string(keyfile, "Account", "username_md", &err);
   strcpy(config.username_md, username_md);
   free(username_md);
   char *passwd_md = g_key_file_get_string(keyfile, "Account", "password_md", &err);
   strcpy(config.passwd_md, passwd_md);
   free(passwd_md);
   char *username_td = g_key_file_get_string(keyfile, "Account", "username_td", &err);
   strcpy(config.username_td, username_td);
   free(username_td);
   char *passwd_td = g_key_file_get_string(keyfile, "Account", "password_td", &err);
   strcpy(config.passwd_td, passwd_td);
   free(passwd_td);
   char *auth_code = g_key_file_get_string(keyfile, "Account", "authCode_md", &err);
   strcpy(config.auth_code_md, auth_code);
   free(auth_code);
   char *auth_code_td = g_key_file_get_string(keyfile, "Account", "authCode_td", &err);
   strcpy(config.auth_code_td, auth_code_td);
   free(auth_code_td);

   char *ip_md = g_key_file_get_string(keyfile, "FrontAddress", "MDAddress", &err);
   strcpy(config.ip_md, ip_md);
   free(ip_md);
   config.port_md = atoi(g_key_file_get_string(keyfile, "FrontAddress", "MDPort", &err));
   char *ip_td = g_key_file_get_string(keyfile, "FrontAddress", "TDAddress", &err);
   strcpy(config.ip_td, ip_td);
   free(ip_td);
   config.port_td = atoi(g_key_file_get_string(keyfile, "FrontAddress", "TDPort", &err));

   char *log_path = g_key_file_get_string(keyfile, "Configure", "logPath", &err);
   strcpy(config.log_path, log_path);
   free(log_path);

   char *array_size_for_strategy = g_key_file_get_string(keyfile, "Strategy", "array_size_for_strategy", &err);
   config.array_size_for_strategy = atol(array_size_for_strategy);

   char *contract_info = g_key_file_get_string(keyfile, "Contract", "contract", &err);
   string contract_all = contract_info;
   std::vector<std::string> contracts = utiltool::string_split(contract_info, ";");  
   for (size_t i = 0; i < contracts.size(); ++i) {
      int space = contracts[i].find(' ');
      //asert(space != -1);
      string names = contracts[i].substr(0, space);
      string times = contracts[i].substr(space + 1, contracts[i].length() - 1);
      std::vector<std::string> name_v = utiltool::string_split(names, ",");
      //contract ct;
      treaty *tv = new treaty();
      //memset(&ct, 0, sizeof(ct));
      strcpy(tv->exchange_name, name_v[0].c_str());
      strcpy(tv->commodity, name_v[1].c_str());
      strcpy(tv->contract_no, name_v[2].c_str());
      tv->quote_interval = atoi(name_v[3].c_str());

      std::vector<std::string> time_v = utiltool::string_split(times, ",");
      for (size_t j = 0; j < time_v.size(); ++j) {
         start_end_time set;
         set.start_time = time_v[j].substr(0, time_v[j].find('-'));
         set.end_time = time_v[j].substr(time_v[j].find('-') + 1, time_v[j].length() - 1);
         tv->se.push_back(set); 
      }
      sprintf(tv->_log_file, "%s/%s%s%s.log", config.log_path, (name_v[0]).c_str(), name_v[1].c_str(), name_v[2].c_str());
      tv->log_file = fopen(tv->_log_file, "a+");
      //tv->array_size_for_strategy = config.array_size_for_strategy;
      tv->setArraySizeAndAllocMem(config.array_size_for_strategy);
      config.all_treatys.push_back(tv);
   }
}
