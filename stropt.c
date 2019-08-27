/*
 *   stropt: Parse options from a string (supports quotation, option arguments, no malloc needed for parsing)
 *
 *   Copyright (C) 2018  Renzo Davoli <renzo@cs.unibo.it> VirtualSquare team.
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stropt.h>

//static char *sn[] = {"CHAR", "SEP", "END", "COMM", "NL", "ARG", "SQ", "DQ", "ESC", "ASQ", "ADQ", "AESC",};
#define CHAR 0
#define SEP 1
#define END 2
#define COMM 3
#define NL 4
#define ARG 5
#define SQ 6 //single quote
#define DQ 7 //double quote
#define ESC 8 // escape '\'
#define ASQ 9 //single quote
#define ADQ 10 //double quote
#define AESC 11 // escape '\'

#define NSTATES (AESC+1)
#define NINTAGS (ESC+1)

#define NEWTAG 0x1
#define NEWARG 0x2
#define CHCOPY 0x4
#define ENDTAG 0x8
#define ENDARG 0x10
#define ENDLINE 0x20
#define EOS 0x40

typedef char stropt_table[NSTATES][NINTAGS];
typedef const char stropt_row[NINTAGS];
typedef stropt_row *const_stropt_table_t;

static const stropt_table nextstate = {
	{CHAR, SEP, END, COMM, SEP, ARG,   SQ, DQ, ESC},//CHAR
	{CHAR, SEP, END, COMM, SEP, CHAR, SQ, DQ, ESC},//SEP
	{0,   0,    END, 0, 0, 0, 0, 0, 0},//END
	{COMM,COMM, END, COMM, SEP, COMM,COMM,COMM,COMM},//COMM
	{CHAR,SEP,  END, COMM, SEP, CHAR, SQ, DQ, ESC},//NL
	{ARG, SEP,  END, COMM, SEP, ARG, ASQ,ADQ, AESC},//ARG
	{SQ,  SQ,   END, SQ,   SQ,  SQ, CHAR, SQ,  SQ},//SQ
	{DQ,  DQ,   END, DQ,   DQ,  DQ,   DQ,CHAR, DQ},//DQ
	{CHAR,CHAR, END, CHAR,CHAR,CHAR, CHAR,CHAR,CHAR},//ESC
	{ASQ, ASQ,  END, ASQ,  ASQ, ASQ,  ARG,ASQ, ASQ},//ASQ
	{ADQ, ADQ,  END, ADQ,  ADQ, ADQ,  ADQ,ARG, ADQ},//ADQ
	{ARG, ARG,  END, ARG,  ARG, ARG,  ARG,ARG, ARG},//AESC
};

static const stropt_table default_action = {
	// CHAR  SEP     END         COMM     NL       ARG     SQ  DQ ESC
	{CHCOPY, ENDTAG, EOS|ENDTAG, ENDTAG,  ENDTAG,  NEWARG, 0, 0, 0}, //CHAR
	{NEWTAG|CHCOPY,0,EOS,        0,       0,       0,      NEWTAG, NEWTAG, NEWTAG}, //SEP
	{0,      0,      EOS, }, //END
	{0,      0,      EOS, }, //COMM
	{NEWTAG|CHCOPY,0,EOS  }, //NL
	{CHCOPY, ENDARG, EOS|ENDARG, ENDARG,  ENDARG,  CHCOPY, 0, 0, 0}, //ARG
	{CHCOPY, CHCOPY, EOS|ENDTAG, CHCOPY,  CHCOPY,  CHCOPY, 0, CHCOPY, CHCOPY}, //SQ
	{CHCOPY, CHCOPY, EOS|ENDTAG, CHCOPY,  CHCOPY,  CHCOPY, CHCOPY, 0, CHCOPY}, //DQ
	{CHCOPY, CHCOPY, EOS|ENDTAG, CHCOPY,  0,       CHCOPY, CHCOPY, CHCOPY, CHCOPY}, //ESC
	{CHCOPY, CHCOPY, EOS|ENDARG, CHCOPY,  CHCOPY,  CHCOPY, 0, CHCOPY, CHCOPY}, //ASQ
	{CHCOPY, CHCOPY, EOS|ENDARG, CHCOPY,  CHCOPY,  CHCOPY, CHCOPY, 0, CHCOPY}, //ADQ
	{CHCOPY, CHCOPY, EOS|ENDARG, CHCOPY,  0,       CHCOPY, CHCOPY, CHCOPY, CHCOPY}, //AESC
};

static const char default_charmap[256] = {
	[0] = END,
	[' '] = SEP, ['\t'] = SEP, [';'] = SEP, [','] = SEP,
	['#'] = COMM, ['='] = ARG, ['\n'] = NL,
	['\''] = SQ, ['"'] = DQ, ['\\'] = ESC
};

static int _stropt_engine(const char *input, const char *charmap, const stropt_table action,
		char **tags, char **args, char *buf)
{
	int state=SEP;
	int tagc=0;
	char *thistag=buf;
	if (args == NULL)
		args = tags;
	for (;state != END;input++) {
		int this = charmap[(int) *input];
#if 0
		/* uncomment for debugging */
		printf("%c %s %s->%s %x\n",
				*input, sn[this], sn[state], sn[nextstate[state][this]], action[state][this]);
#endif
		/* if tags == NULL, it is a dry-run just to count the tag items.
			 All the actions modifying buf/tags/args must be skipped */
		if (tags) {
			if (this == ARG && *args == *tags)
				this = CHAR;
			if (action[state][this] & NEWARG) {
				*buf++=0;
				*tags=thistag;
			}
			if (action[state][this] & ENDTAG) {
				*buf++=0;
				*args++=NULL;
				*tags++=thistag;
				thistag=buf;
			}
			if (action[state][this] & ENDARG) {
				*buf++=0;
				*args++=thistag;
				tags++;
				thistag=buf;
			}
			if (action[state][this] & (NEWTAG | NEWARG))
				thistag=buf;
			if (action[state][this] & CHCOPY)
				*buf++=*input;
			if (action[state][this] & ENDLINE) {
				*args++=NULL;
				*tags++="\n";
			}
			if (action[state][this] & EOS) {
				*args=NULL;
				*tags=NULL;
			}
		}
		if (action[state][this] & (ENDTAG | ENDARG))
			tagc++;
		if (action[state][this] & ENDLINE)
			tagc++;
		if (action[state][this] & EOS)
			tagc++;
		state=nextstate[state][this];
	}
	return tagc;
}

int stropt(const char *input, char **tags, char **args, char *buf) {
	return _stropt_engine(input, default_charmap, default_action, tags, args, buf);
}

int stroptx(const char *input, char *features, char *sep, int flags, char **tags, char **args, char *buf) {
	static char charmap[256];
	stropt_table action;
	memset(charmap, 0, sizeof(charmap));
	memcpy(action, default_action, sizeof(stropt_table));
	charmap[0] = END;
	if (features == NULL)
		features = "#=\n\'\"\\";
	if (sep == NULL)
		sep = " \t;,";
	for (;*features; features++)
		charmap[(int) *features] = default_charmap[(int) *features];
	for (;*sep; sep++)
		charmap[(int) *sep] = SEP;
	if (flags & STROPTX_KEEP_QUOTATION_MARKS_IN_TAGS) {
		action[CHAR][SQ] |= CHCOPY;
		action[CHAR][DQ] |= CHCOPY;
		action[CHAR][ESC] |= CHCOPY;
		action[SEP][SQ] |= CHCOPY;
		action[SEP][DQ] |= CHCOPY;
		action[SEP][ESC] |= CHCOPY;
		action[SQ][SQ] |= CHCOPY;
		action[DQ][DQ] |= CHCOPY;
	}
	if (flags & STROPTX_KEEP_QUOTATION_MARKS_IN_ARGS) {
		action[ARG][SQ] |= CHCOPY;
		action[ARG][DQ] |= CHCOPY;
		action[ARG][ESC] |= CHCOPY;
		action[ASQ][SQ] |= CHCOPY;
		action[ADQ][DQ] |= CHCOPY;
	}
	if (flags & STROPTX_ALLOW_MULTIPLE_SEP) {
		action[SEP][SEP] |= NEWTAG | ENDTAG;
		action[SEP][END] |= ENDTAG;
	}
	if (flags & STROPTX_NEWLINE_TAGS) {
		action[CHAR][NL] |= ENDLINE;
		action[SEP][NL] |= ENDLINE;
		action[COMM][NL] |= ENDLINE;
		action[NL][NL] |= ENDLINE;
		action[ARG][NL] |= ENDLINE;
	}
	return _stropt_engine(input, charmap, (const_stropt_table_t) action, tags, args, buf);
}

char *stropt2buf(void *buf, size_t size, char **tags, char **args, char sep, char eq) {
	char *optstr = NULL;
	char nextsep = 0;
	FILE *mf;
	if (buf == NULL) {
		size = 0;
		mf	= open_memstream(&optstr, &size);
	} else {
		mf = fmemopen(buf, size, "w+");
		optstr = buf;
	}
	if (args == NULL)
		args = tags;
	if (mf != NULL) {
		for (; *tags; tags++, args++) {
			if (*tags != STROPTX_DELETED_TAG) {
				if (nextsep) fprintf(mf, "%c", nextsep);
				fprintf(mf,"%s",*tags);
				if (args != tags && *args) {
					if (eq) fprintf(mf, "%c", eq);
					fprintf(mf, "%s", *args);
				}
				nextsep=sep;
			}
		}
		if (nextsep == '\n')
			fprintf(mf, "%c", nextsep);
		fclose(mf);
		return optstr;
	} else
		return NULL;
}
