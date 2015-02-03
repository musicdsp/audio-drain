/** @file
 * @author Edouard DUPIN 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include "AutoLogInOut.h"
#include "debug.h"

airtalgo::AutoLogInOut::AutoLogInOut(const std::string& _value) :
  m_value(_value) {
	AIRTALGO_VERBOSE("                 '" << m_value << "' [START]");
}

airtalgo::AutoLogInOut::~AutoLogInOut() {
	AIRTALGO_VERBOSE("                 '" << m_value << "' [STOP]");
}