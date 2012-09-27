generic module ResourcePowerManagerProviderP(){
	provides interface Resource;
	provides interface ResourceDefaultOwner;
	uses interface Resource as SubResource;
	uses interface DiagMsg;
}
implementation{
	norace bool requested = FALSE;
	norace bool defaultOwner=TRUE;
	norace bool halfReady=FALSE;
	
  async command error_t Resource.request(){
		if(call DiagMsg.record()){
			call DiagMsg.str("RPMP req");
			call DiagMsg.uint8(requested);
			call DiagMsg.uint8(defaultOwner);
			call DiagMsg.send();
		}
		atomic{
			if(requested){
				if(call SubResource.isOwner())
					return EALREADY;
				else
					return EBUSY;
			}
			requested = TRUE;
		}
		halfReady = FALSE;
		signal ResourceDefaultOwner.requested();
		return call SubResource.request();
	}
	
	async command error_t Resource.immediateRequest(){
		if(call DiagMsg.record()){
			call DiagMsg.str("RPMP ireq");
			call DiagMsg.uint8(requested);
			call DiagMsg.uint8(defaultOwner);
			call DiagMsg.send();
		}
		atomic{
			if(requested){
				if(call SubResource.isOwner())
					return EALREADY;
				else
					return EBUSY;
			}
			requested = TRUE;
		}
		halfReady = FALSE;
		signal ResourceDefaultOwner.immediateRequested();
		if( !defaultOwner )
			return call SubResource.immediateRequest();
	}

	async command error_t Resource.release(){
		error_t err = call SubResource.release();
		if(call DiagMsg.record()){
			call DiagMsg.str("RPMP rel");
			call DiagMsg.uint8(requested);
			call DiagMsg.uint8(defaultOwner);
			call DiagMsg.send();
		}
		if(err == SUCCESS){
			defaultOwner = TRUE;
			signal ResourceDefaultOwner.granted();
			requested = FALSE;
		}
		return err;
	}

	async command bool Resource.isOwner(){
		return call SubResource.isOwner();
	}
	
	event void SubResource.granted(){
		if(call DiagMsg.record()){
			call DiagMsg.str("RPMP subgr");
			call DiagMsg.uint8(requested);
			call DiagMsg.uint8(defaultOwner);
			call DiagMsg.send();
		}
		if(halfReady)
			signal Resource.granted();
		else
			halfReady = TRUE;
	}
	
	task void grantedTask(){
		signal Resource.granted();
	}
	
	async command error_t ResourceDefaultOwner.release(){
		if(call DiagMsg.record()){
			call DiagMsg.str("RPMP do rel");
			call DiagMsg.uint8(requested);
			call DiagMsg.uint8(defaultOwner);
			call DiagMsg.send();
		}
		if( requested && defaultOwner){
			defaultOwner = FALSE;
			if(halfReady)
				post grantedTask();
			else
				halfReady = TRUE;
		}
		return SUCCESS;
	}
	
	async command bool ResourceDefaultOwner.isOwner(){
		return defaultOwner;
	}
}