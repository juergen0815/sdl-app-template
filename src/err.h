/*
 * err.h
 *
 *  Created on: 2013-01-15
 *      Author: jurgens
 */

#ifndef ERR_H_
#define ERR_H_

void ShowWindowsError( const char* msg, unsigned long err, const char* header = "SystemError!" );

void ShowError( const char* msg, const char* header = "Error!" );

#endif /* ERR_H_ */
