#ifndef S2HTML_EVENT_H
#define S2HTML_EVENT_H

/* event data properties */
#define USER_HEADER_FILE		1
#define STD_HEADER_FILE			2
#define RES_KEYWORD_DATA		3
#define RES_KEYWORD_NON_DATA	4

#define PEVENT_DATA_SIZE	1024

typedef enum
{
	PEVENT_NULL,
	PEVENT_PREPROCESSOR_DIRECTIVE,
	PEVENT_RESERVE_KEYWORD,
	PEVENT_NUMERIC_CONSTANT,
	PEVENT_STRING,
	PEVENT_HEADER_FILE,
	PEVENT_REGULAR_EXP,
	PEVENT_SINGLE_LINE_COMMENT,
	PEVENT_MULTI_LINE_COMMENT,
	PEVENT_ASCII_CHAR,
	PEVENT_EOF
}pevent_e;

typedef struct
{
	pevent_e type; // event type
	int property; // property associated with data
	int length; // data length
	char data[PEVENT_DATA_SIZE]; // cwparsed string
}pevent_t;

/********** function prototypes **********/

pevent_t *get_parser_event(FILE *fp);

/********** state handlers **********/
pevent_t * pstate_idle_handler(FILE *fd, int ch);
pevent_t * pstate_single_line_comment_handler(FILE *fd, int ch);
pevent_t * pstate_multi_line_comment_handler(FILE *fd, int ch);
pevent_t * pstate_numeric_constant_handler(FILE *fd, int ch);
pevent_t * pstate_string_handler(FILE *fd, int ch);
pevent_t * pstate_header_file_handler(FILE *fd, int ch);
pevent_t * pstate_ascii_char_handler(FILE *fd, int ch);
pevent_t * pstate_reserve_keyword_handler(FILE *fd, int ch);
pevent_t * pstate_preprocessor_directive_handler(FILE *fd, int ch);
pevent_t * pstate_sub_preprocessor_main_handler(FILE *fd, int ch);
pevent_t * pstate_blank_space_handler(FILE *fd, int ch);
pevent_t * pstate_symbol_handler(FILE *fd, int ch);
pevent_t * pstate_regular_exp_handler(FILE *fd, int ch);
pevent_t * check_for_preprocessor_directive(FILE *fd,int ch);
pevent_t * pstate_operator_handler(FILE *fd,char ch);


#endif
/**** End of file ****/
