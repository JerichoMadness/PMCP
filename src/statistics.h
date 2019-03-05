#ifndef __STATISTICS_H__
#define __STATISTICS_H__

void createSizeString(char *sizeString, int *sizes, int n);
void createStatisticString(char *statString, char *sizeString, int *order, int costFP, int rankFP, int costMEM, int rankMEM, double timeMeasure, int n); 
int createStatisticsFile(int n);
void addStatisticsToFile(char *buffer, char *cpyBuffer, int numCol);

#endif
