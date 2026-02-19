/***************************************************************************
 * print.c: print routines
 *
 * $Id$
 *
 * The Mamou Assembler - A Hitachi 6309 assembler
 *
 * (C) 2004 Boisy G. Pitre
 ***************************************************************************/

#include <time.h>

#include "mamou.h"

/*!
        @function print_line
        @discussion Prints an assembly output line
        @param as The assembler state structure
        @param override Overrides the printing rules
        @param infochar Character to be placed in the info field of the line
        @param counter Line counter?
 */
void print_line(assembler *as, int override, char infochar, int counter) {
  u_int i = 0;
  char Tmp_buff[MAXBUF];
  char Line_buff[MAXBUF];

  Line_buff[0] = EOS;

  if (as->conditional_stack[as->conditional_stack_index] == 0) {
    /* We are currently in a false condition -- return. */
    return;
  }

  if (as->pass == 1 && override == 0) {
    /* We do nothing on pass 1 here unless overridden. */
    return;
  }

  if (override == 0 && (as->o_show_listing == 0 || as->f_new_page == 1)) {
    if (as->line.has_warning) {
      as->num_warnings++;
    }

    return;
  }

  if (as->o_format_only == 0) {
    if (as->current_line == 0) {
      /* 1. We're at top of page, print header. */
      print_header(as);
    }

    /* 1. Print line number. */
    /* 1. Print line number. */
    snprintf(Tmp_buff, MAXBUF, "%05d ", (int)as->current_file->current_line);

    strncat(Line_buff, Tmp_buff, MAXBUF - strlen(Line_buff) - 1);

    /* TODO! warnings, errors will go here later */
    if (as->line.has_warning) {
      as->num_warnings++;

      if (infochar == ' ') {
        infochar = 'W';
      }
    }

    snprintf(Tmp_buff, MAXBUF, " %c ", infochar);

    strncat(Line_buff, Tmp_buff, MAXBUF - strlen(Line_buff) - 1);

    if (as->P_total || as->P_force) {
      snprintf(Tmp_buff, MAXBUF, "%04X ", counter);
      strncat(Line_buff, Tmp_buff, MAXBUF - strlen(Line_buff) - 1);
    } else {
      strncat(Line_buff, "     ", MAXBUF - strlen(Line_buff) - 1);
    }

    if (as->f_count_cycles) {
      if (as->cumulative_cycles) {
        snprintf(Tmp_buff, MAXBUF, "[%2u] ",
                 (unsigned int)as->cumulative_cycles);
        strncat(Line_buff, Tmp_buff, MAXBUF - strlen(Line_buff) - 1);
      } else {
        strncat(Line_buff, "     ", MAXBUF - strlen(Line_buff) - 1);
      }
    }

    for (i = 0; i < as->P_total && i < 4; i++) {
      snprintf(Tmp_buff, MAXBUF, "%02X", lobyte(as->P_bytes[i]));
      strncat(Line_buff, Tmp_buff, MAXBUF - strlen(Line_buff) - 1);
    }

    for (; i < 4; i++) {
      strncat(Line_buff, "  ", MAXBUF - strlen(Line_buff) - 1);
    }

    strncat(Line_buff, "   ", MAXBUF - strlen(Line_buff) - 1);
  }

  as->current_line++;

  if (as->newstyle == 1) {
    if (as->line.label[0] != EOS) {
      char tmp[MAXBUF + 1];

      snprintf(tmp, MAXBUF, "%s:", as->line.label);
      strncpy(as->line.label, tmp, MAXLAB - 1);
      as->line.label[MAXLAB - 1] = EOS;
    }

    if (as->line.Op[0] != EOS && as->line.mnemonic.type == OPCODE_PSEUDO) {
      char tmp[MAXBUF + 1];

      snprintf(tmp, MAXBUF, ".%s", as->line.Op);
      strncpy(as->line.Op, tmp, MAXOP - 1);
      as->line.Op[MAXOP - 1] = EOS;
    }

    if (as->line.comment[0] != EOS) {
      char tmp[MAXBUF + 2];

      if (as->line.comment[0] == '*') {
        as->line.comment[0] = ';';
      } else {
        snprintf(tmp, MAXBUF + 1, "; %s", as->line.comment);
        strncpy(as->line.comment, tmp, MAXBUF - 1);
        as->line.comment[MAXBUF - 1] = EOS;
      }
    }
  }

  /* New -- make all pseudo opcodes uppercase if wanted */
  if (as->pseudoUppercase == 1 && as->line.mnemonic.type == OPCODE_PSEUDO) {
    u_int i;

    for (i = 0; i < strlen(as->line.Op); i++) {
      as->line.Op[i] = toupper((unsigned char)as->line.Op[i]);
    }
  }

  if (as->line.type == LINETYPE_COMMENT)
  //	if (*as->line.label == EOS && *as->line.Op == EOS && *as->line.operand
  //== EOS)
  {
    /* possibly a comment? */
    if (*as->line.comment != EOS) {
      snprintf(Tmp_buff, MAXBUF, "%s", as->line.comment);
      strncat(Line_buff, Tmp_buff, MAXBUF - strlen(Line_buff) - 1);
    }
  } else if (as->line.type != LINETYPE_BLANK) {
    if (*as->line.comment == EOS) {
      if (as->tabbed) {
        snprintf(Tmp_buff, MAXBUF, "%s\t%s\t%.256s", as->line.label,
                 as->line.Op, as->line.operand);
      } else {
        snprintf(Tmp_buff, MAXBUF, "%-14s %-9s %.256s", as->line.label,
                 as->line.Op, as->line.operand);
      }
      strncat(Line_buff, Tmp_buff, MAXBUF - strlen(Line_buff) - 1);
    } else {
      if (as->tabbed) {
        snprintf(Tmp_buff, MAXBUF, "%s\t%s\t%.256s\t%.512s", as->line.label,
                 as->line.Op, as->line.operand, as->line.comment);
      } else {
        snprintf(Tmp_buff, MAXBUF, "%-14s %-9s %-19.256s %.512s",
                 as->line.label, as->line.Op, as->line.operand,
                 as->line.comment);
      }
      strncat(Line_buff, Tmp_buff, MAXBUF - strlen(Line_buff) - 1);
    }
  }

  if (as->Opt_G == 1) {
    int Temp_pc = as->old_program_counter;

    for (; i < as->P_total; i++) {
      if (i % 4 == 0) {
        as->current_file->current_line++;
        Temp_pc += 4;
        sprintf(Tmp_buff, "\n%05d   %04X ", (int)as->current_file->current_line,
                Temp_pc);
        strncat(Line_buff, Tmp_buff, MAXBUF - strlen(Line_buff) - 1);
      }
      snprintf(Tmp_buff, MAXBUF, "%02x", lobyte(as->P_bytes[i]));
      strncat(Line_buff, Tmp_buff, MAXBUF - strlen(Line_buff) - 1);
    }
  }

  /* Print out the built up line. */
  strncpy(Tmp_buff, Line_buff, as->o_pagewidth);
  Tmp_buff[as->o_pagewidth] = EOS;
  printf("%s\n", Tmp_buff);

  /* Check if we are at last line before footer should be printed. */
  if (as->o_format_only == 0) {
    if (as->current_line == as->o_page_depth - as->footer_depth) {
      print_footer(as);
      as->current_line = 0;
      as->current_page++;
    }
  }

  return;
}

/*!
        @function print_summary
        @discussion Prints the summary of the assembler's work
        @param as The assembler state structure
 */
void print_summary(assembler *as) {
  printf("\n");
  printf("Assembler Summary:\n");
  printf(" - %u errors, %u warnings\n", (unsigned int)as->num_errors,
         (unsigned int)as->num_warnings);
  printf(" - %u lines (%u source, %u blank, %u comment)\n",
         (unsigned int)as->cumulative_total_lines,
         (unsigned int)(as->cumulative_total_lines -
                        (as->cumulative_blank_lines +
                         as->cumulative_comment_lines)),
         (unsigned int)as->cumulative_blank_lines,
         (unsigned int)as->cumulative_comment_lines);

  if ((as->o_asm_mode == ASM_DECB) || (as->o_asm_mode == ASM_ROM)) {
    printf(" - $%04X (%u) bytes generated\n", (unsigned int)as->code_bytes,
           (unsigned int)as->code_bytes);
  } else {
    printf(" - $%04X (%u) program bytes, $%04X (%u) data bytes\n",
           (unsigned int)as->code_bytes, (unsigned int)as->code_bytes,
           (unsigned int)as->data_counter, (unsigned int)as->data_counter);
  }

  if (as->object_name == NULL) {
    printf(" - No output file\n");
  } else {
    printf(" - Output file: \"%s\"\n", as->object_name);
  }

  return;
}

/*!
        @function print_header
        @discussion Prints the header of the assembler's output page
        @param as The assembler state structure
 */
void print_header(assembler *as) {
  struct tm *tm;

  tm = localtime(&as->start_time);

  printf("The Mamou Assembler Version %02d.%02d      %02d/%02d/%02d "
         "%02d:%02d:%02d      Page %03u\n",
         VERSION_MAJOR, VERSION_MINOR, tm->tm_mon + 1, tm->tm_mday,
         tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec,
         (unsigned int)as->current_page);

  if (as->name_header[0] != EOS && as->title_header[0] != EOS) {
    printf("%s - %s\n", as->name_header, as->title_header);
  } else if (as->name_header[0] != EOS) {
    printf("%s\n", as->name_header);
  } else if (as->title_header[0] != EOS) {
    printf("%s\n", as->title_header);
  } else {
    printf("\n");
  }

  printf("\n");

  as->current_line += as->header_depth;

  return;
}

/*!
        @function print_footer
        @discussion Prints the footer of the assembler's output page
        @param as The assembler state structure
 */
void print_footer(assembler *as) {
  printf("\n");
  printf("\n");
  printf("\n");

  as->current_line += as->footer_depth;

  return;
}
