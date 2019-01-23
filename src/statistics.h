#ifndef __STATISTICS_H__
#define __STATISTICS_H__

#include <stdio.h>
#include <unistd.h>

int createStatisticsFile(int n);
void addStatisticsToFile(char *buffer, int numCol);

#endif
