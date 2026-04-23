#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define IDX(i,j,k) ((i)*(ny+pad)*(nz+pad) + (j)*(nz+pad) + (k))

int main(int argc, char *argv[]) {

    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int d, ppn, px, py, pz, nx, ny, nz, T, seed, F;
    double isovalue;

    if(argc!=13){
        if(rank==0){
            printf("Usage: mpirun -np <procs> %s d ppn px py pz nx ny nz T seed F isovalue\n", argv[0]);
        }
        MPI_Finalize();
        return 1;
    }

    if(rank==0){
        d = atoi(argv[1]);
        ppn = atoi(argv[2]);
        px = atoi(argv[3]);
        py = atoi(argv[4]);
        pz = atoi(argv[5]);
        nx = atoi(argv[6]);
        ny = atoi(argv[7]);
        nz = atoi(argv[8]);
        T = atoi(argv[9]);
        seed = atoi(argv[10]);
        F = atoi(argv[11]);
        isovalue = atof(argv[12]);
    }

    MPI_Bcast(&d, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&px, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&py, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&pz, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&nx, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&ny, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&nz, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&T, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&seed, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&F, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&isovalue, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    if (px * py * pz != size) {
        if (rank == 0) printf("Error: px*py*pz != number of processes\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    int x = rank/(py*pz);
    int y = (rank/pz)%py;
    int z = rank%pz;

    int left = (x>0)?rank-py*pz:MPI_PROC_NULL;
    int right = (x<px-1)?rank+py*pz:MPI_PROC_NULL;
    int down = (y>0)?rank-pz:MPI_PROC_NULL;
    int up = (y<py-1)?rank+pz:MPI_PROC_NULL;
    int back = (z>0)?rank-1:MPI_PROC_NULL;
    int front = (z<pz-1)?rank+1:MPI_PROC_NULL;

    int radius = (d-1)/6;
    int pad = 2*radius;
    int totalSize = (nx+pad)*(ny+pad)*(nz+pad);

    double **data = malloc(F * sizeof(double*));
    double **newdata = malloc(F * sizeof(double*));

    srand(seed);

    for(int f=0; f<F; f++){
        data[f] = malloc(totalSize*sizeof(double));
        newdata[f] = malloc(totalSize*sizeof(double));

        // initialising all to 0
        for(int i=0; i<nx+pad; i++){
            for(int j=0; j<ny+pad; j++){
                for(int k=0; k<nz+pad; k++){
                    data[f][IDX(i,j,k)] = 0.0;
                }
            }
        }

        // initialisation
        int linear_idx = 0;
        for(int i=radius; i<nx+radius; i++){
            for(int j=radius; j<ny+radius; j++){
                for(int k=radius; k<nz+radius; k++){
                    data[f][IDX(i,j,k)] = (double)rand()*(rank+1)/(110426.0+f+linear_idx);
                    linear_idx++;
                }
            }
        }
    }

    double start = MPI_Wtime();

    for(int t=0; t<T; t++){

        // halo exchange
        for(int f=0; f<F; f++){
            for(int r=0; r<radius; r++){
                
                MPI_Request reqs[4];
                // x-axis
                for(int j=radius; j<ny+radius; j++){
                    for(int k=radius; k<nz+radius; k++){
                        MPI_Irecv(&data[f][IDX(r, j, k)], 1, MPI_DOUBLE, left, 1, MPI_COMM_WORLD, &reqs[0]);
                        MPI_Irecv(&data[f][IDX(nx+radius+r, j, k)], 1, MPI_DOUBLE, right, 0, MPI_COMM_WORLD, &reqs[1]);
                        
                        MPI_Isend(&data[f][IDX(radius+r, j, k)], 1, MPI_DOUBLE, left, 0, MPI_COMM_WORLD, &reqs[2]);
                        MPI_Isend(&data[f][IDX(nx+radius-1-r, j, k)], 1, MPI_DOUBLE, right, 1, MPI_COMM_WORLD, &reqs[3]);
                        
                        MPI_Waitall(4, reqs, MPI_STATUSES_IGNORE);
                    }
                }

                // y-axis
                for(int i=radius; i<nx+radius; i++){
                    for(int k=radius; k<nz+radius; k++){
                        MPI_Irecv(&data[f][IDX(i, r, k)], 1, MPI_DOUBLE, down, 3, MPI_COMM_WORLD, &reqs[0]);
                        MPI_Irecv(&data[f][IDX(i, ny+radius+r, k)], 1, MPI_DOUBLE, up, 2, MPI_COMM_WORLD, &reqs[1]);
                        
                        MPI_Isend(&data[f][IDX(i, radius+r, k)], 1, MPI_DOUBLE, down, 2, MPI_COMM_WORLD, &reqs[2]);
                        MPI_Isend(&data[f][IDX(i, ny+radius-1-r, k)], 1, MPI_DOUBLE, up, 3, MPI_COMM_WORLD, &reqs[3]);
                        
                        MPI_Waitall(4, reqs, MPI_STATUSES_IGNORE);
                    }
                }

                // z-axis
                for(int i=radius; i<nx+radius; i++){
                    for(int j=radius; j<ny+radius; j++){
                        MPI_Irecv(&data[f][IDX(i, j, r)], 1, MPI_DOUBLE, back, 5, MPI_COMM_WORLD, &reqs[0]);
                        MPI_Irecv(&data[f][IDX(i, j, nz+radius+r)], 1, MPI_DOUBLE, front, 4, MPI_COMM_WORLD, &reqs[1]);
                        
                        MPI_Isend(&data[f][IDX(i, j, radius+r)], 1, MPI_DOUBLE, back, 4, MPI_COMM_WORLD, &reqs[2]);
                        MPI_Isend(&data[f][IDX(i, j, nz+radius-1-r)], 1, MPI_DOUBLE, front, 5, MPI_COMM_WORLD, &reqs[3]);
                        
                        MPI_Waitall(4, reqs, MPI_STATUSES_IGNORE);
                    }
                }
            }
        }

        // computation
        for(int f=0; f<F; f++){
            for(int i=radius; i<nx+radius; i++){
                for(int j=radius; j<ny+radius; j++){
                    for(int k=radius; k<nz+radius; k++){
                        int global_x = x * nx + (i - radius);
                        int global_y = y * ny + (j - radius);
                        int global_z = z * nz + (k - radius);
                        double sum = data[f][IDX(i,j,k)];
                        int count = 1;

                        for(int r=1; r<=radius; r++){
                            if(global_x-r>=0){
                                sum += data[f][IDX(i-r,j,k)]; count++;
                            }
                            if(global_x+r<px*nx){
                                sum += data[f][IDX(i+r,j,k)]; count++;
                            }
                            if(global_y-r>=0){
                                sum += data[f][IDX(i,j-r,k)]; count++;
                            }
                            if(global_y+r<py*ny){
                                sum += data[f][IDX(i,j+r,k)]; count++;
                            }
                            if(global_z-r>=0){
                                sum += data[f][IDX(i,j,k-r)]; count++;
                            }
                            if(global_z+r<pz*nz){
                                sum += data[f][IDX(i,j,k+r)]; count++;
                            }
                        }

                        newdata[f][IDX(i,j,k)] = sum/count;
                    }
                }
            }
        }

        // swap
        for(int f=0; f<F; f++){
            double *tmp = data[f];
            data[f] = newdata[f];
            newdata[f] = tmp;
        }

        // isovalue count
        int *local = calloc(F, sizeof(int));
        int *global = NULL;

        for(int f=0; f<F; f++){
            for(int i=radius; i<nx+radius; i++){
                for(int j=radius; j<ny+radius; j++){
                    for(int k=radius; k<nz+radius; k++){
                        double val = data[f][IDX(i,j,k)];
                        int gx = x*nx+(i-radius);
                        int gy = y*ny+(j-radius);
                        int gz = z*nz+(k-radius);

                        if(gx+1<px*nx){
                            double nxv = data[f][IDX(i+1,j,k)];
                            if((val<isovalue&&nxv>=isovalue)||(val>=isovalue&&nxv<isovalue)){
                                local[f]++;
                            }
                        }

                        if(gy+1<py*ny){
                            double nyv = data[f][IDX(i,j+1,k)];
                            if((val<isovalue&&nyv>=isovalue)||(val>=isovalue&&nyv<isovalue)){
                                local[f]++;
                            }
                        }

                        if(gz+1<pz*nz){
                            double nzv = data[f][IDX(i,j,k+1)];
                            if((val<isovalue&&nzv>=isovalue)||(val>=isovalue&&nzv<isovalue)){
                                local[f]++;
                            }
                        }
                    }
                }
            }
        }

        if(rank==0){
            global = malloc(F*sizeof(int));
        }
        MPI_Reduce(local, global, F, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

        if(rank==0){
            for(int f=0; f<F; f++){
                printf("%d", global[f]);
                if(f<F-1) printf(" ");
            }
            printf("\n");
        }

        free(local);
        if(rank==0) free(global);
    }

    double end = MPI_Wtime();

    if(rank==0){
        printf("%lf\n", end-start);
    }

    for(int f=0; f<F; f++){
        free(data[f]);
        free(newdata[f]);
    }
    free(data);
    free(newdata);

    MPI_Finalize();
    return 0;
}
