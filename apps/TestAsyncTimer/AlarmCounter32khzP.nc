// $Id: AlarmCounter32khzP.nc,v 1.1 2010-07-30 19:42:08 mmaroti Exp $
/*
 * Copyright (c) 2005-2006 Intel Corporation
 * All rights reserved.
 *
 * This file is distributed under the terms in the attached INTEL-LICENSE     
 * file. If you do not find these files, copies can be found by writing to
 * Intel Research Berkeley, 2150 Shattuck Avenue, Suite 1300, Berkeley, CA, 
 * 94704.  Attention:  Intel License Inquiry.
 */
/**
 * Configure hardware timer 0 for use as the mica family's millisecond
 * timer.  This component does not follow the TEP102 HAL guidelines as
 * there is only one compare register for timer 0, which is used to
 * implement HilTimerMilliC. Hence it isn't useful to expose an
 * AlarmMilliC or CounterMillIC component.
 * 
 * @author David Gay <dgay@intel-research.net>
 * @author Martin Turon <mturon@xbow.com>
 */
#include <Atm128Timer.h>

configuration AlarmCounter32khzP
{
  provides interface Init;
  provides interface Alarm<T32khz, uint32_t>;
  provides interface Counter<T32khz, uint32_t>;
}
implementation
{
  components new Atm128AlarmAsyncC(T32khz, ATM128_CLK8_NORMAL);

  Init = Atm128AlarmAsyncC;
  Alarm = Atm128AlarmAsyncC;
  Counter = Atm128AlarmAsyncC;
}
