/*
*********************************************************************************************************
*	函 数 名: bsp_Hcs301Task50us
*	功能说明: hcs301编码50us定时任务
*	形    参:
*	返 回 值:
*********************************************************************************************************
*/
#define IsInArea(val, min, max) (((val) > (min)) && ((val) < (max)))
static void bsp_Hcs301Task50us(void)
{
  const struct
  {
    uint32_t th_min;
    uint32_t th_max;
    uint32_t bit_cyc_min;
    uint32_t bit_cyc_max;
  } cn_tWave =
      {
          .th_min = 52,
          .th_max = 132,
          .bit_cyc_min = 14,
          .bit_cyc_max = 40};

  static uint32_t lvl_cnt, high_cnt;
  uint32_t u32Cycle;

  if (s_tHcs301.isready == false)
    return;

  s_tHcs301.lst_io = s_tHcs301.cur_io;
  s_tHcs301.cur_io = s_tHcs301.ReadRfIO();

  if (s_tHcs301.lst_io == s_tHcs301.cur_io)
    lvl_cnt++;
  else
  {
    switch (s_tHcs301.rf_sta)
    {
    case en_rf_sts_th:
      if ((SET == s_tHcs301.cur_io) && 
        IsInArea(lvl_cnt, cn_tWave.th_min, cn_tWave.th_max))
      {
        s_tHcs301.rf_sta = en_rf_sts_data;
        s_tHcs301.u32RecvIdx = 0;
        s_tHcs301.u64Cur_data = 0;
      }

      break;

    case en_rf_sts_data:
      if (RESET == s_tHcs301.cur_io)
      {
        high_cnt = lvl_cnt;
      }
      else
      {
        u32Cycle = high_cnt + lvl_cnt;

        if (!IsInArea(u32Cycle, cn_tWave.bit_cyc_min, cn_tWave.bit_cyc_max))
          s_tHcs301.rf_sta = en_rf_sts_th;
        else
        {
          if (high_cnt < lvl_cnt)
            s_tHcs301.u64Cur_data |= 1ull << s_tHcs301.u32RecvIdx;

          s_tHcs301.u32RecvIdx++;

          if (s_tHcs301.u32RecvIdx >= 64)
            s_tHcs301.rf_sta = en_rf_sts_ok;
        }
      }

      break;

    case en_rf_sts_ok:
      break;

    default:
      break;
    }

    lvl_cnt = 0;
  }
}
#undef IsInArea
