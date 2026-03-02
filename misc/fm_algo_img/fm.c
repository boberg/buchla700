#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fm.h"



int pd[RGT_EDGE*BOT_EDGE];

void vputp(int octad,int xloc,int yloc,int val)
{
    pd[(yloc*RgtEdge)+xloc] = val;
}

void writeASCII()
{
  int i;

  for (i = 0; i < RgtEdge*BotEdge;i++)
  {
    if (i % RgtEdge == 0) {
      printf("\n");
      
    }
    if (pd[i])
      printf("%x",pd[i]);
    else
      printf(" ");
  }
  printf("\n");
}

void writeXBM(int n)
{
  int i,j=0;
  FILE *f;
  char fname[32];
  unsigned p = (unsigned)0;

  sprintf(fname,"./BW/FM-%d.xbm",n);

  f = fopen(fname,"w+");

  fprintf(f,"#define test_width %d\n#define test_height %d\nstatic char test_bits[] = {\n",RgtEdge,BotEdge);

  for (i = 0; i < RgtEdge*BotEdge;i++)
  {
    if (i % (8*16) == 0) {
      fprintf(f,"\n");
    }

    if (pd[i] > 0) {
      p |= (1 << (i % 8));
    }
    if (i % 8 == 7) {
      fprintf(f, "0x%02X, ",p);
      p = (unsigned)0;
    }
    
  }
  fprintf(f,"\n");

  fprintf(f,"};\n");
  fclose(f);
}

#define XPMPALSIZE 240

void genXPMpal(short pal[16][3], char *out)
{
  int i,j=0;

  for (i = 0; i < 16; i++) {
    sprintf(out,"\"%x c #%02x%02x%02x\",\n",i,pal[i][0]*85,pal[i][1]*85,pal[i][2]*85);
    out+=15;
  }
}

void writeXPM1(int n)
{
  int i,j=0;
  FILE *f;
  char fname[32];
  char xpal[XPMPALSIZE + 1];

  sprintf(fname,"./COL/FM-%d.xpm",n);

  f = fopen(fname,"w+");
  fprintf(f,"/* XPM */\nstatic char *img[] = {\n/* columns rows colors chars-per-pixel */\n\"%d %d 16 1\",\n",RgtEdge,BotEdge);

  memset(xpal,0,sizeof xpal);
  genXPMpal(inspal,xpal);
  fprintf(f,xpal);

  for (i = 0; i < RgtEdge*BotEdge;i++)
    {
      if (i % RgtEdge == 0) {
	fprintf(f,"\"");
      }
      
      fprintf(f, "%x",pd[i]);
      
      if (i % RgtEdge == (RgtEdge - 1)) {
	fprintf(f, (i == (RgtEdge*BotEdge)-1?"\"\n":"\",\n"));
      }
    }
  fprintf(f,"};\n");

  fclose(f);
}

void putpat(int pn, int px, int py, int patc)
{
  register short xp, yp, pr, pc;
  register unsigned pw;

  //x
  int idoct=0;
  
  //


  //if (v_regs[5] & 0x0180)
  //  vbank(0);

  for (pr = 0; pr < 16; pr++) {

    pw = pat[pn - 1][pr];
    yp = py - 8 + pr + TopEdge;

    if ((yp >= TopEdge) && (yp <= BotEdge)) {

      for (pc = 0; pc < 16; pc++) {

	xp = px - 8 + pc + LftEdge;

	if ((xp >= LftEdge) && (xp <= RgtEdge) && (pw & (1 << (15 - pc)))) {
	  vputp(idoct, xp, yp, patc);
	}
      }
    }
  }



}


void draw(int nn) 
{
  int i, np;
  char *cfgdat, *cfp;

  memset(pd,0,sizeof(pd));

  cfgdat = cfgptr[nn];

  for (i = 0; i < 64; i++) {

    cfp = cfgdat + (i << 1) + i;
    np = *cfp;

    if (np)
      putpat(np, *(cfp + 1), *(cfp + 2), patctab[np - 1]);
  }
}


int main(int argc, char** argv) {

  if (argc == 2)
  {
    int x;
    x = atoi(argv[1]);
    if (x >= 0 && x < MAXCFG) {
      draw(x);
      writeASCII();
    }
  }
  else
  {
    int i;
    for (i = 0; i < MAXCFG; i++) {
      draw(i);
      writeXPM1(i);
      writeXBM(i);
    }
  }
}
