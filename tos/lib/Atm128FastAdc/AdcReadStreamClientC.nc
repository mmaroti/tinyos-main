/* $Id: AdcReadStreamClientC.nc,v 1.2 2009-12-29 23:11:30 mmaroti Exp $
 * Copyright (c) 2005 Intel Corporation
 * All rights reserved.
 *
 * This file is distributed under the terms in the attached INTEL-LICENSE     
 * file. If you do not find these files, copies can be found by writing to
 * Intel Research Berkeley, 2150 Shattuck Avenue, Suite 1300, Berkeley, CA, 
 * 94704.  Attention:  Intel License Inquiry.
 */
/**
 * Provide, as per TEP101, arbitrated access via a ReadStream interface to
 * the Atmega128 ADC.  Users of this component must link it to an
 * implementation of Atm128AdcConfig which provides the ADC parameters
 * (channel, etc).
 * 
 * @author David Gay
 */

#include "Adc.h"

generic configuration AdcReadStreamClientC() {
  provides interface ReadStream<uint16_t>;
  uses {
    interface Atm128AdcConfig;
    interface ResourceConfigure;
  }
}
implementation {
  components AdcReadStreamC, Atm128AdcC;

  enum {
    ADC_STREAM = unique(UQ_ADC_READSTREAM),
    ADC_CLIENT = unique(UQ_ATM128ADC_RESOURCE)
  };

  ReadStream = AdcReadStreamC.ReadStream[ADC_STREAM];
  Atm128AdcConfig = AdcReadStreamC.Atm128AdcConfig[ADC_STREAM];
  AdcReadStreamC.Resource[ADC_STREAM] -> Atm128AdcC.Resource[ADC_CLIENT];
  ResourceConfigure = Atm128AdcC.ResourceConfigure[ADC_CLIENT];
}
