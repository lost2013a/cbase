#include <iostream>
#include <fstream>
#include <string>
#define len 100
using namespace std;
struct file{    
    char s[len];//����ṹ����洢ÿ�е��ַ���
    int value;  //�洢�к�
}F[1000];
void CutLine(struct file *F,int count){//ȥ���к�
    for(int i=1;i<count;i++){
        for(int j=0;j<100;j++){
            if(F[i].s[j]=='.'){  //�ҵ�.��λ��Ȼ���ַ������ǵ�
                for(int k=j+1;k<100;k++){
                    F[i].s[k-(j+1)] =F[i].s[k];
                }
            }
        }
    }

    fstream f1("d:\\�ļ�.txt",ios::out);
    if(!f1) cout<<"mistake";
    for(i=1;i<count;i++){
        f1<<F[i].s<<endl;
    }
    f1.close();
}
void AddLine(struct file *F,int count){//�����кź�.(ֻҪ����������򿪵���û���кź͵���ļ�����)
    fstream f("d:\\����кŵ��ļ��ļ�.txt",ios::out);
    for(int i=1;i<count;i++){
            f<<F[i].value<<". "<<F[i].s<<endl;
    }
    f.close();
}
int main()
{   
    int count=1;
    //����������ļ�ÿ�����ݶ����ڴ�
    fstream f("d:\\�ļ�.txt",ios::in |ios::out);
    if(!f){cout<<"mistake";}
    while(!f.eof()){
        f.getline(F[count].s,len);
        F[count].value=count;
        count++;
    }   
    f.close();  
    CutLine(F,count);//ȥ���к�
    return 0;
}

