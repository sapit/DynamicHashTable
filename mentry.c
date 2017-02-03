#include <stdio.h>
#include <stdlib.h>
#include "mentry.h"
#include "string.h"
#include <sys/types.h>
#include <ctype.h>
#define MAXLINE 500

char* parse_surname(char* line, int line_len, char **sur_ptr)
{
    char* surname;
    int i;
    for(i=0;line[i]!=',';i++){}

    surname=calloc(i+1,sizeof(char));
    int j;
    for(j=0;j<i;j++){surname[j]=tolower(line[j]);}

    *sur_ptr=surname;
    return(surname);
}

int parse_house_number(char* line, int line_len, int *num_ptr)
{
    int num=0;
    int i=0;
    while(!isspace(line[i]) && isdigit(line[i]))
    {
        num=num*10 + line[i] - '0';
        ++i;
    }
    *num_ptr=num;
    return num;
}

char *parse_postcode(char* line, int line_len, char** postcode_ptr)
{
    char* postcode=calloc(line_len+1,sizeof(char));

    int j=0;
    int i;
    for(i=0;i<line_len;++i)
    {
        if(isalnum(line[i]))
        {
            postcode[j]=tolower(line[i]);
            ++j;
        }
    }
    *postcode_ptr = postcode;
    return postcode;
}

MEntry* me_get(FILE *fd)
{
    char *line = NULL, *line1;

    size_t len = 0;
    ssize_t characters;

    if(fd==NULL)
        return NULL;
    int count = 0;

    MEntry *me;

    characters = getline(&line, &len, fd);
    if(characters==-1)
    {
        if(line)
            free(line);
        return NULL;
    }

    me = malloc(sizeof(MEntry));

    parse_surname(line,characters,&(me->surname));
    unsigned int i=0;

    me->full_address = strdup(line);

    characters = getline(&line, &len, fd);
    if(characters==-1)
    {
	if(me)
	{
		free(me->full_address);
		free(me->surname);
		free(me);
	}
        if(line)
            free(line);
        return NULL;
    }
    parse_house_number(line,characters,&(me->house_number));

    me->full_address = realloc(me->full_address,sizeof(char)*(1 + strlen(me->full_address)
                                                        + characters));
    strcat(me->full_address, line);

    characters = getline(&line, &len, fd);
    if(characters==-1)
    {
	if(me)
		me_destroy(me);
        if(line)
            free(line);
        return NULL;
    }
    parse_postcode(line,characters,&(me->postcode));
    me->full_address = realloc(me->full_address,sizeof(char)*(1 + strlen(me->full_address)
                                                        + characters));
    strcat(me->full_address, line);

    if (line)
        free(line);
    return me;
}

unsigned long me_hash(MEntry *me, unsigned long size)
{
    unsigned long hash=0;
    int i=0;
    char* iter;
    if(me==NULL || me->surname==NULL
            || me->postcode==NULL || me->full_address==NULL)
    {
        return 0;
    }
    for(iter = me->surname;*iter!='\0';iter++)
    {
        hash=hash*31 + *iter;
    }
    i=0;

    for(iter = me->postcode;*iter!='\0';iter++)
    {
        hash=hash*31 + *iter;
    }
    int house_number=me->house_number;

    while(house_number>0)
    {
        hash= hash*31 + house_number%10;
        house_number=house_number/10;
    }
    return hash%size;
}

void me_print(MEntry *me, FILE *fd)
{
    if(me==NULL)
        return;
    fprintf(fd,"%s",me->full_address);
}

int me_compare(MEntry *me1, MEntry *me2)
{
    int cmp_name=0, cmp_house=0, cmp_postcode=0;
    cmp_name=strcmp(me1->surname, me2->surname);
    cmp_postcode=strcmp(me1->postcode, me2->postcode);

    if(me1->house_number==me2->house_number && cmp_name==0 && cmp_postcode==0)
        return 0;
    else
        return -1;
}
void me_destroy(MEntry *me)
{
    if(me==NULL)
        return;

    if(me->full_address!=NULL)
    {
        free(me->full_address);
        me->full_address=NULL;
    }
    if(me->surname!=NULL)
    {
        free(me->surname);
        me->surname=NULL;
    }

    if(me->postcode!=NULL)
    {
        free(me->postcode);
        me->postcode=NULL;
    }
    free(me);
    me=NULL;

}
