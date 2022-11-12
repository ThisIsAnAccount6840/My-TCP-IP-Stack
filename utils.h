#include<iostream>
#include<memory.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>

using namespace std;


/*void apply_mask(char *prefix, int mask, char *str_prefix)
{
	int i, length=sizeof(*prefix)/sizeof(char),max;
	if(mask>length) 
	{
		cout<<"WRONG MASK INPUT!"<<endl;
		return;
	}

	//memset(str_prefix,0,sizeof(*str_prefix));

	for(i=0,max=mask;i<max;i++)
	{
		if(*prefix[i]=='.') max++;
		*str_prefix[i]=*prefix[i];
	}

}



void layer2_fill_with_broadcast_mac(char *mac_array)
{
	char broadcast[]="FF:FF:FF:FF:FF:FF";
	strcpy(mac_array,broadcast);
}
*/

void apply_mask(const char*, int, char*);
void layer2_fill_with_broadcast_mac(char*);

#define IS_MAC_BROADCAST_ADDR(mac) (mac[0]==0xFF && mac[1]==0xFF && mac[2]==0xFF && mac[3]==0xFF && mac[4]==0xFF && mac[5]==0xFF)

