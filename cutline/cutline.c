#include <iostream>
#include <fstream>
#include <string>
#define len 100
using namespace std;
struct file{    
    char s[len];//这个结构里面存储每行的字符串
    int value;  //存储行号
}F[1000];
void CutLine(struct file *F,int count){//去除行号
    for(int i=1;i<count;i++){
        for(int j=0;j<100;j++){
            if(F[i].s[j]=='.'){  //找到.的位置然后将字符串覆盖掉
                for(int k=j+1;k<100;k++){
                    F[i].s[k-(j+1)] =F[i].s[k];
                }
            }
        }
    }

    fstream f1("d:\\文件.txt",ios::out);
    if(!f1) cout<<"mistake";
    for(i=1;i<count;i++){
        f1<<F[i].s<<endl;
    }
    f1.close();
}
void AddLine(struct file *F,int count){//加上行号和.(只要主函数里面打开的是没有行号和点的文件即可)
    fstream f("d:\\添加行号的文件文件.txt",ios::out);
    for(int i=1;i<count;i++){
            f<<F[i].value<<". "<<F[i].s<<endl;
    }
    f.close();
}
int main()
{   
    int count=1;
    //下面代码会把文件每行内容读入内存
    fstream f("d:\\文件.txt",ios::in |ios::out);
    if(!f){cout<<"mistake";}
    while(!f.eof()){
        f.getline(F[count].s,len);
        F[count].value=count;
        count++;
    }   
    f.close();  
    CutLine(F,count);//去除行号
    return 0;
}

