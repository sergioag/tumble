/*
 * tumble: build a PDF file from image files
 *
 * bitblt table generator
 * $Id: bitblt_table_gen.c,v 1.8 2003/08/18 01:59:41 eric Exp $
 * Copyright 2003 Eric Smith <eric@brouhaha.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.  Note that permission is
 * not granted to redistribute this program under the terms of any
 * other version of the General Public License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111 USA
 */


#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void gen_bit_reverse_table (bool header)
{
  int i, j;

  if (header)
    printf ("extern ");
  printf ("const uint8_t bit_reverse_byte [0x100]");
  if (header)
    {
      printf (";\n");
      return;
    }
  printf (" =\n");
  printf ("{\n");
  for (i = 0; i < 0x100; i++)
    {
      if ((i & 7) == 0)
	printf ("  ");
      j = (((i & 0x01) << 7) |
	   ((i & 0x02) << 5) |
	   ((i & 0x04) << 3) |
	   ((i & 0x08) << 1) |
	   ((i & 0x10) >> 1) |
	   ((i & 0x20) >> 3) |
	   ((i & 0x40) >> 5) |
	   ((i & 0x80) >> 7));
      printf ("0x%02x", j);
      if (i != 0xff)
	printf (",");
      if ((i & 7) == 7)
	printf ("\n");
      else
	printf (" ");
    }
  printf ("};\n");
}


int count_run (int byte, int start_bit, int desired_val)
{
  int count = 0;
  int i;

#ifdef WORDS_BIGENDIAN
  for (i = 7 - start_bit; i >= 0; i--)
    {
      int bit = (byte >> i) & 1;
      if (bit == desired_val)
	count++;
      else
	break;
    }
#else
  for (i = start_bit; i < 8; i++)
    {
      int bit = (byte >> i) & 1;
      if (bit == desired_val)
	count++;
      else
	break;
    }
#endif

  return (count);
}


void gen_run_length_table (bool header, int val, char *name)
{
  int i, j;

  if (header)
    printf ("extern ");
  printf ("const uint8_t %s [8][256]", name);
  if (header)
    {
      printf (";\n");
      return;
    }
  printf (" =\n");
  printf ("{\n");
  for (i = 0; i < 8; i++)
    {
      printf ("  {\n");
      for (j = 0; j < 256; j++)
	{
	  if ((j & 15) == 0)
	    printf ("  ");
	  printf ("%d", count_run (j, i, val));
	  if (j != 0xff)
	    printf (",");
	  if ((j & 15) == 15)
	    printf ("\n");
	  else
	    printf (" ");
	}
      printf ("  }");
      if (i != 7)
	printf (",");
      printf ("\n");
    }
  printf ("};\n");
}


int main (int argc, char *argv[])
{
  bool header;

  if (argc != 2)
    {
      fprintf (stderr, "wrong arg count\n");
      exit (2);
    }
  if (strcmp (argv [1], "-h") == 0)
    header = 1;
  else if (strcmp (argv [1], "-c") == 0)
    header = 0;
  else
    {
      fprintf (stderr, "wrong args\n");
      exit (2);
    }

  printf ("/* This file is automatically generated; do not edit */\n");
  printf ("\n");

  if (! header)
    {
      printf ("#include <stdint.h>\n");
      printf ("#include \"bitblt_tables.h\"\n");
      printf ("\n");
    }

  gen_bit_reverse_table (header);
  printf ("\n");

  gen_run_length_table (header, 0, "rle_tab");
  printf ("\n");

  return (0);
}
