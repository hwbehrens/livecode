/* Copyright (C) 2003-2013 Runtime Revolution Ltd.

This file is part of LiveCode.

LiveCode is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License v3 as published by the Free
Software Foundation.

LiveCode is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with LiveCode.  If not see <http://www.gnu.org/licenses/>.  */

#include "prefix.h"

#include "globdefs.h"
#include "filedefs.h"
#include "objdefs.h"
#include "parsedef.h"
#include "mcio.h"

#include "globals.h"
#include "dispatch.h"
#include "flst.h"
#include "uidc.h"
#include "util.h"

#include "exec.h"

////////////////////////////////////////////////////////////////////////////////

MC_EXEC_DEFINE_EVAL_METHOD(Text, FontNames, 2)
MC_EXEC_DEFINE_EVAL_METHOD(Text, FontLanguage, 2)
MC_EXEC_DEFINE_EVAL_METHOD(Text, FontSizes, 2)
MC_EXEC_DEFINE_EVAL_METHOD(Text, FontStyles, 3)

////////////////////////////////////////////////////////////////////////////////

bool MCTextBaseFontName(MCStringRef p_font, MCStringRef& r_base_name)
{
	uindex_t t_index;
	if (MCStringFirstIndexOfChar(p_font, ',', 0, kMCStringOptionCompareExact, t_index))
		return MCStringCopySubstring(p_font, MCRangeMake(0, t_index), r_base_name);
	else
		return MCStringCopy(p_font, r_base_name);
}

////////////////////////////////////////////////////////////////////////////////

void MCTextEvalFontNames(MCExecContext& ctxt, MCStringRef p_type, MCStringRef& r_names)
{
	MCAutoListRef t_name_list;
	if (MCdispatcher->getfontlist()->getfontnames(p_type, &t_name_list) &&
		MCListCopyAsString(*t_name_list, r_names))
		return;

	ctxt . Throw();
}

//////////

void MCTextEvalFontLanguage(MCExecContext& ctxt, MCStringRef p_font, MCNameRef& r_lang)
{
	MCAutoStringRef t_fontname;
	if (!MCTextBaseFontName(p_font, &t_fontname))
	{
		ctxt . Throw();
		return;
	}
	uint1 charset = MCscreen->fontnametocharset(*t_fontname);
	r_lang = MCU_charsettolanguage(charset);
	MCValueRetain(r_lang);
}

//////////

void MCTextEvalFontSizes(MCExecContext& ctxt, MCStringRef p_font, MCStringRef& r_sizes)
{
	MCAutoStringRef t_fontname;
	MCAutoListRef t_size_list;
	if (MCTextBaseFontName(p_font, &t_fontname) &&
		MCdispatcher->getfontlist()->getfontsizes(*t_fontname, &t_size_list) &&
		MCListCopyAsString(*t_size_list, r_sizes))
		return;

	ctxt . Throw();
}

//////////

void MCTextEvalFontStyles(MCExecContext& ctxt, MCStringRef p_font, integer_t p_size, MCStringRef& r_styles)
{
	MCAutoStringRef t_fontname;
	MCAutoListRef t_style_list;
	if (MCTextBaseFontName(p_font, &t_fontname) &&
		MCdispatcher->getfontlist()->getfontstyles(*t_fontname, p_size, &t_style_list) &&
		MCListCopyAsString(*t_style_list, r_styles))
		return;

	ctxt . Throw();
}

////////////////////////////////////////////////////////////////////////////////
