#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

int main(int argc, char *argv[]){
    int myrank;               //rank of current process
    int P;                    //total number of processes
    int M = 1024*1024;        //size of data array
    int D1 = 2;               //first communication distance
    int D2 = 4;               //second communication distance
    int T = 10;               //number of iterations
    int seed = 42;            //random seed
 
    //for reading command line arguments if provided
    if (argc > 1) {
        M = atoi(argv[1]);
    }
    if (argc > 2) {
        D1 = atoi(argv[2]);
    }
    if (argc > 3) {
        D2 = atoi(argv[3]);
    }
    if (argc > 4) {
        T = atoi(argv[4]);
    }
    if (argc > 5) {
        seed = atoi(argv[5]);
    }

    MPI_Init(&argc, &argv);                 
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    MPI_Status status;                      
    MPI_Comm_size(MPI_COMM_WORLD, &P);      

    double *data_received = (double*) malloc(M * sizeof(double));
    double *buffer_updated_for_D1 = (double*) malloc(M * sizeof(double));
    double *buffer_updated_for_D2 = (double*) malloc(M * sizeof(double));
    double *data_at_D1 = (double*) malloc(M * sizeof(double));
    double *data_at_D2 = (double*) malloc(M * sizeof(double));
    double *reply_D1 = (double*) malloc(M * sizeof(double));
    double *reply_D2 = (double*) malloc(M * sizeof(double));

    //initialize random data
    srand(seed); 
    for (int i = 0; i < M; i++) {
        data_received[i] = (double)rand()*(myrank+1)/10000.0;
        buffer_updated_for_D1[i] = data_received[i];
        buffer_updated_for_D2[i] = data_received[i];
    }

    // Determine communication partners
    int dest1 = myrank + D1;
    int dest2 = myrank + D2;
    int source1 = myrank - D1;
    int source2 = myrank - D2;

    double t_start = MPI_Wtime(); 

   
    for (int t = 0; t < T; t++){
        //forward communication
        if (dest2 < P){
            MPI_Send(buffer_updated_for_D2, M, MPI_DOUBLE, dest2, 0, MPI_COMM_WORLD);
        }
        if (dest1 < P){
            MPI_Send(buffer_updated_for_D1, M, MPI_DOUBLE, dest1, 0, MPI_COMM_WORLD);
        }

        if (source2 >= 0){
            MPI_Recv(data_at_D2, M, MPI_DOUBLE, source2, 0, MPI_COMM_WORLD, &status);
        }
        if (source1 >= 0){
            MPI_Recv(data_at_D1, M, MPI_DOUBLE, source1, 0, MPI_COMM_WORLD, &status);
        }

        
        //square values received from D1
        if (source1 >= 0){
            for (int ij = 0; ij < M; ij++){
                data_at_D1[ij] = data_at_D1[ij] * data_at_D1[ij];
            }
        }

        //apply log transformation for D2 (only positive values)
        if (source2 >= 0){
            for (int ij = 0; ij < M ; ij++){
                if (data_at_D2[ij] > 0) data_at_D2[ij] = log(data_at_D2[ij]);
            }
        }

        //backward communication
        if (source2 >= 0){
            MPI_Send(data_at_D2, M, MPI_DOUBLE, source2, 1, MPI_COMM_WORLD);
        }
        if (source1 >= 0){
            MPI_Send(data_at_D1, M, MPI_DOUBLE, source1, 1, MPI_COMM_WORLD);
        }

        if (dest2 < P){
            MPI_Recv(reply_D2, M, MPI_DOUBLE, dest2, 1, MPI_COMM_WORLD, &status);
        }
        if (dest1 < P){
            MPI_Recv(reply_D1, M, MPI_DOUBLE, dest1, 1, MPI_COMM_WORLD, &status);
        }

        
        //modulo transformation for D1 buffer
        if (dest1 < P) {
            for (int i = 0; i < M; i++) {
                buffer_updated_for_D1[i] = (double)((unsigned long long)reply_D1[i] % 100000);
            }
        }
        
        //scaling transformation for D2 buffer
        if (dest2 < P) {
            for (int i = 0; i < M; i++) {
                buffer_updated_for_D2[i] = reply_D2[i] * 100000.0;
            }
        }
    }

    double t_end = MPI_Wtime(); 

    //local maximum calculations
    double local_max_D1 = -1.0e20;
    double local_max_D2 = -1.0e20;

    if (dest1 < P) {
        for(int k = 0; k < M; k++) {
            if(buffer_updated_for_D1[k] > local_max_D1){
                local_max_D1 = buffer_updated_for_D1[k];
            }
            if(buffer_updated_for_D2[k] > local_max_D2){
                local_max_D2 = buffer_updated_for_D2[k];
            }
        }
    }

    // rank 0 collects global maximums
    if (myrank == 0){
        double global_max_D1 = -1.0e20;
        double global_max_D2 = -1.0e20;

        if (dest1 < P) {
            global_max_D1 = local_max_D1;
            global_max_D2 = local_max_D2;
        }

        double maximum_received[2]; 

        //receive maximums from other processes
        for (int r = 1; r < P; r++){
            if (r + D1 < P){
                MPI_Recv(maximum_received, 2, MPI_DOUBLE, r, 99, MPI_COMM_WORLD, &status);
                if (maximum_received[0] > global_max_D1){
                    global_max_D1 = maximum_received[0];
                }
                if (maximum_received[1] > global_max_D2){
                    global_max_D2 = maximum_received[1];
                }
            }
        }

        
        printf("%lf %lf %lf\n", global_max_D1, global_max_D2, t_end - t_start);

    }
    else{
        //Sending  local maximums to rank 0
        if (dest1 < P){
            double maximum_values[2];
            maximum_values[0] = local_max_D1;
            maximum_values[1] = local_max_D2;
            MPI_Send(maximum_values, 2, MPI_DOUBLE, 0, 99, MPI_COMM_WORLD);
        }
    }

    
    free(data_received);
    free(data_at_D1);
    free(data_at_D2);
    free(reply_D1);
    free(reply_D2);
    free(buffer_updated_for_D1);
    free(buffer_updated_for_D2);

    MPI_Finalize(); 
}
