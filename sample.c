/*
Name            :Prathima Vyas Maddineni
Date            :15-12-2023
Description     :WAP to remove dulpicate elements in a given array
Sample input    :Enter the size: 5
                 Enter elements into the array: 5 1 3 1 5
Sample output   :After removing duplicates: 5 1 3
DOC*/

#include"main.h"

void fun(int arr1[],int size);

int main()
{
    int size;
    printf("Enter the size : ");
    scanf("%d",&size);
    int arr1[size];
    printf("Enter elements into the array : ");
    fun(arr1,size);

}

void fun(int *arr1,int size)
{
    for(int i = 0; i < size; i++)
    {
        scanf("%d",&arr1[i]);
    }
    printf("After removing duplicates ");
    for(int i = 0; i < size; i++)
    {
        for(int j = i+1; j < size; j++)
        {
            if ( arr1[i] == arr1[j] )
            {
                arr1[i] = 0;
            }

        }
        if(arr1[i] != 0)
        {
            printf("%d ",arr1[i]);
        }
    }
        printf("\n");
}
