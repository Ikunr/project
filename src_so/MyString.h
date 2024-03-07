#ifndef __MYSTRING_H_
#define __MYSTRING_H_


#define Init_MyString(obj,str)  MyString obj;Initialize(&obj,str);
typedef struct String MyString;
struct String 
{
        char * string;
        int size;

        void (*print)(MyString*obj);
        int (*isEqual)(MyString*obj1,MyString*obj2);
        int (*IsContains)(MyString *dest,MyString*src);
        int (*StrinSize)(MyString*obj);
        void (*removeString)(MyString*dest,const char *str);
        void (*insertString)(MyString *dest,const char *str,int index);
};


void Initialize(MyString *obj,const char *str);
void FreeMyString(MyString *dest);


#endif