#include "../host.h"
#include "../memory.h"
#include "../stats.h"


typedef enum mem_cmd fu_mcommand_t;
typedef md_addr_t fu_address_t;
typedef byte_t buffer_t; 

/* io style */
typedef enum _fu_io_style_t {
	idirBuffer, /* in direction */
	odirBuffer, /* out direction */
	bidirBuffer /* in/out direction */
} fu_io_style_t;


typedef struct _fu_lv1_cache_Ceff_t /* Ceff for cache like structures, read from option */
{
	double iCeff;
	double eCeff;
} fu_lv1_cache_Ceff_t;

typedef struct _fu_lv1_io_opts_t /* Ceff for cache like structures, read from option */
{
	fu_io_style_t style;
	double iCeff;
	double eCeff;
	double svolt;
	unsigned buswidth;
	unsigned nacycles;
	unsigned nctcycles;
	unsigned bsize;
} fu_lv1_io_opts_t;

/* power dissipation type */
typedef struct _fu_lv1_pdissipation_t {
	double external; 	/* external switching power dissipation */
	double internal; 	/* internal switching power dissipation */
	double pdissipation; 
#ifdef PA_TRANS_COUNT	
	int aio_trans;
	int dio_trans;
#endif	
} fu_lv1_pdissipation_t;


typedef enum _pwr_frame_type {root,start} pwr_frame_type;

/* power dissipation window in a linked list design */
typedef struct _fu_lv1_pwrframe_t {
	pwr_frame_type type; /* root, start of frame,*/
	int frame_size;
	tick_t frame_start; /* starting cycle for frame */
	fu_lv1_pdissipation_t *frame_contents;
	struct _fu_lv1_pwr_frame_t * next;
} fu_lv1_pwr_frame_t;


typedef struct _fu_lv1_io_arg_t /* argument structure for message passing */
{
	fu_mcommand_t command;
	fu_address_t address;
	fu_lv1_pwr_frame_t *pwr_frame;
	buffer_t *buffer;
	unsigned bsize;
	tick_t now;
	unsigned lat; 
} fu_lv1_io_arg_t;

typedef struct _fu_lv1_uarch_arg_t /* argument structure for message passing */
{
	fu_lv1_pwr_frame_t * pwr_frame;
	tick_t now;
} fu_lv1_uarch_arg_t;

typedef struct _fu_lv1_pa_trace_arg_t /* argument structure for message passing */
{
	double * window;
	char * miss_window;
#ifdef PA_TRANS_COUNT	
	int * aio_ham_window;
	int * dio_ham_window;
#endif	

	FILE *fp;
	char cache_miss;
	tick_t now;
} fu_lv1_pa_trace_arg_t;


typedef struct _fu_lv1_Ceff_list_t /* list of Ceff for each component, read from option */
{
	double alu_Ceff;
	double fpu_Ceff;
	double mult_Ceff;
	double bpred_Ceff;
	double rf_Ceff;
	fu_lv1_cache_Ceff_t il1_Ceff;
	fu_lv1_cache_Ceff_t dl1_Ceff;
	fu_lv1_cache_Ceff_t il2_Ceff;
	fu_lv1_cache_Ceff_t dl2_Ceff;
	fu_lv1_cache_Ceff_t dtlb_Ceff;
	fu_lv1_cache_Ceff_t itlb_Ceff;
} fu_lv1_Ceff_list_t;
