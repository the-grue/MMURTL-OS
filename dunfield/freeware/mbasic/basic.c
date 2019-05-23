/*
 * MICRO-BASIC:
 *
 * This is a very simple INTEGER BASIC interpreter that I wrote a number
 * of years ago, and subsequently ported to MICRO-C. While not a great
 * example of coding style (it was a quick and dirty hack job), It is
 * quite instructive, as a simple but fairly complete interpreter.
 *
 * Variables:
 * 	260 Numeric   variables	:	A0-A9 ... Z0-Z9
 * 	260 Character variables	:	A0$-A9$ ... Z0$-Z9$
 *	260 Numeric arrays		:	A0()-A9() ... Z0()-Z9()
 * 
 *	For convenience the '0' variables can be referenced by letter
 *	only. IE: A is equivalent to A0 ... Z$ is equivalent to Z0$
 * 
 * Statements:
 * 	CLEAR					- Erase variables only
 * 	CLOSE#n					- Close file (0-9) opened with OPEN
 *	DATA					- Enter "inline" data statements
 *	DIM var(size)[, ... ]	- Dimension an array
 * 	END						- Terminate program with no message
 * 	EXIT					- Terminate MICRO-BASIC
 * 	FOR v=init TO limit [STEP increment] - Perform a counted loop
 * 	GOSUB line				- Call a subroutine
 * 	GOTO  line				- Jump to line
 * 	IF test THEN line		- Conditional goto
 * 	IF test THEN statement	- Conditional statement (next statement only)
 * 	INPUT var				- Get value for variable
 * 	INPUT "prompt",var		- Get value of variable with prompt
 *		prompt must be a constant string, however you can use a char variable
 *		in prompt by concatinating it to such a string: INPUT ""+a$,b$
 * 	INPUT#n,var				- Get value for variable from file (0-9)
 * 	LET (default) 			- variable = expression
 * 	LIF test THEN statements- LONG IF (all statements to end of line)
 * 	LIST [start,[end]]		- List program lines
 * 	LIST#n ...				- List program to file (0-9)
 *  LOAD name               - Load program from disk file
 *		When LOAD is used within a program, execution continues with the
 *		first line of the newly loaded program. In this case, the user
 *		variables are NOT cleared. This provides a means of chaining
 *		to a new program, and passing information to it.
 *		Also note that LOAD must be the LAST statement on a line.
 * 	NEW						- Erase program and variables
 * 	NEXT [v]				- End counted loop
 * 	OPEN#n,"name","opts"	- Open file (0-9), opts are same as "fopen()"
 *	ORDER line				- Position data read pointer
 * 	PRINT expr[,expr ...]	- Print to console
 * 	PRINT#n ...				- Print to file (0-9)
 *	READ var[,var ...]		- Read data from program statements
 *		You MUST issue an "ORDER" statement targeting a line
 *		containing a valid DATA statement before using READ
 * 	RETURN					- Return from subroutine
 * 	REM						- Comment... reminder of line is ignored
 * 	RUN [line]				- Run program
 *  SAVE [name]             - Save program to disk file
 * 	STOP					- Terminate program & issue message
 * 
 * Operators:
 * 	+						- Addition, string concatination
 * 	-						- Unary minus, subtraction
 * 	*, /, %,				- multiplication, division, modulus
 * 	&, |, ^					- AND, OR, Exclusive OR
 * 	=, <>					- Assign/test equal, test NOTequal (num or string)
 * 	<, <=, >, >=			- LT, LE, GT, GE (numbers only)
 * 	!						- Unary NOT
 *		The "test" operators (=, <>, <, <=, >, >=) can be used in any
 *		expression, and evaluate to 1 of the test is TRUE, and 0 if it
 *		is FALSE. The IF and LIF commands accept any non-zero value to
 *		indicate a TRUE condition.
 * 
 * Functions:
 * 	CHR$(value)				- Returns character of passed value
 * 	STR$(value)				- Returns ASCII string of value's digits
 * 	ASC(char)				- Returns value of passed character
 * 	ABS(value)				- Returns absolute value of argument
 *  RND(value)              - Returns random number from 0 to (value-1)
 *
 * When porting to a different compiler, take note:
 *
 *	-	Make sure 'fgets' does not include the trailing NEWLINE
 *		You can use "microc.h" from utilities source directory.
 *
 *	-	Make sure that 'isalpha' and 'isdigit' can deal with negative
 *		character values (most macro implementations can't). If not,
 *		include those functions from the MICRO-C library.
 *
 *	-	Modify the declaration of 'savjmp' to whatever is appropriate
 *		for your compilers 'setjmp' and 'longjmp' functions.
 *
 * Copyright 1982-1994 Dave Dunfield
 * All rights reserved.
 *
 * Permission granted for personal (non-commercial) use only.
 *
 * Compile command: cc basic -fop
 */
#include <stdio.h>

/* Fixed parameters */
#define BUFFER_SIZE 100		/* input buffer size */
#define NUM_VAR 	260		/* number of variables */
#define SA_SIZE 	100		/* string accumulator size */

/* Control stack constant identifiers */
#define _FOR		1000	/* indicate FOR statement */
#define _GOSUB		_FOR+1	/* indicate GOSUB statement */

/* Primary keywords */
#define	LET		1
#define	EXIT	2
#define	LIST	3
#define	NEW		4
#define	RUN		5
#define	CLEAR	6
#define	GOSUB	7
#define	GOTO	8
#define	RETURN	9
#define	PRINT	10
#define	FOR		11
#define	NEXT	12
#define	IF		13
#define	LIF		14
#define	REM		15
#define	STOP	16
#define	END		17
#define	INPUT	18
#define	OPEN	19
#define	CLOSE	20
#define	DIM		21
#define	ORDER	22
#define	READ	23
#define	DATA	24
#define	SAVE	25
#define	LOAD	26

/* Secondary keywords */
#define	TO		27	/* Also used as marker */
#define	STEP	28
#define	THEN	29

/* Operators and functions */
#define	ADD		30	/* Also used as marker */
#define	SUB		31
#define	MUL		32
#define	DIV		33
#define	MOD		34
#define	AND		35
#define	OR		36
#define	XOR		37
#define	EQ		38
#define	NE		39
#define	LE		40
#define	LT		41
#define	GE		42
#define	GT		43
#define CHR		44
#define	STR		45
#define	ASC		46
#define	ABS		47
#define	RND		48

/* Make sure this token table matches the above definitions */
static char *reserved_words[] = {
	"LET", "EXIT", "LIST", "NEW", "RUN", "CLEAR", "GOSUB", "GOTO",
	"RETURN", "PRINT", "FOR", "NEXT", "IF", "LIF", "REM", "STOP",
	"END", "INPUT", "OPEN", "CLOSE", "DIM", "ORDER", "READ", "DATA",
	"SAVE", "LOAD",
	"TO", "STEP", "THEN",
	"+", "-", "*", "/", "%", "&", "|", "^",
	"=", "<>", "<=", "<", ">=", ">",
	"CHR$(", "STR$(", "ASC(", "ABS(", "RND(",
	0 };

/* Table of operator priorities */
static char priority[] = { 0, 1, 1, 2, 2, 2, 3, 3, 3, 1, 1, 1, 1, 1, 1 };

/* Table of error messages */
static char *error_messages[] = {
	"Syntax",
	"Illegal program",
	"Illegal direct",
	"Line number",
	"Wrong type",
	"Divide by zero",
	"Nesting",
	"File not open",
	"File already open",
	"Input",
	"Dimension",
	"Data",
	"Out of memory"
	};

struct line_record {
	unsigned Lnumber;
	struct line_record *Llink;
	char Ltext[]; };

char sa1[SA_SIZE], sa2[SA_SIZE];		/* String accumulators */
struct line_record *pgm_start,			/* Indicates start of program */
	*runptr,							/* Line we are RUNnning */
	*readptr;							/* Line we are READing */

unsigned dim_check[NUM_VAR];			/* Check dim sizes for arrays */

FILE *filein, *fileout;					/* File I/O active pointers */

int savjmp[3];							/* Save area for set/longjmp */

/* Misc. global variables */
char *cmdptr,							/* Command line parse pointer */
	*dataptr,							/* Read data pointer */
	buffer[BUFFER_SIZE],				/* General input buffer */
	mode = 0,							/* 0=Stopped, !0=Running */
	expr_type,							/* Type of last expression */
	nest;								/* Nest level of expr. parser */
unsigned line,							/* Current line number */
	ctl_ptr = 0,						/* Control stack pointer */
	ctl_stk[50];						/* Control stack */

/*
 * The following variables must be iniitialized to zero. If your
 * compiler does not automatically zero uninitialized global
 * variables, modify these declarations to initialize them.
 */
char filename[65];						/* Name of program file */
FILE *files[10];						/* File unit numbers */
int num_vars[NUM_VAR];					/* Numeric variables */
int *dim_vars[NUM_VAR];					/* Dimensioned arrays */
char *char_vars[NUM_VAR];				/* Character variables */

/*
 * Test for end of expression
 */
int is_e_end(char c)
{
	if((c >= (0xFF80+TO)) && (c < (0xFF80+ADD)))
		return(1);
	return (c == '\0') || (c == ':') || (c == ')') || (c == ',');
}

/*
 * Test for end of statement
 */
int is_l_end(char c)
{
	return (c == '\0') || (c == ':');
}

/*
 * Test for terminator character
 */
int isterm(char c)
{
	return (c == ' ') || (c == '\t');
}

/*
 * Advance to next non-blank & retreive data
 */
char skip_blank()
{
	while(isterm(*cmdptr))
		++cmdptr;
	return *cmdptr;
}

/*
 * Advance to., retrieve and skip next non blank
 */
char get_next()
{
	char c;

	while(isterm(c=*cmdptr))
		++cmdptr;
	if(c)
		++cmdptr;
	return c;
}

/*
 * Test for a specific character occuring next & remove if found
 */
int test_next(int token)
{
	if(skip_blank() == token) {
		++cmdptr;
		return -1; }
	return 0;
}

/*
 * Expect a specific token - syntax error if not found
 */
expect(int token)
{
	if(get_next() != token)
		error(0);
}

/*
 * Lookup up word from command line in table
 */
unsigned lookup(char *table[])
{
	unsigned i;
	char *cptr, *optr;

	optr = cmdptr;
	for(i=0; cptr = table[i]; ++i) {
		while((*cptr) && (*cptr == toupper(*cmdptr))) {
			++cptr;
			++cmdptr; }
		if(!*cptr) {
			skip_blank();
			return i+1; }
		cmdptr = optr; }
	return 0;
}

/*
 * Get a number from the input buffer
 */
unsigned get_num()
{
	unsigned value;
	char c;

	value = 0;
	while(isdigit(c=*cmdptr)) {
		++cmdptr;
		value = (value * 10) + (c - '0'); }
	return value;
}

/*
 * Allocate memory and zero it
 */
char *allocate(unsigned size)
{
	char *ptr;
	if(!(ptr = malloc(size)))
		error(12);
	memset(ptr, 0, size);
	return ptr;
}

/*
 * Delete a line from the program
 */
delete_line(unsigned lino)
{
	struct line_record *cptr, *bptr;

	if(!(cptr = pgm_start))					/* no lines in pgm */
		return;
	do {
		if(lino == cptr->Lnumber) {			/* we have line to delete */
			if(cptr == pgm_start) {			/* first line in pgm */	
				pgm_start = cptr->Llink;
				return; }
			else {
				bptr->Llink = cptr->Llink;	/* skip it in linked list */
				free(cptr); } }				/* let it go */
		bptr = cptr; }
	while(cptr = cptr->Llink);
}

/*
 * Insert a line into the program
 */
insert_line(unsigned lino)
{
	unsigned i;
	struct line_record *cptr, *bptr, *optr;
	char *ptr;

	ptr = cmdptr;
	for(i=5; *ptr; ++i)
		++ptr;
	bptr = allocate(i);
	bptr->Lnumber = lino;
	for(i=0; *cmdptr; ++i)
		bptr->Ltext[i] = *cmdptr++;
	bptr->Ltext[i] = 0;
	if((!(cptr = pgm_start)) || (lino < cptr->Lnumber)) {	/* at start */
		bptr->Llink = pgm_start;
		pgm_start = bptr; }
	else {				/* inserting into main part of pgm */
		for(;;) {
			optr = cptr;
			if((!(cptr = cptr->Llink)) || (lino < cptr->Lnumber)) {
				bptr->Llink = optr->Llink;
				optr->Llink = bptr;
				break; } } }
}

/*
 * Tokenize input line and Add/Replace a source line if suitable
 */
edit_program()
{
	unsigned value;
	char *ptr, c;

	cmdptr = ptr = buffer;
	/* Translate special tokens into codes */
	while(c = *cmdptr) {
		if(value = lookup(reserved_words))
			*ptr++ = value | 0x80;
		else {
			*ptr++ = c;
			++cmdptr;
			if(c == '"') {		/* double quote */
				while((c = *cmdptr) && (c != '"')) {
					++cmdptr;
					*ptr++ = c; }
				*ptr++ = *cmdptr++; } } }
	*ptr = 0;
	cmdptr = buffer;

	if(isdigit(skip_blank())) {	/* Valid source line */
		value = get_num();		/* Get line number */
		delete_line(value);		/* Delete the old */
		if(skip_blank())
			insert_line(value);
		return -1; }			/* Insert the new */
	return 0;
}

/*
 * Locate given line in source
 */
struct line_record *find_line(unsigned line)
{
	struct line_record *cptr;

	for(cptr = pgm_start; cptr; cptr = cptr->Llink)
		if(cptr->Lnumber == line)
			return cptr;
	error(3);
}

/*
 * Compute variable address for assignment
 */
unsigned *address()
{
	unsigned i, j, *dptr;
	i = get_var();
	if(expr_type)
		return &char_vars[i];
	else {
		if(test_next('(')) {	/* Array */
			if(expr_type) error(0);
			if(!(dptr = dim_vars[i]))
				error(10);
			nest = 0;
			if((j = eval_sub()) >= dim_check[i])
				error(10);
			return &dptr[j]; } }
	return &num_vars[i];
}

/*
 * Execute a BASIC commands
 */
struct line_record *execute(char cmd)
{
	unsigned i, j, k, *dptr;
	int ii, jj;
	struct line_record *cptr;
	char c;

	switch(cmd & 0x7F) {
		case LET :
			dptr = address();
			j = expr_type;

			expect(0xFF80+EQ);

			k = eval();

			if(j != expr_type)
				error(0);
			if(!expr_type)		/* numeric assignment */
				*dptr = k;
			else {				/* character assignment */
				if(*dptr)
					free(*dptr);
				if(*sa1)
					strcpy(*dptr = allocate(strlen(sa1)+1), sa1);
				else
					*dptr = 0; }
			break;
		case EXIT :
			exit(0);
		case LIST :
			chk_file(1);
			if(!isdigit(skip_blank())) {
				i=0; j=-1; }
			else {
				i = get_num();
				if(get_next() == ',') {
					if(isdigit(skip_blank()))
						j=get_num();
					else
						j = -1; }
				else
					j=i; }
			disp_pgm(fileout,i,j);
			break;
		case NEW :
			clear_vars();
			clear_pgm();
			longjmp(savjmp, 1);
		case RUN :
			direct_only();
			if(is_e_end(skip_blank()))
				runptr = pgm_start;
			else
				runptr = find_line(eval_num());
			--mode;			/* indicate running */
			clear_vars();
newline:
			while(runptr) {
				cmdptr = runptr->Ltext;
				line = runptr->Lnumber;
				do {
					if((cmd = skip_blank()) < 0) {
						++cmdptr;
						if(dptr=execute(cmd)) {
							runptr = dptr;
							goto newline; } }
					else
						execute(1); }
				while((c = get_next()) == ':');
				if(c)
					error(0);
				runptr = runptr->Llink; }
			mode = 0;
			break;
		case CLEAR :
			clear_vars();
			break;
		case GOSUB :
			ctl_stk[ctl_ptr++] = runptr;
			ctl_stk[ctl_ptr++] = cmdptr;
			ctl_stk[ctl_ptr++] = _GOSUB;
		case GOTO :
			pgm_only();
			return find_line(eval_num());
		case RETURN :
			pgm_only();
			if(ctl_stk[--ctl_ptr] != _GOSUB)
				error(6);
			cmdptr = ctl_stk[--ctl_ptr];
			runptr = ctl_stk[--ctl_ptr];
			line = runptr->Lnumber;
			skip_stmt();
			break;
		case PRINT :
			chk_file(1);
			j = 0;
			do {
				if(is_l_end(skip_blank()))
					--j;
				else {
					i = eval();
					if(!expr_type) {
						num_string(i, sa1);
						putc(' ',fileout); }
					fputs(sa1, fileout); } }
			while(test_next(','));
			if(!j)
				putc('\n', fileout);
			break;
		case FOR :
			pgm_only();
			ii = 1;			/* default step value */
			i = get_var();
			if(expr_type) error(0);
			expect(0xFF80+EQ);
			num_vars[i] = eval();
			if(expr_type) error(0);
			expect(0xFF80+TO);
			jj = eval();
			if(test_next(0xFF80+STEP))
				ii = eval();
			skip_stmt();
			ctl_stk[ctl_ptr++] = runptr;	/* line */
			ctl_stk[ctl_ptr++] = cmdptr;	/* command pointer */
			ctl_stk[ctl_ptr++] = ii;		/* step value */
			ctl_stk[ctl_ptr++] = jj;		/* limit value */
			ctl_stk[ctl_ptr++] = i;			/* variable number */
			ctl_stk[ctl_ptr++] = _FOR;
			break;
		case NEXT :
			pgm_only();
			if(ctl_stk[ctl_ptr-1] != _FOR)
				error(6);
			i = ctl_stk[ctl_ptr-2];
			if(!is_l_end(skip_blank()))
				if(get_var() != i) error(6);
			jj = ctl_stk[ctl_ptr-3];	/* get limit */
			ii = ctl_stk[ctl_ptr-4];	/* get step */
			num_vars[i] += ii;
			if((ii < 0) ? num_vars[i] >= jj : num_vars[i] <= jj) {
				cmdptr = ctl_stk[ctl_ptr-5];
				runptr = ctl_stk[ctl_ptr-6];
				line = runptr->Lnumber; }
			else
				ctl_ptr -= 6;
			break;
		case IF :
			i = eval_num();
			expect(0xFF80+THEN);
			if(i) {
				if(isdigit(cmd = skip_blank()))
					return find_line(eval_num());
				else if(cmd < 0) {
					++cmdptr;
					return execute(cmd); }
				else
					execute(1); }
			else
				skip_stmt();
			break;
		case LIF :
			i = eval_num();
			expect(0xFF80+THEN);
			if(i) {
				if((cmd = skip_blank()) < 0) {
					++cmdptr;
					return execute(cmd); }
				else
					execute(1);
				break; }
		case DATA :
			pgm_only();
		case REM :
			if(mode) {
				if(cptr = runptr->Llink)
					return cptr;
				longjmp(savjmp, 1); }
			break;
		case STOP :
			pgm_only();
			printf("STOP in line %u\n",line);
		case END :
			pgm_only();
			longjmp(savjmp, 1);
		case INPUT :
			ii = chk_file(1);
			if(skip_blank() == '"') {		/* special prompt */
				eval();
				expect(','); }
			else
				strcpy(sa1, "? ");
			dptr = address();
			cptr = cmdptr;
input:		if(ii == -1)
				fputs(sa1, stdout);
			cmdptr = fgets(buffer, sizeof(buffer)-1 ,filein);
			if(expr_type) {
				if(*dptr) free(*dptr);
				strcpy(*dptr = allocate(strlen(buffer)+1), buffer); }
			else {
				k = 0;
				if(test_next('-'))
					--k;
				if(!isdigit(*cmdptr)) {
					if(ii != -1) error(9);
					fputs("Input error\n",stdout);
					goto input; }
				j = get_num();
				if(k)
					j=  0 - j;
				*dptr = j; }
			cmdptr = cptr;
			break;
		case OPEN :
			if(skip_blank() != '#') error(0);
			if(files[i = chk_file(0)]) error(8);
			eval_char();
			strcpy(buffer,sa1);
			expect(',');
			eval_char();
			files[i] = fopen(buffer,sa1);
			break;
		case CLOSE :
			if((i = chk_file(1)) == -1) error(0);
			if(!filein) error(8);
			fclose(files[i]);
			files[i] = 0;
			break;
		case DIM :
			do {
				if(dptr = dim_vars[i = get_var()])
					free(dptr);
				dim_vars[i] = allocate((dim_check[i] = eval_num()+1) * 2); }
			while(test_next(','));
			break;
		case ORDER :
			readptr = find_line(eval_num());
			dptr = cmdptr;
			cmdptr = readptr->Ltext;
			if(get_next() != 0xFF80+DATA)
				error(11);
			dataptr = cmdptr;
			cmdptr = dptr;
			break;
		case READ :
			do {
				dptr = address();
				j = expr_type;
				cptr = cmdptr;
				cmdptr = dataptr;
				ii = line;
				if(!skip_blank()) {		/* End of line */
					readptr = readptr->Llink;
					cmdptr = readptr->Ltext;
					if(get_next() != 0xFF80+DATA)
					error(11); }
				line = readptr->Lnumber;
				k = eval();
				test_next(',');
				dataptr = cmdptr;
				cmdptr = cptr;
				line = ii;
				if(j != expr_type)
					error(11);
				if(!expr_type)		/* numeric assignment */
					*dptr = k;
				else {				/* character assignment */
					if(*dptr)
						free(*dptr);
					if(*sa1)
						strcpy(*dptr = allocate(strlen(sa1)+1), sa1);
					else
						*dptr = 0; } }
			while(test_next(','));
			break;
		case SAVE :
			direct_only();
			if(skip_blank())
				concat(filename, cmdptr, ".BAS");
			if(fileout = fopen(filename, "wv")) {
				disp_pgm(fileout, 0, -1);
				fclose(fileout); }
			break;
		case LOAD :
			if(!skip_blank()) error(0);
			concat(filename, cmdptr, ".BAS");
			if(filein = fopen(filename, "rv")) {
				if(!mode) clear_vars();
				clear_pgm();
				while(fgets(buffer, sizeof(buffer)-1, filein))
					edit_program();
				fclose(filein);
				return pgm_start; }
			longjmp(savjmp, 1);
		default :			/* unknown */
			error(0); }
		return 0;
}

/*
 * Test for file operator, and set up pointers
 */
int chk_file(char flag)
{
	unsigned i;

	i = -1;
	if(test_next('#')) {
		if(9 < (i = eval_num())) error(7);
		test_next(',');
		filein = fileout = files[i];
		if(flag && (!filein))
			error(7); }
	else {
		filein = stdin;
		fileout = stdout; }
	return i;
}

/*
 * Display program listing
 */
disp_pgm(FILE *fp, unsigned i, unsigned j)
{
	unsigned k;
	struct line_record *cptr;
	char c;

	for(cptr = pgm_start; cptr; cptr = cptr->Llink) {
		k = cptr->Lnumber;
		if((k >= i) && (k <= j)) {
			fprintf(fp,"%u ",k);
			for(k=0; c = cptr->Ltext[k]; ++k)
				if(c < 0) {
					c = c & 127;
					fputs(reserved_words[c - 1], fp);
					if(c < ADD)
						putc(' ',fp); }
				else
					putc(c,fp);
			putc('\n', fp); } }
}

/*
 * Test for program only, and error if interactive
 */
pgm_only()
{
	if(!mode) error(2);
}

/*
 * Test for direct only, and error if running
 */
direct_only()
{
	if(mode) error(1);
}

/*
 * Skip rest of statement
 */
skip_stmt()
{
	char c;

	while((c=*cmdptr) && (c != ':')) {
		++cmdptr;
		if(c == '"') {
			while((c=*cmdptr) && (c != '"'))
				++cmdptr;
			if(c) ++cmdptr; } }
}

/*
 * Dislay error message
 */
error(unsigned en)
{
	printf("%s error", error_messages[en]);
	if(mode)
		printf(" in line %u", line);
	putc('\n',stdout);
	longjmp(savjmp, 1);
}

/*
 * Evaluate number only (no character)
 */
int eval_num()
{
	unsigned value;

	value = eval();
	if(expr_type)
		error(4);
	return value;
}

/*
 * Evaluate character only (no numeric)
 */
eval_char()
{
	eval();
	if(!expr_type)
		error(4);
}

/*
 * Evaluate an expression (numeric or character)
 */
int eval()
{
	unsigned value;

	nest = 0;
	value = eval_sub();
	if(nest != 1) error(0);
	return value;
}

/*
 * Evaluate a sub expression
 */
int eval_sub()	
{
	unsigned value, nstack[10], nptr, optr;
	char c, ostack[10];

	++nest;								/* indicate we went down */

/* establish first entry on number and operator stacks */
	ostack[optr = nptr = 0] = 0;		/* add zero to init */

	nstack[++nptr] = get_value();		/* get next value */
/* string operations */
	if(expr_type) {						/* string operations */
		while(!is_e_end(c = skip_blank())) {
			++cmdptr;
			c &= 0x7F;
			get_char_value(sa2);
			if(c == ADD)				/* String concatination */
				strcat(sa1, sa2);
			else {
				if(c == EQ)				/* String EQUALS */
					value = !strcmp(sa1, sa2);
				else if(c == NE)		/* String NOT EQUALS */
					value = strcmp(sa1, sa2) != 0;
				else
					error(0);
				nstack[nptr] = value; 
				expr_type = 0; } } }

/* numeric operations */
	else {
		while(!is_e_end(c = skip_blank())) {
			++cmdptr;
			c = (c & 0x7F) - (ADD-1);	/* 0 based priority table */
			if(priority[c] <= priority[ostack[optr]]) {	/* execute operand */
				value = nstack[nptr--];
				nstack[nptr] = do_arith(ostack[optr--], nstack[nptr], value); }
			nstack[++nptr] = get_value();		/* stack next operand */
			if(expr_type) error(0);
			ostack[++optr] = c; }
		while(optr) {				/* clean up all pending operations */
			value = nstack[nptr--];
			nstack[nptr] = do_arith(ostack[optr--], nstack[nptr], value); } }
	if(c == ')') {
		--nest;
		++cmdptr; }
	return nstack[nptr];
}

/*
 * Get a value element for an expression
 */
int get_value()
{
	unsigned value, v, *dptr;
	char c, *ptr;

	expr_type = 0;
	if(isdigit(c = skip_blank()))
		value = get_num();
	else {
		++cmdptr;
		switch(c) {
			case '(' :			/* nesting */
				value = eval_sub();
				break;
			case '!' :			/* not */
				value = ~get_value();
				break;
			case 0xFF80+SUB :	/* negate */
				value = -get_value();
				break;
			case 0xFF80+ASC :	/* Convert character to number */
				eval_sub();
				if(!expr_type) error(4);
				value = *sa1 & 255;
				expr_type = 0;
				break;
			case 0xFF80+ABS :	/* Absolute value */
				if((value = eval_sub()) > 32767)
					value = -value;
				if(expr_type) error(4);
				break;
			case 0xFF80+RND :	/* Random number */
				value = rand(eval_sub());
				if(expr_type) error(4);
				break;
			default:			/* test for character expression */
				--cmdptr;
				if(isalpha(c)) {		/* variable */
					value = get_var();
					if(expr_type) {		/* char */
						if(ptr = char_vars[value])
							strcpy(sa1, ptr);
						else
							strcpy(sa1, ""); }
					else {
						if(test_next('(')) {	/* Array */
							if(!(dptr = dim_vars[value]))
								error(10);
							if((v = eval_sub()) >= dim_check[value])
								error(10);
							value = dptr[v]; }
						else						/* Std variable */
							value = num_vars[value]; } }
				else
					get_char_value(sa1); } }
	return value;
}

/*
 * Get character value
 */
get_char_value(char *ptr)
{
	unsigned i;
	char c, *st;

	if((c = get_next()) == '"') {	/* character value */
		while((c = *cmdptr++) != '"') {
			if(!c) error(0);
			*ptr++ = c; }
		*ptr = 0; }
	else if(isalpha(c)) {			/* variable */
		--cmdptr;
		i = get_var();
		if(!expr_type)
			error(0);
		if(st = char_vars[i])
			strcpy(ptr,st);
		else
			strcpy(ptr,""); }
	else if(c == 0xFF80+CHR) {		/* Convert number to character */
		*ptr++ = eval_sub();
		if(expr_type)
			error(4);
		*ptr = 0; }
	else if(c == 0xFF80+STR) {		/* Convert number to string */
		num_string(eval_sub(), ptr);
		if(expr_type)
			error(4); }
	else
		error(0);
	expr_type = 1;
}

/*
 * Perform an arithmetic operation
 */
int do_arith(char opr, unsigned op1, unsigned op2)
{
	unsigned value;

	switch(opr) {
		case ADD-(ADD-1) :		/* addition */
			value = op1 + op2;
			break;
		case SUB-(ADD-1) :		/* subtraction */
			value = op1 - op2;
			break;
		case MUL-(ADD-1) :		/* multiplication */
			value = op1 * op2;
			break;
		case DIV-(ADD-1) :		/* division */
			value = op1 / op2;
			break;
		case MOD-(ADD-1) :		/* modulus */
			value = op1 % op2;
			break;
		case AND-(ADD-1) :		/* logical and */
			value = op1 & op2;
			break;
		case OR-(ADD-1) :		/* logical or */
			value = op1 | op2;
			break;
		case XOR-(ADD-1) :		/* exclusive or */
			value = op1 ^ op2;
			break;
		case EQ-(ADD-1) :		/* equals */
			value = op1 == op2;
			break;
		case NE-(ADD-1) :		/* not-equals */
			value = op1 != op2;
			break;
		case LE-(ADD-1) :		/* less than or equal to */
			value = op1 <= op2;
			break;
		case LT-(ADD-1) :		/* less than */
			value = op1 < op2;
			break;
		case GE-(ADD-1) :		/* greater than or equal to */
			value = op1 >= op2;
			break;
		case GT-(ADD-1) :		/* greater than */
			value = op1 > op2;
			break;
		default:
			error(0); }
	return value;
}

/*
 * Convert a number to a string, and place in memory
 */
num_string(unsigned value, char *ptr)
{
	char cstack[5], cptr;

	cptr = 0;

	if(value > 32767) {
		*ptr++ = '-';
		value = -value; }
	do
		cstack[cptr++] = (value % 10) + '0';
	while(value /= 10);
	while(cptr)
		*ptr++ = cstack[--cptr];
	*ptr = 0;
}

/*
 * Clear program completely
 */
clear_pgm()
{
	for(runptr = pgm_start; runptr; runptr = runptr->Llink)
		free(runptr);
	pgm_start = 0;
}

/*
 * Clear all variables to zero
 */
clear_vars()
{
	unsigned i;
	char *ptr;

	for(i=0; i < NUM_VAR; ++i) {
		num_vars[i] = 0;
		if(ptr = char_vars[i]) {	/* Character variables */
			free(ptr);
			char_vars[i] = 0; }
		if(ptr = dim_vars[i]) {		/* Dimensioned arrays */
			free(ptr);
			dim_vars[i] = 0; } }
}

/*
 * Get index for variable from its name
 */
int get_var()
{
	unsigned index;
	char c;

	if(!isalpha(c = get_next()))
		error(0);
	index = (c - 'A') & 0x1f;
	if(isdigit(c = *cmdptr)) {
		index += (c - '0') * 26;
		c = *++cmdptr; }
	if(c == '$') {
		++cmdptr;
		expr_type = 1; }
	else
		expr_type = 0;

	return index;
}

/*
 * Main program
 */
main(int argc, char *argv[])
{
	char cmd;

	pgm_start = 0;
/*
 * If a name is given on the command line, load it as a program and
 * run immediately. If the program does not explicitly EXIT, we will
 * then proceed to an interactive session
 */
	if(argc > 1) {		/* Command line argument - run program */
		concat(filename, argv[1], ".BAS");
		if(filein = fopen(filename, "rv")) {
			while(fgets(buffer, sizeof(buffer)-1, filein))
				edit_program();
			fclose(filein);
			if(!setjmp(savjmp))
				execute(RUN); } }
/*
 * Display header AFTER running command line, so that programs run as
 * "batch" commands terminating with EXIT do not get "noise" output.
 */
	printf("MICRO-BASIC 2.0 - Copyright 1982-1994 Dave Dunfield.\n");

	setjmp(savjmp);
	for(;;) {						/* Main interactive loop */
		fputs("Ready\n", stdout);
	noprompt: mode = ctl_ptr = 0;
		fgets(buffer, sizeof(buffer)-1, stdin);
		if(edit_program())			/* Tokenize & edit if OK */
			goto noprompt;
		if((cmd = *cmdptr) < 0) {	/* Keyword, execute command */
			++cmdptr;
			execute(cmd); }
		else						/* Unknown, assume LET */
			execute(1); }
}
