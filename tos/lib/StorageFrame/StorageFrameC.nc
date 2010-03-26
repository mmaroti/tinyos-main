configuration StorageFrameC{
	provides interface StreamStorage;	
}
implementation{
	components StreamStorageP, StorageFrameP;
	StreamStorage=StorageFrameP.Framed;
}