#include "report.h"
#pragma once

typedef struct {
    int total_reports;
    int categories[4];
    int states[3];
    IssueType most_frequent;
} Overview;

Overview *newOverview();

void deleteOverview(Overview *overview);