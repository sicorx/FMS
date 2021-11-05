#include "include.h"

extern struct equip_conn_info *conn_info[];
extern struct equip_status *gp_status[];

int readfrom_module(int fd, unsigned char *buff, int index, int cmd_type)
{
    int ret=0;
    int reply_size=0;
    unsigned char crc8=0;
    unsigned short crc16=0;

    struct equip_conn_info *pConnInfo = conn_info[index];

    switch(pConnInfo->model_seq)
    {
		case MST :
			if(readn_timeout(fd, (char *)buff, MST_RES_SIZE, 2) != MST_RES_SIZE)
			{
				fileLog(WARNING, "eseq=[%d] MST THS Packet Header Recv Fail..\n", pConnInfo->eseq);
				return -1;
			}

			if(buff[0] != MST_STX || buff[MST_RES_SIZE-1] != MST_ETX || buff[MST_RES_SIZE-2] != exelusive_or_crc_gen(buff, MST_RES_SIZE-2))
			{
				fileLog(WARNING, "eseq=[%d] MST THS CRC Check Error\n", pConnInfo->eseq);
				return -1;
			}

			if(pConnInfo->id != (unsigned int)buff[2])
			{
				fileLog(WARNING, "eseq=[%d] MST THS Response ID Check Error: REQ[%02X] RES[%02X]\n", pConnInfo->eseq, pConnInfo->id, buff[2]);
				return -1;
			}
			break;

		case MST_ASCII :
		case MST_HMM21 :
		case MST_COOLER :
			if(readn_timeout(fd, (char *)buff, MST_HMM21_RES_CNT, 2) !=  MST_HMM21_RES_CNT)
			{
				fileLog(WARNING, "THS Packet Header Recv Fail..\n");
				return -1;
			}

			crc8 = crc_gen(buff, 50);

			if(buff[50] != get_hex((crc8 & 0xF0)>>4) || buff[51] != get_hex( crc8 & 0x0F))
			{
				fileLog(WARNING, "THS CRC Check Error\n");
				return -1;
			}
			break;

		case NEMO :
		case NEMO_ECU :
			if(readn_timeout(fd, (char *)buff, NEMO_RES_SIZE, 2) != NEMO_RES_SIZE)
			{
				fileLog(WARNING, "NEMO THS Packet Header Recv Fail..\n");
				return -1;
			}

			if(pConnInfo->id != (unsigned int)(buff[1]-0x40))
			{
				fileLog(WARNING, "eseq=[%d] MST THS Response ID Check Error: REQ[%02X] RES[%02X]\n", pConnInfo->eseq, pConnInfo->id, buff[1]-0x40);
				return -1;
			}
			break;

		case AR :
		case AR_HP8KB  : /* version 1.3.28 same as AR */
		case SYSTRONIC_P :
		case AR_HANP08 : /* version 1.3.28 same as SYSTRONIC_P */
		case AR_HP8EPQ : /* version 1.3.28 */
		case AR_HP8EPF : /* version 1.3.28 */
		case AR_HANE08 : /* version 1.3.28 */
			if(readn_timeout(fd, (char *)buff, AR_RES_SIZE, 2) != AR_RES_SIZE)
			{
				fileLog(WARNING, "THS Packet Header Recv Fail..\n");
				return -1;
			}

			crc8 = crc_gen(buff, AR_RES_SIZE - 1);
			if(crc8 != buff[AR_RES_SIZE - 1])
			{
				fileLog(WARNING, "THS CRC Check Error\n");
				return -1;
			}
			break;

		case AR_OLD :
		case AR_SPHCEX : /* version 1.3.28 same as AR_OLD */
			if(readn_timeout(fd, (char *)buff, AR_OLD_RES_SIZE, 2) != AR_OLD_RES_SIZE)
			{
				fileLog(WARNING, "THS Packet Header Recv Fail..\n");
				return -1;
			}

			crc8 = crc_gen(buff, AR_OLD_RES_SIZE - 1);
			if(crc8 != buff[AR_OLD_RES_SIZE - 1])
			{
				fileLog(WARNING, "THS CRC Check Error\n");
				return -1;
			}
			break;


		case AR_HANE09 :
		case AR_SSHP8Y1 : /* version 1.4.1-Debug */
		case AR_HANE09_EXOR :
			if(readn_timeout(fd, (char *)buff, AR_HANE09_RES_SIZE, 2) != AR_HANE09_RES_SIZE)
			{
				fileLog(WARNING, "THS Packet Header Recv Fail..\n");
				return -1;
			}

			if(pConnInfo->model_seq == AR_HANE09_EXOR)
				crc8 = exelusive_or_crc_gen(buff, AR_HANE09_RES_SIZE - 1);
			else
				crc8 = crc_gen(buff, AR_HANE09_RES_SIZE - 1);
			if(crc8 != buff[AR_HANE09_RES_SIZE - 1])
			{
				fileLog(WARNING, "THS CRC Check Error\n");
				return -1;
			}
			break;

		case AR_HANE09_Q :
			if(readn_timeout(fd, (char *)buff, AR_HANE09_RES_SIZE, 2) != AR_HANE09_RES_SIZE)
			{
				fileLog(WARNING, "THS Packet Header Recv Fail..\n");
				return -1;
			}

			crc8 = exelusive_or_crc_gen(buff, AR_HANE09_RES_SIZE - 1);
			if(crc8 != buff[AR_HANE09_RES_SIZE - 1])
			{
				fileLog(WARNING, "THS CRC Check Error\n");
				return -1;
			}
			break;

		case AR_36BYTE :
			if(readn_timeout(fd, (char *)buff, 36, 2) != 36)
			{
				fileLog(WARNING, "THS Packet Header Recv Fail..\n");
				return -1;
			}

			crc8 = crc_gen(buff, 36 - 1);
			if(crc8 != buff[36 - 1])
			{
				fileLog(WARNING, "THS CRC Check Error\n");
				return -1;
			}
			break;

		case SVU :
			if(readn_timeout(fd, (char *)buff, SVU_RES_SIZE, 2) != SVU_RES_SIZE)
			{
				fileLog(WARNING, "THS Packet Header Recv Fail..\n");
				return -1;
			}
			break;

	/*--------------------------- DDC --------------------------------------------*/
		case DDC_SHINSUNG :
			if(cmd_type == DDC_REQ_DATA_TYPE)
			{
				if(readn_timeout(fd, (char *)buff, DDC_SHINSUNG_DATA_RES_SIZE, 2) != DDC_SHINSUNG_DATA_RES_SIZE)
				{
					fileLog(WARNING, "THS Packet Header Recv Fail..\n");
					return -1;
				}

				crc8 = crc_gen(buff, DDC_SHINSUNG_DATA_RES_SIZE - 1);
				if(crc8 != buff[DDC_SHINSUNG_DATA_RES_SIZE - 1])
				{
				   fileLog(WARNING, "THS CRC Check Error\n");
					return -1;
				}

				if(pConnInfo->id != ((buff[3] << 8) | buff[4]))
				{
					fileLog(WARNING, "eseq=[%d] Response ID Check Error: REQ[%02X] RES[%02X]\n", pConnInfo->eseq, pConnInfo->id, ((buff[3] << 8) | buff[4]));
					return -1;
				}
			}
			else if(cmd_type == DDC_REQ_TIMER_TYPE)
			{
				if(readn_timeout(fd, (char *)buff, DDC_TIMER_RES_SIZE, 2) != DDC_TIMER_RES_SIZE)
				{
					fileLog(WARNING, "THS Packet Header Recv Fail..\n");
					return -1;
				}

				crc8 = crc_gen(buff, DDC_TIMER_RES_SIZE - 1);
				if(crc8 != buff[DDC_TIMER_RES_SIZE - 1])
				{
					fileLog(WARNING, "THS CRC Check Error\n");
					return -1;
				}

				if(pConnInfo->id != ((buff[3] << 8) | buff[4]))
				{
					fileLog(WARNING, "eseq=[%d] Response ID Check Error: REQ[%02X] RES[%02X]\n", pConnInfo->eseq, pConnInfo->id, ((buff[3] << 8) | buff[4]));
					return -1;
				}
			}
			break;

		case DDC_HANGUL :
			if(cmd_type == DDC_REQ_DATA_TYPE)
			{
				if(readn_timeout(fd, (char *)buff, DDC_HANGUL_DATA_RES_SIZE, 2) != DDC_HANGUL_DATA_RES_SIZE)
				{
					fileLog(WARNING, "THS Packet Header Recv Fail..\n");
					return -1;
				}

				crc8 = crc_gen(buff, DDC_HANGUL_DATA_RES_SIZE - 1);
				if(crc8 != buff[DDC_HANGUL_DATA_RES_SIZE - 1])
				{
					fileLog(WARNING, "THS CRC Check Error\n");
					return -1;
				}

				if(pConnInfo->id != ((buff[3] << 8) | buff[4]))
				{
					fileLog(WARNING, "eseq=[%d] Response ID Check Error: REQ[%02X] RES[%02X]\n", pConnInfo->eseq, pConnInfo->id, ((buff[3] << 8) | buff[4]));
					return -1;
				}
			}
			else if(cmd_type == DDC_REQ_TIMER_TYPE)
			{
				if(readn_timeout(fd, (char *)buff, DDC_TIMER_RES_SIZE, 2) != DDC_TIMER_RES_SIZE)
				{
					fileLog(WARNING, "THS Packet Header Recv Fail..\n");
					return -1;
				}

				crc8 = crc_gen(buff, DDC_TIMER_RES_SIZE - 1);
				if(crc8 != buff[DDC_TIMER_RES_SIZE - 1])
				{
					fileLog(WARNING, "THS CRC Check Error\n");
					return -1;
				}

				if(pConnInfo->id != ((buff[3] << 8) | buff[4]))
				{
					fileLog(WARNING, "eseq=[%d] Response ID Check Error: REQ[%02X] RES[%02X]\n", pConnInfo->eseq, pConnInfo->id, ((buff[3] << 8) | buff[4]));
					return -1;
				}
			}
			break;
	/*--------------------------- DDC --------------------------------------------*/
		case SYSTRONIC_I :
			if(readn_timeout(fd, (char *)buff, SYSTRONIC_I_RES_SIZE, 2) != SYSTRONIC_I_RES_SIZE)
			{
				fileLog(WARNING, "THS Packet Header Recv Fail..\n");
				return -1;
			}

			crc8 = crc_gen(buff, SYSTRONIC_I_RES_SIZE - 1);

			if(crc8 != buff[SYSTRONIC_I_RES_SIZE - 1])
			{
				fileLog(WARNING, "THS CRC Check Error\n");
				return -1;
			}
			break;

	/* version 1.3.28 -----------------------------------------------------*/
		case AR_STHCE :
			if(readn_timeout(fd, (char *)buff, AR_STHCE_RES_SIZE, 2) != AR_STHCE_RES_SIZE)
			{
				fileLog(WARNING, "THS Packet Header Recv Fail..\n");
				return -1;
			}

			crc8 = crc_gen(buff, AR_STHCE_RES_SIZE - 1);

			if(crc8 != buff[AR_STHCE_RES_SIZE - 1])
			{
				fileLog(WARNING, "THS CRC Check Error\n");
				return -1;
			}
			break;
	/* version 1.3.28 -----------------------------------------------------*/

	/* FAC1000 항온항습기 시작 =============================== */
		case FAC1000 :
			if(cmd_type == FAC1000_REQ_CMD)
			{
				if(readn_timeout(fd, (char *)buff, FAC1000_REQ_RES_SIZE, 2) != FAC1000_REQ_RES_SIZE)
				{
					fileLog(WARNING, "THS Packet Header Recv Fail..\n");
					return -1;
				}

				crc8 = crc_gen(buff, FAC1000_REQ_RES_SIZE - 1);

				if(crc8 != buff[FAC1000_REQ_RES_SIZE - 1])
				{
					fileLog(WARNING, "THS CRC Check Error\n");
					return -1;
				}

				if(pConnInfo->id != ((get_hex(buff[2]) * 10) + get_hex(buff[3])))
				{
					fileLog(WARNING, "eseq=[%d] Response ID Check Error: REQ[%02X] RES[%02X]\n", pConnInfo->eseq, pConnInfo->id, ((get_hex(buff[2]) * 10) + get_hex(buff[3])));
					return -1;
				}
			}
			else if(cmd_type == FAC1000_ON_CMD || cmd_type == FAC1000_OFF_CMD || cmd_type == FAC1000_SET_CMD)
			{
				if(readn_timeout(fd, (char *)buff, FAC1000_OPR_RES_SIZE, 2) != FAC1000_OPR_RES_SIZE)
				{
					fileLog(WARNING, "THS Packet Header Recv Fail..\n");
					return -1;
				}

				crc8 = crc_gen(buff, FAC1000_OPR_RES_SIZE - 1);

				if(crc8 != buff[FAC1000_OPR_RES_SIZE - 1])
				{
					fileLog(WARNING, "THS CRC Check Error\n");
					return -1;
				}

				if(pConnInfo->id != ((get_hex(buff[2]) * 10) + get_hex(buff[3])))
				{
					fileLog(WARNING, "eseq=[%d] Response ID Check Error: REQ[%02X] RES[%02X]\n", pConnInfo->eseq, pConnInfo->id, ((get_hex(buff[2]) * 10) + get_hex(buff[3])));
					return -1;
				}
			}
			break;
	/* FAC1000 항온항습기 종료 =============================== */

	/* 승일 전자 항온항습기(DDC) 2 ---------------------------------------- */
		case DDC_HANGUL2 :
		case SEUNGIL :
		case SEUNGIL_MLTM :
			if(cmd_type == DDC_REQ_DATA_TYPE)
			{
				if(readn_timeout(fd, (char *)buff, DDC_HANGUL_DATA_RES_SIZE, 2) != DDC_HANGUL_DATA_RES_SIZE)
				{
					fileLog(WARNING, "THS Packet Header Recv Fail..\n");
					return -1;
				}

				crc8 = crc_gen(buff, DDC_HANGUL_DATA_RES_SIZE - 1);

				if(crc8 != buff[DDC_HANGUL_DATA_RES_SIZE - 1])
				{
					fileLog(WARNING, "THS CRC Check Error\n");
					return -1;
				}

				if(pConnInfo->id != ((buff[3] << 8) | buff[4]))
				{
					fileLog(WARNING, "eseq=[%d] Response ID Check Error: REQ[%02X] RES[%02X]\n", pConnInfo->eseq, ((buff[3] << 8) | buff[4]));
					return -1;
				}
			}
			else
			{
				if(readn_timeout(fd, (char *)buff, DDC2_SET_RES_SIZE, 2) != DDC2_SET_RES_SIZE)
				{
					fileLog(WARNING, "THS Packet Header Recv Fail..\n");
					return -1;
				}

				crc8 = crc_gen(buff, DDC2_SET_RES_SIZE - 1);

				if(crc8 != buff[DDC2_SET_RES_SIZE - 1])
				{
					fileLog(WARNING, "THS CRC Check Error\n");
					return -1;
				}

				if(pConnInfo->id != ((buff[3] << 8) | buff[4]))
				{
					fileLog(WARNING, "eseq=[%d] Response ID Check Error: REQ[%02X] RES[%02X]\n", pConnInfo->eseq, ((buff[3] << 8) | buff[4]));
					return -1;
				}
			}
			break;
	/* ------------------ 승일 전자 항온항습기(DDC) 2 ----------------------*/

	/* 승일전자 뀌뚜라미 항온항습기 ---------------------------------------- */
		case SEUNGIL_STHC_MB_FW36 :
			if(cmd_type == DDC_REQ_DATA_TYPE)
			{
				if(readn_timeout(fd, (char *)buff, SEUNGIL_STHC_MB_FW36_RES_CNT, 2) != SEUNGIL_STHC_MB_FW36_RES_CNT)
				{
					fileLog(WARNING, "THS Packet Header Recv Fail..\n");
					return -1;
				}

				crc8 = crc_gen(buff, SEUNGIL_STHC_MB_FW36_RES_CNT - 1);

				if(crc8 != buff[SEUNGIL_STHC_MB_FW36_RES_CNT - 1])
				{
					fileLog(WARNING, "THS CRC Check Error\n");
					return -1;
				}

				if(pConnInfo->id != ((buff[3] << 8) | buff[4]))
				{
					fileLog(WARNING, "eseq=[%d] Response ID Check Error: REQ[%02X] RES[%02X]\n", pConnInfo->eseq, ((buff[3] << 8) | buff[4]));
					return -1;
				}
			}
			else
			{
				if(readn_timeout(fd, (char *)buff, DDC2_SET_RES_SIZE, 2) != DDC2_SET_RES_SIZE)
				{
					fileLog(WARNING, "THS Packet Header Recv Fail..\n");
					return -1;
				}

				crc8 = crc_gen(buff, DDC2_SET_RES_SIZE - 1);

				if(crc8 != buff[DDC2_SET_RES_SIZE - 1])
				{
					fileLog(WARNING, "THS CRC Check Error\n");
					return -1;
				}

				if(pConnInfo->id != ((buff[3] << 8) | buff[4]))
				{
					fileLog(WARNING, "eseq=[%d] Response ID Check Error: REQ[%02X] RES[%02X]\n", pConnInfo->eseq, ((buff[3] << 8) | buff[4]));
					return -1;
				}
			}
			break;
	/* ------------------ 승일전자 뀌뚜라미 항온항습기 ----------------------*/

		case SYSKOREA :
			if(readn_timeout(fd, (char *)buff, SYSKOREA_REQ_RES_SIZE, 2) != SYSKOREA_REQ_RES_SIZE) { fileLog(WARNING, "THS Packet Header Recv Fail..\n"); return -1; }
			crc8 = crc_gen(buff, 35);
			if(crc8 != buff[35]) { fileLog(WARNING, "THS CRC Check Error\n"); return -1; }
			break;

		case BY_CRC2004 :
			if(readn_timeout(fd, (char *)buff, BY_CRC2004_REQ_RES_SIZE, 2) != BY_CRC2004_REQ_RES_SIZE) { fileLog(WARNING, "THS Packet Header Recv Fail..\n"); return -1; }
			if(buff[0] != 0xCC || buff[29] != 0xFF) { fileLog(WARNING, "Invalid Response Packet..\n"); return -1; }
			crc8 = crc_gen(buff+1, 27);
			if(crc8 != buff[28]) { fileLog(WARNING, "THS CRC Check Error\n"); return -1; }
			break;

		case DY_SS2000 :
		case AR_HANE09_DYRCU :
			if(read_dyrcu_com_frame_timeout(fd, buff, 2) != 0) { fileLog(WARNING, "[%s:%d] eseq=[%d] Packet Header Recv Fail..\n", __FUNCTION__, __LINE__, pConnInfo->eseq); return -1; }
			reply_size = (buff[4]<<8)|buff[5];
			crc8 = crc_gen(buff+6, reply_size-1);
			if(crc8 != buff[reply_size+5]) { fileLog(WARNING, "[%s:%d] eseq=[%d] DYRCU Response CRC Check Error.. Compute CRC=[%02X] Recv CRC=[%02x][%02X]\n", __FUNCTION__, __LINE__, pConnInfo->eseq); return -1; }
			break;
		case ITTS2 :
			if(readn_timeout(fd, (char *)buff, 5, 2) != 5) { fileLog(WARNING, "MODBUS Response Packet Header Recv Fail..\n"); return -1; }
			if(buff[1] & 0x80) { modbus_exception(buff[2]); return -1; }
			if(!(buff[1] == 3 || buff[1] == 4 || buff[1] == 5 || buff[1] == 16)) { return -1; }
			if(readn_timeout(fd, (char *)buff+5, buff[2], 1) != buff[2]) { fileLog(WARNING,  "[%s:%d] eseq=[%d] MODBUS Response Packet Body Recv Fail..\n"); return -1; }

			if(pConnInfo->id != (unsigned int)buff[0]) { fileLog(CRITICAL, "[%s:%d] eseq=[%d] itts2(%d) Response ID Error: Request[%02X] Response[%02X]\n", __FUNCTION__, __LINE__, pConnInfo->eseq, pConnInfo->id, buff[0]); return -1; }

			crc16 = itts2_crc16(buff, buff[2]+3);
			if(buff[buff[2]+3] != ((crc16 & 0xFF00) >> 8) || buff[buff[2]+4] !=  (crc16 & 0x00FF))
			{
				fileLog(WARNING,  "[%s:%d] eseq=[%d] MODBUS Response Packet CRC Check Fail.. Compute CRC=[%02X][%02X], Recv CRC=[%02X][%02X]\n", __FUNCTION__, __LINE__, pConnInfo->eseq,(unsigned char)((crc16 & 0xFF00) >> 8), (unsigned char)(crc16 & 0x00FF), buff[buff[2]+3], buff[buff[2]+4]);
				return -1;
			}
			break;

		case INNOTUBE :
			if(pConnInfo->id != (unsigned int)buff[1])
			{
				fileLog(WARNING, "[%s:%d] eseq=[%d] INNOTUBE TH Response ID Check Error: REQ[%02X] RES[%02X]\n", __FUNCTION__, __LINE__, pConnInfo->eseq, pConnInfo->id, buff[1]);
				return -1;
			}

            if(readn_timeout(fd, (char *)buff, 8, 2) != 8)
            {
                fileLog(WARNING, "[%s:%d] eseq=[%d] Response frame read error\n", __FUNCTION__, __LINE__, pConnInfo->eseq);
                return -1;
            }

            // HEADER가 0xFA이고 Tail이 0x03이어야 한다.
            if(!(buff[0] == 0xFA && buff[7] == 0x03))
            {
                fileLog(WARNING, "[%s:%d] eseq=[%d] Received wrong data Error...\n", __FUNCTION__, __LINE__, pConnInfo->eseq);
                return -1;
            }

            if(buff[6] != exelusive_or_crc_gen(buff+1, 5))
            {
                fileLog(WARNING, "[%s:%d] eseq=[%d] CRC Error... response[%02X] compute[%02X]\n", __FUNCTION__, __LINE__, pConnInfo->eseq, buff[6], exelusive_or_crc_gen(buff+1, 5));
                return -1;
            }


			break;

		default :
			fileLog(CRITICAL, "[%s:%d] Undefine Recv Case eseq=[%d], model_seq=[%d]\n", __FUNCTION__, __LINE__, pConnInfo->eseq, pConnInfo->model_seq);
			break;
	}

	return ret;
}
