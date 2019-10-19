/******************************************************************************
* File Name:  JSONLint.h
*
* Description:
* Provides JSON linting with return of status of json text.  If there is an
* error, the index of the location of the error within the string is available.
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
* The Linting of JSON is preformed according to the ECMA-404 standard, 2nd
* edition.  Any deviations or details added to the standard are listed below:
*		- None.
* 
* References:
* The JSON Data Interchange Syntax, ECMA-404, 2nd Edition, December 2017
* http://www.ecma-international.org/publications/files/ECMA-ST/ECMA-404.pdf
* 
******************************************************************************/
#ifndef JSON_LINT_H_
#define JSON_LINT_H_

/******************************************************************************
* Includes
******************************************************************************/
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

/******************************************************************************
* Type Definitions
******************************************************************************/
typedef enum {
	RESULT_JSON_LINT_SUCCESS,
	RESULT_JSON_LINT_INVALID,
} json_lint_result_t;

/******************************************************************************
* Variables
******************************************************************************/
uint8_t *ptr_invalid_json;		//pointer to invalid json, if LintJSON() result
								//is RESULT_JSON_LINT_INVALID

/******************************************************************************
* Function Prototypes
******************************************************************************/
json_lint_result_t LintJSON(uint8_t *ptr_text, bool disp_messages);

#endif
