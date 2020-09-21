/**
 * @file simulation.c
 * @brief Common simulation functions.
 * @author <Pierre-Luc.Drouin@drdc-rddc.gc.ca>, Defence Research and Development Canada Ottawa Research Centre.
 */

#include "simulation.h"

void sim_pars_init(model_pars* pars)
{
  pars->tbar=NAN;
  pars->p=NAN;
  pars->mu=NAN;
  pars->g_ave=NAN;
  pars->lambda=NAN;
  pars->lambdap=NAN;
  pars->pinf=NAN;
  pars->R0=NAN;
  pars->kappa=NAN;
  pars->lbar=NAN;
  pars->kappal=NAN;
  pars->q=0;
  pars->mbar=NAN;
  pars->kappaq=NAN;
  pars->pit=0;
  pars->itbar=NAN;
  pars->kappait=NAN;
  pars->pim=NAN;
  pars->imbar=NAN;
  pars->kappaim=NAN;
  pars->t95=NAN;
  pars->m95=NAN;
  pars->l95=NAN;
  pars->it95=NAN;
  pars->im95=NAN;
  pars->ttpr=NAN;
  pars->mtpr=NAN;
  pars->tdeltat=NAN;
  pars->tmax=INFINITY;
  pars->nstart=1;
  pars->popsize=0;
  pars->pricommpertype=ro_pricommper_main|ro_pricommper_alt|ro_pricommper_main_int|ro_pricommper_alt_int;
  pars->grouptype=ro_group_log_attendees_plus_1;
  pars->trelpriend=false;
}

void sim_init(sim_vars* sv, model_pars const* pars, const gsl_rng* r)
{
  sv->pars=*pars;
  sv->r=r;

  PER_COND;

  sv->dataptr=NULL;
  sv->ii_alloc_proc_func=default_ii_alloc_proc_func;
  sv->new_event_proc_func=default_event_proc_func;
  sv->new_pri_inf_proc_func=dummy_proc_func_sv_ii;
  sv->new_inf_proc_func=dummy_proc_func_sv_ii;
  sv->end_inf_proc_func=dummy_proc_func_two_pars;
  sv->inf_proc_func_noevent=dummy_proc_func_two_pars;
}
