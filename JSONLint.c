/******************************************************************************
* File Name:  JSONLint.c
*
* Description:
* Implementation of a JSON lint utility as defined in ECMA-404.
* 
* LICENSE:
* MIT License
*
* Copyright (c) 2019 EmbeddedWilderness
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
* Notes:
* There is not need for initialization function as everything is setup in the 
* ParseJSON function.
* 
******************************************************************************/

/******************************************************************************
* Includes
******************************************************************************/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "JSONLint.h"

/******************************************************************************
* Defines
******************************************************************************/
#define CHAR_HORIZONTAL_TAB		0x09
#define CHAR_CARRIAGE_RETURN		0x0A
#define CHAR_LINEFEED			0x0D
#define CHAR_SPACE			0x20

#define STRING_OBJECT_START		"{"
#define STRING_ARRAY_START		"["
#define STRING_STRING_START		"\""
#define STRING_OBJECT_STOP		"}"
#define STRING_ARRAY_STOP		"]"
#define STRING_STRING_STOP		"\""
#define STRING_COLON			":"
#define STRING_COMMA			","
#define STRING_BACKSLASH		"\\"
#define STRING_SIGN_POS			"+"
#define STRING_SIGN_NEG			"-"
#define STRING_DECIMAL			"."
#define STRING_0			"0"

#define STRING_FALSE			"false"
#define STRING_TRUE			"true"
#define STRING_NULL			"null"

#define STRING_ESCAPE_CHARS		"\"\\/bfnrtu"
#define STRING_HEX_CHARS		"0123456789abcdefABCDEF"
#define STRING_EXPONENT_DIGITS	"eE"
#define STRING_DIGIT_1_9		"123456789"
#define STRING_DIGIT_0_9		"0123456789"

/******************************************************************************
* Type Definitions
******************************************************************************/
typedef enum {
	RESULT_JSON_SUCCESS,
	RESULT_JSON_INVALID,
	RESULT_JSON_NOT_FOUND,
} json_result_t;

/******************************************************************************
* Function Prototypes
******************************************************************************/
void ProcessWhitespace(uint8_t **index);
json_result_t ProcessValue(uint8_t **index);
json_result_t ProcessObject(uint8_t **index);
json_result_t ProcessArray(uint8_t **index);
json_result_t ProcessString(uint8_t **index);
json_result_t ProcessNumber(uint8_t **index);

/******************************************************************************
* Functions
******************************************************************************/
/******************************************************************************
* Function Name:  LintJSON
*
* Description:
* Lint the JSON text as a recursive tree.  The tree starts with a JSON 
* element which is a value with whitespace on either side.  If the JSON text 
* is empty, the invalid result is returned and pointer to json error is set.
* 
* Parameters:
* ptr_text		uint8_t *		pointer to starting text to lint
* disp_message	bool			1=display the printf messages
* 
* Return Value: 
* json_lint_result_t		result of parsing to calling application
*
* Notes:	None.
* 
******************************************************************************/
json_lint_result_t LintJSON(uint8_t * ptr_text, bool disp_messages) {
	json_result_t internal_result = RESULT_JSON_SUCCESS;
	json_lint_result_t result = RESULT_JSON_LINT_SUCCESS;
	uint8_t *ptr_text_start = ptr_text;

	ptr_invalid_json = NULL;

	if (disp_messages)
		printf("Starting JSON parsing...");
	if (*ptr_text != NULL) {	//check if string exists
		ProcessWhitespace(&ptr_text);
		internal_result = ProcessValue(&ptr_text);
		if (internal_result == RESULT_JSON_SUCCESS) {
			ProcessWhitespace(&ptr_text);
			//check for the end of string is the actual end and no extra text is there
			if (strlen(ptr_text_start) != (ptr_text - ptr_text_start)) {
				internal_result = RESULT_JSON_INVALID;
				ptr_invalid_json = ptr_text;
			} else {
				if (disp_messages)
					printf("Finished.\r\n");
			}
		} else {
			if (disp_messages)
				printf("\r\n");
		}
	}else {
		if (disp_messages)
			printf("No JSON to parse.\r\n");
		internal_result = RESULT_JSON_INVALID;
		ptr_invalid_json = ptr_text;
	}

	//convert internal result typedef to the external one
	if (internal_result == RESULT_JSON_SUCCESS) {
		result = RESULT_JSON_LINT_SUCCESS;
	} else {
		result = RESULT_JSON_LINT_INVALID;
	}
	
	return result;
}

/******************************************************************************
* Function Name:  ProcessWhitespace
*
* Description:
* Increment the pointer as valid whitespace is found (space, line feed, 
* carriage return, tab).
*
* Parameters:
* index		uint8_t **		pointer to starting pointer of text to lint
*
* Return Value:	None.
*
* Notes:	None.
*
******************************************************************************/
void ProcessWhitespace(uint8_t **index) {
	bool whitespace_end = false;

	while (!whitespace_end) {
		switch (**index) {
		case CHAR_SPACE:
		case CHAR_LINEFEED:
		case CHAR_CARRIAGE_RETURN:
		case CHAR_HORIZONTAL_TAB:
			(*index)++;
			break;
		default:
			whitespace_end = true;
			break;
		}
	}
}

/******************************************************************************
* Function Name:  ProcessValue
*
* Description:
* Check the 7 possible values.  Increment the pointer if it is one of the 3
* literal name tokens (true, false, null).  If it is an object, string or 
* array, call the processing function to continue processing the value.  If the
* value encountered is none of the 7 possibilities, set as invalid JSON.
*
* Parameters:
* index		uint8_t **		pointer to a pointer of starting text to lint
*
* Return Value:
* json_result_t		RESULT_JSON_SUCCESS - the value has been parsed
*					RESULT_JSON_NOT_FOUND - there is no value
*					RESULT_JSON_INVALID - the text does not follow JSON
*
* Notes:
* If the result of processing is invalid, the pointer ptr_invalid_json will be
* set to the location of the error.
*
******************************************************************************/
json_result_t ProcessValue(uint8_t **index) {
	json_result_t result = RESULT_JSON_SUCCESS;

	if ((result = ProcessObject(index)) != RESULT_JSON_NOT_FOUND ) {

	}else if ((result = ProcessArray(index)) != RESULT_JSON_NOT_FOUND) {

	} else if ((result = ProcessString(index)) != RESULT_JSON_NOT_FOUND) {

	}else if ((result=ProcessNumber(index)) != RESULT_JSON_NOT_FOUND){

	}else if (strncmp(*index, STRING_FALSE, strlen(STRING_FALSE)) == 0) {
		(*index) += strlen(STRING_FALSE);
		result = RESULT_JSON_SUCCESS;
	}else if (strncmp(*index, STRING_TRUE, strlen(STRING_TRUE)) == 0) {
		(*index) += strlen(STRING_TRUE);
		result = RESULT_JSON_SUCCESS;
	}else if (strncmp(*index, STRING_NULL, strlen(STRING_NULL)) == 0) {
		(*index) += strlen(STRING_NULL);
		result = RESULT_JSON_SUCCESS;
	}else{
		result = RESULT_JSON_INVALID;
		ptr_invalid_json = *index;
	}

	return result;
}

/******************************************************************************
* Function Name:  ProcessObject
*
* Description:
* Search for the object open curly bracket token to start the object.  If found
* then search for 0 or more string value pairs.
* 
* Parameters:
* index		uint8_t **		pointer to a pointer of starting text to lint
*
* Return Value:
* json_result_t		RESULT_JSON_SUCCESS - the object has been parsed
*					RESULT_JSON_NOT_FOUND - there is no object
*					RESULT_JSON_INVALID - the text does not follow JSON
*
* Notes:
* If the result of processing is invalid, the pointer ptr_invalid_json will be
* set to the location of the error.
*
******************************************************************************/
json_result_t ProcessObject(uint8_t **index) {
	json_result_t result = RESULT_JSON_SUCCESS;
	uint8_t *key_name;
	uint8_t *key_position;
	uint8_t key_size;

	if (strncmp(*index, STRING_OBJECT_START, strlen(STRING_OBJECT_START)) == 0 ) {
		(*index) += strlen(STRING_OBJECT_START);
		ProcessWhitespace(index);
		while (!strncmp(*index, STRING_OBJECT_STOP, strlen(STRING_OBJECT_STOP)) == 0 
			&& result == RESULT_JSON_SUCCESS) {
			if ((result = ProcessString(index)) == RESULT_JSON_SUCCESS) {
				ProcessWhitespace(index);
				if (strncmp(*index, STRING_COLON, strlen(STRING_COLON)) == 0) {
					(*index) ++;
					if ((result=ProcessValue(index)) == RESULT_JSON_SUCCESS) {
						if (strncmp(*index, STRING_COMMA, strlen(STRING_COMMA)) == 0) {
							(*index) ++;
						}else if (strncmp(*index, STRING_OBJECT_STOP, strlen(STRING_OBJECT_STOP)) == 0) {

						}else {
							result = RESULT_JSON_INVALID;
							ptr_invalid_json = *index;
						}
					}
				}else {
					result = RESULT_JSON_INVALID;
					ptr_invalid_json = *index;
				}
			}else {
				result = RESULT_JSON_INVALID;
				ptr_invalid_json = *index;
			}
		}
		(*index) += strlen(STRING_OBJECT_STOP);
	} else {
		result = RESULT_JSON_NOT_FOUND;
	}

	return result;
}

/******************************************************************************
* Function Name:  ProcessArray
*
* Description:
* Search for the object open square bracket token to start the array.  If 
* found then search for zero or more values.
*
* Parameters:
* index		uint8_t **		pointer to a pointer of starting text to lint
*
* Return Value:
* json_result_t		RESULT_JSON_SUCCESS - the array has been parsed
*					RESULT_JSON_NOT_FOUND - there is no array
*					RESULT_JSON_INVALID - the text does not follow JSON
*
* Notes:
* If the result of processing is invalid, the pointer ptr_invalid_json will be
* set to the location of the error.
*
******************************************************************************/
json_result_t ProcessArray(uint8_t **index) {
	json_result_t result = RESULT_JSON_SUCCESS;

	if (strncmp(*index, STRING_ARRAY_START, strlen(STRING_ARRAY_START)) == 0) {
		(*index) += strlen(STRING_ARRAY_START);
		ProcessWhitespace(index);
		while (!strncmp(*index, STRING_ARRAY_STOP, strlen(STRING_ARRAY_STOP)) == 0
			&& result == RESULT_JSON_SUCCESS) {
			if ((result = ProcessValue(index)) == RESULT_JSON_SUCCESS) {
				ProcessWhitespace(index);
				if (strncmp(*index, STRING_COMMA, strlen(STRING_COMMA)) == 0) {
					(*index)++;
					ProcessWhitespace(index);
				} else if (strncmp(*index, STRING_ARRAY_STOP, strlen(STRING_ARRAY_STOP)) == 0) {

				} else {
					result = RESULT_JSON_INVALID;
					ptr_invalid_json = *index;
				}
			}
		}
		if (result == RESULT_JSON_SUCCESS) {
			(*index) += strlen(STRING_ARRAY_STOP);
		}
	} else {
		result = RESULT_JSON_NOT_FOUND;
	}

	return result;
}

/******************************************************************************
* Function Name:  ProcessString
*
* Description:
* Search for the start of a string --> ".  If found then process, including
* escape characters.
*
* Parameters:
* index		uint8_t **		pointer to a pointer of starting text to lint
*
* Return Value:
* json_result_t		RESULT_JSON_SUCCESS - the string has been parsed
*					RESULT_JSON_NOT_FOUND - there is no string
*					RESULT_JSON_INVALID - the text does not follow JSON
*
* Notes:
* If the result of processing is invalid, the pointer ptr_invalid_json will be
* set to the location of the error.
*
******************************************************************************/
json_result_t ProcessString(uint8_t **index) {
	json_result_t result = RESULT_JSON_SUCCESS;

	if (strncmp(*index, STRING_STRING_START, strlen(STRING_STRING_START)) == 0) {
		(*index) += strlen(STRING_STRING_START);
		while (!strncmp(*index, STRING_STRING_STOP, strlen(STRING_STRING_STOP)) == 0
			&& result == RESULT_JSON_SUCCESS) {
			if ((*index)[0] == '\\') {
				(*index)++;
				if (strchr(STRING_ESCAPE_CHARS, (*index)[0])) {
					if (*(index)[0] == 'u') {
						(*index)++;
						if (strchr(STRING_HEX_CHARS, (*index)[0]) &&
							strchr(STRING_HEX_CHARS, (*index)[1]) &&
							strchr(STRING_HEX_CHARS, (*index)[2]) &&
							strchr(STRING_HEX_CHARS, (*index)[3])) {
							(*index) += 4;
						} else {
							result = RESULT_JSON_INVALID;
							ptr_invalid_json = *index;
						}
					} else {
						(*index)++;
					}
				} else {
					result = RESULT_JSON_INVALID;
					ptr_invalid_json = *index;
				}
			} else {
				(*index)++;
			}
		}
		if (result == RESULT_JSON_SUCCESS) {
			(*index) += strlen(STRING_STRING_STOP);
		}
	} else {
		result = RESULT_JSON_NOT_FOUND;
	}

	return result;
}

/******************************************************************************
* Function Name:  ProcessNumber
*
* Description:
* Process the number defined in the JSON specification.
* 
* Parameters:
* index		uint8_t **		pointer to a pointer of starting text to lint
*
* Return Value:
* json_result_t		RESULT_JSON_SUCCESS - the number has been parsed
*					RESULT_JSON_NOT_FOUND - there is no number
*					RESULT_JSON_INVALID - the text does not follow JSON
*
* Notes:
* If the result of processing is invalid, the pointer ptr_invalid_json will be
* set to the location of the error.
* 
******************************************************************************/
json_result_t ProcessNumber(uint8_t **index) {
	json_result_t result = RESULT_JSON_SUCCESS;

	if (strncmp(*index, STRING_SIGN_NEG, strlen(STRING_SIGN_NEG)) == 0
		|| strncmp(*index, STRING_0, strlen(STRING_0))==0
		|| strchr(STRING_DIGIT_1_9, (*index)[0])) {
		if(strncmp(*index, STRING_SIGN_NEG, strlen(STRING_SIGN_NEG)) == 0) {
			(*index)++;
			if (strchr(STRING_DIGIT_1_9, (*index)[0])) {
				(*index++);
				while (strchr(STRING_DIGIT_0_9, (*index)[0]) && strlen((*index))) {
					(*index)++;
				}
			}else if(strncmp(*index, STRING_0, strlen(STRING_0)) == 0){
				(*index)++;
			} else {
				result = RESULT_JSON_INVALID;
				ptr_invalid_json = *index;
			}
		} else if (strchr(STRING_DIGIT_1_9, (*index)[0])) {
			(*index)++;
			while (strchr(STRING_DIGIT_0_9, (*index)[0]) && strlen((*index))) {
				(*index)++;
			}
		} else {	//its zero
			(*index)++;
		}
		if (strncmp(*index, STRING_DECIMAL, strlen(STRING_DECIMAL)) == 0){
			(*index)++;
			while (strchr(STRING_DIGIT_0_9, (*index)[0]) && strlen((*index))) {
				(*index)++;
			}
		}
		if(strchr(STRING_EXPONENT_DIGITS, (*index)[0]) && strlen((*index))) {
			(*index)++;
			if (strncmp((*index), STRING_SIGN_POS, strlen(STRING_SIGN_POS)) == 0
				|| strncmp((*index), STRING_SIGN_NEG, strlen(STRING_SIGN_NEG)) == 0
				|| strchr(STRING_DIGIT_0_9, (*index)[0])) {
				(*index)++;
				while (strchr(STRING_DIGIT_0_9, (*index)[0]) && strlen((*index))) {
					(*index)++;
				}
			} else {
				result = RESULT_JSON_INVALID;
				ptr_invalid_json = *index;
			}
		}
	}else{
		result = RESULT_JSON_NOT_FOUND;
	}

	return result;
}

