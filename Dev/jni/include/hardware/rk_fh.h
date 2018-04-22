#define RK_WIN_MAX_REGION 4
#define RK30_MAX_LAYER_SUPPORT 4
#define RK_MAX_BUF_NUM 10
#define u32 unsigned int
#define u8  unsigned char
#define u16 unsigned  short

/*ALPHA BLENDING MODE*/
enum alpha_mode {               /*  Fs       Fd */
	AB_USER_DEFINE     = 0x0,
	AB_CLEAR    	   = 0x1,/*  0          0*/
	AB_SRC      	   = 0x2,/*  1          0*/
	AB_DST    	       = 0x3,/*  0          1  */
	AB_SRC_OVER   	   = 0x4,/*  1   	    1-As''*/
	AB_DST_OVER    	   = 0x5,/*  1-Ad''   1*/
	AB_SRC_IN    	   = 0x6,
	AB_DST_IN    	   = 0x7,
	AB_SRC_OUT    	   = 0x8,
	AB_DST_OUT    	   = 0x9,
	AB_SRC_ATOP        = 0xa,
	AB_DST_ATOP    	   = 0xb,
	XOR                = 0xc,
	AB_SRC_OVER_GLOBAL = 0xd
}; /*alpha_blending_mode*/

struct rk_lcdc_post_cfg{
	u32 xpos;
	u32 ypos;
	u32 xsize;
	u32 ysize;
};
struct rk_fb_area_par {
	int ion_fd;	
	unsigned long phy_addr;
	int acq_fence_fd;
	u32 x_offset;		
	u32 y_offset;		
	u32 xpos;		/*start point in panel  --->LCDC_WINx_DSP_ST*/
	u32 ypos;
	u32 xsize;		/* display window width/height  -->LCDC_WINx_DSP_INFO*/
	u32 ysize;
	u32 xact;		/*origin display window size -->LCDC_WINx_ACT_INFO*/
	u32 yact;
	u32 xvir;		/*virtual width/height     -->LCDC_WINx_VIR*/
	u32 yvir;	
};


struct rk_fb_win_par {
	u8 data_format;        /*layer data fmt*/
	u8 win_id;
	u8 z_order;		/*win sel layer*/
	struct rk_fb_area_par area_par[RK_WIN_MAX_REGION];
	u32 alpha_mode;
	u32 g_alpha_val;
};

struct rk_fb_win_cfg_data {
	int ret_fence_fd;
	int rel_fence_fd[RK_MAX_BUF_NUM];
	struct  rk_fb_win_par win_par[RK30_MAX_LAYER_SUPPORT];
	struct  rk_lcdc_post_cfg post_cfg;
	u8      wait_fs;
	//u8      fence_begin;
};
