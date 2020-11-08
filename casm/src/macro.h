/*****************************************************************************
	macro.h	- Declarations for the macro handler

	Copyright (c) 2004 Chet Simpson, Digital Asphyxia. All rights reserved.

	The distribution, use, and duplication this file in source or binary form
	is restricted by an Artistic License (see license.txt) included with the
	standard distribution. If the license was not included with this package
	please refer to http://www.oarizo.com for more information.


	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that: (1) source code distributions
	retain the above copyright notice and this paragraph in its entirety, (2)
	distributions including binary code include the above copyright notice and
	this paragraph in its entirety in the documentation or other materials
	provided with the distribution, and (3) all advertising materials
	mentioning features or use of this software display the following
	acknowledgement:

		"This product includes software developed by Chet Simpson"
	
	The name of the author may be used to endorse or promote products derived
	from this software without specific priorwritten permission.

	THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED
	WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
	MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/
#ifndef MACRO_H
#define MACRO_H

#include "config.h"
#include "input.h"


/*
	Macro processing pointers, flags and counters
*/

bool OpenMacro(EnvContext *info, const char *name, const char *macroargs);
void CloseMacro(void);
void CreateMacro(EnvContext *info, const char *macroname, const char *macroargs);
void EndMacro(EnvContext *line);
void AddLineToMacro(EnvContext *line, const char *macroline);
char *GetMacroLine(EnvContext *info, char *outBuffer);
bool IsMacroOpen();
bool IsProcessingMacro();
void ResetMacroLocalLabels();
int GetNextMacroLocalLabel();
int GetMacroLocalLabel();
bool OnFirstMacroLine();


#endif	/* MACRO_H */
