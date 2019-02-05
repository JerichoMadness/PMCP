#ifndef __STATISTICS_H__
#define __STATISTICS_H__

void createStatisticString(char *statString, char *sizeString, int *order, int costFP, int costMEM, unsigned long timeMeasure, int n); 
void createSizeString(char *sizeString, int *sizes, int n);
int createStatisticsFile(int n);
void addStatisticsToFile(char *buffer, char *cpyBuffer, int numCol);

#endif
