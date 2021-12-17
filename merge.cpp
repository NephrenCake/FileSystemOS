////////////////////////////////////////////////////////////////////////////
//  
//  ��������ļ�����OSʵ��ο�����2010.CPP
//
////////////////////////////////////////////////////////////////////////////
//
// �����мٶ���
//   FAT[K]�洢FAT����������K��Ϊ5000������FAT[0]��ſ��̿�����
//   Disk[K][R]���ڴ洢��Ӧ���̿������(char��)��ÿ���̿�����R(=64)���ַ���
//   �ļ�Ŀ¼��FCB�У��ļ�����fattrib=1��ʾ��ֻ������=2��ʾ�����ء���=4��ʾ��ϵͳ����
//   �ļ�����fattrib=16��ʾ����Ŀ¼�������ļ��������Կ��Ի�����ϡ�
//   �û����ļ���UOF�У�״̬state=0��ʾ�յǼ�����=1��ʾ����������=2��ʾ���򿪡�״̬��
//   UOF�С��ļ����ԡ���Ϊ���򿪵��ļ������ԣ����ڡ�ֻ�����ļ����򿪺�ֻ�ܶ�������д��
//
//   ��ϵͳ�������������ļ�����Ŀ¼�����ִ�Сд�⣬���ಿ����ĸ�������ִ�Сд��
//
////////////////////////////////////////////////////////////////////////////
//
// ��ģ���ļ�ϵͳ���������²������
// dir [<Ŀ¼��>]������ʾ·����ָ����Ŀ¼���ݣ�
// cd [<Ŀ¼��>]����ָ����ǰĿ¼��·���С�..����ʾ��Ŀ¼��
// md <Ŀ¼��>����������Ŀ¼��
// rd <Ŀ¼��>����ɾ����Ŀ¼��
// create <�ļ���>[ <�ļ�����>]���������ļ���
// open <�ļ���>�������ļ���
// write <�ļ���> [<λ��/app>[ insert]]����д�ļ���
// read <�ļ���> [<λ��m> [<�ֽ���n>]]�������ļ���
// close <�ļ���>�����ر��ļ���
// ren <ԭ�ļ���> <���ļ���>�����ļ�������
// copy <Դ�ļ���> [<Ŀ���ļ���>]���������ļ���
// closeall�����رյ�ǰ�û������д򿪵��ļ�
// del <�ļ���>����ɾ��ָ�����ļ�
// type <�ļ���>������ʾָ���ļ������ݣ�
// undel [<Ŀ¼��>]�����ָ�ָ��Ŀ¼�б�ɾ�����ļ�
// help������ʾ�������ʹ�ø�ʽ��
// attrib <�ļ���> [��<����>]������ʾ[�޸�]�ļ�/Ŀ¼���ԡ�
// rewind <�ļ���>��������дָ���Ƶ��ļ���ͷ(��һ���ֽڴ�)
// fseek <�ļ���> <λ��n>����������дָ�붼�Ƶ�ָ��λ��n����
// block <�ļ���>������ʾ�ļ���Ŀ¼ռ�õ��̿�š�
// uof������ʾ�û���UOF(�ļ��򿪱�)��
// prompt������ʾ������ʾ/����ʾ��ǰĿ¼���л���
// fat������ʾģ����̵Ŀ��п���(FAT����0�ĸ���)��
// check�������˶�FAT���Ӧ�Ŀ��п�����
// exit������������������С�
//
////////////////////////////////////////////////////////////////////////////

#include <iostream>	//cout,cin
#include <iomanip>	//setw(),setiosflags()
#include <stdlib.h>		//exit(),atoi()
#include <string.h>		//strcpy(),_stricmp()
#include <fstream>	//�ļ�������
#include <string>
#include <sstream>
#include <algorithm>
using namespace std;

//�������ɷ��ų���
#define S 32			//�������ͬʱ��32���ļ�
#define K 5000			//������̹���5000���̿�
#define SIZE 64			//������̵Ĵ�С��64�ֽ�
#define CK 8			//����ֽ��Ķ���
#define INPUT_LEN 128	//���뻺��������
#define COMMAND_LEN 11	//�����ַ�������
#define FILENAME_LEN 11	//�ļ�������
#define PATH_LEN INPUT_LEN-COMMAND_LEN
#define DM 40			//�ָ���ɾ���ļ���ı�����


struct FCB			//�����ļ�Ŀ¼��FCB�Ľṹ(��16���ֽ�)
{
	char FileName[FILENAME_LEN];//�ļ���(1��10�ֽ�)
	char Fattrib;				//�ļ�����
	short int Addr;				//�ļ��׿��
	short int Fsize;			//�ļ�����
};

struct UOF			//�����û����ļ���Ľṹ
{
	char fname[PATH_LEN];		//�ļ�ȫ·����
	char attr;					//�ļ����ԣ�1=ֻ�ɶ���0=�ɶ�д
	short int faddr;			//�ļ����׿��
	short int fsize;			//�ļ���С(�ֽ���)
	FCB *fp;					//���ļ���Ŀ¼��ָ��
	short int state;			//״̬��0=�ձ��1=�½���2=��
	short int readp;			//��ָ�룬ָ��ĳ��Ҫ�����ַ�λ�ã�0=���ļ�
	short int writep;			//д��ָ�룬ָ��ĳ��Ҫд�����ַ�λ��
};

struct CurPath		//����洢��ǰĿ¼�����ݽṹ
{
	short int fblock;			//��ǰĿ¼���׿��
	char cpath[PATH_LEN];		//��ǰĿ¼����·���ַ���(ȫ·����)
};

struct UnDel		//�ָ���ɾ���ļ�������ݽṹ
{
	char gpath[PATH_LEN];		//��ɾ���ļ���ȫ·����(�����ļ���)
	char ufname[FILENAME_LEN];	//��ɾ���ļ���
	short ufaddr;				//��ɾ���ļ������׿��
	short fb;					//�洢��ɾ���ļ���ŵĵ�һ�����(����ͷָ��)
								//�׿��Ҳ����fb��ָ���̿���
};

//���ڻָ���ɾ���ļ����⣬�����Բ���������Windows�Ļ���վ�ķ�������������ڸ�Ŀ¼��
//����һ��������ļ���recycled (������Ϊ��ֻ�������ء�ϵͳ)����FCB��¼�ṹ�еĳ�Ա
//Fsize���������洢�ļ����ȣ��������洢һ���̿�ţ����̿��д洢�ļ����Ⱥ��ļ���ȫ·
//����(�����ļ���)������ġ�ȫ·�����������ļ���ԭλ�ã���ԭ�ļ�ʱ����Ϣ�ǲ��ɻ�ȱ�ġ�
//dir�������recycled�ļ���ʱ������ͨ�ļ������в�ͬ(�����ļ����ȵ���ϢҪ��Fsize��
//�̿���ȡ��������ֱ�ӻ��)��rd����Ӧ�޸ĳɲ���ɾ���ļ���recycled��copy,move,replace
//������Ҳ�ĳɲ��ܶ��ļ���recycled������

//����del����ɾ���ļ�ʱ,�����ļ����й���Ϣ���浽������ļ���recycled�У��༴���ļ����ᡱ
//������վ���ļ�ռ�õĴ��̿ռ䲢���ͷţ��ָ�ʱ�����෴����ջ���վʱ���ͷŴ��̿ռ䡣
//�˷�����ǰ��UnDel�ṹ�ķ����ķѸ���Ĵ��̿ռ�(ɾ�����ļ���ռ�ô��̿ռ�)

int FAT[K];						//FAT��,�̿���ΪK
//char (*Disk)[SIZE]=new char [K][SIZE];//������̿ռ䣬ÿ���̿�����ΪSIZE���ֽ�
char Disk[K][SIZE];
UOF uof[S];						//�û����ļ���UOF,���ͬʱ��S���ļ�
char comd[CK][PATH_LEN];		//��������ʱʹ��
char temppath[PATH_LEN];		//��ʱ·��(ȫ·��)
CurPath curpath;
UnDel udtab[DM];				//����ɾ���ļ��ָ����˳�ϵͳʱ�ñ�ɴ����ļ�UdTab.dat��
short Udelp=0;					//udtab��ĵ�һ���ձ�����±꣬ϵͳ��ʼ��ʱ��Ϊ0��
								//��Udelp=DMʱ����ʾ�����������������ı���(������������ǰ��)
short ffbp=1;
//0���̿��д洢�������ݣ�
//	short ffbp;		//�Ӹ�λ�ÿ�ʼ���ҿ����̿�(����ѭ���״���Ӧ����)
//	short Udelp;	//udtab��ĵ�һ���ձ�����±�

int dspath=1;		//dspath=1,��ʾ������ʾ��ǰĿ¼

//����ԭ��˵��
int CreateComd(int);			//create�������
int OpenComd(int);				//open�������
int ReadComd(int);				//read�������
int WriteComd(int);				//write�������
int CloseComd(int);				//close�������
void CloseallComd(int);			//closeaal�������, �رյ�ǰ�û����д򿪵��ļ�
int DelComd(int);				//del�������
int UndelComd(int);				//undel����������ָ���ɾ���ļ�
int CopyComd(int);				//copy�������
int DirComd(int);				//dir�����������ʾָ�����ļ�Ŀ¼����Ƶ��ʹ��
int CdComd(int);				//cd�������
int MdComd(int);				//md�������
int RdComd(int);				//rd�������
int TypeComd(int);				//type�������
int RenComd(int);				//ren�������
int AttribComd(int);			//attrib�������
void UofComd(void);				//uof�������
void HelpComd(void);			//help�������
int FindPath(char*,char,int,FCB* &);	//��ָ��Ŀ¼(���׿��)
int FindFCB(char*,int,char,FCB* &);		//��ָ�����ļ���Ŀ¼
int FindBlankFCB(short s,FCB* &fcbp1);	//Ѱ���׿��Ϊs��Ŀ¼�еĿ�Ŀ¼��
int RewindComd(int);			//rewind�������, ����дָ���Ƶ��ļ���ͷ(��һ���ֽڴ�)
int FseekComd(int);				//fseek�������, ����дָ���Ƶ��ļ���n���ֽڴ�
int blockf(int);				//block�������(��ʾ�ļ�ռ�õ��̿��)
int delall(int);				//delall�������, ɾ��ָ��Ŀ¼�е������ļ�
void save_FAT(void);
void save_Disk(void);
int getblock(void);				//���һ���̿�
void FatComd(void);
void CheckComd(void);
int Check_UOF(char*);
void ExitComd(void);
bool IsName(char*);				//�ж������Ƿ���Ϲ���
void PromptComd(void);			//prompt�����ʾ���Ƿ���ʾ��ǰĿ¼���л�
void UdTabComd(void);			//udtab�����ʾudtab������
void releaseblock(short s);		//�ͷ�s��ʼ���̿���
int buffer_to_file(FCB* fcbp,char* Buffer);	//Bufferд���ļ�
int file_to_buffer(FCB* fcbp,char* Buffer);	//�ļ����ݶ���Buffer,�����ļ�����
int ParseCommand(char *);		//������������зֽ������Ͳ�����
void ExecComd(int);				//ִ������
int MoveComd(int);
int BatchComd(int);
int FcComd(int);				//fc�������
int ReplaceComd(int);			//replace���ʵ���ļ�ȡ��

// todo ��ʱ�ᵼ�� copy * temp ��ѭ����
// #define INIT	//������ʼ�����ǴӴ��̶���

int main()
{

	char cmd[INPUT_LEN];			//�����л�����
	int i,k;
	// ����ϵͳʱ����ǰĿ¼�Ǹ�Ŀ¼
	curpath.fblock=1;				//��ǰĿ¼(��Ŀ¼)���׿��
	strcpy(curpath.cpath,"/");		//��Ŀ¼��·���ַ���

#ifdef INIT

	int j;
	FCB *fcbp;
	// *********** ��ʼ��FAT��Disk ************
	for (i=0;i<K;i++)	//��ʼʱ�����̿����
		FAT[i]=0;		//�����̿���
	FAT[0]=K-1;			//FAT[0]�б�������̿���
	for (i=1;i<30;i++)	//�����Ŀ¼�̿���
	{
		FAT[i]=i+1;		//��ʼ����Ŀ¼��FAT��
		FAT[0]--;		//���̿�����1
	}
	FAT[i]=-1;			//��Ŀ¼β���
	FAT[0]--;			//���̿�����1
	for (i++;i<40;i++)
	{
		FAT[i]=-1;		//����Ŀ¼β���
		FAT[0]--;
	}
	// *********** ��ʼ��Disk ************
	fcbp=(FCB*) Disk[1];
	j=40*SIZE/sizeof(FCB);
	for (i=1;i<=j;i++)
	{
		fcbp->FileName[0]='\0';	//��ʼĿ¼����Ŀ¼�г�ʼ��Ϊ��Ŀ¼��
		fcbp++;
	}
	//���½�����ʼĿ¼���и�����Ŀ¼
	fcbp=(FCB*) Disk[1];
	strcpy(fcbp->FileName,"bin");	//��Ŀ¼bin
	fcbp->Fattrib=16;				//��ʾ����Ŀ¼
	fcbp->Addr=31;					//����Ŀ¼�����̿����31
	fcbp->Fsize=0;					//Լ����Ŀ¼�ĳ���Ϊ0
	fcbp++;							//ָ����һ��Ŀ¼��
	strcpy(fcbp->FileName,"usr");	//��Ŀ¼usr
	fcbp->Fattrib=16;				//��ʾ����Ŀ¼
	fcbp->Addr=32;					//����Ŀ¼�����̿����32
	fcbp->Fsize=0;					//Լ����Ŀ¼�ĳ���Ϊ0
	fcbp++;
	strcpy(fcbp->FileName,"auto");	//�ļ�unix��Ŀ¼��
	fcbp->Fattrib=0;				//��ʾ����ͨ�ļ�
	fcbp->Addr=0;					//����Ŀ¼�����̿����0����ʾ�ǿ��ļ�
	fcbp->Fsize=0;					//���ļ��ĳ���Ϊ0
	fcbp++;
	strcpy(fcbp->FileName,"dev");	//��Ŀ¼etc
	fcbp->Fattrib=16;				//��ʾ����Ŀ¼
	fcbp->Addr=33;					//����Ŀ¼�����̿����33
	fcbp->Fsize=0;					//Լ����Ŀ¼�ĳ���Ϊ0
	fcbp=(FCB*) Disk[31];
	strcpy(fcbp->FileName,"..");	//bin�ĸ�Ŀ¼��Ӧ��Ŀ¼��
	fcbp->Fattrib=16;				//��ʾ��Ŀ¼�������ļ�
	fcbp->Addr=1;					//��Ŀ¼(�˴��Ǹ�Ŀ¼)�����̿����1
	fcbp->Fsize=0;					//Լ����Ŀ¼�ĳ���Ϊ0
	fcbp=(FCB*) Disk[32];
	strcpy(fcbp->FileName,"..");	//usr�ĸ�Ŀ¼��Ӧ��Ŀ¼��
	fcbp->Fattrib=16;				//��ʾ��Ŀ¼�������ļ�
	fcbp->Addr=1;					//��Ŀ¼(�˴��Ǹ�Ŀ¼)�����̿����1
	fcbp->Fsize=0;					//Լ����Ŀ¼�ĳ���Ϊ0
	fcbp++;
	strcpy(fcbp->FileName,"user");	//��Ŀ¼lib
	fcbp->Fattrib=16;				//��ʾ����Ŀ¼
	fcbp->Addr=34;					//����Ŀ¼�����̿����34
	fcbp->Fsize=0;					//Լ����Ŀ¼�ĳ���Ϊ0
	fcbp++;
	strcpy(fcbp->FileName,"lib");	//��Ŀ¼user
	fcbp->Fattrib=16;				//��ʾ����Ŀ¼
	fcbp->Addr=35;					//����Ŀ¼�����̿����35
	fcbp->Fsize=0;					//Լ����Ŀ¼�ĳ���Ϊ0
	fcbp++;
	strcpy(fcbp->FileName,"bin");	//��Ŀ¼bin
	fcbp->Fattrib=16;				//��ʾ����Ŀ¼
	fcbp->Addr=36;					//����Ŀ¼�����̿����36
	fcbp->Fsize=0;					//Լ����Ŀ¼�ĳ���Ϊ0
	fcbp=(FCB*) Disk[33];
	strcpy(fcbp->FileName,"..");	//etc�ĸ�Ŀ¼��Ӧ��Ŀ¼��
	fcbp->Fattrib=16;				//��ʾ��Ŀ¼�������ļ�
	fcbp->Addr=1;					//��Ŀ¼(�˴��Ǹ�Ŀ¼)�����̿����1
	fcbp->Fsize=0;					//Լ����Ŀ¼�ĳ���Ϊ0
	fcbp=(FCB*) Disk[34];
	strcpy(fcbp->FileName,"..");	//lib�ĸ�Ŀ¼��Ӧ��Ŀ¼��
	fcbp->Fattrib=16;				//��ʾ��Ŀ¼�������ļ�
	fcbp->Addr=32;					//��Ŀ¼(�˴���usrĿ¼)�����̿����32
	fcbp->Fsize=0;					//Լ����Ŀ¼�ĳ���Ϊ0
	fcbp++;
	strcpy(fcbp->FileName,"lin");	//��Ŀ¼liu
	fcbp->Fattrib=16;				//��ʾ����Ŀ¼
	fcbp->Addr=37;					//����Ŀ¼�����̿����37
	fcbp->Fsize=0;					//Լ����Ŀ¼�ĳ���Ϊ0
	fcbp++;
	strcpy(fcbp->FileName,"sun");	//��Ŀ¼sun
	fcbp->Fattrib=16;				//��ʾ����Ŀ¼
	fcbp->Addr=38;					//����Ŀ¼�����̿����38
	fcbp->Fsize=0;					//Լ����Ŀ¼�ĳ���Ϊ0
	fcbp++;
	strcpy(fcbp->FileName,"ma");	//��Ŀ¼fti
	fcbp->Fattrib=16;				//��ʾ����Ŀ¼
	fcbp->Addr=39;					//����Ŀ¼�����̿����39
	fcbp->Fsize=0;					//Լ����Ŀ¼�ĳ���Ϊ0
	fcbp=(FCB*) Disk[35];
	strcpy(fcbp->FileName,"..");	//user�ĸ�Ŀ¼��Ӧ��Ŀ¼��
	fcbp->Fattrib=16;				//��ʾ��Ŀ¼�������ļ�
	fcbp->Addr=32;					//��Ŀ¼(�˴���usrĿ¼)�����̿����32
	fcbp->Fsize=0;					//Լ����Ŀ¼�ĳ���Ϊ0
	fcbp=(FCB*) Disk[36];
	strcpy(fcbp->FileName,"..");	//usr/bin�ĸ�Ŀ¼��Ӧ��Ŀ¼��
	fcbp->Fattrib=16;				//��ʾ��Ŀ¼�������ļ�
	fcbp->Addr=32;					//��Ŀ¼(�˴���usrĿ¼)�����̿����32
	fcbp->Fsize=0;					//Լ����Ŀ¼�ĳ���Ϊ0
	fcbp=(FCB*) Disk[37];
	strcpy(fcbp->FileName,"..");	//usr/lib/liu�ĸ�Ŀ¼��Ӧ��Ŀ¼��
	fcbp->Fattrib=16;				//��ʾ��Ŀ¼�������ļ�
	fcbp->Addr=34;					//��Ŀ¼(�˴���usr/libĿ¼)�����̿����34
	fcbp->Fsize=0;					//Լ����Ŀ¼�ĳ���Ϊ0
	fcbp=(FCB*) Disk[38];
	strcpy(fcbp->FileName,"..");	//usr/lib/sun�ĸ�Ŀ¼��Ӧ��Ŀ¼��
	fcbp->Fattrib=16;				//��ʾ��Ŀ¼�������ļ�
	fcbp->Addr=34;					//��Ŀ¼(�˴���usr/libĿ¼)�����̿����34
	fcbp->Fsize=0;					//Լ����Ŀ¼�ĳ���Ϊ0
	fcbp=(FCB*) Disk[39];
	strcpy(fcbp->FileName,"..");	//usr/lib/fti�ĸ�Ŀ¼��Ӧ��Ŀ¼��
	fcbp->Fattrib=16;				//��ʾ��Ŀ¼�������ļ�
	fcbp->Addr=34;					//��Ŀ¼(�˴���usr/libĿ¼)�����̿����34
	fcbp->Fsize=0;					//Լ����Ŀ¼�ĳ���Ϊ0

	// *********** ��ʼ��UnDel�� ************
	Udelp=0;

	ffbp=1;			//��FAT��ͷ���ҿ����̿�

#else

	// �����ļ������FAT
	char yn;
	ifstream ffi("FAT.txt",ios::_Nocreate);//���ļ�FAT.txt
	if (!ffi)
	{
		cout<<"Can't open FAT.txt!\n";
		cin>>yn;
		exit(0);
	}
	for (i=0;i<K;i++)		//���ļ�FAT.txt�����ļ������FAT
		if(ffi)
			ffi>>FAT[i];
		else
			break;
	ffi.close();

	//������̿�Disk[ ]��Ϣ
	ffi.open("Disk.dat",ios::binary|ios::_Nocreate);
	if (!ffi)
	{
		cout<<"Can't open Disk.dat!\n";
		cin>>yn;
		exit(0);
	}
	for (i=0;i<K;i++)		//���ļ�Disk.dat�����̿�����
		if(ffi)
			ffi.read((char*)&Disk[i],SIZE);
		else
			break;
	ffi.close();

	//����ָ�ɾ���ļ���UdTab.dat��Ϣ
	ffi.open("UdTab.dat",ios::binary|ios::_Nocreate);
	if (!ffi)
	{
		cout<<"Can't open UdTab.dat!\n";
		cin>>yn;
		exit(0);
	}
	for (i=0;i<DM;i++)		//���ļ�Disk.dat�����̿�����
		if(ffi)
			ffi.read((char*)&udtab[i],sizeof(udtab[0]));
		else
			break;
	ffi.close();

	short *pp=(short*) Disk[0];
	ffbp=pp[0];
	Udelp=pp[1];

#endif

	for (i=0;i<S;i++)		//��ʼ��UOF��state��0���ձ��1���½���2����
		uof[i].state=0;		//��ʼ��Ϊ�ձ���

	cout<<"\n���������������ֲ�������.\n"
		<<"Help ���� ���װ�����Ϣ.\n"
		<<"exit ���� �˳�������.\n";
	while (1)	//ѭ�����ȴ��û��������ֱ�����롰exit������ѭ�����������
	{			//�������������ִ������

		while (1)
		{
			cout<<"\nC:";					//��ʾ��ʾ��(��ϵͳ�ܼٶ���C��)
			if (dspath)
				cout<<curpath.cpath;
			cout<<">";
			cin.getline(cmd,INPUT_LEN);		//��������
			if (strlen(cmd)>0)
				break;
		}
        k=ParseCommand(cmd);		//�ֽ���������
									//comd[0]�������comd[1],comd[2]...�ǲ���
        ExecComd(k);				//ִ������
    }
}

/////////////////////////////////////////////////////////////////

void ExecComd(int k)		//ִ������
{
	int cid;				//�����ʶ

	//���������
	char CmdTab[][COMMAND_LEN]={"create","open","write","read","close",
		"del","dir","cd","md","rd","ren","copy","type","help","attrib",
		"uof","closeall","block","rewind","fseek","fat","check","exit",
		"undel","Prompt","udtab","move","batch","fc","replace" };
	int M=sizeof(CmdTab)/COMMAND_LEN;	//ͳ���������
	for (cid=0;cid<M;cid++)			//��������м�������
		if (_stricmp(CmdTab[cid],comd[0])==0)//������ִ�Сд
			break;
	//����������У��������ͨ��ȫ�ֱ���comd[][]���ݣ���δ�����ں�����������
	switch(cid)
	{
		 case 0:CreateComd(k);		//create���kΪ�����еĲ�������(��������),��ͬ
				break;
		 case 1:OpenComd(k);		//open������ļ�
				break;
		 case 2:WriteComd(k);		//write���д�ļ�
				break;
		 case 3:ReadComd(k);		//read������ļ�
				break;
		 case 4:CloseComd(k);		//close����ر��ļ�
				break;
		 case 5:DelComd(k);			//del���ɾ���ļ�
				break;
		 case 6:DirComd(k);			//dir�����ʾ�ļ�Ŀ¼
				break;
		 case 7:CdComd(k);			//cd���ָ����ǰĿ¼
				break;
		 case 8:MdComd(k);			//md���������Ŀ¼
				break;
		 case 9:RdComd(k);			//rd���ɾ����Ŀ¼
				break;
		 case 10:RenComd(k);		//ren����ļ�����
				 break;
		 case 11:CopyComd(k);		//copy��������ļ�
				 break;
		 case 12:TypeComd(k);		//type�����ʾ�ļ�����
				 break;
		 case 13:HelpComd();		//help�����ʾ������Ϣ
				 break;
		 case 14:AttribComd(k);		//attrib����޸��ļ�����
				 break;
		 case 15:UofComd();			//uof�����ʾ�ļ��򿪱�(���Գ�����)
				 break;
		 case 16:CloseallComd(1);	//closeall����ر������ļ�
				 break;
		 case 17:blockf(k);			//block�����ʾ�ļ����̿��(���Գ�����)
				 break;
		 case 18:RewindComd(k);		//rewind�������ָ���Ƶ��ļ���ͷ
				 break;
		 case 19:FseekComd(k);		//fseek���������дָ�붼�Ƶ�ָ����¼��
				 break;
		 case 20:FatComd();			//fat�����ʾFAT���п����(���Գ�����)
				 break;
		 case 21:CheckComd();		//check����˶�FAT���п����(���Գ�����)
				 break;
		 case 22:ExitComd();		//exit����˳�ϵͳ
				 break;
		 case 23:UndelComd(k);		//undel����ָ���ɾ���ļ�
				 break;
		 case 24:PromptComd();		//prompt�����ʾ����·������ʾ�����л�
				 break;
		 case 25:UdTabComd();		//udtab�����ʾ��ɾ���ļ���(���Գ�����)
				 break;
		 case 26:MoveComd(k);       //move����ļ�ת�Ƽ���Ŀ¼����
				 break;
		 case 27:BatchComd(k);
				 break;
		 case 28:FcComd(k);			//����fc���ʵ�������ļ��ıȽ�
			 break;
		 case 29:ReplaceComd(k);			//����fc���ʵ�������ļ��ıȽ�
			 break;
		 default:cout<<"\n�����:"<<comd[0]<<endl;
	}
}

//////////////////////////////////////////////////////////////////////////////////////

void HelpComd()				//help���������Ϣ(��ʾ�������ʽ)
 {
    cout<<"\n* * * * * * * * * ��ϵͳ��Ҫ���ļ���������������� * * * * * * * * * *\n\n";
    cout<<"create <�ļ���>[ <�ļ�����>]�����������������������ļ�,�ļ�������r��h��s��\n";
    cout<<"open <�ļ���>                           �������ļ����������Ϳ�Ϊr��h��(��)s��\n";
	cout<<"write <�ļ���> [<λ��/app>[ insert]]    ������ָ��λ��д�ļ�(�в��빦��)��\n";
    cout<<"read <�ļ���> [<λ��m> [<�ֽ���n>]]     �������ļ����ӵ�m�ֽڴ���n���ֽڡ�\n";
    cout<<"close <�ļ���>�������������������������������ر��ļ���\n";
    cout<<"del <�ļ���>                            ��������(ɾ��)�ļ���\n";
    cout<<"dir [<·����>] [|<����>]                ������ʾ��ǰĿ¼��\n";
	cout<<"cd [<·����>]                           �����ı䵱ǰĿ¼��\n";
	cout<<"md <·����> [<����>]                    ��������ָ��Ŀ¼��\n";
	cout<<"rd [<·����>]                           ����ɾ��ָ��Ŀ¼��\n";
	cout<<"ren <���ļ���> <���ļ���>               �����ļ�������\n";
	cout<<"attrib <�ļ���> [��<����>]              �����޸��ļ�����(r��h��s)��\n";
	cout<<"copy <Դ�ļ���> [<Ŀ���ļ���>]          ���������ļ���\n";
	cout<<"type <�ļ���>                           ������ʾ�ļ����ݡ�\n";
	cout<<"rewind <�ļ���>                         ����������дָ���Ƶ��ļ���һ���ַ�����\n";
	cout<<"fseek <�ļ���> <λ��>                   ����������дָ�붼�Ƶ�ָ��λ�á�\n";
	cout<<"block <�ļ���>                          ������ʾ�ļ�ռ�õ��̿�š�\n";
	cout<<"closeall                                �����رյ�ǰ�򿪵������ļ���\n";
	cout<<"uof                                     ������ʾUOF(�û����ļ���)��\n";
	cout<<"undel [<·����>]                        �����ָ�ָ��Ŀ¼�б�ɾ�����ļ���\n";
	cout<<"exit                                    �����˳�������\n";
	cout<<"prompt                                  ������ʾ���Ƿ���ʾ��ǰĿ¼(�л�)��\n";
	cout<<"fat                                     ������ʾFAT���п����̿���(0�ĸ���)��\n";
	cout<<"check                                   �����˶Ժ���ʾFAT���п����̿�����\n";
	cout<<"move <�ļ���> <Ŀ¼��>                  �����ļ���Ҽ���Ŀ¼����\n";
	cout<<"batch batchfile [s]                     ��������������\n";
	cout << "fc <�ļ���1> <�ļ���2>                  �����Ƚ�ָ���������ļ��Ƿ���ͬ��\n";
	cout << "replace <�ļ���> <Ŀ¼��>               �����ԡ��ļ�����ָ�����ļ���ȡ����Ŀ¼����ָ��Ŀ¼�е�ͬ���ļ���\n";
 }

/////////////////////////////////////////////////////////////////

int GetAttrib(char* str,char& attrib)
{
	int i,len;
	char ar='\01',ah='\02',as='\04';
	if (str[0]!='|')
	{
		cout<<"\n���������Բ�������\n";
		return -1;
	}
	len=strlen(str);
	_strlwr(str);		//ת����Сд��ĸ
	for (i=1;i<len;i++)
	{
		switch(str[i])
		{
			case 'r': attrib=attrib|ar;
					break;
			case 'h': attrib=attrib|ah;
					break;
			case 's': attrib=attrib|as;
					break;
			default: cout<<"\n���������Բ�������\n";
					return -1;
		}
	}
	return 1;
}

/////////////////////////////////////////////////////////////////

// 2019-12-13 chen, ���Ķ����ʾ������ʾ������
int DirComd(int k)	//dir�����ʾָ��Ŀ¼�����ݣ��ļ�����Ŀ¼���ȣ�
{
	// ������ʽ��dir[ <Ŀ¼��>[ <����>]]
	// ����ܣ���ʾ"Ŀ¼��"ָ����Ŀ¼���ļ����͵�һ����Ŀ¼������ָ
	// ��Ŀ¼�����ڣ������������Ϣ�����������û��ָ��Ŀ¼��������ʾ
	// ��ǰĿ¼�µ���Ӧ���ݡ�����������"����"����������ʾָ��Ŀ¼��"��
	// ����"���Ե�ȫ���ļ����͵�һ����Ŀ¼��������������"����"��������
	// ����ʾָ�����Ե��ļ�����Ŀ¼����h��r��s�����߶��У�����ʾ������
	// �Ի�ֻ�����Ի������������ֻ�����Ե��ļ������Բ�������ʽ��"|<��
	// �Է���>"���������Է�����r��h��s���֣������ִ�Сд�����ֱ��ʾ"ֻ
	// ��"��"����"��"ϵͳ"��������,���ǿ������ʹ���Ҵ����ޡ�����"|rh"
	// ��"|hr"����ʾҪ����ʾͬʱ����"ֻ��"��"����"���Ե��ļ���Ŀ¼������
	// ʾ�ļ���ʱ����ʾ���ļ����ȣ���ʾĿ¼��ʱ��ͬʱ��ʾ"<DIR>"��������

	// ������
 	//		dir /usr |h
	// ����������ʾ��Ŀ¼��usr��Ŀ¼��ȫ��"����"���Ե��ļ�������Ŀ¼��
 	//		dir ..
	// ����������ʾ��ǰĿ¼�ĸ�Ŀ¼��ȫ��"������"���Ե��ļ�����Ŀ¼��(��
	// ��"ֻ��"���Ե�Ҳ��ʾ����һ�㲻��ʾ"ϵͳ"���Եģ���Ϊ"ϵͳ"���ԵĶ�
	// ��һ��Ҳ��"����"���Ե�)��
	//
	// ѧ���ɿ��ǽ��˺����޸ĳ������е�·��������������ļ����������
	// ���⻹���Կ��Ǻ�ͨ��������⡣

	short i,s;
	short k_tmp,flag=0;
	short filecount,dircount,fsizecount;	//�ļ�����Ŀ¼�����ļ������ۼ�
	char ch,attrib='\0',attr,cc;
	FCB *fcbp,*p;
	char *Buffer;
	string tmp_buffer="";

	filecount=dircount=fsizecount=0;
	if (k>1 && strcmp(comd[k - 1], ">") == 0) {
		flag = 1;
		k_tmp = k;
		k -= 2;
	}
	if (k > 1 && strcmp(comd[k - 1], ">>") == 0) {
		flag = 2;
		k_tmp = k;
		k -= 2;
	}

	if (k<1)	//�����޲�������ʾ��ǰĿ¼
	{
		strcpy(temppath,curpath.cpath);
		s=curpath.fblock;	//��ǰĿ¼���׿�ű�����s
	}
	else if (k==1)		//������1������(k=1)
	{
		if (comd[1][0]=='|')
		{
			i=GetAttrib(comd[1],attrib);
			if (i<0) return i;
			strcpy(temppath,curpath.cpath);
			s=curpath.fblock;	//��ǰĿ¼���׿�ű�����s
		}
		else
		{
			s=FindPath(comd[1],'\020',1,fcbp);	//��ָ��Ŀ¼(���׿��)
			if (s<1)
			{
				cout<<"\n�����·������"<<endl;
				return -1;
			}
		}
	}
	else		//������2������(k=2)
	{
		s=FindPath(comd[1],'\020',1,fcbp);	//��ָ��Ŀ¼(���׿��)
		if (s<1)
		{
			cout<<"\n�����·������"<<endl;
			return -1;
		}
		i=GetAttrib(comd[2],attrib);
		if (i<0) return i;
	}

	if (flag == 0)
		cout << "\nThe Directory of C:" << temppath << endl << endl;
	else {
		tmp_buffer += "\nThe Directory of C:";
		tmp_buffer += temppath;
		tmp_buffer += "\n\n";
	}
	while (s>0)
	{
		p=(FCB*) Disk[s];	//pָ���Ŀ¼�ĵ�һ���̿�
		for (i=0;i<4;i++,p++)
		{
			ch=p->FileName[0];	//ȡ�ļ�(Ŀ¼)���ĵ�һ���ַ�
			if (ch==(char)0xe5)		//��Ŀ¼��
				continue;
			if (ch=='\0')		//����Ŀ¼β��
				break;
			attr=p->Fattrib&'\07';	//�������ļ�����Ŀ¼,ֻ��������
			if (attrib==0)			//������û��ָ������
			{
				if (attr&'\02')		//����ʾ�����ء������ļ�
					continue;
			}
			else
			{
				cc=attr & attrib;
				if (attrib!=cc)		//ֻ��ʾָ�����Ե��ļ�
					continue;
			}

			if (flag == 0)
				cout << left << setw(20) << p->FileName;
			else {
				tmp_buffer += p->FileName;
				tmp_buffer += "\t";
			}

			if (p->Fattrib>='\20')	//����Ŀ¼
			{
				if (flag == 0)
					cout << "<DIR>\n";
				else {
					tmp_buffer += "<DIR>\n";
				}
				dircount++;
			}
			else
			{
				cout<<resetiosflags(ios::left);
				if (flag == 0)
					cout << right << setw(10) << p->Fsize << endl;
				else {
					tmp_buffer += to_string(p->Fsize);
					tmp_buffer += "\n";
				}
				filecount++;
				fsizecount+=p->Fsize;
			}
		}
		if (ch=='\0') break;
		s=FAT[s];		//ָ���Ŀ¼����һ���̿�
	}
	if (flag == 0) {
		cout << left << endl;
		cout << right << setw(6) << filecount << " file(s)";
		cout << setw(8) << fsizecount << " bytes" << endl;
		cout << setw(6) << dircount << " dir(s) " << setw(8) << SIZE * FAT[0];
		cout << " free" << endl;
	}
	else {
		tmp_buffer += "\n\t" + to_string(filecount);
		tmp_buffer += " file(s)\t";
		tmp_buffer += to_string(fsizecount) + " bytes\n\t";
		tmp_buffer += to_string(dircount) + " dir(s)\t" + to_string(SIZE * FAT[0]) + " free\n";
	}

	if (flag != 0) {
		Buffer = (char*)tmp_buffer.c_str();

		//----------
		FCB *p2;
		short pos, len0, len1, ii, len, bn0, bn1;
		FindPath(comd[k_tmp], 0, 0, p2);	//����ȫ·����ȥ����..������temppath��
		short ii_uof = Check_UOF(temppath);			//��UOF
		if (ii_uof == S)
		{
			cout << "\n�ļ�" << temppath << "δ�򿪻򲻴��ڣ�����д�ļ���\n";
			return -2;
		}
		if (uof[ii_uof].attr&'\01' && uof[ii_uof].state != 1)
		{	//ֻ���ļ����Ǵ���״̬����д
			cout << "\n" << temppath << "��ֻ���ļ�������д��\n";
			return -3;
		}

		p2 = uof[ii_uof].fp;
		if (flag==1) {
			ii = buffer_to_file(p2, Buffer);
			if (ii == 0)	//д�ļ�ʧ��
				return ii;
			uof[ii_uof].fsize = uof[ii_uof].fp->Fsize;
			uof[ii_uof].faddr = uof[ii_uof].fp->Addr;
			uof[ii_uof].readp = 1;
			uof[ii_uof].writep = uof[ii_uof].fsize + 1;
			cout << "\n����ض��� >" << uof[ii_uof].fname << " [OK].\n";

			return 1;
		}
		else {
			pos = uof[ii_uof].fsize + 1;
		}

		if (pos <= 0)
		{
			cout << "\nд��λ�ô���\n";
			return -1;
		}
		pos--;							//ʹpos��0��ʼ

		len1 = strlen(Buffer);
		if (len1 == 0)			//���볤��Ϊ0,���ı��ļ�
			return 0;

		len0 = uof[ii_uof].fsize;				//ȡ�ļ�ԭ���ĳ���ֵ

		//���´����ļ��ǿյ����
		len = len1 + pos;		//����д����ɺ��ļ��ĳ���
		bn0 = len0 / SIZE + (short)(len0%SIZE > 0);	//�ļ�ԭ��ռ�õ��̿���
		bn1 = len / SIZE + (short)(len%SIZE > 0);		//д����ļ���ռ�õ��̿���
		if (FAT[0] < bn1 - bn0)
		{
			cout << "\n���̿ռ䲻��,����д���ļ�.\n";
			return -1;
		}
		char *buf = new char[len + 1];
		if (buf == 0)
		{
			cout << "\n�����ڴ�ʧ�ܡ�\n";
			return -1;
		}
		file_to_buffer(p2, buf);		//�ļ�����buf

		strcpy(&buf[pos], Buffer);
		buffer_to_file(p2, buf);
		delete[] buf;
		uof[ii_uof].fsize = uof[ii_uof].fp->Fsize;
		uof[ii_uof].writep = uof[ii_uof].fsize + 1;


		cout << "\n����ض��� >>" << uof[ii_uof].fname << " [OK].\n";
	}

	return 1;
}

/////////////////////////////////////////////////////////////////

int CdComd(int k)
{
	// ��ǰĿ¼������Ŀ¼��ת�Ƶ�ָ��Ŀ¼�¡�ָ��Ŀ¼������ʱ������������Ϣ��
	// ����������Ŀ¼��������ʾ��ǰĿ¼·����

	short i,s;
	char attrib=(char)16;
	FCB* fcbp;
	if (k>1)	//�����ж���1������������
	{
		cout<<"\n������󣺲���̫�ࡣ\n";
		return -1;
	}
	if (k<1)	//�����޲�������ʾ��ǰĿ¼
	{
		cout<<"\nThe Current Directory is C:"<<curpath.cpath<<endl;
		return 1;
	}
	else		//������һ����������ָ��Ŀ¼��Ϊ��ǰĿ¼
	{
		i=strlen(comd[1]);
		if (i>1 && comd[1][i-1]=='/')	//·����"/"��β������
		{
			cout<<"\n·��������\n";
			return -1;
		}
		s=FindPath(comd[1],attrib,1,fcbp);	//��ָ��Ŀ¼(���׿��)
		if (s<1)
		{
			cout<<"\n·��������"<<endl;
			return -1;
		}
		curpath.fblock=s;
		strcpy(curpath.cpath,temppath);
		if (!dspath)
			cout<<"\n��ǰĿ¼��Ϊ C:"<<curpath.cpath<<endl;
		return 1;
	}
}

/////////////////////////////////////////////////////////////////

int M_NewDir(char *Name,FCB* p,short fs,char attrib)	//��pλ�ô���һ����Ŀ¼
{
	//�ɹ���������Ŀ¼���׿��

	short i,b,kk;
	FCB *q;
	kk=SIZE/sizeof(FCB);
	b=getblock();		//��Ŀ¼�����һ���̿����ڴ洢Ŀ¼�..��
	if (b<0)
		return b;
	strcpy(p->FileName,Name);	//Ŀ¼��
	p->Fattrib=attrib;			//Ŀ¼������ΪĿ¼�����ļ�
	p->Addr=b;					//����Ŀ¼���׿��
	p->Fsize=0;					//��Ŀ¼�ĳ���Լ��Ϊ0
	q=(FCB*) Disk[b];
	for (i=0;i<kk;i++,q++)
		q->FileName[0]='\0';	//�ÿ�Ŀ¼���־*/
	q=(FCB*) Disk[b];
	strcpy(q->FileName,"..");	//��Ŀ¼�еĵ�һ��Ŀ¼�����ǡ�..��
	q->Fattrib=(char)16;		//Ŀ¼������ΪĿ¼�����ļ�
	q->Addr=fs;					//��Ŀ¼���׿���Ǹ�Ŀ¼���׿��
	q->Fsize=0;					//��Ŀ¼�ĳ���Լ��Ϊ0
	return b;					//�ɹ�����������
}

/////////////////////////////////////////////////////////////////

int ProcessPath(char* path,char* &Name,int k,int n,char attrib)
{
	// ��path�����һ�����ַ���������������ò���Name���أ�
	// ����path�г���Name�����һ��Ŀ¼�ĵ�ַ(�׿��)��
	// ��Ҫʱ���ú���FindPath()����ͨ��ȫ�ֱ���temppath��
	// ��path(ȥ��Name��)��ȫ·����(����·����)

	short i,len,s;
	FCB* fcbp;

	if (n && k!=n)	//n=0,��������k����,n>0,����k=n
	{
		cout<<"\n���������������\n";
		return -1;
	}
	len=strlen(path);
	for (i=len-1;i>=0;i--)
		if (path[i]=='/')
			break;
	Name=&path[i+1];		//ȡ·�������һ������
	if (i==-1)
	{
		s=curpath.fblock;
		strcpy(temppath,curpath.cpath);
	}
	else
	{
		if (i==0)
		{
			s=1;
			strcpy(temppath,"/");
		}
		else
		{
			path[i]='\0';
			s=FindPath(path,attrib,1,fcbp);
			if (s<1)
			{
				cout<<"\n·��������\n";
				return -3;
			}
		}
	}
	return s;
}

/////////////////////////////////////////////////////////////////

int MdComd(int k)		//md�������
{
	// ������ʽ��md <Ŀ¼��>
	// ���ܣ���ָ��·���´���ָ��Ŀ¼����û��ָ��·�������ڵ�ǰĿ¼�´���ָ��Ŀ¼��
	// ��������Ŀ¼����������Ϣ��Ŀ¼���ļ�Ҳ����������

	// ѧ�����Կ��������мӡ����ԡ����������ڴ���ָ�����Ե���Ŀ¼��������ʽ���£�
	//		md <Ŀ¼��>[ <����>]
	// ���԰���R��H��S�Լ����ǵ����(�����ִ�Сд��˳��Ҳ����)�����磺
	//		md user rh
	// �书�����ڵ�ǰĿ¼�д������С�ֻ�����͡����ء����Ե���Ŀ¼user��

	short i,s,s0,kk;
	char attrib=(char)16,*DirName;
	FCB *p;

	kk=SIZE/sizeof(FCB);

	if (k<1)
	{
		cout<<"\n����������û��Ŀ¼����\n";
		return -1;
	}
	if (k>2)
	{
		cout<<"\n�����������̫�ࡣ\n";
		return -1;
	}
	s=ProcessPath(comd[1],DirName,k,0,attrib);
	if (s<0)
		return s;		//ʧ�ܣ�����
	if (!IsName(DirName))		//�����ֲ����Ϲ���
	{
		cout<<"\n�����е���Ŀ¼������\n";
		return -1;
	}
	i=FindFCB(DirName,s,attrib,p);
	short j= FindFCB(DirName, s, 0, p);
	if (i>0 || j>=0)
	{
		cout<<"\n����Ŀ¼������Ŀ¼���ļ�������\n";
		return -1;
	}
	if (k==2)		//������ʽ��md <Ŀ¼��> |<���Է�>
	{
		i=GetAttrib(comd[2],attrib);
		if (i<0)
			return i;
	}
	s0=FindBlankFCB(s,p);//�ҿհ�Ŀ¼��
	if (s0<0)			//������
		return s0;
	s0=M_NewDir(DirName,p,s,attrib);	//��p��ָλ�ô���һ����Ŀ¼��
	if (s0<0)		//����ʧ��
	{
		cout<<"\n���̿ռ�����������Ŀ¼ʧ�ܡ�\n";
		return -1;
	}
	return 1;		//��Ŀ¼�����ɹ�������
}

/////////////////////////////////////////////////////////////////

int RdComd(int k)
{
	// ��ָ��Ŀ¼Ϊ�գ���ɾ��֮�����򣬸���"�ǿ�Ŀ¼����ɾ��"����ʾ��
	// ����ɾ����ǰĿ¼��

	short i,j,count=0,fs,s0,s;
	char attrib=(char)16,*DirName;
	FCB *p,*fcbp;
	fs=ProcessPath(comd[1],DirName,k,1,attrib);	//����DirName�ĸ�Ŀ¼���׿��
	if (fs<0)
		return fs;				//ʧ�ܣ�����
	s0=s=FindFCB(DirName,fs,attrib,fcbp);//ȡDirName���׿��
	if (s<1)
	{
		cout<<"\nҪɾ����Ŀ¼�����ڡ�\n";
		return -1;
	}
	if (s==curpath.fblock)
	{
		cout<<"\n����ɾ����ǰĿ¼��\n";
		return 0;
	}
	while (s>0)		//ѭ�����ң�ֱ��Ŀ¼β��
	{
		p=(FCB*) Disk[s];
		for (i=0;i<4;i++,p++)
		{
			if (p->FileName[0]!=(char)0xe5 && p->FileName[0]!='\0')//�ۼƷǿ�Ŀ¼��
				count++;
		}
		//s0=s;			//������һ���̿��
		s=FAT[s];		//ȡ��һ���̿��
	}
	if (count>1)
	{
		cout<<"\nĿ¼"<<DirName<<"�ǿգ�����ɾ����\n";
		return -1;
	}
	//s0=fcbp->Addr;		//ȡDirName���׿��
	while (s0>0)			//�黹Ŀ¼DirName��ռ�Ĵ��̿ռ�
	{
		s=FAT[s0];			//���µ�s0��ĺ������
		FAT[s0]=0;			//���յ�s0��
		FAT[0]++;			//�����̿�����1
		s0=s;				//������Ÿ���s0
	}
	fcbp->FileName[0]=(char)0xe5;	//ɾ��DirName��Ŀ¼��
	if (strcmp(temppath,"/")==0)	//��ɾ������Ŀ¼�ڸ�Ŀ¼
		return 1;
	//��ɾ������Ŀ¼DirName���ڸ�Ŀ¼ʱ�����丸Ŀ¼�����´���
	s0=s=fs;				//ȡDirName��Ŀ¼���׿��
	while (s>0)				//����DirName�ĸ�Ŀ¼�ռ�(������Ŀ¼����̿�)
	{
		p=(FCB*) Disk[s];
		for (j=i=0;i<4;i++,p++)
			if (p->FileName[0]!=(char)0xe5 && p->FileName[0]!='\0')//�ۼƷǿ�Ŀ¼��
				j++;
		if (j==0)
		{
			FAT[s0]=FAT[s];		//����ָ��
			FAT[s]=0;			//����s���̿�
			FAT[0]++;			//�����̿�����1
			s=FAT[s0];
		}
		else
		{
			s0=s;				//������һ���̿��
			s=FAT[s];			//sָ����һ���̿�
		}
	}
	return 1;
}

/////////////////////////////////////////////////////////////////

int TypeComd(int k)		//type�������(��ʾ�ļ�����)
{
	// ��ʾ�ļ����ݣ�type [<�ļ���>]����ʾָ���ļ������ݡ�
	// ��ָ���ļ������ڣ������������Ϣ��
	//��ʾָ���ļ������ݡ���������δָ���ļ�������ʾ����ǰ�����ļ��������ݡ�
	//��ν����ǰ�����ļ�������ָ��һ��ʹ��create��open��write��read��rewind��fseek��������������ļ���

	short i, s, size, jj = 0, i_uof;
	char attrib = '\0', *FileName;
	char* Buffer;
	char gFileName[PATH_LEN];	//����ļ�ȫ·����
	FCB* fcbp;

	if (k < 1)
	{
		//cout << "\n���������ļ�����\n";
		//return -1;
		for (i_uof = S - 1; i_uof >= 0; i_uof--) {
			if (uof[i_uof].state != 0) {
				break;
			}
		}
		if (i_uof < 0) {
			cout << "\n�޵�ǰ�����ļ�����ʾ��" << endl;
			return -1;
		}
		else {
			s = uof[i_uof].faddr;
			fcbp = uof[i_uof].fp;
			size = fcbp->Fsize;
			Buffer = new char[size + 1];		//���䶯̬�ڴ�ռ�
			while (s > 0)
			{
				for (i = 0; i < SIZE; i++, jj++)
				{
					if (jj == size)
						break;
					Buffer[jj] = Disk[s][i];
				}
				if (i < SIZE)
					break;
				s = FAT[s];
			}
			Buffer[jj] = '\0';
			cout << Buffer << endl;
			delete[] Buffer;		//�ͷŷ���Ķ�̬�ڴ�ռ�
		}

	}
	else {
		s = ProcessPath(comd[1], FileName, k, 0, '\020');//ȡFileName����Ŀ¼���׿��
		if (s < 1)			//·������
			return s;		//ʧ�ܣ�����
		s = FindFCB(FileName, s, attrib, fcbp);		//ȡFileName���׿��(���������)
		strcpy(gFileName, temppath);
		i = strlen(temppath);
		if (temppath[i - 1] != '/')
			strcat(gFileName, "/");
		strcat(gFileName, FileName);	//�����ļ���ȫ·����
		if (s < 0)
		{
			cout << "\n�ļ�" << gFileName << "�����ڡ�\n";
			return -3;
		}
		if (s == 0) {
			cout << "\n�ļ�" << gFileName << "�ǿ��ļ�\n";
			return -1;
		}
		else
		{
			size = fcbp->Fsize;
			Buffer = new char[size + 1];		//���䶯̬�ڴ�ռ�
			while (s > 0)
			{
				for (i = 0; i < SIZE; i++, jj++)
				{
					if (jj == size)
						break;
					Buffer[jj] = Disk[s][i];
				}
				if (i < SIZE)
					break;
				s = FAT[s];
			}
			Buffer[jj] = '\0';
			//cout << Buffer << endl;

		}

		if (k == 1) {
			cout << Buffer << endl;
		}
		else if (k == 3) {

			FCB *p2;
			short pos,len0,len1,ii,len,bn0,bn1;
			//------------------------------------
			FindPath(comd[3], 0, 0, p2);	//����ȫ·����ȥ����..������temppath��
			short ii_uof = Check_UOF(temppath);			//��UOF
			if (ii_uof == S)
			{
				cout << "\n�ļ�" << temppath << "δ�򿪻򲻴��ڣ�����д�ļ���\n";
				return -2;
			}
			if (uof[ii_uof].attr&'\01' && uof[ii_uof].state != 1)
			{	//ֻ���ļ����Ǵ���״̬����д
				cout << "\n" << temppath << "��ֻ���ļ�������д��\n";
				return -3;
			}
			//-------------------------------------------------------

			p2 = uof[ii_uof].fp;

			//-------------------------------------
			if (strcmp(comd[2], ">") == 0) {
				ii = buffer_to_file(p2, Buffer);
				if (ii == 0)	//д�ļ�ʧ��
					return ii;
				uof[ii_uof].fsize = uof[ii_uof].fp->Fsize;
				uof[ii_uof].faddr = uof[ii_uof].fp->Addr;
				uof[ii_uof].readp = 1;
				uof[ii_uof].writep = uof[ii_uof].fsize + 1;
				cout << "\n����ض��� >" << uof[ii_uof].fname << " [OK].\n";
				//delete[] Buffer;		//�ͷŷ���Ķ�̬�ڴ�ռ�
				return 1;
			}
			else if (strcmp(comd[2], ">>") == 0) {
				pos = uof[ii_uof].fsize + 1;
			}
			else {
				cout << "\n�ض�����Ŵ���" << endl;
				return -1;
			}
			//---------------------------------------
			if (pos <= 0)
			{
				cout << "\nд��λ�ô���\n";
				return -1;
			}
			pos--;							//ʹpos��0��ʼ

			len1 = strlen(Buffer);
			if (len1 == 0)			//���볤��Ϊ0,���ı��ļ�
				return 0;

			len0 = uof[ii_uof].fsize;				//ȡ�ļ�ԭ���ĳ���ֵ


			//���´����ļ��ǿյ����
			len = len1 + pos ;		//����д����ɺ��ļ��ĳ���
			bn0 = len0 / SIZE + (short)(len0%SIZE > 0);	//�ļ�ԭ��ռ�õ��̿���
			bn1 = len / SIZE + (short)(len%SIZE > 0);		//д����ļ���ռ�õ��̿���
			if (FAT[0] < bn1 - bn0)
			{
				cout << "\n���̿ռ䲻��,����д���ļ�.\n";
				return -1;
			}
			char *buf = new char[len + 1];
			if (buf == 0)
			{
				cout << "\n�����ڴ�ʧ�ܡ�\n";
				return -1;
			}
			file_to_buffer(p2, buf);		//�ļ�����buf

			strcpy(&buf[pos], Buffer);
			buffer_to_file(p2, buf);
			delete[] buf;
			uof[ii_uof].fsize = uof[ii_uof].fp->Fsize;
			uof[ii_uof].writep = uof[ii_uof].fsize + 1;


			cout << "\n����ض��� >>" << uof[ii_uof].fname << " [OK].\n";
			//delete[] Buffer;		//�ͷŷ���Ķ�̬�ڴ�ռ�
		}
		else {
			cout << "\n����������߹��٣�" << endl;
			return -1;
		}

		return 1;
	}

	return -1;
}

/////////////////////////////////////////////////////////////////

int blockf(int k)	//block�������(��ʾ�ļ���Ŀ¼ռ�õ��̿��)
{
	// ������ʽ��block [<�ļ���>]
	// block <�ļ���> ��ʾ�ļ���Ŀ¼ռ�õ��̿��
	// block ��ʾ��ǰ�����ļ�ռ�õ��̿��

	short s, i_uof;
	char attrib = '\040';		//32��ʾ����(�ļ�����Ŀ¼)Ŀ¼�����
	FCB* fcbp;

	if (k < 1) {
		//��UOF����Ѱ�����򿪵��ļ�
		for (i_uof = S - 1; i_uof >= 0; i_uof--) {
			if (uof[i_uof].state != 0) {
				break;
			}
		}
		if (i_uof < 0) {
			cout << "�޵�ǰ�����ļ��ɲ�����" << endl;
			return -2;
		}
		else {
			if (uof[i_uof].fsize == 0) {
				cout << "�ļ��ǿ��ļ����޿�ſ���ʾ��" << endl;
				return -2;
			}
			else {
				cout << "\n" << uof[i_uof].fname << "ռ�õ��̿��Ϊ��";
				cout << uof[i_uof].faddr << "  ";
				cout << endl;
				return 1;
			}
		}
	}
	else if (k == 1) {
		s = FindPath(comd[1], attrib, 1, fcbp);	//��ָ��Ŀ¼(���׿��)
		if (s < 0)
		{
			cout << "\n·��������" << endl;
			return -2;
		}
		if (s == 0)
		{
			cout << '\n' << temppath << "�ǿ��ļ�\n";
			return 0;
		}
		cout << "\n" << temppath << "ռ�õ��̿��Ϊ��";
		/*
		��ʦ���Ĳο������Ǵ�������ģ��������Ҫ����ʾ�ļ���Ŀ¼ռ�õ��̿�ż��ɣ�����ѭ����������
		while (s > 0)
		{
			cout << s << "  ";
			s = FAT[s];
		}*/
		cout << s << "  ";
		cout << endl;
		return 1;
	}
	else {
		cout << "\n�����в���̫�ࡣ\n";
		return -1;
	}



}

/////////////////////////////////////////////////////////////////

void Put_UOF(char *gFileName,int i,short status,FCB* fcbp)
{
	strcpy(uof[i].fname,gFileName);	//�����ļ�ȫ·����
	uof[i].attr=fcbp->Fattrib;		//�����ļ�����
	uof[i].faddr=fcbp->Addr;		//�ļ����׿��(0������ļ�)
	uof[i].fsize=fcbp->Fsize;
	uof[i].fp=fcbp;
	uof[i].state=status;					//��״̬
	if (fcbp->Fsize>0)				//���ļ��ǿ�
		uof[i].readp=1;				//��ָ��ָ���ļ���ͷ
	else
		uof[i].readp=0;				//��ָ��ָ���λ��
	uof[i].writep=fcbp->Fsize+1;	//дָ��ָ���ļ�ĩβ
}

/////////////////////////////////////////////////////////////////

int FindBlankFCB(short s,FCB* &fcbp1)	//Ѱ���׿��Ϊs��Ŀ¼�еĿ�Ŀ¼��
{
	short i,s0;
	while (s>0)			//���׿��Ϊs��Ŀ¼�ҿյǼ�����ֱ��Ŀ¼β��
	{
		fcbp1=(FCB*) Disk[s];
		for (i=0;i<4;i++,fcbp1++)
			if (fcbp1->FileName[0]==(char)0xe5 || fcbp1->FileName[0]=='\0')
			{
				fcbp1->Addr=fcbp1->Fsize=0;		//����Ϊ��Ŀ¼��
				return 1;						//�ҵ���Ŀ¼��ɹ�����
			}
		s0=s;		//������һ���̿��
		s=FAT[s];	//ȡ��һ���̿��
	}
	if (strcmp(temppath,"/")==0)	//���Ǹ�Ŀ¼
	{
		cout<<"\n��Ŀ¼�����������ٴ���Ŀ¼�\n";
		return -1;
	}
	s=getblock();	//ȡһ�����̿�
	if (s<0)		//�޿����̿�
	{
		cout<<"\n���̿ռ�����������Ŀ¼ʧ�ܡ�\n";
		return -1;
	}
	FAT[s0]=s;		//����FAT��
	fcbp1=(FCB*) Disk[s];
	for (i=0;i<4;i++,fcbp1++)
		fcbp1->FileName[0]='\0';	//�ÿ�Ŀ¼��־
	fcbp1=(FCB*) Disk[s];
	fcbp1->Addr=fcbp1->Fsize=0;		//����Ϊ��Ŀ¼��
	return 1;
}

/////////////////////////////////////////////////////////////////

int CreateComd(int k)		//create����������������ļ�
{
	// �����ļ���create <�ļ���> [<�ļ�����>]������һ��ָ�����ֵ����ļ���
	// ����Ŀ¼������һĿ¼��������ļ������ݡ����������ļ�����������Ϣ��

	short i,i_uof,s0,s;
	char attrib='\0',*FileName;
	char gFileName[PATH_LEN];	//����ļ�ȫ·����
	char ch,*p;
	FCB* fcbp1;
	if(k>2 || k<1)
	{
		cout<<"\n�����в����������ԡ�\n";
		return -1;
	}
	s=ProcessPath(comd[1],FileName,k,0,'\020');//ȡFileName����Ŀ¼���׿��
	if (s<1)			//·������
		return s;		//ʧ�ܣ�����
	if (!IsName(FileName))		//�����ֲ����Ϲ���
	{
		cout<<"\n�����е����ļ�������\n";
		return -2;
	}
	s0=FindFCB(FileName,s,attrib,fcbp1);	//ȡFileName���׿��(���������)
	if (s0>0)
	{
		cout<<"\n��ͬ���ļ������ܽ�����\n";
		return -2;
	}
	strcpy(gFileName,temppath);
	i=strlen(temppath);
	if (temppath[i-1]!='/')
		strcat(gFileName,"/");
	strcat(gFileName,FileName);	//�����ļ���ȫ·����
	if (k==2)
	{
		p=comd[2];
		while (*p!='\0')	//�����ļ�����
		{
			ch=*p;
			ch=tolower(ch);
			switch(ch)
			{
				case 'r' : attrib=attrib | (char) 1;
					break;
				case 'h' : attrib=attrib | (char) 2;
					break;
				case 's' : attrib=attrib | (char) 4;
					break;
				default : cout<<"\n������ļ����Դ���\n";
					return -3;
			}
			p++;
		}
	}
	for (i_uof=0;i_uof<S;i_uof++)			//��UOF���ҿձ���
		if (uof[i_uof].state==0)
			break;
	if (i_uof==S)
	{
		cout<<"\nUOF���������ܴ����ļ���\n";
		return -4;
	}
	i=FindBlankFCB(s,fcbp1);			//Ѱ���׿��Ϊs��Ŀ¼�еĿ�Ŀ¼��
	if (i<0)
	{
		cout<<"\n�����ļ�ʧ�ܡ�\n";
		return i;
	}
	strcpy(fcbp1->FileName,FileName);	//Ŀ¼���б����ļ���
	fcbp1->Fattrib=attrib;				//�����ļ�����
	fcbp1->Addr=0;						//���ļ��׿����Ϊ0
	fcbp1->Fsize=0;						//���ļ�����Ϊ0
	Put_UOF(gFileName,i_uof,1,fcbp1);	//����UOF�Ǽ���
	cout<<"\n�ļ�"<<gFileName<<"�����ɹ�\n";
	return 1;							//�ļ������ɹ�������
}

/////////////////////////////////////////////////////////////////

int Check_UOF(char *Name)		//���UOF������������ָ�����ļ�
{
	int i;
	for (i=0;i<S;i++)			//���û����ļ���UOF
	{
		if (uof[i].state==0)	//�ձ���
			continue;
		if (strcmp(Name,uof[i].fname)==0)	//�ҵ�
			break;
	}
	return i;
}

/////////////////////////////////////////////////////////////////

int OpenComd(int k)			//open������������ļ�
{
	// ������ʽ��open <�ļ���>
	// ��ָ���ļ���������δ�򿪣����֮�������û����ļ���UOF���е�
	// �Ǹ��ļ����й���Ϣ����ָ���ļ��Ѿ��򿪣�����ʾ"�ļ��Ѵ�"����Ϣ��
	// ��ָ���ļ������ڣ������������Ϣ��ֻ���ļ��򿪺�ֻ�ܶ�����д��

	short i,s0,s;
	char attrib='\0',*FileName;
	char gFileName[PATH_LEN];	//����ļ�ȫ·����
	FCB* fcbp;

	s0=ProcessPath(comd[1],FileName,k,1,'\20');//ȡFileName����Ŀ¼���׿��
	if (s0<1)			//·������
		return s0;		//ʧ�ܣ�����
	s=FindFCB(FileName,s0,attrib,fcbp);		//ȡFileName���׿��(���������)
	if (s<0)
	{
		cout<<"\nҪ�򿪵��ļ������ڡ�\n";
		return -2;
	}
	strcpy(gFileName,temppath);
	i=strlen(temppath);
	if (temppath[i-1]!='/')
		strcat(gFileName,"/");
	strcat(gFileName,FileName);	//�����ļ���ȫ·����
	i=Check_UOF(gFileName);		//��UOF
	if (i<S)					//���ļ�����UOF��
	{
		cout<<"\n�ļ�"<<gFileName<<"ԭ���Ѿ���!\n";
		return -3;
	}
	for (i=0;i<S;i++)			//��UOF���ҿձ���
		if (uof[i].state==0)
			break;
	if (i==S)
	{
		cout<<"\nUOF���������ܴ��ļ���\n";
		return -4;
	}
	Put_UOF(gFileName,i,2,fcbp);
	cout<<"\n�ļ�"<<gFileName<<"�򿪳ɹ���\n";
	return 1;
}

/////////////////////////////////////////////////////////////////

int getblock()	//���һ�������̿飬��fappend()��������
{
	short b;
	if (FAT[0]==0)	//FAT[0]���Ǵ��̿��п���
		return -1;	//��������(���޿����̿�)
	for(b=ffbp;b<K;b++)
		if (!FAT[b])
			break;
	if (b==K)
	{
		for (b=1;b<ffbp;b++)
			if (!FAT[b]) break;
	}
	ffbp=b+1;
	if (ffbp==K) ffbp=1;
	FAT[0]--;	//�̿�����1
	FAT[b]=-1;	//���̿��ѷ����־(�˴�����������Ϊ�ļ�β)
	return b;	//����ȡ�õĿ����̿��
}

////////////////////////////////////////////////////////////////

int WriteComd(int k)		//write����Ĵ�����
{
	//дָ��������ؼ���Ϣ���ǣ�a���ҵ���ǰ�ļ�		b��pos ��writec ��ins


	// д�ļ���write [<�ļ���>] [<λ��>][insert/del]������������"λ��"����������дָ
	// ����ָλ��д���ļ����ݣ����ṩ"λ��"���������ڶ�Ӧλ��д�����ݡ�λ�ÿ���
	// ������n����ָ���ļ��ĵ�n���ֽڴ���ʼд��(λ�ô�1��ʼ���)��"λ��" ������
	// �� "append"��ǰ3���ַ���Ч�������ִ�Сд������ʾ���ļ�β��д����Ϣ������
	// ���� "insert"��ǰ3���ַ���Ч�������ִ�Сд��������д������ݲ��뵽��Ӧλ
	// �ã���Ӧλ�ÿ�ʼ��ԭ���ݺ��ơ����޲��� "insert" ��д������ݴ����ļ�ԭ��
	// ������(��Ӧλ�õ�����)��д����ϵ����ļ����Ⱥ�дָ��ֵ��
	// ���ļ�δ�򿪻��ļ������ڣ��ֱ����������Ϣ��



	// ��������������������ʽ��
	//��ʽA:
	//	write <�ļ���> ������дָ�뵱ǰ��ָλ��д��д�����ݴ���ԭ����(���淽ʽ���д��ʽ)
	//	write <�ļ���> |pn�������ļ���ͷ��n���ֽڴ�д����д��ʽ
	//	write <�ļ���> |ins������дָ����ָλ��д��д�봦��ʼ��ԭ���ݺ���(���뷽ʽ)
	//	write <�ļ���> |pn |ins�������ļ���ͷ��n���ֽڴ�д�����뷽ʽ
	//	write <�ļ���> |app�������ļ�β��д(��ӷ�ʽ)
	//	write <�ļ���> |del������ָ���ļ�����дָ��λ��ɾ�����ļ�ĩβ
	//  write <�ļ���> |lm |del������ָ���ļ�����дָ��λ�ÿ�ʼ��ɾ��m���ֽ�
	//  write <�ļ���> |pn |del������ָ���ļ�����ָ��λ��n����ʼɾ�����ļ�ĩβ
	//  write <�ļ���> |pn |lm |del������ָ���ļ�����ָ��λ��n����ʼɾ��m���ֽ�

	//��ʽB��
	//	write ������дָ�뵱ǰ��ָλ��д��д�����ݴ���ԭ����(���淽ʽ���д��ʽ)
	//	write |pn�������ļ���ͷ��n���ֽڴ�д����д��ʽ
	//	write |ins������дָ����ָλ��д��д�봦��ʼ��ԭ���ݺ���(���뷽ʽ)
	//	write |pn |ins�������ļ���ͷ��n���ֽڴ�д�����뷽ʽ
	//	write |app�������ļ�β��д(��ӷ�ʽ)
	//	write |del�����Ե�ǰ�����ļ�����дָ��λ��ɾ�����ļ�ĩβ
	//  write |lm |del�����Ե�ǰ�����ļ�����дָ��λ�ÿ�ʼ��ɾ��m���ֽ�
	//  write |pn |del�����Ե�ǰ�����ļ�����ָ��λ��n����ʼɾ�����ļ�ĩβ
	//  write |pn |lm |del�����Ե�ǰ�����ļ�����ָ��λ��n����ʼɾ��m���ֽ�

	//��˼�������ʹ������insert������append��ֻҪǰ3���ַ��ԾͿ��ԣ�������3���ַ�Ҳ�С����磺
	// ���ڡ�insert��������ins��inse��inser��insert(�����ִ�Сд)�����ԣ������������С�

#define BSIZE 40*SIZE+1
	short int ii, ii_uof, len0, len, len1, pos, ins = 0, del = 0, mylen = 0, index, wpflag = 0;//wpflag=1��ʾ��β�����
	short int bn0, bn1, jj, count = 0;
	char attrib = '\0', Buffer[BSIZE];		//Ϊ����ƣ�����һ�����д��2560�ֽ�
	char* buf;
	char* nstr1, *nstr2, *nstr3, *delbuf;
	FCB* fcbp;

	if (k < 1) {
		//��ʽB:write ������дָ�뵱ǰ��ָλ��д��д�����ݴ���ԭ����(���淽ʽ���д��ʽ)
		//a�������ҵ��ļ�
		for (ii_uof = S - 1; ii_uof >= 0; ii_uof--) {
			if (uof[ii_uof].state != 0) {
				break;
			}
		}
		if (ii_uof < 0) {
			cout << "�޴򿪵��ļ��ɲ�����" << endl;
			return -3;
		}
		else if (uof[ii_uof].attr & '\01' && uof[ii_uof].state != 1) {
			//ֻ���ļ����Ǵ���״̬����д
			cout << "\n" << temppath << "��ֻ���ļ�������д��\n";
			return -3;
		}

		//bȷ����pos ins del wpflag mylen
		pos = uof[ii_uof].writep;	//��дָ����ָλ�ÿ�ʼд(write <�ļ���>)
		wpflag = 1;
	}
	else if (k == 1) {
		//��ʽB:
		//	write |pn�������ļ���ͷ��n���ֽڴ�д����д��ʽ
		//	write |ins������дָ����ָλ��д��д�봦��ʼ��ԭ���ݺ���(���뷽ʽ)
		//	write |app�������ļ�β��д(��ӷ�ʽ)
		//	write |del�����Ե�ǰ�����ļ�����дָ��λ��ɾ�����ļ�ĩβ

		//��ʽA:
		//	write <�ļ���> ������дָ�뵱ǰ��ָλ��д��д�����ݴ���ԭ����(���淽ʽ���д��ʽ)

		if (comd[1][0] == '|') {
			//a�������ҵ��ļ�
			for (ii_uof = S - 1; ii_uof >= 0; ii_uof--) {
				if (uof[ii_uof].state != 0) {
					break;
				}
			}
			if (ii_uof < 0) {
				cout << "�޴򿪵��ļ��ɲ�����" << endl;
				return -3;
			}
			else if (uof[ii_uof].attr & '\01' && uof[ii_uof].state != 1) {
				//ֻ���ļ����Ǵ���״̬����д
				cout << "\n" << temppath << "��ֻ���ļ�������д��\n";
				return -3;
			}
			//bȷ����pos ins del wpflag mylen
			if (comd[1][1] == 'p' || comd[1][1] == 'P') {//	write |pn�������ļ���ͷ��n���ֽڴ�д����д��ʽ
				nstr1 = &comd[1][2];
				pos = atoi(nstr1);
				if (pos <= 0)
				{
					cout << "\n�������ṩ��д��λ�ô���\n";
					return -4;
				}
			}
			else if (strnicmp(comd[1], "|ins", 4) == 0) {//	write |ins������дָ����ָλ��д��д�봦��ʼ��ԭ���ݺ���(���뷽ʽ)
				pos = uof[ii_uof].writep;	//�ӵ�ǰдָ��λ�ÿ�ʼд
				wpflag = 1;
				ins = 1;					//����ģʽ
			}
			else if (strnicmp(comd[1], "|app", 4) == 0) {//	write |app�������ļ�β��д(��ӷ�ʽ)
				pos = uof[ii_uof].fsize + 1;	//�ļ�β�����ģʽ
				wpflag = 1;
			}
			else if (strnicmp(comd[1], "|del", 4) == 0) {//	write |del�����Ե�ǰ�����ļ�����дָ��λ��ɾ�����ļ�ĩβ
				pos = uof[ii_uof].writep;	//�ӵ�ǰдָ��λ�ÿ�ʼ
				del = 1;					//ɾ��ģʽ
			}
			else {
				cout << "�����ʽ����" << endl;
				return -3;
			}
		}
		else {
			//write <�ļ���> ������дָ�뵱ǰ��ָλ��д��д�����ݴ���ԭ����(���淽ʽ���д��ʽ)
			//a���ҵ���ǰ�ļ�
			FindPath(comd[1], attrib, 0, fcbp);	//����ȫ·����ȥ����..������temppath��
			ii_uof = Check_UOF(temppath);			//��UOF
			if (ii_uof == S)
			{
				cout << "\n�ļ�" << temppath << "δ�򿪻򲻴��ڣ�����д�ļ���\n";
				return -2;
			}
			if (uof[ii_uof].attr & '\01' && uof[ii_uof].state != 1)
			{	//ֻ���ļ����Ǵ���״̬����д
				cout << "\n" << temppath << "��ֻ���ļ�������д��\n";
				return -3;
			}
			//bȷ����pos ins del wpflag mylen
			pos = uof[ii_uof].writep;	//��дָ����ָλ�ÿ�ʼд(write <�ļ���>)
			wpflag = 1;
		}


	}
	else if (k == 2) {
		//��ʽB:
		//	write |pn |ins�������ļ���ͷ��n���ֽڴ�д�����뷽ʽ
		//  write |lm |del�����Ե�ǰ�����ļ�����дָ��λ�ÿ�ʼ��ɾ��m���ֽ�
		//  write |pn |del�����Ե�ǰ�����ļ�����ָ��λ��n����ʼɾ�����ļ�ĩβ

		if (comd[1][0] == '|') {
			//a�������ҵ��ļ�
			for (ii_uof = S - 1; ii_uof >= 0; ii_uof--) {
				if (uof[ii_uof].state != 0) {
					break;
				}
			}
			if (ii_uof < 0) {
				cout << "�޴򿪵��ļ��ɲ�����" << endl;
				return -3;
			}
			else if (uof[ii_uof].attr & '\01' && uof[ii_uof].state != 1) {
				//ֻ���ļ����Ǵ���״̬����д
				cout << "\n" << temppath << "��ֻ���ļ�������д��\n";
				return -3;
			}
			//bȷ����pos ins del wpflag mylen
			if (comd[1][1] == 'p' || comd[1][1] == 'P') {
				nstr1 = &comd[1][2];
				pos = atoi(nstr1);
				if (pos <= 0)
				{
					cout << "\n�������ṩ��д��λ�ô���\n";
					return -4;
				}
				if (strnicmp(comd[2], "|ins", 4) == 0) {//	write |pn |ins�������ļ���ͷ��n���ֽڴ�д�����뷽ʽ
					ins = 1;			//����ģʽ
				}
				else if (strnicmp(comd[2], "|del", 4) == 0) {//  write |pn |del�����Ե�ǰ�����ļ�����ָ��λ��n����ʼɾ�����ļ�ĩβ
					del = 1;		//ɾ��ģʽ

				}
				else {
					cout << "�������" << comd[2] << "����" << endl;
					return -3;
				}
			}
			else if (comd[1][1] == 'l' || comd[1][1] == 'L') {//  write |lm |del�����Ե�ǰ�����ļ�����дָ��λ�ÿ�ʼ��ɾ��m���ֽ�
				if (strnicmp(comd[2], "|del", 4) == 0) {
					pos = uof[ii_uof].writep;	//�ӵ�ǰдָ��λ�ÿ�ʼд
					nstr2 = &comd[1][2];
					mylen = atoi(nstr2);
					if (mylen <= 0) {
						cout << "\n�������ṩ��ɾ�����ȴ���\n";
						return -4;
					}
					del = 1;		//ɾ��ģʽ

				}
				else {
					cout << "�������" << comd[2] << "����" << endl;
					return -3;
				}
			}
			else {
				cout << "�������" << comd[1] << "����" << endl;
				return -3;
			}
		}
		//��ʽA:
		//	write <�ļ���> |pn�������ļ���ͷ��n���ֽڴ�д����д��ʽ
		//	write <�ļ���> |ins������дָ����ָλ��д��д�봦��ʼ��ԭ���ݺ���(���뷽ʽ)
		//	write <�ļ���> |app�������ļ�β��д(��ӷ�ʽ)
		//	write <�ļ���> |del������ָ���ļ�����дָ��λ��ɾ�����ļ�ĩβ
		else {
			//a���ҵ���ǰ�ļ�
			FindPath(comd[1], attrib, 0, fcbp);	//����ȫ·����ȥ����..������temppath��
			ii_uof = Check_UOF(temppath);			//��UOF
			if (ii_uof == S)
			{
				cout << "\n�ļ�" << temppath << "δ�򿪻򲻴��ڣ�����д�ļ���\n";
				return -2;
			}
			if (uof[ii_uof].attr & '\01' && uof[ii_uof].state != 1)
			{	//ֻ���ļ����Ǵ���״̬����д
				cout << "\n" << temppath << "��ֻ���ļ�������д��\n";
				return -3;
			}
			//bȷ����pos ins del wpflag mylen
			if (comd[2][0] == '|' && (comd[2][1] == 'p' || comd[2][1] == 'P')) {//	write <�ļ���> |pn�������ļ���ͷ��n���ֽڴ�д����д��ʽ
				nstr1 = &comd[2][2];
				pos = atoi(nstr1);
				if (pos <= 0)
				{
					cout << "\n�������ṩ��д��λ�ô���\n";
					return -4;
				}
			}
			else if (strnicmp(comd[2], "|ins", 4) == 0) {//	write <�ļ���> |ins������дָ����ָλ��д��д�봦��ʼ��ԭ���ݺ���(���뷽ʽ)
				pos = uof[ii_uof].writep;	//�ӵ�ǰдָ��λ�ÿ�ʼд
				wpflag = 1;
				ins = 1;					//����ģʽ
			}
			else if (strnicmp(comd[2], "|app", 4) == 0) {//	write <�ļ���> |app�������ļ�β��д(��ӷ�ʽ)
				pos = uof[ii_uof].fsize + 1;	//�ļ�β�����ģʽ(write <�ļ���> append)
				wpflag = 1;
			}
			else if (strnicmp(comd[2], "|del", 4) == 0) {//	write <�ļ���> |del������ָ���ļ�����дָ��λ��ɾ�����ļ�ĩβ
				pos = uof[ii_uof].writep;	//�ӵ�ǰдָ��λ�ÿ�ʼд
				del = 1;		//ɾ��ģʽ

			}
			else {
				cout << "�������" << comd[2] << "����" << endl;
				return -3;
			}
		}

	}
	else if (k == 3) {
		//��ʽB:
		//  write |pn |lm |del�����Ե�ǰ�����ļ�����ָ��λ��n����ʼɾ��m���ֽ�
		if (comd[1][0] == '|') {
			//a�������ҵ��ļ�
			for (ii_uof = S - 1; ii_uof >= 0; ii_uof--) {
				if (uof[ii_uof].state != 0) {
					break;
				}
			}
			if (ii_uof < 0) {
				cout << "�޴򿪵��ļ��ɲ�����" << endl;
				return -3;
			}
			else if (uof[ii_uof].attr & '\01' && uof[ii_uof].state != 1) {
				//ֻ���ļ����Ǵ���״̬����д
				cout << "\n" << temppath << "��ֻ���ļ�������д��\n";
				return -3;
			}
			//bȷ����pos ins del wpflag mylen
			if (comd[1][0] != '|' || (comd[1][1] != 'p' && comd[1][1] != 'P')) {
				cout << "�������" << comd[1] << "����" << endl;
				return -3;
			}
			else if (comd[2][0] != '|' || (comd[2][1] != 'l' && comd[2][1] != 'L')) {
				cout << "�������" << comd[2] << "����" << endl;
				return -3;
			}
			else if (strnicmp(comd[3], "|del", 4) != 0) {
				cout << "�������" << comd[3] << "����" << endl;
				return -3;
			}
			else {
				nstr1 = &comd[1][2];
				pos = atoi(nstr1);
				if (pos <= 0)
				{
					cout << "\n�������ṩ��ɾ��λ�ô���\n";
					return -4;
				}
				nstr2 = &comd[2][2];
				mylen = atoi(nstr2);
				if (mylen <= 0) {
					cout << "\n�������ṩ��ɾ�����ȴ���\n";
					return -4;
				}
				del = 1;

			}
		}
		//��ʽA��
		//	write <�ļ���> |pn |ins�������ļ���ͷ��n���ֽڴ�д�����뷽ʽ
		//  write <�ļ���> |lm |del������ָ���ļ�����дָ��λ�ÿ�ʼ��ɾ��m���ֽ�
		//  write <�ļ���> |pn |del������ָ���ļ�����ָ��λ��n����ʼɾ�����ļ�ĩβ
		else {
			//a���ҵ���ǰ�ļ�
			FindPath(comd[1], attrib, 0, fcbp);	//����ȫ·����ȥ����..������temppath��
			ii_uof = Check_UOF(temppath);			//��UOF
			if (ii_uof == S)
			{
				cout << "\n�ļ�" << temppath << "δ�򿪻򲻴��ڣ�����д�ļ���\n";
				return -2;
			}
			if (uof[ii_uof].attr & '\01' && uof[ii_uof].state != 1)
			{	//ֻ���ļ����Ǵ���״̬����д
				cout << "\n" << temppath << "��ֻ���ļ�������д��\n";
				return -3;
			}
			//bȷ����pos ins del wpflag mylen
			if (comd[2][0] == '|' && (comd[2][1] == 'p' || comd[2][1] == 'P')) {
				nstr1 = &comd[2][2];
				pos = atoi(nstr1);
				if (pos <= 0)
				{
					cout << "\n�������ṩ��д��λ�ô���\n";
					return -4;
				}
				if (strnicmp(comd[3], "|ins", 4) == 0) {//	write <�ļ���> |pn |ins�������ļ���ͷ��n���ֽڴ�д�����뷽ʽ
					ins = 1;			//����ģʽ
				}
				else if (strnicmp(comd[3], "|del", 4) == 0) {//  write <�ļ���> |pn |del������ָ���ļ�����ָ��λ��n����ʼɾ�����ļ�ĩβ
					del = 1;			//ɾ��ģʽ
				}
				else {
					cout << "�������" << comd[3] << "����" << endl;
					return -3;
				}
			}
			else if (comd[2][0] == '|' && (comd[2][1] == 'l' || comd[2][1] == 'L')) {//  write <�ļ���> |lm |del������ָ���ļ�����дָ��λ�ÿ�ʼ��ɾ��m���ֽ�

				if (strnicmp(comd[3], "|del", 4) == 0) {
					pos = uof[ii_uof].writep;	//�ӵ�ǰдָ��λ�ÿ�ʼд
					nstr2 = &comd[2][2];
					mylen = atoi(nstr2);
					if (mylen <= 0) {
						cout << "\n�������ṩ��ɾ�����ȴ���\n";
						return -4;
					}

					del = 1;
				}
				else {
					cout << "�������" << comd[3] << "����" << endl;
					return -3;
				}
			}
			else {
				cout << "�������" << comd[2] << "����" << endl;
				return -3;
			}

		}
	}
	else if (k == 4) {
		//��ʽA:
		//  write <�ļ���> |pn |lm |del������ָ���ļ�����ָ��λ��n����ʼɾ��m���ֽ�


		//��ʽB: ����
		if (comd[1][0] == '|') {	//��ʽB��k������Ϊ3
			cout << "�����������" << endl;
			return -3;
		}
		else {
			//a���ҵ���ǰ�ļ�
			FindPath(comd[1], attrib, 0, fcbp);	//����ȫ·����ȥ����..������temppath��
			ii_uof = Check_UOF(temppath);			//��UOF
			if (ii_uof == S)
			{
				cout << "\n�ļ�" << temppath << "δ�򿪻򲻴��ڣ�����д�ļ���\n";
				return -2;
			}
			if (uof[ii_uof].attr & '\01' && uof[ii_uof].state != 1)
			{	//ֻ���ļ����Ǵ���״̬����д
				cout << "\n" << temppath << "��ֻ���ļ�������д��\n";
				return -3;
			}
			//bȷ����pos ins del wpflag mylen
			//  write <�ļ���> |pn |lm |del������ָ���ļ�����ָ��λ��n����ʼɾ��m���ֽ�
			if (comd[2][0] != '|' || (comd[2][1] != 'p' && comd[2][1] != 'P')) {
				cout << "�������" << comd[2] << "����" << endl;
				return -3;
			}
			else if (comd[3][0] != '|' || (comd[3][1] != 'l' && comd[3][1] != 'L')) {
				cout << "�������" << comd[3] << "����" << endl;
				return -3;
			}
			else if (strnicmp(comd[4], "|del", 4) != 0) {
				cout << "�������" << comd[4] << "����" << endl;
				return -3;
			}
			else {
				nstr1 = &comd[2][2];
				pos = atoi(nstr1);
				if (pos <= 0)
				{
					cout << "\n�������ṩ��д��λ�ô���\n";
					return -4;
				}
				nstr2 = &comd[3][2];
				mylen = atoi(nstr2);
				if (mylen <= 0) {
					cout << "\n�������ṩ��ɾ�����ȴ���\n";
					return -4;
				}
				del = 1;	//ɾ��ģʽ
			}
		}
	}
	else {
		cout << "���������������̫��" << endl;
		return  -1;
	}


	/************************************************************************************************************************/
	if (pos <= 0)
	{
		cout << "\n�������ṩ��д���ɾ��λ�ô���\n";
		return -1;
	}
	if (pos >= uof[ii_uof].fsize + 1)
	{
		pos = uof[ii_uof].fsize + 1;
		ins = 0;						//������������ǲ��뷽ʽ
	}

	pos--;							//ʹpos��0��ʼ

	if (del) {
		fcbp = uof[ii_uof].fp;
		len0 = uof[ii_uof].fsize;				//ȡ�ļ�ԭ���ĳ���ֵ
		if (len0 == 0)						//���ǿ��ļ�
		{
			cout << uof[ii_uof].fname << "�ǿ��ļ������ܽ���ɾ����ʽ��д��������" << endl;
			return 1;
		}
		//���´����ļ��ǿյ����

		buf = new char[len0 + 1];
		file_to_buffer(fcbp, buf);		//�ļ�����buf
		if (mylen == 0) {//��posλ��ɾ�����ļ�ĩβ
			buf[pos] = '\0';

			ii = buffer_to_file(fcbp, buf);
			if (ii == 0)	//д�ļ�ʧ��
				return ii;
			uof[ii_uof].fsize = uof[ii_uof].fp->Fsize;
			uof[ii_uof].writep = uof[ii_uof].fsize + 1;
			cout << "\nɾ����ʽ��д������" << uof[ii_uof].fname << "�ɹ�.\n";
			return 1;
		}
		else {//��posλ�ÿ�ʼ��ɾ��mylen���ֽ�

			delbuf = &buf[pos + mylen];
			buf[pos] = '\0';
			strcat(buf, delbuf);	//��delbuf��ָ�ַ�����ӵ�buf��β��(����buf��β���� ��\0��)����� ��\0��

			ii = buffer_to_file(fcbp, buf);
			if (ii == 0)	//д�ļ�ʧ��
				return ii;
			uof[ii_uof].fsize = uof[ii_uof].fp->Fsize;
			uof[ii_uof].writep = uof[ii_uof].fsize + 1;
			cout << "\nɾ����ʽ��д������" << uof[ii_uof].fname << "�ɹ�.\n";
			return 1;

		}
	}
	else {
		cout << "\n������д���ļ�������(�����������" << sizeof(Buffer) - 1 << "���ֽ�)��\n";
		cin.getline(Buffer, BSIZE);
		len1 = strlen(Buffer);
		if (len1 == 0)			//���볤��Ϊ0,���ı��ļ�
			return 0;
		fcbp = uof[ii_uof].fp;
		len0 = uof[ii_uof].fsize;				//ȡ�ļ�ԭ���ĳ���ֵ
		if (len0 == 0)						//���ǿ��ļ�
		{
			ii = buffer_to_file(fcbp, Buffer);
			if (ii == 0)	//д�ļ�ʧ��
				return ii;
			uof[ii_uof].fsize = uof[ii_uof].fp->Fsize;
			uof[ii_uof].faddr = uof[ii_uof].fp->Addr;
			uof[ii_uof].readp = 1;
			uof[ii_uof].writep = uof[ii_uof].fsize + 1;
			return 1;
		}
		//���´����ļ��ǿյ����
		len = len1 + pos + ins * (len0 - pos);		//����д����ɺ��ļ��ĳ���
		bn0 = len0 / SIZE + (short)(len0 % SIZE > 0);	//�ļ�ԭ��ռ�õ��̿���
		bn1 = len / SIZE + (short)(len % SIZE > 0);		//д����ļ���ռ�õ��̿���
		if (FAT[0] < bn1 - bn0)
		{
			cout << "\n���̿ռ䲻��,����д���ļ�.\n";
			return -1;
		}
		buf = new char[len + 1];
		if (buf == 0)
		{
			cout << "\n�����ڴ�ʧ�ܡ�\n";
			return -1;
		}
		file_to_buffer(fcbp, buf);		//�ļ�����buf
		if (ins)	//���ǲ��뷽ʽ
		{
			for (ii = len0; ii >= pos; ii--)
				buf[ii + len1] = buf[ii];	//����,�ճ������Buffer
			jj = pos;
			ii = 0;
			while (Buffer[ii] != '\0')		//Buffer���뵽buf
				buf[jj++] = Buffer[ii++];

			if (wpflag)
				buf[jj] = '\0';
		}
		else {//���Ǹ�д��ʽ
			//���ܽ�����������strcpy(&buf[pos],Buffer);
			ii = pos;
			for (index = 0; index <= len1 && Buffer[index] != '\0'; index++) {
				buf[ii] = Buffer[index];
				ii++;
			}
			if (wpflag)
				buf[ii] = '\0';
		}

		buffer_to_file(fcbp, buf);
		//if(buf)
		//delete [] buf;
		uof[ii_uof].fsize = uof[ii_uof].fp->Fsize;
		uof[ii_uof].writep = uof[ii_uof].fsize + 1;
		cout << "\nд�ļ�" << uof[ii_uof].fname << "�ɹ�.\n";
		return 1;
	}

}

////////////////////////////////////////////////////////////////

int CloseComd(int k)				//close����Ĵ��������ر��ļ�
{
	// close [<�ļ���>]����ָ���ļ��Ѵ򿪣���ر�֮������UOF��ɾ�����ļ�
	// ��Ӧ�ı�����ļ�δ�򿪻��ļ������ڣ��ֱ�����й���Ϣ��
	//�ر�ָ���ļ�����������δָ���ļ�����رա���ǰ�����ļ�����
	//��ν����ǰ�����ļ�������ָ��һ��ʹ��create��open��write��read��rewind��fseek��������������ļ���
	int i_uof;
	char attrib = '\0';
	FCB* p;
	if (k < 1)
	{
		//cout << "\n������ȱ���ļ�����\n";
		//return -1;
		//��UOF����Ѱ�����򿪵��ļ�
		for (i_uof = S - 1; i_uof >= 0; i_uof--) {
			if (uof[i_uof].state != 0) {
				break;
			}
		}
		if (i_uof < 0) {
			cout << "��ǰû���ļ��ɹرա�" << endl;
		}
		else {
			uof[i_uof].state = 0;			//��UOF��������ļ��Ǽ���
			p = uof[i_uof].fp;			//ȡ���ļ���Ŀ¼��λ��ָ��
			p->Addr = uof[i_uof].faddr;	//�����ļ����׿��
			p->Fsize = uof[i_uof].fsize;	//�����ļ��Ĵ�С
			cout << "\n�ر��ļ�" << uof[i_uof].fname << "�ɹ���\n";
		}
	}
	else {
		FindPath(comd[1], attrib, 0, p);	//����ȫ·����ȥ����..������temppath��
		i_uof = Check_UOF(temppath);		//��UOF
		if (i_uof == S)
			cout << "\n�ļ�" << temppath << "δ�򿪻򲻴��ڣ����ܹرա�\n";
		else
		{
			uof[i_uof].state = 0;			//��UOF��������ļ��Ǽ���
			p = uof[i_uof].fp;			//ȡ���ļ���Ŀ¼��λ��ָ��
			p->Addr = uof[i_uof].faddr;	//�����ļ����׿��
			p->Fsize = uof[i_uof].fsize;	//�����ļ��Ĵ�С
			cout << "\n�ر��ļ�" << temppath << "�ɹ���\n";
		}
	}
	return 1;
}

/////////////////////////////////////////////////////////////////

void CloseallComd(int disp)    //closeall����رյ�ǰ�û��������ļ�
{
	int i_uof,j,k;
	FCB *p;
	for (k=i_uof=0;i_uof<S;i_uof++)
	{
		j=uof[i_uof].state;	//UOF��״̬>0Ϊ��Ч�Ǽ���
		if (j>0)
		{
			k++;  //�Ѵ��ļ�����
			uof[i_uof].state=0;			//��UOF��������ļ��Ǽ���
			p=uof[i_uof].fp;			//ȡ���ļ���Ŀ¼��λ��ָ��
			p->Addr=uof[i_uof].faddr;	//�����ļ����׿��
			p->Fsize=uof[i_uof].fsize;	//�����ļ��Ĵ�С
			cout<<"\n�ļ�"<<uof[i_uof].fname<<"�ѹر�.\n";
		}
	}
	if (!disp)
		return;
	if (k==0)
		cout<<"\n��û�д��ļ��������ļ��ɹرա�\n\n";
	else
		cout<<"\n���ر� "<<k<<" ���ļ�.\n\n";
}

/////////////////////////////////////////////////////////////////

short int SAVE_bn(short bb)
{
	// ��udtab�д洢��ɾ���ļ��Ŀ��

	short i=0,b0,b,bs;
	if (bb==0)		//��ɾ���ļ��ǿ��ļ�
		return bb;
	bs=getblock();
	short *pb=(short*) Disk[bs];
	while (bb>0)
	{
		pb[i]=bb;
		bb=FAT[bb];
		i++;
		if (i==SIZE/2)
		{
			i=0;
			b0=b;
			b=getblock();
			FAT[b0]=b;
			pb=(short*) Disk[b];
		}
	}
	pb[i]=-1;
	return bs;
}

/////////////////////////////////////////////////////////////////

void Del1Ud(short a)
{
	// ��udtab����ɾ��һ���ǰ�ƺ�������

	short i,b,b0;
	b=udtab[a].fb;
	while (b>0)
	{	//���մ洢�ļ���ŵĴ��̿ռ�
		b0=b;
		b=FAT[b];
		FAT[b0]=0;
		FAT[0]++;
	}
	for (i=a;i<Udelp-1;i++)		//udtab���б���ǰ��һ��λ��
		udtab[i]=udtab[i+1];
	Udelp--;
}

/////////////////////////////////////////////////////////////////

int PutUdtab(FCB *fp)
{
	//��udtab�м���һ����

	short bb,bn,n,m,size;
	size=fp->Fsize;
	bn=size/SIZE+(size%SIZE>0)+1;	//�ļ����̿�Ÿ���(��-1)
	n=SIZE/sizeof(short);			//ÿ���̿�ɴ洢���̿����
	m=bn/n+(short)(bn%n>0);			//����m���̿�洢�ļ��Ŀ��
	if (Udelp==DM)
		Del1Ud(0);
	if (m>FAT[0])
	{
		cout<<"\n���̿ռ䲻��,���ܱ���ɾ���ָ���Ϣ,���ļ�ɾ���󽫲��ָܻ�.\n";
		return -1;
	}
	strcpy(udtab[Udelp].gpath,temppath);
	strcpy(udtab[Udelp].ufname,fp->FileName);
	bb=udtab[Udelp].ufaddr=fp->Addr;
	udtab[Udelp].fb=SAVE_bn(bb);	//���汻ɾ���ļ����̿��
	Udelp++;						//����ָ��λ��
	return 1;
}

/////////////////////////////////////////////////////////////////

int DelComd(int k) //del(ɾ���ļ�)�������
{
    // ɾ���ļ���del <�ļ���>��ɾ��ָ�����ļ����������Ŀ¼��ͻ���
    // ����ռ�ô��̿ռ䡣����ֻ���ļ���ɾ��ǰӦѯ���û����õ�ͬ���
    // ����ɾ������ָ���ļ�����ʹ��ʱ����ʾ"�ļ�����ʹ�ã�����ɾ��"
    // ����Ϣ����ָ���ļ�������ʱ����������Ϣ��
    // ɾ���ļ�ʱ�������ļ����й���Ϣ��¼��ɾ���ļ��ָ���Ϣ��udtab�У�
    // �Ա������ָ�ʱʹ�á�

    short i, s0, s;
    char yn, attr;
    char attrib = '\0', * FileName;
    char gFileName[PATH_LEN]; //����ļ�ȫ·����
    FCB* fcbp;
    bool matchAll = false;
    char ch = '\0';

    s0 = ProcessPath(comd[1], FileName, k, 1, '\20'); //ȡFileName����Ŀ¼���׿��
    if (strcmp(FileName, "*") == 0)
    { //ͨ���
        matchAll = true;
    }
    else if (s0 < 1)						 //·������
        return s0;							 //ʧ�ܣ�����
    s = FindFCB(FileName, s0, attrib, fcbp); //ȡFileName���׿��(���������)
    if (s < 0 && matchAll == false)
    {
        cout << "\nҪɾ�����ļ������ڡ�\n";
        return -2;
    }
    //////////////////////////////////////////////////////////
    FCB* tmp = (FCB*)Disk[s0];
    do
    {
        if (matchAll == true)
            fcbp = tmp;
        for (i = 0; i < 4; i++, fcbp++)
        {
            ch = fcbp->FileName[0]; //ȡ�ļ�(Ŀ¼)���ĵ�һ���ַ�
            if (ch == (char)0xe5)   //��Ŀ¼��
                continue;
            if (ch == '\0') //����Ŀ¼β��
                break;
            if (fcbp->Fattrib >= '\20') //����Ŀ¼������Ŀ¼
            {
                continue;
            }
            strcpy(gFileName, temppath);
            i = strlen(temppath);
            if (temppath[i - 1] != '/')
                strcat(gFileName, "/");
            FileName = fcbp->FileName;
            strcat(gFileName, FileName); //�����ļ���ȫ·����
            i = Check_UOF(gFileName);	//��UOF
            if (i < S)					 //���ļ�����UOF��
            {
                cout << "\n�ļ�" << gFileName << "����ʹ�ã�����ɾ��!\n";
                return -3;
            }
            attr = fcbp->Fattrib & '\01';
            if (attr == '\01')
            {
                cout << "\n�ļ�" << gFileName << "��ֻ���ļ�����ȷ��Ҫɾ������(y/n) ";
                cin >> yn;
                if (yn != 'Y' && yn != 'y')
                    return 0; //��ɾ��������
            }
            i = PutUdtab(fcbp); //��ɾ���ļ����й���Ϣ���浽udtab����
            if (i < 0)			//����̿ռ䲻�㣬���ܱ��汻ɾ���ļ�����Ϣ
            {
                cout << "\n���Ƿ���Ҫɾ���ļ� " << gFileName << " ? (y/n) : ";
                cin >> yn;
                if (yn == 'N' || yn == 'n')
                    return 0; //��ɾ������
            }
            fcbp->FileName[0] = (char)0xe5; //ɾ��Ŀ¼��
            while (s > 0)					//���մ��̿ռ�
            {
                s0 = s;
                s = FAT[s];
                FAT[s0] = 0;
                FAT[0]++;
            }
            if (matchAll == false)
                break;
        }
        s0 = FAT[s0];
    } while (s0 > 0 && ch != '\0' && matchAll == true);
    return 1;
}

/////////////////////////////////////////////////////////////////

int Udfile(FCB *fdp,short s0,char *fn,short &cc)
{
	// ��Ŀ¼���ҵ���ɾ���ļ�(�ļ������ַ�Ϊ'\0xe5')��Ŀ¼�����ô˺���
	// ��������udtab����������ң����ҵ��뱻ɾ���ļ���·����ͬ������(����
	// ������)��ͬ���׿����ͬ�ı���ʱ����ʾ�����ܿ��Իָ���������ѯ����
	// ���õ��϶��𸴺󣬼���ʼ�ָ��������ָ��������ַ���������ͻʱ������
	// ���������ļ���������ָ����������ļ�ԭ��ռ�õ��̿��������ã���ָ�
	// ʧ�ܡ����ۻָ��ɹ���񣬶���ɾ��udtab�ж�Ӧ�ı��

	int i,j;
	char yn[11],Fname[INPUT_LEN];
	short *stp,b,b0,b1,s;
	FCB* fcbp;

	for (i=0;i<Udelp;i++)
	{
		if (strcmp(udtab[i].gpath,temppath)==0 && strcmp(&udtab[i].ufname[1],fn)==0
			&& udtab[i].ufaddr==fdp->Addr)
		{
			cout<<"\n�ļ�"<<udtab[i].ufname<<"���ܿ��Իָ����Ƿ�ָ�����(y/n) ";
			cin.getline(yn,10);
			if (yn[0]=='y' || yn[0]=='Y')
			{
				if (udtab[i].ufaddr>0)
				{
					b=udtab[i].fb;			//ȡ�洢��ɾ�ļ��̿�ŵĵ�һ�����
					stp=(short*) Disk[b];	//stpָ����̿�
					b0=stp[0];				//ȡ��ɾ���ļ��ĵ�һ����ŵ�b0
					j=1;
					while (b0>0)
					{
						if (FAT[b0]!=0)		//����ɾ���ļ����̿��Ѿ�������
						{
							cout<<"\n�ļ�"<<udtab[i].ufname<<"�Ѳ��ָܻ���\n";
							Del1Ud(i);		//ɾ��udtab���е�i��(�ñ���������)
							return -1;
						}
						b0=stp[j++];		//ȡ��ɾ���ļ�����һ����ŵ�b0
						if (j==SIZE/2 && b0!=-1)
						{
							b=FAT[b];
							j=0;
							stp=(short*) Disk[b];
						}
					}
					b=udtab[i].fb;
					stp=(short*) Disk[b];
					b0=b1=stp[0];
					j=1;
					while (b1>0)
					{
						b1=stp[j];
						FAT[b0]=b1;
						FAT[0]--;
						b0=b1;
						j++;
						if (j==SIZE/2 && b1!=-1)
						{
							b=FAT[b];
							j=0;
							stp=(short*) Disk[b];
						}
					}
				}
				s=FindFCB(udtab[i].ufname,s0,'\0',fcbp);
				fdp->FileName[0]=udtab[i].ufname[0];	//�ָ��ļ���
				if (s>=0)	//�������ļ�
				{
					cout<<"\n��Ŀ¼���Ѿ�������Ϊ"<<udtab[i].ufname<<"���ļ���"
						<<"��Ϊ���ָ��ļ�����һ���µ����֣�";
					while (1)
					{
						cin.getline(Fname,INPUT_LEN);
						if (IsName(Fname))	//����������ַ��Ϲ���
						{
							s=FindFCB(Fname,s0,'\0',fcbp);	//�����������з�����
							if (s>=0)
								cout<<"\n������ļ�������������ͻ��\n�����������ļ�����";
							else
								break;			//�������ֺϷ����������ļ����ڡ��˳�ѭ��
						}
						else					//�������ֲ�������������
							cout<<"\n������ļ������Ϸ���\n�����������ļ�����";
					}
					strcpy(fdp->FileName,Fname);
				}
				cc++;		//���ָ��ļ�����1
				Del1Ud(i);	//ɾ��udtab���е�i��
			}
		}
	}
	return 0;
}

/////////////////////////////////////////////////////////////////

int UndelComd(int k)		//undel����
{
	// ������ʽ��undel [<Ŀ¼��>]
	// ����ܣ��ָ�ָ��Ŀ¼�б�ɾ�����ļ�
	// ����������2��������ʽ��
	//		undel�����ָ���ǰĿ¼�б�ɾ�����ļ�
	//		undel <Ŀ¼��>�����ָ�ָ��Ŀ¼�б�ɾ�����ļ�

	short i,s,s0,cc=0;		//cc�ǻָ��ļ���������
	char *fn;
	FCB *fcbp1;
	if (k>1)
	{
		cout<<"\n������в�����\n";
		return -1;
	}
	if (k<1)		//���������޲���
	{
		strcpy(temppath,curpath.cpath);
		s0=s=curpath.fblock;
	}
	else
	{
		s0=s=FindPath(comd[1],'\020',1,fcbp1);
		if (s<0)
		{
			cout<<"\n������������·������\n";
			return -2;
		}
	}
	while (s>0)			//���׿��Ϊs��Ŀ¼�ұ�ɾ���ļ��ı��ֱ��Ŀ¼β��
	{
		fcbp1=(FCB*) Disk[s];
		for (i=0;i<4;i++,fcbp1++)
		{
			if (fcbp1->FileName[0]==(char)0xe5)		//�ҵ����ܽ���ɾ���ָ���Ŀ¼��
			{
				fn=&(fcbp1->FileName[1]);
				Udfile(fcbp1,s0,fn,cc);
			}
		}
		s=FAT[s];	//ȡ��һ���̿��
	}
	cout<<"\n���ָ��� "<<cc<<" ����ɾ�����ļ���\n";
	return 1;
}

/////////////////////////////////////////////////////////////////

int ReadComd(int k)		//read����Ĵ����������ļ�
{
	//��ָ��������ؼ���Ϣ���� pos ��readc
	// ���ļ���read [<�ļ���>] [<λ��m>] [<�ֽ���n>]�����Ѵ򿪵��ļ����ļ����ݲ���ʾ������
	// ��λ�á���������Ӷ�ָ����ָλ�ÿ�ʼ��������"λ��"���������ָ��λ�ô���ʼ����λ
	// ��m��ָ���ļ���ͷ��m���ֽڴ�����m��1��ʼ��ţ�������"�ֽ���"���������ָ��λ�ö�
	// ���ļ�ĩβ������"�ֽ���n"���������ָ��λ�ÿ�ʼ��n���ֽڡ�ÿ��һ���ֽڣ���ָ���
	// ��һ���ֽڡ����ļ�δ�򿪻��ļ������ڣ��ֱ����������Ϣ��
	// read������������������ʽ��
	//��ʽA:
	//	read <�ļ���>�����Ӷ�ָ��λ�ÿ�ʼ�����ļ�β��
	//	read <�ļ���> | pm������ָ���ļ�����ָ��λ��m��ʼ�����ļ�β��(������ʽ)
	//	read <�ļ���> | ln������ָ���ļ����Ӷ�ָ��λ�ÿ�ʼ��n���ֽ�(������ʽ)
	//	read <�ļ���> | pm | ln������ָ���ļ�����ָ��λ��m��ʼ��n���ֽ�(������ʽ)

	//��ʽB:
	//	read��������ǰ�����ļ����Ӷ�ָ��λ�ÿ�ʼ�����ļ�β��(������ʽ)
	//	read | pm��������ǰ�����ļ�����ָ��λ��m��ʼ�����ļ�β��(������ʽ)
	//	read | ln��������ǰ�����ļ����Ӷ�ָ��λ�ÿ�ʼ��n���ֽ�(������ʽ)
	//	read | pm | ln��������ǰ�����ļ�����ָ��λ��m��ʼ��n���ֽ�(������ʽ)

	// ˵�����մ򿪵��ļ������ָ��ָ���ļ���ͷ(����ָ�����1)��Լ�����ļ��Ķ�ָ�����0��

	short i, j, ii, i_uof, pos, offset;
	short b, b0, bnum, count = 0, readc;
	char attrib = '\0';
	char Buffer[SIZE + 1];
	char* nstr, *nstr1, *nstr2;
	FCB* fcbp;

	if (k < 1) {	//read ����ǰ�����ļ����Ӷ�ָ��λ�ÿ�ʼ�����ļ�β��
		for (i_uof = S - 1; i_uof >= 0; i_uof--) {
			if (uof[i_uof].state != 0) {
				break;
			}
		}
		if (i_uof < 0) {
			cout << "�޴򿪵��ļ��ɲ�����" << endl;
			return -3;
		}
		else if (uof[i_uof].readp == 0) {
			cout << "\n�ļ�" << temppath << "�ǿ��ļ���\n";
			return 1;
		}
		else {
			pos = uof[i_uof].readp;//�Ӷ�ָ����ָλ�ÿ�ʼ��
			if (pos > uof[i_uof].fsize)
			{
				cout << "\n��ָ����ָ���ļ�β�����޿ɶ���Ϣ��\n";
				return 1;
			}
			readc = uof[i_uof].fsize - pos + 1;	//�����ļ�β�������readc���ֽ�
		}

	}
	else if (k == 1) {
		//��ʽB:
		//  read |pm��������ǰ�����ļ�����ָ��λ��m��ʼ�����ļ�β��(������ʽ)
		//	read |ln��������ǰ�����ļ����Ӷ�ָ��λ�ÿ�ʼ��n���ֽ�(������ʽ)
		if (comd[1][0] == '|') {
			//a�������ҵ����ļ�
			for (i_uof = S - 1; i_uof >= 0; i_uof--) {
				if (uof[i_uof].state != 0) {
					break;
				}
			}
			if (i_uof < 0) {
				cout << "�޴򿪵��ļ��ɲ�����" << endl;
				return -3;
			}
			else if (uof[i_uof].readp == 0) {
				cout << "\n�ļ�" << temppath << "�ǿ��ļ���\n";
				return 1;
			}
			else {
				//b��ȷ��pos ��readc
				if (comd[1][1] == 'p' || comd[1][1] == 'P') {
					// read |p2
					nstr1 = &comd[1][2];
					pos = atoi(nstr1);	//��������ָ��λ�ö�
					if (pos <= 0 || pos > uof[i_uof].fsize)
					{
						cout << "\n�������ṩ�Ķ�λ�ô���\n";
						return -3;
					}
					readc = uof[i_uof].fsize - pos + 1;	//�����ļ�β�������readc���ֽ�
				}
				else if (comd[1][1] == 'l' || comd[1][1] == 'L') {
					//read |l2  �Ӷ�ָ��λ�ÿ�ʼ��n���ֽ�,����Ҫ��ʼ��pos
					pos = uof[i_uof].readp;//�Ӷ�ָ����ָλ�ÿ�ʼ��
					if (pos > uof[i_uof].fsize)
					{
						cout << "\n��ָ����ָ���ļ�β�����޿ɶ���Ϣ��\n";
						return 1;
					}

					nstr2 = &comd[1][2];
					readc = atoi(nstr2);
					if (readc < 1)
					{
						cout << "\n�������ṩ�Ķ��ֽ�������\n";
						return -4;
					}
					if (readc > uof[i_uof].fsize - pos + 1)
						readc = uof[i_uof].fsize - pos + 1;
				}
				else {
					cout << "����" << comd[1] << "����" << endl;
					return -3;
				}
			}

		}
		else {//��ʽA��read <�ļ���>�����Ӷ�ָ��λ�ÿ�ʼ�����ļ�β��
			//a�������ҵ����ļ�
			FindPath(comd[1], attrib, 0, fcbp);	//����ȫ·����ȥ����..������temppath��
			i_uof = Check_UOF(temppath);			//��UOF
			if (i_uof == S)
			{
				cout << "\n�ļ�" << temppath << "δ�򿪻򲻴��ڣ����ܶ��ļ���\n";
				return -2;
			}
			if (uof[i_uof].readp == 0)
			{
				cout << "\n�ļ�" << temppath << "�ǿ��ļ���\n";
				return 1;
			}

			//b��ȷ��pos ��readc
			pos = uof[i_uof].readp;//�Ӷ�ָ����ָλ�ÿ�ʼ��
			if (pos > uof[i_uof].fsize)
			{
				cout << "\n��ָ����ָ���ļ�β�����޿ɶ���Ϣ��\n";
				return 1;
			}
			readc = uof[i_uof].fsize - pos + 1;	//�����ļ�β�������readc���ֽ�
		}

	}
	else if (k == 2) {
		//��ʽB��read |pm |ln��������ǰ�����ļ�����ָ��λ��m��ʼ��n���ֽ�(������ʽ)
		if (comd[1][0] == '|') {
			//a�������ҵ����ļ�
			for (i_uof = S - 1; i_uof >= 0; i_uof--) {
				if (uof[i_uof].state != 0) {
					break;
				}
			}
			if (i_uof < 0) {
				cout << "�޴򿪵��ļ��ɲ�����" << endl;
				return -3;
			}
			else if (uof[i_uof].readp == 0) {
				cout << "\n�ļ�" << temppath << "�ǿ��ļ���\n";
				return 1;
			}
			else {
				//b��ȷ��pos ��readc
				if (comd[1][1] != 'p'&& comd[1][1] != 'P') {
					cout << "����" << comd[1] << "����" << endl;
					return -3;
				}
				else if (comd[2][0] != '|' || (comd[2][1] != 'l'&& comd[2][1] != 'L')) {
					cout << "����" << comd[2] << "����" << endl;
					return -3;
				}
				else {
					//pos
					nstr1 = &comd[1][2];
					pos = atoi(nstr1);	//��������ָ��λ�ö�
					if (pos <= 0 || pos > uof[i_uof].fsize)
					{
						cout << "\n�������ṩ�Ķ�λ�ô���\n";
						return -3;
					}
					//readc
					nstr2 = &comd[2][2];
					readc = atoi(nstr2);
					if (readc < 1)
					{
						cout << "\n�������ṩ�Ķ��ֽ�������\n";
						return -4;
					}
					if (readc > uof[i_uof].fsize - pos + 1)
						readc = uof[i_uof].fsize - pos + 1;
				}
			}
		}
		else {
			//��ʽA��
			//  read <�ļ���> |pm������ָ���ļ�����ָ��λ��m��ʼ�����ļ�β��(������ʽ)
			//	read <�ļ���> |ln������ָ���ļ����Ӷ�ָ��λ�ÿ�ʼ��n���ֽ�(������ʽ)

			//a�������ҵ����ļ�
			FindPath(comd[1], attrib, 0, fcbp);	//����ȫ·����ȥ����..������temppath��
			i_uof = Check_UOF(temppath);			//��UOF
			if (i_uof == S)
			{
				cout << "\n�ļ�" << temppath << "δ�򿪻򲻴��ڣ����ܶ��ļ���\n";
				return -2;
			}
			if (uof[i_uof].readp == 0)
			{
				cout << "\n�ļ�" << temppath << "�ǿ��ļ���\n";
				return 1;
			}

			//b��ȷ��pos ��readc
			if (comd[2][0] == '|' && (comd[2][1] == 'p' || comd[2][1] == 'P')) {//  read <�ļ���> |pm������ָ���ļ�����ָ��λ��m��ʼ�����ļ�β��(������ʽ)
				// read boy |p2
				nstr1 = &comd[2][2];
				pos = atoi(nstr1);	//��������ָ��λ�ö�
				if (pos <= 0 || pos > uof[i_uof].fsize)
				{
					cout << "\n�������ṩ�Ķ�λ�ô���\n";
					return -3;
				}
				readc = uof[i_uof].fsize - pos + 1;	//�����ļ�β�������readc���ֽ�
			}
			else if (comd[2][0] == '|' && (comd[2][1] == 'l' || comd[2][1] == 'L')) {//read <�ļ���> |ln������ָ���ļ����Ӷ�ָ��λ�ÿ�ʼ��n���ֽ�(������ʽ)
				//read boy |l2  �Ӷ�ָ��λ�ÿ�ʼ��n���ֽ�,����Ҫ��ʼ��pos
				pos = uof[i_uof].readp;//�Ӷ�ָ����ָλ�ÿ�ʼ��
				if (pos > uof[i_uof].fsize)
				{
					cout << "\n��ָ����ָ���ļ�β�����޿ɶ���Ϣ��\n";
					return 1;
				}

				nstr2 = &comd[2][2];
				readc = atoi(nstr2);
				if (readc < 1)
				{
					cout << "\n�������ṩ�Ķ��ֽ�������\n";
					return -4;
				}
				if (readc > uof[i_uof].fsize - pos + 1)
					readc = uof[i_uof].fsize - pos + 1;
			}
			else {
				cout << "����" << comd[2] << "����" << endl;
				return -3;
			}
		}

	}
	else if (k == 3) {
		//��ʽA��
		//  read <�ļ���> | pm | ln������ָ���ļ�����ָ��λ��m��ʼ��n���ֽ�(������ʽ)

		//��ʽB:
		//����
		if (comd[1][0] == '|') {	//��ʽB��k������Ϊ3
			cout << "�����������" << endl;
			return -3;
		}
		else {
			//a�������ҵ����ļ�
			FindPath(comd[1], attrib, 0, fcbp);	//����ȫ·����ȥ����..������temppath��
			i_uof = Check_UOF(temppath);			//��UOF
			if (i_uof == S)
			{
				cout << "\n�ļ�" << temppath << "δ�򿪻򲻴��ڣ����ܶ��ļ���\n";
				return -2;
			}
			if (uof[i_uof].readp == 0)
			{
				cout << "\n�ļ�" << temppath << "�ǿ��ļ���\n";
				return 1;
			}

			//b��ȷ��pos ��readc
			//read boy |p2 |l10
			if (comd[2][0] == '|' && (comd[2][1] == 'p' || comd[2][1] == 'P') && comd[3][0] == '|' && (comd[3][1] == 'l' || comd[3][1] == 'L')) {
				//pos
				nstr1 = &comd[2][2];
				pos = atoi(nstr1);	//��������ָ��λ�ö�
				if (pos <= 0 || pos > uof[i_uof].fsize)
				{
					cout << "\n�������ṩ�Ķ�λ�ô���\n";
					return -3;
				}
				//readc
				nstr2 = &comd[3][2];
				readc = atoi(nstr2);
				if (readc < 1)
				{
					cout << "\n�������ṩ�Ķ��ֽ�������\n";
					return -4;
				}
				if (readc > uof[i_uof].fsize - pos + 1)
					readc = uof[i_uof].fsize - pos + 1;
			}
			else {
				cout << "ָ���ʽ����" << endl;
				return -3;
			}
		}
	}
	else {
		cout << "�����в�������̫�ࡣ" << endl;
		return -3;
	}


	bnum = (pos - 1) / SIZE;		//���ļ��ĵ�bnum���(bnum��0��ʼ���)
	offset = (pos - 1) % SIZE;	//�ڵ�bnum���ƫ��λ��offset����ʼ��(offset��0��ʼ)
	b = uof[i_uof].faddr;		//ȡ�ļ��׿��
	for (i = 0; i < bnum; i++)	//Ѱ�Ҷ���ĵ�һ���̿��
	{
		b0 = b;
		b = FAT[b];
	}
	ii = offset;
	while (count < readc)		//���ļ���Buffer����ʾ֮
	{
		for (i = ii, j = 0; i < SIZE; i++, j++)
		{
			Buffer[j] = Disk[b][i];
			count++;
			if (count == readc)
			{
				j++;
				break;
			}
		}
		Buffer[j] = '\0';
		cout << Buffer;
		ii = 0;
		b = FAT[b];		//׼������һ���̿�
	}
	cout << endl;
	uof[i_uof].readp = pos + readc;	//������ָ��
	return 1;
}

/////////////////////////////////////////////////////////////////

int CopyComd(int k) //copy����Ĵ������������ļ�
{					// ͨ���: �㶨	Alkane 2019/12/6
    // �޸����� copy <filename> .. ��copy <filename> / �Ĵ��� Alkane 2019/12/11
    // ����	�ϲ����� Alkane 2019/12/18
    // �����ļ���copy <Դ�ļ���> [<Ŀ���ļ���>]
    // ����ܣ�ΪĿ���ļ�����Ŀ¼������µ��̿飬����Դ�ļ������ݸ��Ƶ�Ŀ���ļ���
    // ����������һ��������ġ��ļ���������ָ���һ���������ļ���·������
    // ��Ŀ���ļ���Դ�ļ����ڵ�Ŀ¼��ͬ����ֻ�ܽ��и������ƣ���ʱĿ���ļ�������ʡ��
    // ��Ŀ���ļ���Դ�ļ����ڵ�Ŀ¼��ͬ����ȿɸ�������Ҳ��ͬ�����ƣ�ͬ������ʱĿ���ļ�����ʡ��
    // ���磬����
    //		copy mail email
    // (1) ����ǰĿ¼�в�����email(Ŀ¼���ļ�)����������ǰĿ¼�е��ļ�mail�����Ƴ�
    //     ��ǰĿ¼�µ��ļ�email;
    // (2) ����ǰĿ¼�´���email����email����Ŀ¼�����򽫵�ǰĿ¼�е��ļ�mail�����Ƶ���
    //     ǰĿ¼�е�email��Ŀ¼�ڣ��ļ�����Դ�ļ���ͬ(ͬ������)����ʱ��emailĿ¼���Ѿ�
    //     �����ļ���Ŀ¼mail���������������
    // (3) ����ǰĿ¼�ڴ���email�ļ����������������
    // (4) ����ǰĿ¼�ڲ�����Դ�ļ�mail(������Ȼ��mail����������Ŀ¼��)����Ҳ����
    //����������������Ŀ���ļ�ʱ����Դ�ļ�ͬ�����Ƶ���ǰĿ¼�С����磬��ǰĿ¼Ϊ/usr
    //		copy /box
    // ����������Ѹ�Ŀ¼�е��ļ�box���Ƶ���ǰĿ¼/usr�У��ļ�����Ϊbox��

    //��ע����ͬһĿ¼�У���Ŀ¼����������������ļ���������Ŀ¼������

    // ѧ�����ɿ���ʹ��ͨ����Ķ��ļ�ͬ�����Ƶ����(Ŀ���ļ���Դ�ļ�����Ŀ¼���벻ͬ)��

    short int i, size, s01, s02, s1, s2, s22, b, b0, bnum;
    short int s03, s3, s33;
    char attrib = '\0', * FileName1, * FileName2, * FileName3;
    char gFileName[PATH_LEN]; //����ļ�ȫ·����
    bool matchAll = false;
    char ch;
    //flag1 := .. �Ƿ�ΪFileName2  flag2 := / �Ƿ�ΪFileName2	flag3 := /�Ƿ�ΪFileName3
    bool flag1 = false, flag2 = false, flag3 = false;
    FCB* fcbp, * fcbp1, * fcbp2, * fcbp3;
    //�Ƿ񸲸�ͬ���ļ�
    bool coverSameName = false;
    if (k < 1 || k > 4)
    {
        cout << "\n�����в���̫���̫�١�\n";
        return -1;
    }
    s01 = ProcessPath(comd[1], FileName1, k, 0, '\20'); //ȡFileName����Ŀ¼���׿��
    if (strcmp(FileName1, "*") == 0)
    { // copy * [~]������ͨ���
        matchAll = true;
    }
    else if (s01 < 1)							//·������
        return s01;								//ʧ�ܣ�����
    s1 = FindFCB(FileName1, s01, attrib, fcbp); //ȡFileName(Դ�ļ�)���׿��(���������)
    if (s1 < 0 && matchAll == false)
    {
        cout << "\nҪ���Ƶ��ļ������ڡ�\n";
        return -1;
    }
    //////////////////////////////////////////////////////
    int count_file = 0;
    do
    { //��ʼͨ��������
        fcbp1 = (FCB*)Disk[s01];
        for (i = 0; i < 4; i++, fcbp1++){
            if (matchAll == false){				  //��ͨ�����������ͨ���
                fcbp1 = fcbp; //����Դ�ļ�Ŀ¼��ָ��ֵ
            }
            ch = fcbp1->FileName[0]; //ȡ�ļ�(Ŀ¼)���ĵ�һ���ַ�
            if (ch == (char)0xe5)	//��Ŀ¼��
                continue;
            if (ch == '\0') //����Ŀ¼β��
                break;
            if (fcbp1->Fattrib >= '\20'){//����Ŀ¼������Ŀ¼
                continue;
            }
            FileName1 = fcbp1->FileName;
            strcpy(gFileName, temppath);
            i = strlen(temppath);
            if (temppath[i - 1] != '/')
                strcat(gFileName, "/");
            strcat(gFileName, FileName1); //�����ļ���ȫ·����
            i = Check_UOF(gFileName);	 //��UOF
            if (i < S){ //���ļ�����UOF��
                cout << "\n�ļ�" << gFileName << "�Ѿ��򿪣����ܸ���!\n";
                return -2;
            }
            if (k == 1){//��������Ŀ���ļ�,ͬ�����Ƶ���ǰĿ¼
                s02 = curpath.fblock; //ȡ��ǰĿ¼���׿��
                FileName2 = FileName1;
            }else if (k == 2){ //k=2(�������ṩĿ���ļ�)�����
                s02 = ProcessPath(comd[2], FileName2, k, 0, '\20'); //ȡFileName2����Ŀ¼���׿��
                if (strcmp(comd[2], "/") == 0){
                    if (strcmp(curpath.cpath, "/") == 0){ //��ǰ���ڸ�Ŀ¼��
                        cout << "\n��ǰĿ¼�޸�Ŀ¼\n";
                        return -2;
                    }
                    flag2 = true;
                    strcpy(FileName2, FileName1);
                    s02 = 1;
                }
                else if (s02 < 1) //Ŀ��·������
                    return s02;
                flag1 = strcmp(FileName2, "..") == 0;
            }else{ //k=3 or k=4 ��ʼ�ϲ�����
                if ((strcmp(comd[2], "+") != 0)){ // ����� copy <fileName1> + <fileName> + [obj] ����ʽ,����
                    cout << "\n�����ʽ����\n";
                    return -10086;
                }
                s02 = ProcessPath(comd[3], FileName2, k, 0, '\20'); //ȡFileName2����Ŀ¼���׿��
                if (s02 < 1)										//Ŀ��·������
                    return s02;
            }
            if (!IsName(FileName2)) //�����ֲ����Ϲ���
            {						//���￼���˳��� copy <filename> .. �����
                if (flag1){ //FileName2 ��..
                    if (strcmp(curpath.cpath, "/") == 0)
                    { //��ǰ���ڸ�Ŀ¼��
                        cout << "\n��ǰĿ¼�޸�Ŀ¼\n";
                        return -2;
                    }
                }else{
                    cout << "\n�����е�Ŀ���ļ�������\n";
                    return -2;
                }
            }
            s2 = FindFCB(FileName2, s02, '\040', fcbp2); //ȡFileName2���׿��(���������)
            if (k == 2){											// copy <file1> <file2>
                if (s2 >= 0 && fcbp2->Fattrib <= '\07'){//����ͬ��Ŀ���ļ�
                    cout << "\n�����ļ���Ŀ���ļ�ͬ����\n";
                    char ch;
                    cout << "\n�Ƿ񸲸�ͬ���ļ�?[Y/N]";
                    cin >> ch;
                    if (!(ch == 'Y' || ch == 'y'))
                        return -3;
                    else{			   //ѡ�񸲸�ͬ���ļ�
                        s22 = s02; //����FileName2������������
                        coverSameName = true;
                    }
                }
                if (coverSameName == false){				//FileName2������
                    if (s2 < 0) //FileName2�в����ڣ���s02Ϊ�׿�ŵ�Ŀ¼�ڸ���Ŀ���ļ�
                        s22 = s02;
                    else{//FileName2���ڣ�������Ŀ¼��
                        s22 = s2;
                        if (s2 != s01){//Դ�ļ���Ŀ���ļ���ͬĿ¼
                            b = FindFCB(FileName1, s2, '\040', fcbp); //���FileName2Ŀ¼����û���ļ�FileName1
                            if (b >= 0 && fcbp->Fattrib <= '\07'){
                                cout << "\nĿ��Ŀ¼����ͬ���ļ�.\n";
                                char ch;
                                cout << "\n�Ƿ񸲸�ͬ���ļ�?[Y/N]";
                                cin >> ch;
                                if (!(ch == 'Y' || ch == 'y'))
                                    return -4;
                                else{ //ѡ�񸲸�ͬ���ļ�
                                    coverSameName = true;
                                }
                            }else if (b >= 0){ //���ڵ���ͬ��Ŀ¼
                                cout << "\n����ͬ��Ŀ¼!\n";
                                return -4;
                            }
                            FileName2 = FileName1; //ȱʡĿ���ļ�����ͬ������
                        }
                        else{
                            cout << "\nͬĿ¼����ͬ���ļ�!\n";
                            char ch;
                            cout << "\n�Ƿ񸲸�ͬ���ļ�?[Y/N]";
                            cin >> ch;
                            if (!(ch == 'Y' || ch == 'y'))
                                return -5;
                            else{ //ѡ�񸲸�ͬ���ļ�
                                coverSameName = true;
                                FileName2 = FileName1; //ȱʡĿ���ļ�����ͬ������
                            }
                        }
                    }
                }
                if (coverSameName == false)
                { //ֻ�е�������ͬ���ļ�ʱ��Ѱ���µ�FCB
                    i = FindBlankFCB(s22, fcbp2);
                }else{
                    i = 1;
                    coverSameName = false; //�ָ���־λ
                    fcbp2 = fcbp;
                }
                if (i < 0){
                    cout << "\n�����ļ�ʧ�ܡ�\n";
                    return i;
                }
                size = fcbp1->Fsize;						   //Դ�ļ��ĳ���
                bnum = size / SIZE + (short)(size % SIZE > 0); //����Դ�ļ���ռ�̿���
                if (FAT[0] < bnum){
                    cout << "\n���̿ռ����������ܸ����ļ���\n";
                    return -6; //��ֵ�-6
                }
                *fcbp2 = *fcbp1;					//Դ�ļ���Ŀ¼��Ƹ�Ŀ���ļ�
                strcpy(fcbp2->FileName, FileName2); //дĿ���ļ���
                b0 = 0;
            }else{
                b0 = 0;
                if (s2 < 0)
                { //FileName2 ������
                    cout << "\n�ļ�2������!\n";
                    return -10088;
                }else if (s2 >= 0 && fcbp->Fattrib >= 16){ //FileName2 ��Ŀ¼
                    cout << "\n�޷���Ŀ¼���кϲ����Ʋ���!\n";
                    return -10089;
                }
                else{ // FileName2 ���ڣ�����Ϊ�ļ�
                    if (k == 3){ //��������Ŀ���ļ�,ͬ�����Ƶ���ǰĿ¼
                        s03 = curpath.fblock;
                        FileName3 = FileName1;
                    }
                    else{														//�����д���Ŀ���ļ�
                        s03 = ProcessPath(comd[4], FileName3, k, 0, '\20'); //ȡFileName3����Ŀ¼���׿��
                        if (s03 < 1)										//Ŀ��·������
                            return s03;
                    }
                    if (!IsName(FileName3)) //�����ֲ����Ϲ���
                    {
                        cout << "\n�����е�Ŀ���ļ�������\n";
                        return -2;
                    }
                    s3 = FindFCB(FileName3, s03, '\040', fcbp); //ȡFileName3���׿��ַ,���������
                    if (s3 >= 0 && fcbp->Fattrib <= '\07'){ //����ͬ��Ŀ���ļ�
                        cout << "\n�����ļ���Ŀ���ļ�ͬ��\n";
                        char ch;
                        cout << "\n�Ƿ񸲸�ͬ���ļ�?[Y/N]";
                        cin >> ch;
                        if (!(ch == 'Y' || ch == 'y'))
                            return -100;
                        else{
                            s33 = s03; //��FileName3������
                            coverSameName = true;
                        }
                    }
                    if (coverSameName == false){
                        if (s3 < 0) //FileName3���Ҳ�����
                            s33 = s03;
                        else{ //FileName3����,������Ŀ¼��
                            s33 = s3;
                            if (s3 != s01){ //Դ�ļ���Ŀ���ļ���ͬĿ¼
                                b = FindFCB(FileName1, s3, '\040', fcbp);
                                if (b >= 0 && fcbp->Fattrib <= '\07')
                                {
                                    cout << "\nĿ��Ŀ¼����ͬ���ļ�.\n";
                                    char ch;
                                    cout << "\n�Ƿ񸲸�ͬ���ļ�?[Y/N]";
                                    cin >> ch;
                                    if (!(ch == 'Y' || ch == 'y'))
                                        return -4;
                                    else
                                    { //ѡ�񸲸�ͬ���ļ�
                                        coverSameName = true;
                                    }
                                }else if (b >= 0){ //����ͬ��Ŀ¼
                                    cout << "\n����ͬ��Ŀ¼!\n";
                                    return -1000;
                                }
                                FileName3 = FileName1; //ȱʡĿ���ļ���,ͬ������
                            }else{
                                cout << "\nͬĿ¼����ͬ���ļ�!\n";
                                char ch;
                                cout << "\n�Ƿ񸲸�ͬ���ļ�?[Y/N]";
                                cin >> ch;
                                if (!(ch == 'Y' || ch == 'y'))
                                    return -5;
                                else{ //ѡ�񸲸�ͬ���ļ�
                                    coverSameName = true;
                                    FileName3 = FileName1; //ȱʡĿ���ļ�����ͬ������
                                }
                            }
                        }
                    }
                    if (coverSameName == false){ //ֻ�е�������ͬ���ļ�ʱ��Ѱ�ҿյ�FCB��
                        i = FindBlankFCB(s33, fcbp3);
                    }
                    else{
                        i = 1;
                        coverSameName = false; //�ָ���־λ
                        fcbp3 = fcbp;
                    }
                    if (i < 0){
                        cout << "\n�����ļ�ʧ�ܡ�\n";
                        return -1;
                    }
                }
                size = fcbp1->Fsize + fcbp2->Fsize;			   //Ԥ����Ҫ��size
                bnum = size / SIZE + (short)(size % SIZE > 0); //����Դ�ļ���ռ�̿���
                if (FAT[0] < bnum)
                {
                    cout << "\n���̿ռ䲻��\n";
                    return -1000;
                }
                *fcbp3 = *fcbp1;
                strcpy(fcbp3->FileName, FileName3); //дĿ���ļ���
                b0 = 0;
            }
            if (k <= 2)
                fcbp = fcbp2;
            else
                fcbp = fcbp3;

            while (s1 > 0) //��ʼ�����ļ�����
            {			   //���Դ�ļ����ļ�����Ϊ0,�򲻻�����µ��̿�,ֻ��Դ�ļ���FCB����һ�ݸ�����Ŀ���ļ�
                b = getblock();
                if (b0 == 0)
                    fcbp->Addr = b; //Ŀ���ļ����׿��
                else
                    FAT[b0] = b;
                memcpy(Disk[b], Disk[s1], SIZE); //�����̿�
                s1 = FAT[s1];					 //׼��������һ���̿�
                b0 = b;
            }
            if (k >= 3)
            {
                char buffer1[INPUT_LEN];
                int fsize = file_to_buffer(fcbp, buffer1);
                char buffer2[INPUT_LEN];
                fsize += file_to_buffer(fcbp2, buffer2);
                if (fsize / SIZE > FAT[0])
                {
                    cout << "\n���̿ռ䲻��!\n";
                    return -100;
                }
                strcat(buffer1, buffer2);
                fsize = buffer_to_file(fcbp, buffer1);
            }
            if (matchAll == false)
                return 1;
            if (ch == '\0')
                break;
            s01 = FAT[s01]; //ָ���Ŀ¼����һ���̿�
        }
        count_file ++;
    } while (s01 > 0 && ch != '\0');
    matchAll = false; //���ָ���־λ
    cout << count_file << " file(s) copied";
    return 1;		  //�ļ����Ƴɹ�������
}

/////////////////////////////////////////////////////////////////

int FseekComd(int k)	//fseek����Ĵ�����
{
	// ������ʽ��fseek [<�ļ���>] |p<n>
	// ���ܣ�������дָ���Ƶ�ָ��λ��n��
	//�˴��� | p����ʾλ�ã���<n>����ʾĳ������
	//fseek <�ļ���> |p<n>������ָ���ļ��Ķ���дָ���Ƶ��ļ���n�ֽڴ���
	//fseek |p<n>��������ǰ�����ļ��Ķ���дָ���Ƶ��ļ���n�ֽڴ���������ʽ��
	//Ϊ�˽��ļ�����λ�ò�����������λ�ò���ǰ���ַ���|���Ա����ļ�������
	int i_uof, n;
	char attrib = '\0';
	FCB* fcbp;
	char* nstr;

	if (k < 1)
	{
		cout << "\n�����в�����������\n";
		return -1;
	}
	else if (k == 1) {	//fseek |p<n>��������ǰ�����ļ��Ķ���дָ���Ƶ��ļ���n�ֽڴ���������ʽ��
		//������ʦ�����Ĺ�������������û�н����ж�
		if (comd[1][0] != '|' || comd[1][1] != 'p') {
			cout << "ָ���ʽ����" << endl;
			return -2;
		}
		//ȡ��n
		nstr = &comd[1][2];
		//cout << nstr << endl;
		n = atoi(nstr);

		//�����ҳ���ǰ�����ļ�
		//��UOF����Ѱ�����򿪵��ļ�
		for (i_uof = S - 1; i_uof >= 0; i_uof--) {
			if (uof[i_uof].state != 0) {
				break;
			}
		}
		if (i_uof < 0) {
			cout << "û�д��κ��ļ�����ǰû�пɲ������ļ���" << endl;
			return -2;
		}
		else {
			if (uof[i_uof].fsize == 0)		//���ļ�
			{
				cout << "\n" << temppath << "�ǿ��ļ������ܽ��д˲�����\n";
				return -3;
			}
			if (n <= 0 || n > uof[i_uof].fsize + 1)
			{
				cout << "\nλ�ò������󡣸ò���������1��" << uof[i_uof].fsize + 1 << "֮�䡣\n";
				return -4;
			}
			uof[i_uof].readp = n;				//��ָ���趨Ϊn
			uof[i_uof].writep = n;			//дָ���趨Ϊn
			return 1;						//�޸ĳɹ�������
		}

	}
	else if (k == 2) {	//fseek <�ļ���> |p<n>������ָ���ļ��Ķ���дָ���Ƶ��ļ���n�ֽڴ���
		//������ʦ�����Ĺ�������������û�н����ж�
		if (comd[2][0] != '|' || comd[2][1] != 'p') {
			cout << "ָ���ʽ����" << endl;
			return -2;
		}
		//ȡ��n
		nstr = &comd[2][2];
		//cout << nstr << endl;
		n = atoi(nstr);
		FindPath(comd[1], attrib, 0, fcbp);		//����ȫ·����ȥ����..������temppath��
		i_uof = Check_UOF(temppath);		//��UOF
		if (i_uof == S)
		{
			cout << "\n�ļ�" << temppath << "δ�򿪻򲻴��ڣ����ܲ�����\n";
			return -2;					//����ʧ�ܷ���
		}
		if (uof[i_uof].fsize == 0)		//���ļ�
		{
			cout << "\n" << temppath << "�ǿ��ļ������ܽ��д˲�����\n";
			return -3;
		}
		if (n <= 0 || n > uof[i_uof].fsize + 1)
		{
			cout << "\nλ�ò������󡣸ò���������1��" << uof[i_uof].fsize + 1 << "֮�䡣\n";
			return -4;
		}
		uof[i_uof].readp = n;				//��ָ���趨Ϊn
		uof[i_uof].writep = n;			//дָ���趨Ϊn
		return 1;						//�޸ĳɹ�������
	}
	else {
		cout << "\n�����в�����������\n";
		return -1;
	}

	//cout << comd[1] << " " << comd[2] << endl;
	//cout << comd[2][0] << " " << comd[2][1] << endl;

}

/////////////////////////////////////////////////////////////////

int RenComd(int k)	//ren����Ĵ��������ļ�����
{
	// ������ʽ��ren <ԭ�ļ���> <���ļ���>
	// ��ԭ�ļ������ڣ�����������Ϣ��
	// ��ԭ�ļ����ڣ�������ʹ�ã�Ҳ���ܸ�����ͬ����ʾ������Ϣ��
	// Ӧ������ļ����Ƿ������������

	short i,s0,s;
	char attrib='\0',*FileName;
	char gFileName[PATH_LEN];	//����ļ�ȫ·����
	FCB *fp,*fcbp;
	s0=ProcessPath(comd[1],FileName,k,2,'\20');//ȡFileName����Ŀ¼���׿��
	if (s0<1)			//·������
		return s0;		//ʧ�ܣ�����
	s=FindFCB(FileName,s0,attrib,fcbp);		//ȡFileName���׿��(���������)
	if (s<0)
	{
		cout<<"\nҪ�������ļ������ڡ�\n";
		return -2;
	}
	strcpy(gFileName,temppath);
	i=strlen(temppath);
	if (temppath[i-1]!='/')
		strcat(gFileName,"/");
	strcat(gFileName,FileName);	//�����ļ���ȫ·����
	i=Check_UOF(gFileName);		//��UOF
	if (i<S)					//���ļ�����UOF��
	{
		cout<<"\n�ļ�"<<gFileName<<"�Ѿ��򿪣����ܸ���!\n";
		return -3;
	}
	if (IsName(comd[2]))
	{
		fp=fcbp;						//����ָ��Ҫ�����ļ�Ŀ¼���ָ��
		s=FindFCB(comd[2],s0,attrib,fcbp);	//�����ļ����Ƿ�����
		if (s<0)			//������
		{
			strcpy(fp->FileName,comd[2]);
			return 1;		//��ȷ����
		}
		cout<<"\n���������ļ���ͬ�����ļ���\n";
		return -5;
	}
	cout<<"\n�������ṩ�����ļ�������\n";
	return -4;
}

/////////////////////////////////////////////////////////////////

int AttribComd(int k)	//attrib����Ĵ��������޸��ļ���Ŀ¼����
{
	// ��ʾ�޸��ļ����ԣ�attrib <�ļ���> [��<����>]������������"�ļ�����"������
	// ����ʾָ���ļ������ԣ�����������"�ļ�����"���������޸�ָ���ļ������ԡ�"��
	// ������"����ʽ�С�+r��+h��+s���͡�-r��-h��-s��������ʽ��ǰ��Ϊ����ָ���ļ�
	// Ϊ"ֻ��"��"����"��"ϵͳ"���ԣ�����Ϊȥ��ָ���ļ���"ֻ��"��"����"��"ϵͳ"
	// ���ԡ������Կ����ʹ����˳���ޡ����磺
	//		attrib user/boy +r +h
	// �书�������õ�ǰĿ¼��user��Ŀ¼�е��ļ�boyΪֻ���������ļ�������
	//		attrib /usr/user/box -h -r -s
	// ��������Ĺ�����ȡ���ļ�/usr/user/box��"����"��"ֻ��"��"ϵͳ"���ԡ�
	// ��������ָ�����ļ��Ѵ򿪻򲻴��ڣ�����������Ϣ��
	// ���������ṩ�Ĳ�������Ҳ��ʾ������Ϣ��

	short i,j,i_uof,s;
	char Attrib,attrib='\40';
	char Attr[5],Attr1[4]="RHS";
	char attr[6][3]={"+r","+h","+s","-r","-h","-s"};
	char or_and[6]={'\01','\02','\04','\036','\035','\033'};
	FCB* fcbp;

	if (k<1)
	{
		cout<<"\n������û��ָ���ļ�����\n";
		return -1;
	}
	s=FindPath(comd[1],attrib,1,fcbp);	//Ѱ��ָ�����ļ���Ŀ¼���������׿��
	if (s<0)
	{
		cout<<'\n'<<temppath<<"�ļ���Ŀ¼�����ڡ�\n";
		return -2;
	}
	if (k==1)		//��ʾ�ļ�/Ŀ¼������
	{
		Attrib=fcbp->Fattrib & '\07';
		if (Attrib=='\0')
			strcpy(Attr,"��ͨ");
		else
		{
			for (i=0;i<3;i++)
			{
				if (Attrib & or_and[i])
					Attr[i]=Attr1[i];
				else
					Attr[i]=' ';
			}
			Attr[i]='\0';
		}
		cout<<"\n"<<temppath<<"�������ǣ�"<<Attr<<endl;
		return 1;
	}
	if (fcbp->Fattrib<='\07')		//�����ļ���Ҫ�����Ƿ��ѱ���
	{
		i_uof=Check_UOF(temppath);	//��UOF
		if (i_uof<S)
		{
			cout<<"\n�ļ�"<<temppath<<"�����ţ������޸����ԡ�\n";
			return -3;
		}
	}
	for (i=2;i<=k;i++)		//�������Բ���
	{
		for (j=0;j<6;j++)
			if (_stricmp(comd[i],attr[j])==0)
			{
				if (j<3)
					fcbp->Fattrib=fcbp->Fattrib | or_and[j];
				else
					fcbp->Fattrib=fcbp->Fattrib & or_and[j];
				break;
			}
		if (j==6)
		{
			cout<<"\n�����е����Բ�������\n";
			return -4;
		}

	}
	return 1;	//�޸�������ɣ�����
}

/////////////////////////////////////////////////////////////////

int RewindComd(int k)	//rewind����Ĵ�����������дָ���Ƶ��ļ���ͷ
{
	// ������ʽ��rewind [<�ļ���>]
	// rewind <�ļ���>����ǰĿ¼���ļ��Ķ���дָ���Ƶ��ļ���ͷ�����ļ�boy�����ڻ����ļ�boy��δ�򿪣��򱨴�
	// rewind	������ǰ�����ļ����Ķ���дָ���Ƶ��ļ���ͷ��

	int i_uof;
	char attrib = '\0';
	FCB* fcbp;

	if (k < 1) {
		//��UOF����Ѱ�����򿪵��ļ�
		for (i_uof = S - 1; i_uof >= 0; i_uof--) {
			if (uof[i_uof].state != 0) {
				break;
			}
		}
		if (i_uof < 0) {
			cout << "û�д��κ��ļ�����ǰû�пɲ������ļ���" << endl;
			return -1;
		}
		else {
			if (uof[i_uof].faddr > 0)			//���ǿ��ļ�
				uof[i_uof].readp = 1;			//��ָ���趨Ϊ0
			else
				uof[i_uof].readp = 0;			//�ǿ��ļ��Ķ�ָ���趨Ϊ1
			uof[i_uof].writep = 1;			//�ļ���дָ���趨Ϊ1
			return 1;						// �޸ĳɹ�������
		}
	}
	else if (k == 1) {
		FindPath(comd[1], attrib, 0, fcbp);		//����ȫ·����ȥ����..������temppath��
		i_uof = Check_UOF(temppath);		//��UOF
		if (i_uof == S)
		{
			cout << "\n�ļ�" << temppath << "δ�򿪻򲻴��ڣ����ܲ�����\n";
			return -1;					//����ʧ�ܷ���
		}
		if (uof[i_uof].faddr > 0)			//���ǿ��ļ�
			uof[i_uof].readp = 1;			//��ָ���趨Ϊ0
		else
			uof[i_uof].readp = 0;			//�ǿ��ļ��Ķ�ָ���趨Ϊ1
		uof[i_uof].writep = 1;			//�ļ���дָ���趨Ϊ1
		return 1;						// �޸ĳɹ�������
	}
	else {
		cout << "��������������󡣱��������ֻ����1��������" << endl;
		return -1;
	}

}

/////////////////////////////////////////////////////////////////

void UofComd()	//uof�����ʾ��ǰ�û������ļ���
{
	//��ʾ�û��Ѵ��ļ���UOF������

	int i,k;
	char ch;
	for (k=i=0;i<S;i++)
		k+=uof[i].state;
	if (k>0)
	{
		cout<<"\n���ļ���UOF����������:\n\n"
			<<"�ļ���                       �ļ�����  "
			<<"�׿��  �ļ�����  ״̬  ��ָ��  дָ��\n";
		for (i=0;i<S;i++)
		{
			if (uof[i].state==0)
				continue;					//��Ŀ¼��
			cout.setf(ios::left);
			cout<<setw(32)<<uof[i].fname;	//��ʾ�ļ���
			ch=uof[i].attr;
			switch(ch)
			{
			case '\0': cout<<"��ͨ    ";
				break;
			case '\01': cout<<"R       ";
				break;
			case '\02': cout<<"H       ";
				break;
			case '\03': cout<<"RH      ";
				break;
			case '\04': cout<<"S       ";
				break;
			case '\05': cout<<"RS      ";
				break;
			case '\06': cout<<"HS      ";
				break;
			case '\07': cout<<"RHS     ";
				break;
			default: cout<<"����    ";
			}
			cout<<setw(8)<<uof[i].faddr;	//�׿��
			cout<<setw(8)<<uof[i].fsize;	//�ļ���С
			k=uof[i].state;
			if (k==1)
				cout<<" ����   ";			//״̬Ϊ��������
			else
				cout<<" ��   ";			//״̬Ϊ���򿪡�
			cout<<setw(8)<<uof[i].readp;
			cout<<uof[i].writep<<endl;		//��ָ��
		}
	}
	else
		cout<<"Ŀǰ���޴򿪵��ļ���\n";
}
/*

  */
/////////////////////////////////////////////////////////////////

void save_FAT()	//�����ļ������FAT�������ļ�FAT.txt
{
	int i;
	ofstream ffo;
	ffo.open("FAT.txt");
	for (i=0;i<K;i++)
		ffo<<FAT[i]<<' ';
	ffo.close();
}

/////////////////////////////////////////////////////////////////

void save_Disk() //�����̿��е��ļ�����
{
	int i;
	short *p=(short*) Disk[0];
	p[0]=ffbp;
	p[1]=Udelp;
	ofstream ffo("Disk.dat",ios::binary);
	for (i=0;i<K;i++)
		ffo.write((char*)&Disk[i],SIZE);
	ffo.close();
}

/////////////////////////////////////////////////////////////////

void save_UdTab()	//���汻ɾ���ļ���Ϣ��
{
	int i;
	ofstream ffo("UdTab.dat",ios::binary);
	for (i=0;i<DM;i++)
		ffo.write((char*)&udtab[i],sizeof(udtab[0]));
	ffo.close();
}

/////////////////////////////////////////////////////////////////

int FindFCB(char *Name,int s,char attrib,FCB* &fcbp)
{
	// �ӵ�s�鿪ʼ����������ΪName�ҷ�������attrib��Ŀ¼��
	// ��������Nameû���ҵ����ظ������ҵ����طǸ���(��Ŀ¼ʱ���غ���)
	// ������ȷ����ʱ�����ò���ָ�����fcbpָ��NameĿ¼�

	int i;
	char ch,Attrib;
	while (s>0)
	{
		fcbp=(FCB*) Disk[s];
		for (i=0;i<4;i++,fcbp++)		//ÿ���̿�4��Ŀ¼��
		{
			ch=fcbp->FileName[0];
			if (ch==(char)0xe5)
				continue;
			if (ch=='\0')
				return -1;		//·������(����Ŀ¼β����δ�ҵ�)
			if (strcmp(Name,fcbp->FileName)==0)	//�����ҵ�
			{
				if (attrib=='\040')		//attribΪ32ʱ���ļ�����Ŀ¼����
					return fcbp->Addr;
				Attrib=fcbp->Fattrib;
				if (attrib=='\020' && Attrib>=attrib)	//��Ŀ¼����
					return fcbp->Addr;
				if (attrib=='\0' && Attrib<='\07')		//�ļ�����(�ҵ����ļ�)
					return fcbp->Addr;
				return -1;			//���ַ��ϵ����Բ�����Ȼû���ҵ�
			}
		}
		s=FAT[s];		//ȡ��һ���̿��
	}
	return -2;
}

/////////////////////////////////////////////////////////////////

int FindPath(char *pp,char attrib,int ffcb,FCB* &fcbp)
{
	// ���������и�����·����ȷ��·������ȷ�ԣ�������·�������һ��
	// ����(Ŀ¼��)�����Ŀ¼�ĵ�ַ(�׿��)����·�����д���ȥ��·
	// ���еġ�..����������һ��ȫ·��������temppath�У�����������ffcb
	// ����ʱ��ͨ������FindFCB( )������ʹ�������ɹ�����ʱ,FCB���͵�
	// ���ò���ָ�����fcbpָ��·�����һ��Ŀ¼��Ŀ¼�

	short i,j,len,s=0;
	char paths[60][FILENAME_LEN];	//�ֽ�·����(·������಻����60������)
	char *q,Name[PATH_LEN];

	strcpy(temppath,"/");
	if (strcmp(pp,"/")==0)	//�Ǹ�Ŀ¼
		return 1;			//���ظ�Ŀ¼���׿��
	if (*pp=='/')			//����·�����Ӹ�Ŀ¼��ʼ
	{
		s=1;				//��Ŀ¼���׿��
		pp++;
	}
	else
	{
		s=curpath.fblock;	//���·�����ӵ�ǰĿ¼��ʼ
		strcpy(temppath,curpath.cpath);
	}
	j=0;
	while (*pp!='\0')	//�������е�·���ֽ�
	{
		for (i=0;i<PATH_LEN;i++,pp++)
		{
			if (*pp!='/' && *pp!='\0')
				Name[i]=*pp;
			else
			{
				if (i>0)
				{
					Name[i]='\0';
					if (i>FILENAME_LEN-1)	//���ֹ������ȡǰFILENAME_LEN-1���ַ�
						Name[FILENAME_LEN-1]='\0';
					strcpy(paths[j],Name);
					j++;
				}
				else
					return -1;		//·������
				if (*pp=='/') pp++;
				break;			//�Ѵ����ַ���β��
			}
		}
	}
	for (i=0;i<j;i++)
	{
		if (strcmp(paths[i],"..")==0)
		{
			if (strcmp(temppath,"/")==0)
				return -1;		//·������(��Ŀ¼�޸�Ŀ¼)
			len=strlen(temppath);
			q=&temppath[len-1];
			while (*q!='/') q--;
			*q='\0';
			if (*temppath=='\0')
			{
				*temppath='/';
				temppath[1]='\0';
			}
		}
		else
		{
			if (strcmp(temppath,"/")!=0)
				strcat(temppath,"/");
			strcat(temppath,paths[i]);
		}
		if (ffcb)
		{
			s=FindFCB(paths[i],s,attrib,fcbp);
			if (s<0)
				return s;
		}
	}
	return s;
}

/////////////////////////////////////////////////////////////////

void FatComd()	//������"fat"
{
	cout<<"\n��ǰ����ʣ����п���Ϊ"<<FAT[0]<<endl;
}

/////////////////////////////////////////////////////////////////

void CheckComd()		//check����
{
	int j, i;
	cout<<"\n��ǰ���̿����ǣ�"<<FAT[0]<<endl;
	for (j=0,i=2;i<K;i++)
		if (FAT[i]==0)
			j++;
	FAT[0]=j;
	cout<<"���¼��󣬴��̵Ŀ��п��ǣ�"<<FAT[0]<<endl;
	cout<<"\nffbp="<<ffbp<<endl;
	cout<<"Udelp="<<Udelp<<endl;
}

/////////////////////////////////////////////////////////////////

void ExitComd()		//exit�����
{
	char yn;
	CloseallComd(0);		//�ر����д򿪵��ļ��Է����ݶ�ʧ
	cout<<"\n�˳�ʱFAT��Disk��Udtab�Ƿ�Ҫ���̣�(y/n) ";
	cin>>yn;
	if (yn=='Y' || yn=='y')
	{
		save_FAT();			//FAT�����
		save_Disk();		//���̿��д洢������
		save_UdTab();		//���汻ɾ���ļ���Ϣ��
	}
	exit(0);
}

/////////////////////////////////////////////////////////////////

bool isunname(char ch)		//���ڼ���������Ƿ��зǷ��ַ�
{
	char cc[]="\"*+,/:;<=>?[\\]| ";
	for (int i=0;i<16;i++)
		if (ch==cc[i])
			return true;
	return false;
}

/////////////////////////////////////////////////////////////////

bool IsName(char* Name)
{
	// �ж������Ƿ�������¹���
	// (1) ���ֳ��Ȳ��ܳ���FILENAME_LEN-1���ֽڣ���10���ֽڡ�
	//     ������������ֳ���10���ַ�����ֻ��ǰ10���ַ���Ч��
	// (2) ����һ������ĸ�����ִ�Сд�������֡��»��ߵ���ɣ����������Ǻ��֣�
	// (3) ���ֲ��ܰ�������16���ַ�֮һ��
	//		" * + , / : ; < = > ? [ \ ] | space(�ո�)
	// (4) ��������������ַ���.�����������������ֵĵ�һ���ַ����ʡ�.����
	//    ��.abc������..���͡�..abc���ȶ��ǲ��Ϸ������֡�

	int i,len,Len=FILENAME_LEN-1;
	bool yn=true;
	char ch;
	len=strlen(Name);
	if (len==0)
		return false;
	if (Name[0]=='.')		//���ֵ�һ���ַ��������ַ�'.'
		return false;
	if (len>Len)			//�����ֹ�������ȥ�����β��
	{
		Name[Len]='\0';
		len=Len;
	}
	for (i=0;i<len;i++)
	{
		ch=Name[i];
		if (isunname(ch))	//�������к��в��Ϸ�����
		{
			yn=false;
			break;
		}
	}
	if (!yn)
		cout<<"\n�����в��ܰ����ַ�'"<<ch<<"'��\n";
	return yn;
}

/////////////////////////////////////////////////////////////////

void PromptComd(void)			//prompt����
{
	dspath=!dspath;
}

/////////////////////////////////////////////////////////////////

void UdTabComd(void)			//udtab����
{
	//��ʾɾ���ļ��ָ���udtab������

	cout<<"\n�ָ���ɾ���ļ���Ϣ��(UdTab)�������£�\n\n";
	cout<<"�ļ�·����                      "<<"�ļ���        "
		<<"�׿��      "<<"�洢���"<<endl;
	for (int i=0;i<Udelp;i++)
		cout<<setiosflags(ios::left)<<setw(32)<<udtab[i].gpath
		<<setw(15)<<udtab[i].ufname<<setw(12)<<udtab[i].ufaddr
		<<setw(8)<<udtab[i].fb<<endl;
}

/////////////////////////////////////////////////////////////////

int file_to_buffer(FCB* fcbp,char* Buffer)	//�ļ����ݶ���Buffer,�����ļ�����
{
	//�ļ����ݶ���Buffer,�����ļ�����

	short s,len,i,j=0;

	len=fcbp->Fsize;				//ȡ�ļ�����
	s=fcbp->Addr;					//ȡ�ļ��׿��
	while (s>0)
	{
		for (i=0;i<SIZE;i++,j++)
		{
			if (j>=len)				//�Ѷ�����ļ�
				break;
			Buffer[j]=Disk[s][i];
		}
		s=FAT[s];					//ȡ��һ���̿�
	}
	Buffer[j]='\0';
	return len;						//�����ļ�����
}

/////////////////////////////////////////////////////////////////

int buffer_to_file(FCB* fcbp,char* Buffer)	//Bufferд���ļ�
{
	//�ɹ�д���ļ�������1��д�ļ�ʧ�ܣ�����0

	short bn1,bn2,i,j,s,s0,len,size,count=0;

	len=strlen(Buffer);	//ȡ�ַ���Buffer����
	s0=s=fcbp->Addr;		//ȡ�ļ��׿��
	if (len==0)
	{
		fcbp->Addr=fcbp->Fsize=0;	//�ļ���Ϊ���ļ�
		releaseblock(s);			//�ͷ��ļ�ռ�õĴ��̿ռ�
		return 1;
	}
	size=fcbp->Fsize;	//ȡ�ļ�����
	bn1=len/SIZE+(short)(len%SIZE>0);		//Buffer������ռ�õ��̿���
	bn2=size/SIZE+(short)(size%SIZE>0);	//�ļ�ԭ������ռ�õ��̿���
	if (FAT[0]<bn1-bn2)
	{
		cout<<"\n���̿ռ䲻�㣬���ܽ���Ϣд���ļ���\n";
		return 0;
	}
	if (s==0)				//���ǿ��ļ�
	{
		s0=s=getblock();	//Ϊ������׸��̿�
		fcbp->Addr=s0;		//�����׿��
	}
	j=0;
	while (j<len)		//Bufferд��FilName2
	{
		if (s<0)
		{
			s=getblock();
			FAT[s0]=s;
		}
		for (i=0;i<SIZE;i++,j++)
		{
			if (j==len)
				break;
			if (Buffer[j]=='\\' && Buffer[j+1]=='n')
			{
				Disk[s][i]='\n';
				j++;
				count++;
			}
			else
				Disk[s][i]=Buffer[j];
		}
		s0=s;
		s=FAT[s];
	}
	if (s>0)
	{
		FAT[s0]=-1;			//Ŀ���ļ������̿���
		releaseblock(s);	//��FileName2�����̿�δʹ�ã�Ӧ�ͷ�����
	}
	fcbp->Fsize=len-count;		//�ı��ļ��ĳ���
	return 1;
}

/////////////////////////////////////////////////////////////////

void releaseblock(short s)	//���մ��̿ռ�
{	//�ͷ�s��ʼ���̿���
	short s0;
	while (s>0)				//ѭ��������ֱ���̿���β��
	{
		s0=s;				//s0���µ�ǰ���
		s=FAT[s];			//sָ����һ���̿�
		FAT[s0]=0;			//�ͷ��̿�s0
		FAT[0]++;			//�����̿�����1
	}
}

/////////////////////////////////////////////////////////////////

int ParseCommand(char *p)	//������������зֽ������Ͳ�����
{
	int i,j,k,g=0;
	for (i=0;i<CK;i++)					//��ʼ��comd[][]
		comd[i][0]='\0';
	for (k=0;k<CK;k++)
	{	//�ֽ�����������comd[0]�������comd[1],comd[2]...�ǲ���
		for (i = 0; *p != '\0'; i++, p++)
			if (*p != ' '&&*p != '>')				//�ո����������֮��ķָ���
				comd[k][i] = *p;		//ȡ�����ʶ��
			else if(*p=='>'){
				if (strlen(comd[k]) != 0) {
					comd[k][i] = '\0';
					k++;

				}
				comd[k][0] = '>';
				comd[k][1] = '\0';
				p++;
				if (*p == '>') {
					comd[k][1] = '>';
					comd[k][2] = '\0';
					p++;
				}

				break;
			}else
			{
				comd[k][i]='\0';
				if (strlen(comd[k])==0) k--;
				p++;
				break;
			}
		if (*p=='\0')
		{
			comd[k][i]=*p;
			break;
		}
	}

	for (i=0;comd[0][i]!='\0';i++)
		if	(comd[0][i]=='/')		//����cd/��dir/usr�����
			break;
	if (comd[0][i]!='\0')			//comd[0]�д����ַ�'/'
	{
		if (k>0)
		for (j=k;j>0;j--)
			strcpy(comd[j+1],comd[j]);	//����
		strcpy(comd[1],&comd[0][i]);
		comd[0][i]='\0';
		k++;	//���һ������
	}

	for (i = 1; comd[0][i] != '\0'; i++)   //����cd..ָ�� ��4.2��ɣ�
		if (comd[0][i] == '.' && comd[0][i-1] == '.')
			break;
	if (comd[0][i] != '\0')
	{
		if (k > 0)
			for (j = k; j > 0; j--)
				strcpy(comd[j + 1], comd[j]);	//����
		strcpy(comd[1], &comd[0][i-1]);
		comd[0][i-1] = '\0';
		k++;	//���һ������
	}




	//----������-----
	//for (i = 0; i <= k; i++) {
	//	cout << comd[i] << " ";
	//}
	//cout << endl;
	//---------------

	return k;
}

/////////////////////////////////////////////////////////////////

int MoveComd(int k) {
	char *PathName1, *PathName2;
	FCB *p1,*p2;
	short t, tmp;
	if (k < 1 || k>2) {
		cout << "\n����̫���̫�٣�\n";
		return -1;
	}
	short s1 = ProcessPath(comd[1], PathName1, k, 0, '\040'); //�������ļ�������Ŀ¼
	short s2 = ProcessPath(comd[2], PathName2, k, 0, '\020'); //��������Ŀ¼
	if (s1 < 0) {
		return s1;
	}

	if (s2 < 0) {
		return s2;
	}
	short tmp1 = FindFCB(PathName1, s1, 0, p1); //�����ļ�,��һ��Ŀ¼
	short tmp2 = FindFCB(PathName2, s2, '\020', p2);
	short tmp3;
	if (tmp1 >= 0) {                //����һ���������ļ�ʱ������ɾ����copy����move
		if (tmp2 < 0) {
			cout << "\nĿ��Ŀ¼������\n";
			return -1;
		}
		tmp3= FindFCB(PathName1, tmp2, 0, p2);
		if (tmp3 >= 0) {
			cout << "\n�ļ��Ѵ��ڣ��Ƿ񸲸ǵ�ǰ�ļ���(y/n) ";
			char yn;
			cin >> yn;
			if (yn == 'Y' || yn == 'y') {
				p2->FileName[0] = (char)0xe5; //ɾ��Ŀ¼�
				tmp = tmp3;
				while (tmp > 0){		//���մ��̿ռ�
					t = tmp;
					tmp = FAT[tmp];
					FAT[t] = 0;
					FAT[0]++;
				}
			}
			else {
				return 0;
			}
		}
		else {
			strcpy(temppath, comd[2]);
			tmp3 = FindBlankFCB(tmp2, p2);
		}

		*p2 = *p1;						//Դ�ļ���Ŀ¼��Ƹ�Ŀ���ļ�
		strcpy(p2->FileName, PathName1);	//дĿ���ļ���
		short b0 = 0,b;
		while (tmp1 > 0)		//��ʼ�����ļ�����
		{
			b = getblock();
			if (b0 == 0)
				p2->Addr = b;		//Ŀ���ļ����׿��
			else
				FAT[b0] = b;
			memcpy(Disk[b], Disk[tmp1], SIZE);	//�����̿�
			tmp1 = FAT[tmp1];				//׼��������һ���̿�
			b0 = b;
		}


		p1->FileName[0]= (char)0xe5; //ɾ��Ŀ¼��
		tmp = tmp1;
		while (tmp > 0) {		//���մ��̿ռ�
			t = tmp;
			tmp = FAT[tmp];
			FAT[t] = 0;
			FAT[0]++;
		}
		cout <<comd[1]<<" >> "<<comd[2]<<PathName1<<" [OK]" << endl;
		return 1;
	}

	tmp1 = FindFCB(PathName1, s1, '\020', p1);
	tmp2 = FindFCB(PathName2, s2, '\040', p2);
	if (tmp1 < 0) {
		cout << "\nԴĿ¼������\n";
		return -1;
	}

	if (tmp2 < 0) {
		strcpy(p1->FileName, PathName2);
	}
	else {
		strcpy(temppath, comd[2]);
		tmp3 = FindBlankFCB(tmp2, p2);
		*p2 = *p1;
		p1->FileName[0] = (char)0xe5;
	}
	cout << comd[1] << " >> " << comd[2] << " [OK]" << endl;
	return 1;
}

/////////////////////////////////////////////////////////////////

int BatchComd(int k) {
	char *FileName;
	FCB *p;
	if (k < 1 || k> 2) {
		cout << "\n������������\n";
		return -1;
	}
	if (k == 1) {
		char *cmd;
		char filename[INPUT_LEN] = "";
		strcpy(filename, comd[1]);
		ifstream ff;
		ff.open(filename);
		if (!ff) {
			cout << "\n�޷����ļ�\n";
			return -1;
		}
		string str;
		while (getline(ff,str)){
			cout << "C:>" << str << endl;
			cmd = (char *)str.c_str();
			short n = ParseCommand(cmd);
			ExecComd(n);
			cout << endl;
		}
		ff.close();
		return 1;
	}
	else {
		if (strcmp("s", comd[2]) != 0) {
			cout << "\n���Դ���\n";
			return -1;
		}
		short s = ProcessPath(comd[1], FileName, k, 0, '\0');
		short tmp = FindFCB(FileName, s, 0, p);
		if (tmp < 0) {
			cout << "\n�Ҳ����ļ�\n";
			return -1;
		}
		//-------���ļ�
		short i, j, ii, i_uof, pos, offset;
		short b, b0, bnum, count = 0, readc;
		char attrib = '\0';
		char Buffer[SIZE + 1];
		strcat(temppath, FileName);
		i_uof = Check_UOF(temppath);			//��UOF
		if (i_uof == S)
		{
			cout << "\n�ļ�" << temppath << "δ�򿪻򲻴��ڣ����ܶ��ļ���\n";
			return -2;
		}
		if (uof[i_uof].readp == 0)
		{
			cout << "\n�ļ�" << temppath << "�ǿ��ļ���\n";
			return 1;
		}
		pos = uof[i_uof].readp;//�Ӷ�ָ����ָλ�ÿ�ʼ��
		if (pos > uof[i_uof].fsize)
		{
			cout << "\n��ָ����ָ���ļ�β�����޿ɶ���Ϣ��\n";
			return 1;
		}
		readc = uof[i_uof].fsize - pos + 1;	//�����ļ�β�������readc���ֽ�


		bnum = (pos - 1) / SIZE;		//���ļ��ĵ�bnum���(bnum��0��ʼ���)
		offset = (pos - 1) % SIZE;	//�ڵ�bnum���ƫ��λ��offset����ʼ��(offset��0��ʼ)
		b = uof[i_uof].faddr;		//ȡ�ļ��׿��
		for (i = 0; i < bnum; i++)	//Ѱ�Ҷ���ĵ�һ���̿��
		{
			b0 = b;
			b = FAT[b];
		}
		ii = offset;
		while (count < readc)		//���ļ���Buffer����ʾ֮
		{
			for (i = ii, j = 0; i < SIZE; i++, j++)
			{
				Buffer[j] = Disk[b][i];
				count++;
				if (count == readc)
				{
					j++;
					break;
				}
			}
			Buffer[j] = '\0';
			//cout << Buffer;

			char cmd[INPUT_LEN];
			short index = 0;
			for (int k = 0; k < j; k++) {
				cmd[index++] = Buffer[k];
				if (Buffer[k] == '\n') {
					cmd[index-1] = '\0';
					index = 0;
					cout << cmd << endl;
					short n = ParseCommand(cmd);
					ExecComd(n);
					cout << endl;
				}
			}
			cmd[index] = '\0';
			cout << cmd << endl;
			short n = ParseCommand(cmd);
			ExecComd(n);
			cout << endl;

			ii = 0;
			b = FAT[b];		//׼������һ���̿�
		}
	}
	return 1;
}

/////////////////////////////////////////////////////////////////

int FcComd(int k)			//fc����
{
	//������ʽ��fc <�ļ���1> <�ļ���2>
	//����ܣ�����ֽڱȽ�ָ���������ļ�������ͬ����ʾ���ļ�������ͬ��������
	//����ͬ����ʾ��һ����ͬ�ֽڵ�λ�ú͸��Ե����ݡ����ļ��������򱨴�
	if (k != 2)
	{
		cout << "\n���������������\n";
		return -1;
	}

	short i, s1, s2;
	char attrib = '\0', *FileName1, *FileName2;
	char gFileName1[PATH_LEN], gFileName2[PATH_LEN];	//����ļ�ȫ·����
	FCB* fcbp1;
	FCB* fcbp2;

	s1 = ProcessPath(comd[1], FileName1, k, 2, '\20');//ȡFileName����Ŀ¼���׿��
	if (s1 < 1)			//·������
		return s1;		//ʧ�ܣ�����
	s2 = ProcessPath(comd[2], FileName2, k, 2, '\20');
	if (s2 < 1)			//·������
		return s2;		//ʧ�ܣ�����

	s1 = FindFCB(FileName1, s1, attrib, fcbp1);		//ȡFileName���׿��(���������)
	strcpy(gFileName1, temppath);
	i = strlen(temppath);
	if (temppath[i - 1] != '/')
		strcat(gFileName1, "/");
	strcat(gFileName1, FileName1);//�����ļ�1��ȫ·����
	if (s1 < 0)
	{
		cout << "\n�ļ�C:" << gFileName1 << "�����ڡ�\n";
		return -2;
	}

	attrib = '\0';

	s2 = FindFCB(FileName2, s2, attrib, fcbp2);		//ȡFileName���׿��(���������)
	strcpy(gFileName2, temppath);
	i = strlen(temppath);
	if (temppath[i - 1] != '/')
		strcat(gFileName2, "/");
	strcat(gFileName2, FileName2);	//�����ļ�2��ȫ·����
	if (s2 < 0)
	{
		cout << "\n�ļ�C:" << gFileName2 << "�����ڡ�\n";
		return -2;
	}

	//���潫�����ļ����бȽϣ���������
	char *buf1 = new char[fcbp1->Fsize];
	char *buf2 = new char[fcbp2->Fsize];
	short index, len1, len2;
	bool flag = true;	//���ļ�������һ����ͬ,��flag=false
	len1 = file_to_buffer(fcbp1, buf1);
	len2 = file_to_buffer(fcbp2, buf2);
	if (fcbp1 == fcbp2) {
		cout << "�����ļ�C:" << gFileName1 << "�Լ����Լ��Ƚϡ�" << endl;
	}
	else if (len1 != len2) {
		cout << "\n�ļ�C:" << gFileName1 << "���ļ�C:" << gFileName2 << "�ĳ��Ȳ�ͬ��" << endl;
		cout << "�ļ�C:" << gFileName1 << "�ĳ���Ϊ��" << len1 << endl;
		cout << "�ļ�C:" << gFileName2 << "�ĳ���Ϊ��" << len2 << endl;
		for (index = 0; index < min(len1, len2); index++) {
			if (buf1[index] != buf2[index]) {
				cout << "\n�ļ�C:" << gFileName1 << "���ļ�C:" << gFileName2 << "��" << index + 1 << "�ֽڴ���ͬ��" << buf1[index] << "," << buf2[index] << endl;
				break;
			}
		}
	}
	else {
		for (index = 0; index < len1; index++) {
			if (buf1[index] != buf2[index]) {
				flag = false;
				break;
			}
		}
		if (flag) {
			cout << "\n�ļ�C:" << gFileName1 << "���ļ�C:" << gFileName2 << "��ͬ" << endl;
		}
		else {
			cout << "\n�ļ�C:" << gFileName1 << "���ļ�C:" << gFileName2 << "��" << index + 1 << "�ֽڴ���ͬ��" << buf1[index] << "," << buf2[index] << endl;
		}
	}
//	delete[]buf1;
//	delete[]buf2;
}
/////////////////////////////////////////////////////////////////

int ReplaceComd(int k) {
	//������ʽ��replace <�ļ���> <Ŀ¼��>
	//����ܣ��ԡ��ļ�����ָ�����ļ���ȡ����Ŀ¼����ָ��Ŀ¼�е�ͬ���ļ���
	//�밴����Ҫ�����replace����ĺ�����
		//��ָ���ļ���ȡ���ļ������ڣ��򱨴�
		//����ȡ������ֻ�����Ե��ļ�ʱ��Ӧѯ���û���
		//�������غ�ϵͳ���Ե��ļ����ܱ�ȡ����
		//��Ŀ¼������ȱʡ����ȡ����ǰĿ¼��ͬ���ļ���
		//�ļ���ָ�����ļ��ͱ�ȡ�����ļ���Ӧ����ͬһ���ļ����������Լ�ȡ���Լ���

	short i, s1, s2; //s2Ŀ���ļ�Ŀ¼���ַ
	short size, bnum, b, b0;
	char attrib = '\0', *FileName1, *FileName2;
	char gFileName1[PATH_LEN], gFileName2[PATH_LEN];	//����ļ�ȫ·����
	FCB* fcbp1;
	FCB* fcbp2;
	if (k < 1) {
		cout << "�ļ������ڡ�" << endl;
		return -2;
	}
	if (k == 1) { //Ŀ¼������ȱʡ  ȡ����ǰĿ¼��ͬ���ļ�
		s1 = ProcessPath(comd[1], FileName1, k, 1, '\20');
		if (s1 < 1)			//·������
			return s1;		//ʧ�ܣ�����
		s1 = FindFCB(FileName1, s1, attrib, fcbp1);		//ȡFileName���׿��(���������)
		strcpy(gFileName1, temppath);
		//cout <<"�׿��:"<<s1<<"		·����(�������ļ���)"<< gFileName1 <<"	"<<temppath<< endl;
		//cout <<"��ǰ·����"<< curpath.cpath <<"��ǰ·���׿��" <<curpath.fblock<< endl;
		//���湹������ļ�1��ȫ·����  usr/user/oo
		i = strlen(temppath);
		if (temppath[i - 1] != '/')
			strcat(gFileName1, "/");
		strcat(gFileName1, FileName1);//�����ļ�1��ȫ·����
		if (s1 < 0)
		{
			cout << "\n�ļ�" << FileName1 << "�����ڡ�\n";
			return -2;
		}

		//�ڵ�ǰĿ¼��Ѱ����FileName1ͬ�����ļ������滻���Ҹ��ļ��������¼���������
		//a����ͬ���ļ��������
		//b�������Լ�ȡ���Լ�
		//c����ͬ���ļ����ܴ���һ�����ڴ򿪵�״̬
		//d���������غ�ϵͳ���Ե��ļ����ܱ�ȡ��
		//e������ȡ�����ļ���ֻ�����Ե��ļ�ʱ��Ӧѯ���û�

		//Ѱ�ҵ�ǰĿ¼curpath����FileName1ͬ�����ļ�
		s2 = FindFCB(FileName1, curpath.fblock, attrib, fcbp2);
		strcpy(gFileName2, curpath.cpath);
		i = strlen(curpath.cpath);
		if (curpath.cpath[i - 1] != '/')
			strcat(gFileName2, "/");
		strcat(gFileName2, FileName1);	//����Ŀ���ļ���ȫ·����

		if (s2 < 0) {
			cout << "Ŀ¼��û�п�ȡ����ͬ���ļ���" << endl;
			return -2;
		}
		else {
			//�����Լ�ȡ���Լ�
			if (strcmp(gFileName2, gFileName1) == 0) {
				cout << "�ļ������Լ�ȡ���Լ���" << endl;
				return -2;
			}
			//�жϸ��ļ��Ƿ���һ�����ڴ򿪵�״̬
			i = Check_UOF(gFileName2);		//��UOF
			if (i < S)					//���ļ�����UOF��
			{
				cout << "\n�ļ�" << gFileName2 << "����ʹ�ã����ܱ�ȡ����\n";
				return -2;
			}


			//�������غ�ϵͳ���Ե��ļ����ܱ�ȡ��
			int Fattributes = fcbp2->Fattrib;
			if ((Fattributes >> 1) & 1 || (Fattributes >> 2) & 1) {
				cout << "������ϵͳ���Ե��ļ����ܱ�ȡ����" << endl;
				return -2;
			}
			//����ȡ�����ļ���ֻ�����Ե��ļ�ʱ��Ӧѯ���û�
			char yn;
			if (Fattributes & 1) {
				cout << "�ļ�" << gFileName2 << "��ֻ�������ļ����Ƿ���Ҫȡ������(y/n)" << endl;
				cin >> yn;
				if (yn == 'N' || yn == 'n') {
					return -2;				//��ȡ������
				}
			}

			//���濪ʼ�����ļ���ȡ�����ļ���copy
			size = fcbp1->Fsize;		//Դ�ļ��ĳ���
			bnum = size / SIZE + (short)(size % SIZE > 0);	//����Դ�ļ���ռ�̿���
			if (FAT[0] < bnum)
			{
				cout << "\n���̿ռ����������ܸ����ļ���\n";
				return -6;
			}
			*fcbp2 = *fcbp1;						//Դ�ļ���Ŀ¼��Ƹ�Ŀ���ļ�
			//strcpy(fcbp2->FileName, FileName2);	//дĿ���ļ���
			b0 = 0;
			while (s1 > 0)		//��ʼ�����ļ�����
			{
				b = getblock();
				if (b0 == 0)
					fcbp2->Addr = b;		//Ŀ���ļ����׿��
				else
					FAT[b0] = b;
				memcpy(Disk[b], Disk[s1], SIZE);	//�����̿�
				s1 = FAT[s1];				//׼��������һ���̿�
				b0 = b;
			}
			//�ļ����Ƴɹ�������
		}
	}
	else if (k == 2) { //replace <�ļ���> <Ŀ¼��>
		s1 = ProcessPath(comd[1], FileName1, k, 2, '\20');//ȡFileName����Ŀ¼���׿��
		if (s1 < 1)			//·������
			return s1;		//ʧ�ܣ�����
		/*
		s2 = ProcessPath(comd[2], FileName2, k, 2, '\20');
		if (s2 < 1)			//·������
			return s2;		//ʧ�ܣ�����
		*/
		s1 = FindFCB(FileName1, s1, attrib, fcbp1);		//ȡFileName���׿��(���������)
		strcpy(gFileName1, temppath);
		i = strlen(temppath);
		if (temppath[i - 1] != '/')
			strcat(gFileName1, "/");
		strcat(gFileName1, FileName1);//�����ļ�1��ȫ·����
		if (s1 < 0)
		{
			cout << "\n�ļ�" << FileName1 << "�����ڡ�\n";
			return -2;
		}
		attrib = '\040';
		s2 = FindPath(comd[2], attrib, 1, fcbp2);	//����Ŀ��·�����׿��
		if (s2 < 0) {//�����ڵ�ǰĿ��·���򷵻�
			cout << "Ŀ��·������" << endl;
			return -2;
		}
		s2 = FindFCB(FileName1, s2, attrib, fcbp2);		//Ѱ��<Ŀ¼��>����FileName1ͬ�����ļ�
		strcpy(gFileName2, temppath);
		/*
		i = strlen(temppath);
		if (temppath[i - 1] != '/')
			strcat(gFileName2, "/");
		strcat(gFileName2, FileName1);	//����Ŀ���ļ���ȫ·����
		*/
		if (s2 < 0)
		{
			cout << "Ŀ¼" << gFileName2 << "��û�п�ȡ����ͬ���ļ���";		//�����temppath������һ��Ŀ¼
			return -2;
		}
		else {

			i = strlen(temppath);
			if (temppath[i - 1] != '/')
				strcat(gFileName2, "/");
			strcat(gFileName2, FileName1);	//����Ŀ���ļ���ȫ·����

			//�����Լ�ȡ���Լ�
			if (strcmp(gFileName2, gFileName1) == 0) {
				cout << "�ļ������Լ�ȡ���Լ���" << endl;
				return -2;
			}
			//�жϸ��ļ��Ƿ���һ�����ڴ򿪵�״̬
			i = Check_UOF(gFileName2);		//��UOF
			if (i < S)					//���ļ�����UOF��
			{
				cout << "\n�ļ�" << gFileName2 << "����ʹ�ã����ܱ�ȡ����\n";
				return -2;
			}


			//�������غ�ϵͳ���Ե��ļ����ܱ�ȡ��
			int Fattributes = fcbp2->Fattrib;
			if ((Fattributes >> 1) & 1 || (Fattributes >> 2) & 1) {
				cout << "������ϵͳ���Ե��ļ����ܱ�ȡ����" << endl;
				return -2;
			}
			//����ȡ�����ļ���ֻ�����Ե��ļ�ʱ��Ӧѯ���û�
			char yn;
			if (Fattributes & 1) {
				cout << "�ļ�" << gFileName2 << "��ֻ�������ļ����Ƿ���Ҫȡ������(y/n)" << endl;
				cin >> yn;
				if (yn == 'N' || yn == 'n') {
					return -2;				//��ȡ������
				}
			}

			//���濪ʼ�����ļ���ȡ�����ļ���copy
			size = fcbp1->Fsize;		//Դ�ļ��ĳ���
			bnum = size / SIZE + (short)(size % SIZE > 0);	//����Դ�ļ���ռ�̿���
			if (FAT[0] < bnum)
			{
				cout << "\n���̿ռ����������ܸ����ļ���\n";
				return -6;
			}
			*fcbp2 = *fcbp1;						//Դ�ļ���Ŀ¼��Ƹ�Ŀ���ļ�
			//strcpy(fcbp2->FileName, FileName2);	//дĿ���ļ���
			b0 = 0;
			while (s1 > 0)		//��ʼ�����ļ�����
			{
				b = getblock();
				if (b0 == 0)
					fcbp2->Addr = b;		//Ŀ���ļ����׿��
				else
					FAT[b0] = b;
				memcpy(Disk[b], Disk[s1], SIZE);	//�����̿�
				s1 = FAT[s1];				//׼��������һ���̿�
				b0 = b;
			}
			//�ļ����Ƴɹ�������
		}
	}
	else {
		cout << "�����в���̫�ࡣ" << endl;
		return -2;
	}
}

/////////////////////////////////////////////////////////////////