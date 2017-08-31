#include <stdlib.h>
#include <stdio.h>
#include "linked_list.h"

list_t list_create(){
	list_t emptyList= (list_t) malloc(sizeof(struct list));
	emptyList->length=0;
	emptyList->head= NULL;
	emptyList->tail= NULL;
	return emptyList;

}

void list_delete(list_t list){
	//Get first node
	struct node *Next = list->head;
	struct node *Old;
	//goto next and remove old element from list
	while(Next!=NULL){
		Old=Next;
		Next=list->head->next;
		free(Old);
	}
	//free list holder
	free(list);
}

void list_insert(list_t list, int index, int data)
{	//use append if tail or after
	if (index >= list->length){
		list_append(list,data);
		return;
	}

	//allocate memory for element and set data
	struct node *temp =(struct node*) malloc(sizeof(struct node));
	temp->data=data;
	temp->next=NULL;  //just in case of index 0

	//goto index
	struct node *Walker=list->head;
	for (int i=0;i<index;i++){
		Walker=Walker->next;
	}
	//insert
	temp->prev=Walker->prev;
	temp->next=Walker;
	if(Walker->prev!=NULL){
		Walker->prev->next=temp;
	}
	Walker->prev=temp;
	list->length++;
	//if index 0 change head
	if (index==0){
		list->head=temp;
	}


}

void list_append(list_t list, int data){
	//allocate memory and set data
	struct node *temp =(struct node*) malloc(sizeof(struct node));
	temp->data=data;
	temp->prev=NULL;
	temp->next=NULL;
	//if first element	
	if (list->length==0){
		list->head=temp;
		list->tail=temp;
		list->length++;
		return;
	}
	temp->prev=list->tail;
	list->tail->next=temp;
	list->tail=temp;
	list->length++;
}

void list_print(list_t list)
{ 	if(list->length<=0){return;}
	struct node *Walker = list->head;
	while (Walker!=list->tail){
		printf("%i", Walker->data);
		if (list->length>0){
			Walker=Walker->next;
		}
	}
	printf("%i", Walker->data);
		

}

long list_sum(list_t list){
	long sum=0;
	
	for(struct node *Walker=list->head;Walker!=NULL;Walker=Walker->next){
		sum+= (long) Walker->data;
	}
	return sum;
}

int list_get(list_t list, int index){
	struct node *walker =list->head;
	for(int i=0; i< index; i++){
		walker=walker->next;
	}
	return walker->data;
}

int list_extract(list_t list, int index){	
	if(list->length==1){
		int temp= list->head->data;
		free(list->head);
		list->head=NULL;
		list->tail=NULL;
		
		list->length=0;
		return temp;
	}
		



	struct node *Walker = list->head;
	for(int i=0;i<index;i++){
		Walker=Walker->next;
	}
	int temp = Walker->data;
	if (Walker==list->head && list->length>1){
		list->head= Walker->next;
		list->head->prev=NULL;	
	}else{
		if(Walker==list->tail && list->length>1){
		 	list->tail=Walker->prev;
			list->tail->next=NULL;
		}
		else{
			if(list->length>1){
			Walker->next->prev=Walker->prev;
			Walker->prev->next=Walker->next;
			}
		}
	}
	free(Walker);
	list->length--;
	if(list->length==0){
		list->head=NULL;
		list->tail=NULL;
	}
	return temp;
		
}
