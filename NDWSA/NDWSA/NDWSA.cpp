/* This is a Multi-Objective nondominant sorting weightless swarm algorithm program.
**********************************************************************
*  This program is the implementation of the NDWSA improved by       *
*                                                                    *
*  Lei.Shi based on the NSGA-II proposed by                          *
*                                                                    *
*  Prof. Kalyanmoy Deb and his students .                            *
*                                                                    *
*  copyright Kalyanmoy Deb
**********************************************************************

18.08.2003: The keepaliven.h file is modified to have normalized
crowding distance calculation. The previous version of
the code did not have this feature. This way, maintaining
a good distribution of solutions in problems having quite
a different range of objective functions were difficult.
Hopefully, with this modification, such difficulties will
not appear. --  K. Deb
18.08.2003: Also the dfit.h file is deleted. It was not needed any way.

The user have to give the input manualy or through a data file.

The user needs to enter objective functions in func-con.h
The code can also take care of the constraints. Enter the constraints
in the space provided in the func-con.h file.
Constraints must be of the following type:
g(x) >= 0.0
Also normalize all constraints (see the example problem in func-con.h)

If your program asks you to increase the values of some parameters in the
program come to main program and accordingly changed the values which are
defined against #define ...

The program generates few output files. These are described as
1.output.out
*           This file has the detailed record for all the variables,
*           the fitness values, constraint values, overall constraint
violation (penalty)  and their ranks for all the members
*           of old population in the left hand side of the |**|
*           and of new population in the right hand side.

2.all_fitness.out
*         This file prints the record of all the fitness values for
*         different individual of new popultion created at all
*         generations.

3.g_rank_record.out
*        This file maintains the record of individuals in global pop-
*        -ulation at different ranks for all the generations.

4.ranks.out
*         This file prints the number of individual at different ranks
*          in old and new population and finds rank ratios

5.final_fitness.out
*                 This file has the fitness value of all feasible and
non-dominated individuals at the final generation

6.final_var.out
*                 This file has the all the variables of the feasible
and non-dominated individuals at the final generation.
The i-th solutions here corresponds to the i-th solution
in the final_fitness.out file.

7.plot.out        This file contains gnuplot-based file for plotting
the non-dominated feasible solutions obtained by the code.
*************************************************************************
*         This is recommended to delete or rename all the *.out files
*         obtained from the previous runs as some files are opened in
*         append mode so they give false resemblence of data if the
*         user is not careful

Compilation procedure: Utilizing the Microsoft studio 2013 to finish the
compilation of this program.

Input data files: Three files are included, but at one time one is needed
depending on the type of variables used:
inp-r (template file input-real)  : All variables are real-coded
inp-b (template file input-binary): All variables are binary-coded
inp-rb(template file input-rl+bin): Some variables are real and some are binary
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<time.h>
#include<windows.h>
#define square(x) ((x)*(x))
#define maxpop   500  /*Max population */
#define generation 200 /*max geneartion*/
#define maxchrom 200  /*Max chromosome length*/
#define maxvar    40  /*Max no. of variables*/
#define maxfun    10  /*Max no. of functions */
#define maxcons   20  /*Max no. of Constraints*/

#define V_max 0.15    /*the maximum velocity*/
#define	PI 3.141592653589			/*  */
#define c1 0          /*the coefficient of the locate best term*/
#define c2 3.5        /*the coefficient of the global best term*/
#define w  0          /*the weight of the last generation individuals*/
int gener,       /*No of generations*/
nvar, nchrom,    /*No of variables*/
ncons,         /*No of Constraints*/
vlen[maxvar],  /*Array to store no of bits for each variable*/
//nmut,          /* No of Mutations */
//ncross,        /*No of crossovers*/
ans;
float seed,      /*Random Seed*/
pcross,        /*Cross-over Probability*/
pmut_b, pmut_r,          /*Mutation Probability*/
lim_b[maxvar][2], lim_r[maxvar][2];/*Limits of variable in array*/
float di,        /*Distribution Index for the Cross-over*/
//dim,           /*Distribution Index for the Mutation*/
delta_fit,     /* variables required forfitness for fitness sharing */
min_fit,
front_ratio;
int optype,      /*Cross-over type*/
nfunc,         /*No of functions*/
sharespace;    /*Sharing space (either parameter or fitness)*/

double coef[maxvar]; /*Variable used for decoding*/

static int popsize,  /*Population Size*/
chrom;             /*Chromosome size*/

typedef struct       /*individual properties*/
{
	int genes[maxchrom], /*bianry chromosome*/
	rank,              /*Rank of the individual*/
	flag;              /*Flag for ranking*/
	float xreal[maxvar], /*list of real variables*/
		xbin[maxvar];      /*list of decoded value of the chromosome */
	float fitness[maxfun],/*Fitness values */
		constr[maxcons],     /*Constraints values*/
		cub_len,             /*crowding distance of the individual*/
		error;              /* overall constraint violation for the individual*/
	float particle_v[maxvar];         //记录每个个体的当前代速度向量  
	float loc_best[maxvar];  //每个个体局部最优向量
	//个体的局部最优适应度,有局部最优向量计算而来


}individual;        /*Structure defining individual*/

float gfit[generation][maxfun];                           //全局最优适应度,有全局最优向量计算而来 
float glo_best[generation][maxvar];         //全局最优向量
typedef struct
{
	int maxrank;            /*Maximum rank present in the population*/
	float rankrat[maxpop];  /*Rank Ratio*/
	int rankno[maxpop];     /*Individual at different ranks*/
	individual ind[maxpop], /*Different Individuals*/
		*ind_ptr;
	float gfit1[maxfun];
	float glo_best1[maxvar];
}population;             /*Popuation Structure*/


#include "random.h"       /*Random Number Generator*/

#include "input.h"        /*File Takes Input from user*/

#include "realinit.h"     /*Random Initialization of the populaiton*/

#include "init.h"         /*Random Initialization of the population*/

#include "decode.h"       /*File decoding the binary dtrings*/

#include "ranking.h"      /*File Creating the Pareto Fronts*/

#include "rancon.h"       /*File Creating the Pareto Fronts when
						 Constraints are specified*/
#include "func-con.h"     /*File Having the Function*/

//#include "select.h"       /*File for Tournament Selection*/

//#include "crossover.h"    /*Binary Cross-over*/

//#include "uniformxr.h"    /*Uniform Cross-over*/

//#include "realcross2.h"   /*Real Cross-over*/

//#include "mut.h"          /*Binary Mutation*/

//#include "realmut1.h"     /*Real Mutation*/

#include "keepaliven.h"   /*File For Elitism and Sharing Scheme*/

#include "report.h"       /*Printing the report*/

population oldpop,
newpop,
matepop,
*old_pop_ptr,
*new_pop_ptr,
*mate_pop_ptr;
/*Defining the population Structures*/


/*to inilize the parent population*/
void initial(population *pop_ptr)
{
	float *f1_ptr, *f2_ptr;
	int *ptr1, *ptr2;
	int i, j, k, n;
	realinit(pop_ptr);
	func(pop_ptr);     /*to calculate the fitness of each individual of the old population */
	// find the gloab optimal vector

	for (k = 0; k<popsize - 1; k++)
	{
		pop_ptr->ind_ptr = &(pop_ptr->ind[k]);
		ptr1 = &(pop_ptr->ind_ptr->rank);
		f1_ptr = &(pop_ptr->ind_ptr->cub_len);
		pop_ptr->ind_ptr = &(pop_ptr->ind[k + 1]);
		ptr2 = &(pop_ptr->ind_ptr->rank);
		f2_ptr = &(pop_ptr->ind_ptr->cub_len);
		if (*ptr1>*ptr2)
		{
			for (j = 0; j < nfunc; j++)
			{
				pop_ptr->gfit1[j] = pop_ptr->ind[k + 1].fitness[j];

			}
			for (n = 0; n < nvar; n++)
			{
				pop_ptr->glo_best1[n] = pop_ptr->ind[k + 1].loc_best[n];
			}
		}
		else
		{
			if (*ptr1 < *ptr2)
			{
				for (j = 0; j < nfunc; j++)
				{
					pop_ptr->gfit1[j] = pop_ptr->ind[k].fitness[j];

				}
				for (n = 0; n < nvar; n++)
				{
					pop_ptr->glo_best1[n] = pop_ptr->ind[k].loc_best[n];
				}
			}
			else
			{
				if (*f1_ptr < *f2_ptr)
				{
					for (j = 0; j < nfunc; j++)
					{
						pop_ptr->gfit1[j] = pop_ptr->ind[k + 1].fitness[j];
					}
					for (n = 0; n < nvar; n++)
					{
						pop_ptr->glo_best1[n] = pop_ptr->ind[k + 1].loc_best[n];
					}
				}
				else
				{
					for (j = 0; j < nfunc; j++)
					{
						pop_ptr->gfit1[j] = pop_ptr->ind[k].fitness[j];
					}
					for (n = 0; n < nvar; n++)
					{
						pop_ptr->glo_best1[n] = pop_ptr->ind[k].loc_best[n];
					}
				}
			}
		}
	}
}

/*to generate the new population*/
void renew_particle(population *pop_ptr1, population *pop_ptr)
{
	int i, j;
	for (i = 0; i<popsize; i++)
	{
		for (j = 0; j<nvar; j++){
			pop_ptr1->ind[i].xreal[j] = pop_ptr->ind[i].xreal[j] + pop_ptr->ind[i].particle_v[j];
			if (pop_ptr1->ind[i].xreal[j] >lim_r[j][1])
			{
				pop_ptr1->ind[i].xreal[j] = lim_r[j][1];
			}
			if (pop_ptr1->ind[i].xreal[j] <lim_r[j][0])
			{
				pop_ptr1->ind[i].xreal[j] = lim_r[j][0];
			}
		}
	}
}

/*to update the variables of the new population */
void renew_var(population *pop_ptr1, population *pop_ptr)
{
	int i, j, k, n;
	func(pop_ptr1);/*to calculate the fitness of each individual of the new population */
	for (i = 0; i<popsize; i++)
	{
		if (pop_ptr1->ind[i].rank>pop_ptr->ind[i].rank)    /* update the locate best vector*/
		{
			for (j = 0; j < nfunc; j++){ pop_ptr1->ind[i].fitness[j] = pop_ptr->ind[i].fitness[j]; }
			for (j = 0; j < nvar; j++)       /* update the locate best vector*/
			{
				pop_ptr1->ind[i].loc_best[j] = pop_ptr->ind[i].xreal[j];
			}
		}
		else
		{
			if (pop_ptr1->ind[i].rank < pop_ptr->ind[i].rank)
			{
				for (j = 0; j < nvar; j++)       /* update the locate best vector*/
				{
					pop_ptr1->ind[i].loc_best[j] = pop_ptr1->ind[i].xreal[j];
				}
			}
			else
			{
				if (pop_ptr1->ind[i].cub_len < pop_ptr->ind[i].cub_len)
				{
					for (j = 0; j < nfunc; j++){ pop_ptr1->ind[i].fitness[j] = pop_ptr->ind[i].fitness[j]; }
					for (j = 0; j<nvar; j++)       /* update the locate best vector*/
					{
						pop_ptr1->ind[i].loc_best[j] = pop_ptr->ind[i].xreal[j];
					}
				}
				else
				{
					for (j = 0; j < nvar; j++)       /* update the locate best vector*/
					{
						pop_ptr1->ind[i].loc_best[j] = pop_ptr1->ind[i].xreal[j];
					}
				}
			}
		}
	}
	//update the gloab optima vector
	for (k = 0; k<popsize - 1; k++)
	{
		if (pop_ptr1->ind[k].rank>pop_ptr1->ind[k + 1].rank)
		{
			for (j = 0; j < nfunc; j++)
			{
				pop_ptr1->gfit1[j] = pop_ptr1->ind[k + 1].fitness[j];
			}
			for (n = 0; n < nvar; n++){ pop_ptr1->glo_best1[n] = pop_ptr1->ind[k + 1].loc_best[n]; }
		}
		else
		{
			if (pop_ptr1->ind[k].rank < pop_ptr1->ind[k + 1].rank)
			{
				for (j = 0; j < nfunc; j++)
				{
					pop_ptr1->gfit1[j] = pop_ptr1->ind[k].fitness[j];

				}
				for (n = 0; n < nvar; n++){ pop_ptr1->glo_best1[n] = pop_ptr1->ind[k].loc_best[n]; }
			}
			else
			{
				if (pop_ptr1->ind[k].cub_len < pop_ptr1->ind[k + 1].cub_len)
				{
					for (j = 0; j < nfunc; j++)
					{
						pop_ptr1->gfit1[j] = pop_ptr1->ind[k + 1].fitness[j];
					}
					for (n = 0; n < nvar; n++){ pop_ptr1->glo_best1[n] = pop_ptr1->ind[k + 1].loc_best[n]; }
				}
				else
				{
					for (j = 0; j < nfunc; j++)
					{
						pop_ptr1->gfit1[j] = pop_ptr1->ind[k].fitness[j];
					}
					for (n = 0; n < nvar; n++){ pop_ptr1->glo_best1[n] = pop_ptr1->ind[k].loc_best[n]; }
				}
			}
		}
	}
	/*update the velocity of individuals*/
	for (i = 0; i<popsize; i++)
	{
		for (j = 0; j<nvar; j++)
		{
			pop_ptr1->ind[i].particle_v[j] = w*pop_ptr->ind[i].particle_v[j] +
				c1*randomperc()*(pop_ptr1->ind[i].loc_best[j] - pop_ptr1->ind[i].xreal[j]) +
				c2*randomperc()*(pop_ptr1->glo_best1[j] - pop_ptr1->ind[i].xreal[j]);
			if (pop_ptr1->ind[i].particle_v[j] > V_max)
			{
				pop_ptr1->ind[i].particle_v[j] = V_max;
			}
			if (pop_ptr1->ind[i].particle_v[j] < -V_max)
			{
				pop_ptr1->ind[i].particle_v[j] = -V_max;
			}
		}
	}
}
int main()
{
	/*Some Local variables to this Problem (Counters And some other pointers*/
	DWORD dwStart = GetTickCount();/*to record the start time of the running of the code*/
	/*the declaration of the time of code running */
	double duration;
	int i, j, l, f, maxrank1;
	float *ptr, tot;
	FILE
		*rep_ptr,
		*gen_ptr,
		*rep2_ptr,
		*end_ptr,
		*g_var,
		*lastit;
	/*File Pointers*/
	rep_ptr = fopen("output.out", "w");
	gen_ptr = fopen("all_fitness.out", "w");
	rep2_ptr = fopen("ranks.out", "w");
	end_ptr = fopen("final_fitness.out", "w");
	g_var = fopen("final_var.out", "w");
	lastit = fopen("plot.out", "w");
	/*Opening the files*/

	old_pop_ptr = &(oldpop);
	//nmut = 0;
	//ncross = 0;

	/*Get the input from the file input.h*/
	input(rep_ptr);
	fprintf(rep_ptr, "Results in a file\n");
	fprintf(end_ptr, "# Last generation population (Feasible and non-dominated)\n");
	fprintf(end_ptr, "# Fitness_vector (first %d)  Constraint_violation (next %d)  Overall_penalty\n", nfunc, ncons);
	fprintf(g_var, "#Feasible Variable_vectors for non-dominated solutions at last generation\n");
	fprintf(g_var, "# Real (first %d)  Binary (next %d)\n", nvar, nchrom);
	fprintf(lastit, "# Feasible and Non-dominated Objective Vector\n");

	/*Initialize the random no generator*/
	warmup_random(seed);

	/*Binary Initializaton*/
	if (nchrom > 0)
		init(old_pop_ptr);
	if (nvar > 0)

		initial(old_pop_ptr);

	//old_pop_ptr = &(oldpop);

	// decode binary strings
	//decode(old_pop_ptr);

	old_pop_ptr = &(oldpop);
	new_pop_ptr = &(newpop);

	for (j = 0; j < popsize; j++)
	{
		/*Initializing the Rank array having different individuals
		at a particular  rank to zero*/
		old_pop_ptr->rankno[j] = 0;
		new_pop_ptr->rankno[j] = 0;
	}

	//old_pop_ptr = &(oldpop);

	//func(old_pop_ptr);
	/*Function Calculaiton*/

	fprintf(rep_ptr, "----------------------------------------------------\n");
	fprintf(rep_ptr, "Statistics at Generation 0 ->\n");
	fprintf(rep_ptr, "--------------------------------------------------\n");

	/********************************************************************/
	/*----------------------GENERATION STARTS HERE----------------------*/
	for (i = 0; i < gener; i++)
	{
		printf("Generation = %d\n", i + 1);
		old_pop_ptr = &(oldpop);
		new_pop_ptr = &(newpop);
		//mate_pop_ptr = &(matepop);
		fprintf(rep_ptr, "Population at generation no. -->%d\n", i + 1);
		fprintf(gen_ptr, "#Generation No. -->%d\n", i + 1);
		fprintf(gen_ptr, "#Variable_vector  Fitness_vector Constraint_violation Overall_penalty\n");


		/*to generate the new population;*/
		renew_particle(new_pop_ptr, old_pop_ptr);
		/*to update the variables of the new population */
		old_pop_ptr = &(oldpop);
		new_pop_ptr = &(newpop);
		renew_var(new_pop_ptr, old_pop_ptr);
		new_pop_ptr = &(newpop);


		/*--------SELECT----------------*/
		//nselect(old_pop_ptr, mate_pop_ptr);

		//new_pop_ptr = &(newpop);
		//mate_pop_ptr = &(matepop);

		/*CROSSOVER----------------------------*/
		//if (nchrom > 0)
		//{

		//if (optype == 1)
		//{
		//crossover(new_pop_ptr, mate_pop_ptr);
		/*Binary Cross-over*/
		//}

		//if (optype == 2)
		//{
		//unicross(new_pop_ptr, mate_pop_ptr);
		/*Binary Uniform Cross-over*/
		//}
		//}
		//if (nvar > 0)
		//realcross(new_pop_ptr, mate_pop_ptr);
		/*Real Cross-over*/

		/*------MUTATION-------------------*/
		//new_pop_ptr = &(newpop);

		//if (nchrom > 0)
		//mutate(new_pop_ptr);
		/*Binary Mutation */

		//if (nvar > 0)
		//real_mutate(new_pop_ptr);
		/*Real Mutation*/

		//new_pop_ptr = &(newpop);

		/*-------DECODING----------*/
		//if (nchrom > 0)
		//decode(new_pop_ptr);
		/*Decoding for binary strings*/

		/*----------FUNCTION EVALUATION-----------*/
		new_pop_ptr = &(newpop);
		//func(new_pop_ptr);

		/*-------------------SELECTION KEEPING FRONTS ALIVE--------------*/
		old_pop_ptr = &(oldpop);
		//new_pop_ptr = &(newpop);
		//mate_pop_ptr = &(matepop);

		/*Elitism And Sharing Implemented*/
		keepalive(old_pop_ptr, new_pop_ptr, new_pop_ptr, i + 1);

		//mate_pop_ptr = &(matepop);
		//if (nchrom > 0)
		//decode(mate_pop_ptr);

		new_pop_ptr = &(newpop);
		/*------------------REPORT PRINTING--------------------------------*/
		report(i, old_pop_ptr, new_pop_ptr, rep_ptr, gen_ptr, lastit);

		/*==================================================================*/

		/*----------------Rank Ratio Calculation------------------------*/
		new_pop_ptr = &(newpop);
		old_pop_ptr = &(oldpop);

		/*Finding the greater maxrank among the two populations*/

		if (old_pop_ptr->maxrank > new_pop_ptr->maxrank)
			maxrank1 = old_pop_ptr->maxrank;
		else
			maxrank1 = new_pop_ptr->maxrank;

		fprintf(rep2_ptr, "--------RANK AT GENERATION %d--------------\n", i + 1);
		fprintf(rep2_ptr, "Rank old ranks   new ranks     rankratio\n");

		for (j = 0; j < maxrank1; j++)
		{
			/*Sum of the no of individuals at any rank in old population
			and the new populaion*/

			tot = (old_pop_ptr->rankno[j]) + (new_pop_ptr->rankno[j]);

			/*Finding the rank ratio for new population at this rank*/

			new_pop_ptr->rankrat[j] = (new_pop_ptr->rankno[j]) / tot;

			/*Printing this rank ratio to a file called ranks.dat*/

			fprintf(rep2_ptr, " %d\t  %d\t\t %d\t %f\n", j + 1, old_pop_ptr->rankno[j], new_pop_ptr->rankno[j], new_pop_ptr->rankrat[j]);

		}

		fprintf(rep2_ptr, "-----------------Rank Ratio-------------------\n");
		/*==================================================================*/

		/*=======Copying the new population to old population======*/

		old_pop_ptr = &(oldpop);
		new_pop_ptr = &(newpop);

		for (j = 0; j < popsize; j++)
		{
			old_pop_ptr->ind_ptr = &(old_pop_ptr->ind[j]);
			new_pop_ptr->ind_ptr = &(new_pop_ptr->ind[j]);
			//if (nchrom > 0)
			//{
			/*For Binary GA copying of the chromosome*/

			//for (l = 0; l < chrom; l++)
			//old_pop_ptr->ind_ptr->genes[l] = new_pop_ptr->ind_ptr->genes[l];

			//for (l = 0; l < nchrom; l++)
			//old_pop_ptr->ind_ptr->xbin[l] = new_pop_ptr->ind_ptr->xbin[l];
			//}
			if (nvar > 0)
			{
				/*For Real Coded GA copying of the chromosomes*/
				for (l = 0; l < nvar; l++)
					old_pop_ptr->ind_ptr->xreal[l] = new_pop_ptr->ind_ptr->xreal[l];
				old_pop_ptr->ind_ptr->particle_v[l] = old_pop_ptr->ind_ptr->particle_v[l];
				old_pop_ptr->ind_ptr->loc_best[l] = old_pop_ptr->ind_ptr->loc_best[l];
			}

			/*Copying the fitness vector */
			for (l = 0; l < nfunc; l++)
				old_pop_ptr->ind_ptr->fitness[l] = new_pop_ptr->ind_ptr->fitness[l];

			/*Copying the dummy fitness*/
			old_pop_ptr->ind_ptr->cub_len = new_pop_ptr->ind_ptr->cub_len;

			/*Copying the rank of the individuals*/
			old_pop_ptr->ind_ptr->rank = new_pop_ptr->ind_ptr->rank;

			/*Copying the error and constraints of the individual*/

			old_pop_ptr->ind_ptr->error = new_pop_ptr->ind_ptr->error;
			for (l = 0; l < ncons; l++)
			{
				old_pop_ptr->ind_ptr->constr[l] = new_pop_ptr->ind_ptr->constr[l];
			}

			/*Copying the flag of the individuals*/
			old_pop_ptr->ind_ptr->flag = new_pop_ptr->ind_ptr->flag;
		}   // end of j

		maxrank1 = new_pop_ptr->maxrank;

		/*Copying the array having the record of the individual
		at different ranks */
		for (l = 0; l < popsize; l++)
		{
			old_pop_ptr->rankno[l] = new_pop_ptr->rankno[l];
		}

		/*Copying the maxrank */
		old_pop_ptr->maxrank = new_pop_ptr->maxrank;
		for (l = 0; l < nfunc; l++){
			old_pop_ptr->gfit1[l] = new_pop_ptr->gfit1[l];
			old_pop_ptr->glo_best1[l] = new_pop_ptr->glo_best1[l];
		}
		/*Printing the fitness record for last generation in a file last*/
		if (i == gener - 1)
		{  // for the last generation 
			old_pop_ptr = &(newpop);
			for (f = 0; f < popsize; f++) // for printing
			{
				old_pop_ptr->ind_ptr = &(old_pop_ptr->ind[f]);

				if ((old_pop_ptr->ind_ptr->error <= 0.0) && (old_pop_ptr->ind_ptr->rank == 1))  // for all feasible solutions and non-dominated solutions
				{
					for (l = 0; l < nfunc; l++)
						fprintf(end_ptr, "%f\t", old_pop_ptr->ind_ptr->fitness[l]);
					for (l = 0; l < ncons; l++)
					{
						fprintf(end_ptr, "%f\t", old_pop_ptr->ind_ptr->constr[l]);
					}
					if (ncons > 0)
						fprintf(end_ptr, "%f\t", old_pop_ptr->ind_ptr->error);
					fprintf(end_ptr, "\n");

					if (nvar > 0)
					{
						for (l = 0; l < nvar; l++)
						{
							fprintf(g_var, "%f\t", old_pop_ptr->ind_ptr->xreal[l]);
						}
						fprintf(g_var, "  ");
					}

					//if (nchrom > 0)
					//{
					//for (l = 0; l < nchrom; l++)
					//{
					//fprintf(g_var, "%f\t", old_pop_ptr->ind_ptr->xbin[l]);
					//}
					//}
					fprintf(g_var, "\n");
				}  // feasibility check
			} // end of f (printing)

		} // for the last generation
	}  // end of i 

	/*                   Generation Loop Ends                                */
	/************************************************************************/

	//fprintf(rep_ptr, "NO. OF CROSSOVER = %d\n", ncross);
	//fprintf(rep_ptr, "NO. OF MUTATION = %d\n", nmut);
	fprintf(rep_ptr, "------------------------------------------------------------\n");
	fprintf(rep_ptr, "---------------------------------Thanks---------------------\n");
	fprintf(rep_ptr, "-------------------------------------------------------------\n");
	printf("NOW YOU CAN LOOK IN THE FILE OUTPUT2.DAT\n");

	/*Closing the files*/
	fclose(rep_ptr);
	fclose(gen_ptr);
	fclose(rep2_ptr);
	fclose(end_ptr);
	fclose(g_var);
	fclose(lastit);
	DWORD dwEnd = GetTickCount();
	duration = dwEnd - dwStart;
	printf("Using time = %f milliseconds\n",duration);
}