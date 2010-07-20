#include "AM.h"

interface SpanningTree
{

	/**
	 * Returns the node ID of current root of the network.
	 * @return <code>0xFFFF</code> if no root was detected.
	 */
	command am_addr_t rootAddress();

	/**
	 * Sets the node ID of the network's root.
	 * @param ra node ID of root.
	 */
	command void setRootAddress(am_addr_t ra);

	/**
	* Returns the node ID of the node's parent.
	* @return <code>0xFFFF</code> if no parent was detected.
	*/
	command am_addr_t getParent();

	/**
	* Sets the node ID of the node's parent.
	* @param parent node ID of parent.
	*/
	command  void setParent(am_addr_t parent);

}