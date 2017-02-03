/*Zdravko Ivanov 2144328i APH Exercise 1
This is my own work as defined in the Academic Ethics agreement I have signed.*/

#include <stdio.h>
#include <stdlib.h>
#include "mentry.h"
#include "mlist.h"

int ml_verbose=0;		/* if true, print diagnostics on stderr */

typedef struct node{
    MEntry *entry;
    struct node *next;
} Node;

typedef struct bucket{
    int size;
    Node *head;
}Bucket;

typedef struct mlist{
    unsigned long capacity;
    Bucket **buckets;
} MList;

void ml_destroy_configurable(MList *ml, int flag);

/* ml_create - created a new mailing list */
MList *ml_create(void)
{
    if (ml_verbose)
		fprintf(stderr, "mlist: creating mailing list\n");
    int capacity=20;
    unsigned long i;
    MList *hash_table = NULL;
    if((hash_table = malloc(sizeof(MList)))==NULL)
        return NULL;
    hash_table->capacity=capacity;
    if((hash_table->buckets=malloc(sizeof(Bucket*)*capacity))==NULL)
        return NULL;
    for(i=0;i<capacity;i++)
    {
        if((hash_table->buckets[i]=malloc(sizeof(Bucket)))==NULL)
            return NULL;
        hash_table->buckets[i]->size=0;
        hash_table->buckets[i]->head=(Node*)NULL;

    }
    return hash_table;
}

void ml_print(MList *ml)
{
    Node *iter;
    int sum=0;
    unsigned long i;
    for(i=0;i<ml->capacity;i++)
    {
        iter=ml->buckets[i]->head;
        sum+=ml->buckets[i]->size;
        while(iter!=NULL)
        {
            me_print(iter->entry,stdout);
            iter=iter->next;
        }
    }
}

/* ml_add - adds a new MEntry to the list;
 * returns 1 if successful, 0 if error (malloc)
 * returns 1 if it is a duplicate */
int ml_add(MList **ml, MEntry *me)
{
    if (ml_verbose)
		fprintf(stderr, "mlist: ml_add() entered\n");
    unsigned long hash = me_hash(me,(*ml)->capacity);
    unsigned long i;
    if((*ml)->buckets[hash]->size >= 20)
    {
    	if (ml_verbose)
		fprintf(stderr, "mlist: ml_add() resizing\n");
        MList *new_list;
        if((new_list = malloc(sizeof(MList)))==NULL)
            return 0;
        new_list->capacity=(*ml)->capacity*2;
        if((new_list->buckets = malloc(sizeof(Bucket)*new_list->capacity))==NULL)
            return 0;
        for(i=0;i<new_list->capacity;i++)
        {
            new_list->buckets[i]=malloc(sizeof(Bucket));
            new_list->buckets[i]->size=0;
            new_list->buckets[i]->head=NULL;
        }
        unsigned long new_hash;

        unsigned long new_capacity;

        /*is the new capacity good enough*/
        int fl=0;
        new_capacity=(*ml)->capacity*2;
        while(fl==0)
        {
            fl=1;
            new_capacity=new_capacity*2;
            int new_bucket_sizes[new_capacity];
            for(i=0;i<new_capacity;++i)
                new_bucket_sizes[i]=0;
            for(i=0;i<(*ml)->capacity;i++)
            {
                Node *iter=(*ml)->buckets[i]->head;
                Node *new_list_node=NULL;


                while(iter!=NULL && fl==1)
                {
                    if(iter->entry!=NULL)
                    {
                        new_hash=me_hash(iter->entry,new_capacity);
                        ++new_bucket_sizes[new_hash];
                        if(new_bucket_sizes[new_hash]>=20)
                        {
                            fl=0;
                        }
                    }
                    iter=iter->next;
                }
                if(fl==0)
                    break;
            }
        }

        for(i=0;i<(*ml)->capacity;i++)
        {
            Node *iter=(*ml)->buckets[i]->head;
            Node *new_list_node=NULL;

            while(iter!=NULL)
            {
                if(iter->entry!=NULL)
                {
                    if((new_list_node=malloc(sizeof(Node)))==NULL)
                        return 0;

                    new_list_node->entry=iter->entry;
                    new_hash=me_hash(iter->entry,new_list->capacity);

                    if(new_list->buckets[new_hash]->head==NULL)
                    {
                        new_list->buckets[new_hash]->head=new_list_node;
                        new_list->buckets[new_hash]->head->next=NULL;
                    }
                    else
                    {
                        new_list_node->next=new_list->buckets[new_hash]->head;
                        new_list->buckets[new_hash]->head=new_list_node;
                    }
                    new_list->buckets[new_hash]->size++;
                    new_list_node=NULL;
                }
                iter=iter->next;
            }
        }

        ml_destroy_configurable(*ml,0);
        *ml=new_list;
    }
    Node *new_node;

    if((new_node=malloc(sizeof(Node)))==NULL)
        return 0;
    new_node->entry=me;
    if((*ml)->buckets[hash]->head==NULL)
    {
        new_node->next=NULL;
        (*ml)->buckets[hash]->head=new_node;
    }
    else
    {
        new_node->next=(*ml)->buckets[hash]->head;
        (*ml)->buckets[hash]->head=new_node;
    }
    (*ml)->buckets[hash]->size++;

    return 1;
}

/* ml_lookup - looks for MEntry in the list, returns matching entry or NULL */
MEntry *ml_lookup(MList *ml, MEntry *me)
{
    if (ml_verbose)
		fprintf(stderr, "mlist: ml_lookup() entered\n");
    unsigned long hash = me_hash(me,ml->capacity);
    Node *iter;
    if(ml->buckets[hash]==NULL)
        return NULL;
    iter = ml->buckets[hash]->head;
    while(iter!=NULL)
    {
        if(me_compare(iter->entry,me)==0)
        {
            return iter->entry;
        }
        iter=iter->next;
    }
    return NULL;
}

/* ml_destroy - destroy the mailing list */
void ml_destroy(MList *ml)
{
    if (ml_verbose)
		fprintf(stderr, "mlist: ml_destroy() entered\n");
    ml_destroy_configurable(ml,1);
}
void ml_destroy_configurable(MList *ml, int flag)
{
    Node *next;
    Node *iter;
    Node *last;
    unsigned long i;
    for(i=0;i<ml->capacity;i++)
    {
        iter = ml->buckets[i]->head;
            while(iter!=NULL)
            {
                last=iter;
                iter=iter->next;
                if(flag==1)
                    me_destroy(last->entry);
                free(last);
                last=NULL;
            }
        free(ml->buckets[i]);
        ml->buckets[i]=NULL;
    }
    free(ml->buckets);
    ml->buckets=NULL;
    free(ml);
    ml=NULL;
}
