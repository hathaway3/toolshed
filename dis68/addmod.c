/* addressing mode routines for 68k disassembler */

#include "dis68.h"
#include <stdio.h>
#include <string.h>

static void size(int);
static int acdata(int, int);
static int acmem(int, int);
static int accon(int, int);
static int acalt(int, int);
static int acall(int, int);

void sizen() { size(SIZE(*opcode)); }

static void size(int sz) {
  switch (addsz = sz) {
  case 0:
    strmne(".b ");
    break;

  case 1:
    strmne(".w ");
    break;

  case 2:
    strmne(".l ");
    break;

  default:
    strmne(".? ");
  }
}

void isize(int sz) {
  dat = getdat(1);
  switch (addsz = sz) {
  case 0:
    snprintf(str, sizeof(str), ".b #$%02x", dat & 0xff);
    break;

  case 1:
    snprintf(str, sizeof(str), ".w #$%04x", dat);
    break;

  case 2:
    dat1 = getdat(1);
    snprintf(str, sizeof(str), ".l #$%04x%04x", dat, dat1);
    break;

  default:
    snprintf(str, sizeof(str), ".? #$%04x", dat);
    break;
  }

  strmne(str);
}

/*
 * effective address handlers
 */

void effaddn() { effadd(SMOD(*opcode), SREG(*opcode)); }

/* reg is 3 bits only */
void effadd(int mode, int reg) { (*modetbl[mode])(reg); }

/* reg is 3 bits only */
void drd(int reg) {
  snprintf(str, sizeof(str), "d%1d", reg);
  strmne(str);
}

/* reg is 3 bits only */
void ard(int reg) {
  snprintf(str, sizeof(str), "a%1d", reg);
  strmne(str);
}

/* reg is 3 bits only */
void ri(int reg) {
  snprintf(str, sizeof(str), "(a%1d)", reg);
  strmne(str);
}

/* reg is 3 bits only */
void piri(int reg) {
  snprintf(str, sizeof(str), "(a%1d)+", reg);
  strmne(str);
}

/* reg is 3 bits only */
void pdri(int reg) {
  snprintf(str, sizeof(str), "-(a%1d)", reg);
  strmne(str);
}

/* reg is 3 bits only */
void rio(int reg) {
  dat = getdat(1);
  snprintf(str, sizeof(str), "a%1d%04x(a%1d)", reg, dat, reg);
  add_reg_sym(reg, dat);
  strmne(str);
}

/* reg is 3 bits only */
void irio(int reg) {
  dat = getdat(1);
  if (dat & 0x8000)
    snprintf(str, sizeof(str), "a%1d%04x(a%1d,a%1d", reg, dat & 0xff, reg,
             (dat & 0x7000) >> 12);
  else
    snprintf(str, sizeof(str), "a%1d%04x(a%1d,d%1d", reg, dat & 0xff, reg,
             (dat & 0x7000) >> 12);

  add_reg_sym(reg, dat);

  if (dat & 0x0800)
    strncat(str, ".l)", sizeof(str) - strlen(str) - 1);
  else
    strncat(str, ".w)", sizeof(str) - strlen(str) - 1);

  strmne(str);
}

/*
 * special address mode handlers
 */

/* reg is 3 bits only */
void spadd(int reg) { (*spaddtbl[reg])(); }

void as() {
  pcl = getdat(1);
  snprintf(str, sizeof(str), "z%06x.w", pcl & 0xffffff);
  add_pc_sym(pcl);
  strmne(str);
}

void al() {
  pcl = getldat(1);
  snprintf(str, sizeof(str), "z%06x", pcl & 0xffffff);
  add_pc_sym(pcl);
  strmne(str);
}

void relo() {
  short pcw;

  pcw = getdat(1);
  pcl = ppc + pcw + opwcnt * 2 - 2;
  snprintf(str, sizeof(str), "z%06x(pc)", pcl & 0xffffff);
  add_pc_sym(pcl);
  strmne(str);
}

void relio() {
  char pcb;

  pcl = pcb = getdat(1);
  pcl = ppc + opwcnt * 2 - 2 + pcl;
  add_pc_sym(pcl);
  if (dat & 0x8000)
    snprintf(str, sizeof(str), "z%06x(pc,a%1d", pcl & 0xffffff,
             (dat & 0x7000) >> 12);
  else
    snprintf(str, sizeof(str), "z%06x(pc,d%1d", pcl & 0xffffff,
             (dat & 0x7000) >> 12);

  if (dat & 0x0800)
    strncat(str, ".l)", sizeof(str) - strlen(str) - 1);
  else
    strncat(str, ".w)", sizeof(str) - strlen(str) - 1);

  strmne(str);
}

void aimm() {
  switch (addsz) {
  case 0:
    dat = getdat(1);
    snprintf(str, sizeof(str), "#$%02x", dat & 0xff);
    break;

  case 1:
    dat = getdat(1);
    snprintf(str, sizeof(str), "#$%04x", dat);
    break;

  case 2:
    ldat = getldat(1);
    if (from_moveal) {
      snprintf(str, sizeof(str), "#z%06x", ldat);
      add_pc_sym(ldat);
    } else
      snprintf(str, sizeof(str), "#$%08x", ldat);
    break;

  default:
    *str = 0;
  }

  strmne(str);
}

int acdtaltn() { return (acdatan() & acaltn()); }

/* reg is 3 bits only */
int acdtalt(int mode, int reg) {
  return (acdata(mode, reg) & acalt(mode, reg));
}

int acdatan() { return (acdata(SMOD(*opcode), SREG(*opcode))); }

/* reg is 3 bits only */
static int acdata(int mode, int reg) {
  /* returns 0 if not data addressing mode */
  /* 1 otherwise */
  if ((mode == 1) || ((mode == 7) && (reg > 4)))
    return (0);
  else
    return (1);
}

int acmemn() { return (acmem(SMOD(*opcode), SREG(*opcode))); }

static int acmem(int mode, int reg) {
  /* rwturns 0 if not memory addressing mode */
  /* 1 otherwise */
  if ((mode < 2) || ((mode == 7) && (reg > 4)))
    return (0);
  else
    return (1);
}

int acconn() { return (accon(SMOD(*opcode), SREG(*opcode))); }

/* reg is 3 bits only */
static int accon(int mode, int reg) {
  /* returns 0 if not control addressing mode */
  /* 1 otherwise */
  mode &= 0x7;
  if (((mode < 5) && (mode != 2)) || ((mode == 7) && (reg > 3)))
    return (0);
  else
    return (1);
}

int acaltn() { return (acalt(SMOD(*opcode), SREG(*opcode))); }

/* reg is 3 bits only */
static int acalt(int mode, int reg) {
  if ((mode == 7) && (reg > 1))
    return (0);
  else
    return (1);
}

int acalln() { return (acall(SMOD(*opcode), SREG(*opcode))); }

/* reg is 3 bits only */
static int acall(int mode, int reg) {
  /* check for any addressing category */
  /* returns 1 if is */
  if ((mode == 7) && (reg > 4))
    return (0);
  else
    return (1);
}

/*
 * file read orutines
 */

u_int getldat(int flag) {
  u_int l = 0;

  l |= getdat(flag) << 16;
  l |= getdat(flag);
  return (l);
}

u_short getdat(int flag) {
  u_short w = 0;

  w |= (0x0ff & getc(cfp)) << 8;
  w |= (0x0ff & getc(cfp)) << 0;
  if (flag)
    opcode[opwcnt++] = w;

  return (w);
}

void eatdat(int n) {
  for (; n; --n)
    getc(cfp);
}

/*
 * output routines
 */

void incomma() { strncat(mnebf, ",", sizeof(mnebf) - strlen(mnebf) - 1); }

/* reg is 3 bits only */
void inan(int reg) {
  char str_local[15];

  snprintf(str_local, sizeof(str_local), "a%1d", reg);
  strncat(mnebf, str_local, sizeof(mnebf) - strlen(mnebf) - 1);
}

/* reg is 3 bits only */
void indn(int reg) {
  char str_local[15];

  snprintf(str_local, sizeof(str_local), "d%1d", reg);
  strncat(mnebf, str_local, sizeof(mnebf) - strlen(mnebf) - 1);
}

void strmne(char *str_input) {
  if (pass == 2)
    strncat(mnebf, str_input, sizeof(mnebf) - strlen(mnebf) - 1);
}
