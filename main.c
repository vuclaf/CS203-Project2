#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "course.h"
#include "courseNode.h"
#include "courseLinkedList.h"
#include "department.h"
#include "degree.h"
#include "degreeArrayList.h"
#include "courseBST.h"
#include "deptArrayList.h"
#include "studentBST.h"

#define MAX 256

DegreeArrayList *degreeArray;
DeptArrayList *deptArray;
StudentBST *studentbst;

/*
Function : cleanInput
------------------------------------------
Clean the input string from fgets of any end character

input: pointer to the input string

*/
char *cleanInput(char *input)
{
    if (input[0] == ' ')
    {
        input[0] = NULL;
        input++;
    }
    input[strcspn(input, "\r\n")] = '\0';
    return input;
}

/*
Function : readDept
------------------------------------------
Read in the name of the department and store the information

input: pointer to stdin
name: name of the department

*/
void readDept(FILE *input, char *name)
{
    name = cleanInput(name);
    Department *dept = createDept(name);
    //printf("Department name is: %s\n", dept->name);
    while (!feof(input))
    {
        char *code = (char *)malloc(MAX * sizeof(char));
        char *cname = (char *)malloc(MAX * sizeof(char));
        char *preReqs = (char *)malloc(MAX * sizeof(char));
        char *token = (char *)malloc(MAX * sizeof(char));

        fgets(code, MAX, input);
        code = strtok(code, "\n");
        code = cleanInput(code);
        fgets(cname, MAX, input);
        cname = strtok(cname, "\n");
        cname = cleanInput(cname);
        fgets(preReqs, MAX, input);

        Course *course = createCourse(code, cname);
        CourseLinkedList *cll = course->pre;

        if (strncmp(preReqs, "OR", 2) == 0)
        {
            preReqs = preReqs + 3;
            token = strtok(preReqs, ",");
            while (token != NULL)
            {
                token = cleanInput(token);
                insertCourseLinkedList(cll, token);
                token = strtok(NULL, ",");
            }
        }
        else
        {
            preReqs = cleanInput(preReqs);
            insertCourseLinkedList(cll, preReqs);
        }
        course->pre = cll;
        addCourseDept(dept, course);
    }
    insertDeptArrayList(deptArray, dept);
    free(name);
}

/*
Function : readStudent
------------------------------------------
Read in the name of the student and store the information

input: pointer to stdin
name: name of the department

*/
void readStudent(FILE *input, char *name)
{
    name = cleanInput(name);
    char *studentName = (char *)malloc(MAX * sizeof(char));
    strcpy(studentName, name);
    insertStudentBST(studentbst, studentName);
    StudentBSTNode *student = searchStudentBST(studentbst, studentName);

    char *degree = (char *)malloc(MAX * sizeof(char));
    fgets(degree, MAX, input);
    degree = cleanInput(degree);
    addDegreeStudent(student, degree);

    char *preReq = (char *)malloc(MAX * sizeof(char));
    while (!feof(input))
    {
        fgets(preReq, MAX, input);
        preReq = cleanInput(preReq);
        char *curr = (char *)malloc(MAX * sizeof(char));
        strcpy(curr, preReq);
        addCompletedCourseStudent(student, curr);
    }
    free(name);
    free(studentName);
    free(degree);
    free(preReq);
}

/*
Function : readDeg
------------------------------------------
Read in the name of the degree and store the information

input: pointer to stdin
name: name of the degree

*/
void readDeg(FILE *input, char *name)
{
    name = strtok(name, "\n");
    name = cleanInput(name);
    Degree *deg = createDeg(name);
    while (!feof(input))
    {
        char *preReqs = (char *)malloc(MAX * sizeof(char));
        char *token = (char *)malloc(MAX * sizeof(char));

        fgets(preReqs, MAX, input);

        DegreeReq *degReq = deg->req;

        if (strncmp(preReqs, "OR", 2) == 0)
        {
            preReqs = preReqs + 3;
            token = strtok(preReqs, ",");
            token = cleanInput(token);
            CourseLinkedList *cll = createCourseLinkedList();
            while (token != NULL)
            {
                token = cleanInput(token);
                insertCourseLinkedList(cll, token);
                token = strtok(NULL, ",");
            }
            insertDegreeReq(degReq, cll);
        }
        else
        {
            CourseLinkedList *cll2 = createCourseLinkedList();
            preReqs = cleanInput(preReqs);
            insertCourseLinkedList(cll2, preReqs);
            insertDegreeReq(degReq, cll2);
        }
    }
    insertDegreeArrayList(degreeArray, deg);
    free(name);
}

/*
Function : fileInput
------------------------------------------
Read in information from a text file given the directory

fileName: directory of the text file including ".txt"
*/
void fileInput(char *fileName)
{
    FILE *input = fopen(fileName, "r");
    if (input == NULL)
    {
        printf("Could not open file\n");
        return;
    }
    char *line = (char *)malloc(MAX * sizeof(char));
    fgets(line, MAX, input);
    line = cleanInput(line);
    if (strcmp(line, "DEPARTMENT") == 0)
    {
        char *name;
        name = (char *)malloc(MAX * sizeof(char));
        fgets(name, MAX, input);
        readDept(input, name);
        fclose(input);
        free(line);
        return;
    }
    else if (strcmp(line, "DEGREE") == 0)
    {
        char *name1;
        name1 = (char *)malloc(MAX * sizeof(char));
        fgets(name1, MAX, input);
        readDeg(input, name1);
        fclose(input);
        free(line);
        return;
    }
    else if (strcmp(line, "STUDENT") == 0)
    {
        char *name2;
        name2 = (char *)malloc(MAX * sizeof(char));
        fgets(name2, MAX, input);
        readStudent(input, name2);
        fclose(input);
        free(line);
        return;
    }
    else
    {
        printf("Invalid file opener");
        fclose(input);
        free(line);
        return;
    }
}

/*
Function : commandC
------------------------------------------
Search the department for the course

param: code name of the course

*/
void commandC(char *param)
{
    for (int i = 0; i < deptArray->size; i++)
    {
        Department *dep = deptArray->list[i];
        if (dep == NULL)
            return;
        CourseBSTNode *cn = searchCourseBST(dep->courses, param);
        if (cn == NULL)
            continue;
        Course *c = cn->course;
        if (c != NULL)
        {
            printCourse(c);
        }
    }
}

/*
Function : commandD
------------------------------------------
Search the degree list for the degree

param: code name of the degree

*/
void commandD(char *param)
{
    Degree *deg = getDegreeArrayList(degreeArray, param);
    if (deg != NULL)
        printDeg(deg);
}

/*
Function : commandS
------------------------------------------
Search the lists of Courses to see what are the courses
that have the code name of the course for prerequisite

param: code name of the course

*/
void commandS(char *param)
{
    //clls is the final list of courses that have this prereq
    //currcll is the temporary list in the department being searched
    CourseLinkedList *clls = createCourseLinkedList();
    CourseLinkedList *currcll;
    for (int i = 0; i < deptArray->size; i++)
    {
        //Going through each department in the department array
        Department *dep = deptArray->list[i];
        if (dep == NULL)
            break;
        //Search the department for the courses with this prereq
        //and put it into currcll
        currcll = checkPrereqCourseBST(dep->courses, param);
        //Add each element of currcll to clls
        if (currcll != NULL)
        {
            CourseNode *currnode = currcll->first;
            while (currnode != NULL)
            {
                if (currnode->data != NULL)
                {
                    char *newData = (char *)malloc(MAX * sizeof(char));
                    strcpy(newData, currnode->data);
                    insertCourseLinkedList(clls, newData);
                    currnode = currnode->next;
                }
            }
            //Continue to the next department in the department array
            continue;
        }
    }
    printf("Take next: ");
    printCourseLinkedList(clls);
    //Check for the degrees that contain these courses
    /*
    The while loop traverse through the cll that holds the
    classes that have param as a prereq

    Temp is a degree array that holds the result of the search
    */
    CourseNode *cn = clls->first;
    DegreeArrayList *temp = createDegreeArrayList();
    while (cn != NULL)
    {
        /*
        For each of the courses that have param as prereq,
        search that course in each of the degree of the 
        degreeArray
        */
        char *code = cn->data;
        for (int i = 0; i < degreeArray->size; i++)
        {
            Degree *deg = degreeArray->list[i];
            if (deg == NULL)
                return;
            DegreeReqNode *res = searchDegreeReq(degreeArray->list[i]->req, code);
            /*
            If found, skip to the next degree in the array
            */
            if (res != NULL)
            {
                /*
                Check if this degree is already in temp by searching for it
                doesn't insert when it is already in temp
                */
                if (getDegreeArrayList(temp, deg->name) == NULL)
                {
                    insertDegreeArrayList(temp, deg);
                    continue;
                }
                else
                    continue;
            }
        }
        cn = cn->next;
    }
    //Loop to just print the names
    printf("Degrees:");
    for (int i = 0; i < temp->size; i++)
    {
        printf("%s, ", temp->list[i]->name);
    }
    printf("\n");
    free(clls);
    free(temp);
}

/*
Function : commandP()
*/
void commandP(char *type, char *param)
{
    if (strcmp(type, "c") == 0)
    {
        commandC(param);
        //check to find department and degree
    }
    if (strcmp(type, "g") == 0)
    {
        commandD(param);
    }
    if (strcmp(type, "d") == 0)
    {
        Department *dep = getDeptArrayList(deptArray, param);
        if (dep == NULL)
            return;
        printDept(dep);
    }
    if (strcmp(type, "s") == 0)
    {
        StudentBSTNode *temp = searchStudentBST(studentbst, param);
        if (temp == NULL)
        {
            printf("NOT FOUND");
            return;
        }
        printf("%s\n", temp->name);
        printCourseLinkedList(temp->degs);
        printf("Completed: ");
        printCourseLinkedList(temp->completed);
    }
}

/*
Function : commandM
------------------------------------------
Get input from the command line (stdin)
to execute waned functions

userInput: the input string recorded from stdin

*/
void commandM(char *param)
{
    StudentBSTNode *student = searchStudentBST(studentbst, param);
    DegreeArrayList *copyDegArr = createDegreeArrayList();
    CourseNode *currDeg = student->degs->first;
    if (currDeg == NULL)
    {
        printf("Student does not declare a degree\n");
        return;
    }
    while (currDeg != NULL)
    {
        Degree *degree = (Degree *)malloc(sizeof(Degree));
        Degree *searchedDeg = getDegreeArrayList(degreeArray, currDeg->data);
        if (searchedDeg == NULL)
        {
            printf("Declared Degree Not Found\n");
            return;
        }
        memcpy(degree, searchedDeg, sizeof(Degree));
        DegreeReq *copyReq = degree->req;
        if (student->completed == NULL)
        {
            printf("Student does not have class list\n");
            return;
        }
        CourseNode *curr = student->completed->first;
        if (curr == NULL)
        {
            printf("Student has not taken any class\n");
            return;
        }
        while (curr != NULL)
        {
            removeDegreeReq(copyReq, curr->data);
            curr = curr->next;
        }
        insertDegreeArrayList(copyDegArr, degree);
        currDeg = currDeg->next;
    }
    printDegreeArrayList(copyDegArr);
    free(copyDegArr);
}

/*
Function : commandMwithReturn
------------------------------------------
Get input from the command line (stdin)
to execute waned functions

userInput: the input string recorded from stdin

*/
DegreeArrayList *commandMwithReturn(char *param)
{
    StudentBSTNode *student = searchStudentBST(studentbst, param);
    DegreeArrayList *copyDegArr = createDegreeArrayList();
    CourseNode *currDeg = student->degs->first;
    if (currDeg == NULL)
    {
        printf("Student does not declare a degree\n");
        return NULL;
    }
    while (currDeg != NULL)
    {
        Degree *degree = (Degree *)malloc(sizeof(Degree));
        Degree *searchedDeg = getDegreeArrayList(degreeArray, currDeg->data);
        if (searchedDeg == NULL)
        {
            printf("Declared Degree Not Found\n");
            return NULL;
        }
        memcpy(degree, searchedDeg, sizeof(Degree));
        DegreeReq *copyReq = degree->req;
        if (student->completed == NULL)
        {
            printf("Student does not have class list\n");
            return NULL;
        }
        CourseNode *curr = student->completed->first;
        if (curr == NULL)
        {
            printf("Student has not taken any class\n");
            return NULL;
        }
        while (curr != NULL)
        {
            removeDegreeReq(copyReq, curr->data);
            curr = curr->next;
        }
        insertDegreeArrayList(copyDegArr, degree);
        currDeg = currDeg->next;
    }
    return copyDegArr;
}

/*
commandSwithReturn
*/
CourseLinkedList *commandSwithReturn(char *param)
{
    //clls is the final list of courses that have this prereq
    //currcll is the temporary list in the department being searched
    CourseLinkedList *clls = createCourseLinkedList();
    CourseLinkedList *currcll;
    for (int i = 0; i < deptArray->size; i++)
    {
        //Going through each department in the department array
        Department *dep = deptArray->list[i];
        if (dep == NULL)
            break;
        //Search the department for the courses with this prereq
        //and put it into currcll
        currcll = checkPrereqCourseBST(dep->courses, param);
        //Add each element of currcll to clls
        if (currcll != NULL)
        {
            CourseNode *currnode = currcll->first;
            while (currnode != NULL)
            {
                if (currnode->data != NULL)
                {
                    char *newData = (char *)malloc(MAX * sizeof(char));
                    strcpy(newData, currnode->data);
                    insertCourseLinkedList(clls, newData);
                    currnode = currnode->next;
                }
            }
            //Continue to the next department in the department array
            continue;
        }
    }
    if (clls->first == NULL)
        return NULL;
    return clls;
}

/*
Function : commandN
------------------------------------------

*/
void commandN(char *param)
{
    //Get the student from the bst
    StudentBSTNode *student = searchStudentBST(studentbst, param);
    if (student == NULL)
    {
        printf("Student Not Found\n");
        return;
    }
    //classesLeft stores the classes the student still have to take for their degree through commandM
    DegreeArrayList *classesLeft = commandMwithReturn(param);
    if (classesLeft->size == 0 || classesLeft->list == NULL)
    {
        printf("No Classes left\n");
        return;
    }

    //classesCanTake runs commandS through all of the students completed classes
    DegreeReq *classesCanTake = createDegreeReq();
    CourseLinkedList *completed = student->completed;
    if (completed == NULL)
        return;
    CourseNode *curr = completed->first;
    if (curr == NULL)
        return;
    //Run each of student's completed courses on S to find the classes
    //the student still have to take next
    while (curr != NULL)
    {
        CourseLinkedList *source = commandSwithReturn(curr->data);
        if (source == NULL || source->first == NULL)
        {
            curr = curr->next;
            continue;
        }
        CourseLinkedList *des = createCourseLinkedList();
        //Need to write a routine to full copy
        memcpy(des, source, sizeof(CourseLinkedList));

        //Check if the courses in des are in classesLeft
        CourseNode *currsmall = des->first;
        while (currsmall != NULL)
        {
            for (int i = 0; i < classesLeft->size; i++)
            {
                Degree *currdeg = classesLeft->list[i];
                DegreeReq *currdegreq = currdeg->req;
                DegreeReqNode *currnode = searchDegreeReq(currdegreq, currsmall->data);
                //If the course is not in classesLeft, remove it from des
                if (currnode == NULL)
                {
                    removeCourseLinkedList(des, currsmall->data);
                }
            }
            currsmall = currsmall->next;
        }
        //If des is now empty then no need to insert it, just skip
        if (des->first == NULL)
        {
            curr = curr->next;
            continue;
        }
        insertDegreeReq(classesCanTake, des);
        curr = curr->next;
    }
    printDegreeReq(classesCanTake);
}

/*
commandR
*/
void commandR(char *depname, char *cname)
{
    printf("%s\n", cname);
    for (int i = 0; i < deptArray->size; i++)
    {
        Department *currDep = deptArray->list[i];
        CourseBST *currcbst = currDep->courses;
        removePrereqDept(currDep, cname);
        removeCourseBST(currcbst, cname);
    }
    for (int i = 0; i < degreeArray->size; i++)
    {
        Degree *curr = degreeArray->list[i];
        DegreeReq *currDegReq = curr->req;
        removeDegreeReq(currDegReq, cname);
    }
}

/*
commandQ
*/
void commandQ(char *degname, char *cname)
{
    Degree *currdeg = getDegreeArrayList(degreeArray, degname);
    if (currdeg == NULL)
    {
        printf("Degree not found\n");
        return;
    }
    DegreeReq *currDegReq = currdeg->req;
    removeDegreeReq(currDegReq, cname);
}

/*
commandA
*/
void commandA(char *depname, char *code, char *name, char *preReqs)
{
    Department *dep = getDeptArrayList(deptArray, depname);
    Course *newCourse = createCourse(code, name);
    //Why do we need the next line?
    //newCourse->pre = createCourseLinkedList();
    CourseLinkedList *cll = newCourse->pre;
    if (strncmp(preReqs, "OR", 2) == 0)
    {
        preReqs = preReqs + 3;
        char *token = strtok(preReqs, ",");
        while (token != NULL)
        {
            token = cleanInput(token);
            insertCourseLinkedList(cll, token);
            token = strtok(NULL, ",");
        }
    }
    addCourseDept(dep, newCourse);
}

/*
Function : commandLine
------------------------------------------
Get input from the command line (stdin)
to execute waned functions

userInput: the input string recorded from stdin

*/
void commandLine(char *userInput)
{
    if (strcmp(strtok(userInput, "\n"), "x") == 0)
    {
        exit(0);
    }
    char *command = strtok(userInput, " "); //extract the command
    if (strcmp(command, "c") == 0)
    {
        char *courseName = strtok(userInput + 2, "\n");
        commandC(courseName);
    }
    else if (strcmp(command, "d") == 0)
    {
        char *degName = strtok(userInput + 2, "\n");
        commandD(degName);
    }
    else if (strcmp(command, "s") == 0)
    {
        char *courseName = strtok(userInput + 2, "\n");
        commandS(courseName);
    }
    else if (strcmp(command, "p") == 0)
    {
        char *typeName = strtok(userInput + 2, "\n");
        char *type = strtok(typeName, " ");
        char *name = typeName + 2;
        commandP(type, name);
    }
    else if (strcmp(command, "m") == 0)
    {
        char *studentName = strtok(userInput + 2, "\n");
        commandM(studentName);
    }
    else if (strcmp(command, "n") == 0)
    {
        char *studentName = strtok(userInput + 2, "\n");
        commandN(studentName);
    }
    else if (strcmp(command, "r") == 0)
    {
        char *whole = strtok(userInput + 2, "\n");
        char *depname = strtok(whole, ",");
        char *cname = strtok(NULL, ",");
        cname = cleanInput(cname);
        commandR(depname, cname);
    }
    else if (strcmp(command, "q") == 0)
    {
        char *whole = strtok(userInput + 2, "\n");
        char *depname = strtok(whole, ",");
        char *cname = strtok(NULL, ",");
        cname = cleanInput(cname);
        commandQ(depname, cname);
    }
    else if (strcmp(command, "a") == 0)
    {
        char *whole = strtok(userInput + 2, "\n");
        char *depname = strtok(whole, ",");
        char *ccode = strtok(NULL, ",");
        ccode = cleanInput(ccode);
        char *cname = strtok(NULL, ",");
        cname = cleanInput(cname);
        char *preReqs = strtok(NULL, "\n");
        preReqs = cleanInput(preReqs);
        commandA(depname, ccode, cname, preReqs);
    }
}

int main(int argc, char **argv)
{
    degreeArray = createDegreeArrayList();
    deptArray = createDeptArrayList();
    studentbst = createStudentBST();
    for (int i = 1; i < argc; i++)
    {
        fileInput(argv[i]);
    }
    while (1)
    {
        printf("Type in a command\n");
        char userInput[MAX];
        fgets(userInput, MAX, stdin);
        commandLine(userInput);
    }
}