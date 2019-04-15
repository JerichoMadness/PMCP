#ifndef __STATISTICS_H__
#define __STATISTICS_H__

void createSizeString(char *sizeString, int *sizes, int n);
void createStatisticString(char *statString, char *sizeString, int *order, double costFP, int rankFP, double costMEM, int rankMEM, double timeMeasure, int n, char mode); 
int createStatisticsFile(int n);
void addStatisticsToFile(char *buffer, char *cpyBuffer, int numCol);

#endif
