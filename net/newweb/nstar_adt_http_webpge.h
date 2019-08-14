#ifndef __WEBPAGE_H
#define __WEBPAGE_H
//need save as utf-8 with BOM


#define HOME_HTML  "<!DOCTYPE>"\
"<html style='font-size:400%;'>"\
"<head>"\
"<meta http-equiv='Content-Type' content='text/html' charset='utf-8' />"\
"<title>终端网页管理主页</title>"\
"<style type='text/css'>"\
"body {"\
"	background-color: #35a6e8;"\
"	background: -webkit-linear-gradient(right, #f0f0f0 , #35a6e8);"\
"    background: -o-linear-gradient( right, #f0f0f0, #35a6e8);"\
"    background: -moz-linear-gradient( right, #f0f0f0, #35a6e8);"\
"    background: linear-gradient(to right, #f0f0f0 , #35a6e8);"\
"}"\
"h1 {font-size: 20px;padding: 8px 10%;font-weight: bolder;color: #FF0000;background-color: #FFFFFF;}"\
"h2 {text-align: center;font-size: 12px;font-style: oblique;font-weight: bold;color: #0066CC;}"\
"h3 {text-align: center;color: #222222;font-size: 24px;}"\
"h4 {margin:0.2rem auto;; text-align: center;color: #222222;font-size: 10px;}"\
"div.d0{margin:0 auto;	width: 860px;height:500px;padding: 10px;}"\
"div.d1{display: table-cell;vertical-align: middle;font-size: 24px;text-align: center;float: left;background-color: #F0F0F0;filter:alpha(Opacity=00);-moz-opacity:0.9;opacity: 0.8; width: 300px;padding: 10px;margin: 30px 50px;}"\
"div.d2{text-align: left;float: left;width: 570px;margin: 50px 0 ; }"\
"div.d3{float: left;width: 220px;margin: 0 ; }"\
"a.five{font-size: 24px;margin: 50px ;font-weight: bold; }"\
"a.five:link {color: #0077FF;text-decoration: none;}"\
"a.five:visited{color: #0077FF;text-decoration: none;}"\
"a.five:hover {color: #0000CC;}"\
"section{width:2rem;height:2rem;position: relative;margin:1rem auto 0.2rem; padding:  0rem auto ;}"\
".wrap,.circle,.percent{position: absolute; width: 2rem; height: 2rem; border-radius: 50%;}"\
".wrap{top:0; left:0; background-color:#ccc;}"\
".circle{box-sizing: border-box; -webkit-box-sizing:border-box; -ms-box-sizing:border-box; -moz-box-sizing:border-box; -o-box-sizing:border-box; border:1px solid #ccc; clip:rect(0,2rem,2rem,1rem);}"\
".clip-auto{clip:rect(auto, auto, auto, auto);} .percent{box-sizing: border-box; -webkit-box-sizing:border-box; -ms-box-sizing:border-box; -moz-box-sizing:border-box; -o-box-sizing:border-box; top:-1px; left:-1px;}"\
".left{border:0.1rem solid #ff6300; clip: rect(0,1rem,2rem,0);} "\
".right{border:0.1rem solid #ff6300; clip: rect(0,2rem,2rem,1rem);}"\
".wth0{width:0;}"\
".num{position: absolute; box-sizing: border-box; -webkit-box-sizing:border-box; -ms-box-sizing:border-box; "\
"-moz-box-sizing:border-box; -o-box-sizing:border-box; "\
"width:1.8rem; height:1.8rem; line-height:1.8rem; text-align: center; "\
"font-size: 0.4rem; left:0.1rem; top:0.1rem; border-radius: 50%; color:#0077FF; background: white; z-index: 1;}"\
"table, td, th{border:1px solid black;font-size: 12px;color: #222222;}"\
"th{height:45px;width:100px;}"\
"</style>"\
"</head>"\
"<body>"\
"<h1 > STAR-XINHUI </h1>"\
"<h2> chengdu sida web management interface </h2>"\
"<h3> 终端网页管理Ver1.0 </h3>"\
"<div class='d0'>"\
"	<div class='d1'>"\
"        <a class='five' href='parm_1.html' >网络配置</a>"\
"        <h4 >可以设IP地址，网关地址...</h4>"\
"    </div>"\
"    <div class='d1'>"\
"        <a class='five' href='parm_2.html' >参数设置</a>"\
"        <h4 >可以设置用户密码，背光控制...</h4>"\
"    </div>"\
"    <div class='d1'>"\
"        <a class='five' href='parm_3.html' >通道设置</a>"\
"      <h4 >可以设置通道音量，网络端口...</h4>"\
"	</div>"\
"    <div class='d1'>"\
"        <a class='five' href='parm_4.html' >信息日志</a>"\
"        <h4 >可以查看装置日志和版本信息...</h4>"\
"    </div>"\
"    <div class='d3'>"\
"   	<section ><div class='wrap'>"\
"	<div id= 'cleft' class='circle'>"\
"	<div class='percent left' style='-webkit-transform:rotate(0deg);'></div>"\
"	<div id= 'cright' class='percent right wth0'></div>"\
"	</div>"\
"	<div class='num' id='cpuusage'></div>"\
"	</div>"\
"	</section> "\
"	<h4 >CPU使用率</h4>"\
"	</div>"\
"<div class='d2'>  "\
"<table>"\
"<tr>"\
"<th>通道编号</th>"\
"<th>通道1</th><th>通道2</th><th>通道3</th><th>通道4</th>"\
"<th>通道5</th><th>通道6</th><th>通道7</th></tr>"\
"<tr>"\
"<th>接收/缓冲</th>"\
"<th id='txtsq1'></th><th id='txtsq2'></th>"\
"<th id='txtsq3'></th><th id='txtsq4'></th>"\
"<th id='txtsq5'></th><th id='txtsq6'></th>"\
"<th id='txtsq7'></th></tr>"\
"<tr>"\
"<th>当前状态</th>"\
"<th id='txtstep1'></th><th id='txtstep2'></th>"\
"<th id='txtstep3'></th><th id='txtstep4'></th>"\
"<th id='txtstep5'></th><th id='txtstep6'></th>"\
"<th id='txtstep7'></th></tr>"\
"</table>"\
"<h4>模块状态</h4>"\
"</div>"\
"</div>"\
"</body>"\
"</html>"\
"<script>function $(id) { return document.getElementById(id); };"\
"function myrefresh(){"\
"var xmlHttp = new XMLHttpRequest();"\
"xmlHttp.open('GET', '/sta.php?t='+ Math.random(), true);"\
"xmlHttp.send(null);"\
"xmlHttp.onreadystatechange=function(){"\
"if(xmlHttp.readyState == 4 && xmlHttp.status == 200) {"\
"var json=xmlHttp.responseText;"\
"var o=eval(\'(\'+json+\')\');"\
"if ($('txtsq1')) $('txtsq1').innerHTML = o.sq1;"\
"if ($('txtsq2')) $('txtsq2').innerHTML = o.sq2;"\
"if ($('txtsq3')) $('txtsq3').innerHTML = o.sq3;"\
"if ($('txtsq4')) $('txtsq4').innerHTML = o.sq4;"\
"if ($('txtsq5')) $('txtsq5').innerHTML = o.sq5;"\
"if ($('txtsq6')) $('txtsq6').innerHTML = o.sq6;"\
"if ($('txtsq7')) $('txtsq7').innerHTML = o.sq7;"\
"if ($('txtstep1')) $('txtstep1').innerHTML = o.step1;"\
"if ($('txtstep2')) $('txtstep2').innerHTML = o.step2;"\
"if ($('txtstep3')) $('txtstep3').innerHTML = o.step3;"\
"if ($('txtstep4')) $('txtstep4').innerHTML = o.step4;"\
"if ($('txtstep5')) $('txtstep5').innerHTML = o.step5;"\
"if ($('txtstep6')) $('txtstep6').innerHTML = o.step6;"\
"if ($('txtstep7')) $('txtstep7').innerHTML = o.step7;"\
"var cpushow= parseFloat(o.cpuuse);"\
"var x = document.getElementById('cleft');"\
"var y = document.getElementById('cright');"\
"var z = document.getElementsByClassName('percent left');"\
"var a = document.getElementsByClassName('num');"\
"if(cpushow> 50){x.className  = 'clip-auto circle';y.className  = 'percent right';}"\
"else{x.className  = 'circle';y.className  = 'percent right wth0';}	"\
"a[0].innerHTML = cpushow+'%';"\
"z[0].style='-webkit-transform:rotate(' + 3.6*cpushow +'deg);'"\
"}}}</script>	"\
"<script language='JavaScript'>setInterval('myrefresh()',3000);</script>"\
"<script>myrefresh();</script>"\




#define HTML_PARM_HEAD "<!DOCTYPE html>"\
"<html>"\
"<head>"\
"<title>斯达鑫辉终端网页管理参数配置</title>"\
"<meta http-equiv='Content-Type' content='text/html; charset=utf-8'/>"\
"<style type='text/css'>"\
"label{display: inline-block;width: 120px;font-weight: bolder;color: #0066FF;}"\
"body {background-color: #F5F5F5;}"\
"h1 {font-size: 20px;padding: 8px 10%;font-weight: bolder;color: #FF0000;background-color: #FFFFFF;}"\
"h2 {text-align: center;font-size: 12px;font-style: oblique;font-weight: bold;color: #0066CC;}"\
"h3 {text-align: center;color: #333333;font-size: 24px;}"\
"ul {list-style-type: none;margin: 0px 10%; padding: 0;overflow: hidden;border: 1px solid #e7e7e7;"\
"background-color: #f3f3f3;}"\
"li { float: left;}"\
"li a {display: block;color: #666;text-align: center;padding: 10px 20px;text-decoration: none;}"\
"li a:hover:not(.active) {background-color: #CCCCCC;}"\
"li a.home {color: #F00;font-weight: bold;background-color: #FFFFFF;}"\
"li a.active {color: #F5F5F5;font-weight: bold;background-color: #0066CC;}"\
"#rcorners1{color: #FFFFFF;font-weight: bolder;font-size: 12px;border-radius: 15px;"\
"background: #0066CC;width: 110px;height: 30px;border: 0px;margin: 0.3rem 2rem 1rem 0rem;}"\
"</style>"\
"</head>"\




#define PARM_4_HTML "<body>"\
"<ul>"\
"<li><a class='home'	href='home.html'>STAR-XINHUI</a></li>"\
"<li><a href='parm_1.html'>网络配置</a></li>"\
"<li><a href='parm_2.html'>参数设置</a></li>"\
"<li><a href='parm_3.html'>通道设置</a></li>"\
"<li><a class='active'	href='parm_4.html'>信息日志</a></li>"\
"</ul>"\
"<h2> chengdu sida web management interface </h2>"\
"<h3> 信息日志</h3>"\
"<div style='background:snow; display:block; padding:10px 10%;'>"\
"<form id='frmSetting' method='POST' action='pb.cgi'>"\
"<p><label for='txtver1'>硬件版本:</label><input type='text' id='txtver1' size='16'  disabled='disabled' /></p>"\
"<p><label for='txtver2'>软件版本:</label><input type='text' id='txtver2' size='16'  disabled='disabled' /></p>"\
"<p><textarea name='ph1' style='color: #444444;font-size: 11px;' cols='140' rows='30' id='txtph1' ></textarea>"\
"</p>"\
"<p></p><p><input type='button' onclick ='bootmode();' value='重启进入升级模式' id='rcorners1' style='background: #ff6300'/></p>"\
"<p style='color: #FF2222;font-size: 14px;'> 注意：请在厂家技术指导下进行升级操作</p>"\
"</form>"\
"</div>"\
"</body>"\
"</html>"\
"<script>"\
"function $(id) { return document.getElementById(id); };"\
"function json_callback_pb(o) {"\
"if ($('txtver1')) $('txtver1').value = o.ver1;"\
"if ($('txtver2')) $('txtver2').value = o.ver2;"\
"if ($('txtph1')) $('txtph1').value = o.ph1;"\
"};"\
"</script>"\
"<script type='text/javascript' src='pb.js'></script>"\
"<script>"\
"function bootmode(){document.getElementById('frmSetting').action='bootmode.cgi'; document.getElementById('frmSetting').submit(); }"\
"</script>"





#define INDEX_HTML "<!DOCTYPE html>"\
"<html>"\
"<head>"\
"<title>斯达鑫辉终端网页管理</title>"\
"<meta http-equiv='Content-Type' content='text/html' charset='utf-8'/>"\
"<style type='text/css'>"\
"label{display: inline-block;width: 80px;font-weight: bolder;color: #0066FF;}"\
"body {"\
"	background-color: #35a6e8;"\
"	background: -webkit-linear-gradient(right, #f0f0f0 , #35a6e8);"\
"    background: -o-linear-gradient( right, #f0f0f0, #35a6e8);"\
"    background: -moz-linear-gradient( right, #f0f0f0, #35a6e8);"\
"    background: linear-gradient(to right, #f0f0f0 , #35a6e8);"\
"}"\
"h1 {font-size: 20px;padding: 8px 10%;font-weight: bolder;color: #FF0000;background-color: #FFFFFF;}"\
"h2 {text-align: center;font-size: 12px;font-style: oblique;font-weight: bold;color: #0066CC;}"\
"h3 {text-align: left;color: #ffffff;font-size: 24px;font-weight: bolder;}"\
"h4 {color: #78e4ca;font-size: 33px;font-style: oblique;}"\
"h5 {text-align: end;color: #FFFFFF;font-size: 11px;}"\
"h6 {text-align: left;font-size: 12px;font-style: oblique;font-weight: bold;color: #0066CC;}"\
".wrapper {width:720px;margin:auto;}"\
".wrapper1 {"\
"	width: 310px;"\
"	height: 360px;"\
"	padding: 20px;"\
"	background: #5e85c0;"\
"	float: left;"\
"	border: 5px;"\
"	border-style:solid;"\
"	border-color:#ffffff;"\
"	background: -webkit-linear-gradient(left top, #78e4ca , #506de3);"\
"	background: -o-linear-gradient(bottom right, #78e4ca, #506de3);"\
"	background: -moz-linear-gradient(bottom right, #78e4ca, #506de3);"\
"	background: linear-gradient(to bottom right, #78e4ca , #506de3);"\
"}"\
".wrapper2 {"\
"	width: 330px;"\
"	height: 380px;"\
"	padding: 10px;"\
"	background: #f0f0f0;"\
"	float: left;"\
"	border: 5px;"\
"	border-style:solid;"\
"	border-color:#ffffff;"\
"}"\
"#rcorners1 {"\
"	color: #FFFFFF;"\
"	font-weight: bolder;"\
"	font-size: 18px;"\
"	border-radius: 15px;"\
"	background: #4cbdf7;"\
"	background: #0066CC;"\
"	width: 158px;"\
"	height: 30px;"\
"	border: 0px;"\
"}"\
"</style>"\
"</head>"\
"<body>"\
"<h1 > STAR-XINHUI </h1>"\
"<h2> chengdu sida web management interface </h2>"\
"<br><br><br><br><br><br>"\
"<div class='wrapper' >"\
"<div class='wrapper1' > "\
"<h4> Welcome！ </h4>"\
"<h3> 终端网页管理界面Ver1.1 </h3>"\
"<br><br><br><br>"\
"<h5> 型号：适配器<br>&copy;斯达鑫辉视讯科技有限公司 </h5>"\
"</div>"\
"<div class='wrapper2' >"\
"<div style='padding:100px 00px 00px 85px;'>"\
"<h6> 用户密码：</h6>	"\
"</div>"\
"<form id='frmSetting' method='POST' action='log_in.cgi'>"\
"<div style='padding:0px 00px 00px 0px;'>"\
"<p>"\
"<label for='txtpass' ></label> "\
"<input type='text' value='请输入用户密码'  size='16' style='font-size: 16px; color: #c0c0c0'  name='login_pass'"\
"onFocus=\"if(value==defaultValue){value='';this.style.color='#303030'}\""\
"onBlur=\"if(!value){value=defaultValue;this.style.color='#c0c0c0'}\" />"\
"</p>"\
"</div>"\
"<div style='padding:1px 00px 00px 85px;'>"\
"<p> <input  type='submit' value='登录' id='rcorners1'/></p>"\
"</form>"\
"</div>"\
"</div>"\
"</div>"





#endif
