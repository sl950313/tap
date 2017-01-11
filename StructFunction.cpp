//#include <afx.h>
#include<fstream>
//#include<io.h>
#include<algorithm>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "StructFunction.h"


//读取历史K线
void ReadDatas(string fileName, vector<History_data> &history_data_vec)
{
	History_data stu;
	char dataline[512];//行数变量
	bzero(dataline, 0);
	ifstream file1(fileName.c_str(), ios::in);	//以只读方式读入,读取原始数据
	if(!file1){
		cout<<"no such file!"<<endl;
		abort();
	}	
	while(file1.getline(dataline,1024,'\n'))//while开始，读取一行1024够大？
	{
		int len=strlen(dataline);
		char tmp[20];
		for(int i=0,j=0,count=0;i<=len;i++){
			if(dataline[i]!= ',' && dataline[i]!='\0'){
				tmp[j++]=dataline[i];
			}
			else{
				tmp[j]='\0';
				count++;
				j=0;
				switch(count){
				case 1:
					stu.date=tmp;
					break;
				case 2:
					stu.time=tmp;
					break;
				case 3:
					stu.buy1price=atof(tmp);
					break;
				case 4:
					stu.sell1price=atof(tmp);
					break;
				case 5:
					stu.open=atof(tmp);
					break;
				case 6:
					stu.high=atof(tmp);
					break;
				case 7:
					stu.low=atof(tmp);
					break;

					break;
				case 8:
					stu.close=atof(tmp);
					break;

				}
			}
		}
		history_data_vec.push_back(stu);
	}

	file1.close();
}

/*
 * @shilei the function of operating director is different. thus we implement the Store_filename
 * 2016.12.23
 */
int Store_fileName(string path, vector<string> &FileName) {
   DIR *dir;
   printf("path = %s\n", path.c_str());
   if (!(dir = opendir(path.c_str()))) {
      printf("error open path %s, store filename error\n", path.c_str());
      return -1;
   }
   struct dirent *d_ent;
   char fullpath[128];
   bzero(fullpath, 128);
   if (path[path.length() - 1] == '/') {
      path[path.length() - 1] = '\0';
   }

   string in_name;
   int num = 1;
   while ((d_ent = readdir(dir))) {
      struct stat file_stat;
      bzero(fullpath, 128);
      memcpy(fullpath, path.c_str(), path.length());
      strcat(fullpath, "/");
      strcat(fullpath, d_ent->d_name);
      if (lstat(fullpath, &file_stat)) {
         printf("runtime error\n");
         return -1;
      }
      if ((strcmp(d_ent->d_name, ".") != 0) && (strcmp(d_ent->d_name,  "..") != 0)) {
         printf("For Debug: file %d name : %s\n", num++, d_ent->d_name);
         in_name.append(fullpath);
         FileName.push_back(in_name);
         //FileName.push_back((string)(d_ent->d_name));
      }
      in_name.clear();
   }
   closedir(dir);
   return 0;
}
/*
int Store_fileName(string path, vector<string> &FileName)
{
	struct _finddata_t fileinfo;  
	string in_path;  
	string in_name;  

	//in_path="d:\\future_data";
	in_path = path;
	string curr = in_path+"\\*.txt"; 
	long handle;  

	if((handle=_findfirst(curr.c_str(),&fileinfo))==-1L)  
	{  
		cout<<"没有找到匹配文件!"<<endl;

		return 0;  
	}  

	else  
	{
		do
		{
			in_name=in_path + "\\" +fileinfo.name; 
			FileName.push_back(in_name);
		}while(!(_findnext(handle,&fileinfo)));

		_findclose(handle);

		return 0;

	}
}
*/
