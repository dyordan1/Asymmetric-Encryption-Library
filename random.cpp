#include <conio.h>
#include "mpuint.h"
#include "random.h"

namespace AsymmEL
{

static int RandomKey(void)
{
  int n = 0;
  while (_kbhit() == 0)
    ++n;
  int c = _getch();
  if (c == 0)
    c = _getch();
  _putch (' ' <= c && c <= '~' ? c : ' ');
  return c + n & 0xFF;
}

void PseudoRandom(mpuint &x)
{
  for (unsigned i = 0; i < x.length; ++i)
  {
	unsigned short bytes_left = (BITS_IN_CHUNK/8);
	  CHUNK_DATA_TYPE chunk = rand();
	  while(--bytes_left)
	  {
		  chunk <<= 8;
		  chunk |= rand();
	  }
	x.value[i] = chunk;
  }
}

void Random(mpuint &x)
{
  printf("Please type %d random characters\r\n", x.length*(BITS_IN_CHUNK/8));
  while (_kbhit() != 0)
    RandomKey();
  for (unsigned i = 0; i < x.length; ++i)
  {
	unsigned short bytes_left = (BITS_IN_CHUNK/8);
	  CHUNK_DATA_TYPE chunk = RandomKey();
	  while(--bytes_left)
	  {
		  chunk <<= 8;
		  chunk |= RandomKey();
	  }
	x.value[i] = chunk;
  }
  printf("\r\nThank you\r\n");
}

//end namespace
}
