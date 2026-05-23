#include "report.h"
#include <string.h>

#include <stdio.h>
#include <stdlib.h>

char *generateId(Report *report);

/// @brief Generates a new report with the given details (Status and Urgency are set to default values).
/// @param category Type of category for the report.
/// @param name String containing the report issuer's name.
/// @param description String containing a description for the report.
/// @param date Date/time structure for the issued report.
/// @return Pointer to the newly created report.
Report *newReport(IssueType category, char* name, char* description, struct tm* date) {
    Report *report = malloc(sizeof(Report));
    report->category = category;
    report->time = date;
    //Allocate string fields and populate
    report->issuer_name = malloc(strlen(name));
    strcpy(report->issuer_name, name);
    report->desc = malloc(strlen(description));
    strcpy(report->desc, description);
    //Set Issue-related information to their default values
    setReportState(report, Open);
    setReportUrgency(report, Normal);
    //Generate the ID from date and category
    report->id = malloc(REPORT_ID_SIZE);
    report->id = generateId(report);
    return report;
}

void setReportState(Report *report, IssueState state) {
    if (report) report->state = state;
}

void setReportUrgency(Report *report, IssueUrgency priority) {
    if (report) report->priority = priority;
}

/// @brief Frees the given report from memory.
/// @param report Pointer to the report to delete.
void deleteReport(Report *report) {
    if (!report) return;
    //Free all strings
    free(report->id);
    free(report->issuer_name);
    free(report->desc);
    //Then free the report itself
    free(report);
}

/// @brief Generate a string identifier for the given report.
/// @param report Pointer to the report to generate an identifier for.
/// @return Pointer to the string containing the identifier.
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
    //ID consists of a 4 character category denominator + 2 dashes + 8 chars of date/time + 4 random digits and the terminator
    //Template: IDID-DDMMYYYY-RAND
    //Example:  SRVC-07052026-8732
    char* full_id = malloc(sizeof(char)*REPORT_ID_SIZE);
    snprintf(full_id, REPORT_ID_SIZE, "%s-%02d%02d%04d-%04d\0", string_denom, report->time->tm_mday, report->time->tm_mon+1, report->time->tm_year+1900, rand()%9999);
    return full_id;
}

/// @brief Prints the given report to terminal in a pretty format.
/// @param report Pointer to the report to display.
void printReport(Report *report) {
    char date_buf[18];
    strftime(date_buf, sizeof(date_buf), "%B %d %Y %H:%M", report->time);
    char *str_state = "Unknown";
    switch (report->state) {
        case Open: str_state = "Open";
        break;
        case Ongoing: str_state = "Ongoing";
        break;
        case Closed: str_state = "Closed";
        break;
    }
    char *str_urgency = "Unknown";
    switch (report->priority)
    {
        case Low: str_urgency = "Low";
        break;
        case BelowNormal: str_urgency = "Below Normal";
        break;
        case Normal: str_urgency = "Normal";
        break;
        case AboveNormal: str_urgency = "Above Normal";
        break;
        case High: str_urgency = "High";
        break;
    }
    printf("Report #%s:\n\tIssuer: %s\n\tDescription: %s\n\tDate: %s\n\tStatus: %s\n\tPriority: %s\n", report->id, report->issuer_name, report->desc, date_buf, str_state, str_urgency);
}