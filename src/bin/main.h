/**
 * @file main.h
 * @brief Main function for the simulation executable.
 * @author <Pierre-Luc.Drouin@drdc-rddc.gc.ca>, Defence Research and Development Canada Ottawa Research Centre.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <math.h>

#include <endian.h>

#include <assert.h>

#include <pthread.h>

#include <gsl/gsl_rng.h>

#include "rngstream_gsl.h"

#include "config.h"
#include "model_parameters.h"
#include "infindividual.h"
#include "branchsim.h"
#include "standard_summary_stats.h"

/**
 * @brief Main function.
 */
int main(const int nargs, const char* args[]);

typedef struct {
  config_pars const* cp;
  double npathsperset;
  uint32_t id;
  uint32_t nsets;
  int32_t nbinsperunit;
  uint32_t npers;
  int32_t tlppnnpers;
  uint32_t tlpptnvpers;
  uint32_t volatile* set;
  double commper_mean;
#ifdef NUMEVENTSSTATS
  double nevents_mean;
#endif
  double n_inf;
  double r_mean;
  double pe;
  double pm;
  double te_mean;
  double te_std;
  double* inf_timeline_mean_ext;
  double* inf_timeline_std_ext;
  double* inf_timeline_mean_noext;
  double* inf_timeline_std_noext;
  double* newinf_timeline_mean_ext;
  double* newinf_timeline_std_ext;
  double* newinf_timeline_mean_noext;
  double* newinf_timeline_std_noext;
  double* newpostest_timeline_mean_ext;
  double* newpostest_timeline_std_ext;
  double* newpostest_timeline_mean_noext;
  double* newpostest_timeline_std_noext;
  uint64_t* ngeninfs;
  uint32_t ninfbins;
  int32_t maxedoutmintimeindex;
  gsl_rng* r;
  pthread_mutex_t* tlflock;
  pthread_mutex_t* ctflock;
} thread_data;

void* simthread(void* arg);

inline static void realloc_thread_timelines(thread_data* data, const int32_t ndiff, const int32_t pdiff)
{
  if(pdiff > 0 || ndiff > 0) {
    double* newarray;
    const ssize_t newsize=data->tlpptnvpers+pdiff+ndiff;

    newarray=(double*)malloc(newsize*sizeof(double));
    memcpy(newarray+ndiff,data->inf_timeline_mean_ext,data->tlpptnvpers*sizeof(double));
    free(data->inf_timeline_mean_ext);
    data->inf_timeline_mean_ext=newarray;

    newarray=(double*)malloc(newsize*sizeof(double));
    memcpy(newarray+ndiff,data->inf_timeline_std_ext,data->tlpptnvpers*sizeof(double));
    free(data->inf_timeline_std_ext);
    data->inf_timeline_std_ext=newarray;

    newarray=(double*)malloc(newsize*sizeof(double));
    memcpy(newarray+ndiff,data->newinf_timeline_mean_ext,data->tlpptnvpers*sizeof(double));
    free(data->newinf_timeline_mean_ext);
    data->newinf_timeline_mean_ext=newarray;

    newarray=(double*)malloc(newsize*sizeof(double));
    memcpy(newarray+ndiff,data->newinf_timeline_std_ext,data->tlpptnvpers*sizeof(double));
    free(data->newinf_timeline_std_ext);
    data->newinf_timeline_std_ext=newarray;

    newarray=(double*)malloc(newsize*sizeof(double));
    memcpy(newarray+ndiff,data->newpostest_timeline_mean_ext,data->tlpptnvpers*sizeof(double));
    free(data->newpostest_timeline_mean_ext);
    data->newpostest_timeline_mean_ext=newarray;

    newarray=(double*)malloc(newsize*sizeof(double));
    memcpy(newarray+ndiff,data->newpostest_timeline_std_ext,data->tlpptnvpers*sizeof(double));
    free(data->newpostest_timeline_std_ext);
    data->newpostest_timeline_std_ext=newarray;

    newarray=(double*)malloc(newsize*sizeof(double));
    memcpy(newarray+ndiff,data->inf_timeline_mean_noext,data->tlpptnvpers*sizeof(double));
    free(data->inf_timeline_mean_noext);
    data->inf_timeline_mean_noext=newarray;

    newarray=(double*)malloc(newsize*sizeof(double));
    memcpy(newarray+ndiff,data->inf_timeline_std_noext,data->tlpptnvpers*sizeof(double));
    free(data->inf_timeline_std_noext);
    data->inf_timeline_std_noext=newarray;

    newarray=(double*)malloc(newsize*sizeof(double));
    memcpy(newarray+ndiff,data->newinf_timeline_mean_noext,data->tlpptnvpers*sizeof(double));
    free(data->newinf_timeline_mean_noext);
    data->newinf_timeline_mean_noext=newarray;

    newarray=(double*)malloc(newsize*sizeof(double));
    memcpy(newarray+ndiff,data->newinf_timeline_std_noext,data->tlpptnvpers*sizeof(double));
    free(data->newinf_timeline_std_noext);
    data->newinf_timeline_std_noext=newarray;

    newarray=(double*)malloc(newsize*sizeof(double));
    memcpy(newarray+ndiff,data->newpostest_timeline_mean_noext,data->tlpptnvpers*sizeof(double));
    free(data->newpostest_timeline_mean_noext);
    data->newpostest_timeline_mean_noext=newarray;

    newarray=(double*)malloc(newsize*sizeof(double));
    memcpy(newarray+ndiff,data->newpostest_timeline_std_noext,data->tlpptnvpers*sizeof(double));
    free(data->newpostest_timeline_std_noext);
    data->newpostest_timeline_std_noext=newarray;

    if(ndiff) {
      memset(data->inf_timeline_mean_ext,0,ndiff*sizeof(double));
      memset(data->inf_timeline_std_ext,0,ndiff*sizeof(double));

      memset(data->newinf_timeline_mean_ext,0,ndiff*sizeof(double));
      memset(data->newinf_timeline_std_ext,0,ndiff*sizeof(double));

      memset(data->newpostest_timeline_mean_ext,0,ndiff*sizeof(double));
      memset(data->newpostest_timeline_std_ext,0,ndiff*sizeof(double));

      memset(data->inf_timeline_mean_noext,0,ndiff*sizeof(double));
      memset(data->inf_timeline_std_noext,0,ndiff*sizeof(double));

      memset(data->newinf_timeline_mean_noext,0,ndiff*sizeof(double));
      memset(data->newinf_timeline_std_noext,0,ndiff*sizeof(double));

      memset(data->newpostest_timeline_mean_noext,0,ndiff*sizeof(double));
      memset(data->newpostest_timeline_std_noext,0,ndiff*sizeof(double));
    }

    if(pdiff) {
      memset(data->inf_timeline_mean_ext+ndiff+data->tlpptnvpers,0,pdiff*sizeof(double));
      memset(data->inf_timeline_std_ext+ndiff+data->tlpptnvpers,0,pdiff*sizeof(double));

      memset(data->newinf_timeline_mean_ext+ndiff+data->tlpptnvpers,0,pdiff*sizeof(double));
      memset(data->newinf_timeline_std_ext+ndiff+data->tlpptnvpers,0,pdiff*sizeof(double));

      memset(data->newpostest_timeline_mean_ext+ndiff+data->tlpptnvpers,0,pdiff*sizeof(double));
      memset(data->newpostest_timeline_std_ext+ndiff+data->tlpptnvpers,0,pdiff*sizeof(double));

      memset(data->inf_timeline_mean_noext+ndiff+data->tlpptnvpers,0,pdiff*sizeof(double));
      memset(data->inf_timeline_std_noext+ndiff+data->tlpptnvpers,0,pdiff*sizeof(double));

      memset(data->newinf_timeline_mean_noext+ndiff+data->tlpptnvpers,0,pdiff*sizeof(double));
      memset(data->newinf_timeline_std_noext+ndiff+data->tlpptnvpers,0,pdiff*sizeof(double));

      memset(data->newpostest_timeline_mean_noext+ndiff+data->tlpptnvpers,0,pdiff*sizeof(double));
      memset(data->newpostest_timeline_std_noext+ndiff+data->tlpptnvpers,0,pdiff*sizeof(double));
    }
    data->tlppnnpers+=ndiff;
    data->tlpptnvpers+=ndiff+pdiff;
  }
}

inline static ssize_t tlo_write_reg_path(std_summary_stats const* stats, char* buf)
{
  int32_t b;

  for(b=stats->tlpptnvpers-1; b>0; --b) {
    //printf("TotInf[%" PRIi32 "]=%" PRIu32 ",\tInf[%" PRIi32 "]=%" PRIu32 "\n",b,stats->pp_newinf_timeline[b],b,inf_timeline[b]);

    if(stats->pp_inf_timeline[b]) break;
    assert(stats->pp_newinf_timeline[b]==0);
  }

  //If timelines are all zero
  /*
  if(b==0 && stats->pp_inf_timeline[b]==0) {
    assert(stats->pp_newinf_timeline[b]==0);
    *(uint32_t*)buf=0;
    buf[4]=1;
    return 5;
  }
  */
  assert(b>0 || (stats->pp_inf_timeline[b] && stats->pp_newinf_timeline[b]));

  const uint32_t nbins=b+1;
  *(uint32_t*)buf=htole32(nbins);
  *(uint32_t*)(buf+4)=htole32(stats->maxedoutmintimeindex);
  *(uint32_t*)(buf+8)=htole64((int32_t)(stats->extinction?floor(stats->extinction_time):-INT32_MAX));
  buf+=12;

  for(b=0; b<nbins; ++b) {
    //printf("Inf[%" PRIi32 "]=%" PRIu32 ",\tTotInf[%" PRIi32 "]=%" PRIu32 "\n",b,stats->pp_inf_timeline[b],b,stats->pp_newinf_timeline[b]);
    ((uint32_t*)buf)[b]=htole32(stats->pp_inf_timeline[b]);
    ((uint32_t*)buf)[b+nbins]=htole32(stats->pp_newinf_timeline[b]);
  }

  return 12+nbins*8;
}

inline static ssize_t tlo_write_reg_postest_path(std_summary_stats const* stats, char* buf)
{
  int32_t b;

  for(b=stats->tlpptnvpers-1; b>0; --b) {
    //printf("TotInf[%" PRIi32 "]=%" PRIu32 ",\tInf[%" PRIi32 "]=%" PRIu32 "\n",b,stats->pp_newinf_timeline[b],b,stats->pp_inf_timeline[b]);

    if(stats->pp_inf_timeline[b] || stats->pp_newpostest_timeline[b]) break;
    assert(stats->pp_newinf_timeline[b]==0);
  }

  //If timelines are all zero
  /*
  if(b==0 && stats->pp_inf_timeline[b]==0) {
    assert(stats->pp_newinf_timeline[b]==0);
    assert(stats->pp_newpostest_timeline[b]==0);
    *(uint32_t*)buf=0;
    buf[4]=1;
    return 5;
  }
  */
  assert(b>0 || (stats->pp_inf_timeline[b] && stats->pp_newinf_timeline[b] && stats->pp_newpostest_timeline[b]));

  const uint32_t nbins=b+1;
  *(uint32_t*)buf=htole32(nbins);
  *(uint32_t*)(buf+4)=htole32(stats->maxedoutmintimeindex);
  *(uint32_t*)(buf+8)=htole64((int32_t)(stats->extinction?floor(stats->extinction_time):-INT32_MAX));
  buf+=12;
  const uint32_t tnbins=2*nbins;

  for(b=0; b<nbins; ++b) {
    //printf("Inf[%" PRIi32 "]=%" PRIu32 ",\tTotInf[%" PRIi32 "]=%" PRIu32 "\n",b,stats->pp_inf_timeline[b],b,stats->pp_newinf_timeline[b]);
    ((uint32_t*)buf)[b]=htole32(stats->pp_inf_timeline[b]);
    ((uint32_t*)buf)[b+nbins]=htole32(stats->pp_newinf_timeline[b]);
    ((uint32_t*)buf)[b+tnbins]=htole32(stats->pp_newpostest_timeline[b]);
  }

  return 12+nbins*12;
}

inline static ssize_t tlo_write_reltime_path(std_summary_stats const* stats, char* buf)
{
  int32_t bmin=-stats->tlppnnpers;
  int32_t bmax=bmin+stats->tlpptnvpers-1;

  for(; bmax>bmin; --bmax) {
    //printf("TotInf[%" PRIi32 "]=%" PRIu32 ",\tInf[%" PRIi32 "]=%" PRIu32 "\n",bmax,stats->pp_newinf_timeline[bmax],bmax,stats->pp_inf_timeline[bmax]);

    if(stats->pp_inf_timeline[bmax]) break;
    assert(stats->pp_newinf_timeline[bmax]==0);
  }

  //If timelines are all zero
  /*
  if(bmax==bmin && stats->pp_inf_timeline[bmax]==0) {
    assert(stats->pp_newinf_timeline[bmax]==0);
    *(uint64_t*)buf=0;
    buf[8]=1;
    return 9;
  }
  */
  assert(bmax>bmin || (stats->pp_inf_timeline[bmax] && stats->pp_newinf_timeline[bmax]));

  for(;; ++bmin) {
    //printf("TotInf[%" PRIi32 "]=%" PRIu32 ",\tInf[%" PRIi32 "]=%" PRIu32 "\n",bmin,stats->pp_newinf_timeline[bmin],bmin,stats->pp_inf_timeline[bmin]);

    if(stats->pp_inf_timeline[bmin]) break;
    assert(stats->pp_newinf_timeline[bmin]==0);
  }
  assert(bmin<=0);

  const uint32_t nbins=bmax-bmin+1;

  ((uint32_t*)buf)[0]=htole32(nbins);
  ((uint32_t*)buf)[1]=htole32(-bmin);
  *(uint32_t*)(buf+8)=htole32(stats->maxedoutmintimeindex);
  *(uint32_t*)(buf+12)=htole64((int32_t)(stats->extinction?floor(stats->extinction_time):-INT32_MAX));
  buf+=16;

  int32_t b;
  const int32_t nbmbm=-bmin+nbins;

  for(b=bmin; b<=bmax; ++b) {
    //printf("Inf[%" PRIi32 "]=%" PRIu32 ",\tTotInf[%" PRIi32 "]=%" PRIu32 "\n",b,stats->pp_inf_timeline[b],b,stats->pp_newinf_timeline[b]);
    ((uint32_t*)buf)[b-bmin]=htole32(stats->pp_inf_timeline[b]);
    ((uint32_t*)buf)[b+nbmbm]=htole32(stats->pp_newinf_timeline[b]);
  }

  return 16+nbins*8;
}

inline static ssize_t tlo_write_reltime_postest_path(std_summary_stats const* stats, char* buf)
{
  int32_t bmin=-stats->tlppnnpers;
  int32_t bmax=bmin+stats->tlpptnvpers-1;

  for(; bmax>bmin; --bmax) {
    //printf("TotInf[%" PRIi32 "]=%" PRIu32 ",\tInf[%" PRIi32 "]=%" PRIu32 "\n",bmax,stats->pp_newinf_timeline[bmax],bmax,stats->pp_inf_timeline[bmax]);

    if(stats->pp_inf_timeline[bmax] || stats->pp_newpostest_timeline[bmax]) break;
    assert(stats->pp_newinf_timeline[bmax]==0);
  }

  //If timelines are all zero
  /*
  if(bmax==bmin && stats->pp_inf_timeline[bmax]==0) {
    
    assert(stats->pp_newinf_timeline[bmax]==0);
    assert(stats->pp_newpostest_timeline[bmax]==0);
    *(uint64_t*)buf=0;
    buf[8]=1;
    return 9;
  }
  */
  assert(bmax>bmin || (stats->pp_inf_timeline[bmax] && stats->pp_newinf_timeline[bmax] && stats->pp_newpostest_timeline[bmax]));

  for(;; ++bmin) {
    //printf("TotInf[%" PRIi32 "]=%" PRIu32 ",\tInf[%" PRIi32 "]=%" PRIu32 "\n",bmin,stats->pp_newinf_timeline[bmin],bmin,stats->pp_inf_timeline[bmin]);

    if(stats->pp_inf_timeline[bmin]) break;
    assert(stats->pp_newinf_timeline[bmin]==0);
    assert(stats->pp_newpostest_timeline[bmin]==0);
  }
  assert(bmin<=0);

  const uint32_t nbins=bmax-bmin+1;

  ((uint32_t*)buf)[0]=htole32(nbins);
  ((uint32_t*)buf)[1]=htole32(-bmin);
  *(uint32_t*)(buf+8)=htole32(stats->maxedoutmintimeindex);
  *(uint32_t*)(buf+12)=htole64((int32_t)(stats->extinction?floor(stats->extinction_time):-INT32_MAX));
  buf+=16;

  int32_t b;
  const int32_t nbmbm=-bmin+nbins;
  const int32_t tnbmbm=-bmin+2*nbins;

  for(b=bmin; b<=bmax; ++b) {
    //printf("Inf[%" PRIi32 "]=%" PRIu32 ",\tTotInf[%" PRIi32 "]=%" PRIu32 "\n",b,stats->pp_inf_timeline[b],b,stats->pp_newinf_timeline[b]);
    ((uint32_t*)buf)[b-bmin]=htole32(stats->pp_inf_timeline[b]);
    ((uint32_t*)buf)[b+nbmbm]=htole32(stats->pp_newinf_timeline[b]);
    ((uint32_t*)buf)[b+tnbmbm]=htole32(stats->pp_newpostest_timeline[b]);
  }

  return 16+nbins*12;
}

#ifdef CT_OUTPUT
inline static int ctcompar(const void* first, const void* second){return ((*(ctposinf**)first)->postesttime<(*(ctposinf**)second)->postesttime?-1:((*(ctposinf**)first)->postesttime>(*(ctposinf**)second)->postesttime?1:0));}

inline static ssize_t ct_write_func(std_summary_stats const* stats, char* buf)
{
  uint32_t i;
  char* buf0=buf;

  //printf("Extinction: %u, nctentries: %u\n",stats->extinction,stats->nctentries);

  if(stats->maxedoutmintimeindex==INT32_MAX) {

    for(i=0; i<stats->nctentries; ++i) {
      *((uint32_t*)(buf))=htole32(stats->ctentries[i]->postesttime);
      *((uint32_t*)(buf+4))=htole32(stats->ctentries[i]->presymtime);
      *((uint32_t*)(buf+8))=htole32(stats->ctentries[i]->id);
      *((uint32_t*)(buf+12))=htole32(stats->ctentries[i]->pid);
      *((uint32_t*)(buf+16))=htole32(stats->ctentries[i]->ntracedcts);
      buf+=20;
    }

  } else {

    for(i=0; i<stats->nctentries; ++i) {

      if(floor(stats->ctentries[i]->postesttime/1440.) <= stats->maxedoutmintimeindex) {
	*((uint32_t*)(buf))=htole32(stats->ctentries[i]->postesttime);
	*((uint32_t*)(buf+4))=htole32(stats->ctentries[i]->presymtime);
	*((uint32_t*)(buf+8))=htole32(stats->ctentries[i]->id);
	*((uint32_t*)(buf+12))=htole32(stats->ctentries[i]->pid);
	*((uint32_t*)(buf+16))=htole32(stats->ctentries[i]->ntracedcts);
	buf+=20;
      }
    }
  }
  return buf-buf0;
}
#endif

