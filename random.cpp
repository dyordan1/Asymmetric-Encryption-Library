#include <conio.h>
#include "mpuint.h"
#include "random.h"

static int RandomKey(void)
{
  int n = 0;
  while (_kbhit() == 0)
    n++;
  int c = _getch();
  if (c == 0)
    c = _getch();
  _putch (' ' <= c && c <= '~' ? c : ' ');
  return c + n & 0xFF;
}

void Random(mpuint &x)
{
  printf("Please type %d random characters\r\n", x.length);
  while (_kbhit() != 0)
    RandomKey();
  for (unsigned i = 0; i < x.length; i++)
    x.value[i] = RandomKey();
  printf("\r\nThank you\r\n");
}