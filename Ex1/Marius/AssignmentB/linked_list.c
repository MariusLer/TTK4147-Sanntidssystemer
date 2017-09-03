#include <stdlib.h>
#include <stdio.h>
#include "linked_list.h"

list_t list_create()
{
  list_t list = malloc(sizeof(list_t));
  list->head = NULL;
  list->tail = NULL;
  list->length = 0;
  return list;

}

void list_delete(list_t list)
{
  if (list->head == NULL){
    free(list);
    return;
  }
  struct node* n = list->head;
  struct node* next = n->next;
  while (next != NULL){
    free(n);
    n = next;
    next = n->next;
  }
  free(n);
  free(list);
}

void list_insert(list_t list, int index, int data)
{
  struct node* insert = malloc(sizeof(struct node));
  insert->data = data;

  int current_index = 0;
  struct node* current_node = list->head;
  while (current_index != index){
    current_node = current_node->next;
    current_index++;
  }
  struct node* prev = current_node->prev;

  current_node->prev = insert;
  prev->next = insert;

  insert->next = current_node;
  insert->prev = prev;

  list->length++;
}

void list_append(list_t list, int data)
{
  struct node* insert = malloc(sizeof(struct node));
  insert->data = data;
  insert->next = NULL;
  insert->prev = NULL;

  if (list->length == 0){
    list->head = insert;
    list->tail = insert;
    list->length++;
    return;
  }
  struct node* tail = list->tail;
  insert->prev = tail;
  tail->next = insert;

  list->tail = insert;
  list->length++;
}

void list_print(list_t list)
{
  if (list->length == 0){
    printf("%s\n","Empty list");
    return;
  }
  struct node* current_node = list->head;
  while (current_node->next != NULL){
    printf("%d",current_node->data);
    printf("%s"," ");
    current_node = current_node->next;
  }
  printf("%d\n",current_node->data);
}

long list_sum(list_t list)
{
  long sum = 0;
  struct node* current_node = list->head;
  while (current_node->next != NULL){
    sum += current_node->data;
    current_node = current_node->next;
  }
  sum += current_node->data;
  return sum;
}

int list_get(list_t list, int index)
{
  int current_index = 0;
  struct node* current_node = list->head;
  while (current_index != index){
    current_node=current_node->next;
    current_index++;
  }
  return current_node->data;
}

int list_extract(list_t list, int index)
{
  // Dårlige greier, kan gjøre mye enklerre, jajaj
  if (list->length == 1){
    struct node* extract = list->head;
    int value = extract->data;
    free(extract);
    list->head = NULL;
    list->tail = NULL;
    list->length--;
    return value;
  }
  if (index == 0){
    struct node* extract = list->head;
    struct node* next_node = extract->next;
    int value = extract->data;
    free(extract);
    next_node->prev = NULL;
    list->head = next_node;
    list->length--;
    return value;
  }
  if (index == list->length){
    struct node* extract = list->tail;
    struct node* prev = extract->prev;
    int value = extract->data;
    free(extract);
    prev->next = NULL;
    list->tail = prev;
    list->length--;
    return value;
  }

  int current_index = 0;
  struct node* current_node = list->head;

  while (current_index != index){
    current_node = current_node->next;
    current_index++;
  }
  struct node* prev = current_node->prev;
  struct node* next = current_node->next;
  prev->next = next;
  next->prev = prev;

  int value = current_node->data;
  free(current_node);
  list->length--;
  return value;
}
