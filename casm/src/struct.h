/*****************************************************************************
	struct.h	- Declarations for the struct handler

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
#ifndef STRUCT_H
#define STRUCT_H

#include "config.h"
#include "input.h"


typedef struct _element Element;
typedef struct _struct Struct;


/* Element entry */
struct _element {
	char		name[MAX_LABELSIZE];
	int			offset;		/* Offset into the structure of the element */
	int			size;		/* Size of a single element */
	int			count;		/* Number of elements */
	Struct		*child;		/* Child structures */
	Element		*next;
};

/* Structure entry */
struct _struct {
	char		name[MAX_LABELSIZE];
	int			size;
	Element		*el_head;	/* First element in the list */
	Element		*el_tail;	/* Last element in the list */
	Struct		*next;
};


void StructCreate(EnvContext *ctx, const char *structName);
void StructAddElement(EnvContext *ctx, const char *elementName, const char *elementType);
Struct *StructLookup(const char *);
Element *StructGetElement(const Struct *, const char *);
void UnionCreate(EnvContext *ctx, const char *unionName);
void UnionEnd(EnvContext *ctx);

#endif	/* STRUCT_H */
