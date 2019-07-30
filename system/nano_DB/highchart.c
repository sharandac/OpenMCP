/***************************************************************************
 *            highchart.c
 *
 *  Sun Sep  5 16:56:37 2010
 *  Copyright  2010  Dirk Broßwick
 *  <sharandac@snafu.de>
 ****************************************************************************/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
	 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */

#include <avr/pgmspace.h>
#include "highchart.h"

const char JavaView_1_P[] PROGMEM = {
	"<html>"
	"<head>"
	"<script type=\"text/javascript\" src=\"js/jquery.min.js\"></script>"
	"<script type=\"text/javascript\" src=\"js/highcharts.js\"></script>"
	"<script type=\"text/javascript\" src=\"js/excanvas-compressed.js\"></script>"
	"<script type=\"text/javascript\">"
	"$(document).ready(function(){"
	"var chart=new Highcharts.Chart({"
	"chart:{renderTo:'container',defaultSeriesType:'spline',margin:[50,180,60,80]},"
	"title:{text: '%s',style:{margin:'10px 100px 0 0'}},"
	"xAxis:{type:'datetime'},"
	"plotOptions:{spline:{lineWidth:2,marker:{enabled:false},pointInterval:600000,pointStart:Date.UTC(%d,%d,%d,0,0,0),states:{hover:{marker:{enabled:true,symbol:'circle',radius:3,lineWidth:1}}}}},"
	"yAxis:{title:{text:'%s'}},"
	"tooltip:{formatter:function(){return'<b>'+ this.series.name +'</b><br>'+Highcharts.dateFormat('%%e. %%b %%Y, %%H:%%M',this.x)+': '+this.y+'%s';2}},"
	"legend:{layout:'vertical',style:{left:'auto',bottom:'auto',right:'10px',top: '100px'}},"
	"series:[" };

const char JavaView_2_P[] PROGMEM = {
	"]"
	"});});"
	"</script>"
	"</head>"
	"<body>"
	"<div id=\"container\" style=\"width:800px;height:400px\"></div>"
	"</body>"
	"</html>"
	"\r\n\r\n" };

const char * HIGHCHART_part_1( void )
{
	return( JavaView_1_P );
}

const char * HIGHCHART_part_2( void )
{
	return( JavaView_2_P );
}

