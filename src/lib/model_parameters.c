/**
 * @file model_parameters.c
 * @brief Model parameter functions.
 * @author <Pierre-Luc.Drouin@drdc-rddc.gc.ca>, Defence Research and Development Canada Ottawa Research Centre.
 */

#include "model_parameters.h"

int model_solve_pars(model_pars* pars)
{
  if((isnan(pars->tbar)==0) + (isnan(pars->lambda)==0) + (isnan(pars->p)==0 || isnan(pars->mu)==0) + (isnan(pars->R0)==0) != 3) {
    fprintf(stderr,"%s: Error: An invalid combination of tbar, lambda, p, mu and R0 parameters was provided.\n",__func__);
    return -1;
  }

  if(model_solve_R0_group(pars)) return -2;
  
  printf("Basic reproduction parameters are:\n");
  printf("lambda:\t%22.15e\n",pars->lambda);
  printf("tbar:\t%22.15e\n",pars->tbar);
  printf("mu:\t%22.15e\n",pars->mu);
  printf("p:\t%22.15e\n",pars->p);
  printf("R0:\t%22.15e\n",pars->R0);

  if((isnan(pars->kappa)==0) + (isnan(pars->t95)==0) != 1) {
    fprintf(stderr,"%s: Error: Either the kappa parameter or the t95 parameter must be provided.\n",__func__);
    return -3;
  }

  if(model_solve_gamma_group(&pars->tbar, &pars->kappa, &pars->t95)) {
    fprintf(stderr,"%s: Error: Cannot solve parameters for the main time gamma distribution\n",__func__);
    return -4;

  }
  printf("Parameters for the main time gamma distribution:\n");
  printf("tbar:\t%22.15e\n",pars->tbar);
  printf("kappa:\t%22.15e\n",pars->kappa);
  printf("t95:\t%22.15e\n",pars->t95);

  if(pars->pit) {

    if((isnan(pars->kappait)==0) + (isnan(pars->it95)==0) != 1) {
      fprintf(stderr,"%s: Error: Either the kappait parameter or the it95 parameter must be provided.\n",__func__);
      return -5;
    }

    if(model_solve_gamma_group(&pars->itbar, &pars->kappait, &pars->it95)) {
      fprintf(stderr,"%s: Error: Cannot solve parameters for the interrupted main time gamma distribution\n",__func__);
      return -6;

    } else {
      printf("Parameters for the interrupted main time gamma distribution:\n");
      printf("pit:\t%22.15e\n",pars->pit);
      printf("itbar:\t%22.15e\n",pars->itbar);
      printf("kappait:\t%22.15e\n",pars->kappait);
      printf("it95:\t%22.15e\n",pars->it95);
    }
  }

  if(pars->q>0) {

    if((isnan(pars->kappaq)==0) + (isnan(pars->m95)==0) != 1) {
      fprintf(stderr,"%s: Error: Either the kappaq parameter or the m95 parameter must be provided.\n",__func__);
      return -7;
    }

    if(model_solve_gamma_group(&pars->mbar, &pars->kappaq, &pars->m95)) {
      fprintf(stderr,"%s: Error: Cannot solve parameters for the alternate time gamma distribution\n",__func__);
      return -8;

    } else {
      printf("Parameters for the alternate time gamma distribution:\n");
      printf("q:\t%22.15e\n",pars->q);
      printf("mbar:\t%22.15e\n",pars->mbar);
      printf("kappaq:\t%22.15e\n",pars->kappaq);
      printf("m95:\t%22.15e\n",pars->m95);
    }

    if(isnan(pars->pim)) pars->pim=pars->pit;

    if(pars->pim>0) {

      if(isnan(pars->imbar) && isnan(pars->kappaim) && isnan(pars->im95)) {
	pars->imbar=pars->itbar;
	pars->kappaim=pars->kappait;
	pars->im95=pars->it95;

	printf("Parameters for the interrupted alternate time gamma distribution:\n");
	printf("pim:\t%22.15e\n",pars->pim);
	printf("imbar:\t%22.15e\n",pars->imbar);
	printf("kappaim:\t%22.15e\n",pars->kappaim);
	printf("im95:\t%22.15e\n",pars->im95);

      } else {

	if(isnan(pars->imbar)) pars->imbar=pars->itbar;

	if((isnan(pars->kappaim)==0) + (isnan(pars->im95)==0) != 1) {
	  fprintf(stderr,"%s: Error: Either the kappaim parameter or the im95 parameter must be provided.\n",__func__);
	  return -9;
	}

	if(model_solve_gamma_group(&pars->imbar, &pars->kappaim, &pars->im95)) {
	  fprintf(stderr,"%s: Error: Cannot solve parameters for the interrupted alternate time gamma distribution\n",__func__);
	  return -10;

	} else {
	  printf("Parameters for the interrupted alternate time gamma distribution:\n");
	  printf("pim:\t%22.15e\n",pars->pim);
	  printf("imbar:\t%22.15e\n",pars->imbar);
	  printf("kappaim:\t%22.15e\n",pars->kappaim);
	  printf("im95:\t%22.15e\n",pars->im95);
	}
      }
    }
  }

  if(!isnan(pars->kappal) || !isnan(pars->l95)) {

    if((isnan(pars->kappal)==0) + (isnan(pars->l95)==0) != 1) {
      fprintf(stderr,"%s: Error: Either the kappal parameter or the l95 parameter must be provided.\n",__func__);
      return -11;
    }

    if(model_solve_gamma_group(&pars->lbar, &pars->kappal, &pars->l95)) {
      fprintf(stderr,"%s: Error: Cannot solve parameters for the latent time gamma distribution\n",__func__);
      return -12;

    } else {
      printf("Parameters for the latent time gamma distribution:\n");
      printf("lbar:\t%22.15e\n",pars->lbar);
      printf("kappal:\t%22.15e\n",pars->kappal);
      printf("l95:\t%22.15e\n",pars->l95);
    }
  }

  return 0;
}

int model_solve_R0_group(model_pars* pars)
{
  //If p is provided as an input
  if(!isnan(pars->p)) {

    if(pars->p<0) {
      fprintf(stderr,"%s: Error: p must be non-negative\n",__func__);
      return -1;
    }
    pars->mu=(pars->p>0?-pars->p/((1-pars->p)*log(1-pars->p)):1);
  }

  if(!isnan(pars->tbar) && pars->tbar<=0) {
    fprintf(stderr,"%s: Error: tbar must be greater than 0\n",__func__);
    return -2;
  }

  if(!isnan(pars->lambda) && pars->lambda<=0) {
    fprintf(stderr,"%s: Error: lambda must be greater than 0\n",__func__);
    return -3;
  }

  if(!isnan(pars->R0) && pars->R0<=0) {
    fprintf(stderr,"%s: Error: R0j must be greater than 0\n",__func__);
    return -4;
  }

  //Solve for the missing parameter
  if(isnan(pars->R0)) pars->R0=pars->lambda*pars->tbar*pars->mu;

  else if(isnan(pars->lambda)) pars->lambda=pars->R0/(pars->tbar*pars->mu);

  else if(isnan(pars->tbar)) pars->tbar=pars->R0/(pars->lambda*pars->mu);

  else pars->mu=pars->R0/(pars->lambda*pars->tbar);

  //If p is unknown solve for it numerically
  if(isnan(pars->p)) {

    if(pars->mu > 1) {
      root_finder* rf=root_finder_init(logroot, &pars->mu);
      pars->p=0.999;

      int ret=root_finder_find(rf, RF_P_EPSF, 100, RF_P_EPSF, 1-RF_P_EPSF, &pars->p);

      root_finder_free(rf);

      if(ret) return ret;

    } else pars->p=0;
  }
  return 0;
}

int model_solve_gamma_group(double* ave, double* kappa, double* x95)
{
  if(!(*ave>=0)) {
    fprintf(stderr,"%s: Error: The average of the distribution must be non-negative.\n",__func__);
    return -1;
  }

  if(isnan(*x95)) {

    if(!(*kappa>0)) {
      fprintf(stderr,"%s: Error: The kappa parameter of the distribution must be non-negative.\n",__func__);
      return -1;
    }

    if(*kappa != INFINITY) {
      double pars[2]={*ave * *kappa, *kappa};
      root_finder* rf=root_finder_init(gpercroot, pars);
      *x95=*ave;

      int ret=root_finder_find(rf, RF_GPERC_EPSF, 100, *ave, 1e100, x95);

      root_finder_free(rf);

      if(ret) return ret;

    } else *x95 = *ave;

  } else {

    if(!(*x95>=*ave)) {
      fprintf(stderr,"%s: Error: The 95th percentile of the distribution cannot be smaller than the average\n",__func__);
      return -1;
    }

    if(*x95 != *ave) {
      *kappa=1;
      const double otherkappa=*kappa*0.9;
      double pars[4]={*ave, *x95, otherkappa, gpercrootfunc(*ave * otherkappa, *x95 * otherkappa)};
      root_finder* rf=root_finder_init(gkapparoot, pars);

      int ret=root_finder_find(rf, RF_GKAPPA_EPSF, 100, 1e-100, 1e100, kappa);

      root_finder_free(rf);

      if(ret) return ret;

    } else *kappa = INFINITY;
  }
  return 0;
}