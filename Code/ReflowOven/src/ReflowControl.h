
#ifndef REFLOWCONTROL_H
#define  REFLOWCONTROL_H

void SetProfileValue(char DataSet, int Value);
void PrintReflowDataSet();
float PIDLoop(float CurrentTemp, float SetTemp);
void StartISR();
void StopISR();
void RunProfile();
void ReflowStop();
void ReflowStart();
void ReflowSetProfile(char in);
char ReflowReadProfile();

#endif