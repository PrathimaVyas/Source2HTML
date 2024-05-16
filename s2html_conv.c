
#include <stdio.h>
#include "s2html_event.h"
#include "s2html_conv.h"

/* start_or_end_conv function definitation */
/* type => not used, but can be used to add differnet HTML tags */
void html_begin(FILE* dest_fp, int type) 
{
    /* Add HTML begining tags into destination file */
    fprintf(dest_fp, "<!DOCTYPE html>\n");
    fprintf(dest_fp, "<html lang=\"en-US\">\n");
    fprintf(dest_fp, "<head>\n");
    fprintf(dest_fp, "<title>%s</title>\n", "sode2html");
    fprintf(dest_fp, "<meta charset=\"UTF-8\">\n");
    fprintf(dest_fp, "<link rel=\"stylesheet\" href=\"styles.css\">\n");
    /* styles.css file is having the content related to colors for the keywords */ 
    fprintf(dest_fp, "</head>\n");
    fprintf(dest_fp, "<body style=\"background-color:lightgrey;\">\n");
    /* backgroound colour set as grey color */
    fprintf(dest_fp, "<pre>\n");
}


void html_end(FILE* dest_fp, int type) /* type => not used, but can be used to add differnet HTML tags */
{
    /* Add HTML closing tags into destination file */
    fprintf(dest_fp, "</pre>\n");
    fprintf(dest_fp, "</body>\n");
    fprintf(dest_fp, "</html>\n");
}


/* sourc_to_html function definitation */
void source_to_html(FILE* fp, pevent_t *event)
{
#ifdef DEBUG
    printf("%s", event->data);
#endif
    //`printf("data->%s\t", event->data);
    switch(event->type)
    {
	case PEVENT_PREPROCESSOR_DIRECTIVE:
     	 fprintf(fp,"<span class=\"preprocess_dir\">%s</span>",event->data);
	    break;
	case PEVENT_MULTI_LINE_COMMENT:
	case PEVENT_SINGLE_LINE_COMMENT:
	    fprintf(fp,"<span class=\"comment\">%s</span>",event->data);
	    break;
	case PEVENT_STRING:
	    fprintf(fp,"<span class=\"string\">%s</span>",event->data);
	    break;
	case PEVENT_HEADER_FILE:
	    /* write to html */
if(event->data[0] == '\"')
    event->property = USER_HEADER_FILE;
	    if(event->property == USER_HEADER_FILE)
		fprintf(fp,"<span class=\"header_file\">%s</span>",event->data);
	    else
		fprintf(fp,"<span class=\"header_file\">&lt;%s&gt;</span>",event->data);
	    break;
	case PEVENT_REGULAR_EXP:
	case PEVENT_EOF :
	    fprintf(fp,"%s",event->data);
	    break;
	case PEVENT_NUMERIC_CONSTANT:
	    fprintf(fp,"<span class=\"numeric_constant\">%s</span>",event->data);
	    break;
	case PEVENT_RESERVE_KEYWORD:
	    if(event->property == RES_KEYWORD_DATA)
	    {
		fprintf(fp,"<span class=\"reserved_key1\">%s</span>",event->data);
	    }
	    else
	    {
		fprintf(fp,"<span class=\"reserved_key2\">%s</span>",event->data);
	    }
	    break;
	case PEVENT_ASCII_CHAR:
	    fprintf(fp,"<span class=\"ascii_char\">%s</span>",event->data);
	    break;
	default :
	    printf("Unknow event\n");
	    break;
    }
}

