/*
 * Example program demonstrating calling functions indirectly through
 * a table of pointers.
 *
 * Although MICRO-C does not have a pointer-to-function data type, it does
 * allow you to call indirectly through any other type of pointer, thereby
 * providing the functionality of pointers-to-functions.
 */

#include cflea.h

/* DEMO functions to place in table (defined BEFORE table) */
char *func0() {	return "Zero";  }
char *func1() {	return "One";   }
char *func2() { return "Two";   }
char *func3() { return "Three"; }
char *func4() { return "Four";  }
char *func5() { return "Five";  }
char *func6() { return "Six";   }
char *func7() { return "Seven"; }
char *func8() { return "Eight"; }
char *func9() { return "Nine";  }

/*
Note that in order to initialize a variable with the address of
a symbol, that symbol must be already defined. If you wish to
use functions (or variables) which are defined further down in
the source file, you can use "extern" to define and prototype
the symbol in advance.
*/
extern char *func10();

/*
Table of pointers to functions. Note that one level of indirection
is used to access the function. Therefore, if we want functions which
return pointers-to-char (char *), we have to declare the table as
pointers-to-pointers-to-char (char **)
*/
char **func_table[] = { &func0, &func1, &func2, &func3,
	&func4, &func5, &func6, &func7, &func8, &func9, &func10 };

/* Simple program to loop, calling each function from table */
main()
{
	int i;
	for(i=0; i < sizeof(func_table)/sizeof(char **); ++i)
		printf("func%u() = '%s'\n", i, (*func_table[i])() );
}

/* Example of function defined AFTER table */
char *func10() { return "Last function in table"; }
