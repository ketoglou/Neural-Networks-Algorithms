#pragma once


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>  
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define M 10  //NUMBER OF TEAMS WE WANT
#define D 2 //NUMBER OF VALUES FOR EACH VERTICE
#define N 500 //NUMBER OF VERTICES

//Structure of the team
typedef struct team_{
    int id;
    float W[D]; //not weights but the centre vertice of that team
    float S; //dispersion
    float **team_vertices;
    int size;
}team;

float vertices[N][D]; //array for the vertices
team teams[M]; // teams of vertices

//define the functions
void Initialize();
void addToTeam(float *vertice,int numberOfTeam);
void clearTeams();
void GeneratePlotFiles();
int StartPlot();
