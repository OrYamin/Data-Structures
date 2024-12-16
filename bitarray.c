/*reviewer: Shahar Marom
Was written by: Or Yamin on 01/05/2024 */
#include "bitarray.h"

static void reverse(char str[], int length);
char BitsSetTable256[256];


bitarray_t SetAll(bitarray_t data)
{
	(void)data;
	return -1;
}


bitarray_t ResetAll(bitarray_t data)
{
	data = 0;
	return data;
}


bitarray_t SetBitOn(bitarray_t data, size_t index)
{
	return (data | (bitarray_t)1 << index);
}


bitarray_t SetBitOff(bitarray_t data, size_t index)
{
	size_t mask = ~((bitarray_t)1<<(index));
	return data&mask;
}


bitarray_t SetBit(bitarray_t data, size_t index, int bool_value)
{
	return bool_value == 1 ? SetBitOn(data, index) : SetBitOff(data, index);
}
	
	
int GetVal(bitarray_t data, size_t index)
{
	return (data&((bitarray_t)1<<(index)))>>(index);
}


bitarray_t FlipBit(bitarray_t data, size_t index)
{
	size_t mask = 1;
	return data^(mask<<(index));
}
	
	
bitarray_t FlipAll(bitarray_t data)
{
	return ~data;
}
	

bitarray_t Mirror(bitarray_t data)
{
	data = ((data & 0x5555555555555555) << 1) | ((data & 0xAAAAAAAAAAAAAAAA) >> 1); 
	data = ((data & 0x3333333333333333) << 2) | ((data & 0xCCCCCCCCCCCCCCCC) >> 2); 
	data = ((data & 0x0F0F0F0F0F0F0F0F) << 4) | ((data & 0xF0F0F0F0F0F0F0F0) >> 4); 
	data = ((data & 0x00FF00FF00FF00FF) << 8) | ((data & 0xFF00FF00FF00FF00) >> 8); 
	data = ((data & 0x0000FFFF0000FFFF) << 16) | ((data & 0xFFFF0000FFFF0000) >> 16);
	data = ((data & 0x00000000FFFFFFFF) << 32) | ((data & 0xFFFFFFFF00000000) >> 32);
	return data;
}
	
	
bitarray_t RotateRight(bitarray_t data, size_t rotate_value)
{
	size_t temp = 0;
	size_t mask = 0xffffffffffffffff;
	mask = ~(mask << rotate_value);
	temp = (data&mask);
	temp = temp<<(64-rotate_value);
	data = data>>rotate_value;
	data = data|temp;
	return data;
}
	
	
bitarray_t RotateLeft(bitarray_t data, size_t rotate_value)
{
	size_t temp = 0;
	size_t mask = 0xffffffffffffffff;
	mask = ~(mask >> rotate_value);
	temp = (data&mask);
	temp = temp>>(64-rotate_value);
	data = data<<rotate_value;
	data = data|temp;
	return data;
}	


size_t CountOn(bitarray_t data)
{
	size_t count = 0;
	while(data)
	{
		data&= (data-1);
		count ++;
	}
	return count;
}

size_t CountOn2(bitarray_t data)
{
	data = (data & 0x5555555555555555 ) + ((data >>  1) & 0x5555555555555555 ); /*put count of each  2 bits into those  2 bits */
	data = (data & 0x3333333333333333 ) + ((data >>  2) & 0x3333333333333333 ); /*put count of each  4 bits into those  4 bits */
	data = (data & 0x0f0f0f0f0f0f0f0f ) + ((data >>  4) & 0x0f0f0f0f0f0f0f0f ); /*put count of each  8 bits into those  8 bits */
	data = (data & 0x00ff00ff00ff00ff ) + ((data >>  8) & 0x00ff00ff00ff00ff ); /*put count of each 16 bits into those 16 bits */
	data = (data & 0x0000ffff0000ffff) + ((data >> 16) & 0x0000ffff0000ffff); /*put count of each 32 bits into those 32 bits */
	data = (data & 0x00000000ffffffff) + ((data >> 32) & 0x00000000ffffffff); /*put count of each 64 bits into those 64 bits */
	return data;
}
	
	
size_t CountOff(bitarray_t data)
{
	return 64-CountOn(data);
}
	
	
char *ToString(bitarray_t data, char *buffer)
{
		
	size_t i = 0;
	int isNegative = 0;


	if (data == 0) 
	{
		buffer[i++] = '0';
		buffer[i] = '\0';
		return buffer;
	}


	while (i < 64) 
	{
		buffer[i++] = (data&1) + '0';
		data = data >>1;
	}
	
	if (isNegative)
	{
		buffer[i++] = '-';
	}
	
	buffer[i] = '\0';

	reverse(buffer, i);

	return buffer;
}	
	
void initialize() 
{ 
	int i = 0;
	BitsSetTable256[0] = 0; 
	for ( i = 0; i < 256; i++)
	{ 
		BitsSetTable256[i] = (i & 1) + 
		BitsSetTable256[i / 2]; 
	} 
} 	
	
	
static void reverse(char str[], int length)
{
	int start = 0;
	int end = length - 1;
	while (start < end) 
	{
		char temp = str[start];
		str[start] = str[end];
		str[end] = temp;
		end--;
		start++;
	}
}
	
size_t CountOnLUT(bitarray_t data)
{
	return (BitsSetTable256[data & 0xff] + 
			BitsSetTable256[(data >> 8) & 0xff] + 
			BitsSetTable256[(data >> 16) & 0xff] + 
			BitsSetTable256[(data >> 24) & 0xff] +
			BitsSetTable256[(data >> 32) & 0xff] + 
			BitsSetTable256[(data >> 40) & 0xff] + 
			BitsSetTable256[(data >> 48) & 0xff] + 
			BitsSetTable256[data >> 56]); 
} 



static char mirror_look_up[16] = {0x0, 0x8, 0x4, 0xC, 0x2, 0xA, 0x6, 0xE, 
					   		  	  0x1, 0x9, 0x5, 0xD, 0x3, 0xB, 0x7, 0xF};
					   		  	  
bitarray_t MirrorLUT(bitarray_t data)
{
	bitarray_t result = 0;
	size_t i = 0;
	
	for(; i < 16; ++i)
	{
		result <<= 4;
		result |= mirror_look_up[data & 0xF];
		data >>= 4;
	}
	
	return result;
}


	
