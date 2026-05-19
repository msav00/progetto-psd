#include <time.h>
#pragma once

//Length of the report identifier
#define REPORT_ID_SIZE 19

typedef enum type {
    Road,
    Illumination,
    GarbageDisposal,
    PublicServices
} IssueType;

typedef enum priority {
    High = 2,
    AboveNormal = 1,
    Normal = 0,
    BelowNormal = -1,
    Low = -2,
} IssueUrgency;

typedef enum state {
    Open,
    Ongoing,
    Closed
} IssueState;

typedef struct {
    char* id;
    char* issuer_name;
    IssueType category;
    char* desc;
    struct tm* time;
    IssueUrgency priority;
    IssueState state;
} Report;

Report *newReport(IssueType category, char* name, char* description, struct tm* date);
void setReportState(Report *report, IssueState state);
void setReportUrgency(Report *report, IssueUrgency priority);
void deleteReport(Report *report);

void printReport(Report *report);