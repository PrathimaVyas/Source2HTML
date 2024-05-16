/* DOC

NAME		: Prathima Vyas Maddineni
PROJECT		: Source to HTML
INPUTS		: .c File
OUTPUT		: Should generate .html file

DOC */


#include <stdio.h>
#include "s2html_event.h"
#include "s2html_conv.h"
#include "s2html_conv.c"
#include "s2html_event.c"
#define FAILURE 0
#define SUCCESS 1 

/********** main **********/

int main (int argc, char *argv[])
{
    FILE *src_fp, *dest_fp; // source and destination file descriptors 
    pevent_t *event;
    char dest_file[100];

    if(argc < 2)
    {
	printf("\nError ! please enter file name and mode\n");
	printf("Usage: <executable> <file name> \n");
	printf("Example : ./a.out abc.txt\n\n");
	return FAILURE;
    }
    /* open the file */

    if(NULL == (src_fp = fopen(argv[1], "r")))
    {
	printf("Error! File %s could not be opened\n", argv[1]);
	return FAILURE;
    }

    /* Check for output file */
    if (argc > 2)
    {
	/* sprintf will create the string */
	sprintf(dest_file, "%s.html", argv[2]);
    }
    else
    {
	sprintf(dest_file, "%s.html", argv[1]);
    }

    /* open dest file */
    if (NULL == (dest_fp = fopen(dest_file, "w")))
    {
	printf("Error! could not create %s output file\n", dest_file);
	return FAILURE;
    }

    /* write HTML starting Tags */
    html_begin(dest_fp, HTML_OPEN);

    /* Read from src file convert into html and write to dest file */

    do
    {
	event = get_parser_event(src_fp);
	/* call sourc_to_html */

	source_to_html(dest_fp, event);
    } while (event->type != PEVENT_EOF);

    /* Call start_or_end_conv function for ending the convertation */
    html_end(dest_fp, HTML_CLOSE);

    printf("\nOutput file %s generated\n", dest_file);
    /* close file */
    fclose(src_fp);
    fclose(dest_fp);

    return 0;
}
