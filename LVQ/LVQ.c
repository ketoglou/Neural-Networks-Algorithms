#include "LVQ.h"

void main(){
    Initialize();
    
    int i,j,stop = 0,t = 0; //stop is for break while,t is the season counter
    float d,old_d; //euclidean distance
    int winner_neuron;
    float lastValue0[M],secondLastValue0[M],lastValue1[M],secondLastValue1[M];
    float n = RHYTHM_OF_LEARNING;
    
    //Initialize to small values
    for(i=0;i<M;i++){
        lastValue0[i] = -1;
        lastValue1[i] = -1;
        secondLastValue0[i] = -2;
        secondLastValue1[i] = -2;
    }
    
    //IF WE WANT WE CAN SET MAXIMUN NUMBER OF SEASONS FOR EXIT WHILE
    while(1){
        //Clear dispersion
        for(i=0;i<M;i++){
            neurons[i].S = 0;
        }
        for(i=0;i<N;i++){
            for(j=0;j<M;j++){
                //find the euclidean distance (x,w)
                d = sqrt(pow((neurons[j].W[0]-vertices[i][0]),2) + pow((neurons[j].W[1]-vertices[i][1]),2));
                //select the neuron which is closest to this vert (minimum d)
                if(j == 0 || d < old_d){
                    winner_neuron = j;
                    old_d = d;
                }
            }
            addToTeam(vertices[i],winner_neuron); //add the c
            neurons[winner_neuron].S += old_d; //Compute dispersion
            neurons[winner_neuron].W[0] = neurons[winner_neuron].W[0] + (n*(vertices[i][0] - neurons[winner_neuron].W[0])); //Compute new weights for the neuron
            neurons[winner_neuron].W[1] = neurons[winner_neuron].W[1] + (n*(vertices[i][1] - neurons[winner_neuron].W[1]));
        }
        
        //Check if we must stop
        for(i=0;i<M;i++){
            if((neurons[i].W[0] == lastValue0[i] && neurons[i].W[0] == secondLastValue0[i]) && (neurons[i].W[1] == lastValue1[i] && neurons[i].W[1] == secondLastValue1[i])){
                stop = 1;
            }else{
                secondLastValue0[i] = lastValue0[i];
                lastValue0[i] = neurons[i].W[0];
                secondLastValue1[i] = lastValue1[i];
                lastValue1[i] = neurons[i].W[1];
            }
        }
        
        if(stop){
            break;
        }
        t++;
        clearTeams();
        n = n*0.95;
    }
    
    for(i=0;i<M;i++){
        printf("Team %d : W = (%f,%f)\tS = %f\n",i,neurons[i].W[0],neurons[i].W[1],neurons[i].S);
    }
    GeneratePlotFiles();
    StartPlot();
}

void Initialize(){
    FILE * fp;
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    time_t t;

    fp = fopen("s2_dataset.csv", "r");
    if (fp == NULL){
        printf("Cannot find/load file!\n");
        exit(EXIT_FAILURE);
    }
    
    float number1,number2;
    int n = 0;
    while ((read = getline(&line, &len, fp)) != -1) {
        number1 = atof(strtok(line,","));
        number2 = atof(strtok(NULL,","));
        vertices[n][0] = number1;
        vertices[n][1] = number2;
        n++;
    }
    
    fclose(fp);
    if (line)
        free(line);
    
   srand((unsigned) time(&t)); //Intializes random number generator
   
    //Initialize random neurons weights from vertices and teams vertices size
    int i,randomVert;
    for(i =0;i<M;i++){
        randomVert = rand() % 500;
        neurons[i].W[0] = vertices[randomVert][0];
        neurons[i].W[1] = vertices[randomVert][1];
        
        neurons[i].size = 0;
        neurons[i].id = i;
    }
}

void addToTeam(float *vertice,int numberOfTeam){
    if(neurons[numberOfTeam].size == 0){
        neurons[numberOfTeam].team_vertices = (float**)malloc(sizeof(float*));
        neurons[numberOfTeam].team_vertices[0] = (float*)malloc(sizeof(float)*2);
        neurons[numberOfTeam].team_vertices[0][0] = vertice[0];
        neurons[numberOfTeam].team_vertices[0][1] = vertice[1];
        neurons[numberOfTeam].size ++;
    }else{
        neurons[numberOfTeam].team_vertices = (float**)realloc(neurons[numberOfTeam].team_vertices,(sizeof(float*)*neurons[numberOfTeam].size)+sizeof(float));
        neurons[numberOfTeam].team_vertices[neurons[numberOfTeam].size] = (float*)malloc(sizeof(float)*2);
        neurons[numberOfTeam].team_vertices[neurons[numberOfTeam].size][0] = vertice[0];
        neurons[numberOfTeam].team_vertices[neurons[numberOfTeam].size][1] = vertice[1];
        neurons[numberOfTeam].size ++;
    }
}

void clearTeams(){
    int i;
    for(i=0;i<M;i++){
        free(neurons[i].team_vertices);
        neurons[i].team_vertices = NULL;
        neurons[i].size = 0;
    }
}

void GeneratePlotFiles(){
    int i,j;
    char points[40];
    printf("Generate file Team.dat for plot\n");
    FILE *fp = fopen("Team.dat" ,"a"); //create new file
    for(i=0;i<M;i++){
        char points[40];
        sprintf(points, "%f %f 1\n",neurons[i].W[0],neurons[i].W[1]);
        fprintf(fp,"%s",points);
        for(j=0;j<neurons[i].size;j++){
            memset(points,'\0',40);
            sprintf(points, "%f %f %d\n",neurons[i].team_vertices[j][0],neurons[i].team_vertices[j][1],(i+2));
            fprintf(fp,"%s",points);
        }
        memset(points,'\0',40);
    }
    fclose(fp);
}

int StartPlot(){
    int pid,status;
    if ((pid=fork()) < 0){
       perror("fork failed");
       return -1;
    }
    if(pid == 0){
        //CHILD PROCESS
       printf("Start plot...\nATTENTION : USE Ctrl+D TO EXIT PROGRAM,CLOSING PLOT DOES NOT EXIT THE PROGRAM\n");
       execl("/usr/bin/gnuplot","-c","gnuplot_script.sh",(char*)NULL);
       exit(1);
    }
    waitpid(pid,&status,0);

    //REMOVE FILE
    if (remove("Team.dat") == 0) 
      printf("Team.dat deleted successfully"); 
   else
      printf("Unable to delete the file Team.dat!"); 
	return 1;
}
