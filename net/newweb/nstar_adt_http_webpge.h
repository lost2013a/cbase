#ifndef __WEBPAGE_H
#define __WEBPAGE_H
//need save as utf-8 with BOM


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
