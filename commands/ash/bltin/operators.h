#define NOT 0
#define ISREAD 1
#define ISWRITE 2
#define ISEXEC 3
#define ISFILE 4
#define ISDIR 5
#define ISCHAR 6
#define ISBLOCK 7
#define ISFIFO 8
#define ISSETUID 9
#define ISSETGID 10
#define ISSTICKY 11
#define ISSIZE 12
#define ISLINK1 13
#define ISLINK2 14
#define ISTTY 15
#define NULSTR 16
#define STRLEN 17
#define OR1 18
#define OR2 19
#define AND1 20
#define AND2 21
#define STREQ 22
#define STRNE 23
#define NEWER 24
#define EQ 25
#define NE 26
#define GT 27
#define LT 28
#define LE 29
#define GE 30
#define PLUS 31
#define MINUS 32
#define TIMES 33
#define DIVIDE 34
#define REM 35
#define MATCHPAT 36

#define FIRST_BINARY_OP 18

#define OP_INT 1		/* arguments to operator are integer */
#define OP_STRING 2		/* arguments to operator are string */
#define OP_FILE 3		/* argument is a file name */
#define OP_LFILE 4		/* argument is a file name of a symlink? */

extern char *const unary_op[];
extern char *const binary_op[];
extern const char op_priority[];
extern const char op_argflag[];
