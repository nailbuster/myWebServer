/*
myWebServer.cpp for esp8266

Copyright (c) 2016 David Paiva (david@nailbuster.com). All rights reserved.


This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef _HTMLEMBED_h
#define _HTMLEMBED_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif


/* Stores default pages for WIFI setup and FSBrowse for initial setup.
//
//
//const char PAGE_WIFISETUP[] PROGMEM = R"=====(
//
//const char PAGE_FSBROWSE[] PROGMEM = R"=====(
*/

const char PAGE_FSBROWSE[] PROGMEM = R"=====(
<html><head>
<meta http-equiv="content-type" content="text/html; charset=utf-8">

<style>
body {font-family: "lucida grande","Segoe UI",Arial, sans-serif; font-size: 14px;width:1024;padding:1em;margin:0;}
th {font-weight: normal; color: #1F75CC; background-color: #F0F9FF; padding:.5em 1em .5em .2em; 
	text-align: left;cursor:pointer;user-select: none;}
th .indicator {margin-left: 6px }
thead {border-top: 1px solid #82CFFA; border-bottom: 1px solid #96C4EA;border-left: 1px solid #E7F2FB;
	border-right: 1px solid #E7F2FB; }
#top {height:52px;}
#mkdir {display:inline-block;float:right;padding-top:16px;}
label { display:block; font-size:11px; color:#555;}
#file_drop_target {width:100%; padding:12px 0; border: 4px dashed #ccc;font-size:12px;color:#ccc;
	text-align: center;float:right;margin-right:20px;}
#file_drop_target.drag_over {border: 4px dashed #96C4EA; color: #96C4EA;}
#upload_progress {padding: 4px 0;}
#upload_progress .error {color:#a00;}
#upload_progress > div { padding:3px 0;}
.no_write #mkdir, .no_write #file_drop_target {display: none}
.progress_track {display:inline-block;width:200px;height:10px;border:1px solid #333;margin: 0 4px 0 10px;}
.progress {background-color: #82CFFA;height:10px; }
footer {font-size:11px; color:#bbbbc5; padding:4em 0 0;text-align: left;}
footer a, footer a:visited {color:#bbbbc5;}
#breadcrumb { padding-top:34px; font-size:15px; color:#aaa;display:inline-block;float:left;}
#folder_actions {width: 50%;float:right;}
a, a:visited { color:#00c; text-decoration: none}
a:hover {text-decoration: underline}
.sort_hide{ display:none;}
table {border-collapse: collapse;width:100%;}
thead {max-width: 1024px}
td { padding:.2em 1em .2em .2em; border-bottom:1px solid #def;height:30px; font-size:12px;white-space: nowrap;}
td.first {font-size:14px;white-space: normal;}
td.empty { color:#777; font-style: italic; text-align: center;padding:3em 0;}
.is_dir .size {color:transparent;font-size:0;}
.is_dir .size:before {content: "--"; font-size:14px;color:#333;}
.is_dir .download{visibility: hidden}
a.delete {display:inline-block;
	background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAoAAAAKCAYAAACNMs+9AAAABGdBTUEAAK/INwWK6QAAABl0RVh0U29mdHdhcmUAQWRvYmUgSW1hZ2VSZWFkeXHJZTwAAADtSURBVHjajFC7DkFREJy9iXg0t+EHRKJDJSqRuIVaJT7AF+jR+xuNRiJyS8WlRaHWeOU+kBy7eyKhs8lkJrOzZ3OWzMAD15gxYhB+yzAm0ndez+eYMYLngdkIf2vpSYbCfsNkOx07n8kgWa1UpptNII5VR/M56Nyt6Qq33bbhQsHy6aR0WSyEyEmiCG6vR2ffB65X4HCwYC2e9CTjJGGok4/7Hcjl+ImLBWv1uCRDu3peV5eGQ2C5/P1zq4X9dGpXP+LYhmYz4HbDMQgUosWTnmQoKKf0htVKBZvtFsx6S9bm48ktaV3EXwd/CzAAVjt+gHT5me0AAAAASUVORK5CYII=) no-repeat scroll 0 2px;
	color:#d00;	margin-left: 15px;font-size:11px;padding:0 0 0 13px;
}
.name {
	background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAYAAADgdz34AAABAklEQVRIie2UMW6DMBSG/4cYkJClIhauwMgx8CnSC9EjJKcwd2HGYmAwEoMREtClEJxYakmcoWq/yX623veebZmWZcFKWZbXyTHeOeeXfWDN69/uzPP8x1mVUmiaBlLKsxACAC6cc2OPd7zYK1EUYRgGZFkG3/fPAE5fIjcCAJimCXEcGxKnAiICERkSIcQmeVoQhiHatoWUEkopJEkCAB/r+t0lHyVN023c9z201qiq6s2ZYA9jDIwx1HW9xZ4+Ihta69cK9vwLvsX6ivYf4FGIyJj/rg5uqwccd2Ar7OUdOL/kPyKY5/mhZJ53/2asgiAIHhLYMARd16EoCozj6EzwCYrrX5dC9FQIAAAAAElFTkSuQmCC) no-repeat scroll 0px 12px;
	padding:15px 0 10px 40px;
}
.is_dir .name {
	background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAYAAABzenr0AAAABHNCSVQICAgIfAhkiAAAAAlwSFlzAAADdgAAA3YBfdWCzAAAABl0RVh0U29mdHdhcmUAd3d3Lmlua3NjYXBlLm9yZ5vuPBoAAAI0SURBVFiF7Vctb1RRED1nZu5977VQVBEQBKZ1GCDBEwy+ISgCBsMPwOH4CUXgsKQOAxq5CaKChEBqShNK222327f79n0MgpRQ2qC2twKOGjE352TO3Jl76e44S8iZsgOww+Dhi/V3nePOsQRFv679/qsnV96ehgAeWvBged3vXi+OJewMW/Q+T8YCLr18fPnNqQq4fS0/MWlQdviwVqNpp9Mvs7l8Wn50aRH4zQIAqOruxANZAG4thKmQA8D7j5OFw/iIgLXvo6mR/B36K+LNp71vVd1cTMR8BFmwTesc88/uLQ5FKO4+k4aarbuPnq98mbdo2q70hmU0VREkEeCOtqrbMprmFqM1psoYAsg0U9EBtB0YozUWzWpVZQgBxMm3YPoCiLpxRrPaYrBKRSUL5qn2AgFU0koMVlkMOo6G2SIymQCAGE/AGHRsWbCRKc8VmaBN4wBIwkZkFmxkWZDSFCwyommZSABgCmZBSsuiHahA8kA2iZYzSapAsmgHlgfdVyGLTFg3iZqQhAqZB923GGUgQhYRVElmAUXIGGVgedQ9AJJnAkqyClCEkkfdM1Pt13VHdxDpnof0jgxB+mYqO5PaCSDRIAbgDgdpKjtmwm13irsnq4ATdKeYcNvUZAt0dg5NVwEQFKrJlpn45lwh/LpbWdela4K5QsXEN61tytWr81l5YSY/n4wdQH84qjd2J6vEz+W0BOAGgLlE/AMAPQCv6e4gmWYC/QF3d/7zf8P/An4AWL/T1+B2nyIAAAAASUVORK5CYII=) no-repeat scroll 0px 10px;
	padding:15px 0 10px 40px;
}
.download {
	background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAB2klEQVR4nJ2ST2sTQRiHn5mdmj92t9XmUJIWJGq9NHrRgxQiCtqbl97FqxgaL34CP0FD8Qv07EHEU0Ew6EXEk6ci8Q9JtcXEkHR3k+zujIdUqMkmiANzmJdnHn7vzCuIWbe291tSkvhz1pr+q1L2bBwrRgvFrcZKKinfP9zI2EoKmm7Azstf3V7fXK2Wc3ujvIqzAhglwRJoS2ImQZMEBjgyoDS4hv8QGHA1WICvp9yelsA7ITBTIkwWhGBZ0Iv+MUF+c/cB8PTHt08snb+AGAACZDj8qIN6bSe/uWsBb2qV24/GBLn8yl0plY9AJ9NKeL5ICyEIQkkiZenF5XwBDAZzWItLIIR6LGfk26VVxzltJ2gFw2a0FmQLZ+bcbo/DPbcd+PrDyRb+GqRipbGlZtX92UvzjmUpEGC0JgpC3M9dL+qGz16XsvcmCgCK2/vPtTNzJ1x2kkZIRBSivh8Z2Q4+VkvZy6O8HHvWyGyITvA1qndNpxfguQNkc2CIzM0xNk5QLedCEZm1VKsf2XrAXMNrA2vVcq4ZJ4DhvCSAeSALXASuLBTW129U6oPrT969AK4Bq0AeWARs4BRgieMUEkgDmeO9ANipzDnH//nFB0KgAxwATaAFeID5DQNatLGdaXOWAAAAAElFTkSuQmCC) no-repeat scroll 0px 5px;
	padding:4px 0 4px 20px;
}
</style>
<script src="//ajax.googleapis.com/ajax/libs/jquery/1.8.2/jquery.min.js"></script>
<script> var curURL = '//'+location.hostname + location.pathname; </script>
<script>
(function($){
	$.fn.tablesorter = function() {
		var $table = this;
		this.find('th').click(function() {
			var idx = $(this).index();
			var direction = $(this).hasClass('sort_asc');
			$table.tablesortby(idx,direction);
		});
		return this;
	};
	$.fn.tablesortby = function(idx,direction) {
		var $rows = this.find('tbody tr');
		function elementToVal(a) {
			var $a_elem = $(a).find('td:nth-child('+(idx+1)+')');
			var a_val = $a_elem.attr('data-sort') || $a_elem.text();
			return (a_val == parseInt(a_val) ? parseInt(a_val) : a_val);
		}
		$rows.sort(function(a,b){
			var a_val = elementToVal(a), b_val = elementToVal(b);
			return (a_val > b_val ? 1 : (a_val == b_val ? 0 : -1)) * (direction ? 1 : -1);
		})
		this.find('th').removeClass('sort_asc sort_desc');
		$(this).find('thead th:nth-child('+(idx+1)+')').addClass(direction ? 'sort_desc' : 'sort_asc');
		for(var i =0;i<$rows.length;i++)
			this.append($rows[i]);
		this.settablesortmarkers();
		return this;
	}
	$.fn.retablesort = function() {
		var $e = this.find('thead th.sort_asc, thead th.sort_desc');
		if($e.length)
			this.tablesortby($e.index(), $e.hasClass('sort_desc') );
		
		return this;
	}
	$.fn.settablesortmarkers = function() {
		this.find('thead th span.indicator').remove();
		this.find('thead th.sort_asc').append('<span class="indicator">&darr;<span>');
		this.find('thead th.sort_desc').append('<span class="indicator">&uarr;<span>');
		return this;
	}
})(jQuery);
$(function(){
	var XSRF = (document.cookie.match('(^|; )_sfm_xsrf=([^;]*)')||0)[2];
	var MAX_UPLOAD_SIZE = 2500000;
	var $tbody = $('#list');
	$(window).bind('hashchange',list).trigger('hashchange');
	$('#table').tablesorter();
	
	$('.delete').live('click',function(data) {
	    var r = confirm('Do you Want to Delete '+ $(this).attr('data-file') + '?');
        if(r==true){
	  	$.post("",{'do':'delete',file:$(this).attr('data-file'),xsrf:XSRF},function(response){
			list();
		},'json');
		}
		return false;
	});

		$('#mkdir').submit(function(e) {
		var hashval = window.location.hash.substr(1),
			$dir = $(this).find('[name=name]');
		e.preventDefault();
		$dir.val().length && $.post('?',{'do':'mkdir',name:$dir.val(),xsrf:XSRF,file:hashval},function(data){
			list();
		},'json');
		$dir.val('');
		return false;
	});

		// file upload stuff
	$('#file_drop_target').bind('dragover',function(){
		$(this).addClass('drag_over');
		return false;
	}).bind('dragend',function(){
		$(this).removeClass('drag_over');
		return false;
	}).bind('drop',function(e){
		e.preventDefault();
		var files = e.originalEvent.dataTransfer.files;		
		uploadFile(files,0);
		$(this).removeClass('drag_over');
	});
	$('input[type=file]').change(function(e) {
		e.preventDefault();						
		uploadFile(this.files,0);
	});


		function uploadFile(files,i) {    //uploads all files one at a time!
		var folder = window.location.hash.substr(1);
		
		if(i >= files.length) {
        //$('#message').text('Done!');
		list();			//rerun list()
        return;
    }
        var file = files[i]; 

			if(file.size > MAX_UPLOAD_SIZE) {
			var $error_row = renderFileSizeErrorRow(file,folder);
			$('#upload_progress').append($error_row);
			window.setTimeout(function(){$error_row.fadeOut();},5000);
			return false;
		}
		
		var $row = renderFileUploadRow(file,folder);
		$('#upload_progress').append($row);
		var fd = new FormData();
		fd.append('file_data',file);
		fd.append('file',folder);
		fd.append('xsrf',XSRF);
		fd.append('do','upload');
		var xhr = new XMLHttpRequest();
		
		xhr.open('POST', '/upload');   
		xhr.onload = function() {
			$row.remove();    		
			uploadFile(files,i+1);  //upload nex file...
  		};
		xhr.upload.onprogress = function(e){
			if(e.lengthComputable) {
				$row.find('.progress').css('width',(e.loaded/e.total*100 | 0)+'%' );
			}
		};
	    xhr.send(fd);		
	}
	function renderFileUploadRow(file,folder) {
		return $row = $('<div/>')
			.append( $('<span class="fileuploadname" />').text( (folder ? folder+'/':'')+file.name))
			.append( $('<div class="progress_track"><div class="progress"></div></div>')  )
			.append( $('<span class="size" />').text(formatFileSize(file.size)) )
	};
	function renderFileSizeErrorRow(file,folder) {
		return $row = $('<div class="error" />')
			.append( $('<span class="fileuploadname" />').text( 'Error: ' + (folder ? folder+'/':'')+file.name))
			.append( $('<span/>').html(' file size - <b>' + formatFileSize(file.size) + '</b>'
				+' exceeds max upload size of <b>' + formatFileSize(MAX_UPLOAD_SIZE) + '</b>')  );
	}

		function list() {
		var hashval = window.location.hash.substr(1);
		$.get('?',{'do':'list','file':hashval},function(data) {
			$tbody.empty();
			$('#breadcrumb').empty().html(renderBreadcrumbs(hashval));
			if(data.success) {
				$.each(data.results,function(k,v){
					$tbody.append(renderFileRow(v));
				});
				!data.results.length && $tbody.append('<tr><td class="empty" colspan=5>This folder is empty</td></tr>')
				data.is_writable ? $('body').removeClass('no_write') : $('body').addClass('no_write');
			} else {
				console.warn(data.error.msg);
			}
			$('#table').retablesort();
		},'json');
	}
	function renderFileRow(data) {
		var $link = $('<a class="name" />')
			.attr('href',encodeURIComponent(data.name))
			.text(data.name);
		var $dl_link = $('<a/>').attr('href',encodeURIComponent(data.name)+'?download=true')
			.addClass('download').text('download');
        var $flash_avrlink = $('    <a/>').attr('href','flashavr?fname='+encodeURIComponent(data.name))
			.addClass('download').text('Flash AVR');
		var $flash_esplink = $('    <a/>').attr('href','flashesp?fname='+encodeURIComponent(data.name))
			.addClass('download').text('Flash ESP');
			
					
		var $delete_link = $('<a href="#" />').attr('data-file',data.name).addClass('delete').text('delete');
		var perms = [];
		if(data.is_readable) perms.push('read');
		if(data.is_writable) perms.push('write');
		if(data.is_executable) perms.push('exec');
		var $html = $('<tr />')
			.addClass(data.is_dir ? 'is_dir' : '')
			.append( $('<td class="first" />').append($link) )
			.append( $('<td/>').attr('data-sort',data.is_dir ? -1 : data.size)
				.html($('<span class="size" />').text(formatFileSize(data.size))) ) 
			.append( $('<td/>').attr('data-sort',data.mtime).text(formatTimestamp(data.mtime)) )
			.append( $('<td/>').text(perms.join('+')) )
			.append( $('<td/>').append($dl_link).append( data.is_deleteable ? $delete_link : '')
			.append( data.name.indexOf('.avr') > 0 ? $flash_avrlink : ''  )
			.append( data.name.indexOf('.bin') > 0 ? $flash_esplink : ''  ))
		return $html;
	}
	function renderBreadcrumbs(path) {
		var base = "",
			$html = $('<div/>').append( $('<a href=#>Home</a></div>') );
		$.each(path.split('/'),function(k,v){
			if(v) {
				$html.append( $('<span/>').text(' ▸ ') )
					.append( $('<a/>').attr('href','#'+base+v).text(v) );
				base += v + '/';
			}
		});
		return $html;
	}
	function formatTimestamp(unix_timestamp) {
		var m = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec'];
		var d = new Date(unix_timestamp*1000);
		return [m[d.getMonth()],' ',d.getDate(),', ',d.getFullYear()," ",
			(d.getHours() % 12 || 12),":",(d.getMinutes() < 10 ? '0' : '')+d.getMinutes(),
			" ",d.getHours() >= 12 ? 'PM' : 'AM'].join('');
	}
	function formatFileSize(bytes) {
		var s = ['bytes', 'KB','MB','GB','TB','PB','EB'];
		for(var pos = 0;bytes >= 1000; pos++,bytes /= 1024);
		var d = Math.round(bytes*10);
		return pos ? [parseInt(d/10),".",d%10," ",s[pos]].join('') : bytes + ' bytes';
	}
})

</script>
</head><body>
<div id="top">
	<div id="file_drop_target">
		Drag Files Here To Upload
		<b>or</b>
		<input type="file" multiple />
	</div>
	<div id="breadcrumb">&nbsp;</div>
</div>

<div id="upload_progress"></div>
<table id="table"><thead><tr>
	<th>Name</th>
	<th>Size</th>
	<th>Modified</th>
	<th>Permissions</th>
	<th>Actions</th>
</tr></thead><tbody id="list">
</tbody></table>
<br><br>
<input type=button onClick="location.href='../restartesp'" value='Press to Restart (when finished)'>
<footer>Simple ESP8266 SPIFFs filemanager</footer>
</body></html>

)=====";

/***************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************/


const char PAGE_WIFISETUP[] PROGMEM = R"=====(
   <meta name="viewport" content="width=device-width, initial-scale=1" />
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />

    <style type="text/css">
  form  { display: table;      }
  p     { display: table-row;  }
  label { display: table-cell; }
  input { display: table-cell; }
  </style>  
  
  <h1>WiFi Settings General</h1>     
  <hr>
<form action="">
<fieldset><legend>WiFi Configure</legend>    
<table border="0"  cellspacing="0" cellpadding="3" style="width:400px" >
<tr><td align="right">Device Name:</td><td><input type="text" id="devicename" name="devicename" value="myESP"></td></tr>
<tr><td align="right">SSID:</td><td><input type="text" id="ssid" name="ssid" value=""></td></tr>
<tr><td align="right">WiFi Password:</td><td><input type="password" id="password" name="password" value=""></td></tr>
<tr><td align="right">DHCP:</td><td><select id="dhcp" name="dhcp" value="1">                    
                        <option value="1">Enabled</option>
                    <option value="0">Disable</option>
                </select></td></tr>           
<tr><td align="right">IP:     </td><td><input type="text" id="ip_0" name="ip_0" size="3">.<input type="text" id="ip_1" name="ip_1" size="3">.<input type="text" id="ip_2" name="ip_2" size="3">.<input type="text" id="ip_3" name="ip_3" value="" size="3"></td></tr>
<tr><td align="right">Netmask:</td><td><input type="text" id="nm_0" name="nm_0" size="3">.<input type="text" id="nm_1" name="nm_1" size="3">.<input type="text" id="nm_2" name="nm_2" size="3">.<input type="text" id="nm_3" name="nm_3" size="3"></td></tr>
<tr><td align="right">Gateway:</td><td><input type="text" id="gw_0" name="gw_0" size="3">.<input type="text" id="gw_1" name="gw_1" size="3">.<input type="text" id="gw_2" name="gw_2" size="3">.<input type="text" id="gw_3" name="gw_3" size="3"></td></tr></table>
</fieldset>    
    <br>
<fieldset><legend>NTP Configure</legend>   
<table border="0"  cellspacing="0" cellpadding="3" style="width:310px" >        
<tr><td align="right">NTP Server:</td><td><select id="grabntp" name="grabntp" value="0">                    
                    <option value="0">Disable</option>
                    <option value="1">Enabled</option>
                </select></td></tr>            
<tr><td align="right">NTP Server:</td><td><input type="text" id="ntpserver" name="ntpserver" maxlength="172" value="0.de.pool.ntp.org"></td></tr>
<tr><td align="right">Update:</td><td><input type="text" id="update" name="update" size="3"maxlength="6" value="600"> minutes</td></tr>
<tr><td align="right">Timezone</td><td>
<select  id="tz" name="tz">
  <option value="-120">(GMT-12:00)</option>
  <option value="-110">(GMT-11:00)</option>
  <option value="-100">(GMT-10:00)</option>
  <option value="-90">(GMT-09:00)</option>
  <option value="-80">(GMT-08:00)</option>
  <option value="-70">(GMT-07:00)</option>
  <option value="-60">(GMT-06:00)</option>
  <option value="-50">(GMT-05:00)</option>
  <option value="-40">(GMT-04:00)</option>
  <option value="-35">(GMT-03:30)</option>
  <option value="-30">(GMT-03:00)</option>
  <option value="-20">(GMT-02:00)</option>
  <option value="-10">(GMT-01:00)</option>
  <option value="0">(GMT+00:00)</option>
  <option value="10">(GMT+01:00)</option>
  <option value="20">(GMT+02:00)</option>
  <option value="30">(GMT+03:00)</option>
  <option value="35">(GMT+03:30)</option>
  <option value="40">(GMT+04:00)</option>
  <option value="45">(GMT+04:30)</option>
  <option value="50">(GMT+05:00)</option>
  <option value="55">(GMT+05:30)</option>
  <option value="57">(GMT+05:45)</option>
  <option value="60">(GMT+06:00)</option>
  <option value="65">(GMT+06:30)</option>
  <option value="70">(GMT+07:00)</option>
  <option value="80">(GMT+08:00)</option>
  <option value="90">(GMT+09:00)</option>
  <option value="95">(GMT+09:30)</option>
  <option value="100">(GMT+10:00)</option>
  <option value="110">(GMT+11:00)</option>
  <option value="120">(GMT+12:00)</option>
  <option value="120">(GMT+12:00)</option>
  <option value="130">(GMT+13:00)</option>
</select>
</td></tr>    
<tr><td align="right">Daylight Savings:</td><td><select id="dst" name="dst" value="0">                    
                    <option value="1">Yes</option>
                    <option value="0">No</option>
                </select></td></tr>
</table>           
</fieldset>
    <br>
    
<fieldset><legend>ESP8266 WebServer Configure</legend>   
     <table border="0"  cellspacing="0" cellpadding="3" style="width:400px" >
     <tr><td align="right">Access Username: </td><td align="left">admin </td></tr>
     <tr><td align="right">Access Password:</td><td><input type="password" id="AccessPass" name="AccessPass" value=""></td></tr>
     <tr><td align="right">Confirm Access Password:</td><td><input type="password" id="AccessPass2" name="AccessPass2" value=""></td></tr>
     <tr><td><input type="checkbox" name="mDNSoff" value="true" >Disable mDNS</td></tr>
     <td><input type="checkbox" name="SoftAP" value="true" >SoftAP always ON</td>
     <td><input type="checkbox" name="CDNSoff" value="true" >Disable Captive DNS</td>     
     </table>
</fieldset>    
     <br>
    
      <input type="button" id="savebut" name="savebut" value="Save Settings" onclick="SendFormJson()" style="width: 300px; height: 50px;" />&nbsp&nbsp
      <input action="action" type="button" value="Back" onclick="history.go(-1);" style="width: 100px; height: 50px;" />      
 

</form>

<script>
var xmlhttp = new XMLHttpRequest();
var url = "/jsonload?f=wifiset.json";

xmlhttp.onreadystatechange = function() {
    if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
        var myArr = JSON.parse(xmlhttp.responseText);
        myFunction(myArr);
    }
};
xmlhttp.open("GET", url, true);
xmlhttp.send();

function myFunction(jsonData) {  //process the json from the ESP

var jsonDataKeys = Object.keys(jsonData);
for(var i=0; i<jsonDataKeys.length; i++){
    var fields = document.getElementsByName(jsonDataKeys[i]);                        
    for (var f=0; f<fields.length; f++){
    var field=fields[f];
    if (field.getAttribute('type')=='checkbox')                 
                              {if (field.getAttribute('value')==jsonData[jsonDataKeys[i]])  field.setAttribute('checked','checked');                                   
                              }     
    else if (field.getAttribute('type')=='radio')                 
                              {if (field.getAttribute('value')==jsonData[jsonDataKeys[i]])  field.setAttribute('checked','checked');                                         
                               } 
    else if (field.getAttribute('type')=='button')                 
                              {  }//do nothing  
                                                                      
    else {field.value =  jsonData[jsonDataKeys[i]];}
    } //for fields loop
  }//for loop
} //function

</script>




<script>
function SendFormJson(){ 
 var inputs=document.getElementsByTagName("*")
 if (inputs['AccessPass'].value!=inputs['AccessPass2'].value )
   {
    alert("WebServer Passwords are not the same!!")
    exit;
   } 
    if (inputs['devicename'].value=="" )
   {
    alert("Device Name cannot be blank!!")
    exit;
   } 
 inputs['savebut'].value="Restarting..."; 
 var data = {};
 var field;
 var i;
    for (i=0; i<inputs.length; i++) 
       {
         field = inputs[i];                   
         if (field.tagName == 'INPUT' || field.tagName == 'SELECT')
         {
          if (field.getAttribute('type') != undefined)                                  
                {
                  if (field.getAttribute('type')=='checkbox')                 
                              { 
                               if (field.checked)    
                                      {data[field.name] = field.value;}                                   
                              }     
                   else if (field.getAttribute('type')=='radio')                 
                              {
                               if (field.checked)    
                                      {data[field.name] = field.value;}                                   
                              }      
                   else if (field.getAttribute('type')=='button')                 
                              {  }//do nothing                                          
                   else {data[field.name] = field.value;}  //not checkbox          
                }
           else  {data[field.name] = field.value;}                          
        }//not input or select so do nothing...                       
       } //for loop
var xhttp = new XMLHttpRequest();       
xhttp.open("POST", "/jsonsave?f=wifiset.json", false);
xhttp.setRequestHeader("Content-type","application/json");//"application/x-www-form-urlencoded");  // 
xhttp.send(JSON.stringify(data));  
alert("Please reboot device!");
}
</script>


   



   

)=====";

/***************************************************************************************************************************************************************/
/***************************************************************************************************************************************************************/

   





























#endif

