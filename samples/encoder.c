/**-------------------------------------------------------------------------***
*** Package     : d'Arc - fast Lua sub API                                  ***
*** File        : encoder.c                                                 ***
*** Description : macros for VM value and table node access, Lua & LuaJIT   ***
*** Version     : 0.2.0 / alpha                                             ***
*** Requirement : Lua 5.1.4 or LuaJIT 2 beta 6+                             ***
*** Copyright   : April 1st 2011 Henning Diedrich                           ***
*** Author      : H. Diedrich <hd2010@eonblast.com>                         ***
*** License     : see file LICENSE                                          ***
*** Created     : 23 Mar 2011                                               ***
*** Changed     : 01 Apr 2011                                               ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  d'Arc is a faster way to access Lua values and traverse tables in C.   ***
***  It supports Lua 5.1.4 and LuaJIT 2 beta 6 and is loathable for not     ***
***  using the Lua API. Please use it only if you are 18+ years old.        ***
***                                                                         ***
***-------------------------------------------------------------------------***
***                                                                         ***
***  This file contains a sketchy sample that applies the darc macros.      ***
***  Works with both the classic PUC Lua and Mike Pall's LuaJIT.            ***
***                                                                         ***
***-------------------------------------------------------------------------**/

/*****************************************************************************
***                                                                         ***
***                            VALUE ENCODER                                ***
***                                                                         ***
 ****************************************************************************/ 
/*                                                                           */
/*         THIS IS WORKING SOURCE BUT CITED HERE FOR-READING.                */
/*            IT IS NOT RUNNABLE WITHOUT MORE CONTEXT.                       */

#include "darc.h"

#define stringify_value(_ctrl, _o)  \
{  \
	/* TODO: make switch */ \
    if (XLUA_IS_NIL(_o))  \
    {  \
		buffer_add_string(_ctrl, LIT_NULL, 4);  \
    }  \
    else if (XLUA_IS_BOOLEAN(_o))  \
    {  \
		if(XLUA_IS_TRUE(_o)) /* apply to booleans only */ {  \
			buffer_add_string(_ctrl, LIT_TRUE, 4); /* TODO fix? */ \
		}  \
		else {  \
			buffer_add_string(_ctrl, LIT_FALSE, 5); /* TODO fix? */ \
		}  \
    }  \
    else if (XLUA_IS_NUMBER(_o))  \
    {  \
		int i = (int)XLUA_NUMBER(_o);  \
		if(((double)i) == (XLUA_NUMBER(_o))) {  \
			/* as integer */ \
			buffer_assert(_ctrl, 14);  \
		    size_t len;  \
		    itoa(i, dynp(_ctrl->parts_list_last), &len);  \
		    _ctrl->parts_list_last->len += len;  \
		    _ctrl->total_len += len;  \
		}  \
		else {  \
			/* as float */ \
		    buffer_assert(_ctrl, 34);  \
		    size_t len;   \
   		    dtoa(XLUA_NUMBER(_o),dynp(_ctrl->parts_list_last),FLEECE_DEFAULT_PRECISION,'g', &len); /* TODO: flags */ \
		    _ctrl->parts_list_last->len += len;  \
		    _ctrl->total_len += len;  \
		}  \
	}  \
    else if (XLUA_IS_STRING(_o))  \
    {  \
/*-		buffer_fadd5(_ctrl, // TODO: MAKE AT LEAST THIS FIX. TO SAVE BIG.  \
			pre, '"', svalue(_o), '"', post, tsvalue(_o)->len);		 -*/ \
		buffer_add_char_blindly(_ctrl, '"');  \
		/*- printf("about to add %s (%d) to %s\n", XLUA_STRING(_o), XLUA_STRING_LENGTH(_o), (char *)(_ctrl->parts_list_last + 1)); -*/ \
		buffer_add_string_possibly_escaped(_ctrl, XLUA_STRING(_o), XLUA_STRING_LENGTH(_o));  \
		/*- printf("added: %s\n", (char *)(_ctrl->parts_list_last + 1)); -*/ \
		buffer_add_char_blindly(_ctrl, '"');  \
    }  \
    else if (XLUA_IS_TABLE(_o))  \
    {  \
		buffer_add_char_safely(_ctrl, '{');  \
		char *bracket = dynp(_ctrl->parts_list_last) -1;  \
 \
		size_t count = 1;  \
		int pure = 1;  \
		int tried = 0;  \
		int force = 0;  \
		insp_ctrl store;  \
		/* TODO: assert? to avoid running into a fix here? does that matter? */ \
		memcpy(&store, _ctrl, sizeof(insp_ctrl));  \
		size_t store_parts_buf_len = _ctrl->parts_list_last->len;  \
		stringify_array_part(_ctrl, XLUA_TABLE(_o), &count, &pure, &tried, force);  \
		if(!tried || pure) stringify_hash_part(_ctrl, XLUA_TABLE(_o), &count, &pure);  \
		/* redo when started out w/o index but then encountered one one the way */ \
		if(tried && !pure) {  \
			/* rewind writes*/ \
			buffer_rewind(_ctrl, &store, store_parts_buf_len);  \
			count = 1; force = 1;  \
			stringify_array_part(_ctrl, XLUA_TABLE(_o), &count, &pure, &tried, force);  \
			stringify_hash_part(_ctrl, XLUA_TABLE(_o), &count, &pure);  \
			pure = 0; /* switch for below */ \
		}  \
		if(count == 1) /* empty array = always array, not object */ {  \
			buffer_add_char_safely(_ctrl, ']');  \
			*bracket='[';  \
		}  \
		else if(pure) /* array, not empty */ {  \
			/* writes over last ',' */ \
			*(dynp(_ctrl->parts_list_last) -1)=']';  \
			/* (*) must therefore assert buffer, as could be part of ]]]... */ \
			buffer_assert(_ctrl, 0); /* sic */ \
			*bracket='[';  \
		}  \
		else /* object */ {  \
			/* writes over last ',' */ \
			*(dynp(_ctrl->parts_list_last) -1)='}';  \
			/* (*) must therefore assert buffer, as could be part of }}}... */ \
			buffer_assert(_ctrl, 0); /* sic */ \
		}  \
    }  \
    else if (XLUA_IS_FUNCTION(_o))  \
    {  \
	    stringify_placeholder(_ctrl, LIT_FUNCTION, sizeof(LIT_FUNCTION));  \
    }  \
    else if (XLUA_IS_USERDATA(_o))  \
    {  \
	    stringify_placeholder(_ctrl, LIT_USER_DATA, sizeof(LIT_USER_DATA));  \
    }  \
    else if (XLUA_IS_THREAD(_o))  \
    {  \
	    stringify_placeholder(_ctrl, LIT_THREAD, sizeof(LIT_THREAD));  \
    }  \
    else if (XLUA_IS_LIGHTUSERDATA(_o))  \
    {  \
	    stringify_placeholder(_ctrl, LIT_LIGHT_USER_DATA, sizeof(LIT_LIGHT_USER_DATA));  \
    }  \
    else  \
    {  \
	    stringify_placeholder(_ctrl, LIT_TYPELESS, sizeof(LIT_TYPELESS));  \
    }  \
}  \

