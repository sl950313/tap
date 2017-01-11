#ifndef CONFIG_H
#define CONFIG_H

struct Configure {
   char username[64];
   char passwd[64];
   char auth_code[513];

   char ip_md[32];
   int port_md;
   char ip_td[32];
   int port_td;

   char log_path[64];
};

void loadConfigureFile(char *config_filename, Configure &config);
#endif // CONFIG_H
