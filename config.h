#ifndef CONFIG_H
#define CONFIG_H
#include <string>
#include <vector>
#include <pthread.h>
#include "SimpleEvent.h"
#include "strategy.h"
#include "treaty.h"

/*
struct contract {
public:
   void receiveData(TapAPIQuoteWhole *info);
private:
   char exchange_name[16];
   char commodity[16];
   char contract_no[16];
   int quote_interval; // 接受到合约的间隔. 125ms, 250ms or 500ms.  125 is not used.
   std::vector<start_end_time> se;

   pthread_t pid;
   SimpleEvent ev;
   Strategy *strg;
};
*/

struct Configure {
   char username_md[64];
   char passwd_md[64];
   char username_td[64];
   char passwd_td[64];
   char auth_code_md[513];
   char auth_code_td[513];
   std::vector<treaty *> all_treatys;

   char ip_md[32];
   int port_md;
   char ip_td[32];
   int port_td;

   char log_path[64];
   long array_size_for_strategy;
}; 

void loadConfigureFile(char *config_filename, Configure &config);
#endif // CONFIG_H
