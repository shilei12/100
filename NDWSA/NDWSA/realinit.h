/*This is the file which initializes the population*/
void realinit(population *pop_ptr);

void realinit(population *pop_ptr)
{
  int i,j,r,d2;
  float d,d1;
  
  for (i = 0 ; i < popsize ; i++)
    { 
      for (j = 0; j < nvar; j++)
	{
	  d = randomperc();
	  d1 = 2*d - 1;
	  /*if limits are not specified then generates any number between 
	    zero and infinity*/
	  if(ans != 1)
	    {
	      pop_ptr->ind[i].xreal[j] = 1/d1 ;
	    }
	  
	  /*if limits are specified it generates the value in 
	    range of minimum and maximum value of the variable*/
	  else
	    {
	      pop_ptr->ind[i].xreal[j] = d*(lim_r[j][1] - lim_r[j][0])+lim_r[j][0];
		  pop_ptr->ind[i].loc_best[j] = pop_ptr->ind[i].xreal[j]; //将当前最优结果写入局部最优集合
		  pop_ptr->ind[i].particle_v[j] =-V_max+2* V_max*d;    //速度
	    }
	}
      /* pop_ptr->ind_ptr = &(pop_ptr->ind[i+1]); */
    }
  /*pop_ptr->ind_ptr = &(pop_ptr->ind[0]); */
 return;
}






























































