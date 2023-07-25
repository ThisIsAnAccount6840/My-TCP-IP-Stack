#include"utils.h"
#include"graph.h"

void apply_mask(const char* prefix, int mask, char*str_prefix)
{
	uint32_t b_prefix=0;
	uint32_t subnet_mask=0xFFFFFFFF;

	//mask in binary format
	subnet_mask=subnet_mask<<(32-mask);

	// IP address in binary form
	inet_pton(AF_INET,prefix,&b_prefix);
	b_prefix=htonl(b_prefix);

	//subnet in binary form--apply mask on IP address
	b_prefix=b_prefix & subnet_mask;

	//convert binary form IP address into dot decimal form
	b_prefix=htonl(b_prefix);
	inet_ntop(AF_INET,&b_prefix, str_prefix,16);
	str_prefix[15]='\0';
}



void layer2_fill_with_broadcast_mac(char* mac_array)
{
	int i;
	for(i=0;i<6;i++)
	{
		mac_array[i]=0xFF;
	}
}

/*unsigned int covert_ip_from_str_to_int(char*ip_addr)
{
	int ip_int[4];
	char buffer[4];
	int i,j=3,k,m=3;
	for(i=0,k=0;i<16&&ip_addr[i]!='\0';i++)
	{
		if(ip_addr[i]!='.')
		{
			buffer[k]=ip_addr[i];
			k++;
		}
		else
		{
			buffer[k]='\0';
			k=0;

			ip_int[m]=atoi(buffer);
			m--;
		}
			
	}
	unsigned int result=(ip_int[3]<<24)+(ip_int[2]<<16)+(ip_int[1]<<8)+ip_int[0];
	return result;
} */
