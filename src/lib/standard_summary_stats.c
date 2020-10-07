/**
 * @file standard_summary_stats.c
 * @brief User-defined functions to compute standard summary statistics.
 * @author <Pierre-Luc.Drouin@drdc-rddc.gc.ca>, Defence Research and Development Canada Ottawa Research Centre.
 */

#include "standard_summary_stats.h"

void std_stats_init(sim_vars* sv, bool ngeninfs)
{
  std_summary_stats* stats=(std_summary_stats*)sv->dataptr;

  stats->timelineshift=0;
  stats->tnpersa=stats->npers=(int)sv->pars.tmax+1;
  stats->inf_timeline=(uint32_t*)malloc(stats->tnpersa*sizeof(uint32_t));
  stats->newinf_timeline=(uint32_t*)malloc(stats->tnpersa*sizeof(uint32_t));
  stats->postest_timeline=(uint32_t*)malloc(stats->tnpersa*sizeof(uint32_t));
  stats->newpostest_timeline=(uint32_t*)malloc(stats->tnpersa*sizeof(uint32_t));
  stats->ext_timeline=(ext_timeline_info*)malloc(stats->tnpersa*sizeof(ext_timeline_info));

  int32_t i;

  if(ngeninfs) {
    stats->nainfbins=INIT_NINF_ALLOC;

    for(i=stats->tnpersa-1; i>=0; --i) {
      stats->ext_timeline[i].ngeninfs=(uint64_t*)malloc(INIT_NINF_ALLOC*sizeof(uint64_t));
      //printf("ext_timeline[%i].ngeninfs=%p\n",i,stats->ext_timeline[i].ngeninfs);
      memset(stats->ext_timeline[i].ngeninfs,0,INIT_NINF_ALLOC*sizeof(uint64_t));
    }

  } else stats->nainfbins=stats->ninfbins=0;

  stats->lmax=UINT32_MAX;
  stats->nimax=UINT32_MAX;

#ifdef CT_OUTPUT
  stats->nactentries=INIT_NACTENTRIES;
  stats->ctentries=(ctposinf**)malloc(INIT_NACTENTRIES*sizeof(ctposinf*));

  for(i=stats->nactentries-1; i>=0; --i) stats->ctentries[i]=(ctposinf*)malloc(sizeof(ctposinf));
#endif
}

void std_stats_free(std_summary_stats* stats)
{
  free(stats->inf_timeline-stats->timelineshift);
  free(stats->newinf_timeline-stats->timelineshift);
  free(stats->postest_timeline-stats->timelineshift);
  free(stats->newpostest_timeline-stats->timelineshift);

  ext_timeline_info* const set=stats->ext_timeline-stats->timelineshift;
  int32_t i;

  if(stats->nainfbins) {

    for(i=stats->tnpersa-1; i>=0; --i) {
      //printf("Free ext_timeline[%i] (%p)\n",i-stats->timelineshift,set[i].ngeninfs);
      free(set[i].ngeninfs);
    }
  }
  free(stats->ext_timeline-stats->timelineshift);

#ifdef CT_OUTPUT

  for(i=stats->nactentries-1; i>=0; --i) free(stats->ctentries[i]);
  free(stats->ctentries);
#endif
}
