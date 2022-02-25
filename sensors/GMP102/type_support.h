/*
 *
 ****************************************************************************
 * Copyright (C) 2016 GlobalMEMS, Inc. <www.globalmems.com>
 * All rights reserved.
 *
 * File : type_support.h
 *
 * Date : 2016/07/21
 *
 * Revision : 1.0.0
 *
 * Usage: Define various support type
 *
 ****************************************************************************
 * 
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 **************************************************************************/
 
#ifndef __TYPE_SUPPORT_H__
#define __TYPE_SUPPORT_H__

/*signed integer types*/
typedef	signed char  s8;
typedef	signed short int s16;
typedef	signed int s32;
typedef	signed long int s64;

/*unsigned integer types*/
typedef	unsigned char u8;
typedef	unsigned short int u16;
typedef	unsigned int u32;
typedef	unsigned long int u64;

typedef union {
   struct{
      s32 x;
      s32 y;
      s32 z;
			s32 t;
   } u;
   s32 v[4];
} raw_data_xyzt_t;

#endif //__TYPE_SUPPORT_H__
