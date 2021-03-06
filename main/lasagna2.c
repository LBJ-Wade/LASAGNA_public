/** @file lasagna.c 
 * Thomas Tram B?low, 25.08.2011    
 */
 
#include "lasagna.h"
#include <time.h>
int main(int argc, char **argv) {
  qke_param qke_struct;
  double *y_inout;
  int *interp_idx;
  ErrorMsg error_message;
  int i;
  int func_return;
  clock_t start, end;
  double cpu_time_used, elapsed;
  time_t wtime1, wtime2;
  EvolverOptions options;

  extern int evolver_radau5();
  extern int evolver_ndf15(); 	
  extern int evolver_rkdp45(); 	
  int (*generic_evolver)();  

  func_return = input_init_from_arguments(argc, 
					  argv,
					  &qke_struct,
					  error_message);
  if (func_return == _FAILURE_){
    printf("\n\nError running input_init_from_arguments\n=>%s\n",error_message);
    return _FAILURE_;
  }

  /** Do stuff */
  y_inout = calloc(qke_struct.neq,sizeof(double));
  interp_idx = malloc(sizeof(int)*qke_struct.neq);
  for(i=0; i<qke_struct.neq; i++)
    interp_idx[i] = 1;

  if(qke_struct.evolver == 0){
    generic_evolver = evolver_radau5;
  }
  else if (qke_struct.evolver == 1){
    generic_evolver = evolver_ndf15;
  }
  else {
    generic_evolver = evolver_rkdp45;
  }
  
  qke_initial_conditions(qke_struct.T_initial, 
			 y_inout, 
			 &qke_struct);
  
  qke_init_output(&qke_struct);

//Handle options:
  DefaultEvolverOptions(&options,qke_struct.LinearAlgebraWrapper);
  options.used_in_output=interp_idx;
  options.RelTol = qke_struct.rtol;
  options.AbsTol = qke_struct.abstol;
  options.t_vec = qke_struct.Tvec; 
  options.tres = qke_struct.Tres;
  options.Ap = qke_struct.Ap;
  options.Ai = qke_struct.Ai;
  options.output = qke_store_output;
  //  options.print_variables = qke_print_L;
  //  options.stop_function = qke_stop_at_L;
  options.EvolverVerbose=4;
  options.EvolverVerbose=qke_struct.verbose;
  options.Cores = qke_struct.nproc;

  printf("is_electron = %d. _TRUE_=%d\n",qke_struct.is_electron,_TRUE_);
  start = clock();  
  time(&wtime1);
  func_return = generic_evolver(qke_derivs_fixed_grid,
				&qke_struct,
				qke_struct.T_initial,
				qke_struct.T_final,
				y_inout, 
				qke_struct.neq, 
				&(options),
				error_message);
  end = clock();
  time(&wtime2);  
  cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
  printf("CPU time used: %g minutes.\n",cpu_time_used/60);
  elapsed = difftime(wtime2,wtime1);
  printf("Wall clock time used: %g minutes.\n",elapsed/60);
  
  free(y_inout);
  free(interp_idx);
  free_qke_param(&qke_struct);


  if (func_return == _FAILURE_){
    printf("%s\n",error_message);
    return _FAILURE_;
  }
  else{
    return _SUCCESS_;
  }
}
