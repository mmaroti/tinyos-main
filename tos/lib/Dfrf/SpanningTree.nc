#include "AM.h"

interface SpanningTree
{

	/**
	 * Returns the node ID of current root of the network.
	 * @return <code>0xFFFF</code> if no root was detected.
	 */
	command am_addr_t rootAddress();

	/**
	 * Sets the node ID of the gradient root.
	 * @param ra node ID of root.
	 */
	command void setRootAddress(am_addr_t ra);


	command am_addr_t getParent();


	command  void setParent(am_addr_t par);

}