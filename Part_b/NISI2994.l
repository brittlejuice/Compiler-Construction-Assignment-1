%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <stdbool.h>
    #include <ctype.h>
    #include <math.h>
    int valid_cnt = 0;
    char buf[400];
    char *s;
    int flag = 0;
    char studId[30];
    int comments_cnt = 0;
    int no_of_courses = 0;
    int cgpa = 0;
    int valid_record = 0;
    int correct_guy_flag = 0;
    int current_course_index = 0;
    bool new_course = 1;
    char new_course_id[30];
    int new_course_units;
    int new_course_grade;
    int first_time_for_this_guy = 1;
    char courses[30][32]; // grade is in courses[i][30]; UNIT is in courses[i][31]; for the ith course;
    int min(int a, int b){
        return a>b?b:a;
    }
%}

%s RECORD COURSE COURSEID COURSEUNIT COURSEGRADE

%%
"//"(.)* { comments_cnt++; BEGIN(0); }

"$"[ ]* {BEGIN(RECORD); }



<RECORD>[A-Z]+[0-9]+ {
    if(strcmp(yytext,studId)==0 ){      
 
        correct_guy_flag = 1;
    }
    else 
    {correct_guy_flag=0;} 
    first_time_for_this_guy = 1;
    BEGIN(COURSE);
    }// $LAWDA123 <CSF111 4 A> <> <>/n
<RECORD>[^(A-Z)]*[^(0-9)]* {BEGIN(0);}
<RECORD>.\n {BEGIN(0);}

<COURSE>[ ]*"<"[ ]* {BEGIN(COURSEID);}
    /*<COURSE>">"[ ]*"<"[ ]* {BEGIN(COURSEID);}*/
    /*<COURSE>">"[ ]*\n {BEGIN(0); valid_record++;}*/
<COURSE>[ ]*\n {BEGIN(0);}
<COURSEID>[A-Z]+[0-9]+[ ]* {
    if(correct_guy_flag)
    {
        new_course = 1;
        for(int i=0; i<no_of_courses; i++) /* check if new or not*/
        {
            bool this_is_same_string = true;
            for(int j=0; j<min(yyleng,30); j++)
            {
                if(yytext[j]!=courses[i][j]) {this_is_same_string = false; break;}
            }
            if(yyleng!=30 && courses[i][yyleng]!=' ') this_is_same_string = false;
            if(this_is_same_string) // 
            {
                new_course = false;
                current_course_index = i;
                break;
            }
        }
        if(new_course) /*if new */
        {
            current_course_index = no_of_courses ; 
            
            no_of_courses++;
            for(int i=0; i<30; i++) new_course_id[i] = ' ';
            for(int i=0; i<yyleng; i++)
            {
                if(yytext[i]==' ') break;
                new_course_id[i] = yytext[i];
            }
            
        }
    }
    BEGIN(COURSEUNIT);
}
<COURSEUNIT>[2-4][ ]* {BEGIN(COURSEGRADE); new_course_units = yytext[0]-48;}
<COURSEUNIT>[^(2|3|4)][ ]*">"[ ]* {BEGIN(COURSE); if(correct_guy_flag && new_course) no_of_courses--;} /* not valid */ 

<COURSEGRADE>(A-|A|B-|B|C)[ ]*">"[ ]* {
    if(correct_guy_flag)
    {
        if(new_course_id[0]=='*') {no_of_courses--; }
        else
        {
            if(new_course==true)
            for(int i=0; i<30; i++)
            {
                courses[current_course_index][i] = new_course_id[i];
            }
            courses[current_course_index][30] = new_course_units+48; // char
            courses[current_course_index][31] = yytext[0];
            if(yyleng>1 && yytext[1]=='-') courses[current_course_index][31] += 32; 
            }
        }
    if(first_time_for_this_guy) {valid_record++; first_time_for_this_guy--;}
    BEGIN(COURSE);
}
<COURSEGRADE>[^(A-|A|B-|B|C)][ ]*">"[ ]* {if(new_course && correct_guy_flag)no_of_courses--; BEGIN(COURSE);}
<COURSEGRADE>[ ]*">"[ ]* { if(new_course && correct_guy_flag)no_of_courses--; BEGIN(COURSE);}
%%
int extract_cred(char c){
    if(c == 'A') return 10;
    else if(c == 'a') return 8;
    else if(c == 'B') return 6;
    else if(c == 'b') return 4;
    else if(c == 'C') return 2;
}
int main(){
    FILE* file_ptr;
    file_ptr = fopen("input.txt","r");
    char ch;
    int idx = 0;
    while((ch = fgetc(file_ptr)) != EOF && idx < 30){
        studId[idx++] = ch;
    }
    fclose(file_ptr);
    yyin = fopen("data.txt","r");
    for(int i=0; i<30; i++) for(int j=0; j<32; j++) courses[i][j] = ' ';
    yylex();
    
    file_ptr = fopen("output.txt","w");
    double tot_units=0,units=0; cgpa=0;
    for(int i = 0; i<no_of_courses; i++){
        units = courses[i][30]-48;
        tot_units += units;
        cgpa += 100* units * extract_cred(courses[i][31]);
    }
    cgpa = round(cgpa / tot_units);
    if(no_of_courses==0) cgpa=0;
    fprintf(file_ptr,"@%d@%d@%d@%d#",valid_record,comments_cnt,no_of_courses,cgpa);
}