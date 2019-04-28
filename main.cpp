#include <stdio.h>
#include <stdlib.h>

#define STACK_MAX_SIZE 256
#define IGCT 8

typedef enum {
    INT,
    TWIN
}ObjectType;

typedef struct sObject{
        ObjectType type;
        unsigned char marked;

        struct sObject* next;

    union {
        int value;

        struct {
            struct sObject* head;
            struct sObject* tail;
        };
    };
}Object;

typedef struct {
    Object* stack[STACK_MAX_SIZE];
    int stackSize;

    Object* firstObject;

    int numObjects;

    int maxObjects;

}VM;

void push(VM* vm, Object* value){
     vm->stack[vm->stackSize++]=value;
 }
Object* pop(VM* vm){
    return vm->stack[--vm->stackSize];
 }
 VM* newVM(){
     VM* vm =(VM*)malloc(sizeof(VM));
     vm->stackSize=0;
     vm->firstObject=NULL;
     vm->numObjects=0;
     vm->maxObjects=IGCT;
     return vm;
 }
 void mark(Object* object){
     if (object->marked) return;

     object->marked=1;

     if (object->type==TWIN){
         mark(object->head);
         mark(object->tail);
     }
 }
 void markAll(VM*vm){
     for (int i = 0; i < vm->stackSize; ++i) {
         mark(vm->stack[i]);
     }
 }

 void marksPeep(VM* vm){
     Object** object =&vm->firstObject;
     while (*object){
         if (!(*object)->marked){
             Object* unreached=*object;

             *object=unreached->next;
             free(unreached);

             vm->numObjects--;
         } else {
             (*object)->marked=0;
             object=&(*object)->next;
         }
     }
 }

 void gc(VM* vm){
     int numObjects=vm->numObjects;

     markAll(vm);
     marksPeep(vm);

     vm->maxObjects=vm->numObjects*2;

     printf("Collected %d objects, %d left.\n", numObjects-vm->numObjects,vm->numObjects);
 }

Object* newObject(VM* vm, ObjectType type){
     if (vm->numObjects==vm->maxObjects) gc(vm);

     Object* object=(Object*)malloc(sizeof(Object));
     object->type=type;
     object->next=vm->firstObject;
     vm->firstObject=object;
     object->marked=0;

     vm->numObjects;

    return object;
 }

 void pushInt(VM* vm, int intValue){
      Object* object=newObject(vm, INT);
      object->value=intValue;

      push(vm, object);

 }
  Object* pushPair(VM* vm){
     Object* object=newObject(vm,TWIN);
     object->tail=pop(vm);
     object->head=pop(vm);

     push(vm, object);
     return object;
 }

 void objectPrint(Object* object){
     switch (object->type){
         case INT:
             printf("%d",object->value);
             break;

         case TWIN:
             printf("(");
             objectPrint(object->head);
             printf(", ");
             objectPrint(object->tail);
             printf(")");
             break;
     }
 }
void freeVM(VM* vm){
    vm->stackSize=0;
    gc(vm);
    free(vm);
}

void firstTest(){
     printf("1: Objects one the stack are preserved.\n");
     VM* vm=newVM();
     pushInt(vm,1);
     pushInt(vm,2);

     gc(vm);
     freeVM(vm);
 }

 void secondTest(){

     printf("2: Unreached objects are collected.\n");
     VM* vm=newVM();
     pushInt(vm,1);
     pushInt(vm,2);

     gc(vm);
     freeVM(vm);
 }

 void thirdTest(){
     printf("3: Reach the nested objects.\n");

     VM* vm=newVM();
     pushInt(vm,1);
     pushInt(vm,2);
     pushPair(vm);
     pushInt(vm,3);
     pushInt(vm,4);
     pushPair(vm);
     pushPair(vm);

     gc(vm);
     freeVM(vm);

 }

 void fourthTest(){
     printf("4: Cycles.\n");
     VM* vm =newVM();
     pushInt(vm,1);
     pushInt(vm,2);
    Object* a=pushPair(vm);
     pushInt(vm,3);
     pushInt(vm,4);
    Object* b=pushPair(vm);

    a->tail=b;
    b->tail=a;

    gc(vm);
    freeVM(vm);

 }


 void performance() {
     printf("Performance of GC. \n");
     VM* vm=newVM();

     for (int i=0;i<1000; i++){
         for (int j=0;j<20;j++){
             pushInt(vm, i);
         }
         for (int k = 0; k < 20; k++) {
             pop(vm);
         }
     }
     freeVM(vm);
 }


int main() {
        firstTest();
        secondTest();
        thirdTest();
        fourthTest();
        performance();
    return 0;
}



/*This uses the MIT License:

Copyright (c) 2013 Robert Nystrom

Permission is hereby granted, free of charge, to
any person obtaining a copy of this software and
        associated documentation files (the "Software"),
to deal in the Software without restriction,
including without limitation the rights to use,
copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software,
and to permit persons to whom the Software is
furnished to do so, subject to the following
conditions:

The above copyright notice and this permission
        notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT
        WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
        THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
        PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO
EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR
        OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
        WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
        THE SOFTWARE.*/
