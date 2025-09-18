#include "header_file.h"
#include <stdio.h>
#include <string.h>
#include<unistd.h>
#include<stdlib.h>

int main(int argc, char* argv[])
{
    // Interactive menu if no arguments passed
    if (argc > 1)
    {

        printf("\033[1;31m");
    printf("Please run ./a.out without arguments to use the interactive menu\n");
    printf("\033[0m");
    return 0;
    }
    while(1)
    {
        printf("MP3 Tag Reader and Editor\n");
        printf("1. View Tags\n");
        printf("2. Edit Tags\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        
        int choice = 0;
        int ret = scanf("%d", &choice);
        while(getchar() != '\n'); 
        if (ret != 1 || choice < 1 || choice > 3)
        {
            printf("\033[1;31m");
            printf("Invalid input\n");
            printf("\033[0m");
            continue;
        }
        
        
        if (choice == 3)
        {
            printf("\033[1;31m");
            printf("Exiting.\n");
            printf("\033[0m");
            break;
        }
        
        char filename[256];

        
        
        if (choice == 1)
        {
            printf("Enter the MP3 filename: ");
            scanf("%s", filename);
            FILE* fptr = fopen(filename, "r");
            if (fptr == NULL)
            {
                printf("\033[1;31m");
                printf("Unable to open file '%s'\n", filename);
                printf("\033[0m");
                continue ;
            }
            
            if (read_tags(fptr) != 0)
            {
                printf("\033[1;31m");
                printf("Failed to read tags\n");
                printf("\033[0m");
            }
            else
            {
                printf("\033[1;32m");
                printf("Tags displayed successfully\n");
                printf("\033[0m");
            }

            fclose(fptr);
            
        }
        if (choice == 2)

        {
            printf("Enter the MP3 filename: ");
            scanf("%s", filename);
            FILE* file_check = fopen(filename, "r");
            if (file_check == NULL)
            {
                printf("\033[1;31mFile '%s' not found or cannot be opened.\n\033[0m", filename);
                choice = 0;
                continue;
            }
            if (!check_id3_version(file_check))
            {
                fclose(file_check);
                choice = 0;
                continue;  // back to menu if not supported
            }
            fclose(file_check);
            char tag_menu[6][8] = {"TIT2", "TPE1", "TALB", "TYER", "COMM", "TCON"};
            char *tag_names[] = {"Title", "Artist", "Album", "Year", "Composer", "Genre"};

            // Calculate how many tags are in the menu
            int menu_count = sizeof(tag_menu) / sizeof(tag_menu[0]);  // 48/8=6

            // Ask the user which tag they want to edit
            printf("Select tag to edit:\n");
            for (int i = 0; i < menu_count; i++)
            {
                printf("%d. %s\n", i + 1, tag_names[i]);
            }

            printf("Enter your choice: ");
            int tag_choice = 0;
            scanf("%d", &tag_choice);

            if (tag_choice < 1 || tag_choice > menu_count)
            {
                printf("\033[1;31m");
                printf("Invalid choice\n");
                printf("\033[0m");
                continue;
            }

            char new_value[150];
            printf("Enter new value: ");
            getchar();


            if (fgets(new_value, sizeof(new_value), stdin) == NULL) // reading that new value from the keyboard input to be stored and written into the MP3 tag.
            {//It stops reading after a newline character or EOF (end of file).
                printf("\033[1;31m");
                printf("Input error\n");
                printf("\033[0m");
                return 1;
            }
            

            // Remove newline character if present
            size_t len = strlen(new_value);
            if (len > 0 && new_value[len - 1] == '\n')//checks if the last character (new_value[len - 1]) is a newline ('\n').
            {//If yes, it replaces the newline with a null character ('\0') to remove the newline.
                new_value[len - 1] = '\0';
            }

            FILE* src = fopen("temp.mp3", "w");
            if (src == NULL)
            {
                printf("\033[1;31m");
                printf("Error: Unable to open temporary file for writing.\n");
                printf("\033[0m");
                return 1;
            }
            FILE* des = fopen(filename, "r");
            if (des == NULL)
            {
                printf("\033[1;31m");
                printf("Error: Unable to open original file '%s' for reading.\n", filename);
                printf("\033[0m");
                fclose(src);
                return 1;
            }

            if (!src || !des)
            {
                 printf("\033[1;31m");    
                printf("Error opening files\n");
                 printf("\033[0m");    
                if (src)
                    fclose(src);
                if (des)
                    fclose(des);
                return 1;
            }

   
            int status = 1;//to store the return value of the function copy_remain()
          
            switch (tag_choice)
            {
                case 1: 
                    status = copy_remain(src, des, new_value, "TIT2");
                    break;

                case 2: 
                    status = copy_remain(src, des, new_value, "TPE1");
                    break;

                case 3: 
                    status = copy_remain(src, des, new_value, "TALB");
                    break;

                case 4:
                    status = copy_remain(src, des, new_value, "TYER");
                    break;

                case 5: 
                    status = copy_remain(src, des, new_value, "COMM");
                    break;

                case 6:
                    status = copy_remain(src, des, new_value, "TCON");
                    break;

                default:
                     printf("\033[1;31m");    
                    printf("Invalid tag choice\n");
                     printf("\033[0m");    
                    fclose(src);
                    fclose(des);
                    return 1 ;
            }

            

            if (status == 0)
            {
                 printf("\033[1;32m");    
                printf("Successfully changed the tag\n");
                 printf("\033[0m");    
                fclose(src);
                fclose(des);

                if (remove(filename) != 0)//remove mp3 file 
                {
                    printf("\033[1;31m");    
                    printf("Error deleting original file\n");
                     printf("\033[0m");
                }    
                else if (rename("temp.mp3", filename) != 0)
                {
                    printf("\033[1;31m");    
                    printf("Error renaming temp file\n");
                     printf("\033[0m");   
                } 
                else
                {
                     printf("\033[1;32m");    
                    printf("Original file updated successfully\n");
                     printf("\033[0m");  
                }  
            }
            else
            {
                 printf("\033[1;31m");    
                printf("Failed to change the tag\n");
                 printf("\033[0m");    
                fclose(src);
                fclose(des);
                remove("temp.mp3");
            }
            printf("Do you want to view the updated tags? (y/n): ");
            
            char view_decision;
            scanf("%c",&view_decision);

            if (view_decision == 'y' || view_decision == 'Y')
            {
                continue ;
            }
            else
            {
                break;
            }


            return 0;
        }

    }
    

    
}
