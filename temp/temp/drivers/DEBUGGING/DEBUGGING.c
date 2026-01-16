#include "debugging.h"
#include "main.h"

int _write(int file, char *ptr, int len)
{
	 int DataIdx;
	 for (DataIdx = 0; DataIdx < len; DataIdx++)
	 {
		 ITM_SendChar(*ptr++);
	 }
	 return len;
}
