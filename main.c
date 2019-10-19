/******************************************************************************
* File Name:  main.c
*
* Description:
* This is a test program of JSON lint.  It demonstrates the ability of the
* JSON lint module and features that indicate result and where and error is
* located.
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
* Notes:	None.
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
* Variables
******************************************************************************/
uint8_t my_string[100] =	"   {\"my test\"   :9.e54,\"me\":null5,\"you\":["\
							"true	,false]} ";

const uint8_t *tc_json[][2] = { {"false", RESULT_JSON_LINT_SUCCESS},
{"false true",RESULT_JSON_LINT_INVALID},
{"\"test string\"", RESULT_JSON_LINT_SUCCESS},
{"3.44E44", RESULT_JSON_LINT_SUCCESS} ,
{"0e9", RESULT_JSON_LINT_SUCCESS},
{"0000", RESULT_JSON_LINT_INVALID},
{"[123,1.43,2345.34343434343,4E45343]", RESULT_JSON_LINT_SUCCESS},
{"[true, false,		null]", RESULT_JSON_LINT_SUCCESS},
{"{ \"test 1\":3.4}true", RESULT_JSON_LINT_INVALID} ,
{"{		\"test\":\"4k4k4\"}[false,false]", RESULT_JSON_LINT_INVALID},
{"",RESULT_JSON_LINT_SUCCESS} /*end of tests indicator*/ };

/******************************************************************************
* Function Prototypes
******************************************************************************/
void JSON_Test_Cases();

/******************************************************************************
* Function Name:  main
*
* Description:
* Test the JSON lint module by running some test cases that fail and succeed to
* determine that the result will be correct.  The second part of the test shows
* that a JSON string with an error can be indicated where the error is located
* within the string.
*
* Parameters:	None.
*
* Return Value:	None.
*
* Notes:	None.
*
******************************************************************************/
int main() {
	json_lint_result_t result;
	uint32_t error_start = 10;
	uint32_t error_end = 10;
	uint8_t error_text[22] = { 0 };
	uint8_t error_indicator[22] = { 0 };
	uint32_t i;

	JSON_Test_Cases();

	printf("\r\nTest with an error...");
	printf("\r\nJSON string:\r\n%s\r\n\r\n", my_string);
	result = LintJSON(&my_string[0],false);
	switch (result) {
	case RESULT_JSON_LINT_SUCCESS:
		printf("Sucessfully parsed JSON text.\r\n");
		break;
	case RESULT_JSON_LINT_INVALID:
		if (abs(ptr_invalid_json - &my_string) < 10) {
			error_start = abs(ptr_invalid_json - &my_string);
		}
		if (((&my_string + strlen(my_string)) - ptr_invalid_json) < 10) {
			error_end = ((my_string + strlen(my_string)) - ptr_invalid_json);
		}
		if (error_start + error_end > 0) {
			memcpy(&error_text, ptr_invalid_json -error_start, error_start + error_end);
			error_text[error_start + error_end] = 0x00;
		}
		for (i = 0; i < error_start ; i++) {
			error_indicator[i] = '_';
		}
		error_indicator[i] = '^';
		error_indicator[i + 1] = 0x00;
		printf("Error, issue found at: %s\r\n", error_text);
		printf("                       %s\r\n", error_indicator);
		break;
	default:
		break;
	}
}

/******************************************************************************
* Function Name:  JSON_Test_Cases
*
* Description:
* This test case checks that different values can be parsed both correct and
* wrong and verifies that it matches the expected result.
*
* Parameters:	None.
*
* Return Value:	None.
*
* Notes:	None.
*
******************************************************************************/
void JSON_Test_Cases() {
	json_lint_result_t lint_result;
	uint8_t index = 0;
	bool test_result = true;

	printf("Test cases:  ");
	while (strlen(tc_json[index][0]) > 0) {
		lint_result = LintJSON(tc_json[index][0], false);
		if (lint_result != tc_json[index][1]) {
			printf("FAIL test %i\r\n", index);
			test_result = false;
		}
		index++;
	}
	if (test_result) {
		printf("PASS\r\n");
	}

}


