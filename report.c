#include "report.h"
#include <string.h>

#include <stdio.h>
#include <stdlib.h>

//Length of the report identifier
#define ID_SIZE 19

char *generateId(Report *report);

//Creates a new report and returns a pointer to it
Report *newReport(IssueType category, char* name, char* description) {
    Report *report = malloc(sizeof(Report));
    report->category = category;
    //Get time
    time_t cur_time = time(NULL);
    report->time = gmtime(&cur_time);
    //Allocate string fields and populate
    report->issuer_name = malloc(strlen(name));
    strcpy(report->issuer_name, name);
    report->desc = malloc(strlen(description));
    strcpy(report->desc, description);
    //Set Issue-related information
    setReportState(report, Open);
    setReportUrgency(report, Normal);
    //Generate the ID from date and category
    report->id = malloc(ID_SIZE);
    report->id = generateId(report);
    return report;
}

void setReportState(Report *report, IssueState state) {
    report->state = state;
}

void setReportUrgency(Report *report, IssueUrgency priority) {
    report->priority = priority;
}

//Frees report from memory
void deleteReport(Report *report) {
    //Free all strings
    free(report->id);
    free(report->issuer_name);
    free(report->desc);
    //Then free the report itself
    free(report);
}

//Generate an Id from the given report data and a random number
char *generateId(Report *report) {
    char* string_denom;
    switch (report->category)
    {
        case Road:
            string_denom = "ROAD";
            break;
        case Illumination:
            string_denom = "ILLU";
            break;
        case GarbageDisposal:
            string_denom = "GRBG";
            break;
        case PublicServices:
            string_denom = "SRVC";
            break;
        default:
            string_denom = "UNKN";
            break;
    }
    //ID consists of a 4 character denominator + 2 dashes + 8 chars of date/time + 4 random digits and the terminator
    //Template: IDID-DDMMYYYY-RAND
    //Example:  SRVC-07052026-8732
    char* full_id = malloc(sizeof(char)*ID_SIZE);
    snprintf(full_id, ID_SIZE, "%s-%02d%02d%04d-%04d\0", string_denom, report->time->tm_mday, report->time->tm_mon+1, report->time->tm_year+1900, rand()%9999);
    return full_id;
}