/* Do not make any changes in the skeleton. Your submission will be invalidated if the skeleton is changed */

# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <stdbool.h>
# include <pthread.h>

void read_grid_from_file(int size, char *ip_file, int grid[36][36]) {
	FILE *fp;
	int i, j;
	fp = fopen(ip_file, "r");
	for (i=0; i<size; i++) 
		for (j=0; j<size; j++) {
			fscanf(fp, "%d", &grid[i][j]);
	}
} 

void print_grid(int size, int grid[36][36]) {
	int i, j;
	/* The segment below prints the grid in a standard format. Do not change */
	for (i=0; i<size; i++) {
		for (j=0; j<size; j++)
			printf("%d ", grid[i][j]);
		printf("\n");
	}
}

/* Solver functions start */

struct params {
	int size;
	int grid[36][36];
	int row;
	int col;
	bool result;
};

void copyGrid(int fromGrid[36][36], int toGrid[36][36], int size) {
	for(int i=0; i<size; i++) {
		for(int j=0; j<size; j++) {
			toGrid[i][j] = fromGrid[i][j];
		}
	}
}

bool markEmpty(int size, int grid[36][36], int *row, int *col) {
	for(int i=0; i<size; i++) {
		for(int j=0; j<size; j++) {
			if(grid[i][j] == 0) {
				*row = i;
				*col = j;
				return true;
			}
		}
	}
	return false;
}

bool inRow(int size, int grid[36][36], int row, int num) {
	for(int i=0; i<size; i++) {
		if(grid[row][i] == num) return true;
	}
	return false;
}

bool inCol(int size, int grid[36][36], int col, int num) {
	for(int i=0; i<size; i++) {
		if(grid[i][col] == num) return true;
	}
	return false;
}

bool inBox(int boxSize, int grid[36][36], int row, int col, int num) {
	for(int i=row; i<row+boxSize; i++) {
		for(int j=col; j<col+boxSize; j++) {
			if(grid[i][j] == num) return true;
		}
	}
	return false;
}

bool isValid(int size, int grid[36][36], int row, int col, int num) {
	int boxSize = sqrt(size);
	return !inRow(size, grid, row, num) && !inCol(size, grid, col, num) && !inBox(boxSize, grid, row - row%boxSize, col - col%boxSize, num);
}

void* solveGrid(void *args) {
	struct params* data = (struct params*) args;
	int size = data->size;
	int row = data->row;
	int col = data->col;
	
	if(!markEmpty(size, data->grid, &row, &col)) {
		data->result = true;
		return NULL;
	}

	/* Multithreads */
	pthread_t thread[size];
	struct params* clone[size];
	bool init[size];
	
	for(int i=0; i<size; i++) {
		if(isValid(size, data->grid, row, col, i+1)) {
			data->grid[row][col] = i+1;

			init[i] = true;
			clone[i] = (struct params*) malloc(sizeof(struct params));
			copyGrid(data->grid, clone[i]->grid, size);
			clone[i]->size = data->size;
			clone[i]->result = false;

			pthread_create(&thread[i], NULL, solveGrid, (void *)clone[i]);
		}
		else init[i] = false;
	}

	for(int i=0; i<size; i++) {
		if(init[i]) pthread_join(thread[i], NULL);
	}

	for(int i=0; i<size; i++) {
		if(init[i] && clone[i]->result == true) {
			copyGrid(clone[i]->grid, data->grid, size);
			data->result = true;
			return NULL;
		}
	}
}

void solve(int size, int grid[36][36]) {
	struct params* data = (struct params*) malloc(sizeof(struct params));

	copyGrid(grid, data->grid, size);
	data->size = size;
	data->result = false;

	solveGrid((void *)data);

	copyGrid(data->grid, grid, size);
}

/* Solver functions end */

int main(int argc, char *argv[]) {
	int grid[36][36], size, i, j;
	
	if (argc != 3) {
		printf("Usage: ./sudoku.out grid_size inputfile");
		exit(-1);
	}
	
	size = atoi(argv[1]);
	read_grid_from_file(size, argv[2], grid);
	
	/* Do your thing here */
	solve(size, grid);

	print_grid(size, grid);
}