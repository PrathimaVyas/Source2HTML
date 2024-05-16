#include <stdio.h>
#include <string.h>
#include "s2html_event.h"

#define SIZE_OF_SYMBOLS (sizeof(symbols))
#define SIZE_OF_OPERATORS (sizeof(operators))
#define WORD_BUFF_SIZE  100

#define SUCCESS 1
#define FAILURE 0

/********** Internal states and event of parser **********/
typedef enum
{
    PSTATE_IDLE,
    PSTATE_PREPROCESSOR_DIRECTIVE,
    PSTATE_SUB_PREPROCESSOR_MAIN,
    PSTATE_SUB_PREPROCESSOR_RESERVE_KEYWORD,
    PSTATE_SUB_PREPROCESSOR_ASCII_CHAR,
    PSTATE_HEADER_FILE,
    PSTATE_RESERVE_KEYWORD,
    PSTATE_RESERVE_NON_KEYWORD,
    PSTATE_NUMERIC_CONSTANT,
    PSTATE_STRING,
    PSTATE_SINGLE_LINE_COMMENT,
    PSTATE_MULTI_LINE_COMMENT,
    PSTATE_ASCII_CHAR,
    PSTATE_OPERATOR,
    PSTATE_REGULAR_EXPRESSION,
    PSTATE_SYMBOL,
    PSTATE_BLANK_SPACE,
    PSTATE_SUB_PREPROCESSOR_BLANK_SPACE,
    PSTATE_SUB_PREPROCESSOR_DIRECTIVE_CHECK,
    PSTATE_SUB_PREPROCESSOR_CONSTANT,
    PSTATE_SUB_PREPROCESSOR_OPERATOR,
    PSTATE_SUB_PREPROCESSOR_SYMBOL,
    PSTATE_SUB_PREPROCESSOR_REGULAR_EXPRESSION

}pstate_e;

/********** global variables **********/

/* parser state variable */
static pstate_e state = PSTATE_IDLE;

/* sub state is used only in preprocessor state */
static pstate_e state_sub = PSTATE_SUB_PREPROCESSOR_MAIN;

/* event variable to store event and related properties */
static pevent_t pevent_data;
static int event_data_idx=0;

static char word[WORD_BUFF_SIZE];
static int word_idx=0;

static char *preprocessor[] = {"#include","#define","#if","#elif","#endif","#else","#undef","#ifndef", "#error","#pragma","#line","#warning"};

static char* res_kwords_data[] = {"const", "volatile", "extern", "auto", "register","static", "signed", "unsigned", "short", "long","double", "char", "int", "float", "struct","union", "enum", "void", "typedef", ""};

static char* res_kwords_non_data[] = {"goto", "return", "continue", "break", "if", "else", "for", "while", "do","switch", "case", "default","sizeof", ""};

static char operators[] = {'/', '+', '*', '-', '%', '=', '<', '>', '~', '&', ',', '!', '^', '|'};

static char symbols[] = {'(', ')', '{', '[', ';',':','}',']'};

/* function to check if given word is reserved key word */
static int is_reserved_keyword(char *word)
{
    int idx = 0;

    /* search for data type reserved keyword */
    while(*res_kwords_data[idx])
    {
	if(strcmp(res_kwords_data[idx++], word) == 0)
	    return RES_KEYWORD_DATA;
    }

    idx = 0; // reset index
    /* search for non data type reserved key word */
    while(*res_kwords_non_data[idx])
    {
	if(strcmp(res_kwords_non_data[idx++], word) == 0)
	    return RES_KEYWORD_NON_DATA;
    }

    return FAILURE; // word did not match, return false
}

/* function to check symbols */
static int is_symbol(char c)
{
    int idx;
    for(idx = 0; idx < SIZE_OF_SYMBOLS; idx++)
    {
	if(symbols[idx] == c)
	    return SUCCESS;
    }

    return FAILURE;
}

/* function to check identifier */

static int is_identifier(char *word)
{
    int i=0;
    while(word[i] != '\0')
    {
	if(word[i] >= '0' && word[i] <= '9' || word[i] >= 'a' && word[i] <= 'z' || word[i] >= 'A' && word[i] <= 'Z' || word[i] == '_')
	    return SUCCESS;
	i++;
    }
    return FAILURE;
}


/* function to check preprocessor */

static int is_preprocessor(char *word)
{
    int i=0;
    while(preprocessor[i] != NULL)
    {
	if(strcmp(preprocessor[i],word) == 0)
	    return SUCCESS;
	i++;
    }
    return FAILURE;
}

/* function to check operator */
static int is_operator(char c)
{
    int idx;
    for(idx = 0; idx < SIZE_OF_OPERATORS; idx++)
    {
	if(operators[idx] == c)
	{
	    return SUCCESS;
	}
    }

    return FAILURE;
}

/* to set parser event */
static void set_parser_event(pstate_e s, pevent_e e)
{
    pevent_data.data[event_data_idx] = '\0';
    pevent_data.length = event_data_idx;
    event_data_idx = 0;
    state = s;
    pevent_data.type = e;
}


/************ Event functions **********/

/* This function parses the source file and generate
 * event based on parsed characters and string
 */
pevent_t *get_parser_event(FILE *fd)
{
    int ch, pre_ch;
    pevent_t *evptr = NULL;
    /* Read char by char */
    while((ch = fgetc(fd)) != EOF)
    {
	switch(state)
	{
	    case PSTATE_IDLE :
		if((evptr = pstate_idle_handler(fd, ch)) != NULL)
		    return evptr;
		break;
	    case PSTATE_SINGLE_LINE_COMMENT :
		if((evptr = pstate_single_line_comment_handler(fd, ch)) != NULL)
		    return evptr;
		break;
	    case PSTATE_MULTI_LINE_COMMENT :
		if((evptr = pstate_multi_line_comment_handler(fd, ch)) != NULL)
		    return evptr;
		break;
	    case PSTATE_PREPROCESSOR_DIRECTIVE :
		if((evptr = pstate_preprocessor_directive_handler(fd, ch)) != NULL)
		    return evptr;
		break;
	    case PSTATE_RESERVE_KEYWORD :
		if((evptr = pstate_reserve_keyword_handler(fd, ch)) != NULL)
		    return evptr;
		break;
	    case PSTATE_NUMERIC_CONSTANT :
		if((evptr = pstate_numeric_constant_handler(fd, ch)) != NULL)
		    return evptr;
		break;
	    case PSTATE_STRING :
		if((evptr = pstate_string_handler(fd, ch)) != NULL)
		    return evptr;
		break;
	    case PSTATE_HEADER_FILE :
		if((evptr = pstate_header_file_handler(fd, ch)) != NULL)
		    return evptr;
		break;
	    case PSTATE_ASCII_CHAR :
		if((evptr = pstate_ascii_char_handler(fd, ch)) != NULL)
		    return evptr;
		break;
	    case PSTATE_REGULAR_EXPRESSION:
		if((evptr = pstate_regular_exp_handler(fd,ch)) != NULL)
		    return evptr;
		break;
	    case PSTATE_SYMBOL:
		if((evptr = pstate_symbol_handler(fd,ch))!= NULL)
		    return evptr;
		break;
	    case PSTATE_OPERATOR:
		if((evptr = pstate_operator_handler(fd,ch)) != NULL)
		    return evptr;
		break;
	    case PSTATE_BLANK_SPACE:
		if((evptr = pstate_blank_space_handler(fd,ch)) != NULL)
		    return evptr;
		break;
	    default :
		printf("%c ",ch);
		printf("unknown state\n");
		state = PSTATE_IDLE;
		break;
	}
    }

    /* end of file is reached, move back to idle state and set EOF event */
    set_parser_event(PSTATE_IDLE, PEVENT_EOF);

    return &pevent_data; // return final event
}


/********** IDLE state Handler **********
 * Idle state handler identifies
 ****************************************/

pevent_t * pstate_idle_handler(FILE *fd, int ch)
{
    int pre_ch;
    switch(ch)
    {
	/* beginning of ASCII CHAR */
	case '\'' :
	    if(event_data_idx) //initially it is 0 so it won't enter the condition 1st
	    {
		set_parser_event(PSTATE_IDLE,PEVENT_REGULAR_EXP);
		return &pevent_data;
	    }
	    else //we have to store it in data and set the state as ascii
	    {
		pevent_data.data[event_data_idx++] = ch;
		state = PSTATE_ASCII_CHAR;
	    }
	    break;

	    /* CHECK FOR SINGLE AND MULTILINE COMMENTS */

	case '/' :
	    pre_ch = ch;
	    if((ch = fgetc(fd)) == '*') // multi line comment
	    {
		if(event_data_idx) // we have regular exp in buffer first process that
		{
		    fseek(fd, -2L, SEEK_CUR); // unget chars
		    set_parser_event(PSTATE_IDLE, PEVENT_REGULAR_EXP);
		    return &pevent_data;
		}
		else // multi line comment begin
		{
		    state = PSTATE_MULTI_LINE_COMMENT;
		    pevent_data.data[event_data_idx++] = pre_ch;
		    pevent_data.data[event_data_idx++] = ch;
		}
	    }
	    else if(ch == '/') // single line comment
	    {
		if(event_data_idx) // we have regular exp in buffer first process that
		{
		    fseek(fd, -2L, SEEK_CUR); // unget chars
		    set_parser_event(PSTATE_IDLE, PEVENT_REGULAR_EXP);
		    return &pevent_data;
		}
		else // single line comment begin
		{
		    state = PSTATE_SINGLE_LINE_COMMENT;
		    pevent_data.data[event_data_idx++] = pre_ch;
		    pevent_data.data[event_data_idx++] = ch;
		}
	    }
	    else // it is regular exp
	    {
		fseek(fd,-2,SEEK_CUR);
		state = PSTATE_OPERATOR;
	    }
	    break;

	    /* CHECK FOR PREPROCESSOR DIRECTIVE */

	case '#' :
	    if(event_data_idx)
	    {
		/* set the state as preprocessor directive and pevent type as regular expression */
		set_parser_event(PSTATE_PREPROCESSOR_DIRECTIVE,PEVENT_REGULAR_EXP);
		return &pevent_data;
	    }
	    else
	    {
		fseek(fd,-1,SEEK_CUR);
		state = PSTATE_PREPROCESSOR_DIRECTIVE;
	    }
	    break;

	    /* CHECK FOR STRINGS */

	case '\"' :
	    if(event_data_idx)
	    {
		/* set the state as preprocessor directive and pevent type as regular expression */
		set_parser_event(PSTATE_IDLE,PEVENT_REGULAR_EXP);
		return &pevent_data;
	    }
	    else
	    {
		pevent_data.data[event_data_idx++] = ch;
		state = PSTATE_STRING;
	    }
	    break;

	    /* CHECK FOR NUMBER CONSTANT */

	case '0' ... '9' : // detect numeric constant
	    if(event_data_idx)
	    {
		set_parser_event(PSTATE_IDLE,PEVENT_REGULAR_EXP);
		return &pevent_data;
	    }
	    else
	    {
		pevent_data.data[event_data_idx++] = ch;
		state = PSTATE_NUMERIC_CONSTANT;
	    }
	    break;

	case 'a' ... 'z' : // could be reserved key word
	    if(event_data_idx)
	    {
		set_parser_event(PSTATE_IDLE,PEVENT_REGULAR_EXP);
		return &pevent_data;
	    }
	    else
	    {
		if(is_reserved_keyword(pevent_data.data) == RES_KEYWORD_DATA)
		{
		    pevent_data.property = RES_KEYWORD_DATA;
		    state = PSTATE_RESERVE_KEYWORD;
		}
		else
		{   pevent_data.property = RES_KEYWORD_NON_DATA;
		    state = PSTATE_RESERVE_KEYWORD;
		}
		pevent_data.data[event_data_idx++] = ch;
	    }
	    break;

	case 'A' ... 'Z' : // could be reserved key word
	    if(event_data_idx)
	    {
		set_parser_event(PSTATE_IDLE,PEVENT_REGULAR_EXP);
		return &pevent_data;
	    }
	    else
	    {
		pevent_data.data[event_data_idx++] = ch;
		state = PSTATE_REGULAR_EXPRESSION;
	    }
	    break;

	    /* CHECK FOR THE BLANK SPACES */

	case ' ':
	case '\n':
	case '\t':
	    if(event_data_idx)
	    {
		set_parser_event(PSTATE_IDLE,PEVENT_REGULAR_EXP);
		return &pevent_data;
	    }
	    else
	    {
		fseek(fd,-1,SEEK_CUR);
		state = PSTATE_BLANK_SPACE;
	    }
	    break;

	default : // Assuming common text starts by default.
	    if(is_operator(ch) == SUCCESS)
	    {
		if(event_data_idx)
		{
		    set_parser_event(PSTATE_IDLE,PEVENT_REGULAR_EXP);
		    return &pevent_data;
		}
		else
		{
		    fseek(fd,-1,SEEK_CUR);
		    state = PSTATE_OPERATOR;
		}
	    }
	    else if(is_symbol(ch) == SUCCESS)
	    {
		if(event_data_idx)
		{
		    set_parser_event(PSTATE_IDLE,PEVENT_REGULAR_EXP);
		    return &pevent_data;
		}
		else
		{
		    fseek(fd,-1,SEEK_CUR);
		    state = PSTATE_SYMBOL;
		}
	    }
	    break;
    }
    return NULL;
}

/* STATE FOR SINGLE LINE COMMENT HANDLING */
pevent_t * pstate_single_line_comment_handler(FILE *fd, int ch)
{
    int pre_ch;
    switch(ch)
    {
	case '\n' : /* single line comment ends here */
	    pre_ch = ch;
	    pevent_data.data[event_data_idx++] = ch;
	    set_parser_event(PSTATE_IDLE, PEVENT_SINGLE_LINE_COMMENT);
	    return &pevent_data;
	default :  // collect single line comment chars
	    pevent_data.data[event_data_idx++] = ch;
	    break;
    }

    return NULL;
}

/* STATE FOR MULTILINE COMMENT HANDLING */
pevent_t * pstate_multi_line_comment_handler(FILE *fd, int ch)
{
    int pre_ch;
    switch(ch)
    {
	case '*' : /* comment might end here */
	    pre_ch = ch;
	    pevent_data.data[event_data_idx++] = ch;
	    if((ch = fgetc(fd)) == '/')
	    {
		pre_ch = ch;
		pevent_data.data[event_data_idx++] = ch;
		set_parser_event(PSTATE_IDLE, PEVENT_MULTI_LINE_COMMENT);
		return &pevent_data;
	    }
	    else // multi line comment string still continued
	    {
		pevent_data.data[event_data_idx++] = ch;
	    }
	    break;
	case '/' :
	    /* go back by two steps and read previous char */
	    fseek(fd, -2L, SEEK_CUR); // move two steps back
	    pre_ch = fgetc(fd); // read a char
	    fgetc(fd); // to come back to current offset

	    pevent_data.data[event_data_idx++] = ch;
	    if(pre_ch == '*')
	    {
		set_parser_event(PSTATE_IDLE, PEVENT_MULTI_LINE_COMMENT);
		return &pevent_data;
	    }
	    break;
	default :  // collect multi-line comment chars
	    pevent_data.data[event_data_idx++] = ch;
	    break;
    }

    return NULL;
}

/* STATE FOR THE PREPROCESSOR DIRECTIVE HANDLER */
pevent_t * pstate_preprocessor_directive_handler(FILE *fd, int ch)
{
    int tch;
    switch(state_sub)
    {
	case PSTATE_SUB_PREPROCESSOR_MAIN :
	    return pstate_sub_preprocessor_main_handler(fd, ch);

	case PSTATE_SUB_PREPROCESSOR_RESERVE_KEYWORD :
	    return pstate_reserve_keyword_handler(fd, ch);

	case PSTATE_SUB_PREPROCESSOR_ASCII_CHAR :
	    return pstate_ascii_char_handler(fd, ch);

	case PSTATE_SUB_PREPROCESSOR_DIRECTIVE_CHECK:
	    return check_for_preprocessor_directive(fd,ch);

	case PSTATE_SUB_PREPROCESSOR_BLANK_SPACE:
	    return pstate_blank_space_handler(fd,ch);

	case PSTATE_SUB_PREPROCESSOR_REGULAR_EXPRESSION:
	    return pstate_regular_exp_handler(fd,ch);

	case PSTATE_SUB_PREPROCESSOR_OPERATOR:
	    return pstate_operator_handler(fd,ch);

	case PSTATE_HEADER_FILE:
	    return pstate_header_file_handler(fd,ch);

	case PSTATE_SUB_PREPROCESSOR_SYMBOL:
	    return pstate_symbol_handler(fd,ch);

	case PSTATE_SUB_PREPROCESSOR_CONSTANT:
	    return pstate_numeric_constant_handler(fd,ch);
	default :
	    printf("unknown state\n");
	    state = PSTATE_IDLE;
    }

    return NULL;
}

/* STATE FOR THE SUB PREPROCESSOR MAIN HANDLER */
pevent_t * pstate_sub_preprocessor_main_handler(FILE *fd, int ch)
{
    switch(ch)
    {
	case '#':
	    pevent_data.data[event_data_idx++] = ch;
	    state_sub = PSTATE_SUB_PREPROCESSOR_DIRECTIVE_CHECK;
	    break;
	case 'A' ... 'Z':
	    pevent_data.data[event_data_idx++] = ch;
	    state_sub = PSTATE_SUB_PREPROCESSOR_REGULAR_EXPRESSION;
	    break;
	case 'a' ... 'z':
	    pevent_data.data[event_data_idx++] = ch;
	    state_sub = PSTATE_SUB_PREPROCESSOR_RESERVE_KEYWORD;
	    break;
	case '0' ... '9':
	    pevent_data.data[event_data_idx++] = ch;
	    state_sub = PSTATE_SUB_PREPROCESSOR_CONSTANT;
	    break;
	case '\"':
	    pevent_data.property = USER_HEADER_FILE;
	    pevent_data.data[event_data_idx++] = ch;
	    state_sub = PSTATE_HEADER_FILE;
	    break;
	case '\'':
	    pevent_data.data[event_data_idx++] = ch;
	    state_sub = PSTATE_SUB_PREPROCESSOR_ASCII_CHAR;
	    break;
	case '<':
	    pevent_data.property = USER_HEADER_FILE;
	    pevent_data.data[event_data_idx++] = ' ';
	    state_sub = PSTATE_HEADER_FILE;
	    break;

	    /* if there is a space between #include and header file */

	case ' ':
	case '\t':
	    fseek(fd,-1,SEEK_CUR);
	    state_sub = PSTATE_SUB_PREPROCESSOR_BLANK_SPACE;
	    break;

	    /* IF THE LINE REACHES \n or \0 */

	case '\n':
	case '\0':
	    pevent_data.data[event_data_idx++] = ch;
	    set_parser_event(PSTATE_IDLE, PEVENT_PREPROCESSOR_DIRECTIVE);
	    return &pevent_data;
	    break;

	    /* check for operator or symbol */

	default:
	    if(is_operator(ch) == SUCCESS)
	    {
		if(event_data_idx)
		{
		    set_parser_event(PSTATE_IDLE,PEVENT_REGULAR_EXP);
		    return &pevent_data;
		}
		else
		{
		    fseek(fd,-1,SEEK_CUR);
		    state = PSTATE_OPERATOR;
		}
	    }
	    else if(is_symbol(ch) == SUCCESS)
	    {
		if(event_data_idx)
		{
		    set_parser_event(PSTATE_IDLE,PEVENT_REGULAR_EXP);
		    return &pevent_data;
		}
		else
		{
		    fseek(fd,-1,SEEK_CUR);
		    state_sub = PSTATE_SUB_PREPROCESSOR_SYMBOL;
		}
	    }
	    break;
    }
    return NULL;
}

/* PREPROCESSOR DIRECTIVES ARE ALWAYS IN LOWER CASE */
pevent_t * check_for_preprocessor_directive(FILE *fd,int ch)
{
    switch(ch)
    {
	/* if it is include define or any other directive */
	case 'a' ... 'z':
	    pevent_data.data[event_data_idx++] = ch;
	    break;
	    /* once it reaches end then it will go to the default */

	default:
	    pevent_data.data[event_data_idx++] = '\0';
	    if(is_preprocessor(pevent_data.data) == SUCCESS)
	    {
		state_sub = PSTATE_SUB_PREPROCESSOR_MAIN;
		set_parser_event(PSTATE_PREPROCESSOR_DIRECTIVE,PEVENT_PREPROCESSOR_DIRECTIVE);
		/* we have to go back to consider it again */
		fseek(fd,-1,SEEK_CUR);
		return &pevent_data;
	    }
	    else
	    {
		set_parser_event(PSTATE_IDLE,PEVENT_REGULAR_EXP);
		return &pevent_data;
	    }
    }
    return NULL;
}

/* to find the header file */
pevent_t * pstate_header_file_handler(FILE *fd, int ch)
{
    switch(ch)
    {
	case '\"':
	    pevent_data.data[event_data_idx++] = ch;
	    set_parser_event(PSTATE_PREPROCESSOR_DIRECTIVE,PEVENT_HEADER_FILE);
	    state_sub = PSTATE_SUB_PREPROCESSOR_MAIN;
	    return &pevent_data;
	case '>':
	    // pevent_data.data[event_data_idx++] = ch;
	    set_parser_event(PSTATE_PREPROCESSOR_DIRECTIVE,PEVENT_HEADER_FILE);
	    state_sub = PSTATE_SUB_PREPROCESSOR_MAIN;
	    return &pevent_data;
	default:
	    pevent_data.data[event_data_idx++] = ch;
    }
    return NULL;
}

/* to handle the reserved keywords */
pevent_t * pstate_reserve_keyword_handler(FILE *fd, int ch)
{
    switch(ch)
    {
	/* if it is alphabets store it in pevent_data.data and if not include null char at the end and check it is reserved keyword or not */
	case 'a' ... 'z':
	case 'A' ... 'Z':
	case '0' ... '9':
	case '_':
	    pevent_data.data[event_data_idx++] = ch;
	    break;
	default:
	    pevent_data.data[event_data_idx++] = '\0';
	    if(is_reserved_keyword(pevent_data.data) == RES_KEYWORD_DATA)
	    {
		if(state == PSTATE_PREPROCESSOR_DIRECTIVE)
		{
		    pevent_data.property = RES_KEYWORD_DATA;
		    state_sub = PSTATE_SUB_PREPROCESSOR_MAIN;
		    set_parser_event(PSTATE_PREPROCESSOR_DIRECTIVE,PEVENT_RESERVE_KEYWORD);
		}
		else
		{
		    pevent_data.property = RES_KEYWORD_DATA;
		    set_parser_event(PSTATE_IDLE,PEVENT_RESERVE_KEYWORD);
		}
		/* we have to move the cursor one position back because it already access the next character so to access that char again we have to set the cursor to one position previous */
		fseek(fd,-1,SEEK_CUR);
		return &pevent_data;
	    }
	    else if(is_reserved_keyword(pevent_data.data) == RES_KEYWORD_NON_DATA)
	    {
		if(state == PSTATE_PREPROCESSOR_DIRECTIVE)
		{
		    pevent_data.property = RES_KEYWORD_NON_DATA;
		    state_sub = PSTATE_SUB_PREPROCESSOR_MAIN;
		    set_parser_event(PSTATE_PREPROCESSOR_DIRECTIVE,PEVENT_RESERVE_KEYWORD);
		}
		else
		{
		    pevent_data.property = RES_KEYWORD_NON_DATA;
		    set_parser_event(PSTATE_IDLE,PEVENT_RESERVE_KEYWORD);
		}
		fseek(fd,-1,SEEK_CUR);
		return &pevent_data;
	    }
	    /* if it is not a keyword then check for the identifier */
	    else
	    {
		if(is_identifier(pevent_data.data) == SUCCESS)
		{
		    if(state == PSTATE_PREPROCESSOR_DIRECTIVE)
		    {
			state_sub = PSTATE_SUB_PREPROCESSOR_MAIN;
			set_parser_event(PSTATE_PREPROCESSOR_DIRECTIVE,PEVENT_REGULAR_EXP);
		    }
		    else
		    {
			set_parser_event(PSTATE_IDLE,PEVENT_REGULAR_EXP);
		    }
		    /* to bring the cursor to one position back */
		    fseek(fd,-1,SEEK_CUR);
		    return &pevent_data;
		}
		else
		{
		    state = PSTATE_IDLE;
		    fseek(fd,-(strlen(pevent_data.data)),SEEK_CUR);
		    return NULL;
		}
	    }
    }
    return NULL;
}

/* TO HANDLE THE NUMERIC CONSTANTS */
pevent_t * pstate_numeric_constant_handler(FILE *fd, int ch)
{
    switch(ch)
    {
	case '0' ... '9':
	case '.':
	    pevent_data.data[event_data_idx++] = ch;
	    break;
	default:
	    if(state == PSTATE_PREPROCESSOR_DIRECTIVE)
	    {
		state_sub = PSTATE_SUB_PREPROCESSOR_MAIN;
		set_parser_event(PSTATE_PREPROCESSOR_DIRECTIVE,PEVENT_NUMERIC_CONSTANT);
		fseek(fd,-1,SEEK_CUR);
	    }
	    else
	    {
		set_parser_event(PSTATE_IDLE, PEVENT_NUMERIC_CONSTANT);
		fseek(fd,-1,SEEK_CUR);
	    }
	    return &pevent_data;
    }
    return NULL;
}

/* TO HANDLE THE STRING */
pevent_t * pstate_string_handler(FILE *fd, int ch)
{
    switch(ch)
    {
	case '\"':
	    pevent_data.data[event_data_idx++] = ch;
	    set_parser_event(PSTATE_IDLE,PEVENT_STRING);
	    return &pevent_data;
	default:
	    pevent_data.data[event_data_idx++] = ch;
    }
    return NULL;
}

/* CHECK FOR THE ASCII CHARACTER */
pevent_t * pstate_ascii_char_handler(FILE *fd, int ch)
{
    switch(ch)
    {
	case '\'':
	    pevent_data.data[event_data_idx++] = ch;
	    if((ch=fgetc(fd)) == '\'')
		pevent_data.data[event_data_idx++] = ch;
	    else
		fseek(fd,-1,SEEK_CUR);

	    if(state == PSTATE_PREPROCESSOR_DIRECTIVE)
	    {
		state_sub = PSTATE_SUB_PREPROCESSOR_MAIN;
		set_parser_event(PSTATE_PREPROCESSOR_DIRECTIVE,PEVENT_ASCII_CHAR);
	    }
	    else
		set_parser_event(PSTATE_IDLE,PEVENT_ASCII_CHAR);
	    return &pevent_data;
	    break;
	default:
	    pevent_data.data[event_data_idx++] = ch;
    }
    return NULL;
}

/* REGULAR EXPRESSION HANDLER */
pevent_t * pstate_regular_exp_handler(FILE *fd, int ch)
{
    switch(ch)
    {
	case 'a' ... 'z':
	case 'A' ... 'Z':
	case '0' ... '9':
	case '_':
	    pevent_data.data[event_data_idx++] = ch;
	    break;
	default:
	    pevent_data.data[event_data_idx++] = '\0';
	    if(is_identifier(pevent_data.data) == SUCCESS)
	    {
		if(state == PSTATE_PREPROCESSOR_DIRECTIVE)
		{
		    state_sub = PSTATE_SUB_PREPROCESSOR_MAIN;
		    set_parser_event(PSTATE_PREPROCESSOR_DIRECTIVE,PEVENT_REGULAR_EXP);
		}
		else
		    set_parser_event(PSTATE_IDLE,PEVENT_REGULAR_EXP);
		fseek(fd,-1,SEEK_CUR);
		return &pevent_data;
	    }
	    else
	    {
		state = PSTATE_IDLE;
		fseek(fd, -(strlen(pevent_data.data)), SEEK_CUR);
	    }
    }
    return NULL;
}

/* SYMBOL HANDLER */
pevent_t * pstate_symbol_handler(FILE *fd, int ch)
{
    if(is_symbol(ch) == SUCCESS)
	pevent_data.data[event_data_idx++] = ch;
    else
    {
	if(state == PSTATE_PREPROCESSOR_DIRECTIVE)
	{
	    state_sub = PSTATE_SUB_PREPROCESSOR_MAIN;
	    set_parser_event(PSTATE_PREPROCESSOR_DIRECTIVE,PEVENT_REGULAR_EXP);
	}
	else
	    set_parser_event(PSTATE_IDLE,PEVENT_REGULAR_EXP);
	fseek(fd,-1,SEEK_CUR);
	return &pevent_data;
    }
    return NULL;
}

/* BLANK SPACE HANDLER */
pevent_t * pstate_blank_space_handler(FILE *fd, int ch)
{
    if(ch == ' ' || ch == '\t' || ch == '\n')
    {

	pevent_data.data[event_data_idx++] = ch;
    }
    else
    {
	if(state == PSTATE_PREPROCESSOR_DIRECTIVE)
	{
	    state_sub = PSTATE_SUB_PREPROCESSOR_MAIN;
	    set_parser_event(PSTATE_PREPROCESSOR_DIRECTIVE, PEVENT_REGULAR_EXP);
	}
	else
	    set_parser_event(PSTATE_IDLE, PEVENT_REGULAR_EXP);
	fseek(fd,-1,SEEK_CUR);
	return &pevent_data;
    }
    return NULL;
}

/* OPERATOR HANDLER */
pevent_t * pstate_operator_handler(FILE *fd,char ch)
{
    /* if it is operator store it in the array */
    if(is_operator(ch) == SUCCESS)
	pevent_data.data[event_data_idx++] = ch;
    else
    {
	if(state == PSTATE_PREPROCESSOR_DIRECTIVE)
	{
	    state_sub = PSTATE_SUB_PREPROCESSOR_MAIN;
	    set_parser_event(PSTATE_PREPROCESSOR_DIRECTIVE, PEVENT_REGULAR_EXP);
	}
	else
	    set_parser_event(PSTATE_IDLE, PEVENT_REGULAR_EXP);
	fseek(fd,-1,SEEK_CUR);
	return &pevent_data;
    }
    return NULL;
}

