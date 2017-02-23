#ifndef UTILTOOL_H_
#define UTILTOOL_H_

#include <string>
#include <vector>

using namespace std;

class utiltool
{
public:
	utiltool(void);
	~utiltool(void);

     static vector<string> string_split(string str,string pattern);
};
#endif
