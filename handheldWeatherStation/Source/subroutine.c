#include "subroutine.h"
/**
	FOR ROUTINES USED IN MORE THAN ONE FILE
*/


//converts a char number value into separate ASCII
char* convert(unsigned char value)
{
	char i;
	//support max of size char + NULL terminator
	static unsigned char array[4];	//3
	
	for(i = 0; i < 4; i++)
	{
		//puts value in array after being split by 1 character and converted to ascii.
		array[i] = value % 10 + '0';
		
		//checks if it's the last digit to process && ensures that it won't go out of bounds
		if(value / 10 == 0)// && i < 3)
		{
			//if the number of digits uses is less than 3 e.g. 24, 1
			//set a terminator value in the next position
			array[i+1] = '\0';
			break;
		}
		value /= 10;
	}
	
	//array is currently back to front, reverse to fix
	reverse_array(array, i);
	
	return array;
}

void reverse_array(unsigned char *array, unsigned char end)
{
	unsigned char start = 0, temp;
	
	while(start < end)
	{
		temp = array[start];
		array[start] = array[end];
		array[end] = temp;
		start++;
		end--;
	}
}
