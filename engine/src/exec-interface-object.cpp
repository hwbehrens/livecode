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
#include "sysdefs.h"

#include "globals.h"
#include "object.h"
#include "stack.h"
#include "cdata.h"
#include "objptr.h"
#include "field.h"
#include "object.h"
#include "button.h"
#include "card.h"
#include "exec.h"

#include "stack.h"
#include "card.h"
#include "dispatch.h"
#include "parentscript.h"
#include "hndlrlst.h"
#include "mode.h"
#include "license.h"
#include "scriptpt.h"
#include "mcerror.h"
#include "chunk.h"
#include "pxmaplst.h"
#include "util.h"
#include "sellst.h"
#include "redraw.h"
#include "objectpropsets.h"
#include "font.h"

#include "exec-interface.h"

////////////////////////////////////////////////////////////////////////////////

typedef struct _PropList
{
	const char *token;
	uint2 value;
}
PropList;

static PropList stackprops[] =
    {
        {"altId", P_ALT_ID},
        {"alwaysBuffer", P_ALWAYS_BUFFER},
		{"behavior", P_PARENT_SCRIPT},
        {"blendLevel", P_BLEND_LEVEL},
        {"cantAbort", P_CANT_ABORT},
        {"cantDelete", P_CANT_DELETE},
        {"cantModify", P_CANT_MODIFY},
        {"colors", P_COLORS},
        {"decorations", P_DECORATIONS},
        {"destroyStack", P_DESTROY_STACK},
        {"destroyWindow", P_DESTROY_WINDOW},
        {"dynamicPaths", P_DYNAMIC_PATHS},
        {"formatForPrinting", P_FORMAT_FOR_PRINTING},
        {"hcAddressing", P_HC_ADDRESSING},
        {"icon", P_ICON},
        {"iconic", P_ICONIC},
        {"id", P_ID},
        {"linkColor", P_LINK_COLOR},
        {"linkHiliteColor", P_LINK_HILITE_COLOR},
        {"linkVisitedColor", P_LINK_VISITED_COLOR},
        {"liveresizing", P_LIVE_RESIZING},
        {"maxHeight", P_MAX_HEIGHT},
        {"maxWidth", P_MAX_WIDTH},
        {"menubar", P_MENU_BAR},
        {"metal", P_METAL},
        {"minHeight", P_MIN_HEIGHT},
        {"minWidth", P_MIN_WIDTH},
        {"name", P_SHORT_NAME},
        {"passkey", P_KEY},
        {"password", P_PASSWORD},
        {"patterns", P_PATTERNS},
        {"rect", P_RECTANGLE},
        {"resizable", P_RESIZABLE},
        {"scroll", P_VSCROLL},
        {"shadow", P_SHADOW},
        {"startUpIconic", P_START_UP_ICONIC},
        {"style", P_STYLE},
        {"textFont", P_TEXT_FONT},
        {"textSize", P_TEXT_SIZE},
        {"textStyle", P_TEXT_STYLE},
        {"title", P_LABEL},
        {"underlineLinks", P_UNDERLINE_LINKS},
        {"visible", P_VISIBLE},
        {"windowManagerPlace", P_WM_PLACE},
        {"windowShape", P_WINDOW_SHAPE}
    };

static PropList cardprops[] =
    {
        {"altId", P_ALT_ID},
		{"behavior", P_PARENT_SCRIPT},
        {"borderWidth", P_BORDER_WIDTH},
        {"blendLevel", P_BLEND_LEVEL},
        {"cantDelete", P_CANT_DELETE},
        {"colors", P_COLORS},
        {"defaultButton", P_DEFAULT_BUTTON},
        {"dontSearch", P_DONT_SEARCH},
        {"id", P_ID},
        {"ink", P_INK},
        {"layer", P_LAYER},
        {"mark", P_MARKED},
        {"name", P_SHORT_NAME},
        {"patterns", P_PATTERNS},
        {"rect", P_RECTANGLE},
        {"showBorder", P_SHOW_BORDER},
        {"textFont", P_TEXT_FONT},
        {"textSize", P_TEXT_SIZE},
        {"textStyle", P_TEXT_STYLE},
        {"threeD", P_3D}
    };

static PropList groupprops[] =
    {
        {"altId", P_ALT_ID},
        {"backgroundBehavior", P_BACKGROUND_BEHAVIOR},
		{"behavior", P_PARENT_SCRIPT},
        {"blendLevel", P_BLEND_LEVEL},
        {"borderWidth", P_BORDER_WIDTH},
        {"boundingRect", P_BOUNDING_RECT},
        {"cantDelete", P_CANT_DELETE},
        {"cantSelect", P_CANT_SELECT},
        {"colors", P_COLORS},
        {"disabled", P_DISABLED},
        {"dontSearch", P_DONT_SEARCH},
        {"hilitedButton", P_HILITED_BUTTON},
        {"hScroll", P_HSCROLL},
        {"hScrollbar", P_HSCROLLBAR},
        {"id", P_ID},
        {"ink", P_INK},
        {"label", P_LABEL},
        {"layer", P_LAYER},
        {"lockLoc", P_LOCK_LOCATION},
        {"margins", P_MARGINS},
        {"name", P_SHORT_NAME},
        {"opaque", P_OPAQUE},
        {"patterns", P_PATTERNS},
        {"radioBehavior", P_RADIO_BEHAVIOR},
        {"rect", P_RECTANGLE},
        {"scrollbarWidth", P_SCROLLBAR_WIDTH},
        {"showBorder", P_SHOW_BORDER},
        {"showName", P_SHOW_NAME},
        {"tabGroupBehavior", P_TAB_GROUP_BEHAVIOR},
        {"textFont", P_TEXT_FONT},
        {"textSize", P_TEXT_SIZE},
        {"textStyle", P_TEXT_STYLE},
        {"threeD", P_3D},
        {"toolTip", P_TOOL_TIP},
        {"traversalOn", P_TRAVERSAL_ON},
        {"visible", P_VISIBLE},
        {"vScroll", P_VSCROLL},
        {"vScrollbar", P_VSCROLLBAR}
    };

static PropList buttonprops[] =
    {
        {"accelKey", P_ACCELERATOR_KEY},
        {"accelMods", P_ACCELERATOR_MODIFIERS},
        {"accelText", P_ACCELERATOR_TEXT},
        {"altId", P_ALT_ID},
        {"armed", P_ARM},
        {"armedIcon", P_ARMED_ICON},
        {"armBorder", P_ARM_BORDER},
        {"armFill", P_ARM_FILL},
        {"autoArm", P_AUTO_ARM},
        {"autoHilite", P_AUTO_HILITE},
		{"behavior", P_PARENT_SCRIPT},
        {"blendLevel", P_BLEND_LEVEL},
        {"borderWidth", P_BORDER_WIDTH},
        {"cantSelect", P_CANT_SELECT},
        {"colors", P_COLORS},
        {"default", P_DEFAULT},
        {"disabled", P_DISABLED},
        {"disabledIcon", P_DISABLED_ICON},
        {"family", P_FAMILY},
        {"hilited", P_HILITE},
        {"hiliteBorder", P_HILITE_BORDER},
        {"hiliteFill", P_HILITE_FILL},
        {"hiliteIcon", P_HILITED_ICON},
		{"hoverIcon", P_HOVER_ICON},
        {"icon", P_ICON},
        {"id", P_ID},
        {"ink", P_INK},
        {"label", P_LABEL},
        {"labelWidth", P_LABEL_WIDTH},
        {"layer", P_LAYER},
        {"lockLoc", P_LOCK_LOCATION},
        {"margins", P_MARGINS},
        {"menuHistory", P_MENU_HISTORY},
        {"menuLines", P_MENU_LINES},
        {"menuMouseButton", P_MENU_BUTTON},
        {"menuMode", P_MENU_MODE},
        {"menuName", P_MENU_NAME},
        {"mnemonic", P_MNEMONIC},
        {"name", P_SHORT_NAME},
        {"opaque", P_OPAQUE},
        {"patterns", P_PATTERNS},
        {"rect", P_RECTANGLE},
        {"shadow", P_SHADOW},
        {"shadowOffset", P_SHADOW_OFFSET},
        {"sharedHilite", P_SHARED_HILITE},
        {"showBorder", P_SHOW_BORDER},
        {"showFocusBorder", P_SHOW_FOCUS_BORDER},
        {"showHilite", P_SHOW_HILITE},
        {"showIcon", P_SHOW_ICON},
        {"showName", P_SHOW_NAME},
        {"style", P_STYLE},
        {"text", P_TEXT},
        {"textAlign", P_TEXT_ALIGN},
        {"textFont", P_TEXT_FONT},
        {"textSize", P_TEXT_SIZE},
        {"textStyle", P_TEXT_STYLE},
        {"threeD", P_3D},
        {"toolTip", P_TOOL_TIP},
        {"traversalOn", P_TRAVERSAL_ON},
        {"visible", P_VISIBLE},
        {"visited", P_VISITED},
        {"visitedIcon", P_VISITED_ICON}
    };

static PropList fieldprops[] =
    {
        {"altId", P_ALT_ID},
        {"autoHilite", P_AUTO_HILITE},
        {"autoTab", P_AUTO_TAB},
		{"behavior", P_PARENT_SCRIPT},
        {"blendLevel", P_BLEND_LEVEL},
        {"borderWidth", P_BORDER_WIDTH},
        {"cantSelect", P_CANT_SELECT},
        {"colors", P_COLORS},
        {"disabled", P_DISABLED},
        {"dontSearch", P_DONT_SEARCH},
        {"dontWrap", P_DONT_WRAP},
        {"firstIndent", P_FIRST_INDENT},
        {"fixedLineHeight", P_FIXED_HEIGHT},
        {"hGrid", P_HGRID},
        {"hilitedLines", P_HILITED_LINES},
        {"hScroll", P_HSCROLL},
        {"hScrollbar", P_HSCROLLBAR},
        {"htmlText", P_HTML_TEXT},
        {"id", P_ID},
        {"ink", P_INK},
        {"layer", P_LAYER},
        {"listBehavior", P_LIST_BEHAVIOR},
        {"lockLoc", P_LOCK_LOCATION},
        {"lockText", P_LOCK_TEXT},
        {"margins", P_MARGINS},
        {"multipleHilites", P_MULTIPLE_HILITES},
        {"name", P_SHORT_NAME},
        {"noncontiguousHilites", P_NONCONTIGUOUS_HILITES},
        {"opaque", P_OPAQUE},
        {"patterns", P_PATTERNS},
        {"rect", P_RECTANGLE},
        {"scroll", P_VSCROLL},
        {"scrollbarWidth", P_SCROLLBAR_WIDTH},
        {"shadow", P_SHADOW},
        {"shadowOffset", P_SHADOW_OFFSET},
        {"sharedText", P_SHARED_TEXT},
        {"showBorder", P_SHOW_BORDER},
        {"showFocusBorder", P_SHOW_FOCUS_BORDER},
        {"showLines", P_SHOW_LINES},
        {"style", P_STYLE},
        {"tabStops", P_TAB_STOPS},
        {"textAlign", P_TEXT_ALIGN},
        {"textFont", P_TEXT_FONT},
        {"textHeight", P_TEXT_HEIGHT},
        {"textSize", P_TEXT_SIZE},
        {"textStyle", P_TEXT_STYLE},
        {"threeD", P_3D},
        {"threeDHilite", P_3D_HILITE},
        {"toggleHilites", P_TOGGLE_HILITE},
        {"toolTip", P_TOOL_TIP},
        {"traversalOn", P_TRAVERSAL_ON},
        {"vGrid", P_VGRID},
        {"visible", P_VISIBLE},
        {"vScroll", P_VSCROLL},
        {"vScrollbar", P_VSCROLLBAR}
    };

static PropList imageprops[] =
    {
        {"altId", P_ALT_ID},
        {"angle", P_ANGLE},
        {"alwaysBuffer", P_ALWAYS_BUFFER},
		{"behavior", P_PARENT_SCRIPT},
        {"blendLevel", P_BLEND_LEVEL},
        {"borderWidth", P_BORDER_WIDTH},
        {"cantSelect", P_CANT_SELECT},
        {"colors", P_COLORS},
        {"constantMask", P_CONSTANT_MASK},
        {"currentFrame", P_CURRENT_FRAME},
        {"disabled", P_DISABLED},
        {"dontDither", P_DONT_DITHER},
        {"fileName", P_FILE_NAME},
        {"frameCount", P_FRAME_COUNT},
        {"hotSpot", P_HOT_SPOT},
        {"id", P_ID},
        {"ink", P_INK},
        {"layer", P_LAYER},
        {"lockLoc", P_LOCK_LOCATION},
        {"name", P_SHORT_NAME},
        {"opaque", P_OPAQUE},
        {"palindromeFrames", P_PALINDROME_FRAMES},
        {"rect", P_RECTANGLE},
        {"repeatCount", P_REPEAT_COUNT},
        {"showBorder", P_SHOW_BORDER},
        {"showFocusBorder", P_SHOW_FOCUS_BORDER},
        {"size", P_SIZE},
        {"threeD", P_3D},
        {"toolTip", P_TOOL_TIP},
        {"traversalOn", P_TRAVERSAL_ON},
        {"visible", P_VISIBLE},
        {"xHot", P_XHOT},
        {"yHot", P_YHOT}
    };

static PropList graphicprops[] =
    {
        {"altId", P_ALT_ID},
        {"angle", P_ANGLE},
		{"antiAliased", P_ANTI_ALIASED},
        {"arcAngle", P_ARC_ANGLE},
        {"arrowSize", P_ARROW_SIZE},
		{"behavior", P_PARENT_SCRIPT},
        {"blendLevel", P_BLEND_LEVEL},
        {"borderWidth", P_BORDER_WIDTH},
        {"cantSelect", P_CANT_SELECT},
        {"colors", P_COLORS},
        {"dashes", P_DASHES},
        {"disabled", P_DISABLED},
        {"dontResize", P_DONT_RESIZE},
        {"endArrow", P_END_ARROW},
        {"filled", P_FILLED},
        {"id", P_ID},
        {"ink", P_INK},
        {"label", P_LABEL},
        {"layer", P_LAYER},
        {"lineSize", P_LINE_SIZE},
        {"lockLoc", P_LOCK_LOCATION},
        {"margins", P_MARGINS},
        {"markerDrawn", P_MARKER_DRAWN},
        {"markerFilled", P_MARKER_OPAQUE},
        {"markerPoints", P_MARKER_POINTS},
        {"name", P_SHORT_NAME},
        {"points", P_POINTS},
        {"polySides", P_POLY_SIDES},
        {"opaque", P_OPAQUE},
        {"patterns", P_PATTERNS},
        {"rect", P_RECTANGLE},
        {"roundEnds", P_ROUND_ENDS},
        {"roundRadius", P_ROUND_RADIUS},
        {"showBorder", P_SHOW_BORDER},
        {"showFocusBorder", P_SHOW_FOCUS_BORDER},
        {"showName", P_SHOW_NAME},
        {"startArrow", P_START_ARROW},
        {"style", P_STYLE},
        {"textFont", P_TEXT_FONT},
        {"textSize", P_TEXT_SIZE},
        {"textStyle", P_TEXT_STYLE},
        {"threeD", P_3D},
        {"toolTip", P_TOOL_TIP},
        {"traversalOn", P_TRAVERSAL_ON},
        {"visible", P_VISIBLE},
        {"markerLineSize", P_MARKER_LSIZE},
        {"startAngle", P_START_ANGLE},
		{"fillRule", P_FILL_RULE}, // PROPERTY - FILL RULE
		{"fillGradient", P_GRADIENT_FILL},
		{"strokeGradient", P_GRADIENT_STROKE},
		{"editMode", P_EDIT_MODE},
		{"capStyle", P_CAP_STYLE},
		{"joinStyle", P_JOIN_STYLE},
		{"miterLimit", P_MITER_LIMIT},
    };

static PropList scrollbarprops[] =
    {
        {"altId", P_ALT_ID},
		{"behavior", P_PARENT_SCRIPT},
        {"blendLevel", P_BLEND_LEVEL},
        {"borderWidth", P_BORDER_WIDTH},
        {"cantSelect", P_CANT_SELECT},
        {"colors", P_COLORS},
        {"disabled", P_DISABLED},
        {"endValue", P_END_VALUE},
        {"id", P_ID},
        {"ink", P_INK},
        {"layer", P_LAYER},
        {"lineInc", P_LINE_INC},
        {"lockLoc", P_LOCK_LOCATION},
        {"name", P_SHORT_NAME},
        {"numberFormat", P_NUMBER_FORMAT},
        {"opaque", P_OPAQUE},
        {"orientation", P_ORIENTATION},
        {"pageInc", P_PAGE_INC},
        {"patterns", P_PATTERNS},
        {"rect", P_RECTANGLE},
        {"showBorder", P_SHOW_BORDER},
        {"showFocusBorder", P_SHOW_FOCUS_BORDER},
        {"showValue", P_SHOW_VALUE},
        {"startValue", P_START_VALUE},
        {"style", P_STYLE},
        {"textFont", P_TEXT_FONT},
        {"textSize", P_TEXT_SIZE},
        {"textStyle", P_TEXT_STYLE},
        {"threeD", P_3D},
        {"thumbPosition", P_THUMB_POS},
        {"toolTip", P_TOOL_TIP},
        {"traversalOn", P_TRAVERSAL_ON},
        {"visible", P_VISIBLE},
        {"thumbSize", P_THUMB_SIZE}
    };

static PropList playerprops[] =
    {
        {"altId", P_ALT_ID},
        {"alwaysBuffer", P_ALWAYS_BUFFER},
		{"behavior", P_PARENT_SCRIPT},
        {"blendLevel", P_BLEND_LEVEL},
        {"borderWidth", P_BORDER_WIDTH},
        {"callbacks", P_CALLBACKS},
        {"cantSelect", P_CANT_SELECT},
        {"colors", P_COLORS},
        {"disabled", P_DISABLED},
        {"endTime", P_END_TIME},
        {"fileName", P_FILE_NAME},
        {"id", P_ID},
        {"ink", P_INK},
        {"layer", P_LAYER},
        {"lockLoc", P_LOCK_LOCATION},
        {"looping", P_LOOPING},
        {"name", P_SHORT_NAME},
        {"opaque", P_OPAQUE},
        {"patterns", P_PATTERNS},
        {"playRate", P_PLAY_RATE},
        {"playSelection", P_PLAY_SELECTION},
        {"rect", P_RECTANGLE},
        {"showBadge", P_SHOW_BADGE},
        {"showBorder", P_SHOW_BORDER},
        {"showController", P_SHOW_CONTROLLER},
        {"showFocusBorder", P_SHOW_FOCUS_BORDER},
        {"showSelection", P_SHOW_SELECTION},
        {"startTime", P_START_TIME},
        {"textFont", P_TEXT_FONT},
        {"textSize", P_TEXT_SIZE},
        {"textStyle", P_TEXT_STYLE},
        {"threeD", P_3D},
        {"toolTip", P_TOOL_TIP},
        {"traversalOn", P_TRAVERSAL_ON},
        {"visible", P_VISIBLE},
    };

static PropList epsprops[] =
    {
        {"altId", P_ALT_ID},
		{"behavior", P_PARENT_SCRIPT},
        {"blendLevel", P_BLEND_LEVEL},
        {"borderWidth", P_BORDER_WIDTH},
        {"boundingBox", P_BOUNDING_RECT},
        {"cantSelect", P_CANT_SELECT},
        {"colors", P_COLORS},
        {"currentPage", P_CURRENT_PAGE},
        {"disabled", P_DISABLED},
        {"id", P_ID},
        {"ink", P_INK},
        {"layer", P_LAYER},
        {"lockLoc", P_LOCK_LOCATION},
        {"name", P_SHORT_NAME},
        {"opaque", P_OPAQUE},
        {"pageCount", P_PAGE_COUNT},
        {"patterns", P_PATTERNS},
        {"postScript", P_POSTSCRIPT},
        {"prolog", P_PROLOG},
        {"rect", P_RECTANGLE},
        {"retainImage", P_RETAIN_IMAGE},
        {"retainPostScript", P_RETAIN_POSTSCRIPT},
        {"scale", P_SCALE},
        {"scaleIndependently", P_SCALE_INDEPENDENTLY},
        {"showBorder", P_SHOW_BORDER},
        {"showFocusBorder", P_SHOW_FOCUS_BORDER},
        {"threeD", P_3D},
        {"toolTip", P_TOOL_TIP},
        {"traversalOn", P_TRAVERSAL_ON},
        {"visible", P_VISIBLE},
        {"xExtent", P_X_EXTENT},
        {"xOffset", P_X_OFFSET},
        {"xScale", P_X_SCALE},
        {"yExtent", P_Y_EXTENT},
        {"yOffset", P_Y_OFFSET},
        {"xScale", P_X_SCALE},
        {"yScale", P_Y_SCALE}
    };

static PropList colorpaletteprops[] =
    {
        {"name", P_SHORT_NAME},
        {"id", P_ID},
        {"selectedColor", P_SELECTED_COLOR},
        {"rect", P_RECTANGLE}
    };

static PropList audioclipprops[] =
    {
        {"altID", P_ALT_ID},
        {"id", P_ID},
        {"name", P_NAME},
        {"playLoudness", P_PLAY_LOUDNESS},
        {"size", P_SIZE}
    };

static PropList videoclipprops[] =
    {
        {"altID", P_ALT_ID},
        {"dontRefresh", P_DONT_REFRESH},
        {"frameRate", P_FRAME_RATE},
        {"id", P_ID},
        {"name", P_NAME},
        {"playLoudness", P_PLAY_LOUDNESS},
        {"scale", P_SCALE},
        {"size", P_SIZE}
    };

////////////////////////////////////////////////////////////////////////////////

struct MCInterfaceLayer
{
	uint4 layer;
};

static void MCInterfaceLayerParse(MCExecContext& ctxt, MCStringRef p_input, MCInterfaceLayer& r_output)
{
	uint4 layer;
	if (MCStringIsEqualToCString(p_input, MCtopstring, kMCCompareCaseless))
		r_output . layer = MAXINT2;
	else if (MCStringIsEqualToCString(p_input, MCbottomstring, kMCCompareCaseless))
		r_output . layer = 1;
	else if (MCU_stoui4(p_input, layer))
		r_output . layer = layer;
	else
		ctxt . LegacyThrow(EE_OBJECT_LAYERNAN);
}

static void MCInterfaceLayerFormat(MCExecContext& ctxt, const MCInterfaceLayer& p_input, MCStringRef& r_output)
{
	if (MCStringFormat(r_output, "%d", p_input . layer))
		return;

	ctxt . Throw();
}

static void MCInterfaceLayerFree(MCExecContext& ctxt, MCInterfaceLayer& p_input)
{
}

static MCExecCustomTypeInfo _kMCInterfaceLayerTypeInfo =
{
	"Interface.Layer",
	sizeof(MCInterfaceLayer),
	(void *)MCInterfaceLayerParse,
	(void *)MCInterfaceLayerFormat,
	(void *)MCInterfaceLayerFree
};

//////////

static void MCInterfaceTextStyleParse(MCExecContext& ctxt, MCStringRef p_input, MCInterfaceTextStyle& r_output)
{
	uindex_t t_length;
	t_length = MCStringGetLength(p_input);

	if (t_length == 0)
	{
		r_output . style = 0;
		return;
	}

	uint2 style;
	style = FA_DEFAULT_STYLE;

	bool t_success;
	t_success = true;

	uindex_t t_old_offset;
	t_old_offset = 0;
	uindex_t t_new_offset;
	t_new_offset = 0;

	while (t_success && t_old_offset <= t_length)
	{
		MCAutoStringRef t_text_style;
		
		if (!MCStringFirstIndexOfChar(p_input, ',', t_old_offset, kMCCompareCaseless, t_new_offset))
			t_new_offset = t_length;

		while (MCStringGetNativeCharAtIndex(p_input, t_old_offset) == ' ')
			t_old_offset++;

		t_success = MCStringCopySubstring(p_input, MCRangeMake(t_old_offset, t_new_offset - t_old_offset), &t_text_style);

		if (t_success)
		{
			t_old_offset = t_new_offset + 1;

			if (MCF_setweightstring(style, MCStringGetOldString(*t_text_style)))
				continue;
			if (MCF_setexpandstring(style, MCStringGetOldString(*t_text_style)))
				continue;
			if (MCStringIsEqualToCString(*t_text_style, "oblique", kMCCompareCaseless))
			{
				style |= FA_OBLIQUE;
				continue;
			}
			if (MCStringIsEqualToCString(*t_text_style, "italic", kMCCompareCaseless))
			{
				style |= FA_ITALIC;
				continue;
			}
			if (MCStringIsEqualToCString(*t_text_style, MCplainstring, kMCCompareCaseless))
			{
				style = FA_DEFAULT_STYLE;
				continue;
			}
			if (MCStringIsEqualToCString(*t_text_style, MCmixedstring, kMCCompareCaseless))
			{
				style = FA_DEFAULT_STYLE;
				continue;
			}
			if (MCStringIsEqualToCString(*t_text_style, MCboxstring, kMCCompareCaseless))
			{
				style &= ~FA_3D_BOX;
				style |= FA_BOX;
				continue;
			}
			if (MCStringIsEqualToCString(*t_text_style, MCthreedboxstring, kMCCompareCaseless))
			{
				style &= ~FA_BOX;
				style |= FA_3D_BOX;
				continue;
			}
			if (MCStringIsEqualToCString(*t_text_style, MCunderlinestring, kMCCompareCaseless))
			{
				style |= FA_UNDERLINE;
				continue;
			}
			if (MCStringIsEqualToCString(*t_text_style, MCstrikeoutstring, kMCCompareCaseless))
			{
				style |= FA_STRIKEOUT;
				continue;
			}
			if (MCStringIsEqualToCString(*t_text_style, MCgroupstring, kMCCompareCaseless) || 
				MCStringIsEqualToCString(*t_text_style, MClinkstring, kMCCompareCaseless))
			{
				style |= FA_LINK;
				continue;
			}

			ctxt . LegacyThrow(EE_OBJECT_BADSTYLE);
			return;
		}
	}

	r_output . style = style;
}

static void MCInterfaceTextStyleFormat(MCExecContext& ctxt, const MCInterfaceTextStyle& p_input, MCStringRef& r_output)
{
	if (p_input . style == FA_DEFAULT_STYLE)
	{
		if (!MCStringCreateWithCString(MCplainstring, r_output))
			ctxt . Throw();
		return;
	}

	bool t_success;
	t_success = true;

	MCAutoListRef t_styles;

	if (t_success)
		t_success = MCListCreateMutable(',', &t_styles);

	if (t_success && MCF_getweightint(p_input . style) != MCFW_MEDIUM)
	{
		MCAutoStringRef t_style;
		t_success = (MCStringCreateWithCString(MCF_getweightstring(p_input . style), &t_style) &&
						MCListAppend(*t_styles, *t_style));
	}

	if (t_success && (p_input . style & FA_ITALIC || p_input . style & FA_OBLIQUE))
	{
		MCAutoStringRef t_style;
		t_success = (MCStringCreateWithCString(MCF_getslantlongstring(p_input . style), &t_style) &&
						MCListAppend(*t_styles, *t_style));
	}

	if (t_success && p_input . style & FA_BOX)
	{
		MCAutoStringRef t_style;
		t_success = (MCStringCreateWithCString(MCboxstring, &t_style) &&
						MCListAppend(*t_styles, *t_style));
	}

	if (t_success && p_input . style & FA_3D_BOX)
	{
		MCAutoStringRef t_style;
		t_success = (MCStringCreateWithCString(MCthreedboxstring, &t_style) &&
						MCListAppend(*t_styles, *t_style));
	}

	if (t_success && p_input . style & FA_UNDERLINE)
	{
		MCAutoStringRef t_style;
		t_success = (MCStringCreateWithCString(MCunderlinestring, &t_style) &&
						MCListAppend(*t_styles, *t_style));
	}

	if (t_success && p_input . style & FA_STRIKEOUT)
	{
		MCAutoStringRef t_style;
		t_success = (MCStringCreateWithCString(MCstrikeoutstring, &t_style) &&
						MCListAppend(*t_styles, *t_style));
	}

	if (t_success && p_input . style & FA_LINK)
	{
		MCAutoStringRef t_style;
		t_success = (MCStringCreateWithCString(MClinkstring, &t_style) &&
						MCListAppend(*t_styles, *t_style));
	}

	if (t_success && MCF_getexpandint(p_input . style) != FE_NORMAL)
	{
		MCAutoStringRef t_style;
		t_success = (MCStringCreateWithCString(MCF_getexpandstring(p_input . style), &t_style) &&
						MCListAppend(*t_styles, *t_style));
	}	

	if (t_success)
		t_success = MCListCopyAsString(*t_styles, r_output);

	if (t_success)
		return;

	ctxt . Throw();
}

static void MCInterfaceTextStyleFree(MCExecContext& ctxt, MCInterfaceTextStyle& p_input)
{
}

static MCExecCustomTypeInfo _kMCInterfaceTextStyleTypeInfo =
{
	"Interface.TextStyle",
	sizeof(MCInterfaceTextStyle),
	(void *)MCInterfaceTextStyleParse,
	(void *)MCInterfaceTextStyleFormat,
	(void *)MCInterfaceTextStyleFree
};

//////////

struct MCInterfaceShadow
{
	bool is_flag;

	union
	{
		bool flag;
		int2 shadow;
	};
};

static void MCInterfaceShadowParse(MCExecContext& ctxt, MCStringRef p_input, MCInterfaceShadow& r_output)
{
	if (MCU_stob(p_input, r_output . flag))
		r_output . is_flag = true;
	else if (MCU_stoi2(p_input, r_output . shadow))
		r_output . is_flag = false;
	else
		ctxt . LegacyThrow(EE_OBJECT_NAB);
}

static void MCInterfaceShadowFormat(MCExecContext& ctxt, const MCInterfaceShadow& p_input, MCStringRef& r_output)
{
	if (p_input . flag)
		r_output = (MCStringRef)MCValueRetain(kMCTrue);
	else
		r_output = (MCStringRef)MCValueRetain(kMCTrue);
}

static void MCInterfaceShadowFree(MCExecContext& ctxt, MCInterfaceShadow& p_input)
{
}

static MCExecCustomTypeInfo _kMCInterfaceShadowTypeInfo =
{
	"Interface.Shadow",
	sizeof(MCInterfaceShadow),
	(void *)MCInterfaceShadowParse,
	(void *)MCInterfaceShadowFormat,
	(void *)MCInterfaceShadowFree
};

//////////

static MCExecEnumTypeElementInfo _kMCInterfaceTextAlignElementInfo[] =
{	
	{ MCleftstring, F_ALIGN_LEFT, false },
	{ "", F_ALIGN_LEFT, false },
	{ MCcenterstring, F_ALIGN_CENTER, false },
	{ MCrightstring, F_ALIGN_RIGHT, false },
	{ MCjustifystring, F_ALIGN_JUSTIFY, false },
};

static MCExecEnumTypeInfo _kMCInterfaceTextAlignTypeInfo =
{
	"Interface.TextAlign",
	sizeof(_kMCInterfaceTextAlignElementInfo) / sizeof(MCExecEnumTypeElementInfo),
	_kMCInterfaceTextAlignElementInfo
};

//////////

static MCExecEnumTypeElementInfo _kMCInterfaceInkNamesElementInfo[] =
{	
	{ "clear", GXclear, false },
	{ "srcAnd", GXand, false },
	{ "srcAndReverse", GXandReverse, false },
	{ "srcCopy", GXcopy, false },
	{ "notSrcAnd", GXandInverted, false },
	{ "noop", GXnoop, false },
	{ "srcXor", GXxor, false },
	{ "srcOr", GXor, false },
	{ "notSrcAndReverse", GXnor, false },
	{ "notSrcXor", GXequiv, false },
	{ "reverse", GXinvert, false },
	{ "srcOrReverse", GXorReverse, false },
	{ "notSrcCopy", GXcopyInverted, false },
	{ "notSrcOr", GXorInverted, false },
	{ "notSrcOrReverse", GXnand, false },
	{ "set", GXset, false },
	{ "srcBic", GXsrcBic, false },
	{ "notSrcBic", GXnotSrcBic, false },
	{ "blend", GXblend, false },
	{ "addpin", GXaddpin, false },
	{ "addOver", GXaddOver, false },
	{ "subPin", GXsubPin, false },
	{ "transparent", GXtransparent, false },
	{ "addMax", GXaddMax, false },
	{ "subOver", GXsubOver, false },
	{ "adMin", GXaddMin, false },
	{ "blendClear", GXblendClear, false },
	{ "blendSrc", GXblendSrc, false },
	{ "blendDst", GXblendDst, false },
	{ "blendSrcOver", GXblendSrcOver, false },
	{ "blendDstOver", GXblendDstOver, false },
	{ "blendSrcIn", GXblendSrcIn, false },
	{ "blendDstIn", GXblendDstIn, false },
	{ "blendSrcOut", GXblendSrcOut, false },
	{ "blendDstOut", GXblendDstOut, false },
	{ "blendSrcAtop", GXblendSrcAtop, false },
	{ "blendDstAtop", GXblendDstAtop, false },
	{ "blendXor", GXblendXor, false },
	{ "blendPlus", GXblendPlus, false },
	{ "blendMultiply", GXblendMultiply, false },
	{ "blendScreen", GXblendScreen, false },
	{ "blendOverlay", GXblendOverlay, false },
	{ "blendDarken", GXblendDarken, false },
	{ "blendLighten", GXblendLighten, false },
	{ "blendDodge", GXblendDodge, false },
	{ "blendBurn", GXblendBurn, false },
	{ "blendHardLight", GXblendHardLight, false },
	{ "blendSoftLight", GXblendSoftLight, false },
	{ "blendDifference", GXblendDifference, false },
	{ "blendExclusion", GXblendExclusion, false },
};

static MCExecEnumTypeInfo _kMCInterfaceInkNamesTypeInfo =
{
	"Interface.InkNames",
	sizeof(_kMCInterfaceInkNamesElementInfo) / sizeof(MCExecEnumTypeElementInfo),
	_kMCInterfaceInkNamesElementInfo
};

////////////////////////////////////////////////////////////////////////////////

MCExecCustomTypeInfo *kMCInterfaceLayerTypeInfo = &_kMCInterfaceLayerTypeInfo;
MCExecCustomTypeInfo *kMCInterfaceShadowTypeInfo = &_kMCInterfaceShadowTypeInfo;
MCExecEnumTypeInfo *kMCInterfaceTextAlignTypeInfo = &_kMCInterfaceTextAlignTypeInfo;
MCExecCustomTypeInfo *kMCInterfaceTextStyleTypeInfo = &_kMCInterfaceTextStyleTypeInfo;
MCExecEnumTypeInfo *kMCInterfaceInkNamesTypeInfo = &_kMCInterfaceInkNamesTypeInfo;
MCExecEnumTypeInfo *kMCInterfaceEncodingTypeInfo = &_kMCInterfaceEncodingTypeInfo;

////////////////////////////////////////////////////////////////////////////////

void MCObject::Redraw(void)
{
	if (!opened)
		return;
	
	// MW-2011-08-18: [[ Layers ]] Invalidate the whole object.
	if (gettype() >= CT_GROUP)
		static_cast<MCControl *>(this) -> layer_redrawall();
}

////////////////////////////////////////////////////////////////////////////////

struct MCObjectChangeIdVisitor: public MCObjectVisitor
{
	uint32_t old_card_id;
	uint32_t new_card_id;

	void Process(MCCdata *p_cdata)
	{
		if (p_cdata == nil)
			return;

		MCCdata *t_ptr;
		t_ptr = p_cdata;
		do
		{
			if (t_ptr -> getid() == old_card_id)
			{
				t_ptr -> setid(new_card_id);
				return;
			}

			t_ptr = t_ptr -> next();
		}
		while(t_ptr != p_cdata);
	}

	bool OnField(MCField *p_field)
	{
		Process(p_field -> getcdata());
		return true;
	}

	bool OnButton(MCButton *p_button)
	{
		Process(p_button -> getcdata());
		return true;
	}
};

////////////////////////////////////////////////////////////////////////////////

void MCObject::GetId(MCExecContext& ctxt, uint32_t& r_id)
{
	r_id = obj_id;
}

void MCObject::SetId(MCExecContext& ctxt, uint32_t p_new_id)
{
	if (obj_id == p_new_id)
		return;

	// MW-2010-05-18: (Silently) don't allow id == 0 - this prevents people working around the
	//   script limits, which don't come into effect on objects with 0 id.
	if (p_new_id == 0)
		return;
	
	// MW-2011-02-08: Don't allow id change if the parent is nil as this means its a template
	//   object which doesn't really have an id.
	if (parent == nil)
		return;
	
	MCStack *t_stack;
	t_stack = getstack();

	if (t_stack -> isediting())
	{
		ctxt . LegacyThrow(EE_OBJECT_NOTWHILEEDITING);
		return;
	}

	// If the stack's id is less than the requested id then we are fine
	// since the stack id is always greater or equal to the highest numbered
	// control/card id. Otherwise, check the whole list of controls and cards.
	if (p_new_id <= t_stack -> getid())
	{
		if (t_stack -> getcontrolid(CT_LAYER, p_new_id) != NULL ||
			t_stack -> findcardbyid(p_new_id) != NULL)
		{
			ctxt . LegacyThrow(EE_OBJECT_IDINUSE, p_new_id);
			return;
		}
	}
	else
		t_stack -> obj_id = p_new_id;

	// If the object is a card, we have to reset all the control's data
	// id's.
	// If the object is not a card, but has a card as parent, we need to
	// reset the card's objptr id for it.
	if (gettype() == CT_CARD)
	{
		MCObjectChangeIdVisitor t_visitor;
		t_visitor . old_card_id = obj_id;
		t_visitor . new_card_id = p_new_id;
		t_stack -> visit(VISIT_STYLE_DEPTH_FIRST, 0, &t_visitor);
	}
	else if (parent -> gettype() == CT_CARD)
		static_cast<MCCard *>(parent) -> resetid(obj_id, p_new_id);

	// MW-2012-10-10: [[ IdCache ]] If the object is in the cache, then remove
	//   it since its id is changing.
	if (m_in_id_cache)
		t_stack -> uncacheobjectbyid(this);

	uint4 oldid = obj_id;
	obj_id = p_new_id;
	message_with_args(MCM_id_changed, oldid, obj_id);
}

void MCObject::GetAbbrevId(MCExecContext& ctxt, MCStringRef& r_abbrev_id)
{
	if (names(P_ABBREV_ID, r_abbrev_id))
		return;
	
	ctxt . Throw();
}

void MCObject::GetLongName(MCExecContext& ctxt, MCStringRef& r_long_name)
{
	if (names(P_LONG_NAME, r_long_name))
		return;
	
	ctxt . Throw();
}

void MCObject::GetLongId(MCExecContext& ctxt, MCStringRef& r_long_id)
{
	if (names(P_LONG_ID, r_long_id))
		return;
	
	ctxt . Throw();
}

void MCObject::GetName(MCExecContext& ctxt, MCStringRef& r_name)
{
	if (names(P_NAME, r_name))
		return;
	
	ctxt . Throw();
}

void MCObject::SetName(MCExecContext& ctxt, MCStringRef p_name)
{
	bool t_success;
	t_success = true;

	// Cannot have return characters in object names.
	MCAutoStringRef t_new_string;
	if (t_success)
		t_success = MCStringMutableCopy(p_name, &t_new_string);
	if (t_success)
		t_success = MCStringFindAndReplaceChar(*t_new_string, '\n', '_', kMCStringOptionCompareExact);
		
	MCNewAutoNameRef t_new_name;
	if (t_success)
		t_success = MCNameCreate(*t_new_string, &t_new_name);
		
	// MW-2012-09-12; [[ Bug ]] Make sure we compare literally, otherwise can't
	//   change case of names of objects.
	if (t_success && getname() != *t_new_name)
	{
		MCAutoNameRef t_old_name;
		t_old_name . Clone(getname());
		setname(*t_new_name);
		message_with_valueref_args(MCM_name_changed, t_old_name, getname());
	}
	
	if (t_success)
	{
		Redraw();
		return;
	}

	ctxt . Throw();
}

void MCObject::GetAbbrevName(MCExecContext& ctxt, MCStringRef& r_abbrev_name)
{
	if (names(P_ABBREV_NAME, r_abbrev_name))
		return;
	
	ctxt . Throw();
}

void MCObject::GetShortName(MCExecContext& ctxt, MCStringRef& r_short_name)
{
	if (names(P_SHORT_NAME, r_short_name))
		return;
	
	ctxt . Throw();
}

void MCObject::GetAltId(MCExecContext& ctxt, uint32_t& r_alt_id)
{
	r_alt_id = altid;
}

void MCObject::SetAltId(MCExecContext& ctxt, uint32_t p_new_alt_id)
{
	altid = p_new_alt_id;
}

////////////////////////////////////////////////////////////////////////////////

void MCObject::GetLayer(MCExecContext& ctxt, uint32_t part, MCInterfaceLayer& r_layer)
{
	// OK-2009-03-12: [[Bug 8049]] - Fix relayering of grouped objects broken by 
	// previous fix for crash when attempting to get the layer of an object outside
	// the group being edited in edit group mode.
	
	uint2 num;
	if (parent != nil)
	{
		MCCard *t_card;
		t_card = getcard(part);
		
		if(part != 0 && t_card == NULL)
			t_card = getstack() -> findcardbyid(part);
		
		if (t_card == NULL)
		{
			// This shouldn't happen, but rather than a crash, throw a random execution error..
			ctxt . LegacyThrow(EE_CHUNK_NOCARD);
			return;
		}
		
		t_card -> count(CT_LAYER, CT_UNDEFINED, this, num, True);
	}
	
	r_layer . layer = num;
}

void MCObject::SetLayer(MCExecContext& ctxt, uint32_t part, const MCInterfaceLayer& p_layer)
{
	if (parent == NULL || getcard(part)->relayer((MCControl *)this, p_layer . layer) != ES_NORMAL)
		ctxt . LegacyThrow(EE_OBJECT_BADRELAYER);
}

void MCObject::GetScript(MCExecContext& ctxt, MCStringRef& r_script)
{
	if (!MCdispatcher -> cut(True))
	{
		ctxt . LegacyThrow(EE_OBJECT_NOHOME);
		return;
	}

	if (!getstack() -> iskeyed())
	{
		ctxt . LegacyThrow(EE_STACK_NOKEY);
		return;
	}

	bool t_success = true;
	
	if (script == nil)
		r_script = MCValueRetain(kMCEmptyString);
	else
	{
		getstack() -> unsecurescript(this);
		t_success = MCStringCreateWithCString(script, r_script);
		getstack() -> securescript(this);
	}
	
	if (t_success)
		return;

	ctxt . Throw();
}

void MCObject::SetScript(MCExecContext& ctxt, MCStringRef new_script)
{
	if (!MCdispatcher->cut(True))
	{
		ctxt . LegacyThrow(EE_OBJECT_NOHOME);
		return;
	}
	if (!getstack()->iskeyed())
	{
		ctxt . LegacyThrow(EE_STACK_NOKEY);
		return;
	}
	if (scriptdepth != 0)
	{
		ctxt . LegacyThrow(EE_OBJECT_SCRIPTEXECUTING);
		return;
	}

	bool t_success;
	t_success = true;

	uint4 length;
	length = MCStringGetLength(new_script);

	if (length == 0)
	{
		delete hlist;
		hlist = NULL;
		delete script;
		script = NULL;
		flags &= ~F_SCRIPT;
		hashandlers = 0;
	}
	else
	{
		char *oldscript = script;
		if (MCStringGetNativeCharAtIndex(new_script, length - 1) != '\n')
		{
			MCAutoStringRef t_script;
			if (t_success)
				t_success = MCStringMutableCopy(new_script, &t_script);
			if (t_success)
				t_success = MCStringAppendChar(*t_script, '\n');
			if (t_success)
				t_success = MCStringAppendChar(*t_script, '\0');
			if (t_success)
				script = strclone(MCStringGetCString(*t_script));
		}
		else
			script = strclone(MCStringGetCString(new_script));

		getstack() -> securescript(this);
		
		if (t_success)
		{
			flags |= F_SCRIPT;
			if (MCModeCanSetObjectScript(obj_id))
			{ // not template object
				hashandlers = 0;
				parsescript(False, True);
				if (hlist != NULL && MClicenseparameters . script_limit > 0 && hlist -> linecount() >= MClicenseparameters . script_limit)
				{
					delete hlist;
					hlist = NULL;
					delete script;
					script = oldscript;
					oldscript = NULL;
					if (script == NULL)
						flags &= ~F_SCRIPT;
					MCperror->add(PE_OBJECT_NOTLICENSED, 0, 0);
				}
				if (!MCperror->isempty())
				{
					ctxt . SetTheResultToCString(MCperror->getsvalue() . getstring());
					MCperror->clear();
				}
				else
					ctxt . SetTheResultToEmpty();
			}
		}
		delete oldscript;
	}

	if (t_success)
		return;

	ctxt . Throw();
}

void MCObject::GetParentScript(MCExecContext& ctxt, MCStringRef& r_parent_script)
{
	// MW-2008-10-25: Handle the parentScript property when getting
	// If there is a parent script we return a reference string
	if (parent_script != nil)
	{
		MCParentScript *t_parent;
		t_parent = parent_script -> GetParent();
		
		if (MCStringFormat(r_parent_script, "button id %d of stack \"%s\"", t_parent -> GetObjectId(),
							MCNameGetCString(t_parent -> GetObjectStack())))
			return;

		ctxt . Throw();
	}
}

void MCObject::SetParentScript(MCExecContext& ctxt, MCStringRef new_parent_script)
{
	// MW-2008-10-25: Add the setting logic for parent scripts. This code is a
	//   modified version of what goes on in MCChunk::getobj when the final
	//   target for a chunk is an expression. We first parse the string as a
	//   chunk expression, then attempt to get the object of it. If the object
	//   doesn't exist, the set fails.
	bool t_success;
	t_success = true;

	// MW-2008-11-02: [[ Bug ]] Setting the parentScript of an object to
	//   empty should unset the parent script property and not throw an
	//   error.
	if (new_parent_script == nil || MCStringGetLength(new_parent_script) == 0)
	{
		if (parent_script != NULL)
			parent_script -> Release();
		parent_script = NULL;
		return;
	}

	// Create a script point with the value are setting the property to
	// as source text.
	MCScriptPoint sp(MCStringGetOldString(new_parent_script));

	// Create a new chunk object to parse the reference into
	MCChunk *t_chunk;
	t_chunk = new MCChunk(False);

	// Attempt to parse a chunk. We also check that there is no 'junk' at
	// the end of the string - if there is, its an error. Note the errorlock
	// here - it stops parse errors being pushed onto MCperror.
	Symbol_type t_next_type;
	MCerrorlock++;
	t_success = (t_chunk -> parse(sp, False) == PS_NORMAL && sp.next(t_next_type) == PS_EOF);
	MCerrorlock--;

	// Now attempt to evaluate the object reference - this will only succeed
	// if the object exists.
	MCExecPoint ep2(ctxt . GetEP());
	MCObject *t_object;
	uint32_t t_part_id;
	if (t_success)
		t_success = (t_chunk -> getobj(ep2, t_object, t_part_id, False) == ES_NORMAL);

	// Check that the object is a button
	if (t_success)
		t_success =	t_object -> gettype() == CT_BUTTON;

	// MW-2009-01-28: [[ Bug ]] Make sure we aren't setting the parentScript of
	//   an object to itself.
	if (t_success)
		t_success = t_object != this;

	if (t_success)
	{
		// Check to see if we are already parent-linked to t_object and if so
		// do nothing.
		//
		if (parent_script == NULL || parent_script -> GetParent() -> GetObject() != t_object)
		{
			// We have the target object, so extract its rugged id. That is the
			// (id, stack, mainstack) triple. Note that mainstack is NULL if the
			// object lies on a mainstack.
			//
			uint32_t t_id;
			t_id = t_object -> getid();

			MCNameRef t_stack;
			t_stack = t_object -> getstack() -> getname();

			// Now attempt to acquire a parent script use object. This can only
			// fail if memory is exhausted, so in this case just return an error
			// stat.
			MCParentScriptUse *t_use;
			t_use = MCParentScript::Acquire(this, t_id, t_stack);
			t_success = t_use != nil;

			// MW-2009-01-28: [[ Inherited parentScripts ]]
			// Next we have to ensure the inheritence hierarchy is in place (The
			// inherit call will create super-uses, and will return false if there
				// is not enough memory).
#ifdef FEATURE_INHERITED_PARENTSCRIPTS
			if (t_success)
				t_success = t_use -> Inherit();

			// TODO: Update all the Uses of this object, if it is currently
			// being used as a parentScript, because we have now changed the
			// inheritence hierarchy dynamically, and the various uses need
			// their super_use chains updated.
			// 
#endif

			// We have succeeded in creating a new use of an object as a parent
			// script, so now release the old parent script this object points
			// to (if any) and install the new one.
			if (parent_script != NULL)
				parent_script -> Release();

			parent_script = t_use;

			// Finally resolve the parent script as pointing to the object.
			parent_script -> GetParent() -> Resolve(t_object);
		}
	}
	else
	{
		ctxt . LegacyThrow(EE_PARENTSCRIPT_BADOBJECT);
		delete t_chunk;
		return;
	}

	// Delete our temporary chunk object.
	delete t_chunk;

	if (t_success)
		return;

	ctxt . Throw();
}

void MCObject::GetNumber(MCExecContext& ctxt, uint32_t part, uinteger_t& r_number)
{
	uint2 num;
	if (getstack() -> hcaddress())
	{
		if (parent -> gettype() == CT_CARD)
			getcard(part) -> count(gettype(), CT_CARD, this, num, True);
		else
			getcard(part) -> count(gettype(), CT_BACKGROUND, this, num, True);
	}
	else
		getcard(part) -> count(gettype(), CT_UNDEFINED, this, num, True);

	r_number = num;
}

bool MCObject::GetPixel(MCExecContext& ctxt, Properties which, bool effective, uinteger_t& r_pixel)
{
	// MW-2011-02-27: [[ Bug 9419 ]] If the object isn't already open, then alloc the color
	//   first.
	uint2 i;
	if (getcindex(which - P_FORE_PIXEL, i))
	{
		if (!opened)
			MCscreen -> alloccolor(colors[i]);
		r_pixel = colors[i] . pixel & 0xFFFFFF;
		return true;
	}
	
	if (effective && parent != nil)
		return parent -> GetPixel(ctxt, which, effective, r_pixel);

	return false;
}

void MCObject::SetPixel(MCExecContext& ctxt, Properties which, uinteger_t pixel)
{
	uint2 i;
	if (!getcindex(which - P_FORE_PIXEL, i))
		i = createcindex(which - P_FORE_PIXEL);

	colors[i] . pixel = pixel;
	MCscreen -> querycolor(colors[i]);
	delete colornames[i];
	colornames[i] = NULL;

	Redraw();
}

void MCObject::GetEffectiveForePixel(MCExecContext& ctxt, uinteger_t& r_pixel)
{
	GetPixel(ctxt, P_FORE_PIXEL, true, r_pixel);
}

void MCObject::GetForePixel(MCExecContext& ctxt, uinteger_t*& r_pixel)
{
	if (GetPixel(ctxt, P_FORE_PIXEL, false, *r_pixel))
		return;

	r_pixel = nil;
}

void MCObject::SetForePixel(MCExecContext& ctxt, uinteger_t* pixel)
{
	if (pixel != nil)
		SetPixel(ctxt, P_FORE_PIXEL, *pixel);
}

void MCObject::GetEffectiveBackPixel(MCExecContext& ctxt, uinteger_t& r_pixel)
{
	GetPixel(ctxt, P_BACK_PIXEL, true, r_pixel);
}

void MCObject::GetBackPixel(MCExecContext& ctxt, uinteger_t*& r_pixel)
{
	if (GetPixel(ctxt, P_BACK_PIXEL, false, *r_pixel))
		return;

	r_pixel = nil;
}

void MCObject::SetBackPixel(MCExecContext& ctxt, uinteger_t* pixel)
{
	if (pixel != nil)
		SetPixel(ctxt, P_BACK_PIXEL, *pixel);
}

void MCObject::GetEffectiveHilitePixel(MCExecContext& ctxt, uinteger_t& r_pixel)
{
	GetPixel(ctxt, P_HILITE_PIXEL, true, r_pixel);
}

void MCObject::GetHilitePixel(MCExecContext& ctxt, uinteger_t*& r_pixel)
{
	if (GetPixel(ctxt, P_HILITE_PIXEL, false, *r_pixel))
		return;

	r_pixel = nil;
}

void MCObject::SetHilitePixel(MCExecContext& ctxt, uinteger_t* pixel)
{
	if (pixel != nil)
		SetPixel(ctxt, P_HILITE_PIXEL, *pixel);
}

void MCObject::GetEffectiveBorderPixel(MCExecContext& ctxt, uinteger_t& r_pixel)
{
	GetPixel(ctxt, P_BORDER_PIXEL, true, r_pixel);
}

void MCObject::GetBorderPixel(MCExecContext& ctxt, uinteger_t*& r_pixel)
{
	if (GetPixel(ctxt, P_BORDER_PIXEL, false, *r_pixel))
		return;

	r_pixel = nil;
}

void MCObject::SetBorderPixel(MCExecContext& ctxt, uinteger_t* pixel)
{
	if (pixel != nil)
		SetPixel(ctxt, P_BORDER_PIXEL, *pixel);
}

void MCObject::GetEffectiveTopPixel(MCExecContext& ctxt, uinteger_t& r_pixel)
{
	GetPixel(ctxt, P_TOP_PIXEL, true, r_pixel);
}

void MCObject::GetTopPixel(MCExecContext& ctxt, uinteger_t*& r_pixel)
{
	if (GetPixel(ctxt, P_TOP_PIXEL, false, *r_pixel))
		return;

	r_pixel = nil;
}

void MCObject::SetTopPixel(MCExecContext& ctxt, uinteger_t* pixel)
{
	if (pixel != nil)
		SetPixel(ctxt, P_TOP_PIXEL, *pixel);
}

void MCObject::GetEffectiveBottomPixel(MCExecContext& ctxt, uinteger_t& r_pixel)
{
	GetPixel(ctxt, P_BOTTOM_PIXEL, true, r_pixel);
}

void MCObject::GetBottomPixel(MCExecContext& ctxt, uinteger_t*& r_pixel)
{
	if (GetPixel(ctxt, P_BOTTOM_PIXEL, false, *r_pixel))
		return;

	r_pixel = nil;
}

void MCObject::SetBottomPixel(MCExecContext& ctxt, uinteger_t* pixel)
{
	if (pixel != nil)
		SetPixel(ctxt, P_BOTTOM_PIXEL, *pixel);
}

void MCObject::GetEffectiveShadowPixel(MCExecContext& ctxt, uinteger_t& r_pixel)
{
	GetPixel(ctxt, P_SHADOW_PIXEL, true, r_pixel);
}

void MCObject::GetShadowPixel(MCExecContext& ctxt, uinteger_t*& r_pixel)
{
	if (GetPixel(ctxt, P_SHADOW_PIXEL, false, *r_pixel))
		return;

	r_pixel = nil;
}

void MCObject::SetShadowPixel(MCExecContext& ctxt, uinteger_t* pixel)
{
	if (pixel != nil)
		SetPixel(ctxt, P_SHADOW_PIXEL, *pixel);
}

void MCObject::GetEffectiveFocusPixel(MCExecContext& ctxt, uinteger_t& r_pixel)
{
	GetPixel(ctxt, P_FOCUS_PIXEL, true, r_pixel);
}

void MCObject::GetFocusPixel(MCExecContext& ctxt, uinteger_t*& r_pixel)
{
	if (GetPixel(ctxt, P_FOCUS_PIXEL, false, *r_pixel))
		return;

	r_pixel = nil;
}

void MCObject::SetFocusPixel(MCExecContext& ctxt, uinteger_t* pixel)
{
	if (pixel != nil)
		SetPixel(ctxt, P_FOCUS_PIXEL, *pixel);
}
////////////////////////////////////////////////////////////////////////////////

void MCObject::GetPenBackColor(MCExecContext& ctxt, MCValueRef& r_value)
{
	 // NO OP
}

void MCObject::SetPenBackColor(MCExecContext& ctxt, MCValueRef r_value)
{
	// NO OP
}

void MCObject::GetBrushBackColor(MCExecContext& ctxt, MCValueRef& r_value)
{
	// NO OP
}

void MCObject::SetBrushBackColor(MCExecContext& ctxt, MCValueRef r_value)
{
	// NO OP
}

void MCObject::SetColor(MCExecContext& ctxt, int index, const MCInterfaceNamedColor& p_color)
{
	uint2 i, j;
	if (p_color . name != nil && MCStringGetLength(p_color . name) == 0)
	{
		if (getcindex(index, i))
			destroycindex(index, i);
	}
	else
	{
		if (!getcindex(index, i))
		{
			i = createcindex(index);
			colors[i].red = colors[i].green = colors[i].blue = 0;
			if (opened)
				MCscreen->alloccolor(colors[i]);
		}
		MCColor oldcolor = colors[i];
		MCAutoStringRef t_color_name;
		set_interface_color(colors[i], &t_color_name, p_color);

		if (*t_color_name != nil)
			colornames[i] = strclone(MCStringGetCString(*t_color_name)); 
		j = i;
		if (getpindex(index, j))
		{
			if (opened)
				MCpatterns->freepat(pixmaps[j]);
			destroypindex(index, j);
		}
		if (opened)
			MCscreen->alloccolor(colors[i]);
	}
}

bool MCObject::GetColor(MCExecContext& ctxt, Properties which, bool effective, MCInterfaceNamedColor& r_color)
{
	uint2 i;
	if (getcindex(which - P_FORE_COLOR, i))
	{
		MCAutoStringRef t_color_name;
		if (MCStringCreateWithCString(colornames[i], &t_color_name))
		{
			get_interface_color(colors[i], *t_color_name, r_color);
			return true;
		}
	}
	else if (effective && parent != NULL)
		return parent -> GetColor(ctxt, which, effective, r_color);
	else
	{
		r_color . name = MCValueRetain(kMCEmptyString);
		return true;
	}

	return false;
}

void MCObject::GetForeColor(MCExecContext& ctxt, MCInterfaceNamedColor& r_color)
{
	if (GetColor(ctxt, P_FORE_COLOR, false, r_color))
		return;

	ctxt . Throw();
}

void MCObject::SetForeColor(MCExecContext& ctxt, const MCInterfaceNamedColor& color)
{
	SetColor(ctxt, DI_FORE, color);
	Redraw();
}

void MCObject::GetEffectiveForeColor(MCExecContext& ctxt, MCInterfaceNamedColor& r_color)
{
	GetColor(ctxt, P_FORE_COLOR, true, r_color);
}

void MCObject::GetBackColor(MCExecContext& ctxt, MCInterfaceNamedColor& r_color)
{
	if (GetColor(ctxt, P_BACK_COLOR, false, r_color))
		return;

	ctxt . Throw();
}

void MCObject::SetBackColor(MCExecContext& ctxt, const MCInterfaceNamedColor& color)
{
	SetColor(ctxt, DI_BACK, color);
	Redraw();
}

void MCObject::GetEffectiveBackColor(MCExecContext& ctxt, MCInterfaceNamedColor& r_color)
{
	GetColor(ctxt, P_BACK_COLOR, true, r_color);
}

void MCObject::GetHiliteColor(MCExecContext& ctxt, MCInterfaceNamedColor& r_color)
{
	if (GetColor(ctxt, P_HILITE_COLOR, false, r_color))
		return;

	ctxt . Throw();
}

void MCObject::SetHiliteColor(MCExecContext& ctxt, const MCInterfaceNamedColor& color)
{
	SetColor(ctxt, P_HILITE_COLOR - P_FORE_COLOR, color);
	Redraw();
}

void MCObject::GetEffectiveHiliteColor(MCExecContext& ctxt, MCInterfaceNamedColor& r_color)
{
	GetColor(ctxt, P_HILITE_COLOR, true, r_color);
}

void MCObject::GetBorderColor(MCExecContext& ctxt, MCInterfaceNamedColor& r_color)
{
	if (GetColor(ctxt, P_BORDER_COLOR, false, r_color))
		return;

	ctxt . Throw();
}

void MCObject::SetBorderColor(MCExecContext& ctxt, const MCInterfaceNamedColor& color)
{
	SetColor(ctxt, P_BORDER_COLOR - P_FORE_COLOR, color);
	Redraw();
}

void MCObject::GetEffectiveBorderColor(MCExecContext& ctxt, MCInterfaceNamedColor& r_color)
{
	GetColor(ctxt, P_BORDER_COLOR, true, r_color);
}

void MCObject::GetTopColor(MCExecContext& ctxt, MCInterfaceNamedColor& r_color)
{
	if (GetColor(ctxt, P_TOP_COLOR, false, r_color))
		return;

	ctxt . Throw();
}

void MCObject::SetTopColor(MCExecContext& ctxt, const MCInterfaceNamedColor& color)
{
	SetColor(ctxt, P_TOP_COLOR - P_FORE_COLOR, color);
	Redraw();
}

void MCObject::GetEffectiveTopColor(MCExecContext& ctxt, MCInterfaceNamedColor& r_color)
{
	GetColor(ctxt, P_TOP_COLOR, true, r_color);
}

void MCObject::GetBottomColor(MCExecContext& ctxt, MCInterfaceNamedColor& r_color)
{
	if (GetColor(ctxt, P_BOTTOM_COLOR, false, r_color))
		return;

	ctxt . Throw();
}

void MCObject::SetBottomColor(MCExecContext& ctxt, const MCInterfaceNamedColor& color)
{
	SetColor(ctxt, P_BOTTOM_COLOR - P_FORE_COLOR, color);
	Redraw();
}

void MCObject::GetEffectiveBottomColor(MCExecContext& ctxt, MCInterfaceNamedColor& r_color)
{
	GetColor(ctxt, P_BOTTOM_COLOR, true, r_color);
}

void MCObject::GetShadowColor(MCExecContext& ctxt, MCInterfaceNamedColor& r_color)
{
	if (GetColor(ctxt, P_SHADOW_COLOR, false, r_color))
		return;

	ctxt . Throw();
}

void MCObject::SetShadowColor(MCExecContext& ctxt, const MCInterfaceNamedColor& color)
{
	SetColor(ctxt, P_SHADOW_COLOR - P_FORE_COLOR, color);
	Redraw();
}

void MCObject::GetEffectiveShadowColor(MCExecContext& ctxt, MCInterfaceNamedColor& r_color)
{
	GetColor(ctxt, P_SHADOW_COLOR, true, r_color);
}

void MCObject::GetFocusColor(MCExecContext& ctxt, MCInterfaceNamedColor& r_color)
{
	if (GetColor(ctxt, P_FOCUS_COLOR, false, r_color))
		return;

	ctxt . Throw();
}

void MCObject::SetFocusColor(MCExecContext& ctxt, const MCInterfaceNamedColor& color)
{
	SetColor(ctxt, P_FOCUS_COLOR - P_FORE_COLOR, color);
	Redraw();
}

void MCObject::GetEffectiveFocusColor(MCExecContext& ctxt, MCInterfaceNamedColor& r_color)
{
	GetColor(ctxt, P_FOCUS_COLOR, true, r_color);
}

bool MCObject::GetColors(MCExecContext& ctxt, bool effective, MCStringRef& r_colors)
{
	bool t_success;
	t_success = true;

	MCAutoListRef t_color_list;
	if (t_success)
		t_success = MCListCreateMutable('\n', &t_color_list);

	if (t_success)
	{
		for (uint2 p = P_FORE_COLOR; p <= P_FOCUS_COLOR; p++)
		{
			MCInterfaceNamedColor t_color;
			MCAutoStringRef t_color_string;
			t_success = GetColor(ctxt, (Properties)p, effective, t_color);
			if (t_success)
			{
				MCInterfaceNamedColorFormat(ctxt, t_color, &t_color_string);
				MCInterfaceNamedColorFree(ctxt, t_color);
				t_success = !ctxt . HasError();
			}
			if (t_success)
				t_success = MCListAppend(*t_color_list, *t_color_string);

			if (!t_success)
				break;
		}
	}

	if (t_success)
		t_success = MCListCopyAsString(*t_color_list, r_colors);

	return t_success;
}

void MCObject::GetColors(MCExecContext& ctxt, MCStringRef& r_colors)
{
	if (GetColors(ctxt, false, r_colors))
		return;

	ctxt. Throw();
}

void MCObject::SetColors(MCExecContext& ctxt, MCStringRef p_input)
{
	bool t_success;
	t_success = true;

	uindex_t t_old_offset = 0;
	uindex_t t_new_offset = 0;
	uindex_t t_length;
	t_length = MCStringGetLength(p_input);

	for (uint2 index = DI_FORE; index <= DI_FOCUS; index++)
	{
		MCAutoStringRef t_color_string;
		if (!MCStringFirstIndexOfChar(p_input, '\n', t_old_offset, kMCCompareCaseless, t_new_offset))
			t_new_offset = t_length;
		
		if (t_new_offset > t_old_offset)
		{
			MCInterfaceNamedColor t_color;
			t_success = MCStringCopySubstring(p_input, MCRangeMake(t_old_offset, t_new_offset - t_old_offset), &t_color_string);
			if (t_success)
				MCInterfaceNamedColorParse(ctxt, *t_color_string, t_color);
				t_success = !ctxt . HasError();
			if (t_success)
			{
				uint2 i, j;
				if (getpindex(index, j))
				{
					if (opened)
						MCpatterns->freepat(pixmaps[j]);
					destroypindex(index, j);
				}
				if (!getcindex(index, i))
				{
					if (t_color . color . flags)
					{
						i = createcindex(index);
						colors[i] = t_color . color;
						if (opened)
							MCscreen->alloccolor(colors[i]);
						colornames[i] = t_color . name == nil ? NULL : strclone(MCStringGetCString(t_color . name));
					}
				}
				else
				{
					if (t_color . color . flags)
					{
						delete colornames[i];
						if (opened)
						{
							colors[i] = t_color . color;
							MCscreen->alloccolor(colors[i]);
						}
						colornames[i] = t_color . name == nil ? NULL : strclone(MCStringGetCString(t_color . name));
					}
					else
						destroycindex(index, i);
				}
				MCInterfaceNamedColorFree(ctxt, t_color);
			}
		}
		t_old_offset = t_new_offset + 1;
		if (t_old_offset > t_length)
			break;
	}

	if (t_success)
	{
		Redraw();
		return;
	}

	ctxt . Throw();	
}

void MCObject::GetEffectiveColors(MCExecContext& ctxt, MCStringRef& r_colors)
{
	if (GetColors(ctxt, true, r_colors))
		return;

	ctxt. Throw();
}

////////////////////////////////////////////////////////////////////////////////

bool MCObject::GetPattern(MCExecContext& ctxt, Properties which, bool effective, uint4& r_pattern)
{
	uint2 i;
	if (getpindex(which - P_FORE_PATTERN, i))
	{
		if (pixmapids[i] < PI_END && pixmapids[i] > PI_PATTERNS)
			r_pattern = pixmapids[i] - PI_PATTERNS;
		else
			r_pattern = pixmapids[i];
		return true;
	}
	else
	{
		if (effective && parent != NULL)
			return parent->GetPattern(ctxt, which, effective, r_pattern);
	}

	return false;
}

void MCObject::SetPattern(MCExecContext& ctxt, uint2 p_new_pixmap, uint4* p_new_id)
{
	uint2 i;
	bool t_isopened;
	t_isopened = (opened != 0) || (gettype() == CT_STACK && static_cast<MCStack*>(this)->getextendedstate(ECS_ISEXTRAOPENED));
	if (p_new_id == nil)
	{
		if (getpindex(p_new_pixmap, i))
		{
			if (t_isopened)
				MCpatterns->freepat(pixmaps[i]);
			destroypindex(p_new_pixmap, i);
		}
	}
	else
	{
		if (!getpindex(p_new_pixmap, i))
			i = createpindex(p_new_pixmap);
		else
			if (t_isopened)
				MCpatterns->freepat(pixmaps[i]);
		if (*p_new_id < PI_PATTERNS)
			*p_new_id += PI_PATTERNS;
		pixmapids[i] = *p_new_id;
		if (t_isopened)
			pixmaps[i] = MCpatterns->allocpat(pixmapids[i], this);
		if (getcindex(p_new_pixmap, i))
			destroycindex(p_new_pixmap, i);
	}
}

void MCObject::GetPenPattern(MCExecContext& ctxt, uinteger_t*& r_pattern)
{
	r_pattern = nil;
}

void MCObject::SetPenPattern(MCExecContext& ctxt, uinteger_t* pattern)
{
	SetPattern(ctxt, DI_FORE, pattern);
	Redraw();
}

void MCObject::GetBrushPattern(MCExecContext& ctxt, uinteger_t*& r_pattern)
{
	r_pattern = nil;
}

void MCObject::SetBrushPattern(MCExecContext& ctxt, uinteger_t* pattern)
{
	SetPattern(ctxt, DI_BACK, pattern);
	Redraw();
}

void MCObject::GetForePattern(MCExecContext& ctxt, uinteger_t*& r_pattern)
{
	if (GetPattern(ctxt, P_FORE_PATTERN, false, *r_pattern))
		return;

	r_pattern = nil;
}

void MCObject::SetForePattern(MCExecContext& ctxt, uinteger_t* pattern)
{
	SetPattern(ctxt, DI_FORE, pattern);
	Redraw();
}

void MCObject::GetEffectiveForePattern(MCExecContext& ctxt, uinteger_t& r_pattern)
{
	GetPattern(ctxt, P_FORE_PATTERN, true, r_pattern);
}

void MCObject::GetBackPattern(MCExecContext& ctxt, uinteger_t*& r_pattern)
{
	if (GetPattern(ctxt, P_BACK_PATTERN, false, *r_pattern))
		return;

	r_pattern = nil;
}

void MCObject::SetBackPattern(MCExecContext& ctxt, uinteger_t* pattern)
{
	SetPattern(ctxt, DI_BACK, pattern);
	Redraw();
}

void MCObject::GetEffectiveBackPattern(MCExecContext& ctxt, uinteger_t& r_pattern)
{
	GetPattern(ctxt, P_BACK_PATTERN, true, r_pattern);
}

void MCObject::GetHilitePattern(MCExecContext& ctxt, uinteger_t*& r_pattern)
{
	if (GetPattern(ctxt, P_HILITE_PATTERN, false, *r_pattern))
		return;

	r_pattern = nil;
}

void MCObject::SetHilitePattern(MCExecContext& ctxt, uinteger_t* pattern)
{
	SetPattern(ctxt, P_HILITE_PATTERN - P_FORE_PATTERN, pattern);
	Redraw();
}

void MCObject::GetEffectiveHilitePattern(MCExecContext& ctxt, uinteger_t& r_pattern)
{
	GetPattern(ctxt, P_HILITE_PATTERN, true, r_pattern);
}

void MCObject::GetBorderPattern(MCExecContext& ctxt, uinteger_t*& r_pattern)
{
	if (GetPattern(ctxt, P_BORDER_PATTERN, false, *r_pattern))
		return;

	r_pattern = nil;
}

void MCObject::SetBorderPattern(MCExecContext& ctxt, uinteger_t* pattern)
{
	SetPattern(ctxt, P_BORDER_PATTERN - P_FORE_PATTERN, pattern);
	Redraw();
}

void MCObject::GetEffectiveBorderPattern(MCExecContext& ctxt, uinteger_t& r_pattern)
{
	GetPattern(ctxt, P_BORDER_PATTERN, true, r_pattern);
}

void MCObject::GetTopPattern(MCExecContext& ctxt, uinteger_t*& r_pattern)
{
	if (GetPattern(ctxt, P_TOP_PATTERN, false, *r_pattern))
		return;

	r_pattern = nil;
}

void MCObject::SetTopPattern(MCExecContext& ctxt, uinteger_t* pattern)
{
	SetPattern(ctxt, P_TOP_PATTERN - P_FORE_PATTERN, pattern);
	Redraw();
}

void MCObject::GetEffectiveTopPattern(MCExecContext& ctxt, uinteger_t& r_pattern)
{
	GetPattern(ctxt, P_TOP_PATTERN, true, r_pattern);
}

void MCObject::GetBottomPattern(MCExecContext& ctxt, uinteger_t*& r_pattern)
{
	if (GetPattern(ctxt, P_BOTTOM_PATTERN, false, *r_pattern))
		return;

	r_pattern = nil;
}

void MCObject::SetBottomPattern(MCExecContext& ctxt, uinteger_t* pattern)
{
	SetPattern(ctxt, P_BOTTOM_PATTERN - P_FORE_PATTERN, pattern);
	Redraw();
}

void MCObject::GetEffectiveBottomPattern(MCExecContext& ctxt, uinteger_t& r_pattern)
{
	GetPattern(ctxt, P_BOTTOM_PATTERN, true, r_pattern);
}

void MCObject::GetShadowPattern(MCExecContext& ctxt, uinteger_t*& r_pattern)
{
	if (GetPattern(ctxt, P_SHADOW_PATTERN, false, *r_pattern))
		return;

	r_pattern = nil;
}

void MCObject::SetShadowPattern(MCExecContext& ctxt, uinteger_t* pattern)
{
	SetPattern(ctxt, P_SHADOW_PATTERN - P_FORE_PATTERN, pattern);
	Redraw();
}

void MCObject::GetEffectiveShadowPattern(MCExecContext& ctxt, uinteger_t& r_pattern)
{
	GetPattern(ctxt, P_SHADOW_PATTERN, true, r_pattern);
}

void MCObject::GetFocusPattern(MCExecContext& ctxt, uinteger_t*& r_pattern)
{
	if (GetPattern(ctxt, P_FOCUS_PATTERN, false, *r_pattern))
		return;

	r_pattern = nil;
}

void MCObject::SetFocusPattern(MCExecContext& ctxt, uinteger_t* pattern)
{
	SetPattern(ctxt, P_FOCUS_PATTERN - P_FORE_PATTERN, pattern);
	Redraw();
}

void MCObject::GetEffectiveFocusPattern(MCExecContext& ctxt, uinteger_t& r_pattern)
{
	GetPattern(ctxt, P_FOCUS_PATTERN, true, r_pattern);
}

bool MCObject::GetPatterns(MCExecContext& ctxt, bool effective, MCStringRef& r_patterns)
{	
	bool t_success;
	t_success = true;
	
	MCAutoListRef t_pattern_list;
	if (t_success)
		t_success = MCListCreateMutable('\n', &t_pattern_list);

	if (t_success)
	{
		for (uint2 p = P_FORE_PATTERN; p <= P_FOCUS_PATTERN; p++)
		{
			uint4 t_id;
			t_success = GetPattern(ctxt, (Properties)p, effective, t_id);
			MCAutoStringRef t_pattern;
			if (t_success)
				t_success = MCStringFormat(&t_pattern, "%d", t_id);
			if (t_success)
				t_success = MCListAppend(*t_pattern_list, *t_pattern);
			if (!t_success)
				break;
		}
	}
	
	if (t_success)
		t_success = MCListCopyAsString(*t_pattern_list, r_patterns);

	return t_success;
} 

void MCObject::GetPatterns(MCExecContext& ctxt, MCStringRef& r_patterns)
{
	if (GetPatterns(ctxt, false, r_patterns))
		return;

	ctxt . Throw();
}

void MCObject::SetPatterns(MCExecContext& ctxt, MCStringRef patterns)
{
	bool t_success;
	t_success = true;

	uindex_t t_old_offset = 0;
	uindex_t t_new_offset = 0;
	uindex_t t_length;
	t_length = MCStringGetLength(patterns);

	for (uint2 p = P_FORE_PATTERN; p <= P_FOCUS_PATTERN; p++)
	{
		MCAutoStringRef t_pattern;
		uint4 t_id;
		if (!MCStringFirstIndexOfChar(patterns, '\n', t_old_offset, kMCCompareCaseless, t_new_offset))
			t_new_offset = t_length;
		
		if (t_new_offset > t_old_offset)
		{
			t_success = MCStringCopySubstring(patterns, MCRangeMake(t_old_offset, t_new_offset - t_old_offset), &t_pattern);
			if (t_success)
				t_success = MCU_stoui4(*t_pattern, t_id);
			if (t_success)
				SetPattern(ctxt, (Properties)p - P_FORE_PATTERN, &t_id);
		}
		t_old_offset = t_new_offset + 1;
		if (t_old_offset > t_length)
			break;
	}

	if (t_success)
	{
		Redraw();
		return;
	}

	ctxt . Throw();
}

void MCObject::GetEffectivePatterns(MCExecContext& ctxt, MCStringRef& r_patterns)
{
	if (GetPatterns(ctxt, true, r_patterns))
		return;

	ctxt . Throw();
}

////////////////////////////////////////////////////////////////////////////////

void MCObject::GetLockLocation(MCExecContext& ctxt, bool& r_setting)
{
	r_setting = getflag(F_LOCK_LOCATION);
}

void MCObject::SetLockLocation(MCExecContext& ctxt, bool setting)
{
	if (changeflag(setting, F_LOCK_LOCATION))
		Redraw();
}

////////////////////////////////////////////////////////////////////////////////

bool MCObject::TextPropertyMapFont()
{
	// MW-2013-03-06: [[ Bug 10698 ]] When a text property is set on a stack we
	//   must recomputefonts() due to substacks. However, substacks can be open
	//   independently of their mainstacks which causes a problem for font mapping.
	//   If we map the font after setting attributes, we won't be able to tell if
	//   the concrete font has changed. Therefore, we map here if not mapped and
	//   then unmap afterwards (only stacks need this - for all other objects
	//   child open => parent open).
	// MW-2013-03-21: [[ Bug ]] The templateStack has no parent, so probably best
	//   *not* to attempt to mapfonts on it!
	// MW-2013-03-28: [[ Bug 10791 ]] Exceptions to every rule - the home stack
	//   can be open but with no font...
	if ((opened == 0 || m_font == nil) && gettype() == CT_STACK && parent != nil)
	{
		mapfont();
		return true;
	}
	
	return false;
}

void MCObject::TextPropertyUnmapFont(bool p_unmap)
{
	if (p_unmap)
		unmapfont();
}

void MCObject::GetTextHeight(MCExecContext& ctxt, uinteger_t*& r_height)
{
	if (fontheight != 0)
	{
		uinteger_t height;
		height = (uinteger_t)gettextheight();
		*r_height = height;
	}
	else
		r_height = nil;
}

void MCObject::SetTextHeight(MCExecContext& ctxt, uinteger_t* height)
{
	bool t_unmap_font = TextPropertyMapFont();
	
	fontheight = height == nil ? 0 : *height;
	recompute();
	
	TextPropertyUnmapFont(t_unmap_font);
	
	Redraw();
}

void MCObject::GetEffectiveTextHeight(MCExecContext& ctxt, uinteger_t& r_height)
{
	r_height = gettextheight();
}

void MCObject::GetTextAlign(MCExecContext& ctxt, intenum_t*& r_align)
{
	intenum_t align;
	align = (intenum_t)(flags & F_ALIGNMENT);
	*r_align = align;
}

void MCObject::SetTextAlign(MCExecContext& ctxt, intenum_t* align)
{
	flags &= ~F_ALIGNMENT;
	if (align == nil)
		flags |= F_ALIGN_LEFT;
	else
		flags |= *align;

	Redraw();
}

void MCObject::GetEffectiveTextAlign(MCExecContext& ctxt, intenum_t& r_align)
{
	r_align = (flags & F_ALIGNMENT);
}

void MCObject::GetTextFont(MCExecContext& ctxt, MCStringRef& r_font)
{
	if ((m_font_flags & FF_HAS_TEXTFONT) == 0)
		return;
	else
	{
		uint2 fontsize, fontstyle;
		const char *fontname;
		getfontattsnew(fontname, fontsize, fontstyle);
		if (MCStringCreateWithCString(fontname, r_font))
			return;
	}

	ctxt . Throw();
}

void MCObject::SetTextFont(MCExecContext& ctxt, MCStringRef font)
{
	bool t_redraw = false;
	
	bool t_unmap_font = TextPropertyMapFont();

	MCNewAutoNameRef t_font_name;

	bool t_success;
	t_success = true;
	
	if (font != nil)
	{
		char *newfontname = NULL;
		newfontname = strclone(MCStringGetCString(font));
			
		// MW-2012-02-17: [[ IntrinsicUnicode ]] Strip any lang tag from the
		//   fontname.
		char *t_tag;
		t_tag = strchr(newfontname, ',');
		if (t_tag != nil)
			t_tag[0] = '\0';
		
		if (t_success)
			t_success = MCNameCreateWithCString(newfontname, &t_font_name);

		delete newfontname;
	}

	if (t_success)
	{
		uint32_t t_font_flags;
		t_font_flags = 0;
		t_font_flags |= FF_HAS_TEXTFONT;

		setfontattrs(t_font_flags, *t_font_name, 0, 0);
		
		// MW-2012-12-14: [[ Bug ]] If this object is a stack, always recompute fonts
		//   to ensure substacks update properly.
		// MW-2013-03-21: [[ Bug ]] Unless its the templateStack (parent == nil) in which
		//   case we don't want to do any font recomputation.
		if ((gettype() == CT_STACK && parent != nil) || opened)
		{
			if (recomputefonts(parent -> getfontref()))
			{
				recompute();
				t_redraw = true;
			}
		}
	}

	TextPropertyUnmapFont(t_unmap_font);
	
	if (t_redraw)
		Redraw();
	
	if (t_success)
		return;
	
	ctxt . Throw();
}

void MCObject::GetEffectiveTextFont(MCExecContext& ctxt, MCStringRef& r_font)
{
	if ((m_font_flags & FF_HAS_TEXTFONT) == 0)
	{
		if (parent != nil)
			parent -> GetEffectiveTextFont(ctxt, r_font);
		else
			MCdispatcher -> GetDefaultTextFont(ctxt, r_font);
		return;
	}
	else
	{
		uint2 fontsize, fontstyle;
		const char *fontname;
		getfontattsnew(fontname, fontsize, fontstyle);
		if (MCStringCreateWithCString(fontname, r_font))
			return;
	}

	ctxt . Throw();
}

void MCObject::GetTextSize(MCExecContext& ctxt, uinteger_t*& r_size)
{
	if ((m_font_flags & FF_HAS_TEXTSIZE) == 0)
		return;

	uint2 fontsize, fontstyle;
	const char *fontname;
	getfontattsnew(fontname, fontsize, fontstyle);
	uinteger_t size;
	size = (uinteger_t)fontsize;
	*r_size = size;
}

void MCObject::SetTextSize(MCExecContext& ctxt, uinteger_t* size)
{
	bool t_redraw = false;
	
	bool t_unmap_font = TextPropertyMapFont();

	uint2 fontsize;
	fontsize = size != nil ? *size : 0;

	uint32_t t_font_flags;
	t_font_flags = 0;
	t_font_flags |= FF_HAS_TEXTSIZE;
	setfontattrs(t_font_flags, nil, fontsize, 0);

	fontheight = 0;

	// MW-2012-12-14: [[ Bug ]] If this object is a stack, always recompute fonts
	//   to ensure substacks update properly.
	// MW-2013-03-21: [[ Bug ]] Unless its the templateStack (parent == nil) in which
	//   case we don't want to do any font recomputation.
	if ((gettype() == CT_STACK && parent != nil) || opened)
	{
		if (recomputefonts(parent -> getfontref()))
		{
			recompute();
			t_redraw = true;
		}
	}

	TextPropertyUnmapFont(t_unmap_font);
	
	if (t_redraw)
		Redraw();
}

void MCObject::GetEffectiveTextSize(MCExecContext& ctxt, uinteger_t& r_size)
{
	if ((m_font_flags & FF_HAS_TEXTSIZE) == 0)
	{
		if (parent != nil)
			parent -> GetEffectiveTextSize(ctxt, r_size);
		else
			MCdispatcher -> GetDefaultTextSize(ctxt, r_size);
	}
	else
	{
		uint2 fontsize, fontstyle;
		const char *fontname;
		getfontattsnew(fontname, fontsize, fontstyle);
		r_size = (uinteger_t)fontsize;
	}
}

void MCObject::GetTextStyle(MCExecContext& ctxt, MCInterfaceTextStyle& r_style)
{
	if ((m_font_flags & FF_HAS_TEXTSTYLE) == 0)
		return;

	uint2 fontsize, fontstyle;
	const char *fontname;
	getfontattsnew(fontname, fontsize, fontstyle);
	r_style . style = fontstyle;
}

void MCObject::SetTextStyle(MCExecContext& ctxt, const MCInterfaceTextStyle& p_style)
{
	bool t_redraw = false;
	
	bool t_unmap_font = TextPropertyMapFont();

	uint2 fontstyle;
	fontstyle = p_style . style;

	uint32_t t_font_flags;
	t_font_flags = 0;
	t_font_flags |= FF_HAS_TEXTSTYLE;
	setfontattrs(t_font_flags, nil, 0, fontstyle);

	// MW-2012-12-14: [[ Bug ]] If this object is a stack, always recompute fonts
	//   to ensure substacks update properly.
	// MW-2013-03-21: [[ Bug ]] Unless its the templateStack (parent == nil) in which
	//   case we don't want to do any font recomputation.
	if ((gettype() == CT_STACK && parent != nil) || opened)
	{
		if (recomputefonts(parent -> getfontref()))
		{
			recompute();
			t_redraw = true;
		}
	}

	TextPropertyUnmapFont(t_unmap_font);
	
	if (t_redraw)
		Redraw();
}

void MCObject::GetEffectiveTextStyle(MCExecContext& ctxt, MCInterfaceTextStyle& r_style)
{
	if ((m_font_flags & FF_HAS_TEXTSIZE) == 0)
	{
		if (parent != nil)
			parent -> GetEffectiveTextStyle(ctxt, r_style);
		else
			MCdispatcher -> GetDefaultTextStyle(ctxt, r_style);
	}
	else
	{
		uint2 fontsize, fontstyle;
		const char *fontname;
		getfontattsnew(fontname, fontsize, fontstyle);
		r_style . style = fontstyle;
	}
}

////////////////////////////////////////////////////////////////////////////////

void MCObject::GetShowBorder(MCExecContext& ctxt, bool& r_setting)
{
	r_setting = getflag(F_SHOW_BORDER);
}

void MCObject::SetShowBorder(MCExecContext& ctxt, bool setting)
{
	if (changeflag(setting, F_SHOW_BORDER))
		Redraw();
}

void MCObject::GetShowFocusBorder(MCExecContext& ctxt, bool& r_setting)
{
	r_setting = (extraflags & EF_NO_FOCUS_BORDER) != True;
}

void MCObject::SetShowFocusBorder(MCExecContext& ctxt, bool setting)
{
	// Fetch the current transient rect of the control
	uint2 t_old_trans;
	t_old_trans = gettransient();

	if (setting)
		extraflags &= ~EF_NO_FOCUS_BORDER;
	else
		extraflags |= EF_NO_FOCUS_BORDER;

	// Redraw the control if the parent doesn't and we are open
	if (opened)
	{
		// MW-2011-08-18: [[ Layers ]] Take note of transient change and invalidate.
		if (gettype() >= CT_GROUP)
			static_cast<MCControl *>(this) -> layer_transientchangedandredrawall(t_old_trans);
	}
}

void MCObject::GetBorderWidth(MCExecContext& ctxt, uinteger_t& r_width)
{
	r_width = (uinteger_t)borderwidth;
}

void MCObject::SetBorderWidth(MCExecContext& ctxt, uinteger_t width)
{
	borderwidth = (uint1)width;
	Redraw();
}

void MCObject::GetOpaque(MCExecContext& ctxt, bool& r_setting)
{
	r_setting = getflag(F_OPAQUE);
}
void MCObject::SetOpaque(MCExecContext& ctxt, bool setting)
{
	if (changeflag(setting, F_OPAQUE))
		Redraw();
}
void MCObject::GetShadow(MCExecContext& ctxt, MCInterfaceShadow& r_shadow)
{
	r_shadow . flag = getflag(F_SHADOW);
}

void MCObject::SetShadow(MCExecContext& ctxt, const MCInterfaceShadow& p_shadow)
{
	if (p_shadow . is_flag)
	{
		if (changeflag(p_shadow . flag, F_SHADOW))
			Redraw();
	}
	else
	{
		shadowoffset = (int1)p_shadow . shadow;
		flags |= F_SHADOW;
	}	
}

void MCObject::GetShadowOffset(MCExecContext& ctxt, integer_t& r_offset)
{
	r_offset = (integer_t)shadowoffset;
}

void MCObject::SetShadowOffset(MCExecContext& ctxt, integer_t offset)
{
	shadowoffset = (int1)offset;
	Redraw();
}

void MCObject::Get3D(MCExecContext& ctxt, bool& r_setting)
{
	r_setting = getflag(F_3D);
}

void MCObject::Set3D(MCExecContext& ctxt, bool setting)
{
	if (changeflag(setting, F_3D))
		Redraw();
}

////////////////////////////////////////////////////////////////////////////////

void MCObject::SetVisibility(MCExecContext& ctxt, uint32_t part, bool setting, bool visible)
{
	bool dirty;
	dirty = changeflag(setting, F_VISIBLE);

	if (!visible)
	{
		flags ^= F_VISIBLE;
		dirty = !dirty;
	}

	// MW-2011-10-17: [[ Bug 9813 ]] Record the current effective rect of the object.
	MCRectangle t_old_effective_rect;
	if (dirty && opened && gettype() >= CT_GROUP)
		t_old_effective_rect = static_cast<MCControl *>(this) -> geteffectiverect();
	
	bool needmfocus;
	needmfocus = false;
	if (dirty)
	{
		if (opened && getstack() == MCmousestackptr)
			if (!(flags & F_VISIBLE))
			{
				MCObject *mfocused = MCmousestackptr->getcard()->getmfocused();
				// MW-2012-02-22: [[ Bug 10018 ]] If the target is a group then check
				//   to see if it is the ancestor of the mfocused control; otherwise
				//   just compare directly.
				if (mfocused != nil && gettype() == CT_GROUP)
				{
					while(mfocused -> gettype() != CT_CARD)
					{
						if (mfocused == this)
						{
							needmfocus = True;
							break;
						}
						mfocused = mfocused -> getparent();
					}
				}
				else if (this == mfocused)
					needmfocus = true;
			}
			else if (MCU_point_in_rect(rect, MCmousex, MCmousey))
				needmfocus = true;

		if (state & CS_KFOCUSED)
			getcard(part)->kunfocus();

		// MW-2008-08-04: [[ Bug 7094 ]] If we change the visibility of the control
		//   while its grabbed, we should ungrab it - otherwise it sticks to the
		//   cursor.
		if (gettype() >= CT_GROUP && getstate(CS_GRAB))
			state &= ~CS_GRAB;

		if (resizeparent())
			dirty = false;
	}

	if (dirty)
		signallisteners(P_VISIBLE);
	
	if (dirty && opened)
	{
		// MW-2011-08-18: [[ Layers ]] Take note of the change in visibility.
		if (gettype() >= CT_GROUP)
			static_cast<MCControl *>(this) -> layer_visibilitychanged(t_old_effective_rect);
	}

	if (needmfocus)
		MCmousestackptr->getcard()->mfocus(MCmousex, MCmousey);
}

void MCObject::GetVisible(MCExecContext& ctxt, uint32_t part, bool& r_setting)
{
	r_setting = getflag(F_VISIBLE);
}

void MCObject::SetVisible(MCExecContext& ctxt, uint32_t part, bool setting)
{
	SetVisibility(ctxt, part, setting, true);
}

void MCObject::GetInvisible(MCExecContext& ctxt, uint32_t part, bool& r_setting)
{
	r_setting = (flags & F_VISIBLE) != True;
}

void MCObject::SetInvisible(MCExecContext& ctxt, uint32_t part, bool setting)
{
	SetVisibility(ctxt, part, setting, false);
}

void MCObject::GetEnabled(MCExecContext& ctxt, uint32_t part, bool& r_setting)
{
	r_setting = (flags & F_DISABLED) != True;
}

void MCObject::SetEnabled(MCExecContext& ctxt, uint32_t part, bool setting)
{
	bool t_dirty;
	t_dirty = !changeflag(setting, F_DISABLED);
	
	flags ^= F_DISABLED;
		
	if (flags & F_DISABLED && state & CS_KFOCUSED)
		getcard(part)->kunfocus();

	if (t_dirty)
		Redraw();
}

void MCObject::GetDisabled(MCExecContext& ctxt, uint32_t part, bool& r_setting)
{
	r_setting = getflag(F_DISABLED);
}

void MCObject::SetDisabled(MCExecContext& ctxt, uint32_t part, bool setting)
{
	bool t_dirty;
	t_dirty = changeflag(setting, F_DISABLED);
		
	if (flags & F_DISABLED && state & CS_KFOCUSED)
		getcard(part)->kunfocus();

	if (t_dirty)
		Redraw();
}

void MCObject::GetSelected(MCExecContext& ctxt, bool& r_setting)
{
	r_setting = getstate(CS_SELECTED);
}

void MCObject::SetSelected(MCExecContext& ctxt, bool setting)
{
	if (setting != ((state & CS_SELECTED) != 0))
	{
		if (setting)
			MCselected->add(this);
		else
			MCselected->remove(this);
	}
}

void MCObject::GetTraversalOn(MCExecContext& ctxt, bool& r_setting)
{
	r_setting = getflag(F_TRAVERSAL_ON);
}

void MCObject::SetTraversalOn(MCExecContext& ctxt, bool setting)
{
	bool t_dirty;
	t_dirty = changeflag(setting, F_TRAVERSAL_ON);
		
	if (state & CS_KFOCUSED && !(flags & F_TRAVERSAL_ON))
		state &= ~CS_KFOCUSED;

	if (t_dirty)
		Redraw();
}

////////////////////////////////////////////////////////////////////////////////

void MCObject::GetOwner(MCExecContext& ctxt, MCStringRef& r_owner)
{
	if (parent != nil)
		parent -> GetName(ctxt, r_owner);
}

void MCObject::GetShortOwner(MCExecContext& ctxt, MCStringRef& r_owner)
{
	if (parent != nil)
		parent -> GetShortName(ctxt, r_owner);
}

void MCObject::GetAbbrevOwner(MCExecContext& ctxt, MCStringRef& r_owner)
{
	if (parent != nil)
		parent -> GetAbbrevName(ctxt, r_owner);
}

void MCObject::GetLongOwner(MCExecContext& ctxt, MCStringRef& r_owner)
{
	if (parent != nil)
		parent -> GetLongName(ctxt, r_owner);
}

void MCObject::GetProperties(MCExecContext& ctxt, uint32_t part, MCArrayRef& r_props)
{
	PropList *table;
	uint2 tablesize;

	switch (gettype())
	{
	case CT_STACK:
		table = stackprops;
		tablesize = ELEMENTS(stackprops);
		break;
	case CT_CARD:
		table = cardprops;
		tablesize = ELEMENTS(cardprops);
		break;
	case CT_GROUP:
		table = groupprops;
		tablesize = ELEMENTS(groupprops);
		break;
	case CT_BUTTON:
		table = buttonprops;
		tablesize = ELEMENTS(buttonprops);
		break;
	case CT_FIELD:
		table = fieldprops;
		tablesize = ELEMENTS(fieldprops);
		break;
	case CT_IMAGE:
		table = imageprops;
		tablesize = ELEMENTS(imageprops);
		break;
	case CT_GRAPHIC:
		table = graphicprops;
		tablesize = ELEMENTS(graphicprops);
		break;
	case CT_SCROLLBAR:
		table = scrollbarprops;
		tablesize = ELEMENTS(scrollbarprops);
		break;
	case CT_PLAYER:
		table = playerprops;
		tablesize = ELEMENTS(playerprops);
		break;
	case CT_EPS:
		table = epsprops;
		tablesize = ELEMENTS(epsprops);
		break;
	case CT_COLOR_PALETTE:
		table = colorpaletteprops;
		tablesize = ELEMENTS(colorpaletteprops);
		break;
	case CT_AUDIO_CLIP:
		table = audioclipprops;
		tablesize = ELEMENTS(audioclipprops);
		break;
	case CT_VIDEO_CLIP:
		table = videoclipprops;
		tablesize = ELEMENTS(videoclipprops);
		break;
	default:
		return;
	}
	bool t_success;
	t_success = true;

	MCAutoArrayRef t_array;
	if (t_success)
		t_success = MCArrayCreateMutable(&t_array);
	if (t_success)
	{
		MCerrorlock++;
		while (tablesize--)
		{
			getprop(part, (Properties)table[tablesize].value, ctxt . GetEP(), False);
			ctxt . GetEP() . storearrayelement_cstring(*t_array, table[tablesize].token);
		}
		MCerrorlock--;
		t_success = MCArrayCopy(*t_array, r_props);
	}
	
	if (t_success)
		return;

	ctxt . Throw();
}

void MCObject::SetProperties(MCExecContext& ctxt, uint32_t part, MCArrayRef props)
{
	// MW-2011-08-18: [[ Redraw ]] Update to use redraw.
	MCRedrawLockScreen();
	MCerrorlock++;

	uintptr_t t_iterator;
	t_iterator = 0;
	MCNameRef t_key;
	MCValueRef t_value;
	while(MCArrayIterate(props, t_iterator, t_key, t_value))
	{
		MCScriptPoint sp(MCStringGetCString(MCNameGetString(t_key)));
		Symbol_type type;
		const LT *te;
		if (sp.next(type) && sp.lookup(SP_FACTOR, te) == PS_NORMAL
		        && te->type == TT_PROPERTY && te->which != P_ID)
		{
			ctxt . GetEP() . setvalueref(t_value);
			setprop(part, (Properties)te->which, ctxt . GetEP(), False);
		}
	}	
	MCerrorlock--;
	MCRedrawUnlockScreen();
}

void MCObject::GetCustomPropertySet(MCExecContext& ctxt, MCStringRef& r_propset)
{
	r_propset = (MCStringRef)MCValueRetain(MCNameGetString(getdefaultpropsetname()));
}

void MCObject::SetCustomPropertySet(MCExecContext& ctxt, MCStringRef propset)
{
	MCNewAutoNameRef t_propset_name;
	if (MCNameCreate(propset, &t_propset_name))
	{
		if (props == nil)
			if (!MCObjectPropertySet::createwithname(kMCEmptyName, props))
				return;

		if (props -> hasname(*t_propset_name))
			return;

		MCObjectPropertySet *t_set;
		t_set = props;
		while(t_set -> getnext() != nil && !t_set -> getnext() -> hasname(*t_propset_name))
			t_set = t_set -> getnext();

		if (t_set -> getnext() == nil)
		{
			if (!MCObjectPropertySet::createwithname(*t_propset_name, t_set))
				return;

			t_set -> setnext(props);
			props = t_set;
		}
		else
		{
			MCObjectPropertySet *t_next_set;
			t_next_set = t_set -> getnext();
			t_set -> setnext(t_next_set -> getnext());
			t_next_set -> setnext(props);
			props = t_next_set;
		}
	}

	ctxt . Throw();
}

void MCObject::GetCustomPropertySets(MCExecContext& ctxt, MCStringRef& r_propsets)
{
	bool t_success;
	t_success = true;

	MCAutoListRef t_propsets;
	MCObjectPropertySet *p = props;

	if (t_success)
		t_success = MCListCreateMutable('\n', &t_propsets);

	while (t_success && p != NULL)
	{
		if (!p -> hasname(kMCEmptyName))
			t_success = MCListAppend(*t_propsets, p -> getname());
		p = p -> getnext();
	}

	if (t_success)
		t_success = MCListCopyAsString(*t_propsets, r_propsets);

	if (t_success)
		return;

	ctxt . Throw();
}

void MCObject::SetCustomPropertySets(MCExecContext& ctxt, MCStringRef propsets)
{
	uindex_t t_length;
	t_length = MCStringGetLength(propsets);
	MCAutoStringRef t_propsets_new;

	bool t_success;
	t_success = true;

	if (t_success)
		t_success = MCStringMutableCopy(propsets, &t_propsets_new);

	if (t_success && t_length != 0 && MCStringGetNativeCharAtIndex(propsets, t_length - 1) != '\n')
		t_success = MCStringAppendChar(*t_propsets_new, '\n');

	if (t_success)
		t_success = MCStringAppendChar(*t_propsets_new, '\n');		

	uindex_t t_old_offset;
	t_old_offset = 0;
	uindex_t t_new_offset;
	t_new_offset = 0;

	MCObjectPropertySet *newprops = NULL;
	MCObjectPropertySet *newp = NULL;

	while (t_success && t_old_offset <= t_length)
	{
		MCAutoStringRef t_name_string;
		MCNewAutoNameRef t_name;
		
		if (!MCStringFirstIndexOfChar(*t_propsets_new, '\n', t_old_offset, kMCCompareCaseless, t_new_offset))
			t_new_offset = t_length;

		if (t_new_offset > t_old_offset)
		{
			t_success = MCStringCopySubstring(*t_propsets_new, MCRangeMake(t_old_offset, t_new_offset - t_old_offset), &t_name_string);
			if (t_success)
				t_success = MCNameCreate(*t_name_string, &t_name);
		}
		else
			&t_name = MCValueRetain(kMCEmptyName);

		if (t_success)
		{
			MCObjectPropertySet *lp = NULL;
			MCObjectPropertySet *p = props;
			while (p != NULL && !p->hasname(*t_name))
			{
				lp = p;
				p = p->getnext();
			}
			if (p == NULL)
				/* UNCHECKED */ MCObjectPropertySet::createwithname(*t_name, p);
			else
			{
				if (p == props)
					props = props->getnext();
				else
					lp->setnext(p->getnext());
				p->setnext(NULL);
			}
			if (newprops == NULL)
				newprops = p;
			else
				newp->setnext(p);
			newp = p;
		}
		t_old_offset = t_new_offset + 1;
	}

	bool gotdefault = false;
	while (props != NULL)
	{
		MCObjectPropertySet *sp = props->getnext();
		if (props->hasname(kMCEmptyName))
		{
			props->setnext(newprops);
			newprops = props;
			gotdefault = true;
		}
		else
			delete props;
		props = sp;
	}
	if (!gotdefault && newprops != NULL)
	{
		/* UNCHECKED */ MCObjectPropertySet::createwithname(kMCEmptyName, props);
		props->setnext(newprops);
	}
	else
		props = newprops;
}

////////////////////////////////////////////////////////////////////////////////

void MCObject::GetInk(MCExecContext& ctxt, intenum_t& r_ink)
{
	r_ink = (intenum_t)ink;
}

void MCObject::SetInk(MCExecContext& ctxt, intenum_t newink)
{
	if (ink != (uint1)newink)
	{
		ink = (uint1)newink;
		Redraw();
	}
}

void MCObject::GetCantSelect(MCExecContext& ctxt, bool& r_setting)
{
	r_setting = !(isselectable(true));
}

void MCObject::SetCantSelect(MCExecContext& ctxt, bool setting)
{
	if (setting)
		extraflags |= EF_CANT_SELECT;
	else
		extraflags &= ~EF_CANT_SELECT;
	Redraw();
}

void MCObject::GetEffectiveCantSelect(MCExecContext& ctxt, bool& r_setting)
{
	r_setting = !(isselectable(false));
}

void MCObject::GetBlendLevel(MCExecContext& ctxt, uinteger_t& r_level)
{
	r_level = (uinteger_t)(100 - blendlevel);
}

void MCObject::SetBlendLevel(MCExecContext& ctxt, uinteger_t level)
{
	level = level > 100 ? 0 : 100 - level;
	if ((uint1)level != blendlevel)
	{
		blendlevel = (uint1)level;

		// MW-2012-04-11: [[ Bug ]] Special case for when a dynamic layer has its
		//   blend level changed - all we need do is invalidate the card.
		if (gettype() < CT_GROUP || !static_cast<MCControl *>(this) -> layer_issprite())
			Redraw();
		else
			static_cast<MCCard *>(parent) -> layer_dirtyrect(static_cast<MCControl *>(this) -> geteffectiverect());
	}
}

void MCObject::SetRectProp(MCExecContext& ctxt, bool p_effective, MCRectangle p_rect)
{
	MCRectangle t_rect;
	t_rect = p_rect;
	// MW-2012-10-26: Adjust the rectangle appropriately based on any effective margins.
	if (p_effective)
	{
		MCRectangle t_outer_rect;
		t_outer_rect = getrectangle(true);

		MCRectangle t_inner_rect;
		t_inner_rect = getrectangle(false);

		t_rect . x += t_inner_rect . x - t_outer_rect . x;
		t_rect . y += t_inner_rect . y - t_outer_rect . y;
		t_rect . width -= (t_inner_rect . x - t_outer_rect . x) + (t_outer_rect . x + t_outer_rect . width - (t_inner_rect . x + t_inner_rect . width));
		t_rect . height -= (t_inner_rect . y - t_outer_rect . y) + (t_outer_rect . y + t_outer_rect . height - (t_inner_rect . y + t_inner_rect . height));
	}

	if (!MCU_equal_rect(t_rect, rect))
	{
		bool needmfocus;
		needmfocus = false;

		if (opened && getstack() == MCmousestackptr)
		{
			MCControl *mfocused = MCmousestackptr->getcard()->getmfocused();
			if (MCU_point_in_rect(rect, MCmousex, MCmousey))
			{
				if (!MCU_point_in_rect(t_rect, MCmousex, MCmousey) && this == mfocused)
					needmfocus = true;
			}
			else
				if (MCU_point_in_rect(t_rect, MCmousex, MCmousey) && this != mfocused)
					needmfocus = true;
		}

		if (gettype() >= CT_GROUP)
		{
			// MW-2011-08-18: [[ Layers ]] Notify of change of rect.
			static_cast<MCControl *>(this) -> layer_setrect(t_rect, false);
			// Notify the parent of the resize.
			resizeparent();
		}
		else
			setrect(t_rect);

		if (needmfocus)
			MCmousestackptr->getcard()->mfocus(MCmousex, MCmousey);
	}
}

void MCObject::GetRectPoint(MCExecContext& ctxt, bool effective, Properties which, MCPoint &r_point)
{
	MCRectangle t_rect;
	t_rect = getrectangle(effective);

	switch (which)
	{
	case P_LOCATION:
		r_point . x = t_rect . x + (t_rect . width >> 1);
		r_point . y = t_rect . y + (t_rect . height >> 1);
		break;
	case P_TOP_LEFT:
		r_point . x = t_rect . x;
		r_point . y = t_rect . y;
		break;
	case P_TOP_RIGHT:
		r_point . x = t_rect . x + t_rect . width;
		r_point . y = t_rect . y;
		break;
	case P_BOTTOM_LEFT:
		r_point . x = t_rect . x;
		r_point . y = t_rect . y + t_rect . height;
		break;
	case P_BOTTOM_RIGHT:
		r_point . x = t_rect . x + t_rect . width;
		r_point . y = t_rect . y + t_rect . height;
		break;
	default:
		break;
	}
}

void MCObject::SetRectPoint(MCExecContext& ctxt, bool effective, Properties which, MCPoint point)
{
	MCRectangle t_rect;
	t_rect = getrectangle(effective);

	switch (which)
	{
	case P_LOCATION:
		point . x -= t_rect . width >> 1;
		point . y -= t_rect . height >> 1;
		break;
	case P_BOTTOM_LEFT:
		point . y -= t_rect . height;
		break;
	case P_BOTTOM_RIGHT:
		point . x -= t_rect . width;
		point . y -= t_rect . height;
		break;
	case P_TOP_LEFT:
		break;
	case P_TOP_RIGHT:
		point . x -= t_rect . width;
		break;
	}

	t_rect . x = point . x;
	t_rect . y = point . y;

	SetRectProp(ctxt, effective, t_rect);
}

void MCObject::GetRectValue(MCExecContext& ctxt, bool effective, Properties which, integer_t& r_value)
{
	MCRectangle t_rect;
	t_rect = getrectangle(effective);

	switch (which)
	{
	case P_LEFT:
		r_value = t_rect . x;
		break;
	case P_TOP:
		r_value = t_rect . y;
		break;
	case P_RIGHT:
		r_value = t_rect . x + t_rect . width;
		break;
	case P_BOTTOM:
		r_value = t_rect . y + t_rect . height;
		break;
	case P_WIDTH:
		r_value = t_rect . width;
		break;
	case P_HEIGHT:
		r_value = t_rect . height;
		break;
	default:
		break;
	}
}

void MCObject::SetRectValue(MCExecContext& ctxt, bool effective, Properties which, integer_t value)
{
	MCRectangle t_rect;
	t_rect = getrectangle(effective);

	switch (which)
	{
	case P_LEFT:
		t_rect . x = value;
		break;
	case P_RIGHT:
		t_rect . x = value - t_rect . width;
		break;
	case P_TOP:
		t_rect . y = value;
		break;
	case P_BOTTOM:
		t_rect . y = value - t_rect . height;
		break;
	case P_WIDTH:
		if (!getflag(F_LOCK_LOCATION))
			t_rect . x += (t_rect . width - value) >> 1;
		t_rect . width = MCU_max(value, 1);
		break;
	case P_HEIGHT:
		if (!getflag(F_LOCK_LOCATION))
			t_rect . y += (t_rect . height - value) >> 1;
		t_rect . height = MCU_max(value, 1);
		break;
	}

	SetRectProp(ctxt, effective, t_rect);
}

void MCObject::GetLocation(MCExecContext& ctxt, MCPoint& r_location)
{
	GetRectPoint(ctxt, false, P_LOCATION, r_location);
}

void MCObject::SetLocation(MCExecContext& ctxt, MCPoint location)
{
	SetRectPoint(ctxt, false, P_LOCATION, location);
}

void MCObject::GetEffectiveLocation(MCExecContext& ctxt, MCPoint& r_location)
{
	GetRectPoint(ctxt, true, P_LOCATION, r_location);
}

void MCObject::SetEffectiveLocation(MCExecContext& ctxt, MCPoint location)
{
	SetRectPoint(ctxt, true, P_LOCATION, location);
}

void MCObject::GetLeft(MCExecContext& ctxt, integer_t& r_value)
{
	GetRectValue(ctxt, false, P_LEFT, r_value);
}

void MCObject::SetLeft(MCExecContext& ctxt, integer_t value)
{
	SetRectValue(ctxt, false, P_LEFT, value);
}

void MCObject::GetEffectiveLeft(MCExecContext& ctxt, integer_t& r_value)
{
	GetRectValue(ctxt, true, P_LEFT, r_value);
}

void MCObject::SetEffectiveLeft(MCExecContext& ctxt, integer_t value)
{
	SetRectValue(ctxt, true, P_LEFT, value);
}

void MCObject::GetTop(MCExecContext& ctxt, integer_t& r_value)
{
	GetRectValue(ctxt, false, P_TOP, r_value);
}
void MCObject::SetTop(MCExecContext& ctxt, integer_t value)
{
	SetRectValue(ctxt, false, P_TOP, value);
}

void MCObject::GetEffectiveTop(MCExecContext& ctxt, integer_t& r_value)
{
	GetRectValue(ctxt, true, P_TOP, r_value);
}

void MCObject::SetEffectiveTop(MCExecContext& ctxt, integer_t value)
{
	SetRectValue(ctxt, true, P_TOP, value);
}

void MCObject::GetRight(MCExecContext& ctxt, integer_t& r_value)
{
	GetRectValue(ctxt, false, P_RIGHT, r_value);
}
void MCObject::SetRight(MCExecContext& ctxt, integer_t value)
{
	SetRectValue(ctxt, false, P_RIGHT, value);
}

void MCObject::GetEffectiveRight(MCExecContext& ctxt, integer_t& r_value)
{
	GetRectValue(ctxt, true, P_RIGHT, r_value);
}

void MCObject::SetEffectiveRight(MCExecContext& ctxt, integer_t value)
{
	SetRectValue(ctxt, true, P_RIGHT, value);
}

void MCObject::GetBottom(MCExecContext& ctxt, integer_t& r_value)
{
	GetRectValue(ctxt, false, P_BOTTOM, r_value);
}
void MCObject::SetBottom(MCExecContext& ctxt, integer_t value)
{
	SetRectValue(ctxt, false, P_BOTTOM, value);
}

void MCObject::GetEffectiveBottom(MCExecContext& ctxt, integer_t& r_value)
{
	GetRectValue(ctxt, true, P_BOTTOM, r_value);
}

void MCObject::SetEffectiveBottom(MCExecContext& ctxt, integer_t value)
{
	SetRectValue(ctxt, true, P_BOTTOM, value);
}

void MCObject::GetWidth(MCExecContext& ctxt, uinteger_t& r_value)
{
	integer_t t_value;
	GetRectValue(ctxt, false, P_WIDTH, t_value);
	r_value = (uinteger_t)t_value;
}
void MCObject::SetWidth(MCExecContext& ctxt, uinteger_t value)
{
	SetRectValue(ctxt, false, P_WIDTH, value);
}

void MCObject::GetEffectiveWidth(MCExecContext& ctxt, uinteger_t& r_value)
{
	integer_t t_value;
	GetRectValue(ctxt, true, P_WIDTH, t_value);
	r_value = (uinteger_t)t_value;
}

void MCObject::SetEffectiveWidth(MCExecContext& ctxt, uinteger_t value)
{
	SetRectValue(ctxt, true, P_WIDTH, value);
}

void MCObject::GetHeight(MCExecContext& ctxt, uinteger_t& r_value)
{
	integer_t t_value;
	GetRectValue(ctxt, false, P_HEIGHT, t_value);
	r_value = (uinteger_t)t_value;
}
void MCObject::SetHeight(MCExecContext& ctxt, uinteger_t value)
{
	SetRectValue(ctxt, false, P_HEIGHT, value);
}

void MCObject::GetEffectiveHeight(MCExecContext& ctxt, uinteger_t& r_value)
{
	integer_t t_value;
	GetRectValue(ctxt, true, P_HEIGHT, t_value);
	r_value = (uinteger_t)t_value;
}

void MCObject::SetEffectiveHeight(MCExecContext& ctxt, uinteger_t value)
{
	SetRectValue(ctxt, true, P_HEIGHT, value);
}

void MCObject::GetTopLeft(MCExecContext& ctxt, MCPoint& r_location)
{
	GetRectPoint(ctxt, false, P_TOP_LEFT, r_location);
}

void MCObject::SetTopLeft(MCExecContext& ctxt, MCPoint location)
{
	SetRectPoint(ctxt, false, P_TOP_LEFT, location);
}

void MCObject::GetEffectiveTopLeft(MCExecContext& ctxt, MCPoint& r_location)
{
	GetRectPoint(ctxt, true, P_TOP_LEFT, r_location);
}

void MCObject::SetEffectiveTopLeft(MCExecContext& ctxt, MCPoint location)
{
	SetRectPoint(ctxt, true, P_TOP_LEFT, location);
}

void MCObject::GetTopRight(MCExecContext& ctxt, MCPoint& r_location)
{
	GetRectPoint(ctxt, false, P_TOP_RIGHT, r_location);
}

void MCObject::SetTopRight(MCExecContext& ctxt, MCPoint location)
{
	SetRectPoint(ctxt, false, P_TOP_RIGHT, location);
}

void MCObject::GetEffectiveTopRight(MCExecContext& ctxt, MCPoint& r_location)
{
	GetRectPoint(ctxt, true, P_TOP_RIGHT, r_location);
}

void MCObject::SetEffectiveTopRight(MCExecContext& ctxt, MCPoint location)
{
	SetRectPoint(ctxt, true, P_TOP_RIGHT, location);
}

void MCObject::GetBottomLeft(MCExecContext& ctxt, MCPoint& r_location)
{
	GetRectPoint(ctxt, false, P_BOTTOM_LEFT, r_location);
}

void MCObject::SetBottomLeft(MCExecContext& ctxt, MCPoint location)
{
	SetRectPoint(ctxt, false, P_BOTTOM_LEFT, location);
}

void MCObject::GetEffectiveBottomLeft(MCExecContext& ctxt, MCPoint& r_location)
{
	GetRectPoint(ctxt, true, P_BOTTOM_LEFT, r_location);
}

void MCObject::SetEffectiveBottomLeft(MCExecContext& ctxt, MCPoint location)
{
	SetRectPoint(ctxt, true, P_BOTTOM_LEFT, location);
}

void MCObject::GetBottomRight(MCExecContext& ctxt, MCPoint& r_location)
{
	GetRectPoint(ctxt, false, P_BOTTOM_RIGHT, r_location);
}

void MCObject::SetBottomRight(MCExecContext& ctxt, MCPoint location)
{
	SetRectPoint(ctxt, false, P_BOTTOM_RIGHT, location);
}

void MCObject::GetEffectiveBottomRight(MCExecContext& ctxt, MCPoint& r_location)
{
	GetRectPoint(ctxt, true, P_BOTTOM_RIGHT, r_location);
}

void MCObject::SetEffectiveBottomRight(MCExecContext& ctxt, MCPoint location)
{
	SetRectPoint(ctxt, true, P_BOTTOM_RIGHT, location);
}

void MCObject::GetRectangle(MCExecContext& ctxt, MCRectangle& r_rect)
{
	r_rect = getrectangle(false);
}

void MCObject::SetRectangle(MCExecContext& ctxt, MCRectangle p_rect)
{
	SetRectProp(ctxt, false, p_rect);
}

void MCObject::GetEffectiveRectangle(MCExecContext& ctxt, MCRectangle& r_rect)
{
	r_rect = getrectangle(true);
}

void MCObject::SetEffectiveRectangle(MCExecContext& ctxt, MCRectangle p_rect)
{
	SetRectProp(ctxt, true, p_rect);
}

////////////////////////////////////////////////////////////////////////////////

void MCObject::GetEncoding(MCExecContext& ctxt, intenum_t& r_encoding)
{
	r_encoding = hasunicode() ? 1 : 0;
}

////////////////////////////////////////////////////////////////////////////////
