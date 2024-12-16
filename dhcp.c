#include <math.h> /* pow */
#include <stdlib.h> /* malloc free size_t */
#include <assert.h> /* assert */
#include <string.h> /* memcpy memcmp */

#include "dhcp.h"
#include "trie.h"

#define BITS_IN_BYTE 8
#define BASE 2
#define BYTE_MASK 0x000000ff

struct DHCP_Server
{
	trie_t *trie;
	unsigned char base_network_id[BYTES_IN_IP];
	size_t depth;
};

static unsigned int BuildMaskForHost(size_t num_of_bits);
static unsigned int ConvertToUnsignedInt(unsigned char ip_address[BYTES_IN_IP]);
static status_e InsertPreDefined(DHCP_Server_t *dhcp);
static int CheckMatch(unsigned char ip_address[BYTES_IN_IP], DHCP_Server_t *dhcp);
static void SetOutParam(unsigned char out_param[BYTES_IN_IP], host_id_t id);

static unsigned char pre_defined_ips[][BYTES_IN_IP] = { {0,0,0,0}, 
														{~0,~0,~0,~0}, 
														{~0,~0,~0,~0 - 1} };

/*************************************API functions ***************************/

DHCP_Server_t *DHCPCreate(unsigned char base_network_id[BYTES_IN_IP], 
												size_t subnet_prefix)
{
	DHCP_Server_t *dhcp = NULL;
	size_t depth = BYTES_IN_IP * BITS_IN_BYTE - subnet_prefix;
	status_e status = 0;
	
	assert(subnet_prefix < BYTES_IN_IP * BITS_IN_BYTE - 1 && subnet_prefix > 1);
	
	dhcp = (DHCP_Server_t *)malloc(sizeof(struct DHCP_Server));
	if(NULL == dhcp)
	{
		return NULL;
	}
	
	dhcp->trie = TrieCreate(depth);
	if(NULL == dhcp->trie)
	{
		free(dhcp);
		return NULL;
	}
	
	status = InsertPreDefined(dhcp);
	if(MEMORY_FALIURE == status)
	{
		return NULL;
	}
	
	memcpy(dhcp->base_network_id, base_network_id, BYTES_IN_IP);
	dhcp->depth = depth;
	
	return dhcp;
}


void DHCPDestroy(DHCP_Server_t *dhcp)
{
	if(NULL != dhcp)
	{
		TrieDestroy(dhcp->trie);
		free(dhcp);
	}
}

size_t DHCPCountFree(const DHCP_Server_t *dhcp)
{
	size_t max_addresses = 0;
	
	assert(NULL != dhcp);
	
	max_addresses = pow(BASE, dhcp->depth);
	
	return (max_addresses - TrieCountIps(dhcp->trie));
}

status_e DHCPAllocateIP(DHCP_Server_t *dhcp,
                        unsigned char requested_ip[BYTES_IN_IP],
                        unsigned char received_ip[BYTES_IN_IP])
{
	host_id_t host_id = 0;
	trie_status_e trie_status = TRIE_SUCCESS;
	unsigned char null_handler[BYTES_IN_IP] = {0};
	
	assert(NULL != dhcp);
	assert(NULL != received_ip);
	
	if(NULL == requested_ip || !CheckMatch(requested_ip, dhcp))
	{
		requested_ip = null_handler;
	}
	
	host_id = ConvertToUnsignedInt(requested_ip);
	trie_status = TrieInsert(dhcp->trie, host_id, &host_id);
	if(trie_status == TRIE_FULL)
	{
		return DHCP_FULL;
	}
	
	if(trie_status == TRIE_MEMORY_FALIURE)
	{
		return MEMORY_FALIURE;
	}
	
	SetOutParam(received_ip, host_id);
	
	return SUCCESS;
}

status_e DHCPFreeIP(DHCP_Server_t *dhcp, unsigned char address[BYTES_IN_IP])
{
	host_id_t host_id = 0;
	trie_status_e trie_status = TRIE_SUCCESS;
	
	assert(NULL != dhcp);
	assert(NULL != address);
	
	if(1 != CheckMatch(address, dhcp))
	{
		return BAD_SUBNET;
	}
	
	host_id = ConvertToUnsignedInt(address);
	trie_status = TrieRemove(dhcp->trie, host_id);
	if(trie_status == TRIE_DOUBLE_FREE)
	{
		return DOUBLE_FREE;
	}
	
	return SUCCESS;
}

/*********************************** helpers *********************************/
static status_e InsertPreDefined(DHCP_Server_t *dhcp)
{
	size_t i = 0;
	size_t count = 0;
	host_id_t host_id = 0;
	host_id_t get_element = 0;
	trie_status_e status = TRIE_SUCCESS;
	
	while(i < sizeof(pre_defined_ips)/ sizeof(pre_defined_ips[0]))
	{
		host_id = ConvertToUnsignedInt(pre_defined_ips[i]);
		get_element = host_id;
		status = TrieInsert(dhcp->trie, host_id, &get_element);
		count = TrieCountIps(dhcp->trie);
		if(i + 1 != count || TRIE_SUCCESS != status || host_id != get_element)
		{
			TrieDestroy(dhcp->trie);
			free(dhcp);
			
			return MEMORY_FALIURE;
		}
		++i;
	}
	
	return SUCCESS;
}

static void SetOutParam(unsigned char out_param[BYTES_IN_IP], host_id_t id)
{
	host_id_t mask = BYTE_MASK;
	int i = BYTES_IN_IP - 1;
	
	for(; i >= 0; --i)
	{
		out_param[i] = id & mask;
		id >>= BITS_IN_BYTE;
	}
}

static int CheckMatch(unsigned char ip_address[BYTES_IN_IP], DHCP_Server_t *dhcp)
{
	unsigned int requested_subnet_id = ConvertToUnsignedInt(ip_address);
	unsigned int subnet_id = ConvertToUnsignedInt(dhcp->base_network_id);
	
	unsigned int host_mask = ~(BuildMaskForHost(dhcp->depth));
	
	requested_subnet_id &= host_mask;
	subnet_id &= host_mask;
	
	return (0 == memcmp(&requested_subnet_id, &subnet_id, BYTES_IN_IP));
}

static unsigned int ConvertToUnsignedInt(unsigned char ip_address[BYTES_IN_IP])
{
	return ((ip_address[0] << 24) + 
			(ip_address[1] << 16) + 
			(ip_address[2] << 8) + 
			ip_address[3]);	
}

static unsigned int BuildMaskForHost(size_t num_of_bits)
{
	unsigned int host_mask = pow(BASE, num_of_bits) - 1;
	
	return host_mask;
}
