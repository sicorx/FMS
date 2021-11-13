#ifndef _DPMDEF_H_
#define _DPMDEF_H_ 1

/* MODEL DEFINE */
#define DPM_MPM330				0x0301	//769 //작업완료
#define DPM_IMPRO				0x0302	//770 
#define DPM_ViMAC_IV_V			0x0303	//771 
#define DPM_ODT304				0x0304	//772 
#define DPM_ACCURA3300			0x0305	//773 
#define DPM_DM2C_MT71			0x0306	//774 
#define DPM_ODT3XX				0x0307	//775 
#define DPM_MPM330A				0x0308	//776 
#define DPM_GiMACi				0x0309	//777 
#define DPM_GiPAM115FI			0x030A	//778 
#define DPM_GiMACIIPlus			0x030B	//779 
#define DPM_ODE1XX				0x030C	//780 
#define DPM_TMECS_200			0x030D	//781 
#define DPM_GiPAM2000FIM		0x030E	//782 
#define DPM_ION_6200			0x030F	//783 
#define DPM_ION_7650			0x0310	//784 
#define DPM_KB_GD10_L01			0x0311	//785 
#define DPM_ACCURA3500			0x0312	//786
#define DPM_ACCURA3550			0x0313	//787 
#define DPM_GiPAM2000FIM_DI		0x0314	//788 
#define DPM_GE_F650				0x0315	//789 
#define DPM_ACCURA3300_DYRCU	0x0316	//790 
#define DPM_MAX304_MELSEC		0x0317	//791 
#define DPM_MAX304_MELSEC_DYRCU	0x0318	//792 
#define DPM_RTP300				0x0319	//793 
#define DPM_RTM050				0x031A	//794 
#define DPM_MT4Y				0x031B	//795 
#define DPM_MT4Y_DYRCU			0x031C	//796 
#define DPM_MPM330A_DYRCU		0x031D	//797 
#define DPM_ODT3XX_GLOFA_FENET	0x031E	//798
#define DPM_GiMACi_SG1040		0x0324	//804 
#define DPM_MPM330A_TR			0x0325	//805 
#define DPM_ATCB_VIRTUAL		0x0326	//806 
#define DPM_MPM330A_PMS			0x0327	//807 
#define DPM_HDUNI_TOTAL_POW		0x0328	//808 
#define DPM_KDY_200				0x0329	//809 
#define RTU_MPM330A				0x032A	//810 
#define RTU_DPM_GiMACi			0x032B	//811 
#define RTU_DPM_GiMACIIPlus		0x032C	//812 
#define RTU_DPM_GiPAM115FI		0x032D	//813 
#define DPM_ACCURA3700			0x032E	//814 
#define DPM_ACCURA2300S			0x032F	//815 
/* MODEL DEFINE */



#define KDY200_REQ_CMD			0x04
#define KDY200_REQ_ADDR			0x00
#define KDY200_REQ_CNT			44

#define MPM330_READ_CMD_TYPE    0x03    /* MPM330 계측 데이타 타입 */
#define MPM330_REQUEST_SIZE     8       /* MPM330 계측 데이타 타입 송신 프레임 사이즈 */
#define MPM330_RESPONSE_SIZE    83      /* MPM330 계측 데이타 타입 수신 프레임 사이즈 */

#define IMPRO_READ_CMD_TYPE     0x04    /* i-mPRO 계측 데이타 타입 */
#define IMPRO_REQUEST_SIZE      8       /* i-mPRO 계측 계측 데이타 타입 송신 프레임 사이즈 */

#define ViMAC_IV_V_READ_CMD     0x04    /* ViMAC IV, V 계측 데이타 타입 */
#define ViMAC_IV_V_ADDR         0x0000  /* ViMAC IV, V 설비 어드레스 */
#define ViMAC_IV_V_WORD_CNT     30      /* ViMAC IV, V 설비 워드 카운트 */
#define ViMAC_IV_V_REQ_SIZE     8       /* ViMAC IV, V 설비 송신 프레임 사이즈 */
#define ViMAC_IV_V_RES_SIZE     65      /* ViMAC IV, V 설비 수신 프레임 사이즈 (30*2) + 5 */

#define ACCURA3300_READ_CMD     0x03    /* Read Holding Registers 명령어*/
#define ACCURA3300_ADDR         100     /* 어드레스, 40000번지에서 상대적인 어드레스를 나타냄. 40001=0, 40002=1 */ 
#define ACCURA3300_WORD_CNT     53      /* Read word count */
#define VSA_SCALE   0.1
#define VSB_SCALE   0.001

#define DM2C_MT71_READ_CMD      0x03    /* Read Holding Registers 명령어 */
#define DM2C_MT71_ADDR          0x0000  /* DM2C-MT71 시작 어드레스 */
#define DM2C_MT71_WORD_CNT      52      /* Read word count */

#define ODT3XX_READ_CMD         0x04    /* Read Holding Registers 명령어 */
#define ODT3XX_WORD_CNT         0x0E    /* Read word count */
#define ODT3XX_ADDR             0x0000  /* ODT-3XX 시작 어드레스 */

#define GiMACi_REQ_CMD			0x04
#define GiMACi_REQ_ADDR			0
#define GiMACi_REQ_CNT			44

#define GiPAM115FI_REQ_CMD		0x04
#define GiPAM115FI_REQ_ADDR		4
#define GiPAM115FI_REQ_CNT		42

#define GiMACIIPlus_REQ_CMD		0x04
#define GiMACIIPlus_REQ_ADDR	4
#define GiMACIIPlus_REQ_CNT		68

#define GiPAM2000FIM_READ_CMD   0x04

#define ION_6200_READ_CMD       0x03
#define ION_6200_WORD_CNT       57
#define ION_6200_ADDR           99

#define ION_7650_READ_CMD       0x03    /* Read Holding Registers 명령어*/

#endif /* _DPMDEF_H_ */