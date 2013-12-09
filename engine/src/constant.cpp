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
#include "parsedef.h"
#include "filedefs.h"

//#include "execpt.h"
#include "constant.h"

#include "syntax.h"

#ifdef /* MCConstant::eval */ LEGACY_EXEC
Exec_stat MCConstant::eval(MCExecPoint &ep)
{
	if (nvalue == BAD_NUMERIC)
        MCExecValueTraits<MCStringRef>::set(r_value, MCValueRetain(svalue));
	else
        MCExecValueTraits<double>::set(r_value, nvalue);
}
#endif

void MCConstant::eval_ctxt(MCExecContext& ctxt, MCExecValue& r_value)
{
	if (nvalue == BAD_NUMERIC)
	{
		r_value . type = kMCExecValueTypeStringRef;
		r_value . stringref_value = MCValueRetain(svalue);
	}
	else
	{
		r_value . type = kMCExecValueTypeDouble;
		r_value . double_value = nvalue;
	}
}

void MCConstant::compile(MCSyntaxFactoryRef ctxt)
{
	MCSyntaxFactoryBeginExpression(ctxt, line, pos);
	
	if (nvalue == BAD_NUMERIC)
		MCSyntaxFactoryEvalConstant(ctxt, svalue);
	else
		MCSyntaxFactoryEvalConstantDouble(ctxt, nvalue);
	
	MCSyntaxFactoryEvalResult(ctxt);
	
	MCSyntaxFactoryEndExpression(ctxt);
}
