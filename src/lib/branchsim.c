/**
 * @file branchsim.c
 * @brief Simulation data structures and functions.
 * @author <Pierre-Luc.Drouin@drdc-rddc.gc.ca>, Defence Research and Development Canada Ottawa Research Centre.
 * Original model from <jerome.levesque@tpsgc-pwgsc.gc.ca> and
 * <david.maybury@tpsgc-pwgsc.gc.ca>
 */

#include "branchsim.h"

#define II_ARRAY_GROW_FACT (1.5)  //!< Growing factor for the array of current infectious individuals across all layers.

void branchsim_init(sim_vars* sv)
{
  sv->brsim.iis=(infindividual*)malloc(INIT_N_LAYERS*sizeof(infindividual));

  for(uint32_t i=0; i<INIT_N_LAYERS; ++i) sv->ii_alloc_proc_func(sv->brsim.iis+i);
  sv->brsim.nlayers=INIT_N_LAYERS;
  sv->brsim.iis[0].event_time=0;
}

int branchsim(sim_vars* sv)
{
  int i;
  model_pars const* sim=&(sv->pars);

  sv->curii=sv->brsim.iis;
  sv->curii->event_time=0;
  sv->curii->nevents=1;
  sv->curii->curevent=0;
  sv->curii->ninfections=sim->nstart;
  sv->new_event_proc_func(sv);

  for(i=sim->nstart-1; i>=0; --i) {
    DEBUG_PRINTF("initial individual %i\n",i);
    sv->curii=sv->brsim.iis+1;
    //Generate the communicable period appropriately
    sv->gen_time_periods_func(sv);
    sv->curii->infectious_at_tmax=((sv->curii-1)->event_time + sv->curii->comm_period > sv->pars.tmax);
    DEBUG_PRINTF("Comm period is %f%s\n",sv->curii->comm_period,(sv->curii->infectious_at_tmax?" (reached end)":"")); \
    sv->curii->nevents=gsl_ran_poisson(sv->r, sim->lambda*sv->curii->comm_period);
    DEBUG_PRINTF("Nevents (%f*%f) is %i\n", sim->lambda, sv->curii->comm_period, sv->curii->nevents);

    //If no event for the current individual in the primary layer
    if(!sv->curii->nevents) {
      sv->inf_proc_func_noevent(sv->curii, sv->dataptr);
      continue;
    }

    sv->new_inf_proc_func(sv->curii);
    sv->curii->curevent=0;

    for(;;) {
      sv->curii->event_time=sv->curii->latent_period+sv->curii->comm_period*(1-gsl_rng_uniform(sv->r));
      //sv->curii->event_time=sv->curii->latent_period+sv->curii->comm_period*rng_rand_pu01d((rng_stream*)sv->r->state);
      DEBUG_PRINTF("Event %i/%i at time %f\n",sv->curii->curevent,sv->curii->nevents,sv->curii->event_time);

      sv->curii->ninfections=gsl_ran_logarithmic(sv->r, sv->pars.p);

      if(!sv->new_event_proc_func(sv)) {
	DEBUG_PRINTF("New event returned false\n");

	//If the events have been exhausted, go down another layer
	if(sv->curii->curevent == sv->curii->nevents-1) {
	  sv->end_inf_proc_func(sv->curii, sv->dataptr);
	  goto done_parsing;
	}

	//Else
	//Move to the next event for the individual
	++(sv->curii->curevent);

      } else break;
    }
    sv->curii->curinfection=0;
    DEBUG_PRINTF("Infection %i/%i\n",sv->curii->curinfection,sv->curii->ninfections);

    //Create a new infected individual
    for(;;) {
      ++(sv->curii);
      DEBUG_PRINTF("Move to next layer (%li)\n",sv->curii-sv->brsim.iis);

      //If reaching the end of the allocated array, increase its size
      if(sv->curii==sv->brsim.iis+sv->brsim.nlayers) {
	sv->brsim.nlayers*=II_ARRAY_GROW_FACT;
	DEBUG_PRINTF("Growing layers to %i\n",sv->brsim.nlayers);
	uint32_t layer=sv->curii-sv->brsim.iis;
	sv->brsim.iis=(infindividual*)realloc(sv->brsim.iis,sv->brsim.nlayers*sizeof(infindividual));

	for(uint32_t i=sv->brsim.nlayers-1; i>=layer; --i) sv->ii_alloc_proc_func(sv->brsim.iis+i);
	sv->curii=sv->brsim.iis+layer;
      }
      //Generate the communicable period appropriately
      sv->gen_time_periods_func(sv);
      sv->curii->infectious_at_tmax=((sv->curii-1)->event_time + sv->curii->comm_period > sv->pars.tmax);
      DEBUG_PRINTF("Comm period is %f%s\n",sv->curii->comm_period,(sv->curii->infectious_at_tmax?" (reached end)":"")); \

      //Generate the number of events
      sv->curii->nevents=gsl_ran_poisson(sv->r, sim->lambda*sv->curii->comm_period);
      DEBUG_PRINTF("Nevents (%f*%f) is %i\n", sim->lambda, sv->curii->comm_period, sv->curii->nevents);

      //If the number of events is non-zero
      if(sv->curii->nevents) {
	sv->curii->curevent=0;
	sv->new_inf_proc_func(sv->curii);
	//Generate the event time
gen_event:
	sv->curii->event_time=(sv->curii-1)->event_time+sv->curii->latent_period+sv->curii->comm_period*(1-gsl_rng_uniform(sv->r));
	//sv->curii->event_time=(sv->curii-1)->event_time+sv->curii->latent_period+sv->curii->comm_period*rng_rand_pu01d((rng_stream*)sv->r->state);
	DEBUG_PRINTF("Event %i/%i at time %f\n",sv->curii->curevent,sv->curii->nevents,sv->curii->event_time);

	//Generate the number of infections and the associated index for
	//the current event
	//Move to the next layer
	sv->curii->ninfections=gsl_ran_logarithmic(sv->r, sv->pars.p);

	if(sv->new_event_proc_func(sv)) {
	  sv->curii->curinfection=0;
	  DEBUG_PRINTF("Infection %i/%i\n",sv->curii->curinfection,sv->curii->ninfections);
	  continue;

	} else {

	  //If the events have not been exhausted
	  if(sv->curii->curevent < sv->curii->nevents-1) {
	    //Move to the next event for the individual
	    ++(sv->curii->curevent);
	    goto gen_event;
	  }
	  sv->end_inf_proc_func(sv->curii, sv->dataptr);
	}

      } else sv->inf_proc_func_noevent(sv->curii, sv->dataptr);

      //All events for the current individual have been exhausted
      for(;;) {

	if(sv->curii == sv->brsim.iis+1) goto done_parsing;
	//Move down one layer
	--(sv->curii);
	DEBUG_PRINTF("Move to previous layer (%li)\n",sv->curii-sv->brsim.iis);

	//If the infections have been exhausted
	if(sv->curii->curinfection == sv->curii->ninfections-1) {

	  //If the events have been exhausted, go down another layer
	  if(sv->curii->curevent == sv->curii->nevents-1) {
	    sv->end_inf_proc_func(sv->curii, sv->dataptr);
	    continue;
	  }

	  //Else
	  //Move to the next event for the individual
	  ++(sv->curii->curevent);
	  goto gen_event;
	}

	//Look at the next infected individual in the current event
	++(sv->curii->curinfection);
	DEBUG_PRINTF("Infection %i/%i\n",sv->curii->curinfection,sv->curii->ninfections);
	break;
      }
    }

    //Else if no event for the current infected individual
done_parsing:
    ;
  }

  return 0;
}

void branchsim_free(sim_vars* sv)
{
  for(uint32_t i=0; i<sv->brsim.nlayers; ++i) if(sv->brsim.iis[i].dataptr) free(sv->brsim.iis[i].dataptr);
  free(sv->brsim.iis);
}
